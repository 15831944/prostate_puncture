#pragma once
#include "SurgicalPlan.h"

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
	//���յ�ͷ����MRI/Mask�����ݽ���ģ��������õ���������Ͷ�Ӧ��������
//********������������ΪWLD����ϵ�£������˷ֱ��ʣ���λΪmm���ڲ�ת��ΪIJK����ϵ***
	class SURGICALPLAN_API ImageSampler
	{
	public:
		ImageSampler();
		virtual ~ImageSampler();

		BOOL SetDataPackage(SurgicalPlanPtr pPackage);
		BOOL SetPlaneDir(double nx, double ny, double nz);//����B��ƽ��ķ��������������up����
		BOOL SetMoveDir(double dx, double dy, double dz); //����B��̽ͷ�˶��ķ��򣨼�ǰ��at�ķ���
		BOOL SetCenterPt(double x, double y, double z); //����B��̽ͷ������е�����λ�ã���ͼ��bottom�ߵ����ĵ㣩����λΪmm

		void GetPlaneDir(double &nx, double &ny, double &nz);
		void GetMoveDir(double &dx, double &dy, double &dz);
		void GetCenterPt(double &x, double &y, double &z);

		void GetSampleMaskPlan(MaskDataType *pImage, int cx, int cy, bool type);//��ȡ����ͼ�� true-��״�棬falseʸ״��
		void SetImageResolution(double resolution) { m_fImageRes = resolution; }

	private:
		// �������� ������������Ƭ��ͼ������
		// ��ڲ��� ��pImage �������ָ�룬�ĸ������ڱ�׼ͼ������ϵ�µ�λ�ã���Ƭͼ��Ĵ�С
		// ������ĸ���Ϊͼ������ϵ(IJK)�µ�����
		void GetMaskPlanData(MaskDataType *pImage, Point3D lefttop, Point3D righttop, Point3D leftbottom, Point3D rightbottom, int cx, int cy);
		//�������������������һ�����ĵ㣬����ü�����ĸ��ǵ�����
		void CalculateCoronalMaskPlan();
		void CalculateSagittalMaskPlan();

		Point3D IJKToWLD(Point3D pt, Point3D Res);
		Point3D WLDToIJK(Point3D pt, Point3D Res);

	private:
		SurgicalPlanPtr m_pDataPackage; //ָ�����ݵ�ָ��

		Point3D m_UpDir; //B��ƽ��ķ��������������up����
		Point3D m_MoveDir; //B��̽ͷ�˶��ķ��򣨼�ǰ��at�ķ���
		Point3D m_ScanCenter; //B��̽ͷ������е�����λ��

		Point3D   m_LeftTop, m_RightTop, m_LeftBottom, m_RightBottom;  //����Ƭ�����������ά�ռ��λ��

		ImageSize m_iSize; //��ȡ��ͼ��ƽ��ߴ�
		double  m_fImageRes; //��ȡͼ��ķֱ���
	};

	typedef std::shared_ptr<ImageSampler> ImageSamplerPtr;

}
