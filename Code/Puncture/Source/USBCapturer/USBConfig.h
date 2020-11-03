//*****************************************************************
// �ļ��� :						USBConfig.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				����ͼ��ɼ��豸����
// �������� :					2019.11.1
// �޸����� :					
// ���� :						Fiki
// �޸��� :						
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/
#pragma once


/*****************************************************************
Project Files Included
*****************************************************************/
#include <memory>
#include "ComUtility/singleton.h"
#include "Opencv2/opencv.hpp"

/*****************************************************************
Defines
*****************************************************************/
#ifdef USBCAPTURER_EXPORT
#define USBCAPTURER_API __declspec(dllexport)
#else
#define USBCAPTURER_API __declspec(dllimport)
#endif


/*****************************************************************
						Routine Definitions
*****************************************************************/
namespace USBCAPTURER
{
	//��������ɼ�ͼ��ṹ��
	class USBCAPTURER_API USBConfig : public fsutility::Singleton<USBConfig>
	{
		friend class fsutility::Singleton<USBConfig>;

	private:
		USBConfig();
		virtual ~USBConfig();

	public:
		int LoadConfigFile(CString t_strFilePathName);

	public:
		CvRect m_DualLeftBarRect;  //˫ƽ�����೤������
		CvRect m_CharSensorTypeRect;  //ָʾ�����(T)/ʸ״��(S)������
		CvRect m_OnePlaneRightAxisRect; //��ƽ�� �Ҳ�����������
		CvRect m_DualPlaneUpRightAxisRect;  //˫ƽ��ģʽ �Ϸ�(�����)�Ҳ�����������
		CvRect m_DualPlaneDownRightAxisRect;  //˫ƽ��ģʽ �·�(ʸ״��)�Ҳ�����������
		CvRect m_OnePlaneROI;  //��ƽ��ģʽ�� ͼ������
		CvRect m_DualUpROI;  //˫ƽ��ģʽ�� �Ϸ�(�����)ͼ������
		CvRect m_DualDownROI;  //˫ƽ��ģʽ�� �·�(ʸ״��)ͼ������
	};
}
