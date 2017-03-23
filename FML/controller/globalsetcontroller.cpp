#include "globalsetcontroller.h"
#include "util/util.h"
#include <QDebug>

GlobalSetController::GlobalSetController()
{
	init();
}

GlobalSetController::~GlobalSetController()
{
}

// 获取根节点
void GlobalSetController::getRootFunc(QStringList &rootVal)
{
	if (m_FuncLevel.contains("root"))
		rootVal = m_FuncLevel["root"];
}
// 获取子节点
void GlobalSetController::getChildrenFunc(const QString &funcid, QStringList &chVal)
{
	if (m_FuncLevel.contains(funcid))
		chVal = m_FuncLevel[funcid];
}
// 获取父节点
void GlobalSetController::getParentFunc(const QString &funcid, QString &parval)
{
	if (funcid == "root")
		return;
	for (QMap<QString, QStringList>::iterator itor = m_FuncLevel.begin();
		itor != m_FuncLevel.end(); itor++)
	{
		if (itor.value().contains(funcid))
		{
			parval = itor.key();
			break;
		}
	}
}
// 获取功能info
void GlobalSetController::getFuncInfo(const QString &funcid, CFuncInfo &funcinfo)
{
	if (m_AllFunc.contains(funcid))
		funcinfo = m_AllFunc[funcid];
}

bool GlobalSetController::getFuncInfoFromName(const QString &name, CFuncInfo &info)
{
	auto itor = m_AllFunc.constBegin();
	while (itor != m_AllFunc.constEnd()) {
		const CFuncInfo &curInfo = itor.value();
		if (name == curInfo.getFuncName().getVal().toString()) {
			info = curInfo;
			return true;
		}
		++itor;
	}
	return false;
}

// 是否存在功能
bool GlobalSetController::isExistFuncID(const QString &funcid)
{
	return m_AllFunc.contains(funcid);
}

// 初始化
void GlobalSetController::init()
{
	// 功能菜单
	initFunc();
}

// 初始化功能菜单
void GlobalSetController::initFunc()
{
	QDomDocument doc = qutil::GetXmlDom(qutil::skin("func.xml"));
	if (doc.isNull())
	{
		qDebug() << "wrong function menu settings!";
		return;
	}

	// 解析
	QDomElement root = doc.documentElement();
	QDomNodeList nodes = root.elementsByTagName("AllFunctions");
	for (int i = 0; i < nodes.size(); i++)
	{
		QDomElement itemNode = nodes.at(i).toElement();
		getFunc(itemNode);
	}
}

void GlobalSetController::getFunc(const QDomElement &el)
{
	QString parentID = el.attribute("funcid");
	if (!m_AllFunc.contains(parentID) && !parentID.isEmpty())
	{
		QString funcname = el.attribute("funcname");
		QString funcdesc = el.attribute("funcdesc");
		QString order = el.attribute("order");
		CFuncInfo cf;
		cf.setFuncID(CMyField(parentID));
		cf.setFuncName(CMyField(funcname));
		cf.setFuncDesc(CMyField(funcdesc));
		cf.setOrder(CMyField(order));
		m_AllFunc[parentID] = cf;
	}

	QDomNodeList nodes = el.elementsByTagName("function");
	for (int i = 0; i < nodes.size(); i++)
	{
		// save cache
		QDomElement itemNode = nodes.at(i).toElement();
		QString funcid = itemNode.attribute("funcid");
		if (!m_AllFunc.contains(funcid))
		{
			QString funcname = itemNode.attribute("funcname");
			QString funcdesc = itemNode.attribute("funcdesc");
			QString order = itemNode.attribute("order");
			CFuncInfo cf;
			cf.setFuncID(CMyField(funcid));
			cf.setFuncName(CMyField(funcname));
			cf.setFuncDesc(CMyField(funcdesc));
			cf.setOrder(CMyField(order));
			m_AllFunc[funcid] = cf;
		}
		if (IsChild(parentID, funcid))
			m_FuncLevel[parentID] << funcid;
		getFunc(itemNode);
	}
}

// 判断是否为子一级
bool GlobalSetController::IsChild(const QString &parentid, const QString &thisid)
{
	if (thisid == "root")
		return false;
	CFuncInfo cParent, cinfo;
	getFuncInfo(parentid, cParent);
	getFuncInfo(thisid, cinfo);
	QString pid;
	getParentFunc(thisid, pid);

	long pLevel = cParent.getOrder().getVal().toString().left(cParent.getOrder().getVal().toString().length() - 5).toLong();
	long cLevel = cinfo.getOrder().getVal().toString().left(cinfo.getOrder().getVal().toString().length() - 5).toLong();
	if (pid.isEmpty() && pLevel + 1 == cLevel)
		return true;
	return false;
}