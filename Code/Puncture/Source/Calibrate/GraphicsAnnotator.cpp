#include "GraphicsAnnotator.h"
#include <vector>
#pragma execution_character_set("utf-8")  //������������ �ο�https://blog.csdn.net/liyuanbhu/article/details/72596952
//#include <Windows.h>


GraphicsAnnotator::GraphicsAnnotator(QWidget *parent)
{
	this->setParent(parent);
	m_pScene = new QGraphicsScene(this);
	m_pPixmapItem = new QGraphicsPixmapItem(m_pixmap);  //ͼƬͼԪ
	m_pScene->addItem(m_pPixmapItem);
	this->setScene(m_pScene);

	createGamaLut(m_lut, 1.0, 0.5);  //��ʼ��gamaУ������
}

GraphicsAnnotator::~GraphicsAnnotator()
{
	delete m_pPixmapItem;
	delete m_pScene;
}

void GraphicsAnnotator::keyPressEvent(QKeyEvent * event)
{
	switch (event->key())
	{
	//case Qt::Key_Left:
	//	emit(KeyLeftPressed());
	//	break;
	//case Qt::Key_Right:
	//	emit(KeyRightPressed());
	//	break;
	//case Qt::Key_Up:
	//	emit(KeyUpPressed());
	//	break;
	//case Qt::Key_Down:
	//	emit(KeyDownPressed());
	//	break;
	case Qt::Key_Delete:
		this->DeleteLastPoint();
		break;
	case Qt::Key_Backspace:
		this->DeleteLastPoint();
		break;
	default:
		break;
	}
}

//����8UC3 BGR��cv::Mat��ʽ������
void GraphicsAnnotator::LoadImg(cv::Mat img)
{
	img.copyTo(m_img);

	//ͼƬ����
	cv::Mat m_imgGray;  //�Ҷ�ͼ �м����
	cv::cvtColor(m_img, m_imgGray, cv::COLOR_BGR2GRAY);
	cv::threshold(m_imgGray, m_imgGray, 1, 255, cv::THRESH_BINARY_INV);  //ɸѡ���Ҷ�<=1�Ĳ���,����Ϊ255
	m_img.setTo(0, m_imgGray);  //���Ҷ�<=1�Ĳ��� ��Ϊ0
	this->transformImg(m_img);  //ʹ�ò��ұ�任ͼƬ ʹͼ������׶�
}

//��8UC3 BGR��cv::Mat��ʽ������m_img,תΪQPixmap�洢����ʾ
void GraphicsAnnotator::ShowImg()
{
	cv::Mat convertedImg;
	cv::cvtColor(m_img, convertedImg, cv::COLOR_BGR2RGB);
	QImage tempImg((const unsigned char*)(convertedImg.data), convertedImg.cols, convertedImg.rows, convertedImg.step, QImage::Format_RGB888);
	m_pixmap = QPixmap::fromImage(tempImg);
	m_pPixmapItem->setPixmap(m_pixmap);
	this->setSceneRect(0, 0, m_pixmap.width(), m_pixmap.height());
	this->fitInView(m_pPixmapItem, Qt::KeepAspectRatio);
	this->centerOn(m_pPixmapItem);
	//���ñ�ע����
	m_pointPos.clear();  //��ע�������
	for (int i = 0; i < m_pointItem.size(); i++)  //��ձ�עͼԪ
	{
		m_pScene->removeItem(m_pointItem[i]);
		delete m_pointItem[i];
	}
	m_pointItem.clear();
}

cv::Mat GraphicsAnnotator::Mask2Edge(cv::Mat src)
{
	int ddepth = CV_16S;
	int scale = 1, delta = 0;
	cv::Mat res, grad_x, grad_y, grad;
	//��ˮƽ ��ֱ�����ݶ�
	Sobel(src, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	Sobel(src, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	//�ݶ�תΪ����ֵ
	convertScaleAbs(grad_x, grad_x);
	convertScaleAbs(grad_y, grad_y);
	//�ϲ��ݶ�
	addWeighted(grad_x, 1, grad_y, 1, 0, grad);
	//��Եǿ����Ϊ1
	threshold(grad, res, 1, 1, 0);

	return res;
}

//gama�任 s=c*r^gama  ���� rΪԭʼ�Ҷ�ֵ cΪ�任��Ҷ�ֵ
void GraphicsAnnotator::createGamaLut(uchar lutGama[256], float fC, float fGama)
{
	memset(lutGama, 0, sizeof(uchar) * 2);  //�Ҷ�ֵ���͵ĵ� ֱ����Ϊ0
	for (int i = 2; i < 256; i++)
	{
		lutGama[i] = fC * uchar(pow(float(i) / 255, fGama) * 255) < 255 ? fC * uchar(pow(float(i) / 255, fGama) * 255) : 255;
	}
	return;
}

//ʹ�ò��ұ� ��cv::Mat��ֵ
//Mat������ʽ �ο�http://www.opencv.org.cn/opencvdoc/2.3.2/html/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html
//���η�ʽ �ο�https://blog.csdn.net/u012814856/article/details/84099328
cv::Mat GraphicsAnnotator::transformImg(cv::Mat& srcImg)
{
	//ȷ�������ͼƬ�����uchar��ͬ
	CV_Assert(srcImg.depth() != sizeof(uchar));

	int channels = srcImg.channels();
	int nRows = srcImg.rows;
	int nCols = srcImg.cols*channels;
	int x, y;
	uchar* pCur;

	if (srcImg.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	for (y = 0; y < nRows; y++)
	{
		pCur = srcImg.ptr<uchar>(y);  //��y����Ԫ��
		for (x = 0; x < nCols; x++)
		{
			pCur[x] = m_lut[pCur[x]];
		}
	}
	return srcImg;
}

void GraphicsAnnotator::DeleteLastPoint()
{
	int index = m_pointPos.size() - 1;
	if (index < 0)
		return;
	//ɾ�����һ��������
	m_pointPos.pop_back();
	//ɾ�����һ�����ʾ��ͼԪ
	m_pScene->removeItem(m_pointItem[index]);
	delete m_pointItem[index];
	m_pointItem.pop_back();
}

void GraphicsAnnotator::mouseReleaseEvent(QMouseEvent *event)
{
	//emit mouseMoveSignal(event->x(), event->y());
	QPoint curPos = event->pos();  //��ͼ����ϵ�µ�����
	QPointF scenePos = this->mapToScene(curPos);  //��������ϵ�µ�����
	QPointF itemPos = m_pPixmapItem->mapFromParent(scenePos);  //ͼԪ����ϵ�µ�
	emit mouseMoveSignal(itemPos.x(), itemPos.y());

	m_pointPos.push_back(itemPos);  //��¼��ǰ������
	//����һ��ʾ��ͼԪ
	QGraphicsEllipseItem *item = new QGraphicsEllipseItem(QRectF(0, 0, 12, 12));
	item->setBrush(QColor(255, 255, 0));
	m_pScene->addItem(item);
	item->setPos(scenePos.x()-6,scenePos.y()-6);  //������Բλ�ã�ͼԪ����ϵ�£�
	m_pointItem.push_back(item);

	QGraphicsView::mouseReleaseEvent(event);
}