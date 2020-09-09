//*****************************************************************
// �ļ��� :						Attitude.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				��̬����
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
#include "Attitude.h"


using namespace NDIOPERATOR;

/*****************************************************************
Name:			Attitude(�����)
Inputs:
	none
Return Value:
	none
Description:	Attitude���캯��
*****************************************************************/
Attitude::Attitude()
{
	m_fX = 0;
	m_fY = 0;
	m_fZ = 0;
	m_fAlpha = EmptyAlpha;
	m_fBeta = 0;
	m_fGama = 0;
}//Attitude


/*****************************************************************
Name:			~Attitude(�����)
Inputs:
	none
Return Value:
	none
Description:	Attitude��������
*****************************************************************/
Attitude::~Attitude()
{
}//~Attitude


/*****************************************************************
Name:			GetPosition
Inputs:
	float &t_fX - ƽ������x
	float &t_fY - ƽ������y
	float &t_fZ - ƽ������z
	float &m_fAlpha - ��X��Ƕ���ת
	float &m_fBeta - ��Y��Ƕ���ת
	float &m_fGama - ��Z��Ƕ���ת
Return Value:
	none
Description:	��ȡ����
*****************************************************************/
void Attitude::GetPosition(float &t_fX, float &t_fY, float &t_fZ,
						   float &t_fAlpha, float &t_fBeta, float &t_fGama)
{
	t_fX = float(m_fX);
	t_fY = float(m_fY);
	t_fZ = float(m_fZ);
	t_fAlpha = float(m_fAlpha);
	t_fBeta = float(m_fBeta);
	t_fGama = float(m_fGama);
}//GetPosition


/*****************************************************************
Name:			SetPosition
Inputs:
	float t_fX - ƽ������x
	float t_fY - ƽ������y
	float t_fZ - ƽ������z
	float t_fAlpha - ��X��Ƕ���ת
	float t_fBeta - ��Y��Ƕ���ת
	float t_fGama - ��Z��Ƕ���ת
Return Value:
	none
Description:	��ȡ����
*****************************************************************/
void Attitude::SetPosition(float t_fX, float t_fY, float t_fZ,
						   float t_fAlpha, float t_fBeta, float t_fGama)
{
	m_fX = t_fX;
	m_fY = t_fY;
	m_fZ = t_fZ;
	m_fAlpha = t_fAlpha;
	m_fBeta = t_fBeta;
	m_fGama = t_fGama;
}//SetPosition


/*****************************************************************
Name:			Empty
Inputs:
	none
Return Value:
	bool - �Ƿ�Ϊ�գ�δ������ʼ������trueδ��ʼ��
Description:	�������
*****************************************************************/
bool Attitude::Empty()
{
	if (abs(m_fAlpha - EmptyAlpha) < 0.0001)
	{
		return true;
	}
	else
	{
		return false;
	}
}//Empty


/*****************************************************************
Name:			operator+
Inputs:
	const Attitude& b - ���ӵ�����
Return Value:
	none
Description:	�������
*****************************************************************/
Attitude Attitude::operator+(const Attitude& b)
{
	Attitude result;
	result.m_fX = this->m_fX + b.m_fX;
	result.m_fY = this->m_fY + b.m_fY;
	result.m_fZ = this->m_fZ + b.m_fZ;
	result.m_fAlpha = this->m_fAlpha + b.m_fAlpha;
	result.m_fBeta = this->m_fBeta + b.m_fBeta;
	result.m_fGama = this->m_fGama + b.m_fGama;

	return result;
}//operator+


/*****************************************************************
Name:			operator-
Inputs:
	const Attitude& b - ����������
Return Value:
	none
Description:	�������
*****************************************************************/
Attitude Attitude::operator-(const Attitude& b)
{
	Attitude result;
	result.m_fX = this->m_fX - b.m_fX;
	result.m_fY = this->m_fY - b.m_fY;
	result.m_fZ = this->m_fZ - b.m_fZ;
	result.m_fAlpha = this->m_fAlpha - b.m_fAlpha;
	result.m_fBeta = this->m_fBeta - b.m_fBeta;
	result.m_fGama = this->m_fGama - b.m_fGama;

	return result;
}//operator-


/*****************************************************************
Name:			operator-
Inputs:
	const float& b - �����ı���
Return Value:
	none
Description:	������
*****************************************************************/
Attitude Attitude::operator/(const float& b)
{
	Attitude result;
	if (b != 0)
	{
		result.m_fX = this->m_fX / b;
		result.m_fY = this->m_fY / b;
		result.m_fZ = this->m_fZ / b;
		result.m_fAlpha = this->m_fAlpha / b;
		result.m_fBeta = this->m_fBeta / b;
		result.m_fGama = this->m_fGama / b;
		return result;
	}

	return *this;
}//operator/