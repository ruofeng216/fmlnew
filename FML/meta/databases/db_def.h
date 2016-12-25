#define  DB_HasTable "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA='%1' AND TABLE_NAME='%2'"

/////////////////////table name///////////////////////////
#define  DB_TBNAME_LOGIN "login"



//////////////////////////////////////////////////////////
//////////////////////table sqls////////////////////////////////////
#define  DB_TBSQL_LOGIN "CREATE TABLE `login` ( \
`user` char(50) NOT NULL, \
`pswd` char(50) NOT NULL, \
PRIMARY KEY(`user`) \
) ENGINE = InnoDB DEFAULT CHARSET = utf8; "



/////////////////////////////////////////////////////////////
///////////////////operate sqls/////////////////////
#define DB_SQL_SelectLoginUsers ""
#define DB_SQL_InsertLoginUser ""
#define DB_SQL_UpdateLoginUser ""

