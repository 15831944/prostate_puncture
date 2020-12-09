#include "OpenGLWidget3D.h"
#include <string>
#include <qstring.h>
#include <qmessagebox.h>
#include <qvector3d.h>
#pragma execution_character_set("utf-8")  //����Ĭ�ϱ����ʽ ������������



OpenGLWidget3D::OpenGLWidget3D(QWidget* parent):
	m_vboAnnotation(QOpenGLBuffer::VertexBuffer),  //����m_vboΪ���㻺�����,��Ϊ������ɫ��������(������create()���Żᴴ��)
	m_vboProstate(QOpenGLBuffer::VertexBuffer),  
	m_vboLesion(QOpenGLBuffer::VertexBuffer),
	m_vboPlane(QOpenGLBuffer::VertexBuffer),
	m_eboAnnotation(QOpenGLBuffer::IndexBuffer),  //����m_eboΪ�����������,��ΪͼԪ��ɫ��������(������create()���Żᴴ��)
	m_eboProstate(QOpenGLBuffer::IndexBuffer),  
	m_eboLesion(QOpenGLBuffer::IndexBuffer),
	m_eboPlane(QOpenGLBuffer::IndexBuffer)
{
	this->setParent(parent);

	m_colorAnnotation = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);  //���ñ����ɫΪ��ɫ��͸��
	m_colorProstate = QVector4D(0.0f, 0.5f, 1.0f, 0.5f);  //����ǰ������ɫΪǳ��ɫ��͸��
	m_colorLesion = QVector4D(1.0f, 0.0f, 0.0f, 1.0f);  //���ò�����ɫΪ��ɫ��͸��
	m_colorLight = QVector3D(1.0f, 1.0f, 1.0f);  //��Դ��Ϊ��ɫ
	m_colorPlane = QVector4D(0.5, 0.5, 0.5, 0.5);  //ƽ��ʾ����ɫΪ��ɫ��͸��
	m_bLeftActive = false;  //��ʼ״̬���δ����
	m_fLinearSpeed = 10; 
	m_fRotateSpeed = 100;
	memset(m_verticesPlane, 0, sizeof(GLfloat) * 8 * 6);  //��ʼ״̬ƽ���ϵĵ�ȫΪ0
	m_fTrackballRadius = 1.0;  //trackball�İ뾶Ĭ��Ϊ1

	this->setVisible(true);  //����Ϊ�ɼ� ��������
}


OpenGLWidget3D::~OpenGLWidget3D()
{
	this->makeCurrent();  //�ǰcontextΪOpenGL�����Ŀ��

	//���ٶ��㻺�����(���Դ���ɾ������)
	m_vboAnnotation.destroy();
	m_vboProstate.destroy();
	m_vboLesion.destroy();
	m_vboPlane.destroy();
	//��������������(���Դ���ɾ������)
	m_eboAnnotation.destroy();
	m_eboProstate.destroy(); 
	m_eboLesion.destroy();
	m_eboPlane.destroy();
	//���ٶ����������(���ڴ���ɾ������)
	m_vaoAnnotation.destroy();
	m_vaoProstate.destroy();  
	m_vaoLesion.destroy();
	m_vaoPlane.destroy();

	this->doneCurrent();  //����Ե�ǰcontext�İ�
}

void OpenGLWidget3D::initializeGL()
{
	this->initializeOpenGLFunctions();

	/************������ɫ��*******************/
	this->CreateShader();

	/**************����ģ����ɫ��********************/
	QMatrix4x4 mtxIdentity;  //��λ�� ��Ϊ��ɫ�����������Ĭ��ֵ
	m_shaderObject.bind();  //����ǰ��ɫ������󶨵�opengl������
	//����ģ�;���(���ڽ�boundbox��������)
	m_shaderObject.setUniformValue("model", mtxIdentity);
	//���ù۲����
	m_shaderObject.setUniformValue("view", mtxIdentity);
	//����ͶӰ����
	m_shaderObject.setUniformValue("projection", mtxIdentity);
	//���ù�Դ��ɫ
	m_shaderObject.setUniformValue("lightColor", m_colorLight);
	m_shaderObject.release();
	/**************���ñ�ע��ɫ��********************/
	m_shaderAnnotation.bind();  //����ǰ��ɫ������󶨵�opengl������
	//����ģ�;���
	m_shaderAnnotation.setUniformValue("model", mtxIdentity);
	//���ù۲����
	m_shaderAnnotation.setUniformValue("view", mtxIdentity);
	//����ͶӰ����
	m_shaderAnnotation.setUniformValue("projection", mtxIdentity);
	m_shaderAnnotation.release();

	/*************�������ݵ��Դ�****************/
	this->LoadPlaneToGPU();

	m_shaderObject.release();
}

