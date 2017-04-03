#include "YieldCurveDefinition.h"
#include "metadatabase.h"
#include "util/util.h"

CYieldCurveDefinition::CYieldCurveDefinition()
{

}

CYieldCurveDefinition::~CYieldCurveDefinition()
{

}

const QMap<QString, CKeypoint>& CYieldCurveDefinition::getKeyPoint()
{
	if (m_keypoint.isEmpty()) {
		METADATABASE->getKeypoint(m_keypoint);
	}
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
bool CYieldCurveDefinition::setKeyPoint(const CKeypoint &val)
{
	QList<CKeypoint> valList;
	valList.push_back(val);
	if (METADATABASE->setKeypoint(valList)) {
		m_keypoint[val.getKpcode()] = val;
		return true;
	}
	return false;
}
bool CYieldCurveDefinition::removeKeyPoint(const QString &kpcode)
{
	if (METADATABASE->removeKeypoint(QStringList() << kpcode)) {
		m_keypoint.remove(kpcode);
		return true;
	}
	return false;
}
