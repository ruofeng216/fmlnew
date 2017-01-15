#pragma once

#include <QObject>
#include "util/datatype.h"

#define METADATABASE MetaDatabase::instance()

class QDBMgr;
class MetaDatabase : public QObject
{
	Q_OBJECT
public:
	// »ñÈ¡ÊµÀý
	static MetaDatabase *instance();

	bool getLoginInfo(const QString &username, CLogin &lval);
	bool setLoginInfo(const CLogin &uinfo);
	bool updateLoginInfo(const CLogin &uinfo);

private:
	MetaDatabase(QObject *parent = NULL);
	~MetaDatabase();

private:
	QDBMgr *m_DbMgr;

};
