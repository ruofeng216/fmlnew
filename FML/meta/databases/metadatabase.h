#pragma once

#include <QObject>
#include "util/datatype.h"

#define METADATABASE MetaDatabase::instance()

class QDBMgr;
class MetaDatabase : public QObject
{
	Q_OBJECT
public:
	// ��ȡʵ��
	static MetaDatabase *instance();
	// login
	bool getLoginInfo(const QString &username, CLogin &lval, QString &err);
	bool setLoginInfo(const CLogin &uinfo, QString &err);
	bool updateLoginInfo(const CLogin &uinfo, QString &err);
	// ��������
	bool setFinancialCalendar(const CFinancialCalendar &val, QString &err);
	bool removeFinancialCalendar(int val, QString &err);
	bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val, QString &err);

	// ��Ϲ���
	bool setPortfolio(const CPortfolio &val, QString &err);
	bool removePortfolio(const QStringList &val, QString &err);
	bool getPortfolio(QMap<QString, CPortfolio> &val, QString &err);

	// ��Ʒ����
	bool getProduct(QMap<QString, CProduct> &val, QString &err);
	bool setProduct(const CProduct &val, QString &err);
	bool removeProduct(const QStringList &codeList, QString &err);

	// �����ֵ�
	bool getParadict(QMap<QString, QList<CParaDict>> &val, QString &err);
	bool setParadict(const CParaDict &val, QString &err);
	bool setParadict(const QList<CParaDict> &valList, QString &err);
	bool removeParadict(const QStringList &typeCodeList, const QStringList &paraCodeList, QString &err);
	bool removeParadict(const QStringList &typeCodeList, QString &err);

	// �ؼ��㶨��
	bool getKeypoint(QMap<QString, CKeypoint> &val, QString &err);
	bool setKeypoint(const QList<CKeypoint> &valList, QString &err);
	bool removeKeypoint(const QStringList &valList, QString &err);

private:
	MetaDatabase(QObject *parent = NULL);
	~MetaDatabase();

private:
	QDBMgr *m_DbMgr;

};
