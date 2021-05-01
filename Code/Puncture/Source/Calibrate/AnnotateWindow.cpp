#include "AnnotateWindow.h"
#include <stdio.h>
#include <io.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <opencv2/opencv.hpp>
//#pragma execution_character_set("utf-8")  //������������ �ο�https://blog.csdn.net/liyuanbhu/article/details/72596952

//�Ƚ���������"baseUS_5.bmp"���ַ�����˭����Ÿ���ǰ
bool cmpFileName(string s1, string s2)
{
	int l1, r1, l2, r2;
	l1 = s1.rfind("_") + 1;
	r1 = s1.rfind(".");
	l2 = s2.rfind("_") + 1;
	r2 = s2.rfind(".");
	s1 = s1.substr(l1, r1 - l1);
	s2 = s2.substr(l2, r2 - l2);
	return atoi(s1.data()) < atoi(s2.data());
}

AnnotateWindow::AnnotateWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_nImageIndex = -1;
	m_vctTriangleVertex = vector<QVector3D>(6, QVector3D(0, 0, 0));
	ui.BtnSave3D->setEnabled(false);
	ui.RadioBtnTransverse->setChecked(false);
	ui.RadioBtnSagittal->setChecked(false);
	ui.PixelSizeX->setText(u8"0");
	ui.PixelSizeY->setText(u8"0");
	m_nImageWidth = 0;
	m_nImageHeight = 0;

	//�����ź����
	connect(ui.BtnSetImageDir, SIGNAL(clicked()), this, SLOT(OnBtnBtnSetImageDirClicked()));
	connect(ui.BtnLoadPhantomTriangles, SIGNAL(clicked()), this, SLOT(OnBtnLoadPhantomTriangles()));
	connect(ui.BtnFormerImage, SIGNAL(clicked()), this, SLOT(OnBtnFormerImageClicked()));
	connect(ui.BtnNextImage, SIGNAL(clicked()), this, SLOT(OnBtnNextImageClicked()));
	connect(ui.BtnFinish, SIGNAL(clicked()), this, SLOT(OnBtnFinishClicked()));
	connect(ui.Annotate2D, SIGNAL(markFinishSignal()), this, SLOT(OnBtnFinishClicked()));
	connect(ui.BtnSave2D, SIGNAL(clicked()), this, SLOT(OnBtnSave2DClicked()));
	connect(ui.BtnSave3DInImage, SIGNAL(clicked()), this, SLOT(OnBtnSave3DInImageClicked()));
	connect(ui.BtnSave3D, SIGNAL(clicked()), this, SLOT(OnBtnSave3DInPhantomClicked()));
	connect(ui.Annotate2D, SIGNAL(mouseMoveSignal(float, float)), this, SLOT(UpdateMousePos(float, float)));
}

AnnotateWindow::~AnnotateWindow()
{
}

void AnnotateWindow::UpdateMousePos(float x, float y)
{
	ui.LabelPosX->setText(QString::number(x, 'f', 3));
	ui.LabelPosY->setText(QString::number(y, 'f', 3));
}

