#pragma once

#include <QDialog>
#include <vector>
#include "NDIOperator/NDIOperator.h"
#include "ui_LocatePhantomDlg.h"
using namespace NDIOPERATOR;
using namespace std;

class LocatePhantomDlg : public QDialog
{
	Q_OBJECT

public:
	LocatePhantomDlg(QWidget *parent = Q_NULLPTR);
	//LocatePhantomDlg(QWidget *parent = Q_NULLPTR, NDIOperatorPtr pNDIOperator = nullptr);
	~LocatePhantomDlg();

	void SetNDIOperator(NDIOperatorPtr pNDIOperator);
	void UpdateNDIData(NDIOPERATOR::Attitude euler) { m_euler = euler; }

private:
	Ui::LocatePhantomDlg ui;
	NDIOperatorPtr  m_NDIOperatorPtr;  //NDI�豸����ָ��--���и�ָ�룬ֻ��Ϊ�˷����޸�ʹ�õ�sensor��š�NDI������CalibrateWindow�вɼ�
	NDIOPERATOR::Attitude  m_euler;  //����̽ͷ��̬(ŷ������ʽ)
	vector<vector<float>> m_vctPoints;  //n*3���󣬼�¼ÿ�����λ��
	void UpdatePreview();  //��ʾ��ǰ�ѱ�ע�ĵ�

private slots:
	void ChangeSensor(int n);
	void OnBtnRecordClicked();  //��¼��ǰ��λ��
	void OnBtnDeleteClicked();  //ɾ�������ӵĵ�
	void OnBtnSaveClicked();  //�洢���ļ�
};
