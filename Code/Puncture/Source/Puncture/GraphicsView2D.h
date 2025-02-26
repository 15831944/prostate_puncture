#pragma once
#include <qgraphicsview.h>
#include <qgraphicsitem.h>
#include <Qt3DInput/qkeyevent.h>
#include "opencv2/opencv.hpp"

class GraphicsView2D :
	public QGraphicsView
{
	Q_OBJECT
public:
	GraphicsView2D(QWidget *parent = Q_NULLPTR);
	~GraphicsView2D();

	void keyPressEvent(QKeyEvent * event);

	void LoadImg(cv::Mat img);
	void LoadProstateMask(cv::Mat prostateMask);  //读取前列腺mask并叠加
	void LoadLesionMask(cv::Mat lesionMask);  //读取肿瘤mask并叠加
	void LoadRectumMask(cv::Mat rectumMask);  //读取直肠mask并叠加
	void ShowImg();  //更新图元 显示图像
	cv::Mat Mask2Edge(cv::Mat src);

signals:
	void KeyLeftPressed();
	void KeyRightPressed();
	void KeyUpPressed();
	void KeyDownPressed();

private:
	void createGamaLut(uchar lutGama[256], float fC, float fGama);  //构造gama变换查找表
	cv::Mat transformImg(cv::Mat& srcImg);  //使用查找表进行变换

private:
	cv::Mat m_img;  //图像数据总是存储在此
	cv::Mat m_prostateMask;  //前列腺mask 前景区域为1(下同)
	cv::Mat m_lesionMask;
	cv::Mat m_rectumMask;
	uchar m_lut[256];  //超声图像查找表
	QGraphicsScene* m_pScene;
	QPixmap m_pixmap;
	QGraphicsPixmapItem * m_pPixmapItem;
};