//��ȡ����ͼƬ�� �ο�https://blog.csdn.net/how0723/article/details/78105308
void AnnotateWindow::OnBtnBtnSetImageDirClicked()
{
	m_vctImagePath.clear();
	m_nImageIndex = 0;
	//���û��� ��ȡ�ļ�·��
	QString qstrRoot = QFileDialog::getExistingDirectory(this, "ͼƬĿ¼", "D:/", QFileDialog::ShowDirsOnly);
	m_strRootDir = qstrRoot.toLocal8Bit();
	if (m_strRootDir.empty())
		return;
	//�����ļ���ģ��
	string inPath = m_strRootDir + "/*.bmp";
	//���ڲ��ҵľ��
	intptr_t handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(inPath.data(), &fileinfo);
	//��һ���ļ�
	if (handle == -1)
		return;
	else
		m_vctImagePath.push_back(fileinfo.name);
	//�����ļ�
	while (!_findnext(handle, &fileinfo))
	{
		m_vctImagePath.push_back(fileinfo.name);
	}
	_findclose(handle);
	sort(m_vctImagePath.begin(), m_vctImagePath.end(), cmpFileName);  //����Ŵ�С����
	//��ʼ������ͼƬ���ݵ�����
	m_vctPtPos2D = vector<vector<QPointF>>(m_vctImagePath.size(), vector<QPointF>(POINT_NUM, QPointF(0, 0)));  //��ʼ��Ϊÿһ���㶼��(0,0)
	m_vctPtPos3DInPhantom = vector <vector<QVector3D>>(m_vctImagePath.size(), vector<QVector3D>(POINT_NUM / 3, QVector3D(0, 0, 0)));  //��ʼ��ÿ����ģ����ϵ�µĵ㶼��(0,0,0)
	m_vctPtPos3DInImage = vector <vector<QVector3D>>(m_vctImagePath.size(), vector<QVector3D>(POINT_NUM / 3, QVector3D(0, 0, 0)));  //��ʼ��ÿ��3Dͼ������ϵ�µĵ㶼��(0,0,0)
	//�����0��ͼƬ
	m_nImageIndex = 0;
	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.LabelFilename->setText(QString::fromLocal8Bit(m_vctImagePath[m_nImageIndex].c_str()));
	ui.Annotate2D->ShowImg();
	//��ȡͼƬ����
	cv::Mat img = cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]);
	m_nImageWidth = img.cols;
	m_nImageHeight = img.rows;
}

void AnnotateWindow::Cal3DPosInImage(bool bTransverse, float fPixelSizeX, float fPixelSizeY)
{
	int i, j;
	QPointF ptPos2D;
	float x2D, y2D;  //2DͼƬ������
	float fX, fY, fZ;  //3DͼƬ����ϵ�µ�����
	QVector3D ptPos3D;
	vector<QVector3D> ptsPos3D;
	//m_vctPtPos3DInImage.clear();
	m_vctPtPos3DInImage = vector <vector<QVector3D>>(m_vctImagePath.size(), vector<QVector3D>(POINT_NUM / 3, QVector3D(0, 0, 0)));  //��ʼ��ÿ��3Dͼ������ϵ�µĵ㶼��(0,0,0)
	for (i = 0; i < m_vctPtPos2D.size(); i++)  //����ÿһ��ͼƬ
	{
		ptsPos3D.clear();
		if (m_vctPtPos2D[i][0] == QPointF(0, 0))  //˵������ͼƬδ������ע
			continue;
		for (j = 0; j < POINT_NUM/3; j++)  //��ͼƬ�������м�һ�еĵ�
		{
			ptPos2D = m_vctPtPos2D[i][j * 3 + 1];  //��ȡ��һ���м�ĵ��2Dͼ������ϵ����(��������)
			//��ԭ�� ��ͼƬ���Ͻ� �ƶ���ͼƬ�ױ��е�
			ptPos2D.setX(ptPos2D.x() - m_nImageWidth / 2.0);
			ptPos2D.setY(m_nImageHeight - ptPos2D.y());
			//����2Dͼ������ϵ���꣬����λ������(pixel)��Ϊ����(mm)
			ptPos2D.setX(ptPos2D.x()*fPixelSizeX);
			ptPos2D.setY(ptPos2D.y()*fPixelSizeY);
			//��2Dͼ������ϵ���꣬תΪ3Dͼ������ϵ����
			if (bTransverse)  //����棬��zΪ0
				ptPos3D = QVector3D(ptPos2D.x(), ptPos2D.y(), 0);
			else  //ʸ״�� ��xΪ0
				ptPos3D = QVector3D(0, ptPos2D.y(), ptPos2D.x());
			ptsPos3D.push_back(ptPos3D);
		}
		m_vctPtPos3DInImage[i]=ptsPos3D;
	}
}

