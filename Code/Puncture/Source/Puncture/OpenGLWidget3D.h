#pragma once
//�ο�https://blog.csdn.net/p942005405/article/details/103770259
//�ο�https://www.jianshu.com/p/bccc565b5248
//���ư�͸������ �ο�https://blog.csdn.net/u014386971/article/details/22492631?utm_medium=distribute.pc_relevant.none-task-blog-title-2&spm=1001.2101.3001.4242
#define AFX_EXT_CLASS __declspec(dllimport)  //��Ϊ����Ŀ��qtӦ�ó���δʹ��MFC����Ҫ��������MFC��Ŀ�е��ࣨfsutility::Coordinate��,���Լ򵥵ؽ�MFC����/����dll�ĺ�AFX_EXT_CLASS�ڱ���Ŀ�ж���Ϊ����
#include <qopenglwidget.h>
#include <qopenglfunctions.h>
#include <qopenglshaderprogram.h>
#include <qopenglbuffer.h>
#include <qopenglvertexarrayobject.h>
#include <Qt3DInput/qkeyevent.h>
#include <mutex>
#include "SurgicalPlan/ObjData.h"


using namespace SURGICALPLAN;


class OpenGLWidget3D :
	public QOpenGLWidget,
	protected QOpenGLFunctions
{
	Q_OBJECT
public:
	OpenGLWidget3D(QWidget* parent = nullptr);
	~OpenGLWidget3D();

	void initializeGL() override;
	void resizeGL(int width, int height) override;
	void paintGL() override;
	
	void LoadProstateObj(ObjDataPtr prostateObjPtr);  //��ȡprostateģ��ָ�룬����prostate���ݣ�������3D��ʾ����  //TODO
	void LoadLesionObj(ObjDataPtr lesionObjPtr);  //��ȡlesionģ��ָ�룬����lesion����
	//void SetWorldSize(float width, float height, float depth);  //����obj�������ڵ�����ռ�����Ĵ�С(mm)
	void SetTransversePlane(Coordinate cLeftTop, Coordinate cRightTop, Coordinate cLeftBottom, Coordinate cRightBottom);  //���º�����ĸ����λ��  //TODO(if iscreated())
	void SetSagittalPlane(Coordinate cLeftTop, Coordinate cRightTop, Coordinate cLeftBottom, Coordinate cRightBottom);  //����ʸ״���ĸ����λ��  //TODO

protected:
	void mousePressEvent(QMouseEvent *event) override;  //����϶�trackball �ƶ��ӽ�
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;  //����������

	QVector3D projectToTrackball(const QPoint &screenCoords) const;  //��2D����Ļ�����꣬ת��Ϊ3D��trackball������
	void calRotateParameters(const QPoint &firstPoint,const QPoint &nextPoint, QVector3D &dir, float &angle);  //������ת����

private:
	void CreateShader();  //ʹ��Ӳ���ļ� ������ɫ��
	void UpdateShader();  //���ݲ������� ������ɫ������
	void LoadAnnotationToGPU();  //���첢���������ݵ��Դ�
	void LoadPlaneToGPU();  //���첢����ƽ�����ݵ��Դ�
	void LoadObjToGPU(ObjDataPtr objPtr, QOpenGLVertexArrayObject& vao, QOpenGLBuffer& vbo, QOpenGLBuffer& ebo);  //ʹ��ָ����vao��vbo��ebo���󣬽�ģ������GPU

private:
	QOpenGLShaderProgram m_shaderObject,m_shaderAnnotation;  //���� ģ��/��ע����ɫ������
	QOpenGLVertexArrayObject m_vaoAnnotation,m_vaoProstate, m_vaoLesion,m_vaoPlane;  //�����������(vao����ʵ�ʴ洢���� ֻ�Ǵ洢��vbo��ebo�����ã��Լ��������ݵķ�ʽ)
	QOpenGLBuffer m_vboAnnotation,m_vboProstate, m_vboLesion,m_vboPlane;  //���㻺�����
	QOpenGLBuffer m_eboAnnotation,m_eboProstate, m_eboLesion,m_eboPlane;  //�����������
	ObjDataPtr m_prostateObjPtr, m_lesionObjPtr;  //ģ�Ͷ���ָ��

	QMatrix4x4 m_mtxModel;  //ģ�;���  ��prostate��boundbox��������
	QMatrix4x4 m_mtxView;  //�۲����
	QMatrix4x4 m_mtxProjection;  //ͶӰ����
	QVector3D m_defaultViewerPos;  //Ĭ�ϵĹ۲���λ��
	QVector3D m_defaultUpDir;  //Ĭ�ϵ��Ϸ�

	//float m_fWidth;  //����ռ���(mm)
	//float m_fHeight;  //����ռ�߶�(mm)
	//float m_fDepth;  //����ռ����(mm)
	float m_fRadius;  //��������ڵ���뾶(mm) ��boundbox�Ĵ�С���
	QVector4D m_colorAnnotation;  //�����ɫ
	QVector4D m_colorProstate;  //ǰ������ɫ
	QVector4D m_colorLesion;  //������ɫ
	QVector4D m_colorPlane;  //ƽ��ʾ����ɫ
	QVector3D m_colorLight;  //��Դ��ɫ
	
	bool m_bLeftActive;  //���������
	QPoint m_mouseLastPos,m_mouseCurPos;  //���λ��
	float m_fLinearSpeed;  //�������ƽ���ٶ�
	float m_fRotateSpeed;  //���������ת�ٶ�
	float m_fTrackballRadius;  //trackball�İ뾶(ʵ�������������Ļ�ߴ�ı���)
	float m_fX1, m_fX2, m_fY1, m_fY2, m_fZ1, m_fZ2;  //boundbox
	GLfloat m_verticesPlane[8*6];  //˫ƽ��ʾ�� ��8����,ÿ���������������+����������

	std::mutex m_mutexRender;  //��Ⱦ �������� ������
};

