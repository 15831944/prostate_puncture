//*****************************************************************
// �ļ��� :						AnalyseProcess.cpp
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				ʵ�ʷ�������ģ��
// �������� :					2020.11.01
// �޸����� :					2020.05.07
// ���� :						Fiki
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "pch.h"
#include "AnalyseProcess.h"
#include "ErrorManager/ErrorCodeDefine.h"

using namespace ANALYSEPROCESS;

///////////////////////////////////////////////////////
////////////////////////define/////////////////////////
///////////////////////////////////////////////////////
const int VERTICAL_LINE_NUM = 5;  //ȡ��������ֱ����ǰ�����������ཻ
const int HORIZONTAL_LINE_NUM = 4; //ȡ������ˮƽ����ǰ�����������ཻ
const int THRESHOLD = 127;  //���ڶ�ֵͼ(0\255) ����ǰ��\��������ֵ ��������Ϊ127

/*****************************************************************
Global Variables
*****************************************************************/

/*****************************************************************
Function
*****************************************************************/
/*****************************************************************
Name:			AnalyzeThread
Inputs:
	AnalyseProcess* lpParam - ��������ָ��
Return Value:
	none
Description:	�������߳�
*****************************************************************/
//DWORD WINAPI AnalyseThread(LPVOID lpParam)			//�����߳�
//{
//	AnalyseProcess *t_pPoint;
//	t_pPoint = (AnalyseProcess *)lpParam;
//
//	t_pPoint->Analyse();		//��������
//
//	return 1;
//}//AnalyzeThread


/*****************************************************************
Name:			AnalyzeProcess
Inputs:
	none
Return Value:
	none
Description:	AnalyzeProcess���캯��
*****************************************************************/
AnalyseProcess::AnalyseProcess()
{
	//m_SurgicalPlanPtr.reset(new SurgicalPlan());		//�����ƻ�
	m_PositionManagerPtr.reset(new PositionManager());	//λ�ù���
	m_ImageSamplerPtr.reset(new ImageSampler());		//�и����
	m_RigidTransformPtr.reset(new RigidTransform());	//����任����
	m_ImageProcessorPtr.reset(new ImageProcessor());	//����2Dͼ�񹤾���
	m_nShowImageX = 0;
	m_nShowImageY = 0;
	m_fMaxX = 0;
	m_fMaxY = 0;
	m_fMaxZ = 0;
	m_NextFrameDataPtr.reset(new FrameData);
	m_CurrentFrameDataPtr = nullptr;
	m_bAnalysing = false;
	m_nAnalyseState = INIT;
	m_pProstateMask = nullptr;
	m_pLesionMask = nullptr;
	m_pRectumMask = nullptr;
}//AnalyzeProcess


/*****************************************************************
Name:			~AnalyzeProcess
Inputs:
	none
Return Value:
	none
Description:	AnalyzeProcess��������
*****************************************************************/
AnalyseProcess::~AnalyseProcess()
{
	this->StopAnalyse();
}//~AnalyzeProcess


