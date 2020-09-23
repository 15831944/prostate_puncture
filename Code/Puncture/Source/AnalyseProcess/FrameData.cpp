//*****************************************************************
// �ļ��� :						FrameData.h
// �汾	 :						2.0
// Ŀ�ļ���Ҫ���� :				��֡��Ϣ��
// �������� :					2019.11.1
// �޸����� :					2020.05.07
// ���� :						Fiki
// �޸��� :						SunYizhe
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/

///////////////////////////////////////////////////////
////////////////////////include////////////////////////
///////////////////////////////////////////////////////
#include "pch.h"
#include "AnalyseProcess/FrameData.h"
#include "ErrorManager/ErrorCodeDefine.h"

using namespace ANALYSEPROCESS;

/*****************************************************************
Name:			FrameData
Inputs:
	none
Return Value:
	none
Description:	FrameData���캯��
*****************************************************************/
FrameData::FrameData()
{
	//m_pProstateMask = nullptr;
	//m_pLesionMask = nullptr;
	m_pFusionMask = nullptr;
	m_dImageRes = 0;
}//FrameData


/*****************************************************************
Name:			~FrameData
Inputs:
	none
Return Value:
	none
Description:	FrameData��������
*****************************************************************/
FrameData::~FrameData()
{
	//if (m_pProstateMask != nullptr)
	//{
	//	delete[] m_pProstateMask;
	//}
	//if (m_pLesionMask != nullptr)
	//{
	//	delete[] m_pLesionMask;
	//}
	if (m_pFusionMask != nullptr)
	{
		delete[] m_pFusionMask;
	}
}//~FrameData


/*****************************************************************
Name:			CreatMaskData
Inputs:
	none
Return Value:
	none
Description:	����2D��ģͼ��ռ�
*****************************************************************/
int	FrameData::CreatMaskData(int t_nShowWidth, int t_nShowHeight)
{
	//if (m_pProstateMask != nullptr)
	//{
	//	delete[] m_pProstateMask;
	//}
	//if (m_pLesionMask != nullptr)
	//{
	//	delete[] m_pLesionMask;
	//}
	if (m_pFusionMask != nullptr)
		delete[] m_pFusionMask;
	if (t_nShowWidth <= 100 || t_nShowWidth >= 4000
		|| t_nShowHeight <= 100 || t_nShowHeight >= 4000)
	{
		return ER_2DImageSizeError;
	}

	//m_pProstateMask = new BYTE[t_nShowWidth * t_nShowWidth];
	//m_pLesionMask = new BYTE[t_nShowWidth * t_nShowWidth];
	m_pFusionMask = new BYTE[t_nShowWidth * t_nShowWidth];
	return LIST_NO_ERROR;
}//CreatMaskData

/*****************************************************************
Name:			SetPosition
Inputs:
	Coordinate ScanCenter etc.	-	��̬����(wld)
Return Value:
	none
Description:	���õ�ǰ��ƬMRIģ�������̬����(wld)
*****************************************************************/
void FrameData::SetPosition(Coordinate ScanCenter, Coordinate RightDir, Coordinate UpDir, Coordinate MoveDir)
{
	m_ScanCenter = ScanCenter;
	m_RightDir = RightDir;
	m_UpDir = UpDir;
	m_MoveDir = MoveDir;
}//SetPosition