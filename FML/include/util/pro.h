#pragma once

#include <string>
#include <vector>

typedef std::string STRINGANSI;
typedef std::string STRINGUTF8;
typedef std::string STRINGBASE64;

typedef unsigned long long ULONG64;
typedef unsigned int UINT32;

typedef enum e_ErrType
{
	e_Success = 0, // �ɹ�
	e_NoUser = 1,      // �û�������
	e_ErrPswd = 2,     // �������
	e_Exist = 3,       // �û�����
	e_RegErr = 4,      // ע��ʧ��
	e_ModifyErr = 5,    // �޸�ʧ��

} eERR;





//////////////////////////////////
// color
const QString c_282D31 = "#282D31";
const QString c_323232 = "#323232";