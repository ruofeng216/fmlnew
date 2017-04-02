#include "datatype.h"

////////////////////////////////////////////////////////////////////
//CMyBase
CMyBase::CMyBase(const QString& name)
{
	m_className = name;
	m_errorCode = e_Success;
}

CMyBase::~CMyBase()
{
}
const QString& CMyBase::getClassName() const
{
	return m_className;
}
void CMyBase::setErrorCode(eERR errorCode)
{
	m_errorCode = errorCode;
}
eERR CMyBase::getErrorCode() const
{
	return m_errorCode;
}
//////////////////////////////////////////////////////////////////////
//CMyField
CMyField::CMyField()
{
}
CMyField::CMyField(const QVariant &val, const QString &key, const QString &des)
	: m_key(key)
	, m_val(val)
	, m_describe(des)
{
}
CMyField::~CMyField()
{

}
void CMyField::setKey(const QString &key)
{
	m_key = key;
}
const QString &CMyField::getKey() const
{
	return m_key;
}
void CMyField::setVal(const QVariant &val)
{
	m_val = val;
}
const QVariant &CMyField::getVal() const
{
	return m_val;
}
void CMyField::setDes(const QString &val)
{
	m_describe = val;
}
const QString &CMyField::getDes() const
{
	return m_describe;
}
//////////////////////////////////////////////////////////////////////
// login
CLogin::CLogin(const QString &name, const QString &pswd)
	: CMyBase(CLASSNAME_CLOGIN)
{
	m_uname.setKey("uname");
	m_uname.setVal(name);
	m_password.setKey("password");
	m_password.setVal(pswd);
}
CLogin::~CLogin()
{
}

void CLogin::setUname(const QString &name)
{
	m_uname.setVal(name);
}
const QString CLogin::getUname() const
{
	return m_uname.getVal().toString();
}
const QString CLogin::getUnameKey() const
{
	return m_uname.getKey();
}

void CLogin::setPassword(const QString &pswd)
{
	m_password.setVal(pswd);
}
const QString CLogin::getPassword() const
{
	return m_password.getVal().toString();
}
const QString &CLogin::getPasswordKey() const
{
	return m_password.getKey();
}

