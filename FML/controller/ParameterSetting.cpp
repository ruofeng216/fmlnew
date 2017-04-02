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

//////��������////
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

///////��Ϲ���///////////
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

///////��Ʒ����///////////
const QMap<QString, CProduct>& CParameterSetting::getProduct()
{
	if (m_product.isEmpty()) {
		METADATABASE->getProduct(m_product);

#if 0
		// ɾ�������ݣ���Щ��Ʒ��parentCode������ʵparentCode��Ӧ�Ĳ�Ʒ������
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

///////�����ֵ�///////////
const QList<CParaDict>& CParameterSetting::getParadict()
{
	if (m_paradict.isEmpty()) {
		METADATABASE->getParadict(m_paradict);
		if (m_paradict.isEmpty()) {
			QList<CParaDict> strList;
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("��ϢƵ��"), "", ""));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("��ϢƵ��"), "quarterly", QString::fromLocal8Bit("����")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("��ϢƵ��"), "yearly", QString::fromLocal8Bit("����")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("��ϢƵ��"), "monthly", QString::fromLocal8Bit("����")));
			strList.push_back(CParaDict("CouponFrequency", QString::fromLocal8Bit("��ϢƵ��"), "halfyear", QString::fromLocal8Bit("������")));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("����"), "", ""));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("����"), "CFETS", QString::fromLocal8Bit("���м��г�����")));
			strList.push_back(CParaDict("Calendar", QString::fromLocal8Bit("����"), "SHSE", QString::fromLocal8Bit("�й��Ϻ�����������")));
			strList.push_back(CParaDict("Convention", QString::fromLocal8Bit("��Ϣ�յ���"), "", ""));
			strList.push_back(CParaDict("Convention", QString::fromLocal8Bit("��Ϣ�յ���"), "Modified Following", QString::fromLocal8Bit("��������һ������")));
			strList.push_back(CParaDict("Convention", QString::fromLocal8Bit("��Ϣ�յ���"), "Following", QString::fromLocal8Bit("��һ������")));
			strList.push_back(CParaDict("Convention", QString::fromLocal8Bit("��Ϣ�յ���"), "Preceding", QString::fromLocal8Bit("ǰһ������")));
			strList.push_back(CParaDict("DayCount", QString::fromLocal8Bit("��������"), "", ""));
			strList.push_back(CParaDict("DayCount", QString::fromLocal8Bit("��������"), "Act/Act", QString::fromLocal8Bit("ʵ������/ʵ������")));
			strList.push_back(CParaDict("DayCount", QString::fromLocal8Bit("��������"), "Act/365", QString::fromLocal8Bit("ʵ������/365")));
			strList.push_back(CParaDict("DayCount", QString::fromLocal8Bit("��������"), "Act/360", QString::fromLocal8Bit("ʵ������/360")));
			strList.push_back(CParaDict("CouponType", QString::fromLocal8Bit("ϢƱ����"), "", ""));
			strList.push_back(CParaDict("CouponType", QString::fromLocal8Bit("ϢƱ����"), "ZeroCoupon", QString::fromLocal8Bit("��ϢƱ")));
			strList.push_back(CParaDict("CouponType", QString::fromLocal8Bit("ϢƱ����"), "FixedCoupon", QString::fromLocal8Bit("�̶�ϢƱ")));
			strList.push_back(CParaDict("CouponType", QString::fromLocal8Bit("ϢƱ����"), "FloatCoupon", QString::fromLocal8Bit("����ϢƱ")));
			strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("�ο�����"), "", ""));
			strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("�ο�����"), "shibor3m", QString::fromLocal8Bit("")));
			strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("�ο�����"), "repo7d", QString::fromLocal8Bit("")));
			strList.push_back(CParaDict("ReferenceIndex", QString::fromLocal8Bit("�ο�����"), "depo1y", QString::fromLocal8Bit("")));
			strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("�г�����"), "", ""));
			strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("�г�����"), "IBMoneyMarket", QString::fromLocal8Bit("���м�����г�")));
			strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("�г�����"), "IBBondMarket", QString::fromLocal8Bit("���м�ծȯ�г�")));
			strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("�г�����"), "SHEXMoneyMarket", QString::fromLocal8Bit("�Ϻ������������г�")));
			strList.push_back(CParaDict("MarketType", QString::fromLocal8Bit("�г�����"), "SHEXStockMarket", QString::fromLocal8Bit("�Ϻ���Ʊ�������г�")));
			strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("��Ϣ���ʸ�������"), "", ""));
			strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("��Ϣ���ʸ�������"), "ContinuousCompound", QString::fromLocal8Bit("��������")));
			strList.push_back(CParaDict("ZeroRateCompoundFrequency", QString::fromLocal8Bit("��Ϣ���ʸ�������"), "yearlyCompound", QString::fromLocal8Bit("�긴��")));
			if (METADATABASE->setParadict(strList)) {
				m_paradict = strList;
			}
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
		if (item.getTypeCode() == typeCode) {
			valList.push_back(item);
		}
	}
	return valList.size() > 0;
}

bool CParameterSetting::setParadict(const CParaDict &val)
{
	if (val.getTypeCode().isEmpty()) {
		return false;
	}

	// �ȸ������ʹ���
	if (!setTypeParadict(val)) {
		return false;
	}

	// �޸������ͺ����
	CParaDict oldVal;
	if (getParadict(val.getTypeCode(), val.getParaCode(), oldVal) && oldVal == val) {
		return true;
	}

	// �ٸ��²�������
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

bool CParameterSetting::setTypeParadict(const CParaDict &val)
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
		// ���ڲ������
		if (typeData == oldTypeData) {
			return true;
		} else {
			isModify = true;
		}
	} else {
		// ������
		typeData = getNewTypeData(val, oldTypeData);
		if (METADATABASE->setParadict(typeData)) {
			m_paradict.push_back(typeData);
			return true;
		}
	}

	if (isModify) {
		// ���ڲ����
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
		if (METADATABASE->setParadict(modifyList)) {
			m_paradict = tempList;
			return true;
		}
	}
	return false;
}