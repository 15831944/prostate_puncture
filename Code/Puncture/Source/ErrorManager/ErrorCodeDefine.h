#pragma once

/**************************************************���������ʾ����*******************************************************/
enum
{
	WatchDogCount = 99999
};

enum ErrorCodeType {
	LIST_NO_ERROR = 0x00000000,	//��������

	//Puncture����
	ER_InitNDIDeviceFailed				= 0x00000001,	//����NDI�豸ʧ��
	ER_InitUSBDeviceFailed				= 0x00000002,	//����B��ͼ��ɼ��豸ʧ��

	//NDIOperatorģ��
	ER_WrongNDIConfigFilePathName		= 0x00100001,	//�����NDIģ��ini�ļ�·�����ļ���
	ER_OpenNDIConfigFileFailed			= 0x00100002,	//��NDIģ��ini�ļ�ʧ��
	ER_StartTrackingNDIFailed			= 0x00100003,	//
	ER_TrackingNDIFailed				= 0x00100004,
	ER_StopTrackingNDIFailed			= 0x00100005,

	//SurgicalPlanģ��
	ER_FileOpenFailed					= 0x00200001,	//�޷��򿪶�Ӧ��ini�ļ�
	ER_InportSurgicalPlanFailed			= 0x00200002,	//���������ƻ�ʧ��
	ER_OpenSurgicalPlanConfigFileFailed = 0x00200003,	//���������ƻ�ini�ļ�ʧ��
	ER_InportMaskDataError				= 0x00200004,	//����Mask����ʧ��
	ER_InportMRIDataError				= 0x00200005,	//����MRI����ʧ��
	ER_ReachedRectumEnd					= 0x00200006,	//������ֱ���������������е��յ�
	ER_ImportRectumCenterError			= 0x00200007,	//����ֱ��������txt����ʧ��
	ER_ImportObjDataError				= 0x00200008,	//����Obj����ʧ��

	//AnalyseProcessģ��
	ER_2DImageSizeError					= 0x00300001,	//2Dͼ��ߴ����
	ER_3DImageSizeError					= 0x00300002,	//3Dͼ��ߴ����
	ER_WrongAnalyzeConfigFilePathName	= 0x00300002,	//�޷��򿪶�Ӧ��ini�ļ�
	ER_WrongAnalyseConfigFilePathName	= 0x00300004,	//����Ĳ���MRI����ini�ļ�·�����ļ���
	ER_OpenAnalyseConfigFileFailed		= 0x00300005,	//�򿪲���MRI���ݶ�Ӧ��ini�ļ�ʧ��
	ER_CalculateTransformMatrix			= 0x00300006,	//���㳬��/MRIģ�����ת������ʧ��
	ER_NoSurgicalPlan					= 0x00300007,	//ImageSampler����DataPackageʧ��
	ER_SampleParameterError				= 0x00300008,	//ImageSampler�и�����������
	ER_InitAnalyseProcessFailed			= 0x00300009,	//��������ģ��ʧ��
	ER_CloseAnalyseProcessFailed		= 0x00300010,	//�رշ���ģ��ʧ��

	//USBCapturerģ��
	ER_WrongUSBConfigFilePathName		= 0x00400001,	//�����NDIģ��ini�ļ�·�����ļ���
	ER_OpenUSBConfigFileFailed			= 0x00400002,	//��NDIģ��ini�ļ�ʧ��
	ER_OpenUSBDeviceFailed				= 0x00400003,	//��USB�ɼ����豸ʧ��
	ER_NoCapturePerFrameFun				= 0x00400004,	//δ���òɼ�B��֡����õĻص�����
	ER_NoLetterTemplate					= 0x00400005	//δ�ҵ�S,T���ַ�ģ��

};