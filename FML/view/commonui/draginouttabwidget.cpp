#include "draginouttabwidget.h"
#include "draginouttabbar.h"


DragInOutTabWidget::DragInOutTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	m_tabBar = new DragInOutTabBar(this);
	this->setTabBar(m_tabBar);
	connect(m_tabBar, &DragInOutTabBar::popSignalWnd, this, &DragInOutTabWidget::popSignalWnd,Qt::QueuedConnection);
}

DragInOutTabWidget::~DragInOutTabWidget()
{
}
