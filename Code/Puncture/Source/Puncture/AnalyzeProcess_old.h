//*****************************************************************
// �ļ��� :						AnalyzeProcess.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				ʵ�ʷ�������ģ��
// �������� :					2019.11.1
// �޸����� :					
// ���� :						Fiki
// �޸��� :						
// ��ϵ��ʽ :					
//*****************************************************************/
#pragma once

/*****************************************************************
Library Files Included
*****************************************************************/
#include <vector>
#include "NDIOperator/NDIOperator.h"
#include "SurgicalPlan/SurgicalPlan.h"
#include "USBCapturer/USBCapturer.h"
#include "AnalyseProcess/FrameData.h"
#include "SurgicalPlan/ImageSampler.h"


using namespace NDIOPERATOR;
using namespace SURGICALPLAN;
using namespace USBCAPTURER;


/*****************************************************************
Defines
*****************************************************************/


/*****************************************************************
						Routine Definitions
*****************************************************************/
//ʵ�ʴ���ģ��
class AnalyzeProcess;
typedef std::shared_ptr<AnalyzeProcess> AnalyzeProcessPtr;

//�������ֵ�ǰ����״̬����ͬ����״̬�������ݲ���ͬ
enum AnalyzeState
{
	Init,					//��ʼ��
	Registration,			//��׼
	Puncture,				//����
};

class AnalyzeProcess
{
public:
	AnalyzeProcess();
	virtual ~AnalyzeProcess();

	void StartAnalyze();			//������������
	void StopAnalyze();				//������������
	
	typedef std::function <void(FrameDataPtr m_FrameDataPtr) > Fun_UpdateFrameEvent;	//����һ֡�����󣬽����ݴ���dlg
	void BindUpdateFrameEvent(Fun_UpdateFrameEvent eventFun);	//ע��ص�����

	void SetNDIDevicePtr(NDIOperatorPtr t_NDIOperatorPtr);			//����NDI�豸����ָ��
	void SetUSBCapturerPtr(USBCapturerPtr m_USBCapturerPtr);		//����B��ͼ��ɼ��豸ָ��

	void Set2DImageSize(int t_nWidth, int t_nHeight);				//����2Dͼ���С
	void Set3DSize(float t_fMaxX, float t_fMaxY, float t_fMaxZ);	//����3D�ռ�ߴ�

	SurfaceDataPtr GetSurfaceDataPtr();								//���ر���ģ��
	void GetCenterPt(double &x, double &y, double &z);				//�õ���������
	//void GetParameters(double &t_centerX, double &t_centerY, double &t_centerZ,
	//	double &t_MX, double &t_MY, double &t_MZ,
	//	double &t_UX, double &t_UY, double &t_UZ);

	void CalibratePlanUSBCoordinate();								//���㳬��̽ͷ�������ƻ�����֮���ת������

	int InportData(CString t_strFilePathName);				//���������ƻ�����

	void Analyze();				//ʵ�ʷ�������

private:
	void UpdateNDIData(std::vector<Attitude> t_Attitude);						//����NDI���ݣ��ص���������NDIģ�����
	void UpdateUSBData(cv::Mat t_USBImgA, cv::Mat t_USBImgB, int t_nScale);		//����USB���ݣ��ص���������NDIģ�����

	void ProcessSingleFrame(FrameDataPtr t_FrameDataPtr);	//����֡����
	void ProcessSingleFrameA(FrameDataPtr t_FrameDataPtr);	//����֡���ݣ�δ����״̬��
	void ProcessSingleFrameB(FrameDataPtr t_FrameDataPtr);	//����֡���ݣ�����״̬��

	int InportTransMatrixN_U(CString m_strFileName);		//����NDI����B������ת������

public:

private:
	CMutex			m_ProcessDataMutex;		//��ʾ�������
	FrameDataPtr	m_CurrentFrameDataPtr;	//���ڴ�ŵ�ǰ�����е�֡
	FrameDataPtr	m_NextFrameDataPtr;		//���ڴ����һ֡�������֡

	HANDLE			m_AnalyzeThread;		//ʵ�ʴ����߳�

	AnalyzeState	m_nAnalyzeState;		//��ǰ�����׶�
	bool			m_bStopAnalyze;			//ֹͣ����������ֹͣ�������

	Fun_UpdateFrameEvent m_UpdateFrameFun;	//����dlg���ݻص�����ָ��

	NDIOperatorPtr	m_NDIOperatorPtr;		//NDI�豸����ָ��
	USBCapturerPtr	m_USBCapturerPtr;		//B��ͼ��ɼ��豸ָ��

	SurgicalPlanPtr	m_SurgicalPlanPtr;		//�����ƻ������ݲ���ָ��
	ImageSamplerPtr	m_pMaskSampler;			//����ͼ��

	cv::Mat			m_TransMatrixPlanUSB;	//����̽ͷ�������ƻ�����֮���ת������

	int				m_nShowImageX;			//2Dͼ�����ؿ��
	int				m_nShowImageY;			//2Dͼ�����ظ߶�
		
	float			m_fMaxX;				//3D�ռ��С
	float			m_fMaxY;
	float			m_fMaxZ;	

	vector<Mat>		m_vTransrMatrixN_U;		//NID_��������ת������ΪӦ�Զ��鲻ͬ�ֱ��ʣ�������vector�洢
};