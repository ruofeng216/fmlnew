#pragma once

#include <QtDebug>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker> 
#include <QSettings>
#include <QDate>
#include <QThread>
#include "util/util.h"

enum LogLevel
{
	LL_TRACE,
	LL_DEBUG,
	LL_INFO,
	LL_WARN,
	LL_ERROR,
	LL_FATAL,
	LOGLEVEL_COUNT
};

#define _TIME_ qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))

static struct LogWrap {
	LogWrap::LogWrap() { reset(); }
	LogWrap::~LogWrap() { reset(); }
	void reset() {
		isInit = false;
		level = "";
		date = "";
		if (ts) {
			delete ts;
			ts = nullptr;
		}
		if (file) {
			delete file;
			file = nullptr;
		}
	}
	bool isInit;
	QMutex locker;
	QString level;
	QString date;
	QFile *file;
	QTextStream *ts;
} s_logWrap;
static QStringList logStr = QStringList() << "TRACE" << "DEBUG" << "INFO" << "WARN" << "ERROR" << "FATAL";

//回调函数实现debug信息到文件
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	LogLevel _log;
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

	QString curDate = QDate::currentDate().toString("yyyyMMdd");
	if (curDate != s_logWrap.date) {
		s_logWrap.reset();
	}

	if (!s_logWrap.isInit) {
		s_logWrap.isInit = true;
		QDir().mkpath(qutil::logDir());
		QSettings configIniRead(qutil::qBaseConfigPath(), QSettings::IniFormat);
		s_logWrap.level = configIniRead.value("/Base/loglevel").toString().toUpper();
		s_logWrap.date = QDate::currentDate().toString("yyyyMMdd");
		s_logWrap.file = new QFile(qutil::logDir() + QString("\\%1.log").arg(s_logWrap.date));
		if (s_logWrap.file->open(QIODevice::WriteOnly | QIODevice::Append)) {
			s_logWrap.ts = new QTextStream(s_logWrap.file);
		}
	}

	if (s_logWrap.ts) {
		QString text = _TIME_ + QString("[%1:%2:%3]").arg(context.file).arg(context.function).arg(context.line) + QString("[%1][%2]%3\n").arg(logStr[_log]).arg((qint64)QThread::currentThreadId()).arg(msg);
		OutputDebugString(text.toStdWString().c_str());
		QMutexLocker locker(&s_logWrap.locker);
		(*s_logWrap.ts << text).flush();
	}
}
