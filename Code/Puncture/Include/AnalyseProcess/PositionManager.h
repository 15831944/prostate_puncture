//*****************************************************************
// �ļ��� :						PositionManager.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				B��/MRIģ�����֮�� ����ת��
// �������� :					2020.05.05
// �޸����� :					
// ���� :						SunYizhe
// �޸��� :						
// ��ϵ��ʽ :					
//*****************************************************************/
#pragma once

/*****************************************************************
Project Files Included
*****************************************************************/
#include "Coordinate.h"
#include "Matrix.h"
#include "NDIOperator/Attitude.h"
#include "AnalyseConfig.h"

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
	//λ�ô�����
	class PositionManager;
	typedef std::shared_ptr <PositionManager> PositionManagerPtr;

	class ANALYSEPROCESS_API PositionManager
	{
	public:
		PositionManager();
		~PositionManager();

		BOOL SetBaseUSPosition(NDIOPERATOR::Attitude aBaseUS);	//����base��������λ�ò���
		BOOL SetBaseMRIPosition(AnalyseConfig *config);	//����base����MRIģ�����λ�ò���
		BOOL CalculateTransformMatrix();	//����base��������MRIģ�������δ֪����������ת������
		BOOL SetCurUSPosition(NDIOPERATOR::Attitude aCurUS);	//���õ�ǰ����λ��
		BOOL UpDate();	//���µ�ǰMRIģ�����λ�ò���


	public:
		//��׼base��,����λ�ò���
		Coordinate m_BaseUSScanCenter;
		Coordinate m_BaseUSRightDir;
		Coordinate m_BaseUSUpDir;
		Coordinate m_BaseUSMoveDir;
		//��׼base��,MRIģ�����λ�ò���
		Coordinate m_BaseMRIScanCenter;
		Coordinate m_BaseMRIRightDir;
		Coordinate m_BaseMRIUpDir;
		Coordinate m_BaseMRIMoveDir;
		//ʵʱ����λ�ò���(WLD����)
		Coordinate m_CurUSScanCenter;
		Coordinate m_CurUSRightDir;
		Coordinate m_CurUSUpDir;
		Coordinate m_CurUSMoveDir;
		//ʵʱMRIģ�����λ�ò���(WLD����)
		Coordinate m_CurMRIScanCenter;
		Coordinate m_CurMRIRightDir;
		Coordinate m_CurMRIUpDir;
		Coordinate m_CurMRIMoveDir;
		//����/MRIģ�������λ�ò����任����
		//TransformMatrix��[USScanCenter,USRightDir,USUpDir,USMoveDir]=[MRIScanCenter,MRIRightDir,MRIUpDir,MRIMoveDir]
		Matrix m_TransformMatrix;
	private:
		Matrix ConstructAttitude2Matrix(NDIOPERATOR::Attitude attitude);	//����attitude����4*4�任����
	};
}