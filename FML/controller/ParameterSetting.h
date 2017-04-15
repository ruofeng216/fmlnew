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
	const QMap<int, CFinancialCalendar> &getFinancialCalendar() const;

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
	const QList<CParaDict>& getParadict();
	bool getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val);
	bool getAllParadict(const QString &typeCode, QList<CParaDict> &valList);
	bool setParadict(const CParaDict &val);
	bool removeParadict(const QString &typeCode, const QString &paraCode);

private:
	// 更新参数字典类型代码
	bool setTypeParadict(const CParaDict &val);

private:
	//金融日历
	QMap<int, CFinancialCalendar> m_mapFinancialCalendar;

	QMap<QString, CPortfolio> m_portfolio;
	QMap<QString, CProduct> m_product;
	QList<CParaDict> m_paradict;
};

