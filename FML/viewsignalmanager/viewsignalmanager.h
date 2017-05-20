#pragma once

#include <QObject>
#include "util\datatype.h"

#define VIEWSIGNAL ViewSignalManager::instance()

class ViewSignalManager : public QObject
{
	Q_OBJECT

public:
	ViewSignalManager(QObject *parent = NULL);
	~ViewSignalManager();

	static ViewSignalManager* instance();

signals:
	// 程序退出
	void sigExitProgramme(bool b = true);
	// 处理返回/推送
	void callBackUI(const CMyBasePtr val);

	////////////////////////////////////////
	// 产品变更
	void sigProductChange();
	// 参数字典变更
	void sigParameterChange(const QString &paraCode);
};
