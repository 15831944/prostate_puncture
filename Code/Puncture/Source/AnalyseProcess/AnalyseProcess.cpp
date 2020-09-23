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
DWORD WINAPI AnalyseThread(LPVOID lpParam)			//�����߳�
{
	AnalyseProcess *t_pPoint;
	t_pPoint = (AnalyseProcess *)lpParam;

	t_pPoint->Analyse();		//��������

	return 1;
}//AnalyzeThread


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
	m_nShowImageX = 0;
	m_nShowImageY = 0;
	m_fMaxX = 0;
	m_fMaxY = 0;
	m_fMaxZ = 0;
	m_NextFrameDataPtr.reset(new FrameData);
	m_CurrentFrameDataPtr = nullptr;
	m_nAnalyseState = Init;
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
	return LIST_NO_ERROR;
}//InitAnalyseProcess


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

	m_USBCapturerPtr->BindCapturePerFrameEvent(std::bind(&AnalyseProcess::UpdateUSBData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
Name:			StartAnalyze
Inputs:
	none
Return Value:
	none
Description:	�����������������������߳�
*****************************************************************/
void AnalyseProcess::StartAnalyse()
{
	m_hAnalyzeThread = CreateThread(nullptr, 0, AnalyseThread, this, 0, nullptr);
	return;
}//StartAnalyze


/*****************************************************************
Name:			StopAnalyze
Inputs:
	none
Return Value:
	none
Description:	���������������޸ı�־
*****************************************************************/
void AnalyseProcess::StopAnalyse()
{
	m_USBCapturerPtr->StopGrab();
	m_NDIOperatorPtr->StopTracking();
	CloseHandle(m_hAnalyzeThread);
	m_nAnalyseState = Init;
	return;
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
	while (1)
	{
		CSingleLock singlelock(&m_ProcessDataMutex);
		singlelock.Lock();
		//����д�����ģ�����B���ɼ�ģ���ͼ�����ݣ�������Ӧ��MRIģ����������ɶ�Ӧ��FrameData
		if (!m_NextFrameDataPtr->m_USBImage.empty())
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
	m_nAnalyseState = Registration;
	//����׼ǰ��Ĭ��NDI�豸�Ѿ��ɼ�����ǰ����̽ͷ��λ��
	//��MRIģ�������baseλ�ô��ݸ�PositionManager
	m_PositionManagerPtr->m_BaseMRIScanCenter = AnalyseConfig::Instance().m_ScanCenter;
	m_PositionManagerPtr->m_BaseMRIRightDir = AnalyseConfig::Instance().m_RightDir;
	m_PositionManagerPtr->m_BaseMRIUpDir = AnalyseConfig::Instance().m_UpDir;
	m_PositionManagerPtr->m_BaseMRIMoveDir = AnalyseConfig::Instance().m_MoveDir;
	//����ǰ����λ����Ϊ����baseλ�ã���¼��PositionManager��
	singlelock.Lock();
	m_PositionManagerPtr->m_BaseUSScanCenter = m_PositionManagerPtr->m_CurUSScanCenter;
	m_PositionManagerPtr->m_BaseUSRightDir = m_PositionManagerPtr->m_CurUSRightDir;
	m_PositionManagerPtr->m_BaseUSUpDir = m_PositionManagerPtr->m_CurUSUpDir;
	m_PositionManagerPtr->m_BaseUSMoveDir = m_PositionManagerPtr->m_CurUSMoveDir;
	singlelock.Unlock();
	//�� US��baseλ�á�MRIģ�������baseλ�ã�����任����
	if (m_PositionManagerPtr->CalculateTransformMatrix() != LIST_NO_ERROR)
	{
		return ER_CalculateTransformMatrix;
	}
	else
	{
		m_nAnalyseState = Puncture;
		return LIST_NO_ERROR;
	}
}


/*****************************************************************
Name:			UpdateNDIData
Inputs:
	none
Return Value:
	none
Description:	��NDI���ݸ��³���λ�ò������ص���������NDIģ����á�
*****************************************************************/
void AnalyseProcess::UpdateNDIData(std::vector<Attitude> t_Attitude)
{
	//����Ĭ�ϴ����һ��NDIλ��
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	m_PositionManagerPtr->SetCurUSPosition(t_Attitude[0]);
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
void AnalyseProcess::UpdateUSBData(cv::Mat t_USBImgA, cv::Mat t_USBImgB, double m_dImageRes)
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	m_NextFrameDataPtr->m_USBImage = t_USBImgA;
	m_NextFrameDataPtr->m_dImageRes = m_dImageRes;
	
	m_ImageSamplerPtr->SetUSPixelSize(m_dImageRes);	//����B��ͼ�����ش�С(һ�����ص�����ߴ�)
	m_ImageSamplerPtr->SetImageSize(t_USBImgA.cols, t_USBImgA.rows);	//����B��ͼ���С(������)
	singlelock.Unlock();
}//UpdateUSBData


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
	if (m_nAnalyseState == Puncture)
	{
		ProcessSingleFrameB(t_FrameDataPtr);	//����׼״̬�����÷������� �������
	}
	else
	{
		ProcessSingleFrameA(t_FrameDataPtr);	//δ��׼״̬�����淵��Ĭ��ֵ
	}
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

	//����mask�ռ�
	singlelock.Lock();
	if (t_FrameDataPtr->CreatMaskData(m_nShowImageX, m_nShowImageY) != LIST_NO_ERROR)
	{
		return;
	}
	//δ����״̬���������MRIģ���������MASKֱ����Ϊȫ0
	//TODO�����ÿյ�MRIԭʼ���ݽ���
	memset(t_FrameDataPtr->m_pFusionMask, 0, sizeof(BYTE)*m_nShowImageX*m_nShowImageY);
	//memset(t_FrameDataPtr->m_pLesionMask, 0, sizeof(BYTE)*m_nShowImageX*m_nShowImageY);
	//memset(t_FrameDataPtr->m_pProstateMask, 0, sizeof(BYTE)*m_nShowImageX*m_nShowImageY);
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

	//����mask�ռ�
	singlelock.Lock();
	if (t_FrameDataPtr->CreatMaskData(m_nShowImageX, m_nShowImageY) != LIST_NO_ERROR)
	{
		return;
	}

	//���㵱ǰ����λ��
	m_PositionManagerPtr->UpDate();	//�����Ѿ���ȡ�ĳ���̽ͷλ�ò���������MRIģ�����λ�ò���
	m_ImageSamplerPtr->SetPosition(m_PositionManagerPtr->m_CurMRIScanCenter, m_PositionManagerPtr->m_CurMRIRightDir, m_PositionManagerPtr->m_CurMRIUpDir, m_PositionManagerPtr->m_CurMRIMoveDir);	//ΪImageSampler����λ�ò���

	//���µ�ǰ������̬(����ImageSampler�ĺ���) TODO
	//t_FrameDataPtr->SetPosition(m_ImageSamplerPtr->WLDToIJK(m_PositionManagerPtr->m_CurMRIScanCenter),
	//	m_ImageSamplerPtr->WLDToIJK(m_PositionManagerPtr->m_CurMRIRightDir),
	//	m_ImageSamplerPtr->WLDToIJK(m_PositionManagerPtr->m_CurMRIUpDir),
	//	m_ImageSamplerPtr->WLDToIJK(m_PositionManagerPtr->m_CurMRIMoveDir));
	t_FrameDataPtr->SetPosition(m_PositionManagerPtr->m_CurMRIScanCenter,
		m_PositionManagerPtr->m_CurMRIRightDir,
		m_PositionManagerPtr->m_CurMRIUpDir,
		m_PositionManagerPtr->m_CurMRIMoveDir);

	//���µ�ǰ����4���ǵ��λ��(wld)
	m_ImageSamplerPtr->GetPlaneCorners(t_FrameDataPtr->m_LeftTop, t_FrameDataPtr->m_RightTop, t_FrameDataPtr->m_LeftBottom, t_FrameDataPtr->m_RightBottom);

	//�ü�3��mask����
	if (m_pProstateMask == nullptr)
		m_pProstateMask = new BYTE[m_nShowImageX*m_nShowImageY];
	if (m_pLesionMask == nullptr)
		m_pLesionMask = new BYTE[m_nShowImageX*m_nShowImageY];
	if (m_pRectumMask == nullptr)
		m_pRectumMask = new BYTE[m_nShowImageX*m_nShowImageY];
	m_ImageSamplerPtr->GetSampleMaskPlan(m_pProstateMask, 0, 1);
	////���Դ���TODO
	//int frontSum = 0;
	//for (int i = 0; i < 800 * 700; i++)
	//	frontSum += m_pProstateMask[i];
	////���Դ���TODO
	m_ImageSamplerPtr->GetSampleMaskPlan(m_pLesionMask, 0, 2);
	m_ImageSamplerPtr->GetSampleMaskPlan(m_pRectumMask, 0, 3);
	//maskתΪ����
	Mat prostateContour(m_nShowImageY, m_nShowImageX, CV_8UC1, m_pProstateMask);
	Mat lesionContour(m_nShowImageY, m_nShowImageX, CV_8UC1, m_pLesionMask);
	Mat rectumContour(m_nShowImageY, m_nShowImageX, CV_8UC1, m_pRectumMask);
	prostateContour = Mask2Edge(prostateContour);
	lesionContour = Mask2Edge(lesionContour);
	rectumContour = Mask2Edge(rectumContour);
	//�ں�ΪFusionMask
	Mat fusionContour;
	addWeighted(prostateContour, 1, lesionContour, 2, 0, fusionContour);
	addWeighted(fusionContour, 1, rectumContour, 3, 0, fusionContour);
	//imwrite("D:\\fusionMask.bmp", fusionContour);	//���Դ���TODO
	//����FrameData
	memcpy(t_FrameDataPtr->m_pFusionMask, fusionContour.data, m_nShowImageX*m_nShowImageY * sizeof(BYTE));
	singlelock.Unlock();

	////�ü���������
	////TODO ԭͼ����
	//m_ImageSamplerPtr->GetSampleMaskPlan(t_FrameDataPtr->m_pProstateMask, 0, 0);
	//m_ImageSamplerPtr->GetSampleMaskPlan(t_FrameDataPtr->m_pLesionMask, 0, 1);
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