#pragma once
#include "inc\controller_interface.h"

class CParameterSetting :
	public IParameterSetting
{
public:
	CParameterSetting();
	~CParameterSetting();
	//////金融日历////
	bool isExistFinancialCalendar(const CFinancialCalendar &val);
	bool setFinancialCalendar(const CFinancialCalendar &val);
	bool removeFinancialCalendar(int val);
	bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val);
	bool getFinancialCalendar(int bwdate, CFinancialCalendar &val);

	///////组合管理///////////
	bool setPortfolio(const CPortfolio &val);
	bool removePortfolio(const QString &val);
	const QMap<QString, CPortfolio> &getPortfolio();
	bool isParentCode(const QString &parent, const QString &child);
	bool isExistCode(const QString &val);
	void getAllRootCodes(QStringList &val);
	void getChildren(const QString &key, QStringList &val);
	bool getPortfolio(const QString &key, CPortfolio &val);

	///////产品管理///////////
	const QMap<QString, CProduct>& getProduct();
	bool getProduct(const QString &code, CProduct &val);
	bool setProduct(const CProduct &val);
	bool removeProduct(const QString &code);
	QList<CProduct> getRootProduct();
	// 获取直系子节点
	QList<CProduct> getChildrenProduct(const QString &parentCode);
	// 获取所有子节点
	void getAllChildrenProduct(const QString &parentCode, QList<CProduct> &results);

	///////参数字典///////////
	const QMap<QString, CParaDict>& getParadict();

private:
	QMap<QString, CPortfolio> m_portfolio;
	QMap<QString, CProduct> m_product;
	QMap<QString, CParaDict> m_paradict;
};

