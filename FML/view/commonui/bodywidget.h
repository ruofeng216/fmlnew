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

// ��ʱ�ģ�Ϊ�˹���BodyWidget����
class EmptyWidget : public BodyWidget
{
	Q_OBJECT
public:
	EmptyWidget(QWidget *parent = NULL) : BodyWidget(parent) {}

public slots:
	void slotSkinChange() {}
};