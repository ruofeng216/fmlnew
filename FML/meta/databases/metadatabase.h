#pragma once

#include <QObject>
#include "util/datatype.h"

#define METADATABASE MetaDatabase::instance()

class QDBMgr;
class MetaDatabase : public QObject
{
	Q_OBJECT
public:
	// 获取实例
	static MetaDatabase *instance();
	// login
	bool getLoginInfo(const QString &username, CLogin &lval);
	bool setLoginInfo(const CLogin &uinfo);
	bool updateLoginInfo(const CLogin &uinfo);
	// 金融日历
	bool isExistFinancialCalendar(const CFinancialCalendar &val);
	bool setFinancialCalendar(const CFinancialCalendar &val);
	bool removeFinancialCalendar(int val);
	bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val);

private:
	MetaDatabase(QObject *parent = NULL);
	~MetaDatabase();

private:
	QDBMgr *m_DbMgr;

};
