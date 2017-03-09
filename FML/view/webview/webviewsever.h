#pragma once

#include <QObject>
#include <QWebSocketServer>
#include "websocketclientwrapper.h"

#define WEBVIEWSEVER WebviewSever::instance()


class WebviewSever : public QObject
{
	Q_OBJECT

public:
	WebviewSever(QObject *parent=NULL);
	~WebviewSever();

	void release();

	static WebviewSever *instance();

	WebSocketClientWrapper *getWrapper();

private:
	QWebSocketServer *m_socketSever;
	static WebviewSever *m_pSever;
};
