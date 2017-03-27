#include "ParameterSetting.h"
#include "metadatabase.h"
#include <QSet>

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
const QMap<QString, CParaDict>& CParameterSetting::getParadict()
{
	return m_paradict;
}