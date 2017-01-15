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

	// ִ�����ݿ����
	bool ExecuteSQL(const QString& sql, const QVariantList& paramList);

	// ִ���������ݿ����
	bool ExecuteBatchSQL(const QString& sql, const QList<QVariantList>& paramList);

	// ��ѯ�����ֶ�
	void QueryOneField(const QString& sql, const QVariantList& paramList,
		QVariantList& results);

	// ��ѯ����ֶ�
	void QueryFields(const QString& sql, const QVariantList& paramList,
		const QStringList& fieldList, QList<QVariantList>& results);

	// ɾ����
	void DropTable(const QString &table);

protected:
	// �����ݿ�
	bool openDB();
	// �������ݿ�
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
