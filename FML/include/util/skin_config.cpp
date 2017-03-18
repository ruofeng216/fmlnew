#include "skin_config.h"
#include <QDebug>
#include "util.h"

QString SkinConfig::current()
{
	QDomDocument doc = qutil::GetXmlDom(qutil::getSkinConfigPath());
	if (doc.isNull()) {
		qDebug() << "open skin config error";
		std::string content = "\
<?xml version='1.0' encoding='utf-8'?>\n\
<!--皮肤管理-->\n\
<skin>\n\
	<!--当前选择的皮肤-->\n\
	<current decs=\"当前皮肤\">\n\
		<skin id=\"dark\" />\n\
	</current>\n\
	<!--所有皮肤列表-->\n\
	<list des=\"皮肤列表\">\n\
		<skin id=\"dark\" desc=\"深色版\" />\n\
		<skin id=\"light\" desc=\"浅色版\" />\n\
	</list>\n\
</skin>";
		content = qutil::string_to_utf8(content);
		QFile file(qutil::getSkinConfigPath());
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream out(&file);
			out.setCodec("utf-8");
			out << QString::fromUtf8(content.c_str());
		}
		return "dark";
	}

	QDomElement root = doc.documentElement();
	QDomNodeList currentList = root.elementsByTagName("current");
	if (currentList.size() > 0) {
		QDomElement current = currentList.at(0).toElement();
		QDomNodeList skinList = current.elementsByTagName("skin");
		if (skinList.size() > 0) {
			QDomElement skin = skinList.at(0).toElement();
			return skin.attribute("id");
		}
	}

	return "dark";
}

void SkinConfig::setCurrent(const QString &skinName)
{
	QDomDocument doc = qutil::GetXmlDom(qutil::getSkinConfigPath());
	if (doc.isNull()) {
		qDebug() << "open skin config error";
		return ;
	}

	QDomElement root = doc.documentElement();
	QDomNodeList currentList = root.elementsByTagName("current");
	if (currentList.size() > 0) {
		QDomElement current = currentList.at(0).toElement();
		QDomNodeList skinList = current.elementsByTagName("skin");
		if (skinList.size() > 0) {
			QDomElement skin = skinList.at(0).toElement();
			skin.setAttribute("id", skinName);

			QFile file(qutil::getSkinConfigPath());
			if (file.open(QIODevice::WriteOnly)) {
				QTextStream out(&file);
				doc.save(out, 4);
			}
		}
	}
}

QList<QPair<QString, QString>> SkinConfig::list()
{
	QList<QPair<QString, QString>> result;
	QDomDocument doc = qutil::GetXmlDom(qutil::getSkinConfigPath());
	if (doc.isNull()) {
		qDebug() << "open skin config error";
		return result;
	}

	QDomElement root = doc.documentElement();
	QDomNodeList skinList = root.elementsByTagName("list");
	if (skinList.size() > 0) {
		QDomElement skin = skinList.at(0).toElement();
		QDomNodeList itemList = skin.elementsByTagName("skin");
		for (int i = 0, count = itemList.size(); i < count; i++) {
			QDomElement item = itemList.at(i).toElement();
			QString id = item.attribute("id");
			QString desc = item.attribute("desc");
			if (id.isEmpty()) {
				continue;
			}
			result.push_back(qMakePair(id, desc));
		}
	}
	return result;
}