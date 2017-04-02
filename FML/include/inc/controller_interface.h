#pragma once
#include "util\datatype.h"
#include <QVector>

class ILogin
{
public:
	virtual ~ILogin() = 0;
	// 登录验证
	virtual eERR chkLogin(const QString &uname, const QString &pswd) = 0;
	// 注册
	virtual eERR regLogin(const QString &uname, const QString &pswd) = 0;
	// 修改密码
	virtual eERR modifyLogin(const QString &uname, const QString &pswd) = 0;
};

class IGlobalSetting
{
public:
	virtual ~IGlobalSetting() = 0;
	// 获取功能根节点菜单
	virtual void getRootFunc(QStringList &rootVal) = 0;
	// 获取功能子节点菜单
	virtual void getChildrenFunc(const QString &funcid, QStringList &chVal) = 0;
	// 获取功能父节点菜单
	virtual void getParentFunc(const QString &funcid, QString &parval) = 0;
	// 获取功能info
	virtual void getFuncInfo(const QString &funcid, CFuncInfo &funcinfo) = 0;
	virtual bool getFuncInfoFromName(const QString &name, CFuncInfo &info) = 0;
	// 是否存在功能
	virtual bool isExistFuncID(const QString &funcid) = 0;
};

class IParameterSetting
{
public:
	virtual ~IParameterSetting() = 0;
	//////金融日历////
	virtual bool isExistFinancialCalendar(const CFinancialCalendar &val) = 0;
	virtual bool setFinancialCalendar(const CFinancialCalendar &val) = 0;
	virtual bool removeFinancialCalendar(int val) = 0;
	virtual bool getFinancialCalendar(QMap<int, CFinancialCalendar> &val) = 0;
	virtual bool getFinancialCalendar(int bwdate, CFinancialCalendar &val) = 0;
	///////组合管理///////////
	virtual bool setPortfolio(const CPortfolio &val) = 0;
	virtual bool removePortfolio(const QString &val) = 0;
	virtual const QMap<QString, CPortfolio> &getPortfolio() = 0;
	virtual bool isParentCode(const QString &parent, const QString &child) = 0;
	virtual bool isExistCode(const QString &val) = 0;
	virtual void getAllRootCodes(QStringList &val) = 0;
	virtual void getChildren(const QString &key, QStringList &val) = 0;
	virtual bool getPortfolio(const QString &key, CPortfolio &val) = 0;
	///////产品管理///////////
	virtual const QMap<QString, CProduct>& getProduct() = 0;
	virtual bool getProduct(const QString &code, CProduct &val) = 0;
	virtual bool setProduct(const CProduct &val) = 0;
	virtual bool removeProduct(const QString &code) = 0;
	virtual QList<CProduct> getRootProduct() = 0;
	virtual QList<CProduct> getChildrenProduct(const QString &parentCode) = 0;
	virtual void getAllChildrenProduct(const QString &parentCode, QList<CProduct> &results) = 0;
	///////参数管理///////////
	virtual const QList<CParaDict>& getParadict() = 0;
	virtual bool getParadict(const QString &typeCode, const QString &paraCode, CParaDict &val) = 0;
	virtual bool getAllParadict(const QString &typeCode, QList<CParaDict> &valList) = 0;
	virtual bool setParadict(const CParaDict &val) = 0;
	virtual bool removeParadict(const QString &typeCode, const QString &paraCode) = 0;
};




class IDemo
{
public:
	virtual ~IDemo() = 0;

	virtual bool setData(const QVariant &val) = 0;

	virtual void getData(QVariant &val, int count = 20) = 0;
	virtual int getSN() = 0;
};