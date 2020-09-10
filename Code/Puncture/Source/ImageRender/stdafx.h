// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlimage.h>
#include<gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#ifdef _DEBUG
#pragma comment(lib, "log4cplusUD.lib")
#else
#pragma comment(lib, "log4cplusU.lib")
#endif

using namespace log4cplus;

//#define DISABLE_DEBUGLOG
#ifndef FS_DEBUGLOG
#ifdef DISABLE_DEBUGLOG
#define FS_DEBUGLOG(logfunction, logevent) {}
#else 
#define FS_DEBUGLOG(logfunction, logevent) {\
	log4cplus::Logger _logger = log4cplus::Logger::getInstance(_T("ImageRender")); \
	logfunction(_logger, logevent);  };
#endif

#define FS_DEBUG(x) FS_DEBUGLOG(LOG4CPLUS_DEBUG, x)
#define FS_TRACE(x) FS_DEBUGLOG(LOG4CPLUS_TRACE, x)
#define FS_ERROR(x) FS_DEBUGLOG(LOG4CPLUS_ERROR, x)
#define FS_FATAL(x) FS_DEBUGLOG(LOG4CPLUS_FATAL, x)

#endif
