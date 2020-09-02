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
#include "AnalyseProcess/AnalyseConfig.h"
#include "ComUtility/IniFile.h"
#include "ErrorManager/ErrorCodeDefine.h"

using namespace ANALYSEPROCESS;

/*****************************************************************
Name:			AnalyseConfig
Inputs:
	none
Return Value:
	none
Description:	AnalyseConfig���캯��
*****************************************************************/
AnalyseConfig::AnalyseConfig()
{
	m_ResX = 0;
	m_ResY = 0;
	m_ResZ = 0;
	m_SizeX = 0;
	m_SizeY = 0;
	m_SizeZ = 0;
}

/*****************************************************************
Name:			AnalyseConfig
Inputs:
	none
Return Value:
	none
Description:	AnalyseConfig��������
*****************************************************************/
AnalyseConfig::~AnalyseConfig()
{
}

/*****************************************************************
Name:			LoadConfigFile
Inputs:
	CString t_strFilePathName - Config�ļ�����·�����ļ���
Return Value:
	none
Description:	��ȡConfig�ļ������Ϣ
*****************************************************************/
int AnalyseConfig::LoadConfigFile(CString t_strFilePathName)
{
	//�ж��ļ��Ƿ����
	if (!PathFileExists(t_strFilePathName))
	{
		return ER_WrongAnalyseConfigFilePathName;
	}

	fsutility::CIniFile t_ScanConfig;	//config�ļ�

	if (!t_ScanConfig.Open(t_strFilePathName))
	{
		return ER_OpenAnalyseConfigFileFailed;
	}

	//MRI�ֱ���
	t_ScanConfig.Move2Section(_T("ImageSize"));
	t_ScanConfig.ReadKey(_T("x"), m_ResX);
	t_ScanConfig.ReadKey(_T("y"), m_ResY);
	t_ScanConfig.ReadKey(_T("z"), m_ResZ);
	//MRI���ش�С
	t_ScanConfig.Move2Section(_T("VoxelSize"));
	t_ScanConfig.ReadKey(_T("x"), m_SizeX);
	t_ScanConfig.ReadKey(_T("y"), m_SizeY);
	t_ScanConfig.ReadKey(_T("z"), m_SizeZ);

	//�и�ǰ����������
	//ScanCenter��
	t_ScanConfig.Move2Section(_T("ScanCenter"));
	t_ScanConfig.ReadKey(_T("x"), m_ScanCenter.x);
	t_ScanConfig.ReadKey(_T("y"), m_ScanCenter.y);
	t_ScanConfig.ReadKey(_T("z"), m_ScanCenter.z);
	m_ScanCenter.w = 1;	//���w��Ϊ1
	m_ScanCenter.Normalize();
	//RightDir����
	t_ScanConfig.Move2Section(_T("RightDir"));
	t_ScanConfig.ReadKey(_T("x"), m_RightDir.x);
	t_ScanConfig.ReadKey(_T("y"), m_RightDir.y);
	t_ScanConfig.ReadKey(_T("z"), m_RightDir.z);
	m_RightDir.w = 0;
	m_RightDir.Normalize();		//������λ��
	//UpDir����
	t_ScanConfig.Move2Section(_T("UpDir"));
	t_ScanConfig.ReadKey(_T("x"), m_UpDir.x);
	t_ScanConfig.ReadKey(_T("y"), m_UpDir.y);
	t_ScanConfig.ReadKey(_T("z"), m_UpDir.z);
	m_UpDir.w = 0;
	m_UpDir.Normalize();		//������λ��
	//MoveDir����
	t_ScanConfig.Move2Section(_T("MoveDir"));
	t_ScanConfig.ReadKey(_T("x"), m_MoveDir.x);
	t_ScanConfig.ReadKey(_T("y"), m_MoveDir.y);
	t_ScanConfig.ReadKey(_T("z"), m_MoveDir.z);
	m_MoveDir.w = 0;
	m_MoveDir.Normalize();		//������λ��
	//return LIST_NO_ERROR;
	return 0;	//��ɾ��
}