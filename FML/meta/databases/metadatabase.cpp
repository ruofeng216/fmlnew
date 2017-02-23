#include "metadatabase.h"
#include "qdbmgr.h"
#include "util/pro.h"

MetaDatabase::MetaDatabase(QObject *parent)
	: QObject(parent)
{
	m_DbMgr = new QDBMgr(this);
}

MetaDatabase::~MetaDatabase()
{
}

// 获取实例
MetaDatabase *MetaDatabase::instance()
{
	static MetaDatabase s;
	return &s;
}


bool MetaDatabase::getLoginInfo(const QString &username, CLogin &lval)
{
	FUNCLOG("MetaDatabase::getLoginInfo(const QString &username, CLogin &lval)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << username;
	QStringList fieldList;
	fieldList << "user" << "pswd";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectLoginUser, paramList, fieldList, results);
	if (results.isEmpty()) return false;
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 2)
		{
			lval.setUname(results[i][0].toString());
			lval.setPassword(results[i][1].toString());
			return true;
		}
	}
	return false;
}
bool MetaDatabase::setLoginInfo(const CLogin &uinfo)
{
	FUNCLOG("MetaDatabase::setLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getUname() << uinfo.getPassword();
	if (m_DbMgr->ExecuteSQL(DB_SQL_InsertLoginUser, paramList))
		return true;
	return false;
}
bool MetaDatabase::updateLoginInfo(const CLogin &uinfo)
{
	FUNCLOG("MetaDatabase::updateLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getPassword() << uinfo.getUname();
	if (m_DbMgr->ExecuteSQL(DB_SQL_UpdateLoginUser, paramList))
		return true;
	return false;
}

// 金融日历
bool MetaDatabase::isExistFinancialCalendar(const CFinancialCalendar &val)
{
	FUNCLOG("MetaDatabase::isExistFinancialCalendar(const CFinancialCalendar &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getDate();
	QStringList fieldList;
	fieldList << "bwdate";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectLoginUser, paramList, fieldList, results);
	if (results.isEmpty()) return false;
	return true;
}
bool MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val)
{
	FUNCLOG("MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getYear() << val.getDate() << val.getHolidayType() << val.getHolidayinfo();
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplaceFinancialHoliday, paramList))
		return true;
	return false;
}
bool MetaDatabase::removeFinancialCalendar(int val)
{
	FUNCLOG("MetaDatabase::removeFinancialCalendar(int val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val;
	if (m_DbMgr->ExecuteSQL(DB_SQL_DeleteFinancialHoliday, paramList))
		return true;
	return false;
}
bool MetaDatabase::getFinancialCalendar(QMap<int, CFinancialCalendar> &val)
{
	FUNCLOG("MetaDatabase::getFinancialCalendar(const QMap<int, CFinancialCalendar> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "bwyear" << "bwdate" << "daytype" << "annotation";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectLoginUser, paramList, fieldList, results);
	if (results.isEmpty()) return false;
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 4)
		{
			CFinancialCalendar fc;
			fc.setYear(results[i][0].toInt());
			fc.setDate(results[i][1].toInt());
			fc.setHolidayType(results[i][2].toInt());
			fc.setHolidayinfo(results[i][3].toString());
			val[fc.getDate()] = fc;
		}
	}
	return true;
}