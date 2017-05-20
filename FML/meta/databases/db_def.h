#pragma once
#include <QString>

#ifndef DB_OPERATER
#define  DB_GetTables QString("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='%1';")
#define  DB_HasTable QString("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='%1' AND TABLE_NAME='%2';")
#define  DB_TB_COLUMNS QString("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='%1' AND TABLE_NAME='%2';")
#define  DB_TB_COLUMNSINFO QString("SELECT COLUMN_NAME,COLUMN_DEFAULT,COLUMN_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA='%1' AND TABLE_NAME='%2';")
#define  DB_DROPTABLE QString("DROP TABLE IF EXISTS %1;")
#define  DB_ALERTTABLE QString("alter table %1 add %2 %3;")
#define  DB_UPDATEDATA QString("update %1 set %2 = %3;")
#endif // !DB_OPERATER
/////////////////////table name///////////////////////////
#ifndef TB_NAME
#define  DB_TBNAME_LOGIN "login"						// 登录
#define  DB_TBNAME_FINANCIALCALENDAR "financialholiday" // 金融日历
#define  DB_TBNAME_PORTFOLIO "portfolio"				// 组合管理
#define  DB_TBNAME_PRODUCT "product"					// 产品管理
#define  DB_TBNAME_PARADICT "paradict"					// 参数字典
#define  DB_TBNAME_KEYPOINT "keypoint"					// 关键点

#endif // !TB_NAME
//////////////////////////////////////////////////////////
//////////////////////table sqls////////////////////////////////////
#ifndef TB_CREATE
#define  DB_TBSQL_LOGIN QString("CREATE TABLE IF NOT EXISTS `%1` ( \
`user` char(50) NOT NULL, \
`pswd` char(50) NOT NULL, \
PRIMARY KEY(`user`) \
); ").arg(DB_TBNAME_LOGIN)

#define  DB_TBSQL_FINANCIALCALENDAR QString("CREATE TABLE IF NOT EXISTS `%1` ( \
`bwyear` int NOT NULL, \
`bwdate` int NOT NULL, \
`daytype` int NOT NULL, \
`annotation` text NOT NULL, \
constraint pk_finholiday PRIMARY KEY(`bwdate`) \
); ").arg(DB_TBNAME_FINANCIALCALENDAR)

#define  DB_TBSQL_PORTFOLIO QString("CREATE TABLE IF NOT EXISTS `%1` (\
`portcode` varchar(180), \
`portname`  varchar(255), \
`parentcode` varchar(255), \
`parentname` varchar(255), \
`sdate`  int, \
`edate`  int, \
`annotation` text, \
constraint pk_portfolio PRIMARY KEY(`portcode`)\
);").arg(DB_TBNAME_PORTFOLIO)

#define  DB_TBSQL_PRODUCT QString("CREATE TABLE IF NOT EXISTS `%1` (\
`productcode` varchar(180), \
`productname` varchar(255), \
`parentcode` varchar(255), \
`parentname` varchar(255), \
`sdate`  int, \
`edate`  int, \
`annotation` text, \
constraint pk_product PRIMARY KEY(`productcode`)\
);").arg(DB_TBNAME_PRODUCT)

#define  DB_TBSQL_PARADICT QString("CREATE TABLE IF NOT EXISTS `%1` (\
`inc` int NOT NULL auto_increment, \
`typecode` varchar(180) NOT NULL, \
`typename` varchar(255), \
`paracode` varchar(180), \
`paraname` varchar(255), \
`paraexplain` text, \
constraint pk_paradict PRIMARY KEY(`inc`), \
constraint uk_paradict UNIQUE(`typecode`,`paracode`)\
);").arg(DB_TBNAME_PARADICT)

#define  DB_TBSQL_KEYPOINT QString("CREATE TABLE IF NOT EXISTS `%1` (\
`kpcode` varchar(180) NOT NULL, \
`kpname` varchar(255), \
`productcode` varchar(180), \
`productname` varchar(255), \
`tenor` varchar(180) NOT NULL, \
`marketcode` varchar(180), \
`marketname` varchar(255), \
`calendarcode` varchar(180), \
`calendarname` varchar(255), \
`conventioncode` varchar(180), \
`conventionname` varchar(255), \
`daycountcode` varchar(180), \
`daycountname` varchar(255), \
`spotlagcode` varchar(180), \
`spotlagname` varchar(255), \
`sdate`  int, \
`edate`  int, \
constraint pk_markettenor PRIMARY KEY(`kpcode`) \
);").arg(DB_TBNAME_KEYPOINT)

