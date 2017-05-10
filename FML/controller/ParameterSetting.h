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
	bool setFinancialCalendar(const CFinancialCalendar &val, QString &err);
	bool removeFinancialCalendar(int val, QString &err);
	const QMap<int, CFinancialCalendar> &getFinancialCalendar() const;

	///////组合管理///////////
	bool setPortfolio(const CPortfolio &val, QString &err);
	bool removePortfolio(const QString &val, QString &err);
	bool removePortfoliolst(const QStringList &val, QString &err);
	const QMap<QString, CPortfolio> &getPortfolio();
	bool isExistPortfolioCode(const QString &val);
	void getPortfolioChildren(const QString &key, QStringList &val);
	void getPortfolioAllChildren(const QString &key, QStringList &val);

	///////产品管理///////////
	const QMap<QString, CProduct>& getProduct();
	bool getProduct(const QString &code, CProduct &val);
	bool setProduct(const CProduct &val, QString &err);
	bool removeProduct(const QString &code, QString &err);
	bool removeProducts(const QStringList &code, QString &err);
	QList<CProduct> getRootProduct();
	// 获取直系子节点
	QList<CProduct> getChildrenProduct(const QString &parentCode);
	// 获取所有子节点
	void getAllChildrenProduct(const QString &parentCode, QList<CProduct> &results);

	///////参数字典///////////
	const QMap<QString, CParaDict>& getParadict()const;
	bool getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val);
	bool getAllParadict(const QString &typeCode, QMap<QString, CParaDict> &valList);
	bool setParadict(const CParaDict &val, QString &err);
	bool removeParadict(const QString &typeCode, const QString &paraCode, QString &err);

private:
	//金融日历
	QMap<int, CFinancialCalendar> m_mapFinancialCalendar;
	//组合管理
	QMap<QString, CPortfolio> m_portfolio;
	//产品信息
	QMap<QString, CProduct> m_product;
	//参数字典 <key: pcode/tcode, info>
	QMap<QString, CParaDict> m_paradict;
};

