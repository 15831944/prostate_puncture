#pragma once
#include "Opencv2/opencv.hpp"
#include "TypeDefineHeader.h"

namespace ImageRender
{
	class AFX_EXT_CLASS ImageRenderActor
	{
	public:
		ImageRenderActor();
		virtual ~ImageRenderActor(void);

	public:
		//����ͼ��
		virtual void Render(Graphics* pGraphics, int x, int y, int cx, int cy);

		//virtual void LoadImageFile(CString strFilePath);
		virtual void LoadImageFile(cv::Mat &SrcData);

		virtual void CreateGrayImageRender(int cx ,int cy);
		virtual void CreateRGBImageRender(int cx ,int cy);
		
		virtual ThBitmapPtr GetSliceBitmap();
	public:
		
		//ͼ�����ź�ƽ��
		virtual float GetZoom();
		virtual BOOL  SetZoom(float fZoom);
		virtual void  Zoom(float fZoom);
		virtual void  OffsetImage(float dx, float dy); //���ƫ����

		virtual void  GetOffset(float &fx, float &fy);
		virtual BOOL  SetOffset(float fx, float fy);

		virtual void  ResetZoomAndOffset();
		virtual RectF GetImageRect();
		virtual void  SetImageRect(RectF imageRect);

		//��Ļ�����ͼ������ת������Ҫͼ�����š�ƽ�ƣ��Լ�ͼ��ʹ��ڵȱ�������
		virtual void  ScreenToImage(double &x, double &y);
		virtual void  ImageToScreen(double &x, double &y);

		void  EnableHighQualityRender(BOOL bHighQuality);

	protected:
		//����ͼ�����λ��
		virtual void  CalculateRenderPosition(int x, int y, int cx, int cy);
		static void Mat2Bitmap(cv::Mat &m, Gdiplus::Bitmap *pBitmap);
	protected:
		//Bitmap for render to window;
		ThBitmapPtr m_SliceBitmap;

		float m_fZoom;
		float m_fxoffset;
		float m_fyoffset;

		RectF m_ImageRect;
		BOOL   m_bHighQualityRender;
		bool   m_bModifyImage;
	};
	
}