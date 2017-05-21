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
	enum clomun_e {
		eDate = 0, //年份
		eDateType,     //日期
		eInfo,     //描述
		eEnd       
	};
public:
	FinancialCalendar(QWidget *parent = Q_NULLPTR);
	~FinancialCalendar();
	QString getKey(const CFinancialCalendar &newVal) const;
	// 提交时，检查相关控件值是否合法。
	bool checkValid(bool opr = true);

	void init();

public slots:
	virtual void slotSkinChange();

private slots:
	void addHoliday();
	void modifyHoliday();
	void delHoliday();

private:
	void initDateView();
	CFinancialCalendar getViewData();
	void setViewData(const CFinancialCalendar &val);

	void addDate(const CFinancialCalendar & date);
	void delDate(const CFinancialCalendar & date);
	void locateDate(const CFinancialCalendar & date);
	void clear();

private:
	Ui::FinancialCalendar ui;
	QStandardItemModel *m_pGoodsModel;
	QMap<QString, QList<QStandardItem *>> m_tree;
};
