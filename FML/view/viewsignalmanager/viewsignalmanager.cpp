#include "viewsignalmanager.h"

ViewSignalManager::ViewSignalManager(QObject *parent)
	: QObject(parent)
{
	qRegisterMetaType<CMyBasePtr>("CMyBasePtr");
}

ViewSignalManager::~ViewSignalManager()
{
}

ViewSignalManager* ViewSignalManager::instance()
{
	static ViewSignalManager inst;
	return &inst;
}