/*****************************************************************
Name:			InitAnalyseProcess
Inputs:
	none
Return Value:
	int - error info
Description:	��ʼ������ģ��
*****************************************************************/
int AnalyseProcess::InitAnalyseProcess(CString t_strFilePathName)
{
	//��config�ļ��У���ȡ��ǰMRI���ݷ������
	if (AnalyseConfig::Instance().LoadConfigFile(t_strFilePathName) != LIST_NO_ERROR)
	{
		return ER_OpenAnalyseConfigFileFailed;
	}

	//����ImageSampler��
	if (m_ImageSamplerPtr->SetDataPackage(m_SurgicalPlanPtr) != LIST_NO_ERROR)
	{
		return ER_NoSurgicalPlan;
	}
	m_ImageSamplerPtr->SetMRIPixelSize(AnalyseConfig::Instance().m_fVoxelSizeX, AnalyseConfig::Instance().m_fVoxelSizeY, AnalyseConfig::Instance().m_fVoxelSizeZ);
	//m_ImageSamplerPtr->SetProbeOffset(m_NDIOperatorPtr->GetProbeOffset());  //TODO
	m_ImageSamplerPtr->SetProbeOffset(m_NDIOperatorPtr->GetRightOffset(), m_NDIOperatorPtr->GetUpOffset(), m_NDIOperatorPtr->GetMoveOffset());  //ʸ״��ScanCenter����ں����ScanCenter��ƫ��

	//��MRIģ�������baseλ�ô��ݸ�PositionManager
	m_PositionManagerPtr->m_BaseMRIAttitude = AnalyseConfig::Instance().m_Attitude;

	//����Ĭ�ϵ�USλ�� �����ݲ����ᱻʹ��
	//m_simulatedAttitude.SetValue(fsutility::Coordinate(0, 0, 0, 1), fsutility::Coordinate(1, 0, 0, 0), fsutility::Coordinate(0, 1, 0, 0), fsutility::Coordinate(0, 0, -1, 0));  //��������һ�� ����̽ͷ��λ��
	//m_PositionManagerPtr->m_BaseUSAttitude = m_simulatedAttitude;
	//m_PositionManagerPtr->m_CurUSAttitude = m_simulatedAttitude;
	
	//ʹ��Ĭ��λ�� ���г�ʼ��׼
	//if (m_PositionManagerPtr->CalculateTransformMatrix() != LIST_NO_ERROR)
	//{
	//	return ER_CalculateTransformMatrix;
	//}
	m_nAnalyseState = PENDING;
	return LIST_NO_ERROR;
}//InitAnalyseProcess

int AnalyseProcess::InitAnalyseProcess(std::string t_strFilePathName)
{
	CString t_cstrFilePathName(t_strFilePathName.data());
	return InitAnalyseProcess(t_cstrFilePathName);
}


/*****************************************************************
Name:			SetNDIDevicePtr
Inputs:
	NDIOperatorPtr t_NDIOperatorPtr - NDI�豸ָ��
Return Value:
	none
Description:	����NDI�豸����ָ��
*****************************************************************/
void AnalyseProcess::SetNDIDevicePtr(NDIOperatorPtr t_NDIOperatorPtr)
{
	m_NDIOperatorPtr = t_NDIOperatorPtr;

	m_NDIOperatorPtr->BindUpdateAttitudeEvent(std::bind(&AnalyseProcess::UpdateNDIData, this, std::placeholders::_1));
}//SetNDIDevicePtr


