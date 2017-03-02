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