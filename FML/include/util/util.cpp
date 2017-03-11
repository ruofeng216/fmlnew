#include "util.h"
#include <QRect>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>
#include <QToolTip>
#include "skin.h"

namespace qutil
{
	std::string toString(const QString &str)
	{
		return str.toStdString();
	}
	QString toQString(const std::string &str)
	{
		return QString::fromStdString(str);
	}

	void initSkin(const QString &skinName)
	{
		Skin::instance().setSkinName(skinName);
	}

	QString currentSkin()
	{
		return Skin::instance().skinName();
	}

	// 获取皮肤
	QString skin(const QString &name)
	{
		return Skin::instance().path(name);
		// later get skin set, now default datas
		//QSettings configIniRead(qBaseConfigPath(), QSettings::IniFormat);
		//QString strskin = configIniRead.value("/skin/curskin").toString().toLower();
		//QString strtype = "dark";
		//if (strskin == "dark" ||
		//	strskin == "light")
		//	strtype = strskin;
		//return QString(":/%1/%2").arg(strtype).arg(name);
	}
	QString websrc(const QString &name)
	{
		return Skin::instance().path("web/" + name);
		//QSettings configIniRead(qBaseConfigPath(), QSettings::IniFormat);
		//QString strskin = configIniRead.value("/skin/curskin").toString().toLower();
		//QString strtype = "dark";
		//if (strskin == "dark" ||
		//	strskin == "light")
		//	strtype = strskin;
		//return QString(":/%1/web/%2").arg(strtype).arg(name);
	}

	// 内存整理
	void cacheclean()
	{
		::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
	}

	QPoint GracePoint(const QPoint &point, const QSize &size)
	{
		auto gracePointFunc = [](const QPoint &point, const QSize &size, const QRect &mainRect) -> QPoint {
			QPoint result = point;
			if (point.x() < mainRect.x()) {
				result.setX(mainRect.x());
			}
			else if (point.x() + size.width() > mainRect.width()) {
				result.setX(mainRect.width() - size.width());
			}

			if (point.y() < mainRect.y()) {
				result.setY(mainRect.y());
			}
			else if (point.y() + size.height() > mainRect.height()) {
				result.setY(mainRect.height() - size.height());
			}
			return result;
		};

		QPoint result = point;
		QDesktopWidget *desktop = QApplication::desktop();
		QRect mainRect = desktop->screenGeometry(desktop->primaryScreen());
		if (desktop->screenCount() == 1) {
			result = gracePointFunc(point, size, mainRect);
		}
		else if (desktop->screenCount() == 2) {
			QRect extendRect = desktop->screenGeometry(desktop->primaryScreen() == 0 ? 1 : 0);
			if (extendRect.x() >= 0 && extendRect.y() >= 0) {
				result = gracePointFunc(point, size, desktop->rect());
			}
		}
		else {
			// no handle
		}
		return result;
	}

