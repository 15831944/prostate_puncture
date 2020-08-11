//*****************************************************************
// �ļ��� :						AnalyzeProcess.cpp
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				ʵ�ʷ�������ģ��
// �������� :					2019.11.1
// �޸����� :					
// ���� :						Fiki
// �޸��� :						
// ��ϵ��ʽ :					
//*****************************************************************/

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "stdafx.h"
#include "AnalyzeProcess.h"
#include "ErrorManager//ErrorCodeDefine.h"
#include "ComUtility/SysPathManager.h"
#include "ComUtility//IniFile.h"

using namespace fsutility;


///////////////////////////////////////////////////////
////////////////////////define/////////////////////////
///////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************
Global Variables
*****************************************************************/



/*****************************************************************
Function
*****************************************************************/
/*****************************************************************
Name:			AnalyzeThread
Inputs:
	ScanDeviceManagerPtr pDevice - �豸ָ��
Return Value:
	none
Description:	�������߳�
*****************************************************************/
DWORD WINAPI AnalyzeThread(LPVOID lpParam)			//�����߳�
{
	AnalyzeProcess *t_pPoint;
	t_pPoint = (AnalyzeProcess *)lpParam;
	
	t_pPoint->Analyze();		//��������

	return 1;
}//AnalyzeThread



/*****************************************************************
Name:			AnalyzeProcess(�����)
Inputs:
	none
Return Value:
	none
Description:	AnalyzeProcess���캯��
*****************************************************************/
AnalyzeProcess::AnalyzeProcess()
{
	m_SurgicalPlanPtr.reset(new SurgicalPlan());	//�����ƻ�
	m_pMaskSampler.reset(new ImageSampler());		//��ģ����

	m_nShowImageX = 0;
	m_nShowImageY = 0;

	m_fMaxX = 0;
	m_fMaxY = 0;
	m_fMaxZ = 0;

	m_bStopAnalyze = false;

	m_NextFrameDataPtr.reset(new FrameData);
	m_CurrentFrameDataPtr = nullptr;

	//��������任����
	CString t_strIniFileNamePath;
	t_strIniFileNamePath = CSysPathManager::Instance().GetConfigPath();
	t_strIniFileNamePath = t_strIniFileNamePath + _T("Analyze.ini");
	InportTransMatrixN_U(t_strIniFileNamePath);
}//AnalyzeProcess


/*****************************************************************
Name:			~AnalyzeProcess(�����)
Inputs:
	none
Return Value:
	none
Description:	AnalyzeProcess��������
*****************************************************************/
AnalyzeProcess::~AnalyzeProcess()
{
}//~AnalyzeProcess


/*****************************************************************
Name:			InportTransMatrixN_U(�����)
Inputs:
	none
Return Value:
	none
Description:	����NDI����B������ת������
*****************************************************************/
int AnalyzeProcess::InportTransMatrixN_U(CString m_strFileName)
{
	//�����
	//�ж��ļ��Ƿ����
	if (!PathFileExists(m_strFileName))
	{
		return ER_WrongAnalyzeConfigFilePathName;
	}

	fsutility::CIniFile t_ScanConfig;	//config�ļ�

	if (!t_ScanConfig.Open(m_strFileName))
	{
		return ER_OpenUSBConfigFileFailed;
	}

	int i;
	for (i = 0; i < 6; ++i)
	{
		Mat t_Martix(4, 4, CV_32FC1);

		CString t_strSection;
		t_strSection.Format(_T("Martix%d"), i);
		t_ScanConfig.Move2Section(t_strSection);
		//t_ScanConfig.ReadKey(_T("00"), );


	}
	
	return LIST_NO_ERROR;
}


/*****************************************************************
Name:			BindUpdateFrameEvent
Inputs:
	none
Return Value:
	none
Description:	ʵ�ʷ�������
*****************************************************************/
void AnalyzeProcess::BindUpdateFrameEvent(Fun_UpdateFrameEvent eventFun)
{
	m_UpdateFrameFun = eventFun;
}//BindUpdateFrameEvent


