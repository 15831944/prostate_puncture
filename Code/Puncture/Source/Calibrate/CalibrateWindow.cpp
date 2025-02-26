#include "CalibrateWindow.h"
#include <string>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <fstream>
#include "ErrorManager/ErrorCodeDefine.h"
#pragma execution_character_set("utf-8")  //避免中文乱码 参考https://blog.csdn.net/liyuanbhu/article/details/72596952
#define GAMA_TRANSFORM  //是否对采集到的图像进行gama校正

#define DISPLAY_INTERVAL 20 //图像刷新间隔(ms)
#define CAPTURE_INTERVAL 333  //采集数据间隔(ms)
#define CAPTURE_BATCHSIZE 360  //采集一批数据的总数

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
	createGamaLut(m_lut, 1.0, 0.5);  //初始化gama校正参数

	//界面状态初始化
	ui.BtnStartCapture->setEnabled(false);
	ui.RadioBtnTransverse->setChecked(false);
	ui.RadioBtnSagittal->setChecked(false);
	//信号与槽
	connect(ui.BtnInitDevice, SIGNAL(clicked()), this, SLOT(OnBtnInitDeviceClicked()));
	connect(ui.BtnStartCapture, SIGNAL(clicked()), this, SLOT(OnBtnStartCaptureClicked()));
	connect(ui.BtnAnnotate, SIGNAL(clicked()), this, SLOT(OnBtnAnnotateClicked()));
	connect(ui.BtnLocatePhantom, SIGNAL(clicked()), this, SLOT(OnBtnLocatePhantomClicked()));
	connect(ui.BtnReleaseDevice, SIGNAL(clicked()), this, SLOT(ReleaseDevice()));
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
	//显示单平面类型，存储数据
	if (!imgTransverse.empty())  //优先采集横断面
	{
		m_bUSAcquired = true;
		m_imgUS = imgTransverse;
		m_fUSRes = fRes1;
		ui.RadioBtnTransverse->setChecked(true);
	}
	else if(!imgSagittal.empty())  //其次采集矢状面
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

void CalibrateWindow::createGamaLut(uchar lutGama[256], float fC, float fGama)
{
	memset(lutGama, 0, sizeof(uchar) * 2);  //灰度值过低的点 直接置为0
	for (int i = 2; i < 256; i++)
	{
		lutGama[i] = fC * uchar(pow(float(i) / 255, fGama) * 255) < 255 ? fC * uchar(pow(float(i) / 255, fGama) * 255) : 255;
	}
	return;
}

