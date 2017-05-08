#include "ParameterSetting.h"
#include "metadatabase.h"
#include <QSet>
#include <QObject>
#include <QDebug>
#include "util/util.h"

CParameterSetting::CParameterSetting()
{
	if (m_mapFinancialCalendar.isEmpty())
	{// 金融日历缓存初始化
		QString strErr;
		if (!METADATABASE->getFinancialCalendar(m_mapFinancialCalendar, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
	}
	if (m_portfolio.isEmpty())
	{// 组合管理缓存初始化
		QString strErr;
		if (!METADATABASE->getPortfolio(m_portfolio, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
		// 过滤非法数据
		QStringList data;
		for (QMap<QString, CPortfolio>::iterator itor = m_portfolio.begin();
			itor != m_portfolio.end(); itor++)
		{
			if (!itor->getParentcode().isEmpty() && !m_portfolio.contains(itor->getParentcode()))
				data << itor.key();
		}
		QStringList children;
		foreach(const QString &c, data)
		{
			getPortfolioAllChildren(c, children);
		}
		data.append(children);
		if (!removePortfoliolst(data, strErr)) qDebug() << strErr;
	}
	if (m_product.isEmpty())
	{// 产品管理缓存初始化
		QString strErr;
		if (!METADATABASE->getProduct(m_product, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
		// 过滤非法数据
		QStringList data;
		for (QMap<QString, CProduct>::iterator itor = m_product.begin();
			itor != m_product.end(); itor++)
		{
			if (!itor->getParentCode().isEmpty() && !m_product.contains(itor->getParentCode()))
				data << itor.key();
		}
		QList<CProduct> children;
		foreach(const QString &c, data)
			getAllChildrenProduct(c, children);
		foreach(const CProduct &c, children)
			data.append(c.getCode());
		if (!removeProducts(data, strErr)) qDebug() << strErr;
	}
	if (m_paradict.isEmpty())
	{// 参数管理缓存初始化
		QString strErr;
		if (!METADATABASE->getParadict(m_paradict, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
		else
			initParaDict();
	}
}

CParameterSetting::~CParameterSetting()
{
}

//////金融日历////
bool CParameterSetting::isExistFinancialCalendar(const CFinancialCalendar &val)
{
	return m_mapFinancialCalendar.contains(val.getDate());
}
bool CParameterSetting::setFinancialCalendar(const CFinancialCalendar &val, QString &err)
{
	if (METADATABASE->setFinancialCalendar(val, err))
	{
		m_mapFinancialCalendar[val.getDate()] = val;
		return true;
	}
	return false;
}
bool CParameterSetting::removeFinancialCalendar(int val, QString &err)
{
	if (METADATABASE->removeFinancialCalendar(val, err))
	{
		m_mapFinancialCalendar.remove(val);
		return true;
	}
	return false;
}
const QMap<int, CFinancialCalendar> &CParameterSetting::getFinancialCalendar() const
{
	return m_mapFinancialCalendar;
}

///////组合管理///////////
bool CParameterSetting::setPortfolio(const CPortfolio &val, QString &err)
{
	if (METADATABASE->setPortfolio(val, err))
	{
		m_portfolio[val.getPortcode()] = val;
		return true;
	}
	return false;
}
bool CParameterSetting::removePortfolio(const QString &val, QString &err)
{
	if (!isExistPortfolioCode(val))
	{
		err = "no code.";
		return false;
	}
	QStringList cldlst;
	getPortfolioChildren(val, cldlst);
	if (!cldlst.isEmpty())
	{
		err = "still exist children!";
		return false;
	}
	QSet<QString> dellst;
	dellst << val;
	return removePortfoliolst(dellst.toList(), err);
}
bool CParameterSetting::removePortfoliolst(const QStringList &val, QString &err)
{
	if (METADATABASE->removePortfolio(val, err))
	{
		for (QMap<QString, CPortfolio>::iterator it = m_portfolio.begin();
			it != m_portfolio.end();)
		{
			if (val.contains(it.key()))
				m_portfolio.erase(it++);
			else
				it++;
		}
		return true;
	}
	return false;
}
const QMap<QString, CPortfolio> &CParameterSetting::getPortfolio()
{
	return m_portfolio;
}
bool CParameterSetting::isExistPortfolioCode(const QString &val)
{
	return m_portfolio.contains(val);
}
void CParameterSetting::getPortfolioChildren(const QString &key, QStringList &val)
{
	if (isExistPortfolioCode(key))
	{
		for (QMap<QString, CPortfolio>::iterator it = m_portfolio.begin();
			it != m_portfolio.end(); it++)
		{
			if (it->getParentcode() == key)
				val << it.key();
		}
	}
}
void CParameterSetting::getPortfolioAllChildren(const QString &key, QStringList &results)
{
	QStringList result;
	getPortfolioChildren(key, result);
	if (!result.isEmpty()) {
		results.append(result);
		foreach(const QString &code, result) {
			getPortfolioChildren(code, results);
		}
	}
}

///////产品管理///////////
const QMap<QString, CProduct>& CParameterSetting::getProduct()
{
	return m_product;
}
bool CParameterSetting::setProduct(const CProduct &val, QString &err)
{
	if (METADATABASE->setProduct(val, err)) {
		m_product[val.getCode()] = val;
		return true;
	}
	return false;
}

bool CParameterSetting::getProduct(const QString &code, CProduct &val)
{
	const QMap<QString, CProduct> &data = this->getProduct();
	if (data.contains(code)) {
		val = data[code];
		return true;
	}
	return false;
}

bool CParameterSetting::removeProduct(const QString &code, QString &err)
{
	if (!m_product.contains(code)) 
	{
		err = "no code.";
		return false;
	}
	if (!getChildrenProduct(code).isEmpty())
	{
		err = "still exist children!";
		return false;
	}

	QStringList deleteList;
	deleteList << code;
	return removeProducts(deleteList, err);
}
bool CParameterSetting::removeProducts(const QStringList &deleteList, QString &err)
{
	if (METADATABASE->removeProduct(deleteList, err)) {
		foreach(const QString &deleteCode, deleteList) {
			m_product.remove(deleteCode);
		}
		return true;
	}
	return false;
}
QList<CProduct> CParameterSetting::getRootProduct()
{
	QList<CProduct> result;
	const QMap<QString, CProduct>& data = this->getProduct();
	for (auto iter = data.constBegin(); iter != data.constEnd(); ++iter) {
		const CProduct &val = iter.value();
		if (val.getParentCode().isEmpty()) {
			result.push_back(val);
		}
	}
	return result;
}
QList<CProduct> CParameterSetting::getChildrenProduct(const QString &parentCode)
{
	QList<CProduct> result;
	const QMap<QString, CProduct>& data = this->getProduct();
	for (auto iter = data.constBegin(); iter != data.constEnd(); ++iter) {
		const CProduct &val = iter.value();
		if (val.getParentCode() == parentCode) {
			result.push_back(val);
		}
	}
	return result;
}

void CParameterSetting::getAllChildrenProduct(const QString &parentCode, QList<CProduct> &results)
{
	QList<CProduct> result = this->getChildrenProduct(parentCode);
	if (!result.isEmpty()) {
		results.append(result);
		foreach(const CProduct &product, result) {
			getAllChildrenProduct(product.getCode(), results);
		}
	}
}

///////参数字典///////////
const QMap<QString, QList<CParaDict>>& CParameterSetting::getParadict() const
{
	return m_paradict;
}

bool CParameterSetting::getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val)
{
	if (!m_paradict.contains(typeCode)) return false;

	const QList<CParaDict> &items = m_paradict[typeCode];
	foreach(const CParaDict &item, items) 
	{
		if (item.getTypeCode() == typeCode && item.getParaCode() == paraCode) 
		{
			val = item;
			return true;
		}
	}
	return false;
}

bool CParameterSetting::getAllParadict(const QString &typeCode, QList<CParaDict> &valList)
{
	if (!m_paradict.contains(typeCode)) return false;
	valList = m_paradict[typeCode];
	return true;
}

bool CParameterSetting::setParadict(const CParaDict &val, QString &err)
{
	// 再更新参数代码
	if (METADATABASE->setParadict(val, err)) {
		m_paradict[val.getTypeCode()].push_back(val);
		return true;
	}
	return false;
}

bool CParameterSetting::removeParadict(const QString &typeCode, const QString &paraCode, QString &err)
{
	if (METADATABASE->removeParadict(QStringList() << typeCode, QStringList() << paraCode, err)) 
	{
		if (m_paradict.contains(typeCode))
		{
			for (int i = m_paradict[typeCode].size() - 1; i >= 0; i--) {
				if (m_paradict[typeCode][i].getTypeCode() == typeCode && m_paradict[typeCode][i].getParaCode() == paraCode) {
					m_paradict[typeCode].removeAt(i);
					break;
				}
			}
		}
		return true;
	}
	return false;
}

// 初始化参数字典
void CParameterSetting::initParaDict()
{
	if (!m_paradict.isEmpty()) return;
	QList<CParaDict> strList;
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "quarterly", QObject::tr("quarterly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "yearly", QObject::tr("yearly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "monthly", QObject::tr("monthly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "halfyear", QObject::tr("halfyear")));
	strList.push_back(CParaDict("Calendar", QObject::tr("Calendar"), "CFETS", QObject::tr("CFETS")));
	strList.push_back(CParaDict("Calendar", QObject::tr("Calendar"), "SHSE", QObject::tr("SHSE")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Modified Following", QObject::tr("Modified Following")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Following", QObject::tr("Following")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Preceding", QObject::tr("Preceding")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/Act", QObject::tr("Act/Act")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/365", QObject::tr("Act/365")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/360", QObject::tr("Act/360")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "ZeroCoupon", QString::fromLocal8Bit("零息票")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "FixedCoupon", QString::fromLocal8Bit("固定息票")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "FloatCoupon", QString::fromLocal8Bit("浮动息票")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "shibor3m", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "repo7d", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "depo1y", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "IBMoneyMarket", QString::fromLocal8Bit("银行间货币市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "IBBondMarket", QString::fromLocal8Bit("银行间债券市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "SHEXMoneyMarket", QString::fromLocal8Bit("上海交易所货币市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "SHEXStockMarket", QString::fromLocal8Bit("上海股票交易所市场")));
	strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("零息利率复利类型"), "ContinuousCompound", QString::fromLocal8Bit("连续复利")));
	strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("零息利率复利类型"), "yearlyCompound", QString::fromLocal8Bit("年复利")));
	
	QString strErr;
	if (METADATABASE->setParadict(strList, strErr)) 
	{
		for (QList<CParaDict>::const_iterator itor = strList.begin();
			itor != strList.end(); itor++)
			m_paradict[itor->getTypeCode()].push_back(*itor);
	}
	else
	{
		qDebug() << strErr;
	}
}