/*****************************************************************
Name:			SetUSBCapturerPtr
Inputs:
	USBCapturerPtr t_USBCapturerPtr - ����ͼ���ȡ�豸ָ��
Return Value:
	none
Description:	���ó���ͼ���ȡ�豸����ָ��
*****************************************************************/
void AnalyseProcess::SetUSBCapturerPtr(USBCapturerPtr t_USBCapturerPtr)
{
	m_USBCapturerPtr = t_USBCapturerPtr;

	m_USBCapturerPtr->BindCapturePerFrameEvent(std::bind(&AnalyseProcess::UpdateUSBData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}//SetUSBCapturerPtr


/*****************************************************************
Name:			SetSurgicalPlanPtr
Inputs:
	SurgicalPlanPtr t_SurgicalPlanPtr - �����ƻ�����ָ��
Return Value:
	none
Description:	���������ƻ�����ָ��
*****************************************************************/
void AnalyseProcess::SetSurgicalPlanPtr(SurgicalPlanPtr t_SurgicalPlanPtr)
{
	m_SurgicalPlanPtr = t_SurgicalPlanPtr;
}//SetSurgicalPlanPtr

/*****************************************************************
Name:			Set2DImageSize
Inputs:
	int t_nWidth - 2Dͼ����
	int t_nHeight - 2Dͼ��߶�
Return Value:
	none
Description:	����2Dͼ��ߴ�
*****************************************************************/
void AnalyseProcess::Set2DImageSize(int t_nWidth, int t_nHeight)
{
	if (t_nWidth <= 50 || t_nHeight <= 50
		|| t_nWidth > 3000 || t_nHeight > 3000)
	{
		return;
	}

	m_nShowImageX = t_nWidth;
	m_nShowImageY = t_nHeight;
}//Set2DImageSize


/*****************************************************************
Name:			Set3DSize
Inputs:
	float t_fMaxX - �������߶�
	float t_fMaxY - ���������
	float t_fMaxZ - ���������
Return Value:
	none
Description:	����3Dͼ��ߴ�
*****************************************************************/
void AnalyseProcess::Set3DSize(float t_fMaxX, float t_fMaxY, float t_fMaxZ)
{
	if (t_fMaxX <= 50 || t_fMaxX > 1000
		|| t_fMaxY <= 50 || t_fMaxY > 1000
		|| t_fMaxZ <= 20 || t_fMaxZ > 1000)
	{
		return;
	}

	m_fMaxX = t_fMaxX;
	m_fMaxY = t_fMaxY;
	m_fMaxZ = t_fMaxZ;
}//Set3DSize

/*****************************************************************
Name:			MoveMRIRight
Inputs:
	float step - ��rightDir�ƶ��ľ���(mm)
Return Value:
	none
Description:	ͨ���ƶ�base MRI λ�� ���ƶ�mask
*****************************************************************/
void AnalyseProcess::MoveMRIRight(float distance)
{
	m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter =
		m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter - m_PositionManagerPtr->m_BaseMRIAttitude.m_RightDir*distance;
	CSingleLock singlelock(&m_ProcessDataMutex);
	//singlelock.Lock();
	//m_PositionManagerPtr->CalculateTransformMatrix();
	//singlelock.Unlock();
}

/*****************************************************************
Name:			MoveMRIUp
Inputs:
	float step - ��upDir�ƶ��ľ���(mm)
Return Value:
	none
Description:	ͨ���ƶ�base MRI λ�� ���ƶ�mask
*****************************************************************/
void AnalyseProcess::MoveMRIUp(float distance)
{
	m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter =
		m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter - m_PositionManagerPtr->m_BaseMRIAttitude.m_UpDir*distance;
	CSingleLock singlelock(&m_ProcessDataMutex);
	//singlelock.Lock();
	//m_PositionManagerPtr->CalculateTransformMatrix();
	//singlelock.Unlock();
}

/*****************************************************************
Name:			MoveMRIForward
Inputs:
	float step - ��moveDir�ƶ��ľ���(mm)
Return Value:
	none
Description:	ͨ���ƶ�base MRI λ�� ���ƶ�mask
*****************************************************************/
void AnalyseProcess::MoveMRIForward(float distance)
{
	m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter =
		m_PositionManagerPtr->m_BaseMRIAttitude.m_ScanCenter - m_PositionManagerPtr->m_BaseMRIAttitude.m_MoveDir*distance;
	CSingleLock singlelock(&m_ProcessDataMutex);
	//singlelock.Lock();
	//m_PositionManagerPtr->CalculateTransformMatrix();
	//singlelock.Unlock();
}

/*****************************************************************
Name:			StartAnalyze
Inputs:
	none
Return Value:
	none
Description:	�����������������������߳�
*****************************************************************/
int AnalyseProcess::StartAnalyse()
{
	//m_hAnalyzeThread = CreateThread(nullptr, 0, AnalyseThread, this, 0, nullptr);
	if (m_bAnalysing)
		return ER_InitAnalyseProcessFailed;

	//���ٷ����߳�
	m_bAnalysing = true;
	m_tAnalyseThread = thread(bind(&AnalyseProcess::Analyse, this));
	m_tAnalyseThread.detach();
	return LIST_NO_ERROR;
}//StartAnalyze

/*****************************************************************
Name:			StopAnalyze
Inputs:
	none
Return Value:
	none
Description:	���������������޸ı�־
*****************************************************************/
int AnalyseProcess::StopAnalyse()
{
	if (m_USBCapturerPtr->StopGrab() != LIST_NO_ERROR)
		return ER_CloseAnalyseProcessFailed;
	if (m_NDIOperatorPtr->StopTracking() != LIST_NO_ERROR)
		return ER_CloseAnalyseProcessFailed;
	if (m_bAnalysing == false)
		return ER_CloseAnalyseProcessFailed;

	m_bAnalysing = false;
	m_nAnalyseState = INIT;
	return LIST_NO_ERROR;
}//StopAnalyze


/*****************************************************************
Name:			BindUpdateFrameEvent
Inputs:
	none
Return Value:
	none
Description:	�󶨻ص�����
*****************************************************************/
void AnalyseProcess::BindUpdateFrameEvent(Fun_UpdateFrameEvent eventFun)
{
	m_UpdateFrameFun = eventFun;
	return;
}

/*****************************************************************
Name:			Analyze
Inputs:
	none
Return Value:
	none
Description:	ʵ�ʷ�������
*****************************************************************/
void AnalyseProcess::Analyse()
{
	while (m_bAnalysing)
	{
		CSingleLock singlelock(&m_ProcessDataMutex);
		singlelock.Lock();
		//����д�����ģ�����B���ɼ�ģ���ͼ�����ݣ�������Ӧ��MRIģ����������ɶ�Ӧ��FrameData
		if(m_NextFrameDataPtr->m_USImgT.data || m_NextFrameDataPtr->m_USImgS.data)
		{
			m_CurrentFrameDataPtr = m_NextFrameDataPtr;
			m_NextFrameDataPtr.reset(new FrameData);
			singlelock.Unlock();
		}
		//����û�д������B��ͼ�����ݣ���������
		else
		{
			Sleep(15);
			singlelock.Unlock();
			continue;
		}

		ProcessSingleFrame(m_CurrentFrameDataPtr);		//����֡����
	}
}


/*****************************************************************
Name:			Register
Inputs:
	none
Return Value:
	none
Description:	��׼����,����任������ҽ���ֶ�����
*****************************************************************/
int AnalyseProcess::Register()
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	//���÷���״̬ΪRegistration(������׼)
	m_nAnalyseState = REGISTERING1;
	//�ȴ�NDI�豸�������µ�US̽ͷλ��
	while (m_nAnalyseState != REGISTERING2)
		Sleep(15);
	//����ǰ����λ����Ϊ����baseλ�ã���¼��PositionManager��
	singlelock.Lock();
	m_PositionManagerPtr->m_BaseUSAttitude = m_PositionManagerPtr->m_CurUSAttitude;
	singlelock.Unlock();
	//�� US��baseλ�á�MRIģ�������baseλ�á��������ݣ�����任����
	m_RigidTransformPtr->SetSrcAttitude(m_PositionManagerPtr->m_BaseUSAttitude);
	m_RigidTransformPtr->SetDstAttitude(m_PositionManagerPtr->m_BaseMRIAttitude);
	m_RigidTransformPtr->SetSrcPts(m_USPts);
	m_RigidTransformPtr->SetDstPts(m_SurgicalPlanPtr->GetObjDataPtr(1)->GetVertices());
	m_PositionManagerPtr->m_TransformMatrix = m_RigidTransformPtr->CalTransformMatrix();
	m_nAnalyseState = PUNCTURE;
	return LIST_NO_ERROR;
	//if (m_PositionManagerPtr->CalculateTransformMatrix() != LIST_NO_ERROR)
	//{
	//	return ER_CalculateTransformMatrix;
	//}
	//else
	//{
	//	m_nAnalyseState = PUNCTURE;
	//	return LIST_NO_ERROR;
	//}
}

/*****************************************************************
Name:			ResetRegister
Inputs:
	none
Return Value:
	none
Description:	������׼��������ҽ���ֶ�����
*****************************************************************/
int ANALYSEPROCESS::AnalyseProcess::ResetRegister()
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	//���ó���̽ͷ��λ��
	//m_PositionManagerPtr->m_BaseUSAttitude = m_simulatedAttitude;
	//����MRIģ�����̽ͷ��baseλ��
	m_PositionManagerPtr->m_BaseMRIAttitude = AnalyseConfig::Instance().m_Attitude;  //�������ļ� ����MRIģ�������baseλ��
	//���¼���궨����
	m_RigidTransformPtr->Reset();  //�������ڸ���任���������
	m_nAnalyseState = PENDING;  //����Ϊδ��׼״̬
	singlelock.Unlock();
	return 0;
}

