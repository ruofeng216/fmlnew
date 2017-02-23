#pragma once

#include <QWidget>
#include "ui_mainhomepage.h"

class MainHomePage : public QWidget
{
	Q_OBJECT

public:
	MainHomePage(QWidget *parent = Q_NULLPTR);
	~MainHomePage();

private:
	Ui::MainHomePage ui;
};
