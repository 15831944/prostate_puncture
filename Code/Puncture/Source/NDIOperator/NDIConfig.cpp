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

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "stdafx.h"
#include "NDIConfig.h"
#include "ComUtility//IniFile.h"
#include "ErrorManager//ErrorCodeDefine.h"


using namespace NDIOPERATOR;

/*****************************************************************
Name:			NDIConfig(�����)
Inputs:
	none
Return Value:
	none
Description:	NDIConfig���캯��
*****************************************************************/
NDIConfig::NDIConfig()
{
}//NDIConfig


/*****************************************************************
Name:			~NDIConfig(�����)
Inputs:
	none
Return Value:
	none
Description:	NDIConfig��������
*****************************************************************/
NDIConfig::~NDIConfig()
{
}//~NDIConfig


/*****************************************************************
Name:			LoadConfigFile(�����)
Inputs:
	CString t_strFilePathName - Config�ļ�����·�����ļ���
Return Value:
	none
Description:	��ȡConfig�ļ������Ϣ
*****************************************************************/
int NDIConfig::LoadConfigFile(CString t_strFilePathName)
{
	//�ж��ļ��Ƿ����
	if (!PathFileExists(t_strFilePathName))
	{
		return ER_WrongNDIConfigFilePathName;
	}

	fsutility::CIniFile t_ScanConfig;	//config�ļ�

	if (!t_ScanConfig.Open(t_strFilePathName))
	{
		return ER_OpenNDIConfigFileFailed;
	}
	
	//t_ScanConfig.Move2Section(_T("DataBase"));
	//t_ScanConfig.ReadKey(_T("DataBaseServer"), m_strDataBaseServer);

	//ndi̽ͷ����NDI�豸�Ķ˿ں�
	t_ScanConfig.Move2Section(_T("Hostname"));
	t_ScanConfig.ReadKey(_T("hostname"), m_strHostname);

	//B��̽ͷ��ʼUpDir
	t_ScanConfig.Move2Section(_T("Initial MoveDir"));
	t_ScanConfig.ReadKey(_T("x"), m_MoveDir.x);
	t_ScanConfig.ReadKey(_T("y"), m_MoveDir.y);
	t_ScanConfig.ReadKey(_T("z"), m_MoveDir.z);
	m_MoveDir.w = 0;		//MoveDirΪ����
	m_MoveDir.Normalize();	//������λ��

	//B��̽ͷ��ʼUpDir
	t_ScanConfig.Move2Section(_T("Initial UpDir"));
	t_ScanConfig.ReadKey(_T("x"), m_UpDir.x);
	t_ScanConfig.ReadKey(_T("y"), m_UpDir.y);
	t_ScanConfig.ReadKey(_T("z"), m_UpDir.z);
	m_UpDir.w = 0;			//UpDirΪ����
	m_UpDir.Normalize();	//������λ��

	//�궨����
	t_ScanConfig.Move2Section(_T("Calibration Matrix"));
	t_ScanConfig.ReadKey(_T("T[0][0]"), m_CalibrationMatrixElem[0]);
	t_ScanConfig.ReadKey(_T("T[0][1]"), m_CalibrationMatrixElem[1]);
	t_ScanConfig.ReadKey(_T("T[0][2]"), m_CalibrationMatrixElem[2]);
	t_ScanConfig.ReadKey(_T("T[0][3]"), m_CalibrationMatrixElem[3]);
	t_ScanConfig.ReadKey(_T("T[1][0]"), m_CalibrationMatrixElem[4]);
	t_ScanConfig.ReadKey(_T("T[1][1]"), m_CalibrationMatrixElem[5]);
	t_ScanConfig.ReadKey(_T("T[1][2]"), m_CalibrationMatrixElem[6]);
	t_ScanConfig.ReadKey(_T("T[1][3]"), m_CalibrationMatrixElem[7]);
	t_ScanConfig.ReadKey(_T("T[2][0]"), m_CalibrationMatrixElem[8]);
	t_ScanConfig.ReadKey(_T("T[2][1]"), m_CalibrationMatrixElem[9]);
	t_ScanConfig.ReadKey(_T("T[2][2]"), m_CalibrationMatrixElem[10]);
	t_ScanConfig.ReadKey(_T("T[2][3]"), m_CalibrationMatrixElem[11]);
	t_ScanConfig.ReadKey(_T("T[3][0]"), m_CalibrationMatrixElem[12]);
	t_ScanConfig.ReadKey(_T("T[3][1]"), m_CalibrationMatrixElem[13]);
	t_ScanConfig.ReadKey(_T("T[3][2]"), m_CalibrationMatrixElem[14]);
	t_ScanConfig.ReadKey(_T("T[3][3]"), m_CalibrationMatrixElem[15]);
	m_CalibrationMatrix.SetValue(m_CalibrationMatrixElem);

	return LIST_NO_ERROR;
}//LoadConfigFile
