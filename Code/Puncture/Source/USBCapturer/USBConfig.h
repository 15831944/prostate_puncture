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
		//B���������������
		//CvRect m_Axis;
		//�Ҳ����������� �����жϷֱ���
		CvRect m_RightAxisRect;
		//������������� �����жϵ�ƽ��/˫ƽ��
		CvRect m_UpAxisRect;
		CvRect m_DownAxisRect;

		//����Ļ�µĲ���
		CvRect m_FullRect;

		//˫��Ļ�µĲ���
		CvRect m_UpRect;
		CvRect m_DownRect;
	};
}
