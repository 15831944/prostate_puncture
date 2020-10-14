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
#include "ComUtility/Coordinate.h"
#include "ComUtility/Matrix.h"
#include "NDIOperator/Attitude.h"
#include "AnalyseConfig.h"
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
	//λ�ô�����
	class PositionManager;
	typedef std::shared_ptr <PositionManager> PositionManagerPtr;

	class ANALYSEPROCESS_API PositionManager
	{
	public:
		PositionManager();
		~PositionManager();

		BOOL SetBaseUSPosition(fsutility::Attitude BaseUSAttitude);	//����base��������λ�ò���
		BOOL SetBaseMRIPosition(fsutility::Attitude BaseMRIAttitude);	//����base����MRIģ�����λ�ò���
		BOOL CalculateTransformMatrix();	//����base��������MRIģ�������δ֪����������ת������
		BOOL SetCurUSPosition(fsutility::Attitude CurUSAttitude);	//���õ�ǰ����λ��
		BOOL UpDate();	//���µ�ǰMRIģ�����λ�ò���


	public:
		//��׼base��,����λ�ò���
		//Coordinate m_BaseUSScanCenter;
		//Coordinate m_BaseUSRightDir;
		//Coordinate m_BaseUSUpDir;
		//Coordinate m_BaseUSMoveDir;
		fsutility::Attitude m_BaseUSAttitude;
		//��׼base��,MRIģ�����λ�ò���(wld����)
		//Coordinate m_BaseMRIScanCenter;
		//Coordinate m_BaseMRIRightDir;
		//Coordinate m_BaseMRIUpDir;
		//Coordinate m_BaseMRIMoveDir;
		fsutility::Attitude m_BaseMRIAttitude;
		//ʵʱ����λ�ò���
		//Coordinate m_CurUSScanCenter;
		//Coordinate m_CurUSRightDir;
		//Coordinate m_CurUSUpDir;
		//Coordinate m_CurUSMoveDir;
		fsutility::Attitude m_CurUSAttitude;
		//ʵʱMRIģ�����λ�ò���(wld����)
		//Coordinate m_CurMRIScanCenter;
		//Coordinate m_CurMRIRightDir;
		//Coordinate m_CurMRIUpDir;
		//Coordinate m_CurMRIMoveDir;
		fsutility::Attitude m_CurMRIAttitude;
		//����/MRIģ�������λ�ò����任����
		//TransformMatrix��[USScanCenter,USRightDir,USUpDir,USMoveDir]=[MRIScanCenter,MRIRightDir,MRIUpDir,MRIMoveDir]
		fsutility::Matrix m_TransformMatrix;
	private:
		//fsutility::Matrix ConstructAttitude2Matrix(NDIOPERATOR::Attitude attitude);	//����attitude����4*4�任����
	};
}