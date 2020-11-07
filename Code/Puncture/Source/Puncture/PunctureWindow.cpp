#include "PunctureWindow.h"
#include <string>
#include <qmessagebox.h>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //����Ĭ�ϱ����ʽ ������������

#define UPDATE_INTERVAL 20  //ͼ��ˢ�¼��(ms)
#define MRI_MOVE_STEP 1  //���̿���MRI�ƶ�  ʵ�ʲ���

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
	InitWindow();
}

void PunctureWindow::InitWindow()
{
	//��ť״̬��ʼ��
	ui.BtnInitDevice->setEnabled(true);
	ui.BtnQuit->setEnabled(false);

	//���ź����
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));  //��ʱˢ����ʾͼ��
	connect(ui.BtnInitDevice, SIGNAL(clicked()), this, SLOT(InitDevice()));  //��ʼ���豸
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(OnBtnRegisterClicked()));  //ҽ���ֶ������ť ��������
	connect(ui.BtnQuit, SIGNAL(clicked()), this, SLOT(Quit()));  //����˳����� �ͷŸ����豸
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
		return ER_InitNDIDeviceFailed;
	}

	//��ʼ�������豸
	strIniFileName = strConfigRootPath + "USBDevice.ini";
	m_USBCapturerPtr.reset(new USBCapturer());
	if (m_USBCapturerPtr->InitUSBDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "���������豸ʧ��");
		return ER_InitUSBDeviceFailed;
	}

	//���������ƻ�ģ��
	strIniFileName = strConfigRootPath + "SurgicalPlan.ini";
	m_SurgicalPlanPtr.reset(new SurgicalPlan());
	if (m_SurgicalPlanPtr->InPortAsFileSet(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "���������ƻ�ʧ��");
		return ER_FileOpenFailed;
	}

	//������������ģ��
	strIniFileName = strConfigRootPath + "AnalyseProcess.ini";
	m_AnalyseProcessPtr.reset(new AnalyseProcess());
	m_AnalyseProcessPtr->BindUpdateFrameEvent(std::bind(&PunctureWindow::UpdateFrame, this, std::placeholders::_1));
	m_AnalyseProcessPtr->SetNDIDevicePtr(m_NDIOperatorPtr);
	m_AnalyseProcessPtr->SetUSBCapturerPtr(m_USBCapturerPtr);
	m_AnalyseProcessPtr->SetSurgicalPlanPtr(m_SurgicalPlanPtr);
	if (m_AnalyseProcessPtr->InitAnalyseProcess(strIniFileName)!=LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "��������ģ��ʧ��");
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

void PunctureWindow::OnTimerTimeout()
{
	m_showMutex.lock();
	m_showMutex.unlock();
	//��������Ƿ���
	if (m_FrameDataPtr == nullptr)
		return;

	m_showMutex.lock();
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
		//TODO
	}
	m_showMutex.unlock();
}

void PunctureWindow::OnBtnRegisterClicked()
{
	m_AnalyseProcessPtr->Register();
	m_bRegistered = true;
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

void PunctureWindow::UpdateFrame(FrameDataPtr t_FrameDataPtr)
{
	m_showMutex.lock();
	m_FrameDataPtr = t_FrameDataPtr;
	m_b2DAcquired = true;
	m_b3DAcquired = true;
	m_showMutex.unlock();
}