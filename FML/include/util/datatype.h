#pragma once
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSharedPointer>
#include "pro.h"

class CMyBase
{
public:
	CMyBase(const QString& name);
	virtual ~CMyBase();
	const QString &getClassName() const;

	virtual void setErrorCode(eERR errorCode);
	virtual eERR getErrorCode() const;

	virtual const QVariantMap toJson() = 0;
	virtual bool fromJson(const QVariantMap &val) = 0;

protected:
	QString m_className;               // 派生类的名称
	eERR m_errorCode;               // 错误码，0表示成功
};
typedef QSharedPointer<CMyBase> CMyBasePtr;

class CMyField
{
public:
	CMyField();
	CMyField(const QVariant &val, const QString &key="", const QString &des="");
	~CMyField();
	void setKey(const QString &key);
	const QString &getKey() const;

	void setVal(const QVariant &val);
	const QVariant &getVal() const;

	void setDes(const QString &val);
	const QString &getDes() const;
private:
	QString m_key;
	QVariant m_val;
	QString m_describe;
};


// login
#define CLASSNAME_CLOGIN "CLogin"
class CLogin : public CMyBase
{
public:
	CLogin(const QString &name = "", const QString &pswd = "");
	~CLogin();
	void setUname(const QString &name);
	const QString getUname() const;
	const QString getUnameKey() const;

	void setPassword(const QString &pswd);
	const QString getPassword() const;
	const QString &getPasswordKey() const;

	const QVariantMap toJson();
	bool fromJson(const QVariantMap &val);

protected:
	CMyField m_uname;
	CMyField m_password;
};

///////////////////////////////////////////////////////////
// 功能导航
#define CLASSNAME_CFUNCINFO "CFuncInfo"
class CFuncInfo : public CMyBase
{
public:
	CFuncInfo();
	~CFuncInfo();
	void setFuncName(const CMyField &funcname);
	const CMyField &getFuncName() const;

	void setFuncID(const CMyField &funcID);
	const CMyField &getFuncID() const;

	void setFuncDesc(const CMyField &funcdesc);
	const CMyField &getFuncDesc() const;

	void setOrder(const CMyField &order);
	const CMyField &getOrder() const;

	const QVariantMap toJson();
	bool fromJson(const QVariantMap &val);

protected:
	CMyField m_FuncName;
	CMyField m_FuncID;
	CMyField m_FuncDesc;
	CMyField m_Order;
};


// 金融日历
class CFinancialCalendar
{
public:
	enum EHType { eHoliday=0, eWorkDay };
	CFinancialCalendar(int _y = 0, int _d = 0, EHType e = eHoliday, const QString &val = QString());
	~CFinancialCalendar();
	bool operator==(const CFinancialCalendar &val) const;
	void setYear(const int &val);
	const int &getYear() const;
	void setDate(const int &val);
	const int &getDate() const;
	void setHolidayType(const int &val);
	int getHolidayType() const;
	const QString getHolidayTypeStr() const;
	void setHolidayinfo(const QString &val);
	const QString &getHolidayinfo() const;
protected:
	int m_year;
	int m_date;
	EHType m_holidayType;
	QString m_holidayinfo;
};

// 组合管理
class CPortfolio
{
public:
	CPortfolio(const QString& _portcode = QString(),
		const QString& _portname = QString(),
		const QString& _parentcode = QString(),
		const QString& _parentname = QString(),
		int _sdate = 0, 
		int _edate = 0, 
		const QString &_annotation = QString());
	~CPortfolio();
	bool operator==(const CPortfolio &val) const;
	void setPortcode(const QString &val);
	const QString &getPortcode() const;
	void setPortname(const QString &val);
	const QString &getPortname() const;
	void setParentcode(const QString &val);
	const QString &getParentcode() const;
	void setParentname(const QString &val);
	const QString &getParentname() const;
	void setSdate(const int &val);
	const int &getSdate() const;
	void setEdate(const int &val);
	const int &getEdate() const;
	void setAnnotation(const QString &val);
	const QString &getAnnotation() const;

protected:
	QString m_portcode;
	QString m_portname;
	QString m_parentcode; 
	QString m_parentname;
	int m_sdate;
	int m_edate;
	QString m_annotation;
};
// 产品信息
class CProduct
{
public:
	CProduct();
	CProduct(const QString &code,
		const QString &name = QString(),
		const QString &parentCode = QString(),
		const QString &parentName = QString(),
		int sdate = 0,
		int edate = 0,
		const QString &annotation = QString());
	~CProduct();
	bool operator==(const CProduct &val);
	void setCode(const QString &code);
	const QString& getCode() const;
	void setName(const QString &name);
	const QString& getName() const;
	void setParentCode(const QString &parentCode);
	const QString& getParentCode() const;
	void setParentName(const QString &parentName);
	const QString& getParentName() const;
	void setSdate(int sdate);
	int getSdate() const;
	void setEdate(int edate);
	int getEdate() const;
	void setAnnotation(const QString &annotation);
	const QString& getAnnotation() const;

private:
	QString m_code;
	QString m_name;
	QString m_parentCode;
	QString m_parentName;
	int m_sdate;
	int m_edate;
	QString m_annotation;
};
// 参数字典
class CParaDict {
public:
	CParaDict();
	CParaDict(const QString &typeCode,
		const QString &typeName = QString(),
		const QString &paraCode = QString(),
		const QString &paraName = QString(),
		const QString &paraExplain = QString());
	~CParaDict();
	bool operator==(const CParaDict &val);
	void setTypeCode(const QString &typeCode);
	const QString& getTypeCode() const;
	void setTypeName(const QString &typeName);
	const QString& getTypeName() const;
	void setParaCode(const QString &paraCode);
	const QString& getParaCode() const;
	void setParaName(const QString &paraName);
	const QString& getParaName() const;
	void setParaExplain(const QString &paraExplain);
	const QString& getParaExplain() const;

private:
	QString m_typeCode;
	QString m_typeName;
	QString m_paraCode;
	QString m_paraName;
	QString m_paraExplain;
};

///////////////////////////////////////////////////////////
// demo
#define CLASSNAME_DEMOSTRUCT "DEMOSTRUCT"
class demoStruct : public CMyBase
{
public:
	demoStruct(const QVariant &val = NULL);
	~demoStruct();

	void setBondid(const CMyField &bondid);
	const CMyField &getBondid() const;

	void setBid(const CMyField &bid);
	const CMyField &getBid() const;

	void setOfr(const CMyField &ofr);
	const CMyField &getOfr() const;

	void setVolBid(const CMyField &volbid);
	const CMyField &getVolBid() const;

	void setVolOfr(const CMyField &volofr);
	const CMyField &getVolOfr() const;

	void setSN(const CMyField &sn);
	const CMyField &getSN() const;

	const QVariantMap toJson();
	bool fromJson(const QVariantMap &val);

private:
	CMyField m_bondid;
	CMyField m_bid;
	CMyField m_ofr;
	CMyField m_volBid;
	CMyField m_volOfr;
	CMyField m_sn;
};