#include "fml.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include "view/view_controller.h"


FML::FML(QWidget *parent)
	: QMainWindow(parent)
	, m_showFunc(false)
	, m_pWeball(NULL)
	, m_pWebAdd(NULL)
{
	ui.setupUi(this);

	m_pWeball = new DemoWebview(this);
	m_pWebAdd = new DemoWebview1(this);

	ui.gridLayout_3->addWidget(m_pWeball);
	ui.gridLayout_4->addWidget(m_pWebAdd);

	connect(ViewController::instance(), &ViewController::pushDemoData, this, &FML::slotPushDemoData);

	m_pWeball->loadHtml(qutil::websrc("web/demo/1.html"));
	m_pWebAdd->loadHtml(qutil::websrc("web/demo/2.html"));

	init();
}

void FML::slotPushDemoData(const demoStruct &val)
{
	if (m_pWeball)
	{
		m_pWeball->pushDemoData(val);
	}
}

void FML::init()
{
	initMenuFunc();
}
void FML::initMenuFunc()
{
	// 获取所有功能的根节点
	QStringList rootlst;
	GLBSETCTL->getRootFunc(rootlst);
	if (!rootlst.isEmpty())
	{
		for (int i = 0; i < rootlst.size(); i++)
		{
			QString curfunid = rootlst[i];
			CFuncInfo cf;
			GLBSETCTL->getFuncInfo(curfunid, cf);
			QStringList childlst;
			GLBSETCTL->getChildrenFunc(curfunid, childlst);
			if (childlst.isEmpty())
			{
				QAction *action = new QAction(cf.getFuncName().getVal().toString(), NULL);
				//action->setShortcut(tr("CTRL+B"));
				//action->setCheckable(true);
				connect(action, &QAction::triggered, [this, curfunid] {
					emit ViewController::instance()->sigGotoFunc(curfunid);
				});
				ui.menuBar->addAction(action);
			}
			else
			{
				QMenu *childmenu = new QMenu(this);
				ui.menuBar->addMenu(childmenu)->setText(cf.getFuncName().getVal().toString());
				setMenu(childmenu, curfunid);
			}
		}
	}
}

void FML::setMenu(QMenu *menu, const QString funcid)
{
	// 获取子节点
	QStringList childlst;
	GLBSETCTL->getChildrenFunc(funcid, childlst);
	for (int j = 0; j < childlst.size(); j++)
	{
		QString curfunid = childlst[j];
		CFuncInfo cf;
		GLBSETCTL->getFuncInfo(curfunid, cf);
		QStringList tchildlst;
		GLBSETCTL->getChildrenFunc(curfunid, tchildlst);
		if (tchildlst.isEmpty())
		{
			QAction *action = new QAction(cf.getFuncName().getVal().toString(), NULL);
			//action->setShortcut(tr("CTRL+B"));
			//action->setCheckable(true);
			connect(action, &QAction::triggered, [this, curfunid] {
				emit ViewController::instance()->sigGotoFunc(curfunid);
			});
			menu->addAction(action);
		}
		else
		{
			QMenu *childmenu = new QMenu(this);
			menu->addMenu(childmenu)->setText(cf.getFuncName().getVal().toString());
			setMenu(childmenu, curfunid);
		}
	}
}