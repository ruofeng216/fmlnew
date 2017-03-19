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
		int start = -1;
		int end = -1;
		start = styleFileContent.indexOf("{", pos);
		if (start >= 0) {
			end = styleFileContent.indexOf("}", start);
		}
		if (end - start > 1) {
			QString head = styleFileContent.mid(pos, start - pos - 1);
			QString body = styleFileContent.mid(start + 1, end - start - 1);
			pos += body.length();
			head = head.trimmed();
			body = body.trimmed();
			if (body.size() > 0) {
				m_fmlStyles.push_back(qMakePair(head, body));
			}
		} else {
			pos += rx.matchedLength();
		}
	}
}

QString FmlStyle::style(const QString &fmlName)
{
	QString result;
	QString head = this->head(fmlName);
	for (int i = 0; i < m_fmlStyles.size(); i++) {
		if (m_fmlStyles[i].first.contains(head)) {
			result += QString("%1{%2}").arg(m_fmlStyles[i].first).arg(m_fmlStyles[i].second);
		}
	}
	return result;
}

QString FmlStyle::attr(const QString &fmlName, const QString &attrName)
{
	QString head = this->head(fmlName);
	for (int i = 0; i < m_fmlStyles.size(); i++) {
		if (m_fmlStyles[i].first == head) {
			QStringList strList = m_fmlStyles[i].second.split(":");
			if (strList.size() > 1) {
				return strList[1].remove(";").trimmed();
			}
		}
	}
	return "";
}

QString FmlStyle::head(const QString &fmlName)
{
	QString head;
	int pos = fmlName.indexOf("::");
	if (pos > 0) {
		QString extStatus = fmlName.right(fmlName.size() - pos);
		head = QString("QWidget[fmlName=\"%1\"]%2").arg(fmlName.left(pos)).arg(extStatus);
	} else {
		head = QString("QWidget[fmlName=\"%1\"]").arg(fmlName);
	}
	return head;
}