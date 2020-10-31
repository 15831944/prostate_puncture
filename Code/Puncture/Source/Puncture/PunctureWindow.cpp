#include "PunctureWindow.h"
#include <string>
#include <qmessagebox.h>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //����Ĭ�ϱ����ʽ ������������

#define UPDATE_INTERVAL 100  //ͼ��ˢ�¼��(ms)

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

int PunctureWindow::InitWindow()
{
	//�һ���ť
	ui.BtnRegister->setEnabled(false);

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

	//������ʾģ��ߴ�(�߼�������  ����ȡ��B��ͼ��ߴ�) //TODO �Ƿ�Ӧ�ü���B����ͼ������Ķ���(��ƽ��/˫ƽ�棬���ش�С)
	int t_nShowImageX, t_nShowImageY;
	m_USBCapturerPtr->GetImageSize(t_nShowImageX, t_nShowImageY);
	m_AnalyseProcessPtr->Set2DImageSize(t_nShowImageX, t_nShowImageY);

	//��������
	m_NDIOperatorPtr->StartTracking();
	m_USBCapturerPtr->StartGrab();
	m_AnalyseProcessPtr->StartAnalyse();

	//���ź����
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));  //��ʱˢ����ʾͼ��
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(OnBtnRegisterClicked()));  //ҽ���ֶ������ť ��������
	connect(ui.BtnQuit, SIGNAL(clicked()), this, SLOT(OnBtnQuitClicked()));  //����˳����� �ͷŸ����豸

	//������ʱ��
	m_timer.setInterval(UPDATE_INTERVAL);
	m_timer.start();

	//������ť
	ui.BtnRegister->setEnabled(true);
}

void PunctureWindow::OnTimerTimeout()
{
	m_showMutex.lock();
	m_showMutex.unlock();
	//��������Ƿ���
	if (m_FrameDataPtr == nullptr)
		return;
	//2D��ʾ
	if(m_b2DAcquired)
	{
		ui.view2D1->LoadImg(m_FrameDataPtr->m_USBImage);
		//ui.view2D1->LoadProstateMask(m_FrameDataPtr->m_prostateMask);
		ui.view2D1->LoadLesionMask(m_FrameDataPtr->m_lesionMask);
		//ui.view2D1->LoadRectumMask(m_FrameDataPtr->m_rectumMask);
		ui.view2D1->ShowImg();
	}
	//3D��ʾ
	if(m_b3DAcquired)
	{
		//TODO
	}
}

void PunctureWindow::OnBtnRegisterClicked()
{
	m_AnalyseProcessPtr->Register();
	m_bRegistered = true;
}

void PunctureWindow::OnBtnQuitClicked()
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

void PunctureWindow::UpdateFrame(FrameDataPtr t_FrameDataPtr)
{
	m_showMutex.lock();
	m_FrameDataPtr = t_FrameDataPtr;
	m_b2DAcquired = true;
	m_b3DAcquired = true;
	m_showMutex.unlock();
}