#pragma once
#include "inc\controller_interface.h"

class CLoginController :
	public ILogin
{
public:
	CLoginController();
	~CLoginController();

	// ��¼��֤
	eERR chkLogin(const QString &uname, const QString &pswd, QString &err);
	// ע��
	eERR regLogin(const QString &uname, const QString &pswd, QString &err);
	// �޸�����
	eERR modifyLogin(const QString &uname, const QString &pswd, QString &err);


private:
	CLogin m_curLogin;
};

