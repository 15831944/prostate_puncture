//*****************************************************************
// �ļ��� :						ImageProcessor.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				����2Dͼ�񹤾���
// �������� :					2021.11.12
// �޸����� :					2021.11.12
// ���� :						SunYizhe
// �޸��� :						
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/
#pragma once

/*****************************************************************
Library Files Included
*****************************************************************/
#include "opencv2/opencv.hpp"

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
����2Dͼ�� ������
	i.	��2D�����USͼ�� �ָ�ǰ��������
*****************************************************************/

namespace ANALYSEPROCESS
{
	class ImageProcessor;
	typedef std::shared_ptr<ImageProcessor> ImageProcessorPtr;

	class ImageProcessor
	{
	public:
		ImageProcessor();
		~ImageProcessor();

		static cv::Mat SegmentByThreshold(cv::Mat &srcImg);  //���ں����ͼ�񣬷ָ�ǰ����(0/255 ��ֵͼ)
	private:

	};
}



