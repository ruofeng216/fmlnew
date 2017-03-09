#include <QtWidgets/QApplication>
#include "qmpreload.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker> 
#include <QSettings>
#include <QDate>
#include <QThread>
#include <QDir>
#include "util/log.h"
#include "util/util.h"


#define _TIME_ qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))

static QMutex m_gLocker;
static QString m_loglevel = "";
//回调函数实现debug信息到文件
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QStringList logStr = QStringList() << "TRACE" << "DEBUG" << "INFO" << "WARN" << "ERROR" << "FATAL";
	LogLevel _log;
	if (m_loglevel.isEmpty())
	{
		QSettings configIniRead(qutil::qBaseConfigPath(), QSettings::IniFormat);
		m_loglevel = configIniRead.value("/Base/loglevel").toString().toUpper();
		m_nlog = logStr.indexOf(m_loglevel);
	}

	switch (type) {
	case QtInfoMsg:
		_log = LL_INFO;
	case QtDebugMsg:
		_log = LL_DEBUG;
		break;
	case QtWarningMsg:
		_log = LL_WARN;
		break;
	case QtCriticalMsg:
		_log = LL_ERROR;
		break;
	case QtFatalMsg:
		_log = LL_FATAL;
		abort();
	default:
		_log = LL_TRACE;
	}
	QMutexLocker locker(&m_gLocker);
	QDir().mkpath(qutil::logDir());
	QFile outFile(qutil::logDir() + QString("\\%1.log").arg(QDate::currentDate().toString("yyyyMMdd")));
	if (outFile.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream ts(&outFile);
		QString text = _TIME_ + QString("[%1:%2:%3]").arg(context.file).arg(context.function).arg(context.line) + QString("[%1][%2]%3\n").arg(logStr[_log]).arg((qint64)QThread::currentThreadId()).arg(msg);
		ts << text;
		OutputDebugString(text.toStdWString().c_str());
	}
}

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
	// 程序初始化
	QmPreload qmInit;
	if (!qmInit.init(argc, argv)) {
		return false;
	}
    return a.exec();
}
