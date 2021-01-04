//�����ڱ�ע��GUI���棬�������롢�洢�Ȱ�ť
#pragma once
#include <QMainWindow>
#include <vector>
#include <string>
#include "ui_AnnotateWindow.h"
using namespace std;

class AnnotateWindow : public QMainWindow
{
	Q_OBJECT

public:
	AnnotateWindow(QWidget *parent = Q_NULLPTR);
	~AnnotateWindow();

public slots:
	void UpdateMousePos(float x, float y);

private slots:
	void OnBtnBtnSetImageDirClicked();  //����ͼ�������ڵ�·��(��ʱ�������һ֡)
	void OnBtnFormerImageClicked();  //ת��ǰһ֡
	void OnBtnNextImageClicked();  //ת����һ֡
	void OnBtnFinishClicked();  //�洢��ǰ֡
	void OnBtnSaveClicked();  //�洢�ѱ�ע����������

private:
	Ui::AnnotateWindow ui;
	string m_strRootDir;
	int m_nImageIndex;  //��ǰ���ڴ����ͼƬ���
	vector<string> m_vctImagePath;  //ͼƬ�ļ���
	vector<vector<QPointF>> m_vctImageData;  //����ͼƬ�и������λ��
};
