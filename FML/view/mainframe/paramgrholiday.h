#pragma once

#include <QWidget>
#include "ui_paramgrholiday.h"

// ½ğÈÚÈÕÀú
class ParaMgrHoliday : public QWidget
{
	Q_OBJECT

public:
	ParaMgrHoliday(QWidget *parent = Q_NULLPTR);
	~ParaMgrHoliday();

	void init();

private slots:
	void addHoliday();
	void modifyHoliday();
	void delHoliday();

private:
	Ui::ParaMgrHoliday ui;
};
