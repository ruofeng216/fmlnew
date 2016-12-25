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
	e_Success = 0, // 成功
	e_NoUser = 1,      // 用户不存在
	e_ErrPswd = 2,     // 密码错误
	e_Exist = 3,       // 用户存在
	e_RegErr = 4,      // 注册失败
	e_ModifyErr = 5,    // 修改失败

} eERR;





//////////////////////////////////
// color
const QString c_282D31 = "#282D31";
const QString c_323232 = "#323232";