//*****************************************************************
// �ļ��� :						ImageSampler.cpp
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				��3D���ݵ�2ά����
// �������� :					
// �޸����� :					2020.05.08
// ���� :						Tanghui
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					
//*****************************************************************/

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "pch.h"
#include "AnalyseProcess/ImageSampler.h"
#include "ErrorManager/ErrorCodeDefine.h"
#include "SurgicalPlan/CPubFunc.h"
//#include "ImageSampler.h"
//#include "CPubFunc.h"

using namespace ANALYSEPROCESS;

/*****************************************************************
Name:			ImageSampler
Inputs:
	none
Return Value:
	none
Description:	ImageSampler���캯��
*****************************************************************/
ANALYSEPROCESS::ImageSampler::ImageSampler()
{
	m_pDataPackage = NULL;
	m_dUSPixelSize = 0;
	m_nResolution.cx = 0;
	m_nResolution.cy = 0;
	m_dMRIPixelSizeX = 0;
	m_dMRIPixelSizeY = 0;
	m_dMRIPixelSizeZ = 0;
}


/*****************************************************************
Name:			~ImageSampler
Inputs:
	none
Return Value:
	none
Description:	ImageSampler��������
*****************************************************************/
ANALYSEPROCESS::ImageSampler::~ImageSampler()
{

}


/*****************************************************************
Name:			SetDataPackage
Inputs:
	SurgicalPlanPtr pPackage - �����ƻ�ָ��
Return Value:
	none
Description:	���������ƻ�ָ��
*****************************************************************/
int ANALYSEPROCESS::ImageSampler::SetDataPackage(SurgicalPlanPtr pPackage)
{
	if (pPackage != nullptr)
	{
		m_pDataPackage = pPackage;
		return LIST_NO_ERROR;
	}
	else
		return ER_NoSurgicalPlan;
}


/*****************************************************************
Name:			SetPosition
Inputs:
	Coordinate xxx - ָʾ��ǰ��Ƭ����ά�ռ���̬������һ���������(WLD)
Return Value:
	none
Description:	���õ�ǰ��Ƭ����ά�ռ����̬����
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::SetPosition(Coordinate ScanCenter, Coordinate RightDir, Coordinate UpDir, Coordinate MoveDir)
{
	m_ScanCenter = ScanCenter;
	m_RightDir = RightDir;
	m_UpDir = UpDir;
	m_MoveDir = MoveDir;
	return;
}


/*****************************************************************
Name:			SetUSPixelSize
Inputs:
	double dPixelSize - B��ͼ�����ش�С(���ض�Ӧ������ߴ�)
Return Value:
	none
Description:	����B��ͼ�����ش�С(���ض�Ӧ������ߴ�)
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::SetUSPixelSize(double dPixelSize)
{
	m_dUSPixelSize = dPixelSize;
	return;
}


/*****************************************************************
Name:			SetImageSize
Inputs:
	int cx - B��ͼ����(������)
	int cy - B��ͼ��߶�(������)
Return Value:
	none
Description:	����B��ͼ��ֱ���(������)
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::SetImageSize(int cx, int cy)
{
	m_nResolution.cx = cx;
	m_nResolution.cy = cy;
	return;
}


/*****************************************************************
Name:			SetMRIPixelSize
Inputs:
	double dPixelSizeX - MRI���ش�С(x����)
	double dPixelSizeY - MRI���ش�С(y����)
	double dPixelSizeZ - MRI���ش�С(z����)
Return Value:
	none
Description:	����MRI�������ش�С
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::SetMRIPixelSize(double dPixelSizeX, double dPixelSizeY, double dPixelSizeZ)
{
	m_dMRIPixelSizeX = dPixelSizeX;
	m_dMRIPixelSizeY = dPixelSizeY;
	m_dMRIPixelSizeZ = dPixelSizeZ;
	return;
}


/*****************************************************************
Name:			GetSampleMaskPlan
Inputs:
	MaskDataType* pImage - ���mask����ָ��
	int nScanType - B��ɨ�����ͣ�0-��״�棬1-ʸ״��
	int nMaskType - MRIģ�����mask���ͣ�0-ǰ���٣�1-����
Return Value:
	int - error info
Description:	����MRI�������ش�С
*****************************************************************/
int ANALYSEPROCESS::ImageSampler::GetSampleMaskPlan(MaskDataType *pImage, int nScanType, int nMaskType)
{
	//����B��������ĸ��ǵ㣬��Ӧ��3D MRI���ݵ�WLD����
	if (nScanType==0)
		CalculateCoronalMaskPlan();//�����״���ĸ��ǵ�λ��
	else if (nScanType == 1)
		CalculateSagittalMaskPlan();//����ʸ״���ĸ��ǵ�λ��
	else
		return ER_SampleParameterError;

	//���ĸ��ǵ��WLD����תΪIJK����
	m_LeftTop = WLDToIJK(m_LeftTop);
	m_RightTop = WLDToIJK(m_RightTop);
	m_LeftBottom = WLDToIJK(m_LeftBottom);
	m_RightBottom = WLDToIJK(m_RightBottom);

	//ʹ���ĸ��ǵ��IJK���ָ꣬���и��ά����
	if (nMaskType != 0 && nMaskType != 1)
		return ER_SampleParameterError;
	GetMaskPlanData(m_pDataPackage->GetMaskDataPtr(nMaskType), pImage);
	//if (nMaskType == 0)
	//	//��ʱ����ΪSurgicalPlan��MaskDataptr��Ӧ����ָ��ΪProstateMaskPtr TODO
	//	GetMaskPlanData(m_pDataPackage->GetMaskDataPtr(), pImage);
	//else if (nMaskType == 1)
	//	//Ӧָ��ΪlesionMaskPtr TODO
	//	GetMaskPlanData(m_pDataPackage->GetMaskDataPtr(), pImage);
	//else
	//	return ER_SampleParameterError;
	return LIST_NO_ERROR;
}