/*****************************************************************
Name:			SetNDIDevicePtr
Inputs:
	NDIOperatorPtr t_NDIOperatorPtr - NDI�豸ָ��
Return Value:
	none
Description:	����NDI�豸����ָ��
*****************************************************************/
void AnalyzeProcess::SetNDIDevicePtr(NDIOperatorPtr t_NDIOperatorPtr)
{
	m_NDIOperatorPtr = t_NDIOperatorPtr;

	m_NDIOperatorPtr->BindUpdateAttitudeEvent(std::bind(&AnalyzeProcess::UpdateNDIData, this, std::placeholders::_1));
}//SetNDIDevicePtr


/*****************************************************************
Name:			SetUSBCapturerPtr
Inputs:
	USBCapturerPtr t_USBCapturerPtr - ����ͼ���ȡ�豸ָ��
Return Value:
	none
Description:	���ó���ͼ���ȡ�豸����ָ��
*****************************************************************/
void AnalyzeProcess::SetUSBCapturerPtr(USBCapturerPtr t_USBCapturerPtr)
{
	m_USBCapturerPtr = t_USBCapturerPtr;

	m_USBCapturerPtr->BindCapturePerFrameEvent(std::bind(&AnalyzeProcess::UpdateUSBData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ));
}//SetUSBCapturerPtr


/*****************************************************************
Name:			Set2DImageSize
Inputs:
	int t_nWidth - 2Dͼ����
	int t_nHeight - 2Dͼ��߶�
Return Value:
	none
Description:	����2Dͼ��ߴ�
*****************************************************************/
void AnalyzeProcess::Set2DImageSize(int t_nWidth, int t_nHeight)
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
Description:	����2Dͼ��ߴ�
*****************************************************************/
void AnalyzeProcess::Set3DSize(float t_fMaxX, float t_fMaxY, float t_fMaxZ)
{
	if (t_fMaxX <= 50 || t_fMaxX > 1000
		|| t_fMaxY <= 50 || t_fMaxY > 1000
		|| t_fMaxZ <= 20 || t_fMaxZ > 1000 )
	{
		return;
	}

	m_fMaxX = t_fMaxX;
	m_fMaxY = t_fMaxY;
	m_fMaxZ = t_fMaxZ;
}//Set3DSize


/*****************************************************************
Name:			GetSurfaceDataPtr
Inputs:
	
Return Value:
	SurfaceDataPtr - ���ص���ģ��ָ��
Description:	���ر���ģ��
*****************************************************************/
SurfaceDataPtr AnalyzeProcess::GetSurfaceDataPtr()
{
	return m_SurgicalPlanPtr->GetSurfaceDataPtr();
}//GetSurfaceDataPtr


/*****************************************************************
Name:			GetCenterPt
Inputs:
	double &x - x����
	double &y - y����
	double &z - z����
Return Value:
	none
Description:	�õ���������
*****************************************************************/
void AnalyzeProcess::GetCenterPt(double &x, double &y, double &z)
{
	m_pMaskSampler->GetCenterPt(x, y, z);
}//GetCenterPt


/*****************************************************************
Name:			GetParameters
Inputs:
	�����
Return Value:
	FrameDataPtr - �����ĵ�ǰ֡����
Description:	������ǰһ֡ͼ��
*****************************************************************/
/*
void AnalyzeProcess::GetParameters(double &t_centerX, double &t_centerY, double &t_centerZ,
	double &t_MX, double &t_MY, double &t_MZ,
	double &t_UX, double &t_UY, double &t_UZ)
{
	return m_SurgicalPlanPtr->GetParameters(t_centerX, t_centerY, t_centerZ,
		t_MX, t_MY, t_MZ, t_UX, t_UY, t_UZ);
}//GetCurrentFrameData
*/