	void showWnd(HWND hwnd){
		ShowWindow(hwnd, SW_RESTORE);
	}
	void showWndTop(HWND hWnd){
		if (!::IsWindow(hWnd))
			return;
		::SetForegroundWindow(hWnd);
	}
	void showWndTopMost(HWND hwnd){
		RECT rect;
		GetWindowRect(hwnd, &rect);
		SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, abs(rect.right - rect.left), abs(rect.bottom - rect.top), SWP_SHOWWINDOW);
	}
	void cancelTopMost(HWND hwnd){
		RECT rect;
		GetWindowRect(hwnd, &rect);
		SetWindowPos(hwnd, HWND_NOTOPMOST, rect.left, rect.top, abs(rect.right - rect.left), abs(rect.bottom - rect.top), SWP_SHOWWINDOW);
	}
	void startAlert(HWND hwnd, int durationMs, int timeoutMs) {
		FLASHWINFO info;
		info.cbSize = sizeof(info);
		info.hwnd = hwnd;
		info.dwFlags = FLASHW_TRAY;
		info.dwTimeout = timeoutMs;
		info.uCount = durationMs == 0 ? 10 : durationMs / timeoutMs;
		FlashWindowEx(&info);
	}
	void stopAlert(HWND hwnd) {
		FLASHWINFO info;
		info.cbSize = sizeof(info);
		info.hwnd = hwnd;
		info.dwFlags = FLASHW_STOP;
		info.dwTimeout = 0;
		info.uCount = 0;
		FlashWindowEx(&info);
	}

	//void SendMailLog()
	//{
	//	const std::string MAIL_SECTION = "Mail";
	//	const std::string RECV_MAIL_ADDRESS = "RecvMailAddress";
	//	const std::string SEND_MAIL_ADDRESS = "SendMailAddress";
	//	const std::string SEND_MAIL_PASSWORD = "SendMailPassword";
	//	const std::string RECV_MAIL_ADDRESS_DEFAULT = "qm_report@sumscope.com";
	//	const std::string SEND_MAIL_ADDRESS_DEFAULT = "qm_report@sumscope.com";
	//	const std::string SEND_MAIL_PASSWORD_DEFAULT = "Abc123"; // 使用base64编解码

	//	ConfigIni mailCfg(baseConfigPath(), MAIL_SECTION);
	//	QString recvMailAddress = qutil::toQString(mailCfg.getString(RECV_MAIL_ADDRESS));
	//	if (recvMailAddress.isEmpty()) {
	//		recvMailAddress = qutil::toQString(RECV_MAIL_ADDRESS_DEFAULT);
	//		mailCfg.setString(RECV_MAIL_ADDRESS, RECV_MAIL_ADDRESS_DEFAULT);
	//	}
	//	QString sendMailAddress = qutil::toQString(mailCfg.getString(SEND_MAIL_ADDRESS));
	//	if (sendMailAddress.isEmpty()) {
	//		sendMailAddress = qutil::toQString(SEND_MAIL_ADDRESS_DEFAULT);
	//		mailCfg.setString(SEND_MAIL_ADDRESS, SEND_MAIL_ADDRESS_DEFAULT);
	//	}
	//	QString sendMailPassword = qutil::toQString(mailCfg.getString(SEND_MAIL_PASSWORD));
	//	if (sendMailPassword.isEmpty()) {
	//		sendMailPassword = qutil::toQString(SEND_MAIL_PASSWORD_DEFAULT);
	//		mailCfg.setString(SEND_MAIL_PASSWORD, qutil::toString(QString::fromLocal8Bit(sendMailPassword.toLocal8Bit().toBase64())));
	//	}
	//	else {
	//		sendMailPassword = QString::fromLocal8Bit(QByteArray::fromBase64(sendMailPassword.toLocal8Bit()));
	//	}

	//	QString appPath = toQString(getExceptionHandlerPath());
	//	QString appName = qutil::toQString(util::GetCurrentAppName());
	//	QString curVersion = qutil::toQString(AppInfo::curVersion());
	//	QString userName = qutil::toQString(ControllerManager::instance()->getPersonInstance()->GetCurrentUserName());
	//	if (userName.isEmpty()) {
	//		std::string strUsername, strPassword;
	//		ControllerManager::instance()->getGlobalInstance()->GetLastLoginInfo(strUsername, strPassword);
	//		userName = qutil::toQString(strUsername);
	//	}

	//	// 1：崩溃的应用程序名称，2：接收邮箱，3：发送邮箱，4：发送密码, 5：用户名
	//	QStringList strList;
	//	strList << appName;
	//	strList << recvMailAddress;
	//	strList << sendMailAddress;
	//	strList << sendMailPassword;
	//	strList << (userName + "_" + curVersion);
	//	for (auto iter = strList.begin(); iter != strList.end(); ++iter) {
	//		(*iter).remove(" ");
	//	}

	//	QProcess::startDetached(appPath, strList);
	//}

	// 获取配置文件目录
	const QString qBaseConfigPath(){
		return QCoreApplication::applicationDirPath() + "/config/config.ini";
	}
	const STRINGUTF8 baseConfigPath(){
		return toString(qBaseConfigPath());
	}
	// 获取ExceptionHandler应用路径
	STRINGUTF8 getExceptionHandlerPath(){
		return toString(QCoreApplication::applicationDirPath() + "/ExceptionHandler.exe");
	}
	// 日志路径
	QString logDir() {
		return QCoreApplication::applicationDirPath() + "/log";
	}
	// 获取存储配置信息
	const QString qBaseSettingPath()
	{
		return QCoreApplication::applicationDirPath() + "/config/settings.xml";
	}

	// 编码转换
	std::wstring ansi2unicode(const STRINGANSI& ansi){
		if (ansi.empty()) {
			return std::wstring(L"");
		}
		int len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, NULL, 0);
		std::wstring unicode(len + 1, L'\0');
		len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.size(), &unicode[0], len);
		return unicode;
	}
	STRINGANSI unicode2ansi(const std::wstring& unicode){
		if (unicode.empty()) {
			return std::string("");
		}
		int len = WideCharToMultiByte(CP_ACP, 0, unicode.c_str(), -1, NULL, 0, NULL, NULL);
		std::string ansi(len + 1, '\0');
		WideCharToMultiByte(CP_ACP, 0, unicode.c_str(), unicode.size(), &ansi[0], len, NULL, NULL);
		return ansi;
	}
	STRINGUTF8 string_to_utf8(const STRINGANSI& srcStr){
		if (srcStr.empty())
		{
			return "";
		}

		int nwLen = MultiByteToWideChar(CP_ACP, 0, srcStr.c_str(), -1, NULL, 0);
		wchar_t * pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		MultiByteToWideChar(CP_ACP, 0, srcStr.c_str(), srcStr.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char * pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	STRINGANSI utf8_to_string(const STRINGUTF8& srcStr){
		if (srcStr.empty())
		{
			return "";
		}

		int nwLen = MultiByteToWideChar(CP_UTF8, 0, srcStr.c_str(), -1, NULL, 0);
		wchar_t * pwBuf = new wchar_t[nwLen + 1];
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, srcStr.c_str(), srcStr.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char * pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr = pBuf;
		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return retStr;
	}
	std::wstring UTF8ToWString(const STRINGUTF8& str){
		int len = str.size();
		int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pUnicode;
		pUnicode = new wchar_t[unicodeLen + 1];
		memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);
		std::wstring wstrReturn(pUnicode);
		delete[] pUnicode;
		return wstrReturn;
	}
	STRINGUTF8 WStringToUTF8(const std::wstring& str){
		char* pElementText = NULL;
		int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)str.c_str(), -1, NULL, 0, NULL, NULL);
		pElementText = new char[iTextLen + 1];
		memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
		::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)str.c_str(), -1, pElementText, iTextLen, NULL, NULL);
		std::string strReturn(pElementText);
		delete[] pElementText;
		return strReturn;
	}
	std::string getWindowsLastErrString(){
		DWORD errCode = GetLastError();
		TCHAR buffer[1024];
		std::string errString;
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			(LPTSTR)&buffer[0],
			sizeof(buffer), NULL) == 0) {
			errString = getFormatString("Cannot get text error describing (%u)", errCode);
		}
		else {
			errString = getFormatString("%s (%u)", unicode2ansi(buffer).c_str(), errCode);
		}
		return errString;
	}
	std::string getFormatString(const char *fmt, ...){
		char buffer[1024] = { 0 };
		va_list vaList;
		va_start(vaList, fmt);
		vsnprintf_s(buffer, _countof(buffer), _TRUNCATE, fmt, vaList);
		va_end(vaList);
		return buffer;
	}

	// 获取xml的Dom树
	const QDomDocument GetXmlDom(const QString &path)
	{
		QDomDocument doc;
		QFile xmlFile(path);
		if (!xmlFile.open(QIODevice::ReadOnly)) {
			return doc;
		}
		if (!doc.setContent(&xmlFile)) {
			xmlFile.close();
			return doc;
		}
		xmlFile.close();
		return doc;
	}

	QString splitTooltip(QString text, int lineWidth)
	{
		QFontMetrics fm(QToolTip::font());
		QString result;

		for (;;) {
			int i = 0;
			while (i < text.length()) {
				if (fm.width(text.left(++i + 1)) > lineWidth) {
					int j = text.lastIndexOf(' ', i);
					if (j > 0) {
						i = j;
					}
					result += text.left(i);
					result += '\n';
					text = text.mid(i);
					break;
				}
			}
			if (i >= text.length()) {
				break;
			}
		}

		return result + text;
	}
}

namespace json
{
	QVariantMap toMap(const QByteArray &val)
	{
		QJsonParseError jError;
		QJsonDocument jDoc = QJsonDocument::fromJson(val, &jError);
		if (jError.error == QJsonParseError::NoError) {
			if (jDoc.isObject()) {
				QJsonObject jObj = jDoc.object();
				return jObj.toVariantMap();
			}
		}
		QVariantMap ret;
		return ret;
	}

	QString toString(const QVariantMap &val)
	{
		QJsonObject jobj = QJsonObject::fromVariantMap(val);
		QJsonDocument jdoc(jobj);
		return QString(jdoc.toJson(QJsonDocument::Compact));
	}
}