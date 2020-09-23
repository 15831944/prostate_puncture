#pragma once
//matrix.h
//���������࣬��4��4������ʽ��ʾ�ռ�任
//��ת��ƽ������ĸ��ϣ�����ת����ƽ��
//��4ά��������������

//#include "Point3D.h"
#include "Coordinate.h"

/*****************************************************************
Defines
*****************************************************************/
//#ifdef ANALYSEPROCESS_EXPORT
//#define ANALYSEPROCESS_API _declspec(dllexport)
//#else
//#define ANALYSEPROCESS_API _declspec(dllimport)
//#endif

namespace fsutility
{
	class AFX_EXT_CLASS Matrix
	{
	public:
		Matrix();
		Matrix(double m[16]);	//�����ʼ��(���к���)
		Matrix(Coordinate c0, Coordinate c1, Coordinate c2, Coordinate c3);	//�ĸ�����������һ������
		void SetValue(double m[16]);	//���þ���ֵ(���к���)
		void SetValue(Coordinate c0, Coordinate c1, Coordinate c2, Coordinate c3);//�ĸ�������Ϊ����ֵ
		~Matrix();
		Coordinate GetMultiply(Coordinate c);	//this������������
		Matrix GetMultiply(Matrix m);	//this���������
		Matrix GetInverse();	//�������
		double GetDeterminant();	//��this��������ʽ
		Matrix GetTranspose();		//��this�����ת�þ���
	private:
		double m_dItem[4][4];
		double GetMinor(int y, int x);	//��this������y,x��������ʽ
		double GetCofactor(int y, int x);	//��this������y,x���Ĵ�������ʽ
	};

}

