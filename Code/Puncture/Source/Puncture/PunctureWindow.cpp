#include "PunctureWindow.h"
#include <string>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //����Ĭ�ϱ����ʽ ������������

#define UPDATE_INTERVAL 20  //ͼ��ˢ�¼��(ms)
#define MRI_MOVE_STEP 0.5  //���̿���MRI�ƶ�  ʵ�ʲ���
#define DEBUG_SAVE_RATE 20  //ÿn֡ͼƬ���洢1֡��Ϊdebug���

PunctureWindow::PunctureWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_b2DAcquired = false;
	m_b3DAcquired = false;
	m_bRegistered = false;
	m_NDIOperatorPtr = nullptr;
	m_USBCapturerPtr = nullptr;
	m_SurgicalPlanPtr = nullptr;
	m_AnalyseProcessPtr = nullptr;
	m_FrameDataPtr = nullptr;
	m_strDataDir = "";
	m_strDebugDir = "";
	InitWindow();
}

void PunctureWindow::InitWindow()
{
	//��ť״̬��ʼ��
	ui.BtnInitDevice->setEnabled(true);
	ui.BtnQuit->setEnabled(false);
	ui.radioBtnSaveDebug->setChecked(false);

	//���ź����
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));  //��ʱˢ����ʾͼ��
	connect(ui.BtnLoadPatientData, SIGNAL(clicked()), this, SLOT(LoadPatientData()));  //���벡������
	connect(ui.BtnInitDevice, SIGNAL(clicked()), this, SLOT(InitDevice()));  //��ʼ���豸
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(OnBtnRegisterClicked()));  //ҽ���ֶ������ť ��������
	connect(ui.BtnResetRegister, SIGNAL(clicked()), this, SLOT(OnBtnResetRegisterClicked()));  //ҽ���ֶ������ť��ȡ������ ������ʾ��mask
	connect(ui.BtnUpdateUS, SIGNAL(clicked()), this, SLOT(OnBtnUpdateUSClicked()));  //��USͼ��ֱ��ʸı�ʱ��ҽ����Ҫ�ֶ������ť ����US����
	connect(ui.BtnQuit, SIGNAL(clicked()), this, SLOT(Quit()));  //����˳����� �ͷŸ����豸
	connect(ui.radioBtnSaveDebug, SIGNAL(toggled(bool)), this, SLOT(OnRadioBtnSaveDebugChanged()));
	connect(ui.view2D1, SIGNAL(KeyLeftPressed()), this, SLOT(MoveMRILeft()));
	connect(ui.view2D1, SIGNAL(KeyRightPressed()), this, SLOT(MoveMRIRight()));
	connect(ui.view2D1, SIGNAL(KeyUpPressed()), this, SLOT(MoveMRIUp()));
	connect(ui.view2D1, SIGNAL(KeyDownPressed()), this, SLOT(MoveMRIDown()));
	connect(ui.view2D2, SIGNAL(KeyLeftPressed()), this, SLOT(MoveMRIForward()));
	connect(ui.view2D2, SIGNAL(KeyRightPressed()), this, SLOT(MoveMRIBackward()));
	connect(ui.view2D2, SIGNAL(KeyUpPressed()), this, SLOT(MoveMRIUp()));
	connect(ui.view2D2, SIGNAL(KeyDownPressed()), this, SLOT(MoveMRIDown()));

	//�����ֶ���׼
	ui.BtnRegister->setEnabled(false);
}

void PunctureWindow::closeEvent(QCloseEvent * event)
{
	this->Quit();
}