/*****************************************************************
Name:			InportData(�����)
Inputs:
	none
Return Value:
	none
Description:	���������ƻ�����
*****************************************************************/
int AnalyzeProcess::InportData(CString t_strFilePathName)
{
	//�ж��ļ��Ƿ����
	if (!PathFileExists(t_strFilePathName))
	{
		return ER_FileOpenFailed;
	}

	//���������ƻ�
	if (m_SurgicalPlanPtr->InPortAsFileSet(t_strFilePathName) != LIST_NO_ERROR)
	{
		return ER_InportSurgicalPlanFailed;
	}

	//����ImageSamplerPtr
	m_pMaskSampler->SetDataPackage(m_SurgicalPlanPtr);

	return LIST_NO_ERROR;
}//InportData


/*****************************************************************
Name:			StartAnalyze
Inputs:
	none
Return Value:
	none
Description:	�����������������������߳�
*****************************************************************/
void AnalyzeProcess::StartAnalyze()
{
	m_AnalyzeThread = CreateThread(nullptr, 0, AnalyzeThread, (void *)this, 0, nullptr);	//���������߳�
}//StartAnalyze


/*****************************************************************
Name:			StartAnalyze(�����)
Inputs:
	none
Return Value:
	none
Description:	���������������޸ı�־
*****************************************************************/
void AnalyzeProcess::StopAnalyze()
{
	m_USBCapturerPtr->StopGrab();			//ֹͣ�ɼ�ͼ������
	m_NDIOperatorPtr->StopTracking();		//ֹͣ�ɼ�NDI����
}


/*****************************************************************
Name:			Analyze(�����)
Inputs:
	none
Return Value:
	none
Description:	ʵ�ʷ�������
*****************************************************************/
void AnalyzeProcess::Analyze()
{
	while (1)
	{
		CSingleLock singlelock(&m_ProcessDataMutex);
		singlelock.Lock();
		if (!m_NextFrameDataPtr->m_USBImage.empty() && !m_NextFrameDataPtr->m_NeedlePos.Empty())
		{
			m_CurrentFrameDataPtr = m_NextFrameDataPtr;
			m_NextFrameDataPtr.reset(new FrameData);
			singlelock.Unlock();
		}
		else
		{
			Sleep(15);
			singlelock.Unlock();
			continue;
		}

		ProcessSingleFrame(m_CurrentFrameDataPtr);		//����֡����
	}


}//Analyze


/*****************************************************************
Name:			ProcessSingleFrame
Inputs:
	none
Return Value:
	none
Description:	����֡����
*****************************************************************/
void AnalyzeProcess::ProcessSingleFrame(FrameDataPtr t_FrameDataPtr)
{
	//���ô�һ����������
	if (m_nAnalyzeState == Registration)
	{
		ProcessSingleFrameA(t_FrameDataPtr);
	}
	else
	{
		ProcessSingleFrameB(t_FrameDataPtr);
	}

	//������������͸�dlg
	if (m_UpdateFrameFun != nullptr)
	{
		m_UpdateFrameFun(t_FrameDataPtr);
	}
}//ProcessSingleFrame


/*****************************************************************
Name:			ProcessSingleFrameA����ʵ�֣�
Inputs:
	none
Return Value:
	none
Description:	����֡���ݣ�δ����״̬��
*****************************************************************/
void AnalyzeProcess::ProcessSingleFrameA(FrameDataPtr t_FrameDataPtr)
{
	//�õ���һ֡2Dͼ��
	if (t_FrameDataPtr->CreatMaskData(m_nShowImageX, m_nShowImageY) != LIST_NO_ERROR)
	{
		return;
	}

	m_pMaskSampler->GetSampleMaskPlan(t_FrameDataPtr->m_pMaskData, m_nShowImageX, m_nShowImageY,true);
}//ProcessSingleFrameA


/*****************************************************************
Name:			ProcessSingleFrameB
Inputs:
	none
Return Value:
	none
Description:	����֡���ݣ�����״̬��
*****************************************************************/
void AnalyzeProcess::ProcessSingleFrameB(FrameDataPtr t_FrameDataPtr)
{
	//�õ���һ֡2Dͼ��
	//�õ���һ֡2Dͼ��
	if (t_FrameDataPtr->CreatMaskData(m_nShowImageX, m_nShowImageY) != LIST_NO_ERROR)
	{
		return;
	}

	m_pMaskSampler->GetSampleMaskPlan(t_FrameDataPtr->m_pMaskData, m_nShowImageX, m_nShowImageY,true);
}//ProcessSingleFrameB


