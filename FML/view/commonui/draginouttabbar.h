#pragma once

#include <QTabBar>
class QMouseEvent;
class QLabel;
class QWidget;
class DragInOutTabBar : public QTabBar
{
	Q_OBJECT

public:
	DragInOutTabBar(QWidget *parent);
	~DragInOutTabBar();

protected:
	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
signals:
	void popSignalWnd(int nIndex);

private:
	bool		m_HaveDraged;
	QWidget*	m_MovingWidget;
	QLabel*		m_MovingPic;
};
