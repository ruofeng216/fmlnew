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
}

WebviewSever::~WebviewSever()
{
}

void WebviewSever::release()
{
	if (m_pSever)
	{
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

WebSocketClientWrapper *WebviewSever::getWrapper()
{
	return new WebSocketClientWrapper(m_socketSever);
}