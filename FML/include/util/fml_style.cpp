#include "fml_style.h"


FmlStyle::FmlStyle()
{
}

FmlStyle* FmlStyle::instance()
{
	static FmlStyle s_inst;
	return &s_inst;
}

void FmlStyle::init(const QString &styleFileContent)
{
	int pos = 0;
	QRegExp rx("QWidget\\[fmlName=\"\\w+\"\\]"); // Æ¥Åäkey£¬Èç£ºQWidget[fmlName="calendar"]
	while ((pos = rx.indexIn(styleFileContent, pos)) != -1) {
		QString head = styleFileContent.mid(pos, rx.matchedLength());
		pos += rx.matchedLength();
		int start = -1;
		int end = -1;
		start = styleFileContent.indexOf("{", pos);
		if (start >= 0) {
			end = styleFileContent.indexOf("}", start);
		}
		if (end - start > 1) {
			QString body = styleFileContent.mid(start + 1, end - start - 1);
			pos += body.length();
			body = body.trimmed();
			if (body.size() > 0) {
				m_fmlStyles[head] = body;
			}
		}
	}
}

const QString& FmlStyle::style(const QString &fmlName)
{
	QString head = QString("QWidget[fmlName=\"%1\"]").arg(fmlName);
	return m_fmlStyles[head];
}

QString FmlStyle::attr(const QString &fmlName, const QString &attrName)
{
	const QString &body = style(fmlName);
	QStringList attrList = body.split(";");
	for (int i = 0, count = attrList.size(); i < count; i++) {
		QStringList strList = attrList[i].split(":");
		if (strList.size() == 2) {
			if (strList[0].trimmed() == attrName) {
				return strList[1].trimmed();
			}
		}
	}
	return "";
}