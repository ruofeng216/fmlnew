#pragma once

#include <QtDebug>



static int m_nlog = 0;
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

#define LOG_TRACE if (m_nlog <= LL_TRACE) \
	qDebug()
#define LOG_DEBUG if (m_nlog <= LL_DEBUG) \
	qDebug()
#define LOG_INFO if (m_nlog <= LL_INFO) \
	qInfo()
#define LOG_WARN if (m_nlog <= LL_WARN) \
	qWarning()
#define LOG_ERROR qCritical()
#define LOG_FATAL qFatal()