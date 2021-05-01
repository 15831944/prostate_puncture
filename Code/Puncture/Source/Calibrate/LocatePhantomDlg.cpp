#include "LocatePhantomDlg.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <string>
#include <sstream>
#include <fstream>
//#pragma execution_character_set("utf-8")  //������������ �ο�https://blog.csdn.net/liyuanbhu/article/details/72596952
const int TIME_WAIT = 5000;  //�ɼ�����������ǰ���ȴ���ʱ��
const int TIME_INTERVAL = 200;  //�ɼ�����������֮�� ������г�
const int PT_NUM = 30;  //�ɼ�����������

LocatePhantomDlg::LocatePhantomDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.SensorNumber->setValue(-1);

	//�ź����
	connect(ui.SensorNumber, SIGNAL(valueChanged(int)), this, SLOT(ChangeSensor(int)));
	connect(ui.BtnRecord, SIGNAL(clicked()), this, SLOT(OnBtnRecordClicked()));
	connect(ui.BtnRecordRepeat, SIGNAL(clicked()), this, SLOT(OnBtnRecordRepeatClicked()));
	connect(ui.BtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClicked()));
	connect(ui.BtnSave, SIGNAL(clicked()), this, SLOT(OnBtnSaveClicked()));
}

LocatePhantomDlg::~LocatePhantomDlg()
{
	m_NDIOperatorPtr->ResetSensorNumber();  //��NDI�豸ʹ�õ�sensor������Ϊini�����ļ������涨��
}

void LocatePhantomDlg::SetNDIOperator(NDIOperatorPtr pNDIOperator)
{
	m_NDIOperatorPtr = pNDIOperator;
	if (m_NDIOperatorPtr == nullptr)
	{
		QMessageBox::information(this, u8"����", u8"NDI�豸Ϊ��");
		return;
	}
	else if (!m_NDIOperatorPtr->IsTracking())
	{
		QMessageBox::information(this, u8"����", u8"NDI�豸δ������");
	}
	ui.SensorNumber->setValue(m_NDIOperatorPtr->GetSensorNumber());
}

void LocatePhantomDlg::ChangeSensor(int n)
{
	m_NDIOperatorPtr->SetSensorNumber(n);
}

void LocatePhantomDlg::UpdatePreview()
{
	int i, j;
	string s;
	//��ʾ������
	for (i = 0; i < m_vctPoints.size(); i++)
	{
		s += to_string(i+1);  //�ѱ�ע�����
		s += '\t';
		for (j = 0; j < 3; j++)
		{
			stringstream sstream;  //��float������תΪstring���ͣ��ο�https://blog.csdn.net/feidaji/article/details/85123002
			sstream << m_vctPoints[i][j];
			s += sstream.str() + '\t';
		}
		s += '\n';
	}
	ui.LocatedPts->setPlainText(QString::fromLocal8Bit(s.c_str()));  //������ʾ
}

void LocatePhantomDlg::OnBtnRecordClicked()
{
	float fX, fY, fZ, fAlpha, fBeta, fGama;
	m_euler = m_NDIOperatorPtr->GetCurEulerAttitude();
	m_euler.GetPosition(fX, fY, fZ, fAlpha, fBeta, fGama);  //��ȡŷ���ǲ�����Ϣ
	m_vctPoints.push_back(vector<float>{fX, fY, fZ});  //�洢������
	this->UpdatePreview();
}

//�ɼ�����������
void LocatePhantomDlg::OnBtnRecordRepeatClicked()
{
	int i;
	float fX, fY, fZ, fAlpha, fBeta, fGama;
	Sleep(TIME_WAIT);  //�ȵȴ�һ�����г����Ӷ���������߽�̽ͷ�ŵ���Ӧλ��
	for (i = 0; i < PT_NUM; i++)
	{
		m_euler = m_NDIOperatorPtr->GetCurEulerAttitude();
		m_euler.GetPosition(fX, fY, fZ, fAlpha, fBeta, fGama);  //��ȡŷ���ǲ�����Ϣ
		m_vctPoints.push_back(vector<float>{fX, fY, fZ});  //�洢������
		this->UpdatePreview();
		Sleep(TIME_INTERVAL);
	}
	QMessageBox::information(this, u8"���", u8"�������ɼ����");
}

void LocatePhantomDlg::OnBtnDeleteClicked()
{
	if (!m_vctPoints.empty())
		m_vctPoints.pop_back();
	this->UpdatePreview();
}

void LocatePhantomDlg::OnBtnSaveClicked()
{
	int i, j;
	string s,info;
	stringstream sstream;  //��float������תΪstring���ͣ��ο�https://blog.csdn.net/feidaji/article/details/85123002
	QString qstrTxtFilename = QFileDialog::getSaveFileName(this, tr(u8"�洢�㼯"), "D:/", "*.txt");
	string strTxtFileName = (const char*)qstrTxtFilename.toLocal8Bit();  //ת��Ϊstd::string �ұ��������ַ���������
	if (strTxtFileName.empty())
		return;
	
	ofstream outfile(strTxtFileName);
	for (i = 0; i < m_vctPoints.size(); i++)
	{
		for (j = 0; j < 3; j++)
		{
			outfile << m_vctPoints[i][j] << '\t';
		}
		outfile << '\n';
	}
	outfile.close();

	sstream.clear();
	sstream << "�Ѵ洢��" << m_vctPoints.size() << "�����λ��";
	info = sstream.str();
	QMessageBox::information(this, u8"���", QString::fromLocal8Bit(info.c_str()));
}
