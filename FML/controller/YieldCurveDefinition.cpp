#include "YieldCurveDefinition.h"
#include "metadatabase.h"
#include "util/util.h"

CYieldCurveDefinition::CYieldCurveDefinition()
{
	if (m_keypoint.isEmpty())
	{
		QString strErr;
		if (!METADATABASE->getKeypoint(m_keypoint, strErr) && !strErr.isEmpty())
			qDebug() << strErr;
	}
}

CYieldCurveDefinition::~CYieldCurveDefinition()
{

}

const QMap<QString, CKeypoint>& CYieldCurveDefinition::getKeyPoint() const
{
	return m_keypoint;
}
bool CYieldCurveDefinition::getKeyPoint(const QString &kpcode, CKeypoint &val)
{
	if (kpcode.isEmpty()) {
		return false;
	}
	if (m_keypoint.contains(kpcode)) {
		val = m_keypoint[kpcode];
		return true;
	}
	return false;
}
bool CYieldCurveDefinition::setKeyPoint(const CKeypoint &val, QString &err)
{
	QList<CKeypoint> valList;
	valList.push_back(val);
	if (METADATABASE->setKeypoint(valList, err)) {
		m_keypoint[val.getKpcode()] = val;
		return true;
	}
	return false;
}
bool CYieldCurveDefinition::removeKeyPoint(const QString &kpcode, QString &err)
{
	if (METADATABASE->removeKeypoint(QStringList() << kpcode, err)) {
		m_keypoint.remove(kpcode);
		return true;
	}
	return false;
}
