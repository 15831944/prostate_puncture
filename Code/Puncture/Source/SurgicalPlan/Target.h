//*****************************************************************
// �ļ��� :						Target.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				�е�Ŀ����
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
						Routine Definitions
*****************************************************************/

namespace SURGICALPLAN
{
	//�ռ����
	class SURGICALPLAN_API Target
	{
	public:
		Target();
		virtual ~Target();

	public:
		Point3D m_TargetPoint;		//Ŀ���Ŀռ�����
		Point3D m_StartPoint;		//������ʼ��Ŀռ�����
	};

}