int PunctureWindow::InitDevice()
{
	//�һ���ť
	ui.BtnInitDevice->setEnabled(false);

	//��ʼ��
	char exePath[MAX_PATH];
	memset(exePath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, exePath, MAX_PATH);  //�ҵ���ǰexe�ļ���
	std::string strConfigRootPath = std::string(exePath);
	strConfigRootPath = strConfigRootPath.substr(0, strConfigRootPath.rfind('\\')) + "\\Config\\";  //����config�ļ���·��
	std::string strIniFileName;

	//��ʼ��NDI�豸
	strIniFileName = strConfigRootPath + "NDIDevice.ini";
	m_NDIOperatorPtr.reset(new NDIOperator());
	if (m_NDIOperatorPtr->InitNDIDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "����NDI�豸ʧ��");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitNDIDeviceFailed;
	}

	//��ʼ�������豸
	strIniFileName = strConfigRootPath + "USBDevice.ini";
	m_USBCapturerPtr.reset(new USBCapturer());
	if (m_USBCapturerPtr->InitUSBDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "���������豸ʧ��");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitUSBDeviceFailed;
	}
	
	//������������ģ��
	strIniFileName = m_strDataDir + "/AnalyseProcess.ini";
	m_AnalyseProcessPtr.reset(new AnalyseProcess());
	m_AnalyseProcessPtr->BindUpdateFrameEvent(std::bind(&PunctureWindow::UpdateFrame, this, std::placeholders::_1));
	m_AnalyseProcessPtr->SetNDIDevicePtr(m_NDIOperatorPtr);
	m_AnalyseProcessPtr->SetUSBCapturerPtr(m_USBCapturerPtr);
	m_AnalyseProcessPtr->SetSurgicalPlanPtr(m_SurgicalPlanPtr);
	if (m_AnalyseProcessPtr->InitAnalyseProcess(strIniFileName)!=LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "��������ģ��ʧ��");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitAnalyseProcessFailed;
	}
	
	//��������
	m_NDIOperatorPtr->StartTracking();
	m_USBCapturerPtr->StartGrab();
	m_AnalyseProcessPtr->StartAnalyse();

	//������ʱ��
	m_timer.setInterval(UPDATE_INTERVAL);
	m_timer.start();

	//������ť
	ui.BtnRegister->setEnabled(true);
	ui.BtnQuit->setEnabled(true);

	return LIST_NO_ERROR;
}

int PunctureWindow::LoadPatientData()
{
	////��ʼ��
	//char exePath[MAX_PATH];
	//memset(exePath, 0, MAX_PATH);
	//GetModuleFileNameA(NULL, exePath, MAX_PATH);  //�ҵ���ǰexe�ļ���
	//std::string strConfigRootPath = std::string(exePath);
	//strConfigRootPath = strConfigRootPath.substr(0, strConfigRootPath.rfind('\\')) + "\\Config\\";  //����config�ļ���·��
	//std::string strIniFileName;

	//std::string strIniFileName;
	QString qstrDataDir = QFileDialog::getExistingDirectory(this, tr("��������·��"), "dirNotAcquired", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	m_strDataDir = (const char*)qstrDataDir.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (m_strDataDir.empty())
	{
		return LIST_NO_ERROR;
	}
	
	//���������ƻ�ģ��
	//strIniFileName = m_strDataDir + "SurgicalPlan.ini";
	m_SurgicalPlanPtr.reset(new SurgicalPlan());
	if (m_SurgicalPlanPtr->InPortAsFileSet(m_strDataDir) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "���������ƻ�ʧ��");
		ui.BtnInitDevice->setEnabled(true);
		return ER_FileOpenFailed;
	}
	ui.view3D->LoadProstateObj(m_SurgicalPlanPtr->GetObjDataPtr(1));  //����prostate obj
	ui.view3D->LoadLesionObj(m_SurgicalPlanPtr->GetObjDataPtr(2));  //����lesion obj
	ui.view3D->update();  //ˢ����ʾ3D����
	return LIST_NO_ERROR;
}

