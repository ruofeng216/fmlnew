#include "qdbmgr.h"

QDBMgr::QDBMgr(const QString &dbname, QObject *parent)
	: QObject(parent)
	, m_db(dbname)
	, m_sqlQuery(NULL)
{
}

QDBMgr::~QDBMgr()
{
}

// tables data init.
void QDBMgr::initTables()
{

}

// 打开数据库
bool QDBMgr::openDB()
{
	return true;
}
// 更新数据库
bool QDBMgr::updateDB()
{
	return true;
}