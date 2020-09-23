//*****************************************************************
// �ļ��� :						ImageSampler.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				��3D���ݵ�2ά����
// �������� :					
// �޸����� :					2020.05.08
// ���� :						Tanghui
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					
//*****************************************************************/
#pragma once

/*****************************************************************
Library Files Included
*****************************************************************/
//#include "SurgicalPlan.h"
#include "ComUtility/Coordinate.h"
#include "SurgicalPlan/SurgicalPlan.h"

using namespace SURGICALPLAN;
using namespace fsutility;

/*****************************************************************
Defines
*****************************************************************/
#ifdef ANALYSEPROCESS_EXPORT
#define ANALYSEPROCESS_API __declspec(dllexport)
#else
#define ANALYSEPROCESS_API __declspec(dllimport)
#endif

/*****************************************************************
						Routine Definitions
�����ȡƽ��ImageSampler��
	i.	����MRIģ�������λ�ò������ü�MRIԭʼ���ݵ�2ά����(��״��/ʸ״��)
	ii.	����MRIģ�������λ�ò������ü�ǰ����mask/����mask��2ά����(��״��/ʸ״��)
*****************************************************************/

namespace ANALYSEPROCESS
{
	//���յ�ͷ����MRI/Mask�����ݽ���ģ��������õ���������Ͷ�Ӧ��������
//********������������ΪWLD����ϵ�£������˷ֱ��ʣ���λΪmm���ڲ�ת��ΪIJK����ϵ***
	class ImageSampler;
	typedef std::shared_ptr<ImageSampler> ImageSamplerPtr;
	class ANALYSEPROCESS_API ImageSampler
	{
	public:
		ImageSampler();
		virtual ~ImageSampler();

		int SetDataPackage(SurgicalPlanPtr pPackage);
		void SetPosition(Coordinate ScanCenter, Coordinate RightDir, Coordinate UpDir, Coordinate MoveDir);		//���õ�ǰ��Ƭλ�ò��� WLD����
		void SetUSPixelSize(double dPixelSize);		//����B��ͼ�����ش�С(���ض�Ӧ������ߴ�)
		void SetImageSize(int cx, int cy);			//����B��ͼ��ֱ���(������)
		void SetMRIPixelSize(double dPixelSizeX, double dPixelSizeY, double dPixelSizeZ);	//����MRI�������ش�С
		//void GetSampleOriginPlan(); //TODO
		int GetSampleMaskPlan(MaskDataType *pImage, int nScanType, int nMaskType);	//��ȡMASK���棬nScanType��Ӧɨ�����ͣ�0-��״�棬1-ʸ״�档nMaskType��Ӧmask���ͣ�0-ǰ���٣�1-����
		void GetPlaneCorners(Coordinate& leftTop, Coordinate& rightTop, Coordinate& leftBottom, Coordinate& rightBottom);	//��ȡ����Ƭ�ĸ��ǵ� ��MRIģ�����3D�ռ������ (wld)

		Coordinate IJKToWLD(Coordinate pt);
		Coordinate WLDToIJK(Coordinate pt);
		
	private:
		void CalculateCoronalMaskPlan();	//����B����״��ͼ���ĸ��ǵ��WLD����
		void CalculateSagittalMaskPlan();	//����B��ʸ״��ͼ���ĸ��ǵ��WLD����
		void GetMRIPlaneData(MRIDataType tMRIDataPtr, MaskDataType *pImage);			//����ԭʼMRI��Ƭ����
		void GetMaskPlanData(MaskDataPtr tMaskDataPtr, MaskDataType *pImage);		//����MASK��Ƭ����

	private:
		SurgicalPlanPtr m_pDataPackage; //ָ�����ݵ�ָ��

		//����Ƭ��MRIģ�����3D�ռ��е���̬����(wld)
		Coordinate m_ScanCenter;		//B��̽ͷλ������
		Coordinate m_RightDir;			//2άB��ͼ���x���������Ӧ����ά�ռ�����
		Coordinate m_UpDir;				//2άB��ͼ���y�Ḻ�����Ӧ����ά�ռ�����
		Coordinate m_MoveDir;			//B��̽ͷǰ���ķ��򣬼���ֱ����ڵ�������λ�õ�����

		//����Ƭ�ĸ��ǵ� ��MRIģ�����3D�ռ������ (wld)
		Coordinate m_LeftTop;
		Coordinate m_RightTop;
		Coordinate m_LeftBottom;
		Coordinate m_RightBottom;

		//����Ƭ�ĸ��ǵ� ��MRIģ�����3D�ռ������ (ijk)
		Coordinate m_LeftTopIJK;
		Coordinate m_RightTopIJK;
		Coordinate m_LeftBottomIJK;
		Coordinate m_RightBottomIJK;

		double m_dUSPixelSize;			//B��ͼ���У�1�����ض�Ӧ��ʵ������ߴ�(mm)
		ImageSize m_nResolution;		//B��ͼ��ߴ�(������)

		//MRI������ x,y,z�����ϵ����ش�С(mm)
		double m_dMRIPixelSizeX;
		double m_dMRIPixelSizeY;
		double m_dMRIPixelSizeZ;

	};

	typedef std::shared_ptr<ImageSampler> ImageSamplerPtr;

}
