#ifndef QMPRELOAD_H
#define QMPRELOAD_H

#include <QObject>
#include "util/crash_hook.h"

class CBootParam;
class QmPreload : public QObject
{
	Q_OBJECT

public:
	QmPreload(QObject *parent = NULL);
	~QmPreload();
	// ≥ı ºªØ
	bool init(int argc, char *argv[]);
};

class QmCrashEvents : public ApplicationCrashEvents
{
public:
	void onCrash(const std::string &dumpPath);
};

#endif // QMPRELOAD_H
