//*****************************************************************
// �ļ��� :						USBConfig.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				����ͼ��ɼ��豸����
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
#include "USBConfig.h"
#include "ComUtility//IniFile.h"
#include "ErrorManager//ErrorCodeDefine.h"


using namespace USBCAPTURER;

/*****************************************************************
Name:			USBConfig(�����)
Inputs:
	none
Return Value:
	none
Description:	USBConfig���캯��
*****************************************************************/
USBConfig::USBConfig()
{
	////B���������������
	//m_Axis.x = 0;
	//m_Axis.y = 0;
	//m_Axis.width = 0;
	//m_Axis.height = 0;
	
	//�Ҳ����������� �����жϷֱ���
	m_RightAxisRect.x = 0;
	m_RightAxisRect.y = 0;
	m_RightAxisRect.width = 0;
	m_RightAxisRect.height = 0;

	//������������� �����жϵ�ƽ��/˫ƽ��
	m_UpAxisRect.x = 0;
	m_UpAxisRect.y = 0;
	m_UpAxisRect.width = 0;
	m_UpAxisRect.height = 0;
	m_DownAxisRect.x = 0;
	m_DownAxisRect.y = 0;
	m_DownAxisRect.width = 0;
	m_DownAxisRect.height = 0;

	//����Ļ�µĲ���
	m_FullRect.x = 0;
	m_FullRect.y = 0;
	m_FullRect.width = 0;
	m_FullRect.height = 0;

	//˫��Ļ�µĲ���
	m_UpRect.x = 0;
	m_UpRect.y = 0;
	m_UpRect.width = 0;
	m_UpRect.height = 0;

	m_DownRect.x = 0;
	m_DownRect.y = 0;
	m_DownRect.width = 0;
	m_DownRect.height = 0;

}//USBConfig


/*****************************************************************
Name:			~USBConfig(�����)
Inputs:
	none
Return Value:
	none
Description:	USBConfig��������
*****************************************************************/
USBConfig::~USBConfig()
{
}//~USBConfig


/*****************************************************************
Name:			LoadConfigFile(�����)
Inputs:
	CString t_strFilePathName - Config�ļ�����·�����ļ���
Return Value:
	none
Description:	��ȡConfig�ļ������Ϣ
*****************************************************************/
int USBConfig::LoadConfigFile(CString t_strFilePathName)
{
	//�ж��ļ��Ƿ����
	if (!PathFileExists(t_strFilePathName))
	{
		return ER_WrongUSBConfigFilePathName;
	}

	fsutility::CIniFile t_ScanConfig;	//config�ļ�

	if (!t_ScanConfig.Open(t_strFilePathName))
	{
		return ER_OpenUSBConfigFileFailed;
	}

	////B����������������
	//t_ScanConfig.Move2Section(_T("Axis"));
	//t_ScanConfig.ReadKey(_T("StartX"), m_Axis.x);
	//t_ScanConfig.ReadKey(_T("StartY"), m_Axis.y);
	//t_ScanConfig.ReadKey(_T("Width"), m_Axis.width);
	//t_ScanConfig.ReadKey(_T("Height"), m_Axis.height);

	//�Ҳ����������� �����жϷֱ���
	t_ScanConfig.Move2Section(_T("Right Axis"));
	t_ScanConfig.ReadKey(_T("StartX"), m_RightAxisRect.x);
	t_ScanConfig.ReadKey(_T("StartY"), m_RightAxisRect.y);
	t_ScanConfig.ReadKey(_T("Width"), m_RightAxisRect.width);
	t_ScanConfig.ReadKey(_T("Height"), m_RightAxisRect.height);

	//������������� �����жϵ�ƽ��/˫ƽ��
	t_ScanConfig.Move2Section(_T("Left Axis"));
	t_ScanConfig.ReadKey(_T("StartX_Up"), m_UpAxisRect.x);
	t_ScanConfig.ReadKey(_T("StartY_Up"), m_UpAxisRect.y);
	t_ScanConfig.ReadKey(_T("Width_Up"), m_UpAxisRect.width);
	t_ScanConfig.ReadKey(_T("Height_Up"), m_UpAxisRect.height);

	t_ScanConfig.ReadKey(_T("StartX_Down"), m_DownAxisRect.x);
	t_ScanConfig.ReadKey(_T("StartY_Down"), m_DownAxisRect.y);
	t_ScanConfig.ReadKey(_T("Width_Down"), m_DownAxisRect.width);
	t_ScanConfig.ReadKey(_T("Height_Down"), m_DownAxisRect.height);

	//��ƽ��/˫ƽ��ROI
	t_ScanConfig.Move2Section(_T("ROI"));
	t_ScanConfig.ReadKey(_T("StartX_Full"), m_FullRect.x);
	t_ScanConfig.ReadKey(_T("StartY_Full"), m_FullRect.y);
	t_ScanConfig.ReadKey(_T("Width_Full"), m_FullRect.width);
	t_ScanConfig.ReadKey(_T("Height_Full"), m_FullRect.height);

	t_ScanConfig.ReadKey(_T("StartX_Up"), m_UpRect.x);
	t_ScanConfig.ReadKey(_T("StartY_Up"), m_UpRect.y);
	t_ScanConfig.ReadKey(_T("Width_Up"), m_UpRect.width);
	t_ScanConfig.ReadKey(_T("Height_Up"), m_UpRect.height);

	t_ScanConfig.ReadKey(_T("StartX_Down"), m_DownRect.x);
	t_ScanConfig.ReadKey(_T("StartY_Down"), m_DownRect.y);
	t_ScanConfig.ReadKey(_T("Width_Down"), m_DownRect.width);
	t_ScanConfig.ReadKey(_T("Height_Down"), m_DownRect.height);

	return LIST_NO_ERROR;
}//LoadConfigFile