/*****************************************************************
Name:			UpdateNDIData
Inputs:
	none
Return Value:
	none
Description:	��NDI���ݸ��³���λ�ò������ص���������NDIģ����á�
*****************************************************************/
void AnalyseProcess::UpdateNDIData(fsutility::Attitude attitude)
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	m_PositionManagerPtr->m_CurUSAttitude = attitude;
	if (m_nAnalyseState == REGISTERING1)
		m_nAnalyseState = REGISTERING2;  //US��ǰλ����Ϣ׼���������������ڼ�����׼����
	singlelock.Unlock();
	return;
}//UpdateNDIData

/*****************************************************************
Name:			UpdateUSBData
Inputs:
	cv::Mat t_USBImgA - ȫͼ���ϰ���ͼ��
	cv::Mat t_USBImgB - �°���ͼ�񣬿���Ϊ��
	int m_dImageRes - ���ű���
Return Value:
	none
Description:	����USB���ݣ��ص���������USģ�����
*****************************************************************/
//void AnalyseProcess::UpdateUSBData(cv::Mat t_USBImgA, cv::Mat t_USBImgB, double m_dImageRes)
//{
//	CSingleLock singlelock(&m_ProcessDataMutex);
//	singlelock.Lock();
//	m_NextFrameDataPtr->m_USBImage = t_USBImgA;
//	m_NextFrameDataPtr->m_dImageRes = m_dImageRes;
//	
//	m_ImageSamplerPtr->SetUSPixelSize(m_dImageRes);	//����B��ͼ�����ش�С(һ�����ص�����ߴ�)
//	m_ImageSamplerPtr->SetImageSize(t_USBImgA.cols, t_USBImgA.rows);	//����B��ͼ���С(������)
//	singlelock.Unlock();
//}//UpdateUSBData

