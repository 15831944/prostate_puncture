#pragma once
//���ڱ�ע��ͼƬ��ʾwidget����չ
#include <qgraphicsview.h>
#include <qgraphicsitem.h>
#include <Qt3DInput/qkeyevent.h>
#include <vector>
#include "opencv2/opencv.hpp"
#define POINT_NUM 15  //��Ҫ��ע�Ķ��������
using namespace std;

class GraphicsAnnotator :
	public QGraphicsView
{
	Q_OBJECT
public:
	GraphicsAnnotator(QWidget *parent = Q_NULLPTR);
	~GraphicsAnnotator();

	void keyPressEvent(QKeyEvent * event);

	void LoadImg(cv::Mat img);
	void ShowImg();  //����ͼԪ ��ʾͼ��
	vector<QPointF> GetPointPos() { return m_pointPos; }
	cv::Mat Mask2Edge(cv::Mat src);

signals:
	void markFinishSignal();  //��ǵ�֡ͼƬ�Ѿ���ע���
//	void KeyLeftPressed();
//	void KeyRightPressed();
//	void KeyUpPressed();
//	void KeyDownPressed();

protected:
	void mouseReleaseEvent(QMouseEvent *event) override;  //������¼�


private:
	void createGamaLut(uchar lutGama[256], float fC, float fGama);  //����gama�任���ұ�
	cv::Mat transformImg(cv::Mat& srcImg);  //ʹ�ò��ұ���б任
	void DeleteLastPoint();  //ɾ�����һ��ѡ���ĵ�

private:
	cv::Mat m_img;  //ͼ���������Ǵ洢�ڴ�
	uchar m_lut[256];  //����ͼ��gama�任���ұ�
	QGraphicsScene* m_pScene;
	QPixmap m_pixmap;
	QGraphicsPixmapItem * m_pPixmapItem;
	vector<QPointF> m_pointPos;  //POINT_NUM�����x,y����(ͼԪ����ϵ)
	vector<QGraphicsEllipseItem*> m_pointItem;  //POINT_NUM���㣬��ͼԪ���б�ע
signals:
	void mouseMoveSignal(float x,float y);
};

