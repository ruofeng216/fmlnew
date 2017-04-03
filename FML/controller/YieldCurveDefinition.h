#pragma  once
#include "inc\controller_interface.h"

class CYieldCurveDefinition : public IYieldCurveDefinition
{
public:
	CYieldCurveDefinition();
	~CYieldCurveDefinition();
	/////////////////////////////////////关键点定义/////////////////////////////////////
	const QMap<QString, CKeypoint>& getKeyPoint();
	bool getKeyPoint(const QString &kpcode, CKeypoint &val);
	bool setKeyPoint(const CKeypoint &val);
	bool removeKeyPoint(const QString &kpcode);

private:
	QMap<QString, CKeypoint> m_keypoint;
};


