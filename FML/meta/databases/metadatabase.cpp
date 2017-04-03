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

// 获取实例
MetaDatabase *MetaDatabase::instance()
{
	static MetaDatabase s;
	return &s;
}


bool MetaDatabase::getLoginInfo(const QString &username, CLogin &lval)
{
	FUNCLOG("MetaDatabase::getLoginInfo(const QString &username, CLogin &lval)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << username;
	QStringList fieldList;
	fieldList << "user" << "pswd";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectLoginUser, paramList, fieldList, results);
	if (results.isEmpty()) return false;
	for (int i = 0; i < results.size(); i++)
	{
		if (results[i].size() == 2)
		{
			lval.setUname(results[i][0].toString());
			lval.setPassword(results[i][1].toString());
			return true;
		}
	}
	return false;
}
bool MetaDatabase::setLoginInfo(const CLogin &uinfo)
{
	FUNCLOG("MetaDatabase::setLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getUname() << uinfo.getPassword();
	if (m_DbMgr->ExecuteSQL(DB_SQL_InsertLoginUser, paramList))
		return true;
	return false;
}
bool MetaDatabase::updateLoginInfo(const CLogin &uinfo)
{
	FUNCLOG("MetaDatabase::updateLoginInfo(const CLogin &uinfo)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << uinfo.getPassword() << uinfo.getUname();
	if (m_DbMgr->ExecuteSQL(DB_SQL_UpdateLoginUser, paramList))
		return true;
	return false;
}

// 金融日历
bool MetaDatabase::isExistFinancialCalendar(const CFinancialCalendar &val)
{
	CFinancialCalendar result;
	return getFinancialCalendar(val.getDate(), result);
}
bool MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val)
{
	FUNCLOG("MetaDatabase::setFinancialCalendar(const CFinancialCalendar &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val.getYear() << val.getDate() << val.getHolidayType() << val.getHolidayinfo();
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplaceFinancialHoliday, paramList))
		return true;
	return false;
}
bool MetaDatabase::removeFinancialCalendar(int val)
{
	FUNCLOG("MetaDatabase::removeFinancialCalendar(int val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << val;
	if (m_DbMgr->ExecuteSQL(DB_SQL_DeleteFinancialHoliday, paramList))
		return true;
	return false;
}
bool MetaDatabase::getFinancialCalendar(QMap<int, CFinancialCalendar> &val)
{
	FUNCLOG("MetaDatabase::getFinancialCalendar(const QMap<int, CFinancialCalendar> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "bwyear" << "bwdate" << "daytype" << "annotation";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectFinancialHolidays, paramList, fieldList, results);
	if (results.isEmpty()) return false;
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

bool MetaDatabase::getFinancialCalendar(int bwdate, CFinancialCalendar &val)
{
	FUNCLOG("MetaDatabase::getFinancialCalendar(int bwdate, CFinancialCalendar &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	paramList << bwdate;
	QStringList fieldList;
	fieldList << "bwyear" << "bwdate" << "daytype" << "annotation";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectFinancialHolidayByDate, paramList, fieldList, results);
	if (results.isEmpty()) {
		return false;
	}
	const QVariantList &result = results[0];
	if (result.size() == 4) {
		val.setYear(result[0].toInt());
		val.setDate(result[1].toInt());
		val.setHolidayType(result[2].toInt());
		val.setHolidayinfo(result[3].toString());
		return true;
	}
	return false;
}

// 组合管理
bool MetaDatabase::setPortfolio(const CPortfolio &val)
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
	return false;
}
bool MetaDatabase::removePortfolio(const QStringList &val)
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
	return false;
}
bool MetaDatabase::getPortfolio(QMap<QString, CPortfolio> &val)
{
	FUNCLOG("MetaDatabase::getPortfolio(QMap<QString, CPortfolio> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "portcode" << "portname" << "parentcode" << "parentname" << "sdate" << "edate" << "annotation";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectPortfolios, paramList, fieldList, results);
	if (results.isEmpty()) return false;
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

//////////////////////////////////产品管理////////////////////////////////////////
bool MetaDatabase::getProduct(QMap<QString, CProduct> &val)
{
	FUNCLOG("MetaDatabase::getProduct(QMap<QString, CProduct> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "productcode" << "productname" << "parentcode" << "parentname" << "sdate" << "edate" << "annotation";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectProduct, paramList, fieldList, results);
	if (results.isEmpty()) {
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
bool MetaDatabase::setProduct(const CProduct &val)
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
	if (m_DbMgr->ExecuteSQL(DB_SQL_ReplaceProduct, paramList)) {
		return true;
	}
	return false;
}
bool MetaDatabase::removeProduct(const QStringList &codeList)
{
	FUNCLOG("MetaDatabase::removeProduct(const QStringList &codeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> allParamList;
	QVariantList paramList;
	foreach(const QString &code, codeList) {
		paramList << code;
	}
	allParamList << paramList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteProduct, allParamList)) {
		return true;
	}
	return false;
}

///////////////////////////////////参数字典///////////////////////////////////////
bool MetaDatabase::getParadict(QList<CParaDict> &val)
{
	FUNCLOG("MetaDatabase::getParadict(QList<CParaDict> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "typecode" << "typename" << "paracode" << "paraname" << "paraexplain";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectParadict, paramList, fieldList, results);
	if (results.isEmpty()) {
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
		val.push_back(paraDict);
	}
	return true;
}

bool MetaDatabase::setParadict(const CParaDict &val)
{
	FUNCLOG("MetaDatabase::setParadict(const CParaDict &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<CParaDict> valList;
	valList.push_back(val);
	return setParadict(valList);
}

bool MetaDatabase::setParadict(const QList<CParaDict> &valList)
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
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_ReplaceParadict, allParamList)) {
		return true;
	}
	return false;
}

bool MetaDatabase::removeParadict(const QStringList &typeCodeList, const QStringList &paraCodeList)
{
	FUNCLOG("MetaDatabase::removeParadict(const QStringList &typeCodeList, const QStringList &paraCodeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (typeCodeList.isEmpty() || typeCodeList.size() != paraCodeList.size()) {
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList typeList, paraList;
	for (int i = 0; i < typeCodeList.size(); i++) {
		typeList << typeCodeList[i];
		paraList << paraCodeList[i];
	}
	allParamList << typeList << paraList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteParadictByTypeParaCode, allParamList)) {
		return true;
	}
	return false;
}

bool MetaDatabase::removeParadict(const QStringList &typeCodeList)
{
	FUNCLOG("MetaDatabase::removeParadict(const QStringList &typeCodeList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (typeCodeList.isEmpty()) {
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList typeList;
	for (int i = 0; i < typeCodeList.size(); i++) {
		typeList << typeCodeList[i];
	}
	allParamList << typeList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteParadictByTypeCode, allParamList)) {
		return true;
	}
	return false;
}

/////////////////////////////////////关键点定义/////////////////////////////////////
bool MetaDatabase::getKeypoint(QMap<QString, CKeypoint> &val)
{
	FUNCLOG("MetaDatabase::getKeypoint(QMap<QString, CKeypoint> &val)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QVariantList paramList;
	QStringList fieldList;
	fieldList << "kpcode" << "kpname" << "productcode" << "productname" << "tenor" << "marketcode" << "marketname" << "calendar" << "convention" << "daycount" << "spotlag" << "couponfrequency" << "refindex";
	QList<QVariantList> results;
	m_DbMgr->QueryFields(DB_SQL_SelectKeypoint, paramList, fieldList, results);
	if (results.isEmpty()) {
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
		keypoint.setCalendar(valList.at(7).toString());
		keypoint.setConvention(valList.at(8).toString());
		keypoint.setDayCount(valList.at(9).toString());
		keypoint.setSpotlat(valList.at(10).toString());
		keypoint.setCouponfrequency(valList.at(11).toString());
		keypoint.setRefindex(valList.at(12).toString());
		if (keypoint.getKpcode().isEmpty() || keypoint.getTenor().isEmpty()) {
			continue;
		}
		val[keypoint.getKpcode()] = keypoint;
	}
	return true;

}
bool MetaDatabase::setKeypoint(const QList<CKeypoint> &valList)
{
	FUNCLOG("MetaDatabase::setKeypoint(const QList<CKeypoint> &valList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	QList<QVariantList> allParamList;
	QVariantList kpcodeList, kpnameList, productcodeList, productnameList, tenorList, marketcodeList, 
		marketnameList, calendarList, conventionList, daycountList, spotlagList, couponfrequencyList, refindexList;
	foreach(const CKeypoint &val, valList) {
		kpcodeList << val.getKpcode();
		kpnameList << val.getKpname();
		productcodeList << val.getProductCode();
		productnameList << val.getProductName();
		tenorList << val.getTenor();
		marketcodeList << val.getMarketCode();
		marketnameList << val.getMarketName();
		calendarList << val.getCalendar();
		conventionList << val.getConvention();
		daycountList << val.getDayCount();
		spotlagList << val.getSpotlat();
		couponfrequencyList << val.getCouponfrequency();
		refindexList << val.getRefindex();
	}
	allParamList << kpcodeList << kpnameList << productcodeList << productnameList << tenorList << marketcodeList
		<< marketnameList << calendarList << conventionList << daycountList << spotlagList << couponfrequencyList << refindexList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_ReplaceKeypoint, allParamList)) {
		return true;
	}
	return false;
}
bool MetaDatabase::removeKeypoint(const QStringList &valList)
{
	FUNCLOG("MetaDatabase::removeKeypoint(const QStringList &valList)");
	W_RETURN_VAL_IF_FAIL(NULL != m_DbMgr, false);
	if (valList.isEmpty()) {
		return false;
	}
	QList<QVariantList> allParamList;
	QVariantList kpcodeList;
	for (int i = 0; i < valList.size(); i++) {
		kpcodeList << valList[i];
	}
	allParamList << kpcodeList;
	if (m_DbMgr->ExecuteBatchSQL(DB_SQL_DeleteKeypoint, allParamList)) {
		return true;
	}
	return false;
}