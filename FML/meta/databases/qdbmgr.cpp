#include "qdbmgr.h"
#include "view/commonui/message_box_widget.h"
#include <QDebug>
#include "util/util.h"
#include <QCoreApplication>
#include <QSqlQuery>


QDBMgr::QDBMgr(const QString &dbname, QObject *parent)
	: QObject(parent)
	, m_db(dbname)
	, m_sqlQuery(NULL)
{
	if (!this->openDB())
	{
		qDebug() << "打开数据库失败!";
		ShowErrorMessage(tr("Database"), tr("Open DataBase Faile."), NULL);
	}
	initTables();
}

QDBMgr::~QDBMgr()
{
	if (NULL != m_sqlQuery)
	{
		delete m_sqlQuery;
		m_sqlQuery = NULL;
	}
	if (m_sqldb.isOpen())
	{
		m_sqldb.close();
	}
	QSqlDatabase::removeDatabase(m_db);
}

// tables data init.
void QDBMgr::initTables()
{
	m_tables[DB_TBNAME_LOGIN] = DB_TBSQL_LOGIN;
	m_sqlQuery->exec(DB_TBSQL_LOGIN);
}

// 打开数据库
bool QDBMgr::openDB()
{
	QDir::current().mkpath(QCoreApplication::applicationDirPath()+"/DataBase");
	m_sqldb = QSqlDatabase::addDatabase("QMYSQL3", m_db);
	m_sqldb.setDatabaseName(QCoreApplication::applicationDirPath() + "/DataBase/" + m_db + ".db");
	if (m_sqldb.isOpen())
	{
		m_sqlQuery = new QSqlQuery(m_sqldb);
		return true;
	}
	else
	{
		if (m_sqldb.open())
		{
			m_sqlQuery = new QSqlQuery(m_sqldb);
			return true;
		}
	}
	return false;
}
// 更新数据库
bool QDBMgr::updateDB()
{
	return true;
}