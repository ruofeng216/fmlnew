#include "basewebchannel.h"
#include <QWebChannel>
#include "webviewsever.h"
#include "websockettransport.h"
#include <QDebug>
#include "util/datatype.h"
#include "controller/qcontrollermanager.h"

BaseWebChannel::BaseWebChannel(QObject *parent)
	: QObject(parent)
{
	m_channel = new QWebChannel(this);
	m_channel->registerObject("CallCpp", this);
	QObject::connect(WEBVIEWSEVER->getWrapper(), &WebSocketClientWrapper::clientConnected,
		m_channel, &QWebChannel::connectTo);
}

BaseWebChannel::~BaseWebChannel()
{
}

const QWebChannel *BaseWebChannel::channel() const
{
	return m_channel;
}

///////////////////////////////////////////////////////////
// DemolWebChannel
DemolWebChannel::DemolWebChannel(QObject *parent)
	: BaseWebChannel(parent)
{
}
DemolWebChannel::~DemolWebChannel()
{
}

const QVariant DemolWebChannel::slotHandle(const QVariant &val)
{
	return QVariant();
}
// DemolWebChannel
DemolWebChannel1::DemolWebChannel1(QObject *parent)
	: BaseWebChannel(parent)
{
}
DemolWebChannel1::~DemolWebChannel1()
{
}

const QVariant DemolWebChannel1::slotHandle(const QVariant &val)
{
	QVariant ss = val;
	if (ss.toMap().contains("function") && ss.toMap()["function"].toString() == "adding")
		QControllerManager::instance()->getDemoInst()->setData(ss);
	else if (ss.toMap().contains("function") && ss.toMap()["function"].toString() == "monitoring")
	{
		bool ismonitor = ss.toMap()["ismonitor"].toBool();
		if (ismonitor)
			QControllerManager::instance()->startMonitor();
		else
			QControllerManager::instance()->stopMonitor();
	}
	return QVariant();
}
