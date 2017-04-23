#pragma once
#include "inc\controller_interface.h"

class CLoginController :
	public ILogin
{
public:
	CLoginController();
	~CLoginController();

	// µÇÂ¼ÑéÖ¤
	eERR chkLogin(const QString &uname, const QString &pswd, QString &err);
	// ×¢²á
	eERR regLogin(const QString &uname, const QString &pswd, QString &err);
	// ÐÞ¸ÄÃÜÂë
	eERR modifyLogin(const QString &uname, const QString &pswd, QString &err);


private:
	CLogin m_curLogin;
};