void AnnotateWindow::Cal3DPosInPhantom()
{
	int indexImg, markedNum, row;
	float ratio;  //�߶γ��ȵı�ֵ��[left,mid]/[left,right]�����ڼ������������εĴ�С����
	QPointF ptLeft, ptMid, ptRight;  //��ඥ�㣬�м�㣬�Ҳඥ��(2DͼƬ����)
	QVector3D vertexLeft, mid, vertexRight;  //��ඥ�㣬�м�㣬�Ҳඥ��(3D��ģ����ϵ����)
	//m_vctPtPos3D.clear();
	m_vctPtPos3DInPhantom = vector <vector<QVector3D>>(m_vctImagePath.size(), vector<QVector3D>(POINT_NUM / 3, QVector3D(0, 0, 0)));  //��ʼ��ÿ����ģ����ϵ�µĵ㶼��(0,0,0)
	/********����ÿ��ͼƬÿ�е��м�㣬����ģ����ϵ�µ�����************/
	for (indexImg = 0, markedNum = 0; indexImg < m_vctPtPos2D.size(); indexImg++)  //ÿ��ͼƬ�����ݴ�Ϊһ��
	{
		vector<QVector3D> ptsPos3D;  //��ǰ����ͼƬ�У������е�����ģ����ϵ�µ�����
		if (m_vctPtPos2D[indexImg][0] == QPointF(0, 0))  //˵������ͼƬδ������ע
			continue;
		for (row = 0; row < POINT_NUM / 3; row++)  //�ֱ���ÿһ��(ͼƬ��ÿ�б�ע��3����)
		{
			ptLeft = m_vctPtPos2D[indexImg][row * 3];
			ptMid = m_vctPtPos2D[indexImg][row * 3 +1];
			ptRight = m_vctPtPos2D[indexImg][row * 3 + 2];
			vertexLeft = m_vctTriangleVertex[row * 2];
			vertexRight = m_vctTriangleVertex[row * 2 + 1];
			ratio = sqrt(QPointF::dotProduct((ptMid - ptLeft), (ptMid - ptLeft))) / sqrt(QPointF::dotProduct((ptRight - ptLeft), (ptRight - ptLeft)));  //�����߶γ��ȱ�ֵ,�����������γ��ȱ�
			mid = vertexLeft + ratio * (vertexRight - vertexLeft);  //�м������ģ����ϵ�µ�����
			ptsPos3D.push_back(mid);
		}
		m_vctPtPos3DInPhantom[indexImg]=ptsPos3D;
	}
}

void AnnotateWindow::OnBtnFormerImageClicked()
{
	if (m_nImageIndex <= 0)
		return;
	m_nImageIndex--;
	ui.LabelFilename->setText(QString::fromLocal8Bit(m_vctImagePath[m_nImageIndex].c_str()));
	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.Annotate2D->ShowImg();
}

void AnnotateWindow::OnBtnNextImageClicked()
{
	if (m_nImageIndex >= m_vctImagePath.size() - 1)
		return;
	m_nImageIndex++;
	ui.LabelFilename->setText(QString::fromLocal8Bit(m_vctImagePath[m_nImageIndex].c_str()));
	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.Annotate2D->ShowImg();
}

void AnnotateWindow::OnBtnFinishClicked()
{
	vector<QPointF> pointPos = ui.Annotate2D->GetPointPos();
	if (pointPos.size() != POINT_NUM)
	{
		QMessageBox::information(this, u8"ע��", u8"��ǰ�ѱ�ע�ĵ����������ȷ�� �����±�ע��ǰ֡");
		ui.Annotate2D->ShowImg();  //���õ�ǰ֡
	}
	else
	{
		m_vctPtPos2D[m_nImageIndex]=pointPos;
		if (m_nImageIndex >= m_vctImagePath.size() - 1)
		{
			QMessageBox::information(this, u8"ע��", u8"�ѵ������һ֡");
			return;
		}
		m_nImageIndex++;
		ui.LabelFilename->setText(QString::fromLocal8Bit(m_vctImagePath[m_nImageIndex].c_str()));
		ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
		ui.Annotate2D->ShowImg();
	}
}

