#include "fml.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include "view/view_controller.h"
#include "main_home_page.h"
#include <QPushButton>
#include <QShortcut>

FML::FML(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.tabWidget, &DragInOutTabWidget::popSignalWnd, this, &FML::slotPopSignalWnd, Qt::QueuedConnection);
	connect(ui.tabWidget, &DragInOutTabWidget::tabCloseRequested, [this](int nIndex) {
		if (ui.tabWidget->widget(nIndex)->property("subwndid").toString() == Main_HomePage) return;
		ui.tabWidget->removeTab(nIndex);
	});
	connect(ui.tabWidget, &DragInOutTabWidget::tabBarDoubleClicked, this, &FML::slotPopSignalWndDBClk, Qt::QueuedConnection);
	QShortcut *pHome = new QShortcut(QKeySequence(Qt::Key_Home), this);
	connect(pHome, &QShortcut::activated, [this] {
		emit ViewController::instance()->sigGotoFunc(Main_HomePage);
	});
}

FML::~FML()
{
	saveWidget();
}

QTabWidget *FML::getMainTab()
{
	return (QTabWidget *)ui.tabWidget;
}

void FML::slotPopSignalWnd(int nIndex)
{
	QString id = ui.tabWidget->currentWidget()->property("subwndid").toString();
	if (id == Main_HomePage) return;
	ViewController::instance()->popOutWndFromTab(id, ui.tabWidget->currentWidget(), QCursor::pos());
	ui.tabWidget->setCurrentIndex(qMax(0,qMin(nIndex-1, ui.tabWidget->count()-1)));
}
void FML::slotPopSignalWndDBClk(int nIndex)
{
	QString id = ui.tabWidget->currentWidget()->property("subwndid").toString();
	if (id == Main_HomePage) return;
	ViewController::instance()->popOutWndFromTab(id, ui.tabWidget->currentWidget(), 
		QPoint(QCursor::pos().x(), QCursor::pos().y()+ ui.tabWidget->tabBar()->height() * 2));
	ui.tabWidget->setCurrentIndex(qMax(0, qMin(nIndex - 1, ui.tabWidget->count() - 1)));
}

void FML::init()
{
	initMenuFunc();
	initWidget();
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

void FML::initWidget()
{
	// 添加主页
	{
		ui.tabWidget->addTab(new MainHomePage(), tr("homepage"));
	}
	struct winds {
		QString winname;
		int index;
		bool bcurrent;
	};
	QVector<winds> windlst;
	// 初始化所有历史打开页面
	QDomDocument doc = qutil::GetXmlDom(qutil::qBaseSettingPath());
	if (doc.isNull())
	{
		qDebug() << "wrong settings!";
		return;
	}

	// 解析
	QDomElement root = doc.documentElement();
	QDomNodeList nodes = root.elementsByTagName("openwidgets");
	for (int i = 0; i < nodes.size(); i++)
	{
		QDomElement itemNode = nodes.at(i).toElement();
		QDomNodeList nodelst = itemNode.elementsByTagName("widget");
		bool bFindHome = false;
		for (int j = 0; j < nodelst.size(); j++)
		{
			QDomElement pitemNode = nodelst.at(j).toElement();
			winds s;
			s.winname = pitemNode.attribute("wname");
			s.index = pitemNode.attribute("index").toInt();
			s.bcurrent = bool(pitemNode.attribute("current").toInt());
			windlst.push_back(s);
		}
		break;
	}
	qSort(windlst.begin(), windlst.end(), [](winds l, winds r) ->bool{
		return l.index < r.index;
	});
	int nCurrentIndex = -1;
	for (int i = 0; i < windlst.size(); i++)
	{
		emit ViewController::instance()->sigGotoFunc(windlst[i].winname);
		if (nCurrentIndex == -1 || windlst[i].bcurrent)
		{
			nCurrentIndex = ui.tabWidget->currentIndex();
		}
	}
	ui.tabWidget->setCurrentIndex(nCurrentIndex);
}

void FML::saveWidget()
{
	// 更新配置文件
	QDomDocument doc;
	QDomProcessingInstruction instruction;
	instruction = doc.createProcessingInstruction("xml", "version='1.0' encoding='utf-8'");
	doc.appendChild(instruction);
	QDomElement root = doc.createElement("configuration");
	doc.appendChild(root);
	QDomElement element_root = doc.createElement("openwidgets");
	for (int i = 0; i < ui.tabWidget->count(); i++)
	{
		QDomElement element_rmt = doc.createElement("widget");
		QDomAttr attr_index = doc.createAttribute("index");
		attr_index.setValue(QString::number(i));
		element_rmt.setAttributeNode(attr_index);
		QDomAttr attr_wname = doc.createAttribute("wname");
		attr_wname.setValue(ui.tabWidget->widget(i)->property("subwndid").toString());
		element_rmt.setAttributeNode(attr_wname);
		QDomAttr attr_current = doc.createAttribute("current");
		attr_current.setValue(QString::number(i == ui.tabWidget->currentIndex()));
		element_rmt.setAttributeNode(attr_current);
		element_root.appendChild(element_rmt);
	}
	root.appendChild(element_root);

	// 写文件
	QFile file(qutil::qBaseSettingPath());
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		doc.save(out, 4);
		file.close();
	}
}