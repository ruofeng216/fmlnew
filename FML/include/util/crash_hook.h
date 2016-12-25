#ifndef CRASHHOOK_H
#define CRASHHOOK_H

#include <string>

class ApplicationCrashEvents
{
public:
    virtual void onCrash(const std::string &dumpPath) = 0;
};

class CrashHook
{
public:
    explicit CrashHook(ApplicationCrashEvents *notifier = NULL);
    virtual ~CrashHook();
};

#endif // CRASHHOOK_H
