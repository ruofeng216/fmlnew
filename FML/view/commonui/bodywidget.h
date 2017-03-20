#pragma once

#include <QWidget>

class BodyWidget : public QWidget
{
	Q_OBJECT

public:
	BodyWidget(QWidget *parent);
	virtual ~BodyWidget();

public slots:
	virtual void slotSkinChange() = 0;
};
