#pragma  once

#include <QString>
#include <QPair>

class SkinConfig {
public:
	static QString current();
	static void setCurrent(const QString &skinName);
	static QList<QPair<QString, QString>> list();
};