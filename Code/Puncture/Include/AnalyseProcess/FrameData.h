//*****************************************************************
// �ļ��� :						FrameData.h
// �汾	 :						2.0
// Ŀ�ļ���Ҫ���� :				��֡��Ϣ��
// �������� :					2019.11.1
// �޸����� :					2010.05.07
// ���� :						Fiki
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/
#pragma once


/*****************************************************************
Project Files Included
*****************************************************************/
#include <memory>
#include "Opencv2/opencv.hpp"
#include "NDIOperator/Attitude.h"
#include "ComUtility/Coordinate.h"
#include "ComUtility/Attitude.h"

//using namespace cv;
using namespace NDIOPERATOR;
using namespace fsutility;

/*****************************************************************
Defines
��֡����FrameData��
	i.	��¼ĳһʱ�̵Ľ��������
	ii.	�����ɳ����豸��ȡ�ĳ���ͼ��
	iii.������MRIģ����� ��ȡ����2άMRI ǰ����/�������
*****************************************************************/
#ifdef ANALYSEPROCESS_EXPORT
#define ANALYSEPROCESS_API _declspec(dllexport)
#else
#define ANALYSEPROCESS_API _declspec(dllimport)
#endif


/*****************************************************************
						Routine Definitions
*****************************************************************/

namespace ANALYSEPROCESS
{
	//��֡��Ϣ��
	class FrameData;
	typedef std::shared_ptr<FrameData> FrameDataPtr;

	class ANALYSEPROCESS_API FrameData
	{
	public:
		FrameData();
		virtual ~FrameData();

		//int	CreatMaskData(int t_nShowWidth, int t_nShowHeight);		//����2D��ģͼ��ռ�
		void SetPosition(fsutility::Attitude Attitude);	//���õ�ǰ��ƬMRIģ�������̬����(wld)
		//void SetPosition(Coordinate ScanCenter, Coordinate RightDir, Coordinate UpDir, Coordinate MoveDir);	//���õ�ǰ��ƬMRIģ�������̬����(wld)

	public:
		//Transverse���������
		cv::Mat		m_USImgT;  //����ͼ��
		cv::Mat		m_prostateMaskT;  //mask���� CV_8UC1
		cv::Mat		m_lesionMaskT;
		cv::Mat		m_rectumMaskT;
		double		m_dPixelSizeT;  //��������ߴ�(mm)

		//Sagittalʸ״������
		cv::Mat		m_USImgS;  //����ͼ��
		cv::Mat		m_prostateMaskS;  //mask���� CV_8UC1
		cv::Mat		m_lesionMaskS;
		cv::Mat		m_rectumMaskS;
		double		m_dPixelSizeS;  //��������ߴ�(mm)

		//int			m_nScale;		//��ǰb��ͼ��ı�����
		//double m_dImageRes;		//���ű�������1�����ض�Ӧ������ߴ�(mm)

		//Attitude	m_USBPos;		//����̽ͷ�Ŀռ�λ��
		//Attitude	m_NeedlePos;	//̽��ռ�λ�ã�Ԥ�������������ƣ�

		//BYTE *m_pProstateMask;		//ǰ����2D��ģͼ��
		//BYTE *m_pLesionMask;		//����2D��ģͼ��
		//BYTE *m_pFusionMask;		//�ܵ�2Dmask����ͼ��1-ǰ���٣�2-���3-ֱ��

		////MRIģ����� �������̬���� (wld)
		//Coordinate m_ScanCenter;
		//Coordinate m_RightDir;
		//Coordinate m_UpDir;
		//Coordinate m_MoveDir;

		//MRIģ����� �������̬���� (wld)
		fsutility::Attitude m_Attitude;

		//MRIģ����� ������ĸ��ǵ�(wld)
		Coordinate m_LeftTop;
		Coordinate m_RightTop;
		Coordinate m_LeftBottom;
		Coordinate m_RightBottom;
	};
}

