//*****************************************************************
// �ļ��� :						AnalyseProcess.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				ʵ�ʷ�������ģ��
// �������� :					2020.11.01
// �޸����� :					2020.05.07
// ���� :						Fiki
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/
#pragma once

/*****************************************************************
Library Files Included
*****************************************************************/
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include "AnalyseProcess/AnalyseConfig.h"
#include "AnalyseProcess/PositionManager.h"
#include "AnalyseProcess/ImageSampler.h"
#include "AnalyseProcess/ImageProcessor.h"
#include "AnalyseProcess/FrameData.h"
#include "AnalyseProcess/RigidTransform.h"
#include "NDIOperator/NDIOperator.h"
#include "SurgicalPlan/SurgicalPlan.h"
#include "USBCapturer/USBCapturer.h"

using namespace NDIOPERATOR;
using namespace USBCAPTURER;
using namespace SURGICALPLAN;
using namespace fsutility;
using namespace std;

/*****************************************************************
Defines
*****************************************************************/
#ifdef ANALYSEPROCESS_EXPORT
#define ANALYSEPROCESS_API _declspec(dllexport)
#else
#define ANALYSEPROCESS_API _declspec(dllimport)
#endif

/*****************************************************************
						Routine Definitions
�����������AnalyseProcess��
	i.	�ɴ�ini�ļ��ж�ȡMRI��Ϣ������baseλ�ò���
	ii.	λ��״ָ̬��PositionManager�����㳬��/MRIģ�����֮���λ�ñ任
	iii.����MRIģ�������λ�ò�������ȡ2ά����
	iv.	�ص�������ʽ���ظ�������
*****************************************************************/


namespace ANALYSEPROCESS
{
	//��������ָ����
	class AnalyseProcess;
	typedef std::shared_ptr<AnalyseProcess> AnalyseProcessPtr;

	//�������ֵ�ǰ����״̬����ͬ����״̬�������ݲ���ͬ
	enum AnalyzeState
	{
		INIT,					//��ʼ�� δ��׼
								//��ʱ��ʹ��mriģ�����baseλ�� ����ȡmask����
		PENDING,				//��ʼ����ɣ�����MRIģ���������ʵUS̽ͷλ�˹�����
		REGISTERING1,			//��׼��������� ��δ��ȡ���µ�US̽ͷ��̬
		REGISTERING2,			//��׼��������� �ѻ�ȡ���µ�US̽ͷ��̬
		PUNCTURE,				//����	(�Ѿ���׼���)
	};
	class ANALYSEPROCESS_API AnalyseProcess
	{
	public:
		AnalyseProcess();
		~AnalyseProcess();

		int InitAnalyseProcess(CString t_strFilePathName);		//��ʼ������ģ��
		int InitAnalyseProcess(std::string t_strFilePathName);	//��ʼ������ģ��

		void SetNDIDevicePtr(NDIOperatorPtr t_NDIOperatorPtr);			//����NDI�豸����ָ��
		void SetUSBCapturerPtr(USBCapturerPtr t_USBCapturerPtr);		//����B��ͼ��ɼ��豸ָ��
		void SetSurgicalPlanPtr(SurgicalPlanPtr t_SurgicalPlanPtr);		//���������ƻ�����ָ��

		void Set2DImageSize(int t_nWidth, int t_nHeight);				//����2Dͼ���С
		void Set3DSize(float t_fMaxX, float t_fMaxY, float t_fMaxZ);	//����3D�ռ�ߴ�

		void MoveMRIRight(float distance);  //MRI mask����(rightDir)�ƶ�
		void MoveMRIUp(float distance);  //MRI mask����(upDir)�ƶ�
		void MoveMRIForward(float distance);  //MRI mask��ǰ(moveDir)�ƶ�

		int	 StartAnalyse();			//������������
		int	 StopAnalyse();				//������������

		typedef std::function <void(FrameDataPtr m_FrameDataPtr) > Fun_UpdateFrameEvent;	//����һ֡�����󣬽����ݴ���dlg
		void BindUpdateFrameEvent(Fun_UpdateFrameEvent eventFun);	//ע��ص�����

		void Analyse();				//ʵ�ʷ�������

		int Register();				//��׼����,��ҽ���ֶ�����
		int ResetRegister();		//������׼��������ҽ���ֶ�����
	private:
		void UpdateNDIData(fsutility::Attitude attitude);  //����NDI��λ���ݣ��ص���������NDIģ�����
		void UpdateUSBData(cv::Mat t_USBImgT, cv::Mat t_USBImgS, double dPixelSizeT, double dPixelSizeS);	//����B�����ݣ��ص���������B��ģ�����

		vector<Coordinate> GetContourPts(FrameDataPtr t_FrameDataPtr);  //�ӵ�֡�����У���ȡ��������������
		void ProcessSingleFrame(FrameDataPtr t_FrameDataPtr);	//����֡����
		void ProcessSingleFrameA(FrameDataPtr t_FrameDataPtr);	//����֡���ݣ�δ����״̬��
		void ProcessSingleFrameB(FrameDataPtr t_FrameDataPtr);	//����֡���ݣ�����״̬��
		Mat Mask2Edge(Mat src);								//��maskȡ����
	private:
		CMutex			m_ProcessDataMutex;		//��ʾ�������
		FrameDataPtr	m_CurrentFrameDataPtr;	//���ڴ�ŵ�ǰ�����е�֡
		FrameDataPtr	m_NextFrameDataPtr;		//���ڴ����һ֡�������֡

		//HANDLE			m_hAnalyzeThread;	//ʵ�ʴ����߳�
		std::thread			m_tAnalyseThread;	//�����߳�
		atomic<bool>	m_bAnalysing;			//���ڷ�����־λ

		AnalyzeState	m_nAnalyseState;		//��ǰ�����׶�

		Fun_UpdateFrameEvent m_UpdateFrameFun;	//����dlg���ݻص�����ָ��

		NDIOperatorPtr		m_NDIOperatorPtr;			//NDI�豸����ָ��
		USBCapturerPtr		m_USBCapturerPtr;			//B��ͼ��ɼ��豸ָ��
		SurgicalPlanPtr		m_SurgicalPlanPtr;			//�����ƻ������ݲ���ָ��
		PositionManagerPtr	m_PositionManagerPtr;		//λ�ù���ָ�� ���е���̬����������������й���ͼ���
		RigidTransformPtr	m_RigidTransformPtr;		//����任�������ָ��
		ImageSamplerPtr		m_ImageSamplerPtr;			//�и��άƽ�����ָ��
		ImageProcessorPtr	m_ImageProcessorPtr;		//����2Dͼ�񹤾���

		vector<Coordinate> m_USPts;				//����USͼ�� ��ȡ��һ�����
		BYTE			*m_pProstateMask;		//�ݴ�prostate mask
		BYTE			*m_pLesionMask;			//�ݴ�lesion mask
		BYTE			*m_pRectumMask;			//�ݴ�rectum mask

		int				m_nShowImageX;			//2Dͼ�����ؿ��	��PunctureDlg.cpp�У����ݳ���ͼ���С���趨
		int				m_nShowImageY;			//2Dͼ�����ظ߶�
		float			m_fMaxX;				//3D�ռ��С
		float			m_fMaxY;
		float			m_fMaxZ;
		
		//fsutility::Attitude m_simulatedAttitude;  //����̽ͷ����̬����ֵ�������ֶ���׼ǰ������������
	};
}