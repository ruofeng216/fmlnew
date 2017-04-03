#pragma once

#include <QObject>
#include "inc/controller_interface.h"
#include "democrawler.h"

#define CONTROLMGR QControllerManager::instance()
#define LONGINCTL QControllerManager::instance()->getLoginInst()
#define GLBSETCTL QControllerManager::instance()->getGlobalSettingInst()
#define PARASETCTL QControllerManager::instance()->getParameterSetting()
#define YIELDCURVECTL QControllerManager::instance()->getYieldCurveDefinition()

class QControllerManager : public QObject
{
	Q_OBJECT

public:
	QControllerManager(QObject *parent = NULL);
	~QControllerManager();

	// »ñÈ¡ÊµÀý
	static QControllerManager *instance();
	// release
	void release();

	ILogin *getLoginInst();
	IGlobalSetting *getGlobalSettingInst();
	IParameterSetting *getParameterSetting();
	IYieldCurveDefinition* getYieldCurveDefinition();

	IDemo *getDemoInst();

	void startMonitor();
	void stopMonitor();

private:
	static QControllerManager* s_instance;

	ILogin *m_pLoginCtrl;
	IGlobalSetting *m_pGlobalSetting;
	IParameterSetting *m_pParameterSetting;
	IYieldCurveDefinition *m_pYieldCurveDefinition;

	IDemo *m_pDemo;
	DemoCrawler m_DemoCrawler;
};
