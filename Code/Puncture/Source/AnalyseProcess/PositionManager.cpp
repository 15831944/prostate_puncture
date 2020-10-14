//*****************************************************************
// �ļ��� :						AnalyseConfig.cpp
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				AnalyseProcess��������
// �������� :					2020.4.28
// �޸����� :					
// ���� :						SunYizhe
// �޸��� :						
// ��ϵ��ʽ :					
//*****************************************************************/

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "pch.h"
#include "PositionManager.h"
#include "ErrorManager/ErrorCodeDefine.h"

using namespace ANALYSEPROCESS;
using namespace fsutility;

/*****************************************************************
Name:			PositionManager
Inputs:
	none
Return Value:
	none
Description:	PositionManager���캯��
*****************************************************************/
PositionManager::PositionManager()
{
	//TODO
}

/*****************************************************************
Name:			PositionManager
Inputs:
	none
Return Value:
	none
Description:	PositionManager��������
*****************************************************************/
PositionManager::~PositionManager()
{
}

/*****************************************************************
Name:			SetBaseUSPosition
Inputs:
	fsutility Attitude BaseUSAttitude - �洢����base��λ��
Return Value:
	 int - Error Info
Description:	����NDI�豸������attitude������base��������̽ͷλ�ò���
*****************************************************************/
BOOL PositionManager::SetBaseUSPosition(fsutility::Attitude BaseUSAttitude)
{
	m_BaseUSAttitude = BaseUSAttitude;
	return LIST_NO_ERROR;
}


/*****************************************************************
Name:			SetBaseMRIPosition
Inputs:	
	AnalyseConfig config - Config���󣬴洢�����ļ���Ϣ
Return Value:
	int - Error Info
Description:	����config�ļ�������base����MRIģ�����λ�ò���
*****************************************************************/
BOOL PositionManager::SetBaseMRIPosition(fsutility::Attitude BaseMRIAttitude)
{
	m_BaseMRIAttitude = BaseMRIAttitude;
	return LIST_NO_ERROR;
}

/*****************************************************************
Name:			CalculateTransformMatrix
Inputs:
	None
Return Value:
	int - Error Info
Description:	
	US��һ��λ�ò����������任�������㣬���ɵõ�MRIģ�������һ��λ�ò���
	TransformMatrix��[USScanCenter,USRightDir,USUpDir,USMoveDir]=[MRIScanCenter,MRIRightDir,MRIUpDir,MRIMoveDir]
	����base����US��MRIģ�������һ��λ�ò�����������øñ任����
	���任���󲻴��ڣ��򷵻ش�����Ϣ
*****************************************************************/
BOOL PositionManager::CalculateTransformMatrix()
{
	//Matrix matrixMRIBasePosition(m_BaseMRIScanCenter, m_BaseMRIRightDir, m_BaseMRIUpDir, m_BaseMRIMoveDir);
	//Matrix matrixUSBasePosition(m_BaseUSScanCenter, m_BaseUSRightDir, m_BaseUSUpDir, m_BaseUSMoveDir);

	Matrix matrixMRIBasePosition(m_BaseMRIAttitude.m_ScanCenter, m_BaseMRIAttitude.m_RightDir, m_BaseMRIAttitude.m_UpDir, m_BaseMRIAttitude.m_MoveDir);
	Matrix matrixUSBasePosition(m_BaseUSAttitude.m_ScanCenter, m_BaseUSAttitude.m_RightDir, m_BaseUSAttitude.m_UpDir, m_BaseUSAttitude.m_MoveDir);
	//�����ж�ת�������Ƿ����
	if (matrixUSBasePosition.GetDeterminant() == 0)
	{
		return ER_CalculateTransformMatrix;
	}

	m_TransformMatrix = matrixMRIBasePosition.GetMultiply(matrixUSBasePosition.GetInverse());
	return LIST_NO_ERROR;
}

/*****************************************************************
Name:			SetCurUSPosition
Inputs:
	Attitude aBaseUS - �洢����base��λ��
Return Value:
	 int - Error Info
Description:	����NDI�豸������attitude�����õ�ǰ ����̽ͷ��̬����
*****************************************************************/
BOOL PositionManager::SetCurUSPosition(fsutility::Attitude CurUSAttitude)
{
	m_CurUSAttitude = CurUSAttitude;
	return LIST_NO_ERROR;
}

/*****************************************************************
Name:			UpDate
Inputs:
	None
Return Value:
	int - Error Info
Description:	���ݵ�ǰ��USδ֪�������Լ��任������ȡ��ǰMRIģ�����λ�ò���
*****************************************************************/
BOOL PositionManager::UpDate()
{
	//m_CurMRIScanCenter = m_TransformMatrix.GetMultiply(m_CurUSScanCenter);
	//m_CurMRIRightDir = m_TransformMatrix.GetMultiply(m_CurUSRightDir);
	//m_CurMRIUpDir = m_TransformMatrix.GetMultiply(m_CurUSUpDir);
	//m_CurMRIMoveDir = m_TransformMatrix.GetMultiply(m_CurUSMoveDir);
	m_CurMRIAttitude=m_CurUSAttitude.Transform(m_TransformMatrix);
	return LIST_NO_ERROR;
}

/*****************************************************************
Name:			ConstructAttitude2Matrix
Inputs:
	Attitude attitude
Return Value:
	 Matrix - �任����
Description:	��Attitude����任����
*****************************************************************/
//Matrix PositionManager::ConstructAttitude2Matrix(NDIOPERATOR::Attitude attitude)
//{
//	double m[16];
//	float x, y, z, roll, pitch, yaw;
//	attitude.GetPosition(x, y, z, roll, pitch, yaw);
//	m[0] = cos(yaw) * cos(pitch);
//	m[1] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
//	m[2] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
//	m[3] = x;
//	m[4] = sin(yaw) * cos(pitch);
//	m[5] = sin(yaw) * sin(pitch)*sin(roll) + cos(yaw) * cos(roll);
//	m[6] = sin(yaw) * sin(pitch)*cos(roll) - cos(yaw) * sin(roll);
//	m[7] = y;
//	m[8] = -sin(pitch);
//	m[9] = cos(pitch) * sin(roll);
//	m[10] = cos(pitch) * cos(roll);
//	m[11] = z;
//	m[12] = 0;
//	m[13] = 0;
//	m[14] = 0;
//	m[15] = 1;
//	return Matrix(m);
//}