void OpenGLWidget3D::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void OpenGLWidget3D::paintGL()
{
	glEnable(GL_DEPTH_TEST);  //������Ȳ���
	glEnable(GL_BLEND);  //���û��
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  //���ñ�����ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //�����ɫ�����Լ���Ȼ���

	//���Ǳ�����Ȳ��Կ���
	//�Ȼ��Ʋ�͸�����壬ͬʱ���������Ϣд����Ȼ���
	//�ٻ��ư�͸�����壬�����������Ϣд�뻺�壬��͸�����岻���ڵ���������
	/*********���Ʋ�͸������***********/
	glDepthMask(GL_TRUE);  //����Ȼ���������Ϊ�ɶ���д��ʽ
	m_mutexRender.lock();  //��ʼ��Ⱦ����
	m_shaderAnnotation.bind();  //����ע��ɫ����opengl�����İ�
	m_shaderAnnotation.setUniformValue("view", m_mtxView);
	m_shaderAnnotation.setUniformValue("color", m_colorAnnotation);
	//����boundbox
	if(m_vboAnnotation.isCreated())
	{
		m_vboAnnotation.bind();
		m_eboAnnotation.bind();
		unsigned int eboData[24];
		m_eboAnnotation.read(0, eboData, sizeof(eboData));
		m_vboAnnotation.release();
		m_eboAnnotation.release();
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoAnnotation);
		m_shaderAnnotation.setUniformValue("color", m_colorAnnotation);
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);  //���Ƴ������12����
		vaoBind.release();
	}
	m_shaderAnnotation.release();

	m_shaderObject.bind();  //��ģ����ɫ����opengl�����İ�
	m_shaderObject.setUniformValue("view", m_mtxView);  //���ù۲����
	//���Ʋ���
	if(m_vboLesion.isCreated())
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoLesion);
		m_shaderObject.setUniformValue("objectColor", m_colorLesion);  //���ò�����ɫ
		glDrawElements(GL_TRIANGLES, m_lesionObjPtr->GetTriangleNum() * 3, GL_UNSIGNED_INT, 0);  //����������Ƭ
		vaoBind.release();
	}

	/*********���ư�͸������*************/
	glDepthMask(GL_FALSE);  //����Ȼ���������Ϊֻ����ʽ
	//����ǰ����
	if(m_vboProstate.isCreated())
	{
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoProstate);
		m_shaderObject.setUniformValue("objectColor", m_colorProstate);  //����ǰ������ɫ
		glDrawElements(GL_TRIANGLES, m_prostateObjPtr->GetTriangleNum() * 3, GL_UNSIGNED_INT, 0);  //����������Ƭ
		vaoBind.release();
	}
	//����ʾ��ƽ��
	if(m_vboPlane.isCreated())
	{
		//TODO
		QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoPlane);
		m_shaderObject.setUniformValue("objectColor", m_colorPlane);  //����ʾ��ƽ����ɫ
		glDrawElements(GL_TRIANGLES, 4 * 3, GL_UNSIGNED_INT, 0);  //����˫ƽ�� ��4��������Ƭ
		vaoBind.release();
	}
	glDepthMask(GL_TRUE);  //������ɺ�  һ��Ҫ����Ȼ���������Ϊ�ɶ���д��ʽ�����������δ֪�Ĵ���
	m_shaderObject.release();
	m_mutexRender.unlock();  //������Ⱦ����
}