//使用查找表 对cv::Mat赋值
//Mat遍历方式 参考http://www.opencv.org.cn/opencvdoc/2.3.2/html/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html
//传参方式 参考https://blog.csdn.net/u012814856/article/details/84099328
cv::Mat CalibrateWindow::transformImg(cv::Mat & srcImg)
{
	//确保输入的图片深度与uchar相同
	CV_Assert(srcImg.depth() != sizeof(uchar));

	int channels = srcImg.channels();
	int nRows = srcImg.rows;
	int nCols = srcImg.cols*channels;
	int x, y;
	uchar* pCur;

	if (srcImg.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	for (y = 0; y < nRows; y++)
	{
		pCur = srcImg.ptr<uchar>(y);  //第y行首元素
		for (x = 0; x < nCols; x++)
		{
			pCur[x] = m_lut[pCur[x]];
		}
	}
	return srcImg;
}

int CalibrateWindow::OnBtnInitDeviceClicked()
{
	ui.BtnStartCapture->setEnabled(false);
	ui.BtnReleaseDevice->setEnabled(false);

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
	m_NDIOperatorPtr->BindUpdateAttitudeEvent(std::bind(&CalibrateWindow::UpdateNDIData, this, std::placeholders::_1));  //绑定回调函数
	m_NDIOperatorPtr->StartTracking();  //开始采集

	//初始化超声设备
	strIniFileName = strConfigRootPath + "USBDevice.ini";
	m_USBCapturerPtr.reset(new USBCapturer());
	if (m_USBCapturerPtr->InitUSBDevice(strIniFileName) != LIST_NO_ERROR)
	{
		QMessageBox::information(this, "错误", "启动超声设备失败");
		ui.BtnInitDevice->setEnabled(true);
		return ER_InitUSBDeviceFailed;
	}
	m_USBCapturerPtr->BindCapturePerFrameEvent(std::bind(&CalibrateWindow::UpdateUSImage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));  //绑定回调函数
	m_USBCapturerPtr->StartGrab();  //开始采集

	ui.BtnStartCapture->setEnabled(true);
	ui.BtnReleaseDevice->setEnabled(true);
	//开始显示
	m_timerDisplay.start();
	return LIST_NO_ERROR;
}

void CalibrateWindow::OnBtnStartCaptureClicked()
{
	m_nDataIndex = 0;
	//数据存储路径
	QString qstrRoot = QFileDialog::getExistingDirectory(this, "数据存储目录", "D:/", QFileDialog::ShowDirsOnly);
	m_strDataDir = qstrRoot.toLocal8Bit();
	if (m_strDataDir.empty())
		return;
	ui.BtnStartCapture->setEnabled(false);
	m_timerCapture.start();
}

void CalibrateWindow::OnBtnAnnotateClicked()
{
	m_AnnotateWindowPtr = new AnnotateWindow();
	m_AnnotateWindowPtr->setWindowModality(Qt::NonModal);  //设置为非模态窗口
	m_AnnotateWindowPtr->show();
}

void CalibrateWindow::OnBtnLocatePhantomClicked()
{
	if (m_NDIOperatorPtr==nullptr||!m_NDIOperatorPtr->IsTracking())
	{
		QMessageBox::information(this, "错误", "NDI设备未在运行");
		return;
	}
	m_LocatePhantomDlgPtr = new LocatePhantomDlg();
	m_LocatePhantomDlgPtr->setWindowModality(Qt::NonModal);  //设置为非模态窗口
	m_LocatePhantomDlgPtr->setAttribute(Qt::WA_DeleteOnClose);   //在关闭窗口时即销毁(调用析构函数) 参考 https://blog.csdn.net/lovebird_27/article/details/49255715
	m_LocatePhantomDlgPtr->SetNDIOperator(m_NDIOperatorPtr);  //将NDI设备指针赋给LocatePhantomDlg，只是为了方便修改使用的sensor序号。NDI数据仍然在CalibrateWindow中采集
	m_LocatePhantomDlgPtr->show();
}


void CalibrateWindow::ReleaseDevice()
{
	m_timerCapture.stop();
	m_timerDisplay.stop();
	//停止各个模块
	if (m_NDIOperatorPtr != nullptr)
		m_NDIOperatorPtr->StopTracking();
	if (m_USBCapturerPtr != nullptr)
		m_USBCapturerPtr->StopGrab();
}

void CalibrateWindow::OnTimerDisplay()
{
	m_showMutex.lock();
	//显示图像
	if (m_bUSAcquired)
	{
		ui.view2D->LoadImg(m_imgUS);
		ui.view2D->ShowImg();
	}
	//if (m_LocatePhantomDlgPtr!=nullptr)
	//{
	//	m_LocatePhantomDlgPtr->UpdateNDIData(m_euler);  //TODO 当LocatePhantomDlg关闭后，这个指针可能非空，且已被delete。因此 此处可能引发问题
	//}
	m_showMutex.unlock();
}

void CalibrateWindow::OnTimerCapture()
{
	float fX, fY, fZ, fAlpha, fBeta, fGama;
	string strAbsName;
	
	m_showMutex.lock();
	//存储图片数据
#ifdef GAMA_TRANSFORM
	//gama校正  TODO
	cv::Mat imgGray;  //灰度图 中间变量
	cv::cvtColor(m_imgUS, imgGray, cv::COLOR_BGR2GRAY);
	cv::threshold(imgGray, imgGray, 1, 255, cv::THRESH_BINARY_INV);  //筛选出灰度<=1的部分,设置为255
	m_imgUS.setTo(0, imgGray);  //将灰度<=1的部分 置为0
	this->transformImg(m_imgUS);  //使用查找表变换图片 使图像更加易读
#endif
	strAbsName = m_strDataDir + '/' + "baseUS_" + std::to_string(m_nDataIndex) + ".bmp";
	cv::imwrite(strAbsName, m_imgUS);
	//存储NDI姿态参数
	m_euler.GetPosition(fX, fY, fZ, fAlpha, fBeta, fGama);  //获取欧拉角参数信息
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
	//更新下标
	m_nDataIndex++;
	if (m_nDataIndex >= CAPTURE_BATCHSIZE)
	{
		m_timerCapture.stop();
		ui.BtnStartCapture->setEnabled(true);
		QMessageBox::information(this, "通知", "采集完成");
	}
	m_showMutex.unlock();
}
