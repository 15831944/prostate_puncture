//*****************************************************************
// �ļ��� :						MRIData.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				�Ź������ݹ���ģ��
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
#include "StructDef.h"

/*****************************************************************
Defines
*****************************************************************/
#ifdef SURGICALPLAN_EXPORT
#define SURGICALPLAN_API __declspec(dllexport)
#else
#define SURGICALPLAN_API __declspec(dllimport)
#endif

/*****************************************************************
Defines
*****************************************************************/


/*****************************************************************
						Routine Definitions
*****************************************************************/
namespace SURGICALPLAN
{
	class SURGICALPLAN_API MRIData
	{
	public:
		MRIData();
		virtual ~MRIData();

	public:
		int LoadRawMRI(CString t_strFileName, int cx, int cy, int cz);		//��������
		void SetMRIVolumeResolution(double ResX, double ResY, double ResZ);
		void GetMRIVolumeResolution(double &ResX, double &ResY, double &ResZ);

		void GetMRIVolumeSize(int &x, int &y, int &z);
		MRIDataType GetPointValue(double x, double y, double z); //��ȡĳһ���MRI���ݣ��������ڽ���ֵ
		
	protected:
		void InitData();
		void DestroyData();

	private:
		MRIDataType * m_pMRIData;			//�Ź���������
		Point3D m_MRIRes;					//MRIͼ��ķֱ���

		int m_nPixNumberX;					//������������ߴ�
		int m_nPixNumberY;
		int m_nPixNumberZ;
	};

	typedef std::shared_ptr<MRIData> MRIDataPtr;

}
