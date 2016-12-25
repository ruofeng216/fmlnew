#include "crash_hook.h"
#include <Windows.h>
#include <string.h>
#include "util.h"
#include <DbgHelp.h>
#include <sstream>
#include <QDateTime>

#pragma comment(lib, "DbgHelp.lib")

static ApplicationCrashEvents *s_notifier = 0;
static const int s_pathLen = 1024;
LONG WINAPI topLevelExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo);

CrashHook::CrashHook(ApplicationCrashEvents *notifier)
{
    s_notifier = notifier;
    ::SetUnhandledExceptionFilter(topLevelExceptionFilter);
}

CrashHook::~CrashHook()
{
}

LONG WINAPI topLevelExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo)
{
    LONG retValue = EXCEPTION_CONTINUE_SEARCH;
	std::string buffer;
	buffer.resize(s_pathLen + 1, '\0');
	::GetModuleFileNameA(NULL, &buffer[0], s_pathLen);
	std::string dir = buffer.substr(0, buffer.find_last_of("\\"));

	char path[s_pathLen + 1] = {0};
	sprintf_s(path, "%s\\crash%s.dmp", dir.c_str(), qutil::toString(QDateTime::currentDateTime().toString("yyyyMMddHHmmss")).c_str());
	
    HANDLE hFile = ::CreateFileA(path,
                                GENERIC_WRITE,
                                FILE_SHARE_WRITE,
                                0,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                0);
    if (hFile == INVALID_HANDLE_VALUE) {
		std::wstringstream wss;
		wss << L"create dump error, last error:" << GetLastError();
		MessageBox(NULL, wss.str().c_str(), L"warning", MB_OK);
        return retValue;
    }

    _MINIDUMP_EXCEPTION_INFORMATION exInfo;
    exInfo.ThreadId = ::GetCurrentThreadId();
    exInfo.ExceptionPointers = pExceptionInfo;
    exInfo.ClientPointers = 0;
    BOOL result = MiniDumpWriteDump(GetCurrentProcess(),
                                    GetCurrentProcessId(),
                                    hFile,
                                    MiniDumpNormal,
                                    &exInfo,
                                    0,
                                    0);
    if (result == 0) {
		std::wstringstream wss;
		wss << L"write dump error, last error:" << GetLastError();
		MessageBox(NULL, wss.str().c_str(), L"warning", MB_OK);
        return retValue;
    }

    retValue = EXCEPTION_EXECUTE_HANDLER;
    if (hFile != 0) {
        CloseHandle(hFile);
    }

	if (s_notifier) {
		s_notifier->onCrash(path);
	}

    return retValue;
}
