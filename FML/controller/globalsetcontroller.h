#pragma once
#include "inc\controller_interface.h"
#include <QDomElement>

class GlobalSetController : public IGlobalSetting
{

public:
	GlobalSetController();
	~GlobalSetController();

	// 获取功能根节点菜单
	void getRootFunc(QStringList &rootVal);
	// 获取功能子节点菜单
	void getChildrenFunc(const QString &funcid, QStringList &chVal);
	// 获取功能父节点菜单
	void getParentFunc(const QString &funcid, QString &parval);
	// 获取功能info菜单
	void getFuncInfo(const QString &funcid, CFuncInfo &funcinfo);
	bool getFuncInfoFromName(const QString &name, CFuncInfo &info);

	// 是否存在功能
	bool isExistFuncID(const QString &funcid);

	// 初始化
	void init();

private:
	// 初始化功能菜单
	void initFunc();
	void getFunc(const QDomElement &el);
	// 判断是否为子一级
	bool IsChild(const QString &parentid, const QString &thisid);

private:
	QMap<QString, CFuncInfo> m_AllFunc;
	QMap<QString, QStringList> m_FuncLevel;
};