/*****************************************************************
Name:			IJKToWLD
Inputs:
	Coordinate pt - IJK����
Return Value:
	none
Description:	��IJK����ת����WLD����
*****************************************************************/
Coordinate ANALYSEPROCESS::ImageSampler::IJKToWLD(Coordinate pt)
{
	Coordinate ptRes;
	ptRes.x = pt.x*m_dMRIPixelSizeX;
	ptRes.y = pt.y*m_dMRIPixelSizeY;
	ptRes.z = pt.z*m_dMRIPixelSizeZ;
	ptRes.w = pt.w;
	return ptRes;
}


/*****************************************************************
Name:			WLDToIJK
Inputs:
	Coordinate pt - WLD����
Return Value:
	none
Description:	��WLD����ת����IJK����
*****************************************************************/
Coordinate ANALYSEPROCESS::ImageSampler::WLDToIJK(Coordinate pt)
{
	Coordinate ptRes;
	ptRes.x = pt.x/m_dMRIPixelSizeX;
	ptRes.y = pt.y/m_dMRIPixelSizeY;
	ptRes.z = pt.z/m_dMRIPixelSizeZ;
	ptRes.w = pt.w;
	return ptRes;
}


/*****************************************************************
Name:			CalculateCoronalMaskPlan
Inputs:
	none
Return Value:
	none
Description:	����B����״��ͼ���ĸ��ǵ��WLD����
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::CalculateCoronalMaskPlan()
{
	double dTrueHalfCx = m_nResolution.cx*m_dUSPixelSize / 2.0;	//����ͼ���ȵ�һ��(ʵ������ߴ�/mm)
	double dTrueCy = m_nResolution.cy*m_dUSPixelSize;			//����ͼ��߶�(ʵ������ߴ�/mm)

	m_LeftBottom = m_ScanCenter - m_RightDir * dTrueHalfCx;
	m_RightBottom = m_ScanCenter + m_RightDir * dTrueHalfCx;
	m_LeftTop = m_LeftBottom + m_UpDir * dTrueCy;
	m_RightTop = m_RightBottom + m_UpDir * dTrueCy;
	return;
}


/*****************************************************************
Name:			CalculateCoronalMaskPlan
Inputs:
	none
Return Value:
	none
Description:	����B��ʸ״��ͼ���ĸ��ǵ��WLD����
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::CalculateSagittalMaskPlan()
{
	double dTrueHalfCx = m_nResolution.cx*m_dUSPixelSize / 2.0;	//����ͼ���ȵ�һ��(ʵ������ߴ�)
	double dTrueCy = m_nResolution.cy*m_dUSPixelSize;			//����ͼ��߶�(ʵ������ߴ�)

	m_LeftBottom = m_ScanCenter - m_MoveDir * dTrueHalfCx;
	m_RightBottom = m_ScanCenter + m_MoveDir * dTrueHalfCx;
	m_LeftTop = m_LeftBottom + m_UpDir * dTrueCy;
	m_RightTop = m_RightBottom + m_UpDir * dTrueCy;
	return;
}


/*****************************************************************
Name:			GetMaskPlanData
Inputs:
	none
Return Value:
	none
Description:	����MASK��Ƭ����,��pImageָ�����
*****************************************************************/
void ANALYSEPROCESS::ImageSampler::GetMaskPlanData(MaskDataPtr tMaskDataPtr, MaskDataType *pImage)
{
	int x, y;	//B��ͼ����������
	int tx, ty, tz;	//3D MRIģ�����ITK����
	MaskDataType nCur;		//B��������ĵ�ǰ�� �Ҷ�ֵ
	Coordinate dxUS, dyUS;	//2άB��ͼ��x��y������һ�����أ���Ӧ��3άMRIģ�������IJK����
	Coordinate pCur;		//B��������ĵ�ǰ�� ��Ӧ��3άMRIģ�������IJK����
	int nMaskResX, nMaskResY, nMaskResZ;	//3άB��MASK��x,y,z�����ϵķֱ���(������)

	tMaskDataPtr->GetMaskVolumeSize(nMaskResX, nMaskResY, nMaskResZ);
	dxUS = (m_RightBottom - m_LeftBottom)*(1.0/(m_nResolution.cx - 1));
	dyUS = (m_LeftBottom - m_LeftTop)*(1.0/(m_nResolution.cy - 1));
	dxUS.w = 0;	//����
	dyUS.w = 0;	//����
	for (y = 0; y < m_nResolution.cy; y++)
	{
		pCur = m_LeftTop + dyUS * y;	//�趨ÿһ�е���ʼ��(��άͼ���������(0,0)����Ӧ���Ͻ�))
		for (x = 0; x < m_nResolution.cx; x++)
		{
			tx = CPubFunc::Round(pCur.x);
			ty = CPubFunc::Round(pCur.y);
			tz = CPubFunc::Round(pCur.z);
			if ((tx >= 0) && (ty >= 0) && (tz >= 0) && (tx < nMaskResX) && (ty < nMaskResY) && (tz < nMaskResZ))	//�ж��Ƿ�������ռ���
				nCur = tMaskDataPtr->GetPointValue(tx, ty, tz);	//ʹ������ڲ�ֵ
			else
				nCur = 0;
			*(pImage + x) = nCur;
			pCur = pCur + dxUS;
		}
		pImage += m_nResolution.cx;
	}
	return;
}