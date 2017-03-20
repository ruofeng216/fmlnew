#pragma once

#include <QWidget>

class BodyWidget : public QWidget
{
	Q_OBJECT

public:
	BodyWidget(QWidget *parent = NULL);
	virtual ~BodyWidget();

public slots:
	virtual void slotSkinChange() = 0;
};

// 临时的，为了构造BodyWidget对象
class EmptyWidget : public BodyWidget
{
	Q_OBJECT
public:
	EmptyWidget(QWidget *parent = NULL) : BodyWidget(parent) {}

public slots:
	void slotSkinChange() {}
};