void PunctureWindow::OnTimerTimeout()
{
	m_showMutex.lock();
	m_showMutex.unlock();
	//��������Ƿ���
	if (m_FrameDataPtr == nullptr)
		return;

	m_showMutex.lock();
	//����MRIģ�������̬����
	ui.ScanCenterX->setText("123");
	ui.ScanCenterX->setText(QString::number(m_FrameDataPtr->m_Attitude.m_ScanCenter.x, 'f', 4));
	ui.ScanCenterY->setText(QString::number(m_FrameDataPtr->m_Attitude.m_ScanCenter.y, 'f', 4));
	ui.ScanCenterZ->setText(QString::number(m_FrameDataPtr->m_Attitude.m_ScanCenter.z, 'f', 4));
	ui.ScanCenterW->setText(QString::number(m_FrameDataPtr->m_Attitude.m_ScanCenter.w, 'f', 4));
	ui.RightDirX->setText(QString::number(m_FrameDataPtr->m_Attitude.m_RightDir.x, 'f', 4));
	ui.RightDirY->setText(QString::number(m_FrameDataPtr->m_Attitude.m_RightDir.y, 'f', 4));
	ui.RightDirZ->setText(QString::number(m_FrameDataPtr->m_Attitude.m_RightDir.z, 'f', 4));
	ui.RightDirW->setText(QString::number(m_FrameDataPtr->m_Attitude.m_RightDir.w, 'f', 4));
	ui.UpDirX->setText(QString::number(m_FrameDataPtr->m_Attitude.m_UpDir.x, 'f', 4));
	ui.UpDirY->setText(QString::number(m_FrameDataPtr->m_Attitude.m_UpDir.y, 'f', 4));
	ui.UpDirZ->setText(QString::number(m_FrameDataPtr->m_Attitude.m_UpDir.z, 'f', 4));
	ui.UpDirW->setText(QString::number(m_FrameDataPtr->m_Attitude.m_UpDir.w, 'f', 4));
	ui.MoveDirX->setText(QString::number(m_FrameDataPtr->m_Attitude.m_MoveDir.x, 'f', 4));
	ui.MoveDirY->setText(QString::number(m_FrameDataPtr->m_Attitude.m_MoveDir.y, 'f', 4));
	ui.MoveDirZ->setText(QString::number(m_FrameDataPtr->m_Attitude.m_MoveDir.z, 'f', 4));
	ui.MoveDirW->setText(QString::number(m_FrameDataPtr->m_Attitude.m_MoveDir.w, 'f', 4));

	//2D��ʾ
	if(m_b2DAcquired)
	{
		if (!m_FrameDataPtr->m_USImgT.empty())  //�����
		{
			ui.view2D1->LoadImg(m_FrameDataPtr->m_USImgT);
			ui.view2D1->LoadProstateMask(m_FrameDataPtr->m_prostateMaskT);
			ui.view2D1->LoadLesionMask(m_FrameDataPtr->m_lesionMaskT);
			ui.view2D1->LoadRectumMask(m_FrameDataPtr->m_rectumMaskT);
			ui.view2D1->ShowImg();
		}
		
		if (!m_FrameDataPtr->m_USImgS.empty())  //ʸ״��
		{
			ui.view2D2->LoadImg(m_FrameDataPtr->m_USImgS);
			ui.view2D2->LoadProstateMask(m_FrameDataPtr->m_prostateMaskS);
			ui.view2D2->LoadLesionMask(m_FrameDataPtr->m_lesionMaskS);
			ui.view2D2->LoadRectumMask(m_FrameDataPtr->m_rectumMaskS);
			ui.view2D2->ShowImg();
		}
	}
	//3D��ʾ
	if(m_b3DAcquired)
	{
		ui.view3D->SetTransversePlane(m_FrameDataPtr->m_TransverseLeftTop, m_FrameDataPtr->m_TransverseRightTop, 
			m_FrameDataPtr->m_TransverseLeftBottom, m_FrameDataPtr->m_TransverseRightBottom);  //���º����ʾ��
		ui.view3D->SetSagittalPlane(m_FrameDataPtr->m_SagittalLeftTop, m_FrameDataPtr->m_SagittalRightTop,
			m_FrameDataPtr->m_SagittalLeftBottom, m_FrameDataPtr->m_SagittalRightBottom);  //����ʸ״��ʾ��
		ui.view3D->update();
	}
	//�洢������
	if (ui.radioBtnSaveDebug->isChecked())
	{
		//TODO
		static int index = 0;
		if (++index % DEBUG_SAVE_RATE == 0)  //ÿDEBUG_SAVE_RATE֡�洢һ֡
		{
			if (!m_FrameDataPtr->m_USImgT.empty())  //�����
			{
				cv::imwrite(m_strDebugDir + "/Tra_Original/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_USImgT);
				cv::imwrite(m_strDebugDir + "/Tra_Prostate/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_prostateMaskT);
				cv::imwrite(m_strDebugDir + "/Tra_Lesion/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_lesionMaskT);
				cv::imwrite(m_strDebugDir + "/Tra_Rectum/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_rectumMaskT);
			}
			if (!m_FrameDataPtr->m_USImgS.empty())  //ʸ״��
			{
				cv::imwrite(m_strDebugDir + "/Sag_Original/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_USImgS);
				cv::imwrite(m_strDebugDir + "/Sag_Prostate/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_prostateMaskS);
				cv::imwrite(m_strDebugDir + "/Sag_Lesion/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_lesionMaskS);
				cv::imwrite(m_strDebugDir + "/Sag_Rectum/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_rectumMaskS);
			}
		}
	}
	m_showMutex.unlock();
}

void PunctureWindow::OnBtnRegisterClicked()
{
	m_AnalyseProcessPtr->Register();
	m_bRegistered = true;
}

void PunctureWindow::OnBtnResetRegisterClicked()
{
	m_AnalyseProcessPtr->ResetRegister();
	m_bRegistered = false;
}

//��B��ͼ��ֱ��ʸı�ʱ��ҽ����Ҫ�ֶ������ť�����³�������
void PunctureWindow::OnBtnUpdateUSClicked()
{
	m_USBCapturerPtr->UpdateParameters();
}

void PunctureWindow::Quit()
{
	//ֹͣ����ģ��
	if (m_AnalyseProcessPtr != nullptr)
		m_AnalyseProcessPtr->StopAnalyse();
	if (m_NDIOperatorPtr != nullptr)
		m_NDIOperatorPtr->StopTracking();
	if (m_USBCapturerPtr != nullptr)
		m_USBCapturerPtr->StopGrab();
	//�رմ���
	this->close();
}

void PunctureWindow::MoveMRILeft()
{
	m_AnalyseProcessPtr->MoveMRIRight(-MRI_MOVE_STEP);
}

void PunctureWindow::MoveMRIRight()
{
	m_AnalyseProcessPtr->MoveMRIRight(MRI_MOVE_STEP);
}

void PunctureWindow::MoveMRIUp()
{
	m_AnalyseProcessPtr->MoveMRIUp(MRI_MOVE_STEP);
}

void PunctureWindow::MoveMRIDown()
{
	m_AnalyseProcessPtr->MoveMRIUp(-MRI_MOVE_STEP);
}

void PunctureWindow::MoveMRIForward()
{
	m_AnalyseProcessPtr->MoveMRIForward(MRI_MOVE_STEP);
}

void PunctureWindow::MoveMRIBackward()
{
	m_AnalyseProcessPtr->MoveMRIForward(-MRI_MOVE_STEP);
}

void PunctureWindow::OnRadioBtnSaveDebugChanged()
{
	if (!ui.radioBtnSaveDebug->isChecked())
		return;
	QString qstrDebugDir = QFileDialog::getExistingDirectory(this, tr("dubug�ļ��洢·��"), "dirNotAcquired", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	m_strDebugDir = (const char*)qstrDebugDir.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������

	//�������ڴ洢�������ݵ����ļ���
	QDir dir(qstrDebugDir);
	dir.mkdir(QString::fromLocal8Bit("Tra_Original"));
	dir.mkdir(QString::fromLocal8Bit("Tra_Prostate"));
	dir.mkdir(QString::fromLocal8Bit("Tra_Lesion"));
	dir.mkdir(QString::fromLocal8Bit("Tra_Rectum"));
	dir.mkdir(QString::fromLocal8Bit("Sag_Original"));
	dir.mkdir(QString::fromLocal8Bit("Sag_Prostate"));
	dir.mkdir(QString::fromLocal8Bit("Sag_Lesion"));
	dir.mkdir(QString::fromLocal8Bit("Sag_Rectum"));
}

void PunctureWindow::UpdateFrame(FrameDataPtr t_FrameDataPtr)
{
	m_showMutex.lock();
	m_FrameDataPtr = t_FrameDataPtr;
	m_b2DAcquired = true;
	m_b3DAcquired = true;
	m_showMutex.unlock();
}