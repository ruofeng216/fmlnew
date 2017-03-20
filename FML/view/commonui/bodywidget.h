#pragma once

#include <QObject>

class BodyWidget : public QObject
{
	Q_OBJECT

public:
	BodyWidget(QObject *parent);
	~BodyWidget();
public slots:
	virtual void slotSkinChange() = 0;
};
