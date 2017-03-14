#pragma once

#include <QWidget>
#include "ui_paramgrholiday.h"

// ��������
class QStandardItemModel;
class QStandardItem;
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
	void initDateView();
	void expand(int y);
private:
	Ui::ParaMgrHoliday ui;
	QStandardItemModel *m_pGoodsModel;
};
