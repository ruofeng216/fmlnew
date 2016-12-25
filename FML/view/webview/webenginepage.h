#pragma once

#include <QtWebEngineWidgets/QWebEnginePage>

class WebEnginePage : public QWebEnginePage
{
	Q_OBJECT

public:
	explicit WebEnginePage(QObject *parent = 0);
	~WebEnginePage();

signals:
	void sigLoadUrl(const QUrl &url);

protected:
	bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
	void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID);
};
