#include "metadatabase.h"
#include "qdbmgr.h"
#include "util/pro.h"

MetaDatabase::MetaDatabase(QObject *parent)
	: QObject(parent)
{
	m_DbMgr = new QDBMgr(this);
}

MetaDatabase::~MetaDatabase()
{
}

// ��ȡʵ��
MetaDatabase *MetaDatabase::instance()
{
	static MetaDatabase s;
	return &s;
}


bool MetaDatabase::getLoginInfo(const QString &username, CLogin &lval, QString &err)
{
	FUNCLOG("MetaDatabase::getLoginInfo(const QString &username, CLogin &lval)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << username;
	QStringList fieldList;
	fieldList << "user" << "pswd";
	QList<QVariantList> results;
	bool res = m_DbMgr->QueryFields(DB_SQL_SelectLoginUser, paramList, fieldList, results);
	if (!res)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 2)
		{
			lval.setUname(results[i][0].toString());
			lval.setPassword(results[i][1].toString());
			return true;
		}
	}
	err = "no user.";
	return false;
}
bool MetaDatabase::setLoginInfo(const CLogin &uinfo, QString &err)
{
	FUNCLOG("MetaDatabase::setLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getUname() << uinfo.getPassword();
	if (m_DbMgr->ExecuteSQL(DB_SQL_InsertLoginUser, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::updateLoginInfo(const CLogin &uinfo, QString &err)
{
	FUNCLOG("MetaDatabase::updateLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getPassword() << uinfo.getUname();
	if (m_DbMgr->ExecuteSQL(DB_SQL_UpdateLoginUser, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}

// ��������
bool MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val, QString &err)
{
	FUNCLOG("MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getYear() << val.getDate() << val.getHolidayType() << val.getHolidayinfo();
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplaceFinancialHoliday, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::removeFinancialCalendar(int val, QString &err)
{
	FUNCLOG("MetaDatabase::removeFinancialCalendar(int val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val;
	if (m_DbMgr->ExecuteSQL(DB_SQL_DeleteFinancialHoliday, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::getFinancialCalendar(QMap<int, CFinancialCalendar> &val, QString &err)
{
	FUNCLOG("MetaDatabase::getFinancialCalendar(const QMap<int, CFinancialCalendar> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "bwyear" << "bwdate" << "daytype" << "annotation";
	QList<QVariantList> results;
	bool ret = m_DbMgr->QueryFields(DB_SQL_SelectFinancialHolidays, paramList, fieldList, results);
	if (!ret)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 4)
		{
			CFinancialCalendar fc;
			fc.setYear(results[i][0].toInt());
			fc.setDate(results[i][1].toInt());
			fc.setHolidayType(results[i][2].toInt());
			fc.setHolidayinfo(results[i][3].toString());
			val[fc.getDate()] = fc;
		}
	}
	return true;
}

// ��Ϲ���
bool MetaDatabase::setPortfolio(const CPortfolio &val, QString &err)
{
	FUNCLOG("MetaDatabase::setPortfolio(const CPortfolio &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getPortcode() 
		<< val.getPortname() 
		<< val.getParentcode() 
		<< val.getParentname()
		<< val.getSdate()
		<< val.getEdate()
		<< val.getAnnotation();
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplacePortfolio, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::removePortfolio(const QStringList &val, QString &err)
{
	FUNCLOG("MetaDatabase::removePortfolio(const QVector<CPortfolio> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> lsts;
	QVariantList paramList;
	for (QStringList::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		paramList << *itor;
	}
	lsts << paramList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeletePortfolio, lsts))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::getPortfolio(QMap<QString, CPortfolio> &val, QString &err)
{
	FUNCLOG("MetaDatabase::getPortfolio(QMap<QString, CPortfolio> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "portcode" << "portname" << "parentcode" << "parentname" << "sdate" << "edate" << "annotation";
	QList<QVariantList> results;
	bool ret = m_DbMgr->QueryFields(DB_SQL_SelectPortfolios, paramList, fieldList, results);
	if (!ret)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 7)
		{
			CPortfolio cp;
			cp.setPortcode(results[i][0].toString());
			if (cp.getPortcode().isEmpty()) {
				continue;
			}
			cp.setPortname(results[i][1].toString());
			cp.setParentcode(results[i][2].toString());
			cp.setParentname(results[i][3].toString());
			cp.setSdate(results[i][4].toInt());
			cp.setEdate(results[i][5].toInt());
			cp.setAnnotation(results[i][6].toString());
			val[cp.getPortcode()] = cp;
		}
	}
	return true;
}

//////////////////////////////////��Ʒ����////////////////////////////////////////
bool MetaDatabase::getProduct(QMap<QString, CProduct> &val, QString &err)
{
	FUNCLOG("MetaDatabase::getProduct(QMap<QString, CProduct> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "productcode" << "productname" << "parentcode" << "parentname" << "sdate" << "edate" << "annotation";
	QList<QVariantList> results;
	bool ret = m_DbMgr->QueryFields(DB_SQL_SelectProduct, paramList, fieldList, results);
	if (!ret)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	for (int i = 0; i < results.size(); i++) {
		const QVariantList &valList = results[i];
		if (valList.size() != 7) {
			continue;
		}
		CProduct product;
		product.setCode(valList.at(0).toString());
		if (product.getCode().isEmpty()) {
			continue;
		}
		product.setName(valList.at(1).toString());
		product.setParentCode(valList.at(2).toString());
		product.setParentName(valList.at(3).toString());
		product.setSdate(valList.at(4).toInt());
		product.setEdate(valList.at(5).toInt());
		product.setAnnotation(valList.at(6).toString());
		val[product.getCode()] = product;
	}
	return true;
}
bool MetaDatabase::setProduct(const CProduct &val, QString &err)
{
	FUNCLOG("MetaDatabase::setProduct(const CProduct &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getCode()
		<< val.getName()
		<< val.getParentCode()
		<< val.getParentName()
		<< val.getSdate()
		<< val.getEdate()
		<< val.getAnnotation();
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplaceProduct, paramList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::removeProduct(const QStringList &codeList, QString &err)
{
	FUNCLOG("MetaDatabase::removeProduct(const QStringList &codeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> allParamList;
	QVariantList paramList;
	foreach(const QString &code, codeList) {
		paramList << code;
	}
	allParamList << paramList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteProduct, allParamList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}

///////////////////////////////////�����ֵ�///////////////////////////////////////
// ��ʼ�������ֵ�
bool MetaDatabase::initParaDict(QMap<QString, CParaDict> &val, QString &err)
{
	QList<CParaDict> strList;
	strList.push_back(CParaDict("","",COUPONFREQUENCY, tr("CouponFrequency")));
	strList.push_back(CParaDict(COUPONFREQUENCY, tr("CouponFrequency"), QUARTERLY, tr("quarterly")));
	strList.push_back(CParaDict(COUPONFREQUENCY, tr("CouponFrequency"), YEARLY, tr("yearly")));
	strList.push_back(CParaDict(COUPONFREQUENCY, tr("CouponFrequency"), MONTHLY, tr("monthly")));
	strList.push_back(CParaDict(COUPONFREQUENCY, tr("CouponFrequency"), HALFYEAR, tr("halfyear")));
	strList.push_back(CParaDict("","",CALENDAR, tr("Calendar")));
	strList.push_back(CParaDict(CALENDAR, tr("Calendar"), CFETS, tr("CFETS")));
	strList.push_back(CParaDict(CALENDAR, tr("Calendar"), SHSE, tr("SHSE")));
	strList.push_back(CParaDict("","",CONVENTION, tr("Convention")));
	strList.push_back(CParaDict(CONVENTION, tr("Convention"), MODIFIEDFOLLOWING, tr("Modified Following")));
	strList.push_back(CParaDict(CONVENTION, tr("Convention"), FOLLOWING, tr("Following")));
	strList.push_back(CParaDict(CONVENTION, tr("Convention"), PRECEDING, tr("Preceding")));
	strList.push_back(CParaDict("","",DAYCOUNT, tr("DayCount")));
	strList.push_back(CParaDict(DAYCOUNT, tr("DayCount"), ACT_ACT, tr("Act/Act")));
	strList.push_back(CParaDict(DAYCOUNT, tr("DayCount"), ACT_365, tr("Act/365")));
	strList.push_back(CParaDict(DAYCOUNT, tr("DayCount"), ACT_360, tr("Act/360")));
	strList.push_back(CParaDict("","",COUPONTYPE, tr("CouponType")));
	strList.push_back(CParaDict(COUPONTYPE, tr("CouponType"), ZEROCOUPON, tr("ZeroCoupon")));//��ϢƱ
	strList.push_back(CParaDict(COUPONTYPE, tr("CouponType"), FIXEDCOUPON, tr("FixedCoupon")));//�̶�ϢƱ
	strList.push_back(CParaDict(COUPONTYPE, tr("CouponType"), FLOATCOUPON, tr("FloatCoupon")));//����ϢƱ
	strList.push_back(CParaDict("","",REFERENCEINDEX, tr("ReferenceIndex")/*�ο�����*/));
	strList.push_back(CParaDict(REFERENCEINDEX, tr("ReferenceIndex")/*�ο�����*/, SHIBOR3M, tr("")));
	strList.push_back(CParaDict(REFERENCEINDEX, tr("ReferenceIndex")/*�ο�����*/, REPO7D, tr("")));
	strList.push_back(CParaDict(REFERENCEINDEX, tr("ReferenceIndex")/*�ο�����*/, DEPO1Y, tr("")));
	strList.push_back(CParaDict("","",MARKETTYPE, tr("MarketType")/*�г�����*/));
	strList.push_back(CParaDict(MARKETTYPE, tr("MarketType")/*�г�����*/, IBMONEYMARKET, tr("IBMoneyMarket")));//���м�����г�
	strList.push_back(CParaDict(MARKETTYPE, tr("MarketType")/*�г�����*/, IBBONDMARKET, tr("IBBondMarket")));//���м�ծȯ�г�
	strList.push_back(CParaDict(MARKETTYPE, tr("MarketType")/*�г�����*/, SHEXMONEYMARKET, tr("SHEXMoneyMarket")));//�Ϻ������������г�
	strList.push_back(CParaDict(MARKETTYPE, tr("MarketType")/*�г�����*/, SHEXSTOCKMARKET, tr("SHEXStockMarket")));//�Ϻ���Ʊ�������г�
	strList.push_back(CParaDict("","",ZERORATECOMPOUNDFREQUENCY, tr("ZeroRateCompoundFrequency")/*��Ϣ���ʸ�������*/));
	strList.push_back(CParaDict(ZERORATECOMPOUNDFREQUENCY, tr("ZeroRateCompoundFrequency")/*��Ϣ���ʸ�������*/, CONTINUOUSCOMPOUND, tr("ContinuousCompound")));//��������
	strList.push_back(CParaDict(ZERORATECOMPOUNDFREQUENCY, tr("ZeroRateCompoundFrequency")/*��Ϣ���ʸ�������*/, YEARLYCOMPOUND, tr("yearlyCompound")));//�긴��

	if (METADATABASE->setParadict(strList, err))
	{
		for (QList<CParaDict>::const_iterator itor = strList.begin();
			itor != strList.end(); itor++)
		{
			//QString k = itor->getParaCode().isEmpty() ? itor->getTypeCode() : itor->getParaCode();
			QString k = itor->getParaCode();
			val[k] = *itor;
		}
		return true;
	}
	return false;
}
bool MetaDatabase::getParadict(QMap<QString, CParaDict> &val, QString &err)
{
	FUNCLOG("MetaDatabase::getParadict(QList<CParaDict> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "typecode" << "typename" << "paracode" << "paraname" << "paraexplain";
	QList<QVariantList> results;
	bool ret = m_DbMgr->QueryFields(DB_SQL_SelectParadict, paramList, fieldList, results);
	if (!ret)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	for (int i = 0; i < results.size(); i++) {
		const QVariantList &valList = results[i];
		if (valList.size() != 5) {
			continue;
		}
		CParaDict paraDict;
		paraDict.setTypeCode(valList.at(0).toString());
		paraDict.setTypeName(valList.at(1).toString());
		paraDict.setParaCode(valList.at(2).toString());
		paraDict.setParaName(valList.at(3).toString());
		paraDict.setParaExplain(valList.at(4).toString());
		if (paraDict.getTypeCode().isEmpty() && paraDict.getParaCode().isEmpty()) {
			continue;
		}
		//QString k = paraDict.getParaCode().isEmpty() ? paraDict.getTypeCode() : paraDict.getParaCode();
		QString k = paraDict.getParaCode();
		val[k] = paraDict;
	}
	if (val.isEmpty())
		return initParaDict(val, err);
	return true;
}

bool MetaDatabase::setParadict(const CParaDict &val, QString &err)
{
	FUNCLOG("MetaDatabase::setParadict(const CParaDict &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<CParaDict> valList;
	valList.push_back(val);
	return setParadict(valList, err);
}

bool MetaDatabase::setParadict(const QList<CParaDict> &valList, QString &err)
{
	FUNCLOG("MetaDatabase::setParadict(const QList<CParaDict> &valList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> allParamList;
	QVariantList typeCodeList, typeNameList, paraCodeList, paraNameList, paraExplainList;
	foreach(const CParaDict &val, valList) {
		typeCodeList << val.getTypeCode();
		typeNameList << val.getTypeName();
		paraCodeList << val.getParaCode();
		paraNameList << val.getParaName();
		paraExplainList << val.getParaExplain();
	}
	allParamList << typeCodeList << typeNameList << paraCodeList << paraNameList << paraExplainList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_ReplaceParadict, allParamList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}

bool MetaDatabase::removeParadict(const QStringList &typeCodeList, const QStringList &paraCodeList, QString &err)
{
	FUNCLOG("MetaDatabase::removeParadict(const QStringList &typeCodeList, const QStringList &paraCodeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (typeCodeList.isEmpty() || typeCodeList.size() != paraCodeList.size()) 
	{
		err = QString(tr("parameters error."));
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList typeList, paraList;
	for (int i = 0; i < typeCodeList.size(); i++) {
		typeList << typeCodeList[i];
		paraList << paraCodeList[i];
	}
	allParamList << typeList << paraList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteParadictByTypeParaCode, allParamList)) 
		return true;
	err = m_DbMgr->lastError();
	return false;
}

bool MetaDatabase::removeParadict(const QStringList &typeCodeList, QString &err)
{
	FUNCLOG("MetaDatabase::removeParadict(const QStringList &typeCodeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (typeCodeList.isEmpty()) 
	{
		err = QString("parameters empty.");
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList typeList;
	for (int i = 0; i < typeCodeList.size(); i++) {
		typeList << typeCodeList[i];
	}
	allParamList << typeList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteParadictByTypeCode, allParamList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}

/////////////////////////////////////�ؼ��㶨��/////////////////////////////////////
bool MetaDatabase::getKeypoint(QMap<QString, CKeypoint> &val, QString &err)
{
	FUNCLOG("MetaDatabase::getKeypoint(QMap<QString, CKeypoint> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "kpcode" << "kpname" << "productcode" << "productname" << "tenor" << "marketcode" << "marketname" << "calendar" << "convention" << "daycount" << "spotlag" << "sdate" << "edate";
	QList<QVariantList> results;
	bool ret = m_DbMgr->QueryFields(DB_SQL_SelectKeypoint, paramList, fieldList, results);
	if (!ret)
	{
		err = m_DbMgr->lastError();
		return false;
	}
	
	for (int i = 0; i < results.size(); i++) {
		const QVariantList &valList = results[i];
		if (valList.size() != 13) {
			continue;
		}
		CKeypoint keypoint;
		keypoint.setKpcode(valList.at(0).toString());
		keypoint.setKpname(valList.at(1).toString());
		keypoint.setProductCode(valList.at(2).toString());
		keypoint.setProductName(valList.at(3).toString());
		keypoint.setTenor(valList.at(4).toString());
		keypoint.setMarketCode(valList.at(5).toString());
		keypoint.setMarketName(valList.at(6).toString());
		keypoint.setCalendarCode(valList.at(7).toString());
		keypoint.setCalendarName(valList.at(8).toString());
		keypoint.setConventionCode(valList.at(9).toString());
		keypoint.setConventionName(valList.at(10).toString());
		keypoint.setDayCountCode(valList.at(11).toString());
		keypoint.setDayCountName(valList.at(12).toString());
		keypoint.setSpotlatCode(valList.at(13).toString());
		keypoint.setSpotlatName(valList.at(14).toString());
		keypoint.setSdate(valList.at(15).toInt());
		keypoint.setEdate(valList.at(16).toInt());
		if (keypoint.getKpcode().isEmpty() || keypoint.getTenor().isEmpty()) {
			continue;
		}
		val[keypoint.getKpcode()] = keypoint;
	}
	return true;

}
bool MetaDatabase::setKeypoint(const QList<CKeypoint> &valList, QString &err)
{
	FUNCLOG("MetaDatabase::setKeypoint(const QList<CKeypoint> &valList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> allParamList;
	QVariantList kpcodeList, kpnameList, productcodeList, productnameList, tenorList, marketcodeList, 
		marketnameList, calendarCodeList, calendarNameList, conventionCodeList, conventionNameList, 
		daycountCodeList, daycountNameList, spotlagCodeList, spotlagNameList, sdateList, edateList;
	foreach(const CKeypoint &val, valList) {
		kpcodeList << val.getKpcode();
		kpnameList << val.getKpname();
		productcodeList << val.getProductCode();
		productnameList << val.getProductName();
		tenorList << val.getTenor();
		marketcodeList << val.getMarketCode();
		marketnameList << val.getMarketName();
		calendarCodeList << val.getCalendarCode();
		calendarNameList << val.getCalendarName();
		conventionCodeList << val.getConventionCode();
		conventionNameList << val.getConventionName();
		daycountCodeList << val.getDayCountCode();
		daycountNameList << val.getDayCountName();
		spotlagCodeList << val.getSpotlatCode();
		spotlagNameList << val.getSpotlatName();
		sdateList << val.getSdate();
		edateList << val.getEdate();
	}
	allParamList << kpcodeList << kpnameList << productcodeList << productnameList << tenorList << marketcodeList
		<< marketnameList << calendarCodeList << calendarNameList << conventionCodeList << conventionNameList 
		<< daycountCodeList << daycountNameList << spotlagCodeList << spotlagNameList 
		<< sdateList << edateList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_ReplaceKeypoint, allParamList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}
bool MetaDatabase::removeKeypoint(const QStringList &valList, QString &err)
{
	FUNCLOG("MetaDatabase::removeKeypoint(const QStringList &valList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (valList.isEmpty()) 
	{
		err = QString("parameters empty.");
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList kpcodeList;
	for (int i = 0; i < valList.size(); i++) {
		kpcodeList << valList[i];
	}
	allParamList << kpcodeList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteKeypoint, allParamList))
		return true;
	err = m_DbMgr->lastError();
	return false;
}