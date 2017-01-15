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

#endif // !TB_NAME
//////////////////////////////////////////////////////////
//////////////////////table sqls////////////////////////////////////
#ifndef TB_CREATE
#define  DB_TBSQL_LOGIN QString("CREATE TABLE IF NOT EXISTS `%1` ( \
`user` char(50) NOT NULL, \
`pswd` char(50) NOT NULL, \
PRIMARY KEY(`user`) \
); ").arg(DB_TBNAME_LOGIN)


#endif // !TB_CREATE
/////////////////////////////////////////////////////////////
///////////////////operate sqls/////////////////////
#ifndef TB_OPERATE
#define DB_SQL_SelectLoginUser "SELECT * from login where `user`=?; "
#define DB_SQL_InsertLoginUser "INSERT login(`user`, `pswd`) VALUES(?,?);"
#define DB_SQL_UpdateLoginUser "UPDATE login SET `pswd`=? WHERE `user`=?;"
#endif // !TB_OPERATE



