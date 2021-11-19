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
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLandmarkTransform.h>
#include <vtkLine.h>
#include <vtkMath.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkVertexGlyphFilter.h>
#include "pch.h"
#include "RigidTransform.h"
#include "ErrorManager/ErrorCodeDefine.h"

using namespace ANALYSEPROCESS;

///////////////////////////////////////////////////////
////////////////////////define/////////////////////////
///////////////////////////////////////////////////////
#define MAX_ITERATION 30
/*****************************************************************
Global Variables
*****************************************************************/

/*****************************************************************
Function
*****************************************************************/
#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

RigidTransform::RigidTransform()
{
	m_bHasSrcAttitude = false;
	m_bHasDstAttitude = false;
}


RigidTransform::~RigidTransform()
{
}

void ANALYSEPROCESS::RigidTransform::SetSrcAttitude(const fsutility::Attitude & attitude)
{
	this->m_SrcAttitude = attitude;
	this->m_bHasSrcAttitude = true;
}

void ANALYSEPROCESS::RigidTransform::SetDstAttitude(const fsutility::Attitude & attitude)
{
	this->m_DstAttitude = attitude;
	this->m_bHasDstAttitude = true;
}

void ANALYSEPROCESS::RigidTransform::SetSrcPts(const vector<fsutility::Coordinate>& pts)
{
	this->m_SrcPts = pts;
}

void ANALYSEPROCESS::RigidTransform::SetDstPts(const vector<fsutility::Coordinate>& pts)
{
	this->m_DstPts = pts;
}

void ANALYSEPROCESS::RigidTransform::Reset()
{
	this->m_bHasSrcAttitude = false;
	this->m_bHasDstAttitude = false;
	m_SrcPts.clear();
	m_DstPts.clear();
}

fsutility::Matrix ANALYSEPROCESS::RigidTransform::CalTransformMatrix()
{
	double defaultElems[16]{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	fsutility::Matrix defaultTransform=fsutility::Matrix(defaultElems);  //��������� �����λ����
	fsutility::Matrix result = defaultTransform;
	vtkNew<vtkPolyData> src;
	vtkNew<vtkPolyData> dst;
	vtkNew<vtkIterativeClosestPointTransform> icp;

	//����һ���Ӧ��λ�ˣ�ֱ�Ӽ������任����
	if (m_bHasSrcAttitude && m_bHasDstAttitude) 
	{
		Matrix srcPosition(m_SrcAttitude.m_ScanCenter, m_SrcAttitude.m_RightDir, m_SrcAttitude.m_UpDir, m_SrcAttitude.m_MoveDir);
		Matrix dstPosition(m_DstAttitude.m_ScanCenter, m_DstAttitude.m_RightDir, m_DstAttitude.m_UpDir, m_DstAttitude.m_MoveDir);
		if(abs(srcPosition.GetDeterminant())!=0)  //����ʽΪ0����������£��޷����������
			result = dstPosition.GetMultiply(srcPosition.GetInverse());
	}
	//����һ���Ӧ�ĵ��ƣ�ʹ��icp�㷨�����������任����
	if (m_SrcPts.size() > 4 && m_DstPts.size() > 4)
	{
		Coordinates2Polydata(m_SrcPts, src);
		Coordinates2Polydata(m_DstPts, dst);
		icp->SetSource(src);
		icp->SetTarget(dst);
		icp->GetLandmarkTransform()->SetModeToRigidBody();  //����Ϊ����任
		icp->SetMaximumNumberOfIterations(MAX_ITERATION);  //���õ�������
		//��λ�˼�����ı任���� ��Ϊicp�㷨�ĳ�ʼֵ
		if (m_bHasSrcAttitude && m_bHasDstAttitude)
		{
			vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();  //��ȡָ��icp��������ָ�룬���ڸ�ֵ����Ԫ��(�ڲ�Ĭ�ϵ���һ��icp->update() ��Ϊû��Ҫ�Ĳ��� ���޷�����)
			std::cout << "The resulting matrix is: " << *m << std::endl;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
					m->SetElement(y, x, result.GetValue(y, x));
		}
		icp->Modified();  //���±�������޸�ʱ��(�ڱ�����ʵ�����ò���)
		icp->Update();
		vtkSmartPointer<vtkMatrix4x4> n = icp->GetMatrix();
		VtkMatrix2Matrix(n, result);
	}
	return result;
}

// ��Coordinate��ʽ�ĵ㼯��תΪvtkPolyData��������
void ANALYSEPROCESS::RigidTransform::Coordinates2Polydata(vector<Coordinate>& src, vtkSmartPointer<vtkPolyData> res)
{
	int i, j;
	float pt[3];

	//�ֱ���Ӹ�����
	vtkNew<vtkPoints> points;
	for (i = 0; i < src.size(); i++) {
		pt[0] = src[i].x;
		pt[1] = src[i].y;
		pt[2] = src[i].z;
		points->InsertNextPoint(pt);
	}
	//����ģ��
	vtkNew<vtkPolyData> obj;
	obj->SetPoints(points);
	//�㼯ת��Ϊ����
	vtkNew<vtkVertexGlyphFilter> vertexFilter;
	vertexFilter->SetInputData(obj);
	vertexFilter->Update();
	res->ShallowCopy(vertexFilter->GetOutput());
}

// ��vtk�����4*4���󣬸�ֵ������Ŀ�����4*4����
void ANALYSEPROCESS::RigidTransform::VtkMatrix2Matrix(vtkSmartPointer<vtkMatrix4x4> src, Matrix & dst)
{
	int y, x;
	double elememts[16];
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			elememts[y * 4 + x] = src->GetElement(y, x);
	dst.SetValue(elememts);
}

// ������Ŀ�����4*4���󣬸�ֵ��vtk�����4*4����
void ANALYSEPROCESS::RigidTransform::Matrix2VtkMatrix(Matrix & src, vtkSmartPointer<vtkMatrix4x4> dst)
{
	int y, x;
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			dst->SetElement(y, x, src.GetValue(y, x));
}
