#include "qdbmgr.h"
#include "view/commonui/message_box_widget.h"
#include <QDebug>
#include "util/util.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include "viewsignalmanager/viewsignalmanager.h"
#include <QMutexLocker> 
#include <QRegExp>
#include <QSet>

QDBMgr::QDBMgr(QObject *parent)
	: QObject(parent)
	, m_sqlQuery(NULL)
{
	if (!this->openDB())
	{
		qDebug() << "打开数据库失败!";
		ShowErrorMessage(tr("Database"), tr("Open DataBase Faile."), NULL);
		emit VIEWSIGNAL->sigExitProgramme();
		return;
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
	QSqlDatabase::removeDatabase(m_dbname);
}

// tables data init.
void QDBMgr::initTables()
{
	m_tables[DB_TBNAME_LOGIN] = DB_TBSQL_LOGIN;
	m_tables[DB_TBNAME_FINANCIALCALENDAR] = DB_TBSQL_FINANCIALCALENDAR;
	m_tables[DB_TBNAME_PORTFOLIO] = DB_TBSQL_PORTFOLIO;
	m_tables[DB_TBNAME_PRODUCT] = DB_TBSQL_PRODUCT;
	m_tables[DB_TBNAME_PARADICT] = DB_TBSQL_PARADICT;
	m_tables[DB_TBNAME_KEYPOINT] = DB_TBSQL_KEYPOINT;

	for (QMap<QString, QString>::const_iterator itor = m_tables.begin();
		itor != m_tables.end(); itor++)
	{
		if (hasTable(itor.key()))
		{
			if (!updateTable(itor.key()))
			{
				qDebug() << "更新数据库失败!";
				ShowErrorMessage(tr("Database"), tr("Update DataBase Faile."), NULL);
				emit VIEWSIGNAL->sigExitProgramme();
				return;
			}
		}
		else
		{
			m_sqlQuery->clear();
			if (!m_sqlQuery->exec(itor.value()))
			{
				qDebug() << "创建表失败!" << "table name: "<< itor.value();
				ShowErrorMessage(tr("Database"), tr("Create table Faile."), NULL);
				emit VIEWSIGNAL->sigExitProgramme();
				return;
			}
		}
	}

	QSettings configIniRead(qutil::qBaseConfigPath(), QSettings::IniFormat);
	bool DropUselessTable = configIniRead.value("/DataBase/DropUselessTable").toBool();
	if (DropUselessTable)
	{
		QString tablesSql = DB_GetTables.arg(m_dbname);
		QVariantList tables;
		QueryOneField(tablesSql, QVariantList(), tables);
		if (tables.isEmpty())
		{
			if (m_sqlQuery)
				qDebug() << m_sqlQuery->lastError().type() <<
				m_sqlQuery->lastError().number() <<
				m_sqlQuery->lastError().driverText() <<
				m_sqlQuery->lastError().databaseText();
			ShowErrorMessage(tr("Database"), tr("Get tables Faile."), NULL);
			emit VIEWSIGNAL->sigExitProgramme();
			return;
		}
		for (QVariantList::iterator itor = tables.begin(); itor != tables.end(); itor++)
		{
			if (!m_tables.contains(itor->toString()))
			{
				DropTable(itor->toString());
			}
		}
	}
}

bool QDBMgr::hasTable(const QString &table)
{
	QString sql = DB_HasTable.arg(m_dbname).arg(table);
	try
	{
		if (NULL == m_sqlQuery)
		{
			return false;
		}
		m_sqlQuery->clear();

		m_sqlQuery->prepare(sql);
		m_sqlQuery->exec();
		return m_sqlQuery->next();
	}
	catch (...)
	{
		qDebug() << QString("DB Error: sql:") << sql;
		return false;
	}
}

bool QDBMgr::updateTable(const QString &table)
{
	QString cloSql = DB_TB_COLUMNS.arg(m_dbname).arg(table);
	QVariantList clos;
	QueryOneField(cloSql, QVariantList(), clos);
	if (clos.isEmpty())
	{
		if (m_sqlQuery)
			qDebug() << m_sqlQuery->lastError().type() <<
						m_sqlQuery->lastError().number() <<
						m_sqlQuery->lastError().driverText() <<
						m_sqlQuery->lastError().databaseText();
		return false;
	}
	
	m_sqlQuery->clear();
	QString tmpSql = m_tables[table].replace(m_tables[table].indexOf(table), table.length(), table + "tmp");
	m_sqlQuery->prepare(tmpSql);
	if (!m_sqlQuery->exec())
	{
		qDebug() << m_sqlQuery->lastError().type() <<
					m_sqlQuery->lastError().number() <<
					m_sqlQuery->lastError().driverText() <<
					m_sqlQuery->lastError().databaseText();
		return false;
	}

	m_sqlQuery->clear();
	QString tmpcloSql = DB_TB_COLUMNSINFO.arg(m_dbname).arg(table + "tmp");
	QList<QVariantList> tmpclos;
	QueryFields(tmpcloSql, QVariantList(), QStringList() << "COLUMN_NAME" << "COLUMN_DEFAULT" << "COLUMN_TYPE", tmpclos);
	if (tmpclos.isEmpty())
	{
		if (m_sqlQuery)
			qDebug() << m_sqlQuery->lastError().type() <<
			m_sqlQuery->lastError().number() <<
			m_sqlQuery->lastError().driverText() <<
			m_sqlQuery->lastError().databaseText();
		return false;
	}
	
	DropTable(table + "tmp");

	if (clos.size() < tmpclos.size())
	{
		m_sqldb.transaction();
		QStringList newColumn;
		for (QList<QVariantList>::iterator itorNew = tmpclos.begin();
			itorNew != tmpclos.end(); itorNew++)
		{
			if (!clos.contains(*(itorNew->begin())))
			{
				m_sqlQuery->clear();
				QString sqladd = DB_ALERTTABLE.arg(table).arg(itorNew->begin()->toString()).arg(itorNew->rbegin()->toString());
				if (!m_sqlQuery->exec(sqladd))
				{
					m_sqldb.rollback();
					if (m_sqlQuery)
						qDebug() << m_sqlQuery->lastError().type() <<
						m_sqlQuery->lastError().number() <<
						m_sqlQuery->lastError().driverText() <<
						m_sqlQuery->lastError().databaseText();
					return false;
				}
				if (!itorNew->at(1).toString().isEmpty())
				{
					m_sqlQuery->clear();
					QString sql2 = DB_UPDATEDATA.arg(table).arg(itorNew->begin()->toString()).arg(itorNew->at(1).toString());
					if (!m_sqlQuery->exec(sql2))
					{
						m_sqldb.rollback();
						if (m_sqlQuery)
							qDebug() << m_sqlQuery->lastError().type() <<
							m_sqlQuery->lastError().number() <<
							m_sqlQuery->lastError().driverText() <<
							m_sqlQuery->lastError().databaseText();
						return false;
					}
				}
			}
		}
		m_sqldb.commit();
	}
	return true;
}

void QDBMgr::getTableColumn(const QString &table, QVector<QString> &col)
{
	QString sql = DB_TB_COLUMNS.arg(m_dbname).arg(table);
	try
	{
		if (m_sqlQuery)
		{
			m_sqlQuery->clear();

			m_sqlQuery->prepare(sql);
			m_sqlQuery->exec();
		}
	}
	catch (...)
	{
		qDebug() << QString("DB Error: sql:") << sql;
	}
}

// 打开数据库
bool QDBMgr::openDB()
{
	QSettings configIniRead(qutil::qBaseConfigPath(), QSettings::IniFormat);
	m_dbname = configIniRead.value("/DataBase/dbname").toString();
	m_host = configIniRead.value("/DataBase/host").toString();
	m_port = configIniRead.value("/DataBase/port").toString();
	m_user = configIniRead.value("/DataBase/user").toString();
	m_password = configIniRead.value("/DataBase/password").toString();

	m_sqldb = QSqlDatabase::addDatabase("QMYSQL", m_dbname);
	m_sqldb.setDatabaseName(m_dbname);
	m_sqldb.setHostName(m_host);
	m_sqldb.setPort(m_port.toInt());
	m_sqldb.setUserName(m_user);
	m_sqldb.setPassword(m_password);

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
		else
		{
			qDebug() << m_sqldb.lastError().type() << 
				m_sqldb.lastError().number() << 
				m_sqldb.lastError().driverText() <<
				m_sqldb.lastError().databaseText();
		}
	}
	return false;
}
// 更新数据库
bool QDBMgr::updateDB()
{
	return true;
}

