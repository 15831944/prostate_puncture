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
#include "Coordinate.h"
#include "Matrix.h"

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

		Coordinate m_ScanCenter;	//������λ�õ�����
		Coordinate m_RightDir;		//������λ�ã�2άB��ͼ���x���������Ӧ����ά�ռ�����
		Coordinate m_UpDir;			//������λ�ã�2άB��ͼ���y�Ḻ�����Ӧ����ά�ռ�����
		Coordinate m_MoveDir;		//B��̽ͷǰ���ķ��򣬼���ֱ����ڵ�������λ�õ�����
		int m_ResX;		//x����������
		int m_ResY;		//y����������
		int m_ResZ;		//z����������
		float m_SizeX;	//x�������ش�С
		float m_SizeY;	//y�������ش�С
		float m_SizeZ;	//z�������ش�С
	};
}
