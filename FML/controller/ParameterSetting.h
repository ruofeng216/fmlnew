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
	bool setProduct(const CProduct &val);
	bool removeProduct(const QString &code);
	QList<CProduct> getRootProduct() const;
	QList<CProduct> getChildrenProduct(const QString &parentCode);

private:
	QMap<QString, CPortfolio> m_portfolio;
	QMap<QString, CProduct> m_product;
};