/*****************************************************************
Name:			UpdateUSBData
Inputs:
	cv::Mat t_USBImgA - ȫͼ���ϰ���ͼ��
	cv::Mat t_USBImgB - �°���ͼ�񣬿���Ϊ��
	int m_dImageRes - ���ű���
Return Value:
	none
Description:	����USB���ݣ��ص���������USģ�����
*****************************************************************/
void AnalyseProcess::UpdateUSBData(cv::Mat t_USBImgT, cv::Mat t_USBImgS, double dPixelSizeT, double dPixelSizeS)
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();

	m_NextFrameDataPtr->m_USImgT = t_USBImgT;
	m_NextFrameDataPtr->m_USImgS = t_USBImgS;
	m_NextFrameDataPtr->m_dPixelSizeT = dPixelSizeT;
	m_NextFrameDataPtr->m_dPixelSizeS = dPixelSizeS;
	
	singlelock.Unlock();
}//UpdateUSBData


vector<Coordinate> ANALYSEPROCESS::AnalyseProcess::GetContourPts(FrameDataPtr t_FrameDataPtr)
{
	int width, height,y,x,step,count;
	double pixelSize;
	Coordinate pixelPos;  //��ǰ���� ����ʵUS̽ͷ���ڵ���������ϵ�µ�����
	vector<pair<int, int>> pixels;  //һ�����������أ���2Dͼ����������(x,y)
	vector <Coordinate> result;  //һ�����������أ���NDI�ų��涨��3D�ռ�����(x,y,z,1)
	cv::Mat srcImg;
	cv::Mat binaryImg;

	//�ж������Ƿ���ڡ���������������ۺ����
	srcImg = t_FrameDataPtr->m_USImgT;
	if (srcImg.empty())
		return result;
	pixelSize = t_FrameDataPtr->m_dPixelSizeS;
	width = srcImg.cols;
	height = srcImg.rows;
	//ת�Ҷȱ���ȡ��ֵ
	binaryImg = m_ImageProcessorPtr->SegmentByThreshold(srcImg);
	//�����ߣ����������ཻ�����ɸ���
	step = height / (HORIZONTAL_LINE_NUM + 1);
	for (y = step, count = 0; count < HORIZONTAL_LINE_NUM; y += step, count++)
	{
		for(x=0;x<width;x++)
			if (binaryImg.at<uchar>(y, x) > THRESHOLD)
			{
				pixels.push_back({ x,y });
				break;
			}
		for(x=width-1;x>=0;x--)
			if (binaryImg.at<uchar>(y, x) > THRESHOLD)
			{
				pixels.push_back({ x,y });
				break;
			}
	}
	//�����ߣ����������ཻ�����ɸ���
	step = width / (VERTICAL_LINE_NUM+1);
	for (x = step, count = 0; count < VERTICAL_LINE_NUM; x += step, count++)
	{
		for(y=0;y<height;y++)
			if (binaryImg.at<uchar>(y, x) > THRESHOLD)
			{
				pixels.push_back({ x,y });
				break;
			}
		for(y=height-1;y>=0;y--)
			if (binaryImg.at<uchar>(y, x) > THRESHOLD)
			{
				pixels.push_back({ x,y });
				break;
			}
	}
	//��2Dͼ���������꣬תΪ3D�ռ�����
	for (int i = 0; i < pixels.size(); i++) {
		x = pixels[i].first;
		y = pixels[i].second;
		pixelPos = m_PositionManagerPtr->m_CurUSAttitude.m_ScanCenter
			+ m_PositionManagerPtr->m_CurUSAttitude.m_UpDir*(height - y) * pixelSize
			+ m_PositionManagerPtr->m_CurUSAttitude.m_RightDir*(x - width / 2)*pixelSize;
		result.push_back(pixelPos);
	}
	return result;
}

