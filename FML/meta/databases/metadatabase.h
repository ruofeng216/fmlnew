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
	bool getFinancialCalendar(int bwdate, CFinancialCalendar &val);
	// 组合管理
	bool setPortfolio(const CPortfolio &val);
	bool removePortfolio(const QStringList &val);
	bool getPortfolio(QMap<QString, CPortfolio> &val);
	// 产品管理
	bool getProduct(QMap<QString, CProduct> &val);
	bool setProduct(const CProduct &val);
	bool removeProduct(const QStringList &codeList);
	// 参数字典
	bool getParadict(QList<CParaDict> &val);
	bool setParadict(const CParaDict &val);

private:
	MetaDatabase(QObject *parent = NULL);
	~MetaDatabase();

private:
	QDBMgr *m_DbMgr;

};
