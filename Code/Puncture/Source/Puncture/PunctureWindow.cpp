#include "PunctureWindow.h"
#include <string>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //设置默认编码格式 避免中文乱码

#define UPDATE_INTERVAL 20  //图像刷新间隔(ms)
#define MRI_MOVE_STEP 0.5  //键盘控制MRI移动  实际步长
#define DEBUG_SAVE_RATE 20  //每n帧图片，存储1帧作为debug输出

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
	//按钮状态初始化
	ui.BtnInitDevice->setEnabled(true);
	ui.BtnQuit->setEnabled(false);
	ui.radioBtnSaveDebug->setChecked(false);

	//绑定信号与槽
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimerTimeout()));  //定时刷新显示图像
	connect(ui.BtnLoadPatientData, SIGNAL(clicked()), this, SLOT(LoadPatientData()));  //载入病人数据
	connect(ui.BtnInitDevice, SIGNAL(clicked()), this, SLOT(InitDevice()));  //初始化设备
	connect(ui.BtnRegister, SIGNAL(clicked()), this, SLOT(OnBtnRegisterClicked()));  //医生手动点击按钮 锁定坐标
	connect(ui.BtnResetRegister, SIGNAL(clicked()), this, SLOT(OnBtnResetRegisterClicked()));  //医生手动点击按钮，取消锁定 并重置示意mask
	connect(ui.BtnUpdateUS, SIGNAL(clicked()), this, SLOT(OnBtnUpdateUSClicked()));  //在US图像分辨率改变时，医生需要手动点击按钮 更新US参数
	connect(ui.BtnQuit, SIGNAL(clicked()), this, SLOT(Quit()));  //点击退出程序 释放各个设备
	connect(ui.radioBtnSaveDebug, SIGNAL(toggled(bool)), this, SLOT(OnRadioBtnSaveDebugChanged()));
	connect(ui.view2D1, SIGNAL(KeyLeftPressed()), this, SLOT(MoveMRILeft()));
	connect(ui.view2D1, SIGNAL(KeyRightPressed()), this, SLOT(MoveMRIRight()));
	connect(ui.view2D1, SIGNAL(KeyUpPressed()), this, SLOT(MoveMRIUp()));
	connect(ui.view2D1, SIGNAL(KeyDownPressed()), this, SLOT(MoveMRIDown()));
	connect(ui.view2D2, SIGNAL(KeyLeftPressed()), this, SLOT(MoveMRIForward()));
	connect(ui.view2D2, SIGNAL(KeyRightPressed()), this, SLOT(MoveMRIBackward()));
	connect(ui.view2D2, SIGNAL(KeyUpPressed()), this, SLOT(MoveMRIUp()));
	connect(ui.view2D2, SIGNAL(KeyDownPressed()), this, SLOT(MoveMRIDown()));

	//允许手动配准
	ui.BtnRegister->setEnabled(false);
}

void PunctureWindow::closeEvent(QCloseEvent * event)
{
	this->Quit();
}

int PunctureWindow::InitDevice()
{
	//灰化按钮
	ui.BtnInitDevice->setEnabled(false);

	//初始化
	char exePath[MAX_PATH];
	memset(exePath, 0, MAX_PATH);
	GetModuleFileNameA(NULL, exePath, MAX_PATH);  //找到当前exe文件名
	std::string strConfigRootPath = std::string(exePath);
	strConfigRootPath = strConfigRootPath.substr(0, strConfigRootPath.rfind('\\')) + "\\Config\\";  //生成config文件夹路径
	std::string strIniFileName;

	//初始化NDI设备
	strIniFileName = strConfigRootPath + "NDIDevice.ini";
	m_NDIOperatorPtr.reset(new NDIOperator());
	if (m_NDIOperatorPtr->InitNDIDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "错误", "启动NDI设备失败");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitNDIDeviceFailed;
	}

	//初始化超声设备
	strIniFileName = strConfigRootPath + "USBDevice.ini";
	m_USBCapturerPtr.reset(new USBCapturer());
	if (m_USBCapturerPtr->InitUSBDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "错误", "启动超声设备失败");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitUSBDeviceFailed;
	}
	
	//创建分析处理模块
	strIniFileName = m_strDataDir + "/AnalyseProcess.ini";
	m_AnalyseProcessPtr.reset(new AnalyseProcess());
	m_AnalyseProcessPtr->BindUpdateFrameEvent(std::bind(&PunctureWindow::UpdateFrame, this, std::placeholders::_1));
	m_AnalyseProcessPtr->SetNDIDevicePtr(m_NDIOperatorPtr);
	m_AnalyseProcessPtr->SetUSBCapturerPtr(m_USBCapturerPtr);
	m_AnalyseProcessPtr->SetSurgicalPlanPtr(m_SurgicalPlanPtr);
	if (m_AnalyseProcessPtr->InitAnalyseProcess(strIniFileName)!=LIST_NO_ERROR)
	{
		QMessageBox::information(this, "错误", "启动分析模块失败");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitAnalyseProcessFailed;
	}
	
	//启动分析
	m_NDIOperatorPtr->StartTracking();
	m_USBCapturerPtr->StartGrab();
	m_AnalyseProcessPtr->StartAnalyse();

	//开启定时器
	m_timer.setInterval(UPDATE_INTERVAL);
	m_timer.start();

	//开启按钮
	ui.BtnRegister->setEnabled(true);
	ui.BtnQuit->setEnabled(true);

	return LIST_NO_ERROR;
}

