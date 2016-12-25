#include "LoginController.h"


CLoginController::CLoginController()
{
	init();
}
CLoginController::~CLoginController()
{
}
// µÇÂ¼ÑéÖ¤
eERR CLoginController::chkLogin(const QString &uname, const QString &pswd)
{
	for (QVector<CLogin>::const_iterator itor = m_logins.begin();
		itor != m_logins.end(); itor++)
	{
		QString u = itor->getUname();
		if (u == uname)
		{
			QString p = itor->getPassword();
			if (p == pswd)
			{
				return e_Success;
			}
			else
				return e_ErrPswd;
		}
	}
	return e_NoUser;
}
// ×¢²á
eERR CLoginController::regLogin(const QString &uname, const QString &pswd)
{
	eERR e = chkLogin(uname, pswd);
	if (e == e_Success)
		return e_Success;
	if (e == e_NoUser)
	{
		m_logins.append(CLogin(uname, pswd));
		// TODO: update db

		return e_Success;
	}
	if (e == e_ErrPswd)
		return e_Exist;
	return e_RegErr;
}
// ÐÞ¸ÄÃÜÂë
eERR CLoginController::modifyLogin(const QString &uname, const QString &pswd)
{
	return e_Success;
}

void CLoginController::init()
{
	// load from database
	m_logins.append(CLogin("root", "123456"));
}