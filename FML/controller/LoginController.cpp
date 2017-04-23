#include "LoginController.h"
#include "metadatabase.h"
#include "util/pro.h"

CLoginController::CLoginController()
{
}
CLoginController::~CLoginController()
{
}
// ��¼��֤
eERR CLoginController::chkLogin(const QString &uname, const QString &pswd, QString &err)
{
	FUNCLOG("CLoginController::chkLogin(const QString &uname, const QString &pswd)");
	CLogin cinfo;
	if (METADATABASE->getLoginInfo(uname, cinfo, err))
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
// ע��
eERR CLoginController::regLogin(const QString &uname, const QString &pswd, QString &err)
{
	FUNCLOG("CLoginController::regLogin(const QString &uname, const QString &pswd)");
	eERR e = chkLogin(uname, pswd, err);
	if (e == e_Success)
		return e_Success;
	if (e == e_NoUser)
	{
		if (!METADATABASE->setLoginInfo(CLogin(uname, pswd), err))
			return e_RegErr;
		m_curLogin.setUname(uname);
		m_curLogin.setPassword(pswd);
		return e_Success;
	}
	if (e == e_ErrPswd)
		return e_Exist;
	return e_RegErr;
}
// �޸�����
eERR CLoginController::modifyLogin(const QString &uname, const QString &pswd, QString &err)
{
	FUNCLOG("CLoginController::modifyLogin(const QString &uname, const QString &pswd)");
	CLogin cinfo;
	if (METADATABASE->getLoginInfo(uname, cinfo, err))
	{
		if (!METADATABASE->updateLoginInfo(CLogin(uname, pswd), err))
			return e_ModifyErr;
		return e_Success;
	}
	return e_NoUser;
}