void AnnotateWindow::OnBtnSave2DClicked()
{
	int indexImg, indexPt, markedNum;
	std::string info;
	QString qstrTxtFilename = QFileDialog::getSaveFileName(this, tr(u8"�洢�㼯��ͼ������ϵ�µ�2D����(ͼƬ��*ÿ��ͼ9����*xy��������)"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	ofstream outfile(strTxtFileName);
	for (indexImg = 0,markedNum=0; indexImg < m_vctPtPos2D.size(); indexImg++)  //ÿ��ͼƬ�����ݴ�Ϊһ��
	{
		if (m_vctPtPos2D[indexImg][0] == QPointF(0, 0))  //���õ��Ƿ񱻸�ֵ��
			continue;
		markedNum++;
		outfile << std::to_string(indexImg) << '\t';  //�洢����±�
		//�洢��ͼƬ���е��x����
		for (indexPt = 0; indexPt < POINT_NUM; indexPt++)
			outfile << m_vctPtPos2D[indexImg][indexPt].x() << '\t';
		//�洢��ͼƬ���е��y����
		for (indexPt = 0; indexPt < POINT_NUM; indexPt++)
			outfile << m_vctPtPos2D[indexImg][indexPt].y() << '\t';
		outfile << '\n';
	}
	outfile.close();
	stringstream sstream;
	ostringstream ostream;
	sstream << "�ܹ�" << m_vctPtPos2D.size() << "��ͼƬ���洢�������ѱ�ע��" << markedNum << "�š�";
	info = sstream.str();
	QMessageBox::information(this, u8"���", QString::fromLocal8Bit(info.c_str()));
}

void AnnotateWindow::OnBtnSave3DInImageClicked()
{
	//��ȷ�Լ��
	bool bTransverse;
	float fPixelSizeX,fPixelSizeY;
	QString qStr;
	if ((!ui.RadioBtnTransverse->isChecked()) && !(ui.RadioBtnSagittal->isChecked()))
	{
		QMessageBox::information(this, u8"����", u8"��ѡ��ɨ��ƽ������");
		return;
	}
	else
		bTransverse = ui.RadioBtnTransverse->isChecked();
	qStr = ui.PixelSizeX->text();
	fPixelSizeX = qStr.toFloat();
	qStr = ui.PixelSizeY->text();
	fPixelSizeY = qStr.toFloat();
	if (fPixelSizeX <= 0 || fPixelSizeY<=0)
	{
		QMessageBox::information(this, u8"����", u8"��������ߴ�Ӧ����0");
		return;
	}
	if (m_nImageWidth <= 0 || m_nImageHeight <= 0)
	{
		QMessageBox::information(this, u8"����", u8"��ȡͼƬ�ߴ����");
		return;
	}
	//����ÿ��ͼƬÿ�е��м�㣬��3DͼƬ����ϵ�µ�����
	this->Cal3DPosInImage(bTransverse, fPixelSizeX,fPixelSizeY);
	//������洢��txt�ļ�
	/********������洢��txt�ļ�************/
	std::string info;  //��ʾ��Ϣ
	QString qstrTxtFilename = QFileDialog::getSaveFileName(this, tr(u8"�洢�㼯��3Dͼ������ϵ�µ���������(ͼƬ��*ÿ��ͼ3����*xyz3������)"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	ofstream outfile(strTxtFileName);
	int indexImg = 0, markedNum;
	for (indexImg = 0, markedNum = 0; indexImg < m_vctPtPos3DInImage.size(); indexImg++)  //ÿ��ͼƬ�����ݴ�Ϊһ��
	{
		if (m_vctPtPos3DInImage[indexImg][0]==QVector3D(0,0,0))  //���õ��Ƿ񱻸�ֵ��
			continue;
		markedNum++;
		outfile << std::to_string(indexImg) << '\t';  //�洢����±�
		vector<QVector3D> ptPos3D;  //��ǰ����ͼƬ�У������е�����ģ����ϵ�µ�����
		for (int row = 0; row < POINT_NUM / 3; row++)  //���洢ÿһ���м�ĵ�
		{
			outfile << m_vctPtPos3DInImage[indexImg][row].x() << '\t'
				<< m_vctPtPos3DInImage[indexImg][row].y() << '\t'
				<< m_vctPtPos3DInImage[indexImg][row].z() << '\t';
		}
		outfile << '\n';
	}
	outfile.close();
	/********�����ʾ��Ϣ************/
	stringstream sstream;
	sstream << "�Ѵ洢��" << markedNum << "��ͼƬ";
	info = sstream.str();
	QMessageBox::information(this, u8"���", QString::fromLocal8Bit(info.c_str()));
}

void AnnotateWindow::OnBtnSave3DInPhantomClicked()
{
	//TODO
	int indexImg, markedNum, row, col;
	float ratio;  //�߶γ��ȵı�ֵ��[left,mid]/[left,right]�����ڼ������������εĴ�С����
	QPointF ptLeft, ptMid, ptRight;  //��ඥ�㣬�м�㣬�Ҳඥ��(2DͼƬ����)
	QVector3D vertexLeft, mid, vertexRight;  //��ඥ�㣬�м�㣬�Ҳඥ��(3D��ģ����ϵ����)
	/********����ÿ��ͼƬÿ�е��м�㣬����ģ����ϵ�µ�����************/
	this->Cal3DPosInPhantom();
	/********������洢��txt�ļ�************/
	std::string info;  //��ʾ��Ϣ
	QString qstrTxtFilename = QFileDialog::getSaveFileName(this, tr(u8"�洢�㼯����ģ����ϵ�µ�3D����(ͼƬ��*ÿ��ͼ3����*xyz3������)"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	ofstream outfile(strTxtFileName);
	for (indexImg = 0, markedNum = 0; indexImg < m_vctPtPos3DInPhantom.size(); indexImg++)  //ÿ��ͼƬ�����ݴ�Ϊһ��
	{
		//if (m_vctPtPos2D[indexImg][0] == QPointF(0, 0))  //���õ��Ƿ񱻸�ֵ��
		if(m_vctPtPos3DInPhantom[indexImg][0]==QVector3D(0,0,0))  //���õ��Ƿ񱻸�ֵ��
			continue;
		markedNum++;
		outfile << std::to_string(indexImg) << '\t';  //�洢����±�
		vector<QVector3D> ptPos3D;  //��ǰ����ͼƬ�У������е�����ģ����ϵ�µ�����
		for (row = 0; row < POINT_NUM / 3; row++)  //�ֱ���ÿһ��(ͼƬ��ÿ�б�ע��3����)
		{
			outfile << m_vctPtPos3DInPhantom[indexImg][row].x() << '\t'
				<< m_vctPtPos3DInPhantom[indexImg][row].y() << '\t'
				<< m_vctPtPos3DInPhantom[indexImg][row].z() << '\t';
		}
		outfile << '\n';
	}
	outfile.close();
	/********�����ʾ��Ϣ************/
	stringstream sstream;
	sstream << "�ܹ�" << m_vctPtPos2D.size() << "��ͼƬ���洢�������ѱ�ע��" << markedNum << "�š�";
	info = sstream.str();
	QMessageBox::information(this, u8"���", QString::fromLocal8Bit(info.c_str()));
}

void AnnotateWindow::OnBtnLoadPhantomTriangles()
{
	int vertexNum;
	float x, y, z;
	m_vctTriangleVertex.clear();
	vertexNum = POINT_NUM / 3 * 2;  //��POINT_NUM/3*2������ΪͼƬ��ÿ����3���㣬����2��Ϊ���������εĶ���
	string info;  //��ʾ��Ϣ
	
	QString qstrTxtFilename = QFileDialog::getOpenFileName(this, tr(u8"������ģ�����νǵ�����(��ģ����ϵ�� 3D����)"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	ifstream infile(strTxtFileName);
	//��ȡ���ɸ����ǵ�
	while (infile >> x && vertexNum>0)
	{
		infile >> y;
		infile >> z;
		m_vctTriangleVertex.push_back(QVector3D(x, y, z));
		vertexNum--;
	}
	infile.close();
	//�����ʾ��Ϣ
	stringstream sstream;
	if (vertexNum > 0)
	{
		sstream << "Ӧ����" << POINT_NUM / 3 * 2 << "������\tȱʧ" << vertexNum << "������";
		info = sstream.str();
		QMessageBox::information(this, "����", QString::fromLocal8Bit(info.c_str()));
	}
	else
	{
		sstream << "�ɹ�������ģ�������ζ�������";
		info = sstream.str();
		ui.BtnSave3D->setEnabled(true);
		QMessageBox::information(this, u8"���", QString::fromLocal8Bit(info.c_str()));
	}
}