/*****************************************************************
Name:			ProcessSingleFrame
Inputs:
	none
Return Value:
	none
Description:	����֡����
*****************************************************************/
void AnalyseProcess::ProcessSingleFrame(FrameDataPtr t_FrameDataPtr)
{
	//���ݵ�ǰ����״̬(����׼/δ��׼),���ô�һ����������
	if (m_nAnalyseState == PUNCTURE)
	{
		ProcessSingleFrameB(t_FrameDataPtr);	//����׼״̬�����÷������� �������
	}
	else if (m_nAnalyseState == PENDING)
	{
		ProcessSingleFrameA(t_FrameDataPtr);	//δ��׼״̬�����淵��Ĭ��ֵ
		vector<Coordinate> pts = GetContourPts(t_FrameDataPtr);  //δ��׼״̬����ȡǰ�������������ɵ�����
		m_USPts.insert(m_USPts.end(), pts.begin(), pts.end());
	}
	else
		;
	//������������͸�dlg
	if (m_UpdateFrameFun != nullptr)
	{
		m_UpdateFrameFun(t_FrameDataPtr);
	}
	return;
}


/*****************************************************************
Name:			ProcessSingleFrameA
Inputs:
	none
Return Value:
	none
Description:	����֡����(Init��Registration��δ����״̬)
*****************************************************************/
void AnalyseProcess::ProcessSingleFrameA(FrameDataPtr t_FrameDataPtr)
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();

	//���㵱ǰ����λ��
	//m_PositionManagerPtr->UpDate();	//�����Ѿ���ȡ�ĳ���̽ͷλ�ò���������MRIģ�����λ�ò���
	//m_ImageSamplerPtr->SetPosition(m_PositionManagerPtr->m_CurMRIAttitude);	//ΪImageSampler������̬����
	m_ImageSamplerPtr->SetPosition(m_PositionManagerPtr->m_BaseMRIAttitude);  //ΪImageSampler������̬����
	t_FrameDataPtr->SetPosition(m_PositionManagerPtr->m_CurMRIAttitude);  //����ǰmriģ�������̬��������FrameDataPtr

	int width, height;
	//��������Transverse
	if (!t_FrameDataPtr->m_USImgT.empty())
	{
		width = t_FrameDataPtr->m_USImgT.cols;
		height = t_FrameDataPtr->m_USImgT.rows;
		m_ImageSamplerPtr->SetUSPixelSize(t_FrameDataPtr->m_dPixelSizeT);
		m_ImageSamplerPtr->SetImageSize(width, height);
		m_pProstateMask = new BYTE[width * height];
		m_pLesionMask = new BYTE[width * height];
		m_pRectumMask = new BYTE[width * height];
		memset(m_pProstateMask, 0, sizeof(BYTE)*width*height);
		memset(m_pLesionMask, 0, sizeof(BYTE)*width*height);
		memset(m_pRectumMask, 0, sizeof(BYTE)*width*height);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pProstateMask, 0, 1);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pLesionMask, 0, 2);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pRectumMask, 0, 3);
		Mat prostateMask(height, width, CV_8UC1, m_pProstateMask);
		Mat lesionMask(height, width, CV_8UC1, m_pLesionMask);
		Mat rectumMask(height, width, CV_8UC1, m_pRectumMask);
		t_FrameDataPtr->m_prostateMaskT = prostateMask.clone();
		t_FrameDataPtr->m_lesionMaskT = lesionMask.clone();
		t_FrameDataPtr->m_rectumMaskT = rectumMask.clone();
		delete[] m_pProstateMask;
		delete[] m_pLesionMask;
		delete[] m_pRectumMask;
	}
	//����ʸ״��Sagittal
	if (!t_FrameDataPtr->m_USImgS.empty())
	{
		width = t_FrameDataPtr->m_USImgS.cols;
		height = t_FrameDataPtr->m_USImgS.rows;
		m_ImageSamplerPtr->SetUSPixelSize(t_FrameDataPtr->m_dPixelSizeS);
		m_ImageSamplerPtr->SetImageSize(width, height);
		m_pProstateMask = new BYTE[width * height];
		m_pLesionMask = new BYTE[width * height];
		m_pRectumMask = new BYTE[width * height];
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pProstateMask, 1, 1);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pLesionMask, 1, 2);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pRectumMask, 1, 3);
		Mat prostateMask(height, width, CV_8UC1, m_pProstateMask);
		Mat lesionMask(height, width, CV_8UC1, m_pLesionMask);
		Mat rectumMask(height, width, CV_8UC1, m_pRectumMask);
		t_FrameDataPtr->m_prostateMaskS = prostateMask.clone();
		t_FrameDataPtr->m_lesionMaskS = lesionMask.clone();
		t_FrameDataPtr->m_rectumMaskS = rectumMask.clone();
		delete[] m_pProstateMask;
		delete[] m_pLesionMask;
		delete[] m_pRectumMask;
	}
	singlelock.Unlock();
	return;
}


