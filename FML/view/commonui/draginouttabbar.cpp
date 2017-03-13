#include "draginouttabbar.h"
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QMouseEvent>
#include <QTabWidget>
#include <QPushButton>
#include <QDebug>
#include "util/pro.h"
#define MOVING_PIXMAP_PERSENT 0.5

DragInOutTabBar::DragInOutTabBar(QWidget *parent)
	: QTabBar(parent)
{
	setUsesScrollButtons(true);
	setTabsClosable(true);
	setMovable(true);
	setShape(QTabBar::TriangularNorth);
	setElideMode(Qt::ElideRight);
	setExpanding(true);

	m_HaveDraged = false;
	m_MoveIndex = -1;

	m_MovingWidget = new QWidget;
	m_MovingWidget->setObjectName("MovingWidget");
	m_MovingPic = new QLabel(m_MovingWidget);
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(m_MovingPic);
	m_MovingWidget->setLayout(layout);
	m_MovingWidget->setWindowFlags(Qt::FramelessWindowHint);
	m_MovingWidget->hide();
}

DragInOutTabBar::~DragInOutTabBar()
{
	if (m_MovingWidget)
		m_MovingWidget->deleteLater();
}

void DragInOutTabBar::mousePressEvent(QMouseEvent * event)
{
	QList<QRect> lstRange;
	for (int i = 0; i < count(); i++)
		lstRange << tabRect(i);
	auto isInRect = [this, event](const QList<QRect> &lst)->bool {
		for (int n = 0; n < lst.size(); n++)
		{
			if (lst[n].contains(event->pos()))
			{
				m_MoveIndex = n;
				return true;
			}
		}
		return false;
	};
	bool btakeTab = isInRect(lstRange);
	if (!btakeTab) m_MoveIndex = -1;
	bool bPressedTab = btakeTab &&
		((QTabWidget*)(this->parentWidget()))->currentWidget()->property("subwndid").toString() != Main_HomePage;
	if (bPressedTab)
		m_HaveDraged = true;
	
	m_MovingPic->setPixmap(QPixmap());
	QTabBar::mousePressEvent(event);
}
void DragInOutTabBar::mouseMoveEvent(QMouseEvent * event)
{
	if (m_HaveDraged)
	{
		setCursor(Qt::SizeAllCursor);
		if (m_MovingPic->pixmap()->isNull()) {
			QTabWidget *tab = (QTabWidget*)this->parentWidget();
			QRect tabRect = tab->rect();
			tabRect.setTop(tab->tabBar()->rect().height());
			QPixmap pixmap = tab->grab(tabRect).scaled(QSize(tabRect.width()*MOVING_PIXMAP_PERSENT, tabRect.height()*MOVING_PIXMAP_PERSENT));
			m_MovingPic->setPixmap(pixmap);
		}
		m_MovingWidget->show();
		m_MovingWidget->move(QCursor::pos());
	}
	QTabBar::mouseMoveEvent(event);
}
void DragInOutTabBar::mouseReleaseEvent(QMouseEvent * event)
{
	if (m_HaveDraged && 
		m_MoveIndex!=-1 && 
		m_MoveIndex<count() &&
		(event->pos().y() > height() || event->pos().y() < 0))
	{
		emit popSignalWnd(m_MoveIndex);
	}
	m_MoveIndex = -1;
	m_HaveDraged = false;
	unsetCursor();
	m_MovingWidget->hide();
	QTabBar::mouseReleaseEvent(event);
}
