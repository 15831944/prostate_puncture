//*****************************************************************
// �ļ��� :						AnalyseConfig.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				AnalyseProcess��������
// �������� :					2020.4.28
// �޸����� :					
// ���� :						SunYizhe
// �޸��� :						
// ��ϵ��ʽ :					
//*****************************************************************/
#pragma once

/*****************************************************************
Project Files Included
*****************************************************************/
#include "ComUtility/singleton.h"
#include "ComUtility/Coordinate.h"
#include "ComUtility/Matrix.h"
#include "ComUtility/Attitude.h"

//using namespace fsutility;

/*****************************************************************
Defines
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
	class ANALYSEPROCESS_API AnalyseConfig : public fsutility::Singleton<AnalyseConfig>
	{
		friend class fsutility::Singleton<AnalyseConfig>;
	private:
		AnalyseConfig();
		virtual ~AnalyseConfig();
	public:
		int LoadConfigFile(CString t_strFilePathName);

		fsutility::Coordinate m_ScanCenter;		//������λ�õ�����
		fsutility::Coordinate m_RightDir;		//������λ�ã�2άB��ͼ���x���������Ӧ����ά�ռ�����
		fsutility::Coordinate m_UpDir;			//������λ�ã�2άB��ͼ���y�Ḻ�����Ӧ����ά�ռ�����
		fsutility::Coordinate m_MoveDir;		//B��̽ͷǰ���ķ��򣬼���ֱ����ڵ�������λ�õ�����
		fsutility::Attitude m_Attitude;			//MRIģ����� ��ȡǰ���������洦��ģ��̽ͷ����̬

		int m_nSizeX;		//x����������
		int m_nSizeY;		//y����������
		int m_nSizeZ;		//z����������
		float m_fVoxelSizeX;	//x�������ش�С
		float m_fVoxelSizeY;	//y�������ش�С
		float m_fVoxelSizeZ;	//z�������ش�С
	};
}
