#pragma once

#include "ui_financial_calendar.h"
#include "bodywidget.h"
#include "util/datatype.h"

// ½ðÈÚÈÕÀú
class QStandardItemModel;
class QStandardItem;
class FinancialCalendar : public BodyWidget, public CAction<CFinancialCalendar>
{
	Q_OBJECT

public:
	FinancialCalendar(QWidget *parent = Q_NULLPTR);
	~FinancialCalendar();
	bool isKeyModify(const CFinancialCalendar &newVal);
	bool isEqual(const CFinancialCalendar &newVal);
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
