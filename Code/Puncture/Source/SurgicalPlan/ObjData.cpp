#include "stdafx.h"
#include "ObjData.h"
#include <fstream>

using namespace SURGICALPLAN;

ObjData::ObjData()
{
	m_pVbo = nullptr;
	m_pEbo = nullptr;
	m_BoundBox.x1 = m_BoundBox.y1 = m_BoundBox.z1 = 99999999;
	m_BoundBox.x2 = m_BoundBox.y2 = m_BoundBox.z2 = -1;
}


ObjData::~ObjData()
{
	if(m_pVbo!=nullptr)
		delete[] m_pVbo;
	if (m_pEbo != nullptr)
		delete[] m_pEbo;
}

BOOL ObjData::LoadObjFile(CString strFileName)
{
	int temp = 0;
	int i,j;
	char cType;  //��� c-���� f-������Ƭ
	float x, y, z;
	std::fstream file;
	file.open(strFileName.GetBuffer());

	//��ȡ��������
	file >> m_nVertexNum;
	if (m_nVertexNum <= 0)
		return ER_ImportObjDataError;
	//m_pVbo = new float[m_nVertexNum * 3];
	m_vertices = std::vector<Coordinate>(m_nVertexNum);
	for (i = 0; i < m_nVertexNum; i++)
	{
		file >> cType;
		if (cType == 'v')
		{
			file >> m_vertices[i].x >> m_vertices[i].y >> m_vertices[i].z;
			m_vertices[i].w = 1;  //w=1����������ǵ�����
			//ͳ��boundbox
			m_BoundBox.x1 = min(m_BoundBox.x1, m_vertices[i].x);
			m_BoundBox.x2 = max(m_BoundBox.x2, m_vertices[i].x);
			m_BoundBox.y1 = min(m_BoundBox.y1, m_vertices[i].y);
			m_BoundBox.y2 = max(m_BoundBox.y2, m_vertices[i].y);
			m_BoundBox.z1 = min(m_BoundBox.z1, m_vertices[i].z);
			m_BoundBox.z2 = max(m_BoundBox.z2, m_vertices[i].z);
		}
	}

	//��ȡ������Ƭ�±�
	file >> m_nTriangleNum;
	if (m_nTriangleNum <= 0)
		return ER_ImportObjDataError;
	m_indices = std::vector<std::vector<int>>(m_nTriangleNum, std::vector<int>(3));  //(m_nTriangleNum*3)������
	for (i = 0; i < m_nTriangleNum; i++)
	{
		file >> cType;
		if (cType == 'f')
		{
			file >> m_indices[i][0] >> m_indices[i][1] >> m_indices[i][2];
		}
	}
	for (i = 0; i < m_nTriangleNum; i++)
	{
		for (j = 0; j < 3; j++)
		{
			m_indices[i][j] -= 1;  //opengl�涨�����±��0��ʼ������obj�ļ��������±��1��ʼ���ʽ�������
		}
	}
	
	//���㷨��
	this->CalculateNormal();
	return 0;
}

void SURGICALPLAN::ObjData::GetBoundBox(float & x1, float & x2, float & y1, float & y2, float & z1, float & z2)
{
	x1 = m_BoundBox.x1;
	x2 = m_BoundBox.x2;
	y1 = m_BoundBox.y1;
	y2 = m_BoundBox.y2;
	z1 = m_BoundBox.z1;
	z2 = m_BoundBox.z2;
}

std::vector<Coordinate> SURGICALPLAN::ObjData::GetVertices()
{
	return m_vertices;
}

float* ObjData::GetVboArray()
{
	//������λ�á����㷨�������ݴ洢��float������
	if (m_pVbo == nullptr)
	{
		int vertexIndex;
		m_pVbo = new float[m_nVertexNum*6];  //����λ�� float*3 ���㷨���� float*3
		for (vertexIndex = 0; vertexIndex < m_nVertexNum; vertexIndex++)
		{
			//����λ������
			m_pVbo[vertexIndex * 6 + 0] = float(m_vertices[vertexIndex].x);
			m_pVbo[vertexIndex * 6 + 1] = float(m_vertices[vertexIndex].y);
			m_pVbo[vertexIndex * 6 + 2] = float(m_vertices[vertexIndex].z);
			//���㷨������
			m_pVbo[vertexIndex * 6 + 3] = float(m_normal[vertexIndex].x);
			m_pVbo[vertexIndex * 6 + 4] = float(m_normal[vertexIndex].y);
			m_pVbo[vertexIndex * 6 + 5] = float(m_normal[vertexIndex].z);
		}
	}
	return m_pVbo;
}

unsigned int* ObjData::GetEboArray()
{
	//��������Ƭ�Ķ����±����� �洢��unsigned int������
	if (m_pEbo == nullptr)
	{
		int triIndex;
		m_pEbo = new unsigned int[m_nTriangleNum*3];
		for (triIndex = 0; triIndex < m_nTriangleNum; triIndex++)
		{
			m_pEbo[triIndex*3 + 0] = unsigned int(m_indices[triIndex][0]);
			m_pEbo[triIndex*3 + 1] = unsigned int(m_indices[triIndex][1]);
			m_pEbo[triIndex*3 + 2] = unsigned int(m_indices[triIndex][2]);
		}
	}
	return m_pEbo;
}

void ObjData::CalculateNormal()
{
	int triIndex,vertexIndex;
	Coordinate ptA, ptB, ptC;  //��ǰ������Ƭ��3������
	Coordinate vAB, vBC;  //��ǰ������Ƭ��AB,BC����
	Coordinate vNormal;  //��ǰ��Ƭ�ķ�����
	m_normal = std::vector<Coordinate>(m_nVertexNum,Coordinate(0,0,0,0));  //Ĭ�ϳ�ʼ��Ϊȫ0����
	//ÿ������ķ�����Ϊ��������ķ�����֮��
	for (triIndex = 0; triIndex < m_nTriangleNum; triIndex++)
	{
		ptA = m_vertices[m_indices[triIndex][0]];
		ptB = m_vertices[m_indices[triIndex][1]];
		ptC = m_vertices[m_indices[triIndex][2]];
		vAB = ptB - ptA;
		vBC = ptC - ptB;
		vNormal = vAB.GetCrossProduct(vBC);  //��������������Ƭ�������߲�˻��
		m_normal[m_indices[triIndex][0]] = m_normal[m_indices[triIndex][0]] + vNormal;  //Ϊ������Ƭ��ÿ������ ���Ӹ�������ķ�����
		m_normal[m_indices[triIndex][1]] = m_normal[m_indices[triIndex][1]] + vNormal;
		m_normal[m_indices[triIndex][2]] = m_normal[m_indices[triIndex][2]] + vNormal;
	}
	//��������һ��
	for (vertexIndex = 0; vertexIndex < m_nVertexNum; vertexIndex++)
	{
		m_normal[vertexIndex].Normalize();
	}
}