void OpenGLWidget3D::LoadProstateObj(ObjDataPtr prostateObjPtr)
{
	m_mutexRender.lock();  //���⿪ʼ��Ⱦ
	m_prostateObjPtr = prostateObjPtr;
	m_prostateObjPtr->GetBoundBox(m_fX1, m_fX2, m_fY1, m_fY2, m_fZ1, m_fZ2);
	m_mtxModel = QMatrix4x4();
	m_mtxModel.translate(QVector3D(-(m_fX1 + m_fX2) / 2, -(m_fY1 + m_fY2) / 2, -(m_fZ1 + m_fZ2) / 2));  //�൱�ڰ�ģ���ƶ�����boundboxΪ����
	m_fRadius = 1.5*sqrt(pow(m_fX2 - m_fX1, 2) + pow(m_fY2 - m_fY1, 2) + pow(m_fZ2 - m_fZ1, 2));  //�������뾶 �趨ΪboundBox�Խ��߳��ȵ�1.5��
	this->UpdateShader();  //ʹ�ò���ǰ�������� ������ɫ������
	this->LoadAnnotationToGPU();  //�����(boundBox)��������GPU
	this->LoadObjToGPU(m_prostateObjPtr, m_vaoProstate, m_vboProstate, m_eboProstate);  //��prostate��������GPU
	m_mutexRender.unlock();  //���Կ�ʼ��Ⱦ
}

void OpenGLWidget3D::LoadLesionObj(ObjDataPtr lesionObjPtr)
{
	m_mutexRender.lock();  //���⿪ʼ��Ⱦ
	m_lesionObjPtr = lesionObjPtr;
	this->LoadObjToGPU(m_lesionObjPtr, m_vaoLesion, m_vboLesion, m_eboLesion);
	m_mutexRender.unlock();  //���Կ�ʼ��Ⱦ
}

//void OpenGLWidget3D::SetWorldSize(float width, float height, float depth)
//{
//	m_fWidth = width;
//	m_fHeight = height;
//	m_fDepth = depth;
//	//���������뾶����Ϊ�Խ��߳��ȵ�һ��
	//m_fRadius = sqrt(width*width + height * height + depth * depth);
	//m_fRadius = width > height ? width : height;
	//m_fRadius = m_fRadius > depth ? m_fRadius : depth;
//}

void OpenGLWidget3D::SetTransversePlane(Coordinate cLeftTop, Coordinate cRightTop, Coordinate cLeftBottom, Coordinate cRightBottom)
{
	int i;
	Coordinate v1, v2, vNormal;
	v1 = cLeftTop - cLeftBottom;
	v2 = cLeftTop - cRightTop;
	vNormal = v1.GetCrossProduct(v2);
	vNormal.w = 0;  //normal������
	vNormal.Normalize();
	//���õ�����
	m_verticesPlane[0] = cLeftTop.x;
	m_verticesPlane[1] = cLeftTop.y;
	m_verticesPlane[2] = cLeftTop.z;
	m_verticesPlane[6] = cRightTop.x;
	m_verticesPlane[7] = cRightTop.y;
	m_verticesPlane[8] = cRightTop.z;
	m_verticesPlane[12] = cLeftBottom.x;
	m_verticesPlane[13] = cLeftBottom.y;
	m_verticesPlane[14] = cLeftBottom.z;
	m_verticesPlane[18] = cRightBottom.x;
	m_verticesPlane[19] = cRightBottom.y;
	m_verticesPlane[20] = cRightBottom.z;
	//���÷�����
	for (i = 0; i < 4; i++)
	{
		m_verticesPlane[i * 6 + 3] = vNormal.x;
		m_verticesPlane[i * 6 + 4] = vNormal.y;
		m_verticesPlane[i * 6 + 5] = vNormal.z;
	}
	//�������Դ�
	m_mutexRender.lock();  //���⿪ʼ��Ⱦ
	if (!m_vboPlane.isCreated())  //���vboPlane��δ��������������Ҳ�޷��������Դ�
		return;
	m_vboPlane.bind();  
	m_vboPlane.allocate(m_verticesPlane, sizeof(m_verticesPlane));  
	m_vboPlane.release();
	m_mutexRender.unlock();  //���Կ�ʼ��Ⱦ
}

