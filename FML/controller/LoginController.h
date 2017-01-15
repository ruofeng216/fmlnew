#pragma once
#include "inc\controller_interface.h"

class CLoginController :
	public ILogin
{
public:
	CLoginController();
	~CLoginController();

	// µÇÂ¼ÑéÖ¤
	eERR chkLogin(const QString &uname, const QString &pswd);
	// ×¢²á
	eERR regLogin(const QString &uname, const QString &pswd);
	// ÐÞ¸ÄÃÜÂë
	eERR modifyLogin(const QString &uname, const QString &pswd);


private:
	CLogin m_curLogin;
};

