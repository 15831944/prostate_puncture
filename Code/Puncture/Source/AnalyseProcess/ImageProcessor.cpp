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
#include "pch.h"
#include <vector>
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "ImageProcessor.h"


using namespace ANALYSEPROCESS;
using namespace cv;
using namespace std;

const int SPACE_X = 50;  //ͼ�����������ı߽�����(��λ pixel) ����ΪROI
const int SPACE_Y = 50;  //ͼ�����������ı߽�����(��λ pixel) ����ΪROI

ImageProcessor::ImageProcessor()
{
}


ImageProcessor::~ImageProcessor()
{
}

cv::Mat ImageProcessor::SegmentByThreshold(cv::Mat & srcImg)
{
	int maxIdx = 0, max = 0;
	cv::Mat gryImg;
	cv::Mat binaryImg;
	cv::Mat dstImg;
	vector<cv::Point> targetContour;  //Ŀ�� ����������
	vector<vector<cv::Point>> contours;  //��������

	//ת�Ҷ�
	cv::cvtColor(srcImg, gryImg, CV_BGR2GRAY);
	//ȡROI
	int width = gryImg.cols;
	int height = gryImg.rows;
	cv::Rect roi(SPACE_X, SPACE_Y, width - SPACE_X * 2, height - SPACE_Y * 2);
	cv::Mat roiImg = gryImg(roi).clone();
	gryImg.setTo(0);
	roiImg.copyTo(gryImg(roi));
	//ȡ��ֵ
	cv::threshold(gryImg, binaryImg, -1, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);  //OTSU�����Զ�ȡ��ֵ��ǰ����Ϊ255
	//������
	Mat element;  //����������
	element = cv::getStructuringElement(MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(binaryImg, binaryImg, MORPH_CLOSE, element);
	//ȡ����
	cv::findContours(binaryImg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);  //������������ ��������ι�ϵ���洢����������
	//���������
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > max) {
			maxIdx = i;
			max = contours[i].size();
		}
	}
	//�������������
	targetContour = contours[maxIdx];
	dstImg = gryImg.clone();
	dstImg.setTo(0);
	cv::drawContours(dstImg, vector<vector<cv::Point>>{targetContour}, -1, Scalar(255), CV_FILLED);

	return dstImg;
}
