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

	return LIST_NO_ERROR;
}//LoadConfigFile
