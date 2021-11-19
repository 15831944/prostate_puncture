//*****************************************************************
// �ļ��� :						RigidTransform.h
// �汾	 :						1.0
// Ŀ�ļ���Ҫ���� :				�������任
// �������� :					2021.11.05
// �޸����� :					2021.11.05
// ���� :						SunYizhe
// �޸��� :						
// ��ϵ��ʽ :					fiki@seu.edu.cn
//*****************************************************************/
#pragma once

/*****************************************************************
Library Files Included
*****************************************************************/
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vector>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include "ComUtility/Attitude.h"
#include "ComUtility/Coordinate.h"
#include "ComUtility/Matrix.h"

using namespace fsutility;
using namespace std;

/*****************************************************************
Defines
*****************************************************************/
#ifdef ANALYSEPROCESS_EXPORT
#define ANALYSEPROCESS_API _declspec(dllexport)
#else
#define ANALYSEPROCESS_API _declspec(dllimport)
#endif


/*****************************************************************
						Routine Definitions
�������任����RigidTransform��
	i.	�ɸ���һ���Ӧ��λ�ˣ�ֱ����ֵ�������任����
	ii.	����һ���Ӧ�ĵ��ƣ���ʹ��vtk��װ��icp�㷨�����������任����
	iii.��������������ݶ����ڣ�����i������ĸ���任���������� ʹ��icp�㷨�����Ż�
	iv.	һ��ģ�sourceָUS��dstinationָMRI
*****************************************************************/
namespace ANALYSEPROCESS
{
	//λ�ô�����
	class RigidTransform;
	typedef std::shared_ptr <RigidTransform> RigidTransformPtr;

	class ANALYSEPROCESS_API RigidTransform
	{
	public:
		RigidTransform();
		~RigidTransform();

		void SetSrcAttitude(const fsutility::Attitude& attitude);
		void SetDstAttitude(const fsutility::Attitude& attitude);
		void SetSrcPts(const vector<fsutility::Coordinate>& pts);
		void SetDstPts(const vector<fsutility::Coordinate>& pts);
		void Reset();  //������������������
		fsutility::Matrix CalTransformMatrix();  //�����������ݣ��������任����
	public:
	private:
		void Coordinates2Polydata(vector<Coordinate>& src, vtkSmartPointer<vtkPolyData> res);  //��Coordinate��ʽ�ĵ㼯��תΪvtkPolyData��������
		void VtkMatrix2Matrix(vtkSmartPointer<vtkMatrix4x4> src, Matrix & dst);  //��vtk�����4*4���󣬸�ֵ������Ŀ�����4*4����
		void Matrix2VtkMatrix(Matrix & src, vtkSmartPointer<vtkMatrix4x4> dst);  //������Ŀ�����4*4���󣬸�ֵ��vtk�����4*4����
	private:
		bool m_bHasSrcAttitude;  //Դλ�˲����Ƿ�׼������ ��־λ
		bool m_bHasDstAttitude;  //Ŀ��λ�˲����Ƿ�׼������ ��־λ

		// һ���Ӧ��λ�ˣ�������ֱ�Ӽ������任����
		fsutility::Attitude m_SrcAttitude;  
		fsutility::Attitude m_DstAttitude;  

		//һ���Ӧ�ĵ��ƣ��ɲ���icp�㷨�����������任����
		vector<fsutility::Coordinate> m_SrcPts; 
		vector<fsutility::Coordinate> m_DstPts;
	};
}