/*****************************************************************
Name:			ProcessSingleFrameB
Inputs:
	none
Return Value:
	none
Description:	����֡����(Puncture���ѹ���״̬)
*****************************************************************/
void AnalyseProcess::ProcessSingleFrameB(FrameDataPtr t_FrameDataPtr)
{
	//����������
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();

	//���㵱ǰ����λ��
	m_PositionManagerPtr->UpDate();	//�����Ѿ���ȡ�ĳ���̽ͷλ�ò���������MRIģ�����λ�ò���
	m_ImageSamplerPtr->SetPosition(m_PositionManagerPtr->m_CurMRIAttitude);	//ΪImageSampler������̬����
	t_FrameDataPtr->SetPosition(m_PositionManagerPtr->m_CurMRIAttitude);  //����ǰmriģ�������̬��������FrameDataPtr

	//���µ�ǰ����4���ǵ��λ��(wld)
	//m_ImageSamplerPtr->GetPlaneCorners(t_FrameDataPtr->m_LeftTop, t_FrameDataPtr->m_RightTop, t_FrameDataPtr->m_LeftBottom, t_FrameDataPtr->m_RightBottom);

	int width, height;
	//��������Transverse
	if (!t_FrameDataPtr->m_USImgT.empty())
	{
		width = t_FrameDataPtr->m_USImgT.cols;
		height = t_FrameDataPtr->m_USImgT.rows;
		m_ImageSamplerPtr->SetUSPixelSize(t_FrameDataPtr->m_dPixelSizeT);
		m_ImageSamplerPtr->SetImageSize(width, height);
		m_pProstateMask = new BYTE[width * height];
		m_pLesionMask = new BYTE[width * height];
		m_pRectumMask = new BYTE[width * height];
		//����mask����
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pProstateMask, 0, 1);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pLesionMask, 0, 2);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pRectumMask, 0, 3);
		//���½ǵ�λ����FrameDataPtr
		m_ImageSamplerPtr->GetPlaneCorners(t_FrameDataPtr->m_TransverseLeftTop, t_FrameDataPtr->m_TransverseRightTop, 
			t_FrameDataPtr->m_TransverseLeftBottom, t_FrameDataPtr->m_TransverseRightBottom);
		Mat prostateMask(height, width, CV_8UC1, m_pProstateMask);
		Mat lesionMask(height, width, CV_8UC1, m_pLesionMask);
		Mat rectumMask(height, width, CV_8UC1, m_pRectumMask);
		//����mask��FrameDataPtr
		t_FrameDataPtr->m_prostateMaskT = prostateMask.clone();
		t_FrameDataPtr->m_lesionMaskT = lesionMask.clone();
		t_FrameDataPtr->m_rectumMaskT = rectumMask.clone();
		delete[] m_pProstateMask;
		delete[] m_pLesionMask;
		delete[] m_pRectumMask;
	}
	//����ʸ״��Sagittal
	if (!t_FrameDataPtr->m_USImgS.empty())
	{
		width = t_FrameDataPtr->m_USImgS.cols;
		height = t_FrameDataPtr->m_USImgS.rows;
		m_ImageSamplerPtr->SetUSPixelSize(t_FrameDataPtr->m_dPixelSizeS);
		m_ImageSamplerPtr->SetImageSize(width, height);
		m_pProstateMask = new BYTE[width * height];
		m_pLesionMask = new BYTE[width * height];
		m_pRectumMask = new BYTE[width * height];
		//����mask����
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pProstateMask, 1, 1);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pLesionMask, 1, 2);
		m_ImageSamplerPtr->GetSampleMaskPlan(m_pRectumMask, 1, 3);
		//���½ǵ�λ����FrameDataPtr
		m_ImageSamplerPtr->GetPlaneCorners(t_FrameDataPtr->m_SagittalLeftTop, t_FrameDataPtr->m_SagittalRightTop,
			t_FrameDataPtr->m_SagittalLeftBottom, t_FrameDataPtr->m_SagittalRightBottom);
		Mat prostateMask(height, width, CV_8UC1, m_pProstateMask);
		Mat lesionMask(height, width, CV_8UC1, m_pLesionMask);
		Mat rectumMask(height, width, CV_8UC1, m_pRectumMask);
		//����mask��FrameDataPtr
		t_FrameDataPtr->m_prostateMaskS = prostateMask.clone();
		t_FrameDataPtr->m_lesionMaskS = lesionMask.clone();
		t_FrameDataPtr->m_rectumMaskS = rectumMask.clone();
		delete[] m_pProstateMask;
		delete[] m_pLesionMask;
		delete[] m_pRectumMask;
	}

	singlelock.Unlock();
	return;
}

/*****************************************************************
Name:			Mask2Edge
Inputs:
	Mat src - �������maskͼƬ 
Return Value:
	Mat - mask��Ե
Description:	ʹ��sobel���� ��mask������(ǿ��Ϊ1)
*****************************************************************/
Mat AnalyseProcess::Mask2Edge(Mat src)
{
	int ddepth = CV_16S;
	int scale = 1, delta = 0;
	Mat res, grad_x, grad_y, grad;
	//��ˮƽ ��ֱ�����ݶ�
	Sobel(src, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	Sobel(src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	//�ݶ�תΪ����ֵ
	convertScaleAbs(grad_x, grad_x);
	convertScaleAbs(grad_y, grad_y);
	//�ϲ��ݶ�
	addWeighted(grad_x, 1, grad_y, 1, 0, grad);
	//��Եǿ����Ϊ1
	threshold(grad, res, 1, 1, 0);
	return res;
}