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
	bool getLoginInfo(const QString &username, CLogin &lval);
	bool setLoginInfo(const CLogin &uinfo);
	bool updateLoginInfo(const CLogin &uinfo);
	// ��������
	bool isExistFinancialCalendar(const CFinancialCalendar &val);
	bool setFinancialCalendar(const CFinancialCalendar &val);
	bool removeFinancialCalendar(int val);
	bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val);
	bool getFinancialCalendar(int bwdate, CFinancialCalendar &val);
	// ��Ϲ���
	bool setPortfolio(const CPortfolio &val);
	bool removePortfolio(const QStringList &val);
	bool getPortfolio(QMap<QString, CPortfolio> &val);
	// ��Ʒ����
	bool getProduct(QMap<QString, CProduct> &val);
	bool setProduct(const CProduct &val);
	bool removeProduct(const QStringList &codeList);
	// �����ֵ�
	bool getParadict(QList<CParaDict> &val);
	bool setParadict(const CParaDict &val);

private:
	MetaDatabase(QObject *parent = NULL);
	~MetaDatabase();

private:
	QDBMgr *m_DbMgr;

};
