#pragma once

#include "ui_paramgrholiday.h"
#include "bodywidget.h"

// ½ğÈÚÈÕÀú
class QStandardItemModel;
class QStandardItem;
class ParaMgrHoliday : public BodyWidget
{
	Q_OBJECT

public:
	ParaMgrHoliday(QWidget *parent = Q_NULLPTR);
	~ParaMgrHoliday();

	void init();

public slots:
	virtual void slotSkinChange();

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
