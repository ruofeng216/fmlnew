#pragma once

#include <QTabWidget>
class DragInOutTabBar;
class DragInOutTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	DragInOutTabWidget(QWidget *parent);
	~DragInOutTabWidget();
signals:
	void popSignalWnd(int nIndex);
private:
	DragInOutTabBar *m_tabBar;
};
