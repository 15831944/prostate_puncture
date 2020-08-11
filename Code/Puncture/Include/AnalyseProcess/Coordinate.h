#pragma once
//Coordinate.h
//���������
//w=0�����ʾ����(x,y,z)
//w!=0,���ʾ��(x/w,y/w,z/w)��wһ��ȡ1

#include <math.h>

/*****************************************************************
Defines
*****************************************************************/
#ifdef ANALYSEPROCESS_EXPORT
#define ANALYSEPROCESS_API _declspec(dllexport)
#else
#define ANALYSEPROCESS_API _declspec(dllimport)
#endif

namespace ANALYSEPROCESS
{
	class Coordinate
	{
	public:
		double x;
		double y;
		double z;
		double w;

		Coordinate();
		Coordinate(double x, double y, double z, double w);
		~Coordinate();

		//��������أ�����������ļӼ�����
		//��Ӧ���е��������ļӼ������ǳ����в������
		Coordinate operator+(Coordinate c);
		Coordinate operator-(Coordinate c);
		
		Coordinate operator*(const double &c);	//���������������

		void Normalize();	//�����w��Ϊ1 ��������λ��
		double GetDotProduct(Coordinate c);	//�������
		Coordinate GetCrossProduct(Coordinate c);	//�������

	private:
	};
}

