#pragma once
//�����ڱ�ע��GUI���棬�������롢�洢�Ȱ�ť
#include <QMainWindow>
#include <vector>
#include <string>
#include <qvector3d.h>
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

private:
	void Cal3DPosInImage(bool bTransverse, float fPixelSizeX, float fPixelSizeY); //�����Ѿ���ע�õ�ͼƬ���ݣ�����ÿ��ͼƬÿ�����ĵ����������(ͼ������ϵ��)
	void Cal3DPosInPhantom();  //�����Ѿ���ע�õ�ͼƬ���ݣ�����ÿ��ͼƬÿ���м�������(��ģ����ϵ��)

private slots:
	void OnBtnBtnSetImageDirClicked();  //����ͼ�������ڵ�·��(��ʱ�������һ֡),���ҽ�����װ��ͼƬ���ݵ�vector��size����ΪͼƬ��!!!
	void OnBtnFormerImageClicked();  //ת��ǰһ֡
	void OnBtnNextImageClicked();  //ת����һ֡
	void OnBtnFinishClicked();  //�洢��ǰ֡
	void OnBtnSave2DClicked();  //�洢�ѱ�ע����������(��2Dͼ������ϵ�е����� ͼƬ��*ÿ��ͼ9����)
	void OnBtnSave3DInImageClicked ();  //�洢����ͼƬ�У������м�ĵ㣬��3DͼƬ����ϵ�µ���������(��λ:mm)
	void OnBtnSave3DInPhantomClicked();  //���㲢�洢�ѱ�ע����������(����ģ����ϵ�е����� ͼƬ��*ÿ��ͼ3����)
	void OnBtnLoadPhantomTriangles();  //������ģ�ο�������(��ÿ��N�ߵ�2���ǵ�����ģ����ϵ�µ����� ����3N�� ��Ϊx1y1z1 x2y2z2 x3y3z3 x4y4z4 x5y5z5 x6y6z6)

private:
	Ui::AnnotateWindow ui;
	string m_strRootDir;
	int m_nImageIndex;  //��ǰ���ڴ����ͼƬ���
	int m_nImageWidth, m_nImageHeight;
	vector<string> m_vctImagePath;  //���������ͼƬ�ļ���
	vector<vector<QPointF>> m_vctPtPos2D;  //����ͼƬ��,��������2DͼƬ����ϵ��(ԭ�������Ͻ�)����������(��λ������)
	vector<vector<QVector3D>> m_vctPtPos3DInImage;  //����ͼƬ�У������м�ĵ㣬��3DͼƬ����ϵ��(ԭ���ڵײ��м�)����������(��λ:mm)
	vector <vector<QVector3D>> m_vctPtPos3DInPhantom;  //����ͼƬ�У������м�ĵ㣬��3D��ģ����ϵ�µ���������(��λ:mm)
	vector<QVector3D> m_vctTriangleVertex;  //��ģ����ϵ�£����������εĶ�������(��POINT_NUM/3*2������ΪͼƬ��ÿ����3���㣬����2��Ϊ���������εĶ���)
};
