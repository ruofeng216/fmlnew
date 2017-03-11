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
	mainlayout->setContentsMargins(10, 0, 0, 0);

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
	mainlayout->setContentsMargins(10, 0, 0, 0);

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
	QSpacerItem *horizontalSpacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Maximum);
	mainlayout->addItem(horizontalSpacer);
	m_Search = new QLineEdit(this);
	mainlayout->addWidget(m_Search);
	m_Search->setPlaceholderText(tr("Navigation function"));
	m_Search->setAlignment(Qt::AlignHCenter);

	this->setLayout(mainlayout);
}