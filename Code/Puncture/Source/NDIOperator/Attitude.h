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
#pragma once

/*****************************************************************
Project Files Included
*****************************************************************/
#include <memory>


/*****************************************************************
Defines
*****************************************************************/
#ifdef NDIOPERATOR_EXPORT
#define NDIOPERATOR_API __declspec(dllexport)
#else
#define NDIOPERATOR_API __declspec(dllimport)
#endif

#define EmptyAlpha 99999

/*****************************************************************
						Routine Definitions
*****************************************************************/
namespace NDIOPERATOR
{
	//��������ɼ�ͼ��ṹ��
	class NDIOPERATOR_API Attitude
	{
	public:
		Attitude();
		virtual ~Attitude();

	public:
		void GetPosition(float &t_fX, float &t_fY, float &t_fZ,
						 float &t_fAlpha, float &t_fBeta, float &t_fGama);	//��ȡ����

		void SetPosition(float t_fX, float t_fY, float t_fZ,
						 float t_fAlpha, float t_fBeta, float t_fGama);	//��������
		
		//���������
		Attitude operator+(const Attitude& b);			
		Attitude operator-(const Attitude& b);
		Attitude operator/(const float& b);

		bool Empty();		//�ж��Ƿ�Ϊ�գ�û�г�ʼ����

	private:
		float m_fX;			//ƽ��
		float m_fY;
		float m_fZ;

		float m_fAlpha;		//��X��Ƕ���ת
		float m_fBeta;		//��Y��Ƕ���ת
		float m_fGama;		//��Z��Ƕ���ת
	};
}