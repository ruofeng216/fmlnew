#pragma once

#include "ui_financial_calendar.h"
#include "bodywidget.h"
#include "util/datatype.h"

// 金融日历
class QStandardItemModel;
class QStandardItem;
class FinancialCalendar : public BodyWidget, public CAction<CFinancialCalendar>
{
	Q_OBJECT

public:
	FinancialCalendar(QWidget *parent = Q_NULLPTR);
	~FinancialCalendar();
	QString getKey(const CFinancialCalendar &newVal) const;
	// 提交时，检查相关控件值是否合法。
	bool checkValid();

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
	CFinancialCalendar getViewData();
	void setViewData(const CFinancialCalendar &val);

private:
	Ui::FinancialCalendar ui;
	QStandardItemModel *m_pGoodsModel;
};
