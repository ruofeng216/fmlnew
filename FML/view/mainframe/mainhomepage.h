#pragma once

#include "ui_mainhomepage.h"
#include "bodywidget.h"

class MainHomePage : public BodyWidget
{
	Q_OBJECT

public:
	MainHomePage(QWidget *parent = Q_NULLPTR);
	~MainHomePage();

public slots:
	virtual void slotSkinChange();

private:
	Ui::MainHomePage ui;
};
