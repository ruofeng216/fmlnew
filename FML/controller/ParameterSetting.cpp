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
const QMap<QString, CParaDict>& CParameterSetting::getParadict() const
{
	return m_paradict;
}

bool CParameterSetting::getParaDict(const QString &paraCode, CParaDict &val) {
	if (m_paradict.contains(paraCode))
	{
		val = m_paradict[paraCode];
		return true;
	}
	return false;
}

bool CParameterSetting::getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val)
{
	QString K = paraCode.isEmpty() ? typeCode : paraCode;
	if (m_paradict.contains(K))
	{
		if (m_paradict[K].getParaCode() == paraCode &&
			m_paradict[K].getTypeCode() == typeCode)
		val = m_paradict[K];
		return true;
	}
	return false;
}

bool CParameterSetting::getAllParadict(const QString &typeCode, QMap<QString, CParaDict> &valList)
{
	for (QMap<QString, CParaDict>::const_iterator itor = m_paradict.begin(); itor != m_paradict.end(); itor++)
	{
		if (itor->getTypeCode() == typeCode && !itor->getParaCode().isEmpty())
			valList[itor->getParaCode()] = itor.value();
	}
	return true;
}

bool CParameterSetting::setParadict(const CParaDict &val, QString &err)
{
	// 再更新参数代码
	if (METADATABASE->setParadict(val, err)) 
	{
		QString k = val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode();
		m_paradict[k] = val;
		return true;
	}
	return false;
}

bool CParameterSetting::removeParadict(const QString &typeCode, const QString &paraCode, QString &err)
{
	if (METADATABASE->removeParadict(QStringList() << typeCode, QStringList() << paraCode, err)) 
	{
		for (QMap<QString, CParaDict>::iterator itor = m_paradict.begin(); itor != m_paradict.end(); itor++)
		{
			if (itor->getTypeCode() == typeCode && itor->getParaCode() == paraCode)
			{
				m_paradict.erase(itor);
				break;
			}
		}
		return true;
	}
	return false;
}