#pragma once
#include "inc\controller_interface.h"

class CLoginController :
	public ILogin
{
public:
	CLoginController();
	~CLoginController();

	// ��¼��֤
	eERR chkLogin(const QString &uname, const QString &pswd);
	// ע��
	eERR regLogin(const QString &uname, const QString &pswd);
	// �޸�����
	eERR modifyLogin(const QString &uname, const QString &pswd);


private:
	CLogin m_curLogin;
};

