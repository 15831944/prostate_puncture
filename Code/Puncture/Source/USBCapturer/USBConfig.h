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
		cv::Rect m_DualLeftBarRect;  //˫ƽ�����೤������
		cv::Rect m_CharSensorTypeRect;  //ָʾ�����(T)/ʸ״��(S)������
		cv::Rect m_OnePlaneRightAxisRect; //��ƽ�� �Ҳ�����������
		cv::Rect m_DualPlaneUpRightAxisRect;  //˫ƽ��ģʽ �Ϸ�(�����)�Ҳ�����������
		cv::Rect m_DualPlaneDownRightAxisRect;  //˫ƽ��ģʽ �·�(ʸ״��)�Ҳ�����������
		cv::Rect m_OnePlaneROI;  //��ƽ��ģʽ�� ͼ������
		cv::Rect m_DualUpROI;  //˫ƽ��ģʽ�� �Ϸ�(�����)ͼ������
		cv::Rect m_DualDownROI;  //˫ƽ��ģʽ�� �·�(ʸ״��)ͼ������
	};
}