/*****************************************************************
Name:			CalibratePlanUSBCoordinate(�����)
Inputs:
	none
Return Value:
	none
Description:	���㳬��̽ͷ�������ƻ�����֮���ת������
*****************************************************************/
void AnalyzeProcess::CalibratePlanUSBCoordinate()
{

}//CalibratePlanUSBCoordinate


/*****************************************************************
Name:			UpdateNDIData(�����)
Inputs:
	none
Return Value:
	none
Description:	����NDI���ݣ��ص���������NDIģ�����
*****************************************************************/
void AnalyzeProcess::UpdateNDIData(std::vector<Attitude> t_Attitude)
{
	//����Ĭ��ֻ�����һ��
	if (t_Attitude.size() < 1)
	{
		return;
	}

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�Ȱ�Attitude��������

	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	m_NextFrameDataPtr->m_USBPos = t_Attitude[0];
	singlelock.Unlock();
}//UpdateNDIData


/*****************************************************************
Name:			UpdateUSBData(Mat t_USBImg)
Inputs:
	cv::Mat t_USBImgA - ȫͼ���ϰ���ͼ��
	cv::Mat t_USBImgB - �°���ͼ�񣬿���Ϊ��
	int t_nScale - ���ű���
Return Value:
	none
Description:	//����USB���ݣ��ص���������NDIģ�����
*****************************************************************/
void AnalyzeProcess::UpdateUSBData(cv::Mat t_USBImgA, cv::Mat t_USBImgB, int t_nScale)
{
	CSingleLock singlelock(&m_ProcessDataMutex);
	singlelock.Lock();
	m_NextFrameDataPtr->m_USBImage = t_USBImgA;
	m_NextFrameDataPtr->m_nScale = t_nScale;
	singlelock.Unlock();
}//UpdateUSBData


/*
void CALLBACK UpdateAttitudeCallBack(std::vector<NDIOPERATOR::Attitude> attitude)
{
	//HWND hWnd = AfxGetMainWnd()->m_hWnd;
	float x, y, z, a, b, c;

	//����Ĭ��ֻ�����һ��
	if (attitude.size() < 1)
	{
		return;
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�Ȱ�Attitude��������

	CSingleLock singlelock(&pPuncturedlg->m_PosMutex);
	singlelock.Lock();
	pPuncturedlg->m_FrameDataPtr->m_USBPos = attitude[0];
	singlelock.Unlock();
	//double t_mat[16];
	//NDIOPERATOR::NDIOperator::ConstructMatRtoTusingAttitude(attitude[0], t_mat);

	//��ñ任����
	//CString str(""), temp;
	//for (int i = 0; i < 16; i++)
	//{
	//	temp.Format(_T("%.6f"), mat[i]);
	//	str += temp;
	//	str += CString(" ");
	//	if (i % 4 == 3)
	//		str += CString("\r\n");
	//}
	//GetDlgItem(IDC_EDIT_MatRtoT_1)->SetWindowText(str);
}



//void CALLBACK ProcessFrameCallBack(cv::Mat t_USBImgA, cv::Mat t_USBImgB, int t_nScale)
//{
//	//��ʼ����ͼƬ����ȡһ����ͼƬ����,��ת����bitmap
//
//	CSingleLock singlelock(&pPuncturedlg->m_ShowMutex);
//	singlelock.Lock();
//
//	//pPuncturedlg->m_FrameDataPtr->m_USBImage = t_USBImgA;
//	//pPuncturedlg->m_FrameDataPtr->m_nScale = t_nScale;
//	//pPuncturedlg->m_b2Dshow = true;
//
//	singlelock.Unlock();
//}*/