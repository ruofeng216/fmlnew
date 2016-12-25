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