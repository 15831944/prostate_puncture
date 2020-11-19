#include "GraphicsView2D.h"
#include <vector>
#include <Windows.h>


GraphicsView2D::GraphicsView2D(QWidget *parent)
{
	this->setParent(parent);
	m_pScene = new QGraphicsScene(this);
	m_pPixmapItem = new QGraphicsPixmapItem(m_pixmap);
	m_pScene->addItem(m_pPixmapItem);
	this->setScene(m_pScene);

	createGamaLut(m_lut, 1.0, 0.5);
}

GraphicsView2D::~GraphicsView2D()
{
	delete m_pPixmapItem;
	delete m_pScene;
}

void GraphicsView2D::keyPressEvent(QKeyEvent * event)
{
	switch (event->key())
	{
	case Qt::Key_Left:
		emit(KeyLeftPressed());
		break;
	case Qt::Key_Right:
		emit(KeyRightPressed());
		break;
	case Qt::Key_Up:
		emit(KeyUpPressed());
		break;
	case Qt::Key_Down:
		emit(KeyDownPressed());
		break;
	default:
		break;
	}
}

//����8UC3 BGR��cv::Mat��ʽ������
void GraphicsView2D::LoadImg(cv::Mat img)
{	
	img.copyTo(m_img);

	//ͼƬ����
	cv::Mat m_imgGray;  //�Ҷ�ͼ �м����
	cv::cvtColor(m_img, m_imgGray, cv::COLOR_BGR2GRAY);
	cv::threshold(m_imgGray, m_imgGray, 1, 255, cv::THRESH_BINARY_INV);  //ɸѡ���Ҷ�<=1�Ĳ���,����Ϊ255
	m_img.setTo(0, m_imgGray);  //���Ҷ�<=1�Ĳ��� ��Ϊ0
	this->transformImg(m_img);  //ʹ�ò��ұ�任ͼƬ ʹͼ������׶�
	   
	//�洢ͼƬ����
	static int index = 0;
	std::string filename;
	filename = "D:\\other\\�����ͼ��\\" + std::to_string(index) + ".bmp";
	cv::imwrite(filename, m_img);
	Sleep(100);
	index++;
	//����
}

void GraphicsView2D::LoadProstateMask(cv::Mat prostateMask)
{
	if (prostateMask.empty())
		return;
	m_prostateMask = Mask2Edge(prostateMask.clone());
	cv::Mat prostatePalette(m_prostateMask.rows, m_prostateMask.cols, CV_8UC3, cv::Scalar(0, 255, 0));
	prostatePalette.copyTo(m_img, m_prostateMask);
}

void GraphicsView2D::LoadLesionMask(cv::Mat lesionMask)
{
	if (lesionMask.empty())
		return;
	m_lesionMask = Mask2Edge(lesionMask.clone());
	cv::Mat lesionPalette(m_lesionMask.rows, m_lesionMask.cols, CV_8UC3, cv::Scalar(0, 0, 255));
	lesionPalette.copyTo(m_img, m_lesionMask);
}

void GraphicsView2D::LoadRectumMask(cv::Mat rectumMask)
{
	if (rectumMask.empty())
		return;
	m_rectumMask = Mask2Edge(rectumMask.clone());
	cv::Mat rectumPalette(m_rectumMask.rows, m_rectumMask.cols, CV_8UC3, cv::Scalar(255, 0, 0));
	rectumPalette.copyTo(m_img, m_rectumMask);
}

//��8UC3 BGR��cv::Mat��ʽ������m_img,תΪQPixmap�洢����ʾ
void GraphicsView2D::ShowImg()
{
	cv::Mat convertedImg;
	cv::cvtColor(m_img, convertedImg, cv::COLOR_BGR2RGB);
	QImage tempImg((const unsigned char*)(convertedImg.data), convertedImg.cols, convertedImg.rows, convertedImg.step, QImage::Format_RGB888);
	m_pixmap = QPixmap::fromImage(tempImg);
	m_pPixmapItem->setPixmap(m_pixmap);
	this->setSceneRect(0, 0, m_pixmap.width(), m_pixmap.height());
	this->fitInView(m_pPixmapItem, Qt::KeepAspectRatio);
	this->centerOn(m_pPixmapItem);
}

cv::Mat GraphicsView2D::Mask2Edge(cv::Mat src)
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
void GraphicsView2D::createGamaLut(uchar lutGama[256], float fC, float fGama)
{
	memset(lutGama, 0, sizeof(uchar) * 2);  //�Ҷ�ֵ���͵ĵ� ֱ����Ϊ0
	for (int i = 2; i < 256; i++)
	{
		lutGama[i] = min(fC*uchar(pow(float(i) / 255, fGama) * 255), 255);
	}
	return;
}

//ʹ�ò��ұ� ��cv::Mat��ֵ
//Mat������ʽ �ο�http://www.opencv.org.cn/opencvdoc/2.3.2/html/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html
//���η�ʽ �ο�https://blog.csdn.net/u012814856/article/details/84099328
cv::Mat GraphicsView2D::transformImg(cv::Mat& srcImg)
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