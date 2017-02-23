#include "drop_widget.h"
#include <QBitmap>
#include <QPainter>
#include "util/util.h"
#include "util/pro.h"

DropWidget::DropWidget(QWidget *parent) : QDialog(parent)
{
	setObjectName("DropWidget");
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setMouseTracking(true);
	m_mousePressed = false;
}

DropWidget::~DropWidget()
{
}

void DropWidget::setTitle(const QString &title)
{
	m_title = title;
}

void DropWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) {
		m_mousePressed = true;
	}

	m_movePoint = event->globalPos() - pos(); 
}

void DropWidget::mouseReleaseEvent(QMouseEvent *)
{
	m_mousePressed = false;
}

void DropWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_mousePressed) {
		QPoint move_pos = event->globalPos();
		move(move_pos - m_movePoint);
	}
}

void DropWidget::paintEvent( QPaintEvent *event)
{
	if (!m_title.isEmpty()) {
		QFontMetrics fm = this->fontMetrics();
		int textWidth = fm.width(m_title);
		int x = (this->width() - textWidth) / 2;
		int y = 20;
		QPainter painter(this);
		painter.drawText(x, y, m_title);
	}
}

