#pragma once
#include <QString>
#include <qt_windows.h>
#include <QPoint>
#include <QSize>
#include <QVariantMap>
#include "pro.h"
#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateEdit>

namespace qutil
{
	std::string toString(const QString &str);
	QString toQString(const std::string &str);

	// 获取皮肤
	void initSkin(const QString &skinName);
	void refreshSkin();
	QString currentSkin();
	QString skin(const QString &name);
	QString websrc(const QString &name);
	// 获取FmlStyle
	QString fmlStyle(const QString &fmlName);
	QString fmlAttr(const QString &fmlName, const QString &attrName);
	void setFmlStyle(QWidget *widget, const QString &fmlName);

	// 内存整理
	void cacheclean();

	// 当point超出屏幕范围时，给一个合理的值
	QPoint GracePoint(const QPoint &point, const QSize &size);

	void showWnd(HWND hwnd);
	bool isWndTopMost(HWND hwnd);
	void showWndTop(HWND hwnd);
	void showWndTopMost(HWND hwnd);
	void cancelTopMost(HWND hwnd);
	void startAlert(HWND hwnd, int durationMs, int timeoutMs);
	void stopAlert(HWND hwnd);

	// 发送邮件日志
	//void SendMailLog();

	// 获取配置文件目录
	const QString qBaseConfigPath();
	const STRINGUTF8 baseConfigPath();
	// 获取ExceptionHandler应用路径
	STRINGUTF8 getExceptionHandlerPath();
	// 日志路径
	QString logDir();
	// 获取存储配置信息
	const QString qBaseSettingPath();
	// 获取皮肤配置路径
	QString getSkinConfigPath();

	// 编码转换
	std::wstring ansi2unicode(const STRINGANSI& ansi);
	STRINGANSI unicode2ansi(const std::wstring& unicode);

	STRINGUTF8 string_to_utf8(const STRINGANSI& str);
	STRINGANSI utf8_to_string(const STRINGUTF8& str);

	std::wstring UTF8ToWString(const STRINGUTF8& str);
	STRINGUTF8 WStringToUTF8(const std::wstring& str);
	std::string getWindowsLastErrString();
	std::string getFormatString(const char *fmt, ...);


	// 获取xml的Dom树
	const QDomDocument GetXmlDom(const QString &path);
	// tooltip换行
	QString splitTooltip(QString text, int lineWidth);

	// 设置日期周末颜色
	void setWeekendStyle(QDateEdit *widget);
}



namespace json
{
	QVariantMap toMap(const QByteArray &val);
	QString toString(const QVariantMap &val);
}

namespace config
{
	QVariant value(const QString &section, const QString &key);
	void setValue(const QString &section, const QString &key, const QString &value);
}
