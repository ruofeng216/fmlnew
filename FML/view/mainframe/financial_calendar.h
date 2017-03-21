#pragma once

#include "ui_financial_calendar.h"
#include "bodywidget.h"

// ½ğÈÚÈÕÀú
class QStandardItemModel;
class QStandardItem;
class FinancialCalendar : public BodyWidget
{
	Q_OBJECT

public:
	FinancialCalendar(QWidget *parent = Q_NULLPTR);
	~FinancialCalendar();

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
	Ui::FinancialCalendar ui;
	QStandardItemModel *m_pGoodsModel;
};