int PunctureWindow::LoadPatientData()
{
	////初始化
	//char exePath[MAX_PATH];
	//memset(exePath, 0, MAX_PATH);
	//GetModuleFileNameA(NULL, exePath, MAX_PATH);  //找到当前exe文件名
	//std::string strConfigRootPath = std::string(exePath);
	//strConfigRootPath = strConfigRootPath.substr(0, strConfigRootPath.rfind('\\')) + "\\Config\\";  //生成config文件夹路径
	//std::string strIniFileName;

	//std::string strIniFileName;
	QString qstrDataDir = QFileDialog::getExistingDirectory(this, tr("病人数据路径"), "dirNotAcquired", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	m_strDataDir = (const char*)qstrDataDir.toLocal8Bit();  //转换为std::string 且避免中文字符出现乱码
	if (m_strDataDir.empty())
	{
		return LIST_NO_ERROR;
	}
	
	//创建手术计划模块
	//strIniFileName = m_strDataDir + "SurgicalPlan.ini";
	m_SurgicalPlanPtr.reset(new SurgicalPlan());
	if (m_SurgicalPlanPtr->InPortAsFileSet(m_strDataDir) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "错误", "导入手术计划失败");
		ui.BtnInitDevice->setEnabled(true);
		return ER_FileOpenFailed;
	}
	ui.view3D->LoadProstateObj(m_SurgicalPlanPtr->GetObjDataPtr(1));  //载入prostate obj
	ui.view3D->LoadLesionObj(m_SurgicalPlanPtr->GetObjDataPtr(2));  //载入lesion obj
	ui.view3D->update();  //刷新显示3D窗口
	return LIST_NO_ERROR;
}

void PunctureWindow::OnTimerTimeout()
{
	m_showMutex.lock();
	m_showMutex.unlock();
	//检测数据是否导入
	if (m_FrameDataPtr == nullptr)
		return;

	m_showMutex.lock();
	//更新MRI模拟采样姿态参数
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

	//2D显示
	if(m_b2DAcquired)
	{
		if (!m_FrameDataPtr->m_USImgT.empty())  //横断面
		{
			ui.view2D1->LoadImg(m_FrameDataPtr->m_USImgT);
			ui.view2D1->LoadProstateMask(m_FrameDataPtr->m_prostateMaskT);
			ui.view2D1->LoadLesionMask(m_FrameDataPtr->m_lesionMaskT);
			ui.view2D1->LoadRectumMask(m_FrameDataPtr->m_rectumMaskT);
			ui.view2D1->ShowImg();
		}
		
		if (!m_FrameDataPtr->m_USImgS.empty())  //矢状面
		{
			ui.view2D2->LoadImg(m_FrameDataPtr->m_USImgS);
			ui.view2D2->LoadProstateMask(m_FrameDataPtr->m_prostateMaskS);
			ui.view2D2->LoadLesionMask(m_FrameDataPtr->m_lesionMaskS);
			ui.view2D2->LoadRectumMask(m_FrameDataPtr->m_rectumMaskS);
			ui.view2D2->ShowImg();
		}
	}
	//3D显示
	if(m_b3DAcquired)
	{
		ui.view3D->SetTransversePlane(m_FrameDataPtr->m_TransverseLeftTop, m_FrameDataPtr->m_TransverseRightTop, 
			m_FrameDataPtr->m_TransverseLeftBottom, m_FrameDataPtr->m_TransverseRightBottom);  //更新横断面示意
		ui.view3D->SetSagittalPlane(m_FrameDataPtr->m_SagittalLeftTop, m_FrameDataPtr->m_SagittalRightTop,
			m_FrameDataPtr->m_SagittalLeftBottom, m_FrameDataPtr->m_SagittalRightBottom);  //更新矢状面示意
		ui.view3D->update();
	}
	//存储至本地
	if (ui.radioBtnSaveDebug->isChecked())
	{
		//TODO
		static int index = 0;
		if (++index % DEBUG_SAVE_RATE == 0)  //每DEBUG_SAVE_RATE帧存储一帧
		{
			if (!m_FrameDataPtr->m_USImgT.empty())  //横断面
			{
				cv::imwrite(m_strDebugDir + "/Tra_Original/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_USImgT);
				cv::imwrite(m_strDebugDir + "/Tra_Prostate/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_prostateMaskT);
				cv::imwrite(m_strDebugDir + "/Tra_Lesion/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_lesionMaskT);
				cv::imwrite(m_strDebugDir + "/Tra_Rectum/" + to_string(index/DEBUG_SAVE_RATE) + ".bmp", m_FrameDataPtr->m_rectumMaskT);
			}
			if (!m_FrameDataPtr->m_USImgS.empty())  //矢状面
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

//在B超图像分辨率改变时，医生需要手动点击按钮，更新超声参数
void PunctureWindow::OnBtnUpdateUSClicked()
{
	m_USBCapturerPtr->UpdateParameters();
}

void PunctureWindow::Quit()
{
	//停止各个模块
	if (m_AnalyseProcessPtr != nullptr)
		m_AnalyseProcessPtr->StopAnalyse();
	if (m_NDIOperatorPtr != nullptr)
		m_NDIOperatorPtr->StopTracking();
	if (m_USBCapturerPtr != nullptr)
		m_USBCapturerPtr->StopGrab();
	//关闭窗口
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
	QString qstrDebugDir = QFileDialog::getExistingDirectory(this, tr("dubug文件存储路径"), "dirNotAcquired", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	m_strDebugDir = (const char*)qstrDebugDir.toLocal8Bit();  //转换为std::string 且避免中文字符出现乱码

	//创建用于存储各类数据的子文件夹
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