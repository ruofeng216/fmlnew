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
	}
	if (m_product.isEmpty())
	{// 产品管理缓存初始化
		QString strErr;
		if (!METADATABASE->getProduct(m_product, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
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
	if (!isExistCode(val)) return false;
	QSet<QString> dellst;
	dellst << val;
	for (QSet<QString>::iterator itor = dellst.begin(); itor != dellst.end();)
	{
		for (QMap<QString, CPortfolio>::const_iterator it = m_portfolio.begin();
			it != m_portfolio.end(); it++)
		{
			if (isParentCode(*itor, it.key()))
				dellst << it.key();
		}
		itor++;
	}
	if (METADATABASE->removePortfolio(dellst.toList(), err))
	{
		for (QMap<QString, CPortfolio>::iterator it = m_portfolio.begin();
			it != m_portfolio.end();)
		{
			if (dellst.contains(it.key()))
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
bool CParameterSetting::isParentCode(const QString &parent, const QString &child)
{
	if (!isExistCode(parent) || !isExistCode(child)) return false;
	return parent == m_portfolio[child].getParentcode() || parent==child;
}
bool CParameterSetting::isExistCode(const QString &val)
{
	return m_portfolio.contains(val);
}
void CParameterSetting::getAllRootCodes(QStringList &val)
{
	for (QMap<QString, CPortfolio>::iterator it = m_portfolio.begin();
		it != m_portfolio.end(); it++)
	{
		if (it->getParentcode().isEmpty())
			val << it.key();
	}
}
void CParameterSetting::getChildren(const QString &key, QStringList &val)
{
	if (isExistCode(key))
	{
		for (QMap<QString, CPortfolio>::iterator it = m_portfolio.begin();
			it != m_portfolio.end(); it++)
		{
			if (it->getParentcode() == key)
				val << it.key();
		}
	}
}

bool CParameterSetting::getPortfolio(const QString &key, CPortfolio &val)
{
	if (m_portfolio.contains(key)) {
		val = m_portfolio[key];
		return true;
	}
	return false;
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
	if (!m_product.contains(code)) {
		return false;
	}
	QList<CProduct> children;
	getAllChildrenProduct(code, children);
	QStringList deleteList;
	deleteList << code;
	foreach(const CProduct &child, children) {
		deleteList << child.getCode();
	}
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
const QList<CParaDict>& CParameterSetting::getParadict()
{
	if (m_paradict.isEmpty()) {
		METADATABASE->getParadict(m_paradict);
		if (m_paradict.isEmpty()) 
		{
			initParaDict();
		}
	}
	return m_paradict;
}

bool CParameterSetting::getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val)
{
	if (typeCode.isEmpty()) {
		return false;
	}
	const QList<CParaDict> &items = this->getParadict();
	foreach(const CParaDict &item, items) {
		if (item.getTypeCode() == typeCode && item.getParaCode() == paraCode) {
			val = item;
			return true;
		}
	}
	return false;
}

bool CParameterSetting::getAllParadict(const QString &typeCode, QList<CParaDict> &valList)
{
	if (typeCode.isEmpty()) {
		return false;
	}
	const QList<CParaDict> &items = this->getParadict();
	foreach(const CParaDict &item, items) {
		if (item.getTypeCode().toLower() == typeCode.toLower() && !item.isTypeData()) {
			valList.push_back(item);
		}
	}
	return valList.size() > 0;
}

bool CParameterSetting::setParadict(const CParaDict &val, QString &err)
{
	if (val.getTypeCode().isEmpty()) {
		return false;
	}

	// 先更新类型代码
	if (!setTypeParadict(val,err)) {
		return false;
	}

	// 修改完类型后相等
	CParaDict oldVal;
	if (getParadict(val.getTypeCode(), val.getParaCode(), oldVal) && oldVal == val) {
		return true;
	}

	// 再更新参数代码
	if (METADATABASE->setParadict(val, err)) {
		bool isUpdate = false;
		for (int i = 0; i < m_paradict.size(); i++) {
			if (m_paradict[i].getTypeCode() == val.getTypeCode() && m_paradict[i].getParaCode() == val.getParaCode()) {
				isUpdate = true;
				m_paradict[i] = val;
				break;
			}
		}
		if (!isUpdate) {
			m_paradict.push_back(val);
		}
		return true;
	}
	return false;
}

bool CParameterSetting::removeParadict(const QString &typeCode, const QString &paraCode, QString &err)
{
	if (typeCode.isEmpty()) {
		return false;
	}
	if (!paraCode.isEmpty()) {
		if (METADATABASE->removeParadict(QStringList() << typeCode, QStringList() << paraCode, err)) {
			for (int i = m_paradict.size() - 1; i >= 0; i--) {
				if (m_paradict[i].getTypeCode() == typeCode && m_paradict[i].getParaCode() == paraCode) {
					m_paradict.removeAt(i);
					break;
				}
			}
			return true;
		}
	} else {
		if (METADATABASE->removeParadict(QStringList() << typeCode, err)) {
			for (int i = m_paradict.size() - 1; i >= 0; i--) {
				if (m_paradict[i].getTypeCode() == typeCode) {
					m_paradict.removeAt(i);
				}
			}
			return true;
		}
	}
	return false;
}

bool CParameterSetting::setTypeParadict(const CParaDict &val, QString &err)
{
	if (val.getTypeCode().isEmpty()) {
		return false;
	}

	auto getNewTypeData = [](const CParaDict &val, const CParaDict &oldTypeVal) -> CParaDict {
		if (val.isTypeData()) {
			return val;
		} else {
			CParaDict result;
			result.setTypeCode(val.getTypeCode());
			result.setTypeName(val.getTypeName());
			result.setParaExplain(oldTypeVal.getParaExplain());
			return result;
		}
	};

	bool isModify = false;
	CParaDict oldTypeData;
	CParaDict typeData;
	if (this->getParadict(val.getTypeCode(), "", oldTypeData)) {
		typeData = getNewTypeData(val, oldTypeData);
		// 存在并且相等
		if (typeData == oldTypeData) {
			return true;
		} else {
			isModify = true;
		}
	} else {
		// 不存在
		typeData = getNewTypeData(val, oldTypeData);
		if (METADATABASE->setParadict(typeData, err)) {
			m_paradict.push_back(typeData);
			return true;
		}
	}

	if (isModify) {
		// 存在不相等
		QList<CParaDict> modifyList, tempList;
		tempList = m_paradict;
		for (int i = 0; i < tempList.size(); i++) {
			if (tempList[i].getTypeCode() == typeData.getTypeCode()) {
				if (!tempList[i].isTypeDataEqual(typeData)) {
					tempList[i].setTypeData(typeData);
					modifyList.push_back(tempList[i]);
				}
			}
		}
		if (METADATABASE->setParadict(modifyList, err)) {
			m_paradict = tempList;
			return true;
		}
	}
	return false;
}

// 初始化参数字典
void CParameterSetting::initParaDict()
{
	if (!m_paradict.isEmpty()) return;
	QList<CParaDict> strList;
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "", ""));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "quarterly", QObject::tr("quarterly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "yearly", QObject::tr("yearly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "monthly", QObject::tr("monthly")));
	strList.push_back(CParaDict("CouponFrequency", QObject::tr("CouponFrequency"), "halfyear", QObject::tr("halfyear")));
	strList.push_back(CParaDict("Calendar", QObject::tr("Calendar"), "", ""));
	strList.push_back(CParaDict("Calendar", QObject::tr("Calendar"), "CFETS", QObject::tr("CFETS")));
	strList.push_back(CParaDict("Calendar", QObject::tr("Calendar"), "SHSE", QObject::tr("SHSE")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "", ""));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Modified Following", QObject::tr("Modified Following")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Following", QObject::tr("Following")));
	strList.push_back(CParaDict("Convention", QObject::tr("Convention"), "Preceding", QObject::tr("Preceding")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "", ""));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/Act", QObject::tr("Act/Act")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/365", QObject::tr("Act/365")));
	strList.push_back(CParaDict("DayCount", QObject::tr("DayCount"), "Act/360", QObject::tr("Act/360")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "", ""));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "ZeroCoupon", QString::fromLocal8Bit("零息票")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "FixedCoupon", QString::fromLocal8Bit("固定息票")));
	strList.push_back(CParaDict("CouponType", QObject::tr("CouponType"), "FloatCoupon", QString::fromLocal8Bit("浮动息票")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "", ""));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "shibor3m", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "repo7d", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("参考利率"), "depo1y", QString::fromLocal8Bit("")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "", ""));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "IBMoneyMarket", QString::fromLocal8Bit("银行间货币市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "IBBondMarket", QString::fromLocal8Bit("银行间债券市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "SHEXMoneyMarket", QString::fromLocal8Bit("上海交易所货币市场")));
	strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("市场类型"), "SHEXStockMarket", QString::fromLocal8Bit("上海股票交易所市场")));
	strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("零息利率复利类型"), "", ""));
	strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("零息利率复利类型"), "ContinuousCompound", QString::fromLocal8Bit("连续复利")));
	strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("零息利率复利类型"), "yearlyCompound", QString::fromLocal8Bit("年复利")));
	
	QString strErr;
	if (METADATABASE->setParadict(strList, strErr)) 
	{
		m_paradict = strList;
	}
	else
	{
		qDebug() << strErr;
	}
}