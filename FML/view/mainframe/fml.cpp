#include "fml.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include "view/view_controller.h"

FML::FML(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.tabWidget, &DragInOutTabWidget::popSignalWnd, this, &FML::slotPopSignalWnd);
	connect(ui.tabWidget, &DragInOutTabWidget::tabCloseRequested, [this](int nIndex) {
		ui.tabWidget->removeTab(nIndex);
	});

	init();
}

QTabWidget *FML::getMainTab()
{
	return (QTabWidget *)ui.tabWidget;
}

void FML::slotPopSignalWnd(int nIndex)
{
	QString id = ui.tabWidget->currentWidget()->property("subwndid").toString();
	ViewController::instance()->popOutWndFromTab(id, ui.tabWidget->currentWidget(), QCursor::pos());
	qDebug() << qMax(0, qMin(nIndex, ui.tabWidget->count() - 1));
	ui.tabWidget->setCurrentIndex(qMax(0,qMin(nIndex-1, ui.tabWidget->count()-1)));
	update();
}

void FML::init()
{
	initMenuFunc();
}
void FML::initMenuFunc()
{
	// ��ȡ���й��ܵĸ��ڵ�
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
	// ��ȡ�ӽڵ�
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