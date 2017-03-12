#include <QtWidgets/QApplication>
#include "qmpreload.h"
#include <QCoreApplication>
#include "util/log.h"

int main(int argc, char *argv[])
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
	qInstallMessageHandler(customMessageHandler);
#ifdef _DEBUG
	qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9000");
#endif
	// If this property is true, the applications quits when the last visible primary window (i.e. window with no parent) is closed.
	a.setQuitOnLastWindowClosed(false);
	CrashHook crashHook(new QmCrashEvents());
	// ≥Ã–Ú≥ı ºªØ
	QmPreload qmInit;
	if (!qmInit.init(argc, argv)) {
		return false;
	}
    return a.exec();
}
