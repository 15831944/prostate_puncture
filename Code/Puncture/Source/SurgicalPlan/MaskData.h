#pragma once
#include "StructDef.h"

/*****************************************************************
Defines
*****************************************************************/
#ifdef SURGICALPLAN_EXPORT
#define SURGICALPLAN_API __declspec(dllexport)
#else
#define SURGICALPLAN_API __declspec(dllimport)
#endif

namespace SURGICALPLAN
{
	class SURGICALPLAN_API MaskData
	{
	public:
		MaskData();
		~MaskData();

		int LoadRawMask(CString strFileName, int cx, int cy, int cz); //����ǰ������MRI�е�Mask

		void SetMaskVolumeResolution(double ResX, double ResY, double ResZ);
		void GetMaskVolumeResolution(double &ResX, double &ResY, double &ResZ);

		void GetMaskVolumeSize(int &x, int &y, int &z);
		MaskDataType GetPointValue(double x, double y, double z); //��ȡĳһ�����Ĥ���ݣ��������ڽ���ֵ

	protected:
		void InitData();
		void DestroyData();

	protected:
		MaskDataType * m_pMRIMaskData;		//�Ź���ָ���Ĥ������
		Point3D m_MaskRes;					//Maskͼ��ķֱ���

		int m_nPixNumberX;					//������������ߴ�
		int m_nPixNumberY;
		int m_nPixNumberZ;

	};

	typedef std::shared_ptr<MaskData> MaskDataPtr;

}