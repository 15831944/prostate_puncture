#pragma once
#include "StructDef.h"

/*****************************************************************
Defines
*****************************************************************/
#ifdef SURGICALPLAN_EXPORT
#define SURGICALPLAN_API __declspec(dllexport)
#else
#define SURGICALPLAN_API __declspec(dllimport)
#endif

namespace SURGICALPLAN
{
	class SURGICALPLAN_API SurfaceData
	{
	public:
		SurfaceData();
		virtual ~SurfaceData();

		void GetBoundBox(float &x1, float &x2, float &y1, float &y2, float &z1, float &z2);
		void SetBoundBox(float x1, float x2, float y1, float y2, float z1, float z2);

		void SetSamplePlan(Point3D lefttop, Point3D righttop, Point3D leftbottom, Point3D rightbottom);
		Point3D * GetSamplePlan();

	protected:
		ThreeD_BOUNDBOX m_BoundBox;
		//��������
		//��Ƭ�����������ά�ռ��λ��
		Point3D m_pPlanPts[4]; //��Ƭ�������� GL_QUADS

	};

	typedef std::shared_ptr<SurfaceData> SurfaceDataPtr;

}