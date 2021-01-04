#include "CalibrateWindow.h"
#include <string>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <fstream>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //������������ �ο�https://blog.csdn.net/liyuanbhu/article/details/72596952

#define DISPLAY_INTERVAL 20 //ͼ��ˢ�¼��(ms)
#define CAPTURE_INTERVAL 333  //�ɼ����ݼ��(ms)
#define CAPTURE_BATCHSIZE 360  //�ɼ�һ�����ݵ�����

CalibrateWindow::CalibrateWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	m_AnnotateWindowPtr = nullptr;
	m_LocatePhantomDlgPtr = nullptr;
	m_USBCapturerPtr = nullptr;
	m_NDIOperatorPtr = nullptr;
	m_bUSAcquired = false;
	m_nDataIndex = 0;

	//����״̬��ʼ��
	ui.BtnStartCapture->setEnabled(false);
	ui.RadioBtnTransverse->setChecked(false);
	ui.RadioBtnSagittal->setChecked(false);
	//�ź����
	connect(ui.BtnInitDevice, SIGNAL(clicked()), this, SLOT(OnBtnInitDeviceClicked()));
	connect(ui.BtnStartCapture, SIGNAL(clicked()), this, SLOT(OnBtnStartCaptureClicked()));
	connect(ui.BtnAnnotate, SIGNAL(clicked()), this, SLOT(OnBtnAnnotateClicked()));
	connect(ui.BtnLocatePhantom, SIGNAL(clicked()), this, SLOT(OnBtnLocatePhantomClicked()));
	connect(&m_timerDisplay, SIGNAL(timeout()), this, SLOT(OnTimerDisplay()));
	connect(&m_timerCapture, SIGNAL(timeout()), this, SLOT(OnTimerCapture()));
	m_timerDisplay.setInterval(DISPLAY_INTERVAL);
	m_timerCapture.setInterval(CAPTURE_INTERVAL);
}

CalibrateWindow::~CalibrateWindow()
{
	if(m_AnnotateWindowPtr != nullptr)
		delete m_AnnotateWindowPtr;
}

void CalibrateWindow::closeEvent()
{
	this->ReleaseDevice();
	this->close();
}

void CalibrateWindow::UpdateUSImage(cv::Mat imgTransverse, cv::Mat imgSagittal, double fRes1, double fRes2)
{
	m_showMutex.lock();
	//��ʾ��ƽ�����ͣ��洢����
	if (!imgTransverse.empty())  //���Ȳɼ������
	{
		m_bUSAcquired = true;
		m_imgUS = imgTransverse;
		m_fUSRes = fRes1;
		ui.RadioBtnTransverse->setChecked(true);
	}
	else if(!imgSagittal.empty())  //��βɼ�ʸ״��
	{
		m_bUSAcquired = true;
		m_imgUS = imgSagittal;
		m_fUSRes = fRes2;
		ui.RadioBtnSagittal->setChecked(true);
	}
	m_showMutex.unlock();
}

void CalibrateWindow::UpdateNDIData(NDIOPERATOR::Attitude euler)
{
	m_showMutex.lock();
	m_euler = euler;
	m_showMutex.unlock();
}

int CalibrateWindow::OnBtnInitDeviceClicked()
{
	ui.BtnStartCapture->setEnabled(false);
	ui.BtnReleaseDevice->setEnabled(false);

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
	m_NDIOperatorPtr->BindUpdateAttitudeEvent(std::bind(&CalibrateWindow::UpdateNDIData, this, std::placeholders::_1));  //�󶨻ص�����
	m_NDIOperatorPtr->StartTracking();  //��ʼ�ɼ�

	//��ʼ�������豸
	strIniFileName = strConfigRootPath + "USBDevice.ini";
	m_USBCapturerPtr.reset(new USBCapturer());
	if (m_USBCapturerPtr->InitUSBDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "����", "���������豸ʧ��");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitUSBDeviceFailed;
	}
	m_USBCapturerPtr->BindCapturePerFrameEvent(std::bind(&CalibrateWindow::UpdateUSImage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));  //�󶨻ص�����
	m_USBCapturerPtr->StartGrab();  //��ʼ�ɼ�

	ui.BtnStartCapture->setEnabled(true);
	ui.BtnReleaseDevice->setEnabled(true);
	//��ʼ��ʾ
	m_timerDisplay.start();
	return LIST_NO_ERROR;
}

