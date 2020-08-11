#pragma once
#include <string>
class AFX_EXT_CLASS Base64
{
public:
	Base64(void);
	~Base64(void);
	
	static std::string Encode(const unsigned char* Data,int DataByte);
    /*����
    DataByte
        [in]��������ݳ���,���ֽ�Ϊ��λ
    OutByte
        [out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ����
        ������ݵĳ���
    */
	 static std::string Decode(const char* Data,int DataByte,int& OutByte);
};

