#include "qcontrollermanager.h"
#include "LoginController.h"
#include "globalsetcontroller.h"
#include "ParameterSetting.h"

#include "DemoController.h"


QControllerManager* QControllerManager::s_instance = NULL;
QControllerManager::QControllerManager(QObject *parent)
	: QObject(parent)
	, m_pLoginCtrl(NULL)
	, m_pDemo(NULL)
	, m_pGlobalSetting(NULL)
	, m_pParameterSetting(NULL)
{
}

QControllerManager::~QControllerManager()
{
	if (m_pLoginCtrl)
	{
		delete m_pLoginCtrl;
		m_pLoginCtrl = NULL;
	}
	if (m_pGlobalSetting)
	{
		delete m_pGlobalSetting;
		m_pGlobalSetting = NULL;
	}
	if (m_pParameterSetting)
	{
		delete m_pParameterSetting;
		m_pParameterSetting = NULL;
	}



	if (m_pDemo)
	{
		delete m_pDemo;
		m_pDemo = NULL;
	}
	if (m_DemoCrawler.isRunning())
	{
		m_DemoCrawler.terminate();
		m_DemoCrawler.wait();
	}
	
}

// ��ȡʵ��
QControllerManager *QControllerManager::instance()
{
	if (NULL == s_instance)
	{
		s_instance = new QControllerManager();
	}
	return s_instance;
}
void QControllerManager::release()
{
	if (NULL != s_instance)
	{
		delete s_instance;
		s_instance = NULL;
	}
}

ILogin *QControllerManager::getLoginInst()
{
	if (m_pLoginCtrl == NULL)
	{
		m_pLoginCtrl = new CLoginController();
	}
	return m_pLoginCtrl;
}

IGlobalSetting *QControllerManager::getGlobalSettingInst()
{
	if (m_pGlobalSetting == NULL)
	{
		m_pGlobalSetting = new GlobalSetController();
	}
	return m_pGlobalSetting;
}

IParameterSetting *QControllerManager::getParameterSetting()
{
	if (m_pParameterSetting == NULL)
	{
		m_pParameterSetting = new CParameterSetting();
	}
	return m_pParameterSetting;
}


IDemo *QControllerManager::getDemoInst()
{
	if (m_pDemo == NULL)
	{
		m_pDemo = new CDemoController();
	}
	return m_pDemo;
}

void QControllerManager::startMonitor()
{
	m_DemoCrawler.start();
}

void QControllerManager::stopMonitor()
{
	m_DemoCrawler.terminate();
}