#pragma once

#include <QWidget>

template <class T>
class CAction
{
public:
	CAction() {}
	virtual ~CAction() {}
	virtual bool isEqual(const T &newVal) = 0;
	virtual bool isKeyModify(const T &newVal) = 0;
};

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