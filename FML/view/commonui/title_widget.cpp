#include "title_widget.h"
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpacerItem>
#include <QMenu>
#include <QAction>
#include "view/view_controller.h"
#include "qcontrollermanager.h"

TitleWidget::TitleWidget(QWidget *parent)
	: QWidget(parent)
	, m_logo(NULL)
	, m_title(NULL)
{
	this->setObjectName("titlewidget");
	// 背景色自动填充
	//setAutoFillBackground(true);
	//可获取鼠标跟踪效果
	//setMouseTracking(true);
	//setAttribute(Qt::WA_TranslucentBackground, true);
}

TitleWidget::~TitleWidget()
{

}

void TitleWidget::init(const QPixmap &logoPixmap, const QString &title, bool isCenter)
{
	QHBoxLayout *mainlayout = new QHBoxLayout;
	mainlayout->setContentsMargins(0, 0, 0, 0);

	if (!logoPixmap.isNull())
	{
		m_logo = new QLabel(this);
		m_logo->setPixmap(logoPixmap);
		m_logo->setFixedSize(logoPixmap.size());
		mainlayout->addWidget(m_logo);
	}
	if (!title.isEmpty()) {
		m_title = new QLabel(this);
		m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_title->setText(title);
		if (isCenter)
		{
			m_title->setAlignment(Qt::AlignHCenter);
		}
		mainlayout->addWidget(m_title, 1);
	}
	this->setLayout(mainlayout);
}


/////////////////////////////////////////////////////////////
MainTitle::MainTitle(QWidget *parent)
	: QWidget(parent)
	, m_logo(NULL)
	, m_title(NULL)
	, m_book(NULL)
	, m_Search(NULL)
{
	this->setObjectName("MainTitle");
}
MainTitle::~MainTitle()
{

}
void MainTitle::init(const QPixmap &logoPixmap, const QString &title)
{
	QHBoxLayout *mainlayout = new QHBoxLayout;
	mainlayout->setContentsMargins(0, 0, 0, 0);

	if (!logoPixmap.isNull())
	{
		m_logo = new QLabel(this);
		m_logo->setPixmap(logoPixmap);
		m_logo->setFixedSize(logoPixmap.size());
		mainlayout->addWidget(m_logo);
	}
	if (!title.isEmpty()) {
		m_title = new QLabel(this);
		m_title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		m_title->setText(title);
		mainlayout->addWidget(m_title);
	}
	m_book = new QPushButton(this);
	m_book->setContentsMargins(2, 1, 2, 1);
	m_book->setText(tr("SysBook"));
	connect(m_book, SIGNAL(clicked()), this, SLOT(slotBook()));
	mainlayout->addWidget(m_book);

	m_Search = new QLineEdit(this);
	mainlayout->addWidget(m_Search);
	m_Search->setPlaceholderText(tr("Navigation function"));
	m_Search->setAlignment(Qt::AlignHCenter);

	QSpacerItem *horizontalSpacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Maximum);
	mainlayout->addItem(horizontalSpacer);

	this->setLayout(mainlayout);
}

void MainTitle::slotBook()
{
	// 获取所有功能的根节点
	QStringList rootlst;
	GLBSETCTL->getRootFunc(rootlst);
	if (!rootlst.isEmpty())
	{
		QMenu m;
		for (int i = 0; i < rootlst.size(); i++)
		{
			setMenu(&m, rootlst[i]);
		}
		m.exec(m_book->mapToGlobal(QPoint(0, m_book->rect().height())));
	}
}

void MainTitle::setMenu(QMenu *menu, const QString funcid)
{
	// 获取当前funcinfo
	CFuncInfo cf;
	GLBSETCTL->getFuncInfo(funcid, cf);
	// 获取子节点
	QStringList childlst;
	GLBSETCTL->getChildrenFunc(funcid, childlst);
	if (childlst.isEmpty())
	{
		QAction *action = new QAction(cf.getFuncName().getVal().toString(), NULL);
		//action->setShortcut(tr("CTRL+B"));
		//action->setCheckable(true);
		connect(action, &QAction::triggered, [this, funcid]{
			emit ViewController::instance()->sigGotoFunc(funcid);
		});
		menu->addAction(action);
	}
	else
	{
		QMenu *childmenu = new QMenu(NULL);
		menu->addMenu(childmenu)->setText(cf.getFuncName().getVal().toString());
		for (int j = 0; j < childlst.size(); j++)
		{
			setMenu(childmenu, childlst[j]);
		}
	}
}