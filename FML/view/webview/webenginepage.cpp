#include "webenginepage.h"

WebEnginePage::WebEnginePage(QObject *parent)
	: QWebEnginePage(parent)
{
}

WebEnginePage::~WebEnginePage()
{
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
	if (isMainFrame) {
		if (NavigationTypeLinkClicked == type) {
			emit sigLoadUrl(url);
			return false;
		}
	}
	return true;
}
void WebEnginePage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
	QString levelStr = (level == 0 ? "INFO" : (level == 1 ? "WARN" : "ERROR"));
	QString htmlName = sourceID;
	int findPos = sourceID.lastIndexOf("/");
	if (findPos >= 0) {
		htmlName = sourceID.mid(findPos + 1);
	}
	QString webLog = QString("[WEBLOG level:%1,line:%2,name:%3] %4").arg(levelStr).arg(lineNumber).arg(htmlName).arg(message);
	qDebug() << webLog;
}