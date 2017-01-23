#include "draginouttabbar.h"
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QMouseEvent>
#include <QTabWidget>
#define MOVING_PIXMAP_SIZE 300

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
	m_MovingWidget = new QWidget;
	m_MovingPic = new QLabel(m_MovingWidget);
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(m_MovingPic);
	m_MovingWidget->setLayout(layout);
	QString style = "QWidget{background-color:rgba(0,0,0,50%);background:transparent;};QLabel{background:transparent};";
	m_MovingWidget->setStyleSheet(style);
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
	bool bPressedTab = event->pos().x()>0 && event->pos().x()<width() && event->pos().y()>0 && event->pos().y()<height();
	if (bPressedTab)
		m_HaveDraged = true;
	QTabBar::mousePressEvent(event);
}
void DragInOutTabBar::mouseMoveEvent(QMouseEvent * event)
{
	if (m_HaveDraged)
	{
		setCursor(Qt::SizeAllCursor);
		QPixmap pixmap = QPixmap::grabWidget(((QTabWidget*)(this->parentWidget()))->currentWidget());
		pixmap = pixmap.scaled(QSize(MOVING_PIXMAP_SIZE, MOVING_PIXMAP_SIZE));
		m_MovingPic->setPixmap(pixmap);
		m_MovingWidget->show();
		m_MovingWidget->move(QCursor::pos());
	}
	QTabBar::mouseMoveEvent(event);
}
void DragInOutTabBar::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->pos().y() > height() || event->pos().y() < 0)
	{
		emit popSignalWnd(currentIndex());
	}
		
	m_HaveDraged = false;
	unsetCursor();
	m_MovingWidget->hide();
	QTabBar::mouseReleaseEvent(event);
}