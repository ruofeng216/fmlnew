#pragma once

#include <QObject>
#include <QSqlDatabase> 
#include <QVariant>
#include <QMap>
#include <QMutex>
#include "db_def.h"

class QSqlQuery;
class QDBMgr : public QObject
{
	Q_OBJECT

public:
	QDBMgr(const QString &dbname, QObject *parent = NULL);
	~QDBMgr();

protected:
	// 打开数据库
	bool openDB();
	// 更新数据库
	bool updateDB();

private:
	// tables data init.
	void initTables();

private:
	QString m_db;
	QSqlDatabase m_sqldb;
	QSqlQuery* m_sqlQuery;
	QMap<QString, QString> m_tables;
	QMutex m_dbLock;
};
