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
	QDBMgr(QObject *parent = NULL);
	~QDBMgr();

	// 执行数据库操作
	bool ExecuteSQL(const QString& sql, const QVariantList& paramList);

	// 执行批量数据库操作
	bool ExecuteBatchSQL(const QString& sql, const QList<QVariantList>& paramList);

	// 查询单个字段
	void QueryOneField(const QString& sql, const QVariantList& paramList,
		QVariantList& results);

	// 查询多个字段
	void QueryFields(const QString& sql, const QVariantList& paramList,
		const QStringList& fieldList, QList<QVariantList>& results);

	// 删除表
	void DropTable(const QString &table);

protected:
	// 打开数据库
	bool openDB();
	// 更新数据库
	bool updateDB();

private:
	// tables data init.
	void initTables();
	bool hasTable(const QString &table);
	bool updateTable(const QString &table);
	void getTableColumn(const QString &table, QVector<QString> &col);
private:
	QSqlDatabase m_sqldb;
	QSqlQuery* m_sqlQuery;
	QMap<QString, QString> m_tables;
	QMutex m_dbLock;

	QString m_dbname;
	QString m_host;
	QString m_port;
	QString m_user;
	QString m_password;
};
