#include "ParameterSetting.h"
#include "metadatabase.h"
#include <QSet>
#include "util/util.h"

CParameterSetting::CParameterSetting()
{
	if (m_portfolio.isEmpty()) 
		METADATABASE->getPortfolio(m_portfolio);
}


CParameterSetting::~CParameterSetting()
{
}

//////金融日历////
bool CParameterSetting::isExistFinancialCalendar(const CFinancialCalendar &val)
{
	return METADATABASE->isExistFinancialCalendar(val);
}
bool CParameterSetting::setFinancialCalendar(const CFinancialCalendar &val)
{
	return METADATABASE->setFinancialCalendar(val);
}
bool CParameterSetting::removeFinancialCalendar(int val)
{
	return METADATABASE->removeFinancialCalendar(val);
}
bool CParameterSetting::getFinancialCalendar(QMap<int, CFinancialCalendar> &val)
{
	return METADATABASE->getFinancialCalendar(val);
}

bool CParameterSetting::getFinancialCalendar(int bwdate, CFinancialCalendar &val)
{
	return METADATABASE->getFinancialCalendar(bwdate, val);
}

///////组合管理///////////
bool CParameterSetting::setPortfolio(const CPortfolio &val)
{
	if (METADATABASE->setPortfolio(val))
	{
		m_portfolio[val.getPortcode()] = val;
		return true;
	}
	return false;
}
bool CParameterSetting::removePortfolio(const QString &val)
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
	if (METADATABASE->removePortfolio(dellst.toList()))
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
	if (m_portfolio.isEmpty()) METADATABASE->getPortfolio(m_portfolio);
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
	if (m_product.isEmpty()) {
		METADATABASE->getProduct(m_product);

#if 0
		// 删除脏数据，有些产品有parentCode，但其实parentCode对应的产品不存在
		QStringList deleteList;
		for (auto iter = m_product.begin(); iter != m_product.end(); ) {
			const QString &parentCode = iter.value().getParentCode();
			if (!parentCode.isEmpty() && !m_product.contains(parentCode)) {
				qDebug() << "parent code is not existing, invalid product:" << iter.key();
				deleteList << iter.key();
				iter = m_product.erase(iter);
			} else {
				++iter;
			}
		}
		foreach(const QString &code, deleteList) {
			this->removeProduct(code);
		}
#endif
	}
	return m_product;
}
bool CParameterSetting::setProduct(const CProduct &val)
{
	if (METADATABASE->setProduct(val)) {
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

bool CParameterSetting::removeProduct(const QString &code)
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
	if (METADATABASE->removeProduct(deleteList)) {
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
#if 1 // 调试
		if (m_paradict.isEmpty()) {
			QList<CParaDict> strList;
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("付息频率")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("付息频率"), "quarterly", QString::fromLocal8Bit("按季")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("付息频率"), "yearly", QString::fromLocal8Bit("按年")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("付息频率"), "monthly", QString::fromLocal8Bit("按月")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("付息频率"), "halfyear", QString::fromLocal8Bit("按半年")));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("日历")));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("日历"), "CFETS", QString::fromLocal8Bit("银行间市场日历")));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("日历"), "SHSE", QString::fromLocal8Bit("中国上海交易所日历")));
			foreach(const CParaDict &str, strList) {
				setParadict(str);
			}
		}
#endif
	}
	return m_paradict;
}

bool CParameterSetting::getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val)
{
	const QList<CParaDict> &items = this->getParadict();
	foreach(const CParaDict &item, items) {
		if (item.getTypeCode() == typeCode && item.getParaCode() == paraCode) {
			val = item;
			return true;
		}
	}
	return false;
}

bool CParameterSetting::setParadict(const CParaDict &val)
{
	if (METADATABASE->setParadict(val)) {
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

bool CParameterSetting::removeParadict(const QString &typeCode, const QString &paraCode)
{
	if (typeCode.isEmpty()) {
		return false;
	}
	if (!paraCode.isEmpty()) {
		if (METADATABASE->removeParadict(QStringList() << typeCode, QStringList() << paraCode)) {
			for (int i = m_paradict.size() - 1; i >= 0; i--) {
				if (m_paradict[i].getTypeCode() == typeCode && m_paradict[i].getParaCode() == paraCode) {
					m_paradict.removeAt(i);
					break;
				}
			}
			return true;
		}
	} else {
		if (METADATABASE->removeParadict(QStringList() << typeCode)) {
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