void OpenGLWidget3D::SetSagittalPlane(Coordinate cLeftTop, Coordinate cRightTop, Coordinate cLeftBottom, Coordinate cRightBottom)
{
	int i;
	Coordinate v1, v2, vNormal;
	v1 = cLeftTop - cLeftBottom;
	v2 = cLeftTop - cRightTop;
	vNormal = v1.GetCrossProduct(v2);
	vNormal.w = 0;  //normal������
	vNormal.Normalize();
	//���õ�����
	m_verticesPlane[24] = cLeftTop.x;
	m_verticesPlane[25] = cLeftTop.y;
	m_verticesPlane[26] = cLeftTop.z;
	m_verticesPlane[30] = cRightTop.x;
	m_verticesPlane[31] = cRightTop.y;
	m_verticesPlane[32] = cRightTop.z;
	m_verticesPlane[36] = cLeftBottom.x;
	m_verticesPlane[37] = cLeftBottom.y;
	m_verticesPlane[38] = cLeftBottom.z;
	m_verticesPlane[42] = cRightBottom.x;
	m_verticesPlane[43] = cRightBottom.y;
	m_verticesPlane[44] = cRightBottom.z;
	//���÷�����
	for (i = 5; i < 8; i++)
	{
		m_verticesPlane[i * 6 + 3] = vNormal.x;
		m_verticesPlane[i * 6 + 4] = vNormal.y;
		m_verticesPlane[i * 6 + 5] = vNormal.z;
	}
	//�������Դ�
	m_mutexRender.lock();  //���⿪ʼ��Ⱦ
	if (!m_vboPlane.isCreated())  //���vboPlane��δ��������������Ҳ�޷��������Դ�
		return;
	m_vboPlane.bind();  
	m_vboPlane.allocate(m_verticesPlane, sizeof(m_verticesPlane));
	m_vboPlane.release();
	m_mutexRender.unlock();  //���Կ�ʼ��Ⱦ
}

void OpenGLWidget3D::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bLeftActive = true;
		m_mouseLastPos = event->pos();
		int temp = 0;
	}
}

void OpenGLWidget3D::mouseReleaseEvent(QMouseEvent * event)
{
	Q_UNUSED(event);  //event�ò���
	m_bLeftActive = false;
}

void OpenGLWidget3D::mouseMoveEvent(QMouseEvent * event)
{
	//TODO
	m_mouseCurPos = event->pos();

	if (m_bLeftActive)  //�������϶� ʹ��trackball��ת�ӽ�
	{
		QVector3D dirWorld,dirLocal;  //��ת��
		float angle;  //��ת��
		this->calRotateParameters(m_mouseLastPos, m_mouseCurPos, dirWorld, angle);  //������ת�����ת��
		dirLocal = m_mtxView.inverted().mapVector(dirWorld);  //��trackball����ת��ӳ��Ϊopengl����ϵ�µ���ת��
		
		angle *= m_fRotateSpeed;
		m_mtxView.rotate(angle, dirLocal);  //��Ӧ��ת�۲����
		qDebug() << "dir" << dirLocal;
		this->update();  //�ػ�
	}

	m_mouseLastPos = m_mouseCurPos;
}

void OpenGLWidget3D::wheelEvent(QWheelEvent * event)
{
	//TODO
}

QVector3D OpenGLWidget3D::projectToTrackball(const QPoint & screenCoords) const
{
	float sCoordsX = screenCoords.x(), sCoordsY = this->height() - screenCoords.y();  //�������Ͻ�Ϊԭ���2d���꣬��Ϊ�����½�Ϊԭ���2D����
	float z = 0.0f;  //ӳ�䵽��trackball�ϵ�z����
	float r2 = m_fTrackballRadius * m_fTrackballRadius;

	QVector2D pos2D(sCoordsX / this->width() - 0.5f, sCoordsY / this->height() - 0.5f);  //��2D��opengl����x��y ӳ�䵽[-0.5,0.5]��������
	//qDebug()<<"pos2D"<< pos2D;
	if (pos2D.lengthSquared() <= r2 * 0.5f) {  //ӳ��Ϊ���淶Χ�ڵĵ�
		z = sqrt(r2 - pos2D.lengthSquared());
	}
	else {  //ӳ��Ϊ˫�����ϵĵ�
		z = r2 * 0.5f / pos2D.length();
	}
	QVector3D pos3D(pos2D, z);
	//qDebug()<<p3d;
	return pos3D;
}

