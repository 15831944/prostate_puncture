#pragma once

#include "ui_PunctureWindow.h"
#include <QtWidgets/QMainWindow>
#include <qtimer.h>
#include <thread>
#include <mutex>
//#include <afx.h>  //MFC֧��
#include "NDIOperator/NDIOperator.h"
#include "SurgicalPlan/SurgicalPlan.h"
#include "USBCapturer/USBCapturer.h"
#include "AnalyseProcess/AnalyseProcess.h"

using namespace NDIOPERATOR;
using namespace SURGICALPLAN;
using namespace USBCAPTURER;
using namespace ANALYSEPROCESS;

class PunctureWindow : public QMainWindow
{
    Q_OBJECT

public:
    PunctureWindow(QWidget *parent = Q_NULLPTR);
	void InitWindow();  //��ʼ������

public slots:
	void OnTimerTimeout();
	int InitDevice();  //��ʼ���豸
	void OnBtnRegisterClicked();
	void OnBtnQuitClicked();
	void MoveMRILeft();
	void MoveMRIRight();
	void MoveMRIUp();
	void MoveMRIDown();
	void MoveMRIForward();
	void MoveMRIBackward();

private:
	void UpdateFrame(FrameDataPtr t_FrameDataPtr);  //��Ϊ�ص��������ݸ�AnalyseProcess�����´����ĵ�֡����


private:
    Ui::PunctureWindowClass ui;
	QTimer m_timer;
	bool m_b2DAcquired;  //�Ƿ��ѻ�ȡ2Dͼ���־λ
	bool m_b3DAcquired;  //�Ƿ��ѻ�ȡ3Dͼ���־λ
	bool m_bRegistered;  //�Ƿ��Ѿ���׼���
	std::mutex m_showMutex;  //��ʾ������
	FrameDataPtr		m_FrameDataPtr;			//��֡����
	NDIOperatorPtr		m_NDIOperatorPtr;		//NDI�豸����ָ��
	USBCapturerPtr		m_USBCapturerPtr;		//B��ͼ��ɼ��豸ָ��
	SurgicalPlanPtr		m_SurgicalPlanPtr;		//�����ƻ�ģ��ָ��
	AnalyseProcessPtr	m_AnalyseProcessPtr;	//����ģ��ָ��
};