// 执行数据库操作
bool QDBMgr::ExecuteSQL(const QString& sql, const QVariantList& paramList)
{
	QMutexLocker guard(&m_dbLock);
	try
	{
		if (NULL == m_sqlQuery)
		{
			return false;
		}
		m_sqlQuery->clear();

		m_sqlQuery->prepare(sql);
		for (int i = 0; i < paramList.size(); i++)
		{
			m_sqlQuery->addBindValue(paramList[i]);
		}

		return m_sqlQuery->exec();
	}
	catch (...)
	{
		qDebug() << QString("DB Error:") << sql;
		for (int i = 0; i < paramList.size(); i++)
		{
			qDebug() << QString("DB Error: param: ") << paramList[i].toString();
		}
		return false;
	}
}

// 执行批量数据库操作
bool QDBMgr::ExecuteBatchSQL(const QString& sql, const QList<QVariantList>& paramList)
{
	QMutexLocker guard(&m_dbLock);
	try
	{
		if (NULL == m_sqlQuery)
		{
			return false;
		}
		m_sqlQuery->clear();

		m_sqldb.transaction();
		m_sqlQuery->prepare(sql);
		for (int i = 0; i < paramList.size(); i++)
		{
			m_sqlQuery->addBindValue(paramList[i]);
		}
		if (m_sqlQuery->execBatch())
		{
			m_sqldb.commit();
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (...)
	{
		qDebug() << QString("DB Error:") << sql;
		for (int i = 0; i < paramList.size(); i++)
		{
			qDebug() << QString("DB Error: QList") << i;
			QVariantList varlist = paramList[i];
			for (int j = 0; j < varlist.size(); j++)
			{
				qDebug() << QString("DB Error: param: ") << varlist[i];
			}
		}
		return false;
	}
}

// 查询单个字段
bool QDBMgr::QueryOneField(const QString& sql, const QVariantList& paramList,
	QVariantList& results)
{
	QMutexLocker guard(&m_dbLock);
	try
	{
		results.clear();
		if (NULL == m_sqlQuery)
		{
			return false;
		}
		m_sqlQuery->clear();

		m_sqlQuery->prepare(sql);
		for (int i = 0; i < paramList.size(); i++)
		{
			m_sqlQuery->addBindValue(paramList[i]);
		}
		bool res = m_sqlQuery->exec();
		if (res)
		{
			while (m_sqlQuery->next())
			{
				results << m_sqlQuery->value(0);
			}
		}
		return res;
	}
	catch (...)
	{
		qDebug() << QString("DB Error:") << sql;
		for (int i = 0; i < paramList.size(); i++)
		{
			qDebug() << QString("DB Error: param: ") << paramList[i].toString();
		}
	}
	return false;
}

// 查询多个字段
bool QDBMgr::QueryFields(const QString& sql, const QVariantList& paramList,
	const QStringList& fieldList, QList<QVariantList>& results)
{
	QMutexLocker guard(&m_dbLock);
	try
	{
		results.clear();
		if (NULL == m_sqlQuery)
		{
			return false;
		}
		m_sqlQuery->clear();

		m_sqlQuery->prepare(sql);
		for (int i = 0; i < paramList.size(); i++)
		{
			m_sqlQuery->addBindValue(paramList[i]);
		}
		bool res = m_sqlQuery->exec();

		if (res)
		{
			while (m_sqlQuery->next())
			{
				QVariantList resultList;
				for (int i = 0; i < fieldList.size(); i++)
				{
					//int index = rec.indexOf(fieldList[i]);
					//resultList << m_sqlQuery->value(index);
					resultList << m_sqlQuery->value(fieldList[i]);
				}
				results << resultList;
			}
		}
		return res;
	}
	catch (...)
	{
		qDebug() << QString("DB Error:") << sql;
		for (int i = 0; i < paramList.size(); i++)
		{
			qDebug() << QString("DB Error: param: ") << paramList[i].toString();
		}
		for (int i = 0; i < fieldList.size(); i++)
		{
			qDebug() << QString("DB Error: field: ") << fieldList[i];
		}
	}
	return false;
}

// 删除表
void QDBMgr::DropTable(const QString &table)
{
	QMutexLocker guard(&m_dbLock);
	QString sql = DB_DROPTABLE.arg(table);
	try
	{
		if (m_sqlQuery)
		{
			m_sqlQuery->clear();
			m_sqlQuery->prepare(sql);
			m_sqlQuery->exec();
		}
	}
	catch (...)
	{
		qDebug() << QString("DB Error: sql:") << sql;
	}
}

QString QDBMgr::lastError()
{
	if (m_sqlQuery) {
		return m_sqlQuery->lastError().text();
	}
	return "sql query not init";
}