#pragma once
//̽ͷ(mriģ�����̽ͷ/ʵ�ʳ���̽ͷ)�Ŀռ���̬����λ��+����
//ScanCenter	̽ͷ���ĵ�
//RightDir		2άB��ͼ���x���������Ӧ����ά�ռ�����
//UpDir			2άB��ͼ���y�Ḻ�����Ӧ����ά�ռ�����
//MoveDir		B��̽ͷָ��ķ���

#include "Coordinate.h"
#include "Matrix.h"

/*****************************************************************
Defines
*****************************************************************/

namespace fsutility
{
	class AFX_EXT_CLASS Attitude
	{
	public:
		Attitude();
		~Attitude();
		void SetValue(Coordinate ScanCenter, Coordinate RightDir, Coordinate UpDir, Coordinate MoveDir);
		Attitude Transform(Matrix mtx);		//������mtx�����ڱ���̬����TransformMatrix��[SrcScanCenter,SrcRightDir,SrcUpDir,SrcMoveDir]=[ResScanCenter,ResRightDir,ResUpDir,ResMoveDir]
	
	public:
		Coordinate m_ScanCenter;
		Coordinate m_RightDir;
		Coordinate m_UpDir;
		Coordinate m_MoveDir;
	};
}