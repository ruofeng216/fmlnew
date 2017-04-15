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

	// ��ȡƤ��
	void initSkin(const QString &skinName);
	void refreshSkin();
	QString currentSkin();
	QString skin(const QString &name);
	QString websrc(const QString &name);
	// ��ȡFmlStyle
	QString fmlStyle(const QString &fmlName);
	QString fmlAttr(const QString &fmlName, const QString &attrName);
	void setFmlStyle(QWidget *widget, const QString &fmlName);

	// �ڴ�����
	void cacheclean();

	// ��point������Ļ��Χʱ����һ�������ֵ
	QPoint GracePoint(const QPoint &point, const QSize &size);

	void showWnd(HWND hwnd);
	bool isWndTopMost(HWND hwnd);
	void showWndTop(HWND hwnd);
	void showWndTopMost(HWND hwnd);
	void cancelTopMost(HWND hwnd);
	void startAlert(HWND hwnd, int durationMs, int timeoutMs);
	void stopAlert(HWND hwnd);

	// �����ʼ���־
	//void SendMailLog();

	// ��ȡ�����ļ�Ŀ¼
	const QString qBaseConfigPath();
	const STRINGUTF8 baseConfigPath();
	// ��ȡExceptionHandlerӦ��·��
	STRINGUTF8 getExceptionHandlerPath();
	// ��־·��
	QString logDir();
	// ��ȡ�洢������Ϣ
	const QString qBaseSettingPath();
	// ��ȡƤ������·��
	QString getSkinConfigPath();

	// ����ת��
	std::wstring ansi2unicode(const STRINGANSI& ansi);
	STRINGANSI unicode2ansi(const std::wstring& unicode);

	STRINGUTF8 string_to_utf8(const STRINGANSI& str);
	STRINGANSI utf8_to_string(const STRINGUTF8& str);

	std::wstring UTF8ToWString(const STRINGUTF8& str);
	STRINGUTF8 WStringToUTF8(const std::wstring& str);
	std::string getWindowsLastErrString();
	std::string getFormatString(const char *fmt, ...);


	// ��ȡxml��Dom��
	const QDomDocument GetXmlDom(const QString &path);
	// tooltip����
	QString splitTooltip(QString text, int lineWidth);

	// ����������ĩ��ɫ
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