void OpenGLWidget3D::calRotateParameters(const QPoint & firstPoint, const QPoint & nextPoint, QVector3D & dir, float & angle)
{
	QVector3D lastPos3D = projectToTrackball(firstPoint).normalized();  //����������2D���Ϊtrackball��������
	QVector3D currentPos3D = projectToTrackball(nextPoint).normalized();  //����������2D���Ϊ3d trackball��������

	// ������ת�᷽��
	dir = QVector3D::crossProduct(lastPos3D, currentPos3D);  //��ת�᷽���������������Ĳ��

	// ������ת��
	float dotVal;
	dotVal = QVector3D::dotProduct(lastPos3D, currentPos3D);
	dotVal = dotVal < 1 ? dotVal : 1;
	dotVal = dotVal > -1 ? dotVal : 1;
	angle = acos(dotVal);  //��ת��Ϊ�����������ļн�(������)

	//qDebug()<<"dir:"<<dir<<"angle:"<<angle;
}

void OpenGLWidget3D::CreateShader()
{
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	std::string strResDir = exePath;
	strResDir = strResDir.substr(0, strResDir.rfind("\\")) + "\\Res\\";
	QString strFileName;
	/***********�������ڱ�ע����ɫ������**********/
	strFileName = QString::fromStdString(strResDir + "annotation.vert");
	if (!m_shaderAnnotation.addShaderFromSourceFile(QOpenGLShader::Vertex, strFileName))  //���붥����ɫ��
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_shaderAnnotation.log();
		QMessageBox::information(0, "����", "��ȡ��ע��ɫ��vert�ļ�ʧ��");
	}
	strFileName = QString::fromStdString(strResDir + "annotation.frag");
	if (!m_shaderAnnotation.addShaderFromSourceFile(QOpenGLShader::Fragment, strFileName))  //����Ƭ����ɫ��
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_shaderAnnotation.log();
		QMessageBox::information(0, "����", "��ȡ��ע��ɫ��frag�ļ�ʧ��");
	}
	if (!m_shaderAnnotation.link())  //����������ɫ�� ������ɫ������
	{
		qDebug() << "shaderProgram link failed!" << m_shaderAnnotation.log();
		QMessageBox::information(0, "����", "���ӱ�ע��ɫ��ʧ��");
	}
	/***********��������ģ�͵���ɫ������**********/
	strFileName = QString::fromStdString(strResDir + "object.vert");
	if (!m_shaderObject.addShaderFromSourceFile(QOpenGLShader::Vertex, strFileName))  //���붥����ɫ��
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_shaderObject.log();
		QMessageBox::information(0, "����", "��ȡģ����ɫ��vert�ļ�ʧ��");
	}
	strFileName = QString::fromStdString(strResDir + "object.frag");
	if (!m_shaderObject.addShaderFromSourceFile(QOpenGLShader::Fragment, strFileName))  //����Ƭ����ɫ��
	{
		qDebug() << "shaderProgram addShaderFromSourceFile failed!" << m_shaderObject.log();
		QMessageBox::information(0, "����", "��ȡģ����ɫ��frag�ļ�ʧ��");
	}
	if (!m_shaderObject.link())  //����������ɫ�� ������ɫ������
	{
		qDebug() << "shaderProgram link failed!" << m_shaderObject.log();
		QMessageBox::information(0, "����", "����ģ����ɫ��ʧ��");
	}
}

