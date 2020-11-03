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

	//˫ƽ�����೤������
	t_ScanConfig.Move2Section(_T("Dual Left Bar"));
	t_ScanConfig.ReadKey(_T("x"), m_DualLeftBarRect.x);
	t_ScanConfig.ReadKey(_T("y"), m_DualLeftBarRect.y);
	t_ScanConfig.ReadKey(_T("width"), m_DualLeftBarRect.width);
	t_ScanConfig.ReadKey(_T("height"), m_DualLeftBarRect.height);

	//ָʾ�����(T)/ʸ״��(S)���ַ�����
	t_ScanConfig.Move2Section(_T("Char Sensor Type"));
	t_ScanConfig.ReadKey(_T("x"), m_CharSensorTypeRect.x);
	t_ScanConfig.ReadKey(_T("y"), m_CharSensorTypeRect.y);
	t_ScanConfig.ReadKey(_T("width"), m_CharSensorTypeRect.width);
	t_ScanConfig.ReadKey(_T("height"), m_CharSensorTypeRect.height);

	//��ƽ�� �Ҳ�����������
	t_ScanConfig.Move2Section(_T("One Plane Right Axis"));
	t_ScanConfig.ReadKey(_T("x"), m_OnePlaneRightAxisRect.x);
	t_ScanConfig.ReadKey(_T("y"), m_OnePlaneRightAxisRect.y);
	t_ScanConfig.ReadKey(_T("width"), m_OnePlaneRightAxisRect.width);
	t_ScanConfig.ReadKey(_T("height"), m_OnePlaneRightAxisRect.height);

	//˫ƽ��ģʽ �Ϸ�(�����)�Ҳ�����������
	t_ScanConfig.Move2Section(_T("Dual Plane Up Right Axis"));
	t_ScanConfig.ReadKey(_T("x"), m_DualPlaneUpRightAxisRect.x);
	t_ScanConfig.ReadKey(_T("y"), m_DualPlaneUpRightAxisRect.y);
	t_ScanConfig.ReadKey(_T("width"), m_DualPlaneUpRightAxisRect.width);
	t_ScanConfig.ReadKey(_T("height"), m_DualPlaneUpRightAxisRect.height);

	//˫ƽ��ģʽ �·�(ʸ״��)�Ҳ�����������
	t_ScanConfig.Move2Section(_T("Dual Plane Down Right Axis"));
	t_ScanConfig.ReadKey(_T("x"), m_DualPlaneDownRightAxisRect.x);
	t_ScanConfig.ReadKey(_T("y"), m_DualPlaneDownRightAxisRect.y);
	t_ScanConfig.ReadKey(_T("width"), m_DualPlaneDownRightAxisRect.width);
	t_ScanConfig.ReadKey(_T("height"), m_DualPlaneDownRightAxisRect.height);

	//��ƽ��ģʽ�� ͼ������
	t_ScanConfig.Move2Section(_T("One Plane ROI"));
	t_ScanConfig.ReadKey(_T("x"), m_OnePlaneROI.x);
	t_ScanConfig.ReadKey(_T("y"), m_OnePlaneROI.y);
	t_ScanConfig.ReadKey(_T("width"), m_OnePlaneROI.width);
	t_ScanConfig.ReadKey(_T("height"), m_OnePlaneROI.height);

	//˫ƽ��ģʽ�� �Ϸ�(�����)ͼ������
	t_ScanConfig.Move2Section(_T("Dual Up ROI"));
	t_ScanConfig.ReadKey(_T("x"), m_DualUpROI.x);
	t_ScanConfig.ReadKey(_T("y"), m_DualUpROI.y);
	t_ScanConfig.ReadKey(_T("width"), m_DualUpROI.width);
	t_ScanConfig.ReadKey(_T("height"), m_DualUpROI.height);

	//˫ƽ��ģʽ�� �·�(ʸ״��)ͼ������
	t_ScanConfig.Move2Section(_T("Dual Down ROI"));
	t_ScanConfig.ReadKey(_T("x"), m_DualDownROI.x);
	t_ScanConfig.ReadKey(_T("y"), m_DualDownROI.y);
	t_ScanConfig.ReadKey(_T("width"), m_DualDownROI.width);
	t_ScanConfig.ReadKey(_T("height"), m_DualDownROI.height);

	return LIST_NO_ERROR;
}//LoadConfigFile
