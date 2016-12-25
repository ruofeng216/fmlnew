#include "webviewsever.h"

WebviewSever *WebviewSever::m_pSever = NULL;

WebviewSever::WebviewSever(QObject *parent)
	: QObject(parent)
{
	m_socketSever = new QWebSocketServer(QStringLiteral("QWebChannelServer"), QWebSocketServer::NonSecureMode, this);
	if (!m_socketSever->listen(QHostAddress::LocalHost, 9999)) {
		qFatal("Failed to open web socket server.");
		return;
	}
	m_wrapper = new WebSocketClientWrapper(m_socketSever);
}

WebviewSever::~WebviewSever()
{
}

void WebviewSever::release()
{
	if (m_pSever)
	{
		if (m_wrapper)
		{
			m_wrapper->deleteLater();
		}
		m_socketSever->close();
		if (m_socketSever)
		{
			m_socketSever->deleteLater();
		}
		delete m_pSever;
		m_pSever = NULL;
	}
}
WebviewSever *WebviewSever::instance()
{
	if (m_pSever == NULL)
	{
		m_pSever = new WebviewSever(NULL);
	}
	return m_pSever;
}

const WebSocketClientWrapper *WebviewSever::getWrapper() const
{
	return m_wrapper;
}