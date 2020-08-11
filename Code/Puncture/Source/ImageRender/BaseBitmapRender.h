#pragma once
#include "TypeDefineHeader.h"
//�ڴ�bitmap��render ���Ȱ����ݻ��Ƶ���bitmap��,Ȼ���ٰ�Bitmap����DIB����

using namespace Gdiplus;
namespace ImageRender
{
	class AFX_EXT_CLASS BaseBitmapRender
	{
	public:
		BaseBitmapRender(void);
		BaseBitmapRender(int cx, int cy);
		virtual ~BaseBitmapRender(void);

		virtual BOOL  CreateBitmapRender(int cx, int cy);

		//ֻ�е����˸ú���������ʹ��RenderToWnd��������Ƴ����Ŀհ�ͼ��
		virtual BOOL  FlushBitmapToDib();

		//Call next in Wnd's OnPaint() function
		virtual BOOL  RenderToWnd(CDC *pDC, int x=0, int y=0);
		virtual BOOL  RenderToWnd(CDC *pDC, CRect dstRect);

		virtual BOOL  DrawTransparent(CDC * pDC, int x=0, int y=0);
		virtual BOOL  DrawTransparent(CDC *pDC, CRect dstRect);

		//save render result to file
		virtual BOOL  SaveToBmpFile(CString strFileName);
		virtual BOOL  SaveTojppFile(CString strFileName);

		//ʹ�ñ���ɫ����Bitmap����
		virtual void RendBackgroundImage();

	public:
		Graphics* GetRenderGraphic();
		Bitmap*   GetBitmap(); 


	protected:
		//������Bitmap�����С��ͬ��DIB���������OnPaint�����ǵĻ����ٶ�
		virtual BOOL CreateDib(int w, int h, OUT HBITMAP& hBmpSection, OUT BYTE** bmpData);

		//��Bitmap�������ݸ��µ�DIB�����У����ڵ�OnPaint����ֱ��ʹ��DIB������л���
		virtual void FlushToDib(IN Bitmap* pMemBitmap, IN OUT BYTE** bmpData);

		virtual void MaskColor(COLORREF color);

		virtual void ClearDIBResource();

	protected:
		CRect m_FullRegion;
		int   m_nCX;
		int   m_nCY;

		ThBitmapPtr m_MemBitmap;
		ThGraphicsPtr m_MemGraphics;

		//DIB ����ͼ�����
		HBITMAP    m_hbitmap;       // bitmap handle
		BITMAPINFO m_bmi;           // bitmap header
		VOID       *m_pvBits;       // pointer to DIB section
		CImage	   m_Image;

		COLORREF   m_MaskColor;
		BOOL       m_bEnableMaskColor;
	};

}

