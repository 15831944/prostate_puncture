//*****************************************************************
// �ļ��� :						NDIConfig.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				NID�豸����
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
#include <string>
#include "ComUtility/singleton.h"
#include "ComUtility/Coordinate.h"
#include "ComUtility/Matrix.h"


/*****************************************************************
Defines
*****************************************************************/
#ifdef NDIOPERATOR_EXPORT
#define NDIOPERATOR_API __declspec(dllexport)
#else
#define NDIOPERATOR_API __declspec(dllimport)
#endif


/*****************************************************************
						Routine Definitions
*****************************************************************/
namespace NDIOPERATOR
{
	//��������ɼ�ͼ��ṹ��
	class NDIOPERATOR_API NDIConfig : public fsutility::Singleton<NDIConfig>
	{
		friend class fsutility::Singleton<NDIConfig>;

	private:
		NDIConfig();
		virtual ~NDIConfig();

	public:
		int LoadConfigFile(CString t_strFilePathName);

	public:
		double						m_CalibrationMatrixElem[16];
		std::string					m_strHostname;
		fsutility::Coordinate		m_MoveDir;
		fsutility::Coordinate		m_UpDir;
		fsutility::Matrix			m_CalibrationMatrix;

	};
}