void OpenGLWidget3D::UpdateShader()
{
	m_shaderObject.bind();  //����ǰ��ɫ������󶨵�opengl������
	//����Ĭ�ϵ��������
	m_defaultViewerPos = QVector3D(0.577*m_fRadius, 0.577*m_fRadius, 0.577*m_fRadius);
	m_defaultUpDir = QVector3D(0.0f, 1.0f, 0.0f);
	//����ģ�;���(���ڽ�boundbox��������)
	m_shaderObject.setUniformValue("model", m_mtxModel);
	//���ù۲����
	//m_mtxView.lookAt(QVector3D(0, 0, -m_fRadius), QVector3D(0, 0, 0), m_defaultUpDir);  //��ǰ��
	m_mtxView.lookAt(m_defaultViewerPos, QVector3D(0.0f, 0.0f, 0.0f), m_defaultUpDir);  //45�Ƚ�
	m_shaderObject.setUniformValue("view", m_mtxView);
	//����ͶӰ����
	m_mtxProjection.perspective(45.0f, 1.0f*this->width() / this->height(), 0.1f, 500.0f);  //����͸��ͶӰ
	//m_mtxProjection.ortho(-m_fRadius, m_fRadius, -m_fRadius, m_fRadius, -2*m_fRadius, 2*m_fRadius);  //��������ͶӰ
	m_shaderObject.setUniformValue("projection", m_mtxProjection);
	//���ù۲��ߵ�λ��(�����ڼ������)
	m_shaderObject.setUniformValue("viewerPos", m_defaultViewerPos);
	m_shaderObject.release();
	/**************���ñ�ע��ɫ��********************/
	m_shaderAnnotation.bind();  //����ǰ��ɫ������󶨵�opengl������
	//����ģ�;���
	m_shaderAnnotation.setUniformValue("model", m_mtxModel);
	//���ù۲����
	m_shaderAnnotation.setUniformValue("view", m_mtxView);
	//����ͶӰ����
	m_shaderAnnotation.setUniformValue("projection", m_mtxProjection);
	m_shaderAnnotation.release();
}

void OpenGLWidget3D::LoadAnnotationToGPU()
{
	this->makeCurrent();
	//boundbox�������8������
	GLfloat vertices[] = {
		m_fX1,m_fY1,m_fZ1,
		m_fX1,m_fY1,m_fZ2,
		m_fX1,m_fY2,m_fZ1,
		m_fX1,m_fY2,m_fZ2,
		m_fX2,m_fY1,m_fZ1,
		m_fX2,m_fY1,m_fZ2,
		m_fX2,m_fY2,m_fZ1,
		m_fX2,m_fY2,m_fZ2
	};
	//�������12����
	unsigned int indices[] = {
	0,1,  //z����
	2,3,
	4,5,
	6,7,
	0,2,  //y����
	1,3,
	4,6,
	5,7,
	0,4,  //x����
	1,5,
	2,6,
	3,7
	};

	QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoAnnotation);
	m_vboAnnotation.create();  //��opengl�������д����ö���buffer����
	m_vboAnnotation.bind();  //����˶�������Ļ������󶨵���ǰOpenGL������
	m_vboAnnotation.allocate(vertices, sizeof(vertices));  //����ģ�Ͷ������ݵ��Դ�
	m_eboAnnotation.create();  //��opengl�������д���������buffer����
	m_eboAnnotation.bind();  //����˶�������Ļ������󶨵���ǰopengl������
	m_eboAnnotation.allocate(indices, sizeof(indices));  //����ģ���������ݵ��Դ�

	//֪ͨvao��ν���vbo����
	int attr = -1;
	attr = m_shaderAnnotation.attributeLocation("aPos");  //Ϊvao���� ��ν������㻺�����vbo�ڵĶ�����������
	m_shaderAnnotation.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 3);  //Ϊvao���� ��ν������㻺�����vbo�ڵĶ�����������
	m_shaderAnnotation.enableAttributeArray(attr);  //ʹλ��attr�������� ��λ�����飬�ɱ���ɫ��ʹ��

	//�����opengl�����ĵİ�(������Ȼ���Դ���)
	vaoBind.release();
	m_vboAnnotation.release();
	this->doneCurrent();
}

