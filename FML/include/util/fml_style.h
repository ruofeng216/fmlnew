#pragma once

#include <QString>
#include <QMap>

class FmlStyle {
public:
	static FmlStyle* instance();
	void init(const QString &styleFileContent);
	const QString& style(const QString &fmlName);
	QString attr(const QString &fmlName, const QString &attrName);

private:
	FmlStyle();

private:
	QMap<QString, QString> m_fmlStyles;
};
