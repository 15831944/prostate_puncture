#pragma once
//matrix.h
//矩阵运算类，以4×4矩阵形式表示空间变换
//旋转、平移运算的复合，先旋转、后平移
//对4维齐次坐标进行运算

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
		Matrix(double m[16]);	//矩阵初始化(先行后列)
		Matrix(Coordinate c0, Coordinate c1, Coordinate c2, Coordinate c3);	//四个列向量构造一个矩阵
		void SetValue(double m[16]);	//设置矩阵值(先行后列)
		void SetValue(Coordinate c0, Coordinate c1, Coordinate c2, Coordinate c3);//四个列向量为矩阵赋值
		double GetValue(int y, int x);
		~Matrix();
		Coordinate GetMultiply(Coordinate c);	//this矩阵×齐次坐标
		Matrix GetMultiply(Matrix m);	//this矩阵×矩阵
		Matrix GetInverse();			//求逆矩阵
		double GetDeterminant();		//求this矩阵行列式
		Matrix GetTranspose();			//求this矩阵的转置矩阵
		void ConstructEulerTransform(double x, double y, double z, double roll, double pitch, double yaw);		//根据欧拉角形式的旋转参数，求变换矩阵
		void ConstructQuaternionTransform(double q0, double qx, double qy, double qz, double tx, double ty, double tz);	//根据四元数 构造变换矩阵
	private:
		double m_dItem[4][4];
		double GetMinor(int y, int x);	//求this矩阵在y,x处的余子式
		double GetCofactor(int y, int x);	//求this矩阵在y,x处的代数余子式
	};

}

