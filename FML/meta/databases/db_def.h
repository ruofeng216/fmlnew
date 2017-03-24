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
#define  DB_TBNAME_LOGIN "login"
#define  DB_TBNAME_FINANCIALCALENDAR "financialholiday"
#define  DB_TBNAME_PORTFOLIO "portfolio"
#define  DB_TBNAME_PRODUCT "product"

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
`productname`  varchar(255), \
`parentcode` varchar(255), \
`parentname` varchar(255), \
`sdate`  int, \
`edate`  int, \
`annotation` text, \
constraint pk_product PRIMARY KEY(`productcode`)\
);").arg(DB_TBNAME_PRODUCT)

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
#define DB_SQL_SelectFinancialHolidayByDate QString("SELECT bwdate from %1 where `bwdate`=?; ").arg(DB_TBNAME_FINANCIALCALENDAR)
#define DB_SQL_ReplaceFinancialHoliday QString("replace into %1(bwyear,bwdate,daytype,annotation) values(?,?,?,?);").arg(DB_TBNAME_FINANCIALCALENDAR)
#define DB_SQL_DeleteFinancialHoliday QString("delete from %1 where `bwdate`=?;").arg(DB_TBNAME_FINANCIALCALENDAR)

// 组合管理
#define DB_SQL_SelectPortfolios QString("SELECT * from %1; ").arg(DB_TBNAME_PORTFOLIO)
#define DB_SQL_ReplacePortfolio QString("replace into %1(portcode,portname,parentcode,parentname,sdate,edate,annotation) values(?,?,?,?,?,?,?);").arg(DB_TBNAME_PORTFOLIO)
#define DB_SQL_DeletePortfolio QString("delete from %1 where `portcode`=?;").arg(DB_TBNAME_PORTFOLIO)

// 产品管理
#define DB_SQL_SelectProduct QString("SELECT * from %1; ").arg(DB_TBNAME_PRODUCT)
#define DB_SQL_ReplaceProduct QString("replace info %1(productcode,productname,parentcode,parentname,sdate,edate,annotation) value(?,?,?,?,?,?,?);").arg(DB_TBNAME_PRODUCT)
#define DB_SQL_DeleteProduct QString("delete from %1 where `productcode`=?;").arg(DB_TBNAME_PRODUCT)


#endif // !TB_OPERATE



