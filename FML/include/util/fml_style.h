#pragma once

#include <QString>
#include <QPair>
#include <QList>

class FmlStyle {
public:
	static FmlStyle* instance();
	void init(const QString &styleFileContent);
	QString style(const QString &fmlName);
	QString attr(const QString &fmlName, const QString &attrName);

private:
	FmlStyle();
	QString head(const QString &fmlName);

private:
	QList<QPair<QString, QString>> m_fmlStyles;
};
