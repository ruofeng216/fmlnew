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

	const WebSocketClientWrapper *getWrapper() const;

private:
	QWebSocketServer *m_socketSever;
	WebSocketClientWrapper *m_wrapper;
	static WebviewSever *m_pSever;
};
