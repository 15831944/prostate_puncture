#include "AnnotateWindow.h"
#include <stdio.h>
#include <io.h>
#include <algorithm>
#include <fstream>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <string>
#include <opencv2/opencv.hpp>
#pragma execution_character_set("utf-8")  //������������ �ο�https://blog.csdn.net/liyuanbhu/article/details/72596952

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

	//�����ź����
	connect(ui.BtnSetImageDir, SIGNAL(clicked()), this, SLOT(OnBtnBtnSetImageDirClicked()));
	connect(ui.BtnFormerImage, SIGNAL(clicked()), this, SLOT(OnBtnFormerImageClicked()));
	connect(ui.BtnNextImage, SIGNAL(clicked()), this, SLOT(OnBtnNextImageClicked()));
	connect(ui.BtnFinish, SIGNAL(clicked()), this, SLOT(OnBtnFinishClicked()));
	connect(ui.BtnSave, SIGNAL(clicked()), this, SLOT(OnBtnSaveClicked()));
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
	//��ʼ����ע��Ϣ
	m_vctImageData = vector<vector<QPointF>>(m_vctImagePath.size(), vector<QPointF>(POINT_NUM, QPointF(0, 0)));  //��ʼ��Ϊÿһ���㶼��[0,0]
	//�����0��ͼƬ
	m_nImageIndex = 0;
	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.LabelFilename->setText(QString::fromLocal8Bit(m_vctImagePath[m_nImageIndex].c_str()));
	ui.Annotate2D->ShowImg();
}

void AnnotateWindow::OnBtnFormerImageClicked()
{
	if (m_nImageIndex <= 0)
		return;
	m_nImageIndex--;

	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.Annotate2D->ShowImg();
}

void AnnotateWindow::OnBtnNextImageClicked()
{
	if (m_nImageIndex >= m_vctImagePath.size() - 1)
		return;
	m_nImageIndex++;
	ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
	ui.Annotate2D->ShowImg();
}

void AnnotateWindow::OnBtnFinishClicked()
{
	vector<QPointF> pointPos = ui.Annotate2D->GetPointPos();
	if (pointPos.size() != POINT_NUM)
	{
		QMessageBox::information(this, "ע��", "��ǰ�ѱ�ע�ĵ����������ȷ�� �����±�ע��ǰ֡");
		ui.Annotate2D->ShowImg();  //���õ�ǰ֡
	}
	else
	{
		m_vctImageData[m_nImageIndex]=pointPos;
		if (m_nImageIndex >= m_vctImagePath.size() - 1)
		{
			QMessageBox::information(this, "ע��", "�ѵ������һ֡");
			return;
		}
		m_nImageIndex++;
		ui.Annotate2D->LoadImg(cv::imread(m_strRootDir + '/' + m_vctImagePath[m_nImageIndex]));
		ui.Annotate2D->ShowImg();
	}
}

void AnnotateWindow::OnBtnSaveClicked()
{
	int indexImg, indexPt, markedNum;
	std::string info;
	QString qstrTxtFilename = QFileDialog::getSaveFileName(this, tr("�洢�㼯"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	ofstream outfile(strTxtFileName);
	for (indexImg = 0,markedNum=0; indexImg < m_vctImageData.size(); indexImg++)  //ÿ��ͼƬ�����ݴ�Ϊһ��
	{
		if (m_vctImageData[indexImg][0] == QPointF(0, 0))  //���õ��Ƿ񱻸�ֵ��
			continue;
		markedNum++;
		outfile << std::to_string(indexImg) << '\t';  //�洢����±�
		//�洢��ͼƬ���е��x����
		for (indexPt = 0; indexPt < POINT_NUM; indexPt++)
			outfile << m_vctImageData[indexImg][indexPt].x() << '\t';
		//�洢��ͼƬ���е��y����
		for (indexPt = 0; indexPt < POINT_NUM; indexPt++)
			outfile << m_vctImageData[indexImg][indexPt].y() << '\t';
		outfile << '\n';
	}
	outfile.close();
	stringstream sstream;
	sstream << "�ܹ�" << markedNum << "��ͼƬ���洢�������ѱ�ע��" << m_vctImageData.size() << "�š�";
	info = sstream.str();
	QMessageBox::information(this, "���", QString::fromLocal8Bit(info.c_str()));
}


