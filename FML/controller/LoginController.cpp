#include "LoginController.h"
#include "metadatabase.h"
#include "util/pro.h"

CLoginController::CLoginController()
{
}
CLoginController::~CLoginController()
{
}
// µÇÂ¼ÑéÖ¤
eERR CLoginController::chkLogin(const QString &uname, const QString &pswd)
{
	FUNCLOG("CLoginController::chkLogin(const QString &uname, const QString &pswd)");
	CLogin cinfo;
	if (METADATABASE->getLoginInfo(uname, cinfo))
	{
		const QString &u = cinfo.getUname();
		if (u == uname)
		{
			const QString &p = cinfo.getPassword();
			if (p == pswd)
			{
				m_curLogin.setUname(uname);
				m_curLogin.setPassword(pswd);
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
	FUNCLOG("CLoginController::regLogin(const QString &uname, const QString &pswd)");
	eERR e = chkLogin(uname, pswd);
	if (e == e_Success)
		return e_Success;
	if (e == e_NoUser)
	{
		if (!METADATABASE->setLoginInfo(CLogin(uname, pswd)))
			return e_RegErr;
		m_curLogin.setUname(uname);
		m_curLogin.setPassword(pswd);
		return e_Success;
	}
	if (e == e_ErrPswd)
		return e_Exist;
	return e_RegErr;
}
// ÐÞ¸ÄÃÜÂë
eERR CLoginController::modifyLogin(const QString &uname, const QString &pswd)
{
	FUNCLOG("CLoginController::modifyLogin(const QString &uname, const QString &pswd)");
	CLogin cinfo;
	if (METADATABASE->getLoginInfo(uname, cinfo))
	{
		if (!METADATABASE->updateLoginInfo(CLogin(uname, pswd)))
			return e_ModifyErr;
		return e_Success;
	}
	return e_NoUser;
}