void OpenGLWidget3D::LoadPlaneToGPU()
{
	this->makeCurrent();
	unsigned int indices[] = {  //˫ƽ�� ������Ƭ������һ������ ����Ķ�
	0,1,2,  //�����
	1,2,3,
	4,5,6,  //ʸ״��
	5,6,7
	};

	QOpenGLVertexArrayObject::Binder vaoBind(&m_vaoPlane);  //����binder�󶨣��Ӷ�ʹ�øö����������vao,��binder��������ʱ���Զ�����vao.release()
	m_vboPlane.create();  //��opengl�������д����ö���buffer����
	m_vboPlane.bind();  //����˶�������Ļ������󶨵���ǰOpenGL������
	m_vboPlane.allocate(m_verticesPlane, sizeof(m_verticesPlane));  //����ģ�Ͷ��㼰�������ݵ��Դ�
	m_eboPlane.create();  //��opengl�������д���������buffer����
	m_eboPlane.bind();  //����˶�������Ļ������󶨵���ǰopengl������
	m_eboPlane.allocate(indices, sizeof(indices));  //����ģ���������ݵ��Դ�

	//֪ͨvao��ν���vbo����
	int attr = -1;
	attr = m_shaderObject.attributeLocation("aPos");
	m_shaderObject.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);  //Ϊvao���� ��ν������㻺�����vbo�ڵĶ�����������
	m_shaderObject.enableAttributeArray(attr);  //ʹλ��attr�������� ��λ�����飬�ɱ���ɫ��ʹ��
	attr = m_shaderObject.attributeLocation("aNormal");
	m_shaderObject.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);  //Ϊvao���� ��ν������㻺�����vbo�ڵķ���������
	m_shaderObject.enableAttributeArray(attr);  //ʹλ��attr�������� �����������飬�ɱ���ɫ��ʹ��

	//�����opengl�����ĵİ�(������Ȼ���Դ���)
	vaoBind.release();
	m_vboPlane.release();
	m_eboPlane.release();
	this->doneCurrent();
}

void OpenGLWidget3D::LoadObjToGPU(ObjDataPtr objPtr, QOpenGLVertexArrayObject & vao, QOpenGLBuffer & vbo, QOpenGLBuffer & ebo)
{
	this->makeCurrent();
	QOpenGLVertexArrayObject::Binder vaoBind(&vao);  //����binder�󶨣��Ӷ�ʹ�øö����������vao,��binder��������ʱ���Զ�����vao.release()

	vbo.create();  //��opengl�������д����ö���buffer����
	vbo.bind();  //����˶�������Ļ������󶨵���ǰOpenGL������
	vbo.allocate(objPtr->GetVboArray(), objPtr->GetVertexNum() * sizeof(float) * 6);  //����ģ�Ͷ��㼰�������ݵ��Դ�

	ebo.create();  //��opengl�������д���������buffer����
	ebo.bind();  //����˶�������Ļ������󶨵���ǰopengl������
	ebo.allocate(objPtr->GetEboArray(), objPtr->GetTriangleNum() * sizeof(unsigned int) * 3);  //����ģ���������ݵ��Դ�

	int attr = -1;
	//֪ͨvao��ν���vbo����
	attr = m_shaderObject.attributeLocation("aPos");
	m_shaderObject.setAttributeBuffer(attr, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);  //Ϊvao���� ��ν������㻺�����vbo�ڵĶ�����������
	m_shaderObject.enableAttributeArray(attr);  //ʹλ��attr�������� ��λ�����飬�ɱ���ɫ��ʹ��
	attr = m_shaderObject.attributeLocation("aNormal");
	m_shaderObject.setAttributeBuffer(attr, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);  //Ϊvao���� ��ν������㻺�����vbo�ڵķ���������
	m_shaderObject.enableAttributeArray(attr);  //ʹλ��attr�������� �����������飬�ɱ���ɫ��ʹ��

	//�����opengl�����ĵİ�(������Ȼ���Դ���)
	vaoBind.release();
	vbo.release();
	ebo.release();

	this->doneCurrent();
}