void CalibrateWindow::OnBtnStartCaptureClicked()
{
	m_nDataIndex = 0;
	//���ݴ洢·��
	QString qstrRoot = QFileDialog::getExistingDirectory(this, "���ݴ洢Ŀ¼", "D:/", QFileDialog::ShowDirsOnly);
	m_strDataDir = qstrRoot.toLocal8Bit();
	if (m_strDataDir.empty())
		return;
	ui.BtnStartCapture->setEnabled(false);
	m_timerCapture.start();
}

void CalibrateWindow::OnBtnAnnotateClicked()
{
	m_AnnotateWindowPtr = new AnnotateWindow();
	m_AnnotateWindowPtr->setWindowModality(Qt::NonModal);  //����Ϊ��ģ̬����
	m_AnnotateWindowPtr->show();
}

void CalibrateWindow::OnBtnLocatePhantomClicked()
{
	if (m_NDIOperatorPtr==nullptr||!m_NDIOperatorPtr->IsTracking())
	{
		QMessageBox::information(this, "����", "NDI�豸δ������");
		return;
	}
	m_LocatePhantomDlgPtr = new LocatePhantomDlg();
	m_LocatePhantomDlgPtr->setWindowModality(Qt::NonModal);  //����Ϊ��ģ̬����
	m_LocatePhantomDlgPtr->SetNDIOperator(m_NDIOperatorPtr);  //��NDI�豸ָ�븳��LocatePhantomDlg��ֻ��Ϊ�˷����޸�ʹ�õ�sensor��š�NDI������Ȼ��CalibrateWindow�вɼ�
	m_LocatePhantomDlgPtr->show();
}

void CalibrateWindow::ReleaseDevice()
{
	m_timerCapture.stop();
	m_timerDisplay.stop();
	//ֹͣ����ģ��
	if (m_NDIOperatorPtr != nullptr)
		m_NDIOperatorPtr->StopTracking();
	if (m_USBCapturerPtr != nullptr)
		m_USBCapturerPtr->StopGrab();
}

void CalibrateWindow::OnTimerDisplay()
{
	m_showMutex.lock();
	//��ʾͼ��
	if (m_bUSAcquired)
	{
		ui.view2D->LoadImg(m_imgUS);
		ui.view2D->ShowImg();
	}
	if (m_LocatePhantomDlgPtr!=nullptr)
	{
		m_LocatePhantomDlgPtr->UpdateNDIData(m_euler);  //TODO ��LocatePhantomDlg�رպ����ָ����ܷǿգ����ѱ�delete����� �˴�������������
	}
	m_showMutex.unlock();
}

void CalibrateWindow::OnTimerCapture()
{
	float fX, fY, fZ, fAlpha, fBeta, fGama;
	string strAbsName;
	
	//�洢ͼƬ����
	strAbsName = m_strDataDir + '/' + "baseUS_" + std::to_string(m_nDataIndex) + ".bmp";
	cv::imwrite(strAbsName, m_imgUS);
	//�洢NDI��̬����
	m_euler.GetPosition(fX, fY, fZ, fAlpha, fBeta, fGama);  //��ȡŷ���ǲ�����Ϣ
	strAbsName = m_strDataDir + '/' + "BaseParameters_" + std::to_string(m_nDataIndex) + ".txt";
	ofstream outfile(strAbsName);
	outfile << "fX\t" << fX << '\n'
		<< "fY\t" << fY << '\n'
		<< "fZ\t" << fZ << '\n'
		<< "fAlpha\t" << fAlpha << '\n'
		<< "fBeta\t" << fBeta << '\n'
		<< "fGama\t" << fGama << '\n'
		<< "USImageRes\t" << m_fUSRes << '\n';
	outfile.close();
	//�����±�
	m_nDataIndex++;
	if (m_nDataIndex > CAPTURE_BATCHSIZE)
	{
		m_timerCapture.stop();
		ui.BtnStartCapture->setEnabled(true);
		QMessageBox::information(this, "֪ͨ", "�ɼ����");
	}
}