const QVariantMap CLogin::toJson()
{
	QVariantMap mJson;
	mJson["classname"] = getClassName();
	mJson[getUnameKey()] = getUname();
	mJson[getPasswordKey()] = getPassword();
	return mJson;
}
bool CLogin::fromJson(const QVariantMap &val)
{
	if (val.contains("classname") &&
		val["classname"]==getClassName())
	{
		if (val.contains(getUnameKey())) setUname(val[getUnameKey()].toString());
		if (val.contains(getPasswordKey())) setPassword(val[getPasswordKey()].toString());
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
CFuncInfo::CFuncInfo()
	:CMyBase(CLASSNAME_CFUNCINFO)
{
	m_FuncName.setKey("funcname");
	m_FuncID.setKey("funcid");
	m_FuncDesc.setKey("funcdesc");
	m_Order.setKey("order");
}
CFuncInfo::~CFuncInfo()
{
}
void CFuncInfo::setFuncName(const CMyField &funcname)
{
	m_FuncName.setVal(funcname.getVal());
	if (m_FuncName.getDes().isEmpty())
		m_FuncName.setDes(funcname.getDes());
}
const CMyField &CFuncInfo::getFuncName() const
{
	return m_FuncName;
}

void CFuncInfo::setFuncID(const CMyField &funcID)
{
	m_FuncID.setVal(funcID.getVal());
	if (m_FuncID.getDes().isEmpty())
		m_FuncID.setDes(funcID.getDes());
}
const CMyField &CFuncInfo::getFuncID() const
{
	return m_FuncID;
}

void CFuncInfo::setFuncDesc(const CMyField &funcdesc)
{
	m_FuncDesc.setVal(funcdesc.getVal());
	if (m_FuncDesc.getDes().isEmpty())
		m_FuncDesc.setDes(funcdesc.getDes());
}
const CMyField &CFuncInfo::getFuncDesc() const
{
	return m_FuncDesc;
}

void CFuncInfo::setOrder(const CMyField &order)
{
	m_Order.setVal(order.getVal());
	if (m_Order.getDes().isEmpty())
		m_Order.setDes(order.getDes());
}
const CMyField &CFuncInfo::getOrder() const
{
	return m_Order;
}

const QVariantMap CFuncInfo::toJson()
{
	QVariantMap mJson;
	mJson["classname"] = getClassName();
	mJson[getFuncName().getKey()] = getFuncName().getVal();
	mJson[getFuncID().getKey()] = getFuncID().getVal();
	mJson[getFuncDesc().getKey()] = getFuncDesc().getVal();
	mJson[getOrder().getKey()] = getOrder().getVal();
	return mJson;
}
bool CFuncInfo::fromJson(const QVariantMap &val)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////
CFinancialCalendar::CFinancialCalendar(int _y, int _d, EHType e, const QString &val)
	:m_year(_y)
	,m_date(_d)
	,m_holidayinfo(val)
	,m_holidayType(e)
{
}
CFinancialCalendar::~CFinancialCalendar()
{
}
bool CFinancialCalendar::operator==(const CFinancialCalendar &val) const
{
	return m_year == val.getYear() &&
		m_date == val.getDate() &&
		m_holidayinfo == val.getHolidayinfo() &&
		m_holidayType == val.getHolidayType();
}
void CFinancialCalendar::setYear(const int &val)
{
	m_year = val;
}
const int &CFinancialCalendar::getYear() const
{
	return m_year;
}
void CFinancialCalendar::setDate(const int &val)
{
	m_date = val;
}
const int &CFinancialCalendar::getDate() const
{
	return m_date;
}
void CFinancialCalendar::setHolidayType(const int &val)
{
	m_holidayType = EHType(val);
}
int CFinancialCalendar::getHolidayType() const
{
	return m_holidayType;
}
const QString CFinancialCalendar::getHolidayTypeStr() const
{
	switch (m_holidayType)
	{
	case eHoliday:
		return QObject::tr("holiday");
	case eWorkDay:
		return QObject::tr("workday");
	default:
		return "";
	}
}
void CFinancialCalendar::setHolidayinfo(const QString &val)
{
	m_holidayinfo = val;
}
const QString &CFinancialCalendar::getHolidayinfo() const
{
	return m_holidayinfo;
}

//////////////////////////////////////////////////////////////////
CPortfolio::CPortfolio(const QString& _portcode,
	const QString& _portname,
	const QString& _parentcode,
	const QString& _parentname,
	int _sdate,
	int _edate,
	const QString &_annotation)
{
	m_portcode = _portcode;
	m_portname = _portname;
	m_parentcode = _parentcode;
	m_parentname = _parentname;
	m_sdate = _sdate;
	m_edate = _edate;
	m_annotation = _annotation;
}
CPortfolio::~CPortfolio()
{
}
bool CPortfolio::operator==(const CPortfolio &val) const
{
	return m_portcode == val.getPortcode() &&
		m_portname == val.getPortname() &&
		m_parentcode == val.getParentcode() &&
		m_parentname == val.getParentname() &&
		m_sdate == val.getSdate() &&
		m_edate == val.getEdate() &&
		m_annotation == val.getAnnotation();
}
void CPortfolio::setPortcode(const QString &val)
{
	m_portcode = val;
}
const QString &CPortfolio::getPortcode() const
{
	return m_portcode;
}
void CPortfolio::setPortname(const QString &val)
{
	m_portname = val;
}
const QString &CPortfolio::getPortname() const
{
	return m_portname;
}
void CPortfolio::setParentcode(const QString &val)
{
	m_parentcode = val;
}
const QString &CPortfolio::getParentcode() const
{
	return m_parentcode;
}
void CPortfolio::setParentname(const QString &val)
{
	m_parentname = val;
}
const QString &CPortfolio::getParentname() const
{
	return m_parentname;
}
void CPortfolio::setSdate(const int &val)
{
	m_sdate = val;
}
const int &CPortfolio::getSdate() const
{
	return m_sdate;
}
void CPortfolio::setEdate(const int &val)
{
	m_edate = val;
}
const int &CPortfolio::getEdate() const
{
	return m_edate;
}
void CPortfolio::setAnnotation(const QString &val)
{
	m_annotation = val;
}
const QString &CPortfolio::getAnnotation() const
{
	return m_annotation;
}

///////////////////////////////////////////////////////////////
CProduct::CProduct()
	: m_sdate(0), m_edate(0)
{
}

CProduct::CProduct(const QString &code
	, const QString &name
	, const QString &parentCode
	, const QString &parentName
	, int sdate, int edate
	, const QString &annotation) 
	: m_code(code)
	, m_name(name)
	, m_parentCode(parentCode)
	, m_parentName(parentName)
	, m_sdate(sdate)
	, m_edate(edate)
	, m_annotation(annotation)
{
}

CProduct::~CProduct()
{

}

bool CProduct::operator==(const CProduct &val)
{
	return m_code == val.getCode() &&
		m_name == val.getName() &&
		m_parentCode == val.getParentCode() &&
		m_parentName == val.getParentName() &&
		m_sdate == val.getSdate() &&
		m_edate == val.getEdate() &&
		m_annotation == val.getAnnotation();
}

void CProduct::setCode(const QString &code)
{
	m_code = code;
}
const QString& CProduct::getCode() const
{
	return m_code;
}
void CProduct::setName(const QString &name)
{
	m_name = name;
}
const QString& CProduct::getName() const
{
	return m_name;
}
void CProduct::setParentCode(const QString &parentCode)
{
	m_parentCode = parentCode;
}
const QString& CProduct::getParentCode() const
{
	return m_parentCode;
}
void CProduct::setParentName(const QString &parentName)
{
	m_parentName = parentName;
}
const QString& CProduct::getParentName() const
{
	return m_parentName;
}
void CProduct::setSdate(int sdate)
{
	m_sdate = sdate;
}
int CProduct::getSdate() const
{
	return m_sdate;
}
void CProduct::setEdate(int edate)
{
	m_edate = edate;
}
int CProduct::getEdate() const
{
	return m_edate;
}
void CProduct::setAnnotation(const QString &annotation)
{
	m_annotation = annotation;
}
const QString& CProduct::getAnnotation() const
{
	return m_annotation;
}

////////////////////////////////////////////////////////////////
CParaDict::CParaDict()
{
}

CParaDict::CParaDict(const QString &typeCode,
	const QString &typeName,
	const QString &paraCode,
	const QString &paraName,
	const QString &paraExplain)
	: m_typeCode(typeCode)
	, m_typeName(typeName)
	, m_paraCode(paraCode)
	, m_paraName(paraName)
	, m_paraExplain(paraExplain)

{
}

CParaDict::~CParaDict()
{
}

bool CParaDict::operator==(const CParaDict &val)
{
	return m_typeCode == val.getTypeCode() && 
		m_typeName == val.getTypeName() &&
		m_paraCode == val.getParaCode() && 
		m_paraName == val.getParaName() &&
		m_paraExplain == val.getParaExplain();
}
void CParaDict::setTypeCode(const QString &typeCode)
{
	m_typeCode = typeCode;
}
const QString& CParaDict::getTypeCode() const
{
	return m_typeCode;
}
void CParaDict::setTypeName(const QString &typeName)
{
	m_typeName = typeName;
}
const QString& CParaDict::getTypeName() const
{
	return m_typeName;
}
void CParaDict::setParaCode(const QString &paraCode)
{
	m_paraCode = paraCode;
}
const QString& CParaDict::getParaCode() const
{
	return m_paraCode;
}
void CParaDict::setParaName(const QString &paraName)
{
	m_paraName = paraName;
}
const QString& CParaDict::getParaName() const
{
	return m_paraName;
}
void CParaDict::setParaExplain(const QString &paraExplain)
{
	m_paraExplain = paraExplain;
}
const QString& CParaDict::getParaExplain() const
{
	return m_paraExplain;
}

bool CParaDict::isTypeData() const
{
	return !m_typeCode.isEmpty() && m_paraCode.isEmpty();
}

CParaDict CParaDict::getTypeData() const
{
	if (this->isTypeData()) {
		return *this;
	} else {
		return CParaDict(m_typeCode, m_typeName);
	}
}

void CParaDict::setTypeData(const CParaDict &val)
{
	if (this->isTypeData()) {
		m_paraExplain = val.getParaExplain();
	}
	m_typeCode = val.getTypeCode();
	m_typeName = val.getTypeName();
}

bool CParaDict::isTypeDataEqual(const CParaDict &val)
{
	return this->getTypeData() == val.getTypeData();
}

////////////////////////////////////////////////////////////////

demoStruct::demoStruct(const QVariant &val)
	: CMyBase(CLASSNAME_DEMOSTRUCT)
{
	m_bondid.setKey("bondid");
	m_bid.setKey("bid");
	m_volBid.setKey("volbid");
	m_ofr.setKey("ofr");
	m_volOfr.setKey("volofr");
	m_sn.setKey("sn");
	if (val != NULL)
	{
		fromJson(val.toMap());
	}
}
demoStruct::~demoStruct()
{
}
void demoStruct::setBondid(const CMyField &bondid)
{
	m_bondid.setVal(bondid.getVal());
	if (!m_bondid.getDes().isEmpty())
		m_bondid.setDes(bondid.getDes());
}
const CMyField &demoStruct::getBondid() const
{
	return m_bondid;
}

void demoStruct::setBid(const CMyField &bid)
{
	m_bid.setVal(bid.getVal());
	if (!m_bid.getDes().isEmpty())
		m_bid.setDes(bid.getDes());
}
const CMyField &demoStruct::getBid() const
{
	return m_bid;
}

void demoStruct::setOfr(const CMyField &ofr)
{
	m_ofr.setVal(ofr.getVal());
	if (!m_ofr.getDes().isEmpty())
		m_ofr.setDes(ofr.getDes());
}
const CMyField &demoStruct::getOfr() const
{
	return m_ofr;
}

void demoStruct::setVolBid(const CMyField &volbid)
{
	m_volBid.setVal(volbid.getVal());
	if (!m_volBid.getDes().isEmpty())
		m_volBid.setDes(volbid.getDes());
}
const CMyField &demoStruct::getVolBid() const
{
	return m_volBid;
}

void demoStruct::setVolOfr(const CMyField &volofr)
{
	m_volOfr.setVal(volofr.getVal());
	if (!m_volOfr.getDes().isEmpty())
		m_volOfr.setDes(volofr.getDes());
}
const CMyField &demoStruct::getVolOfr() const
{
	return m_volOfr;
}

void demoStruct::setSN(const CMyField &sn)
{
	m_sn.setVal(sn.getVal());
	if (!m_sn.getDes().isEmpty())
		m_sn.setDes(sn.getDes());
}
const CMyField &demoStruct::getSN() const
{
	return m_sn;
}

const QVariantMap demoStruct::toJson()
{
	QVariantMap mJson;
	mJson["classname"] = getClassName();
	mJson[getBondid().getKey()] = getBondid().getVal();
	mJson[getBid().getKey()] = getBid().getVal();
	mJson[getVolBid().getKey()] = getVolBid().getVal();
	mJson[getOfr().getKey()] = getOfr().getVal();
	mJson[getVolOfr().getKey()] = getVolOfr().getVal();
	mJson[getSN().getKey()] = getSN().getVal();
	return mJson;
}
bool demoStruct::fromJson(const QVariantMap &val)
{
	if (val.contains("classname") &&
		val["classname"] == getClassName())
	{
		if (val.contains(getBondid().getKey())) setBondid(CMyField(val[getBondid().getKey()].toString()));
		if (val.contains(getBid().getKey())) setBid(CMyField(val[getBid().getKey()].toString()));
		if (val.contains(getVolBid().getKey())) setVolBid(CMyField(val[getVolBid().getKey()].toString()));
		if (val.contains(getOfr().getKey())) setOfr(CMyField(val[getOfr().getKey()].toString()));
		if (val.contains(getVolOfr().getKey())) setVolOfr(CMyField(val[getVolOfr().getKey()].toString()));
		if (val.contains(getSN().getKey())) setSN(CMyField(val[getSN().getKey()].toString()));
		return true;
	}
	return false;
}