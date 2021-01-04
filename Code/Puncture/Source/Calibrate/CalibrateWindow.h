#pragma once

#include <mutex>
#include <QtWidgets/QMainWindow>
#include <qtimer.h>
#include "NDIOperator/NDIOperator.h"
#include "USBCapturer/USBCapturer.h"
#include "AnnotateWindow.h"
#include "LocatePhantomDlg.h"
#include "ui_CalibrateWindow.h"
using namespace NDIOPERATOR;
using namespace USBCAPTURER;

class CalibrateWindow : public QMainWindow
{
    Q_OBJECT

public:
    CalibrateWindow(QWidget *parent = Q_NULLPTR);
	~CalibrateWindow();
	void closeEvent();

private:
	void UpdateUSImage(cv::Mat, cv::Mat, double, double);  //��Ϊ�ص���������USCapturer,���³�������
	void UpdateNDIData(NDIOPERATOR::Attitude);  //��Ϊ�ص���������NDIOperator,����NDI����(ŷ������ʽ)
private slots:
	int OnBtnInitDeviceClicked();
	void OnBtnStartCaptureClicked();
	void OnBtnAnnotateClicked();
	void OnBtnLocatePhantomClicked();
	void ReleaseDevice();  //�ر��豸
	void OnTimerDisplay();  //��ʱˢ����ʾ
	void OnTimerCapture();  //��ʱ�ɼ�
private:
    Ui::CalibrateWindowClass ui;
	AnnotateWindow * m_AnnotateWindowPtr;  //���ڱ�עͼƬ�ϵĵ�
	LocatePhantomDlg * m_LocatePhantomDlgPtr;  //���ڶ�λ��ģ�ļ����ǵ�
	QTimer m_timerDisplay;  //ˢ����ʾ��ʱ��
	QTimer m_timerCapture;  //�ɼ����ݼ�ʱ��
	NDIOperatorPtr		m_NDIOperatorPtr;		//NDI�豸����ָ��
	USBCapturerPtr		m_USBCapturerPtr;		//B��ͼ��ɼ��豸ָ��
	cv::Mat m_imgUS;  //����ͼ��
	float m_fUSRes;  //����ͼ��ֱ���(һ�����ص�����ߴ� mm/pixel)
	NDIOPERATOR::Attitude m_euler;  //����̽ͷ��̬(ŷ������ʽ)
	bool m_bUSAcquired;  //����ͼ���ѻ�ñ�־λ
	int m_nDataIndex;  //�ɼ������洢�������±�
	std::mutex m_showMutex;  //��ʾ�����ݸ��� ������
	string m_strDataDir;  //�ɼ����ݴ洢��λ��

};
