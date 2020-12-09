#pragma once
/*****************************************************************
Defines
*****************************************************************/
#ifdef SURGICALPLAN_EXPORT
#define SURGICALPLAN_API __declspec(dllexport)
#else
#define SURGICALPLAN_API __declspec(dllimport)
#endif

#include <atlstr.h>
#include <string>
#include <vector>
#include "ErrorManager//ErrorCodeDefine.h"
#include "StructDef.h"
#include <ComUtility/Coordinate.h>

using namespace fsutility;

namespace SURGICALPLAN
{
	//class Coordinate;  //��Coordinate��ǰ�������������ø����ͷ�ļ�����Ϊ�˱���ͷ�ļ��е�AFX_EXT_CLASS��������Ŀ���޷�������

	class SURGICALPLAN_API ObjData
	{
	public:
		ObjData();
		~ObjData();

		BOOL LoadObjFile(CString strFileName);
		int GetVertexNum() { return m_nVertexNum; }
		int GetTriangleNum() { return m_nTriangleNum; }
		void GetBoundBox(float &x1, float &x2, float &y1, float &y2, float &z1, float &z2);
		float* GetVboArray();  //��float�������ʽ������vbo����(ÿ���������3*float ����x��y��z����;3*float ���㷨����)
		unsigned int* GetEboArray(); //��unsigned int�������ʽ ����ebo����

	private:
		void CalculateNormal();  //���·�������
	private:
		int m_nVertexNum;  //��������
		int m_nTriangleNum;  //������Ƭ����
		std::vector<Coordinate> m_vertices;  //ģ�Ͷ���
		float* m_pVbo;  //���㻺����� vbo���� (����=��������*3������+����������*3������)
		std::vector<Coordinate> m_normal;  //��������ķ�������
		std::vector<std::vector<int>> m_indices;  //ģ���±�
		unsigned int* m_pEbo;  //����������� ebo���� (����=������Ƭ��������*3���±�)

		ThreeD_BOUND_BOX m_BoundBox;
	};
	typedef std::shared_ptr<ObjData> ObjDataPtr;
}