#endif // !TB_CREATE
/////////////////////////////////////////////////////////////
///////////////////operate sqls/////////////////////
#ifndef TB_OPERATE
// login
#define DB_SQL_SelectLoginUser QString("SELECT * from %1 where `user`=?; ").arg(DB_TBNAME_LOGIN)
#define DB_SQL_InsertLoginUser QString("INSERT %1(`user`, `pswd`) VALUES(?,?);").arg(DB_TBNAME_LOGIN)
#define DB_SQL_UpdateLoginUser QString("UPDATE %1 SET `pswd`=? WHERE `user`=?;").arg(DB_TBNAME_LOGIN)

// 金融日历
#define DB_SQL_SelectFinancialHolidays QString("SELECT * from %1; ").arg(DB_TBNAME_FINANCIALCALENDAR)
#define DB_SQL_SelectFinancialHolidayByDate QString("SELECT * from %1 where `bwdate`=?; ").arg(DB_TBNAME_FINANCIALCALENDAR)
#define DB_SQL_ReplaceFinancialHoliday QString("replace into %1(bwyear,bwdate,daytype,annotation) values(?,?,?,?);").arg(DB_TBNAME_FINANCIALCALENDAR)
#define DB_SQL_DeleteFinancialHoliday QString("delete from %1 where `bwdate`=?;").arg(DB_TBNAME_FINANCIALCALENDAR)

// 组合管理
#define DB_SQL_SelectPortfolios QString("SELECT * from %1; ").arg(DB_TBNAME_PORTFOLIO)
#define DB_SQL_ReplacePortfolio QString("replace into %1(portcode,portname,parentcode,parentname,sdate,edate,annotation) values(?,?,?,?,?,?,?);").arg(DB_TBNAME_PORTFOLIO)
#define DB_SQL_DeletePortfolio QString("delete from %1 where `portcode`=?;").arg(DB_TBNAME_PORTFOLIO)

// 产品管理
#define DB_SQL_SelectProduct QString("SELECT * from %1;").arg(DB_TBNAME_PRODUCT)
#define DB_SQL_ReplaceProduct QString("replace into %1(productcode,productname,parentcode,parentname,sdate,edate,annotation) values(?,?,?,?,?,?,?);").arg(DB_TBNAME_PRODUCT)
#define DB_SQL_DeleteProduct QString("delete from %1 where `productcode`=?;").arg(DB_TBNAME_PRODUCT)

// 参数字典
#define DB_SQL_SelectParadict QString("SELECT * from %1;").arg(DB_TBNAME_PARADICT)
#define DB_SQL_ReplaceParadict QString("replace into %1(typecode,typename,paracode,paraname,paraexplain) values(?,?,?,?,?);").arg(DB_TBNAME_PARADICT)
#define DB_SQL_DeleteParadictByTypeParaCode QString("delete from %1 where `typecode`=? and `paracode`=?;").arg(DB_TBNAME_PARADICT)
#define DB_SQL_DeleteParadictByTypeCode QString("delete from %1 where `typecode`=?;").arg(DB_TBNAME_PARADICT)

// 关键点定义
#define DB_SQL_SelectKeypoint QString("SELECT * from %1;").arg(DB_TBNAME_KEYPOINT)
#define DB_SQL_ReplaceKeypoint QString("replace into %1(kpcode,kpname,productcode,productname,tenor,marketcode,marketname,calendarcode,calendarname,conventioncode,conventionname,daycountcode,daycountname,spotlagcode,spotlagname,sdate,edate) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);").arg(DB_TBNAME_KEYPOINT)
#define DB_SQL_DeleteKeypoint QString("delete from %1 where `kpcode`=?;").arg(DB_TBNAME_KEYPOINT)

#endif // !TB_OPERATE



