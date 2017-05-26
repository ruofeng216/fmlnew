#pragma once

#include "ui_financial_calendar.h"
#include "bodywidget.h"
#include "util/datatype.h"
#include "bwtreeoper.h"

// 金融日历
class QStandardItemModel;
class QStandardItem;
class FinancialCalendar 
	: public BodyWidget
	, public CAction<CFinancialCalendar>
	, public BWTreeOper<CFinancialCalendar>
{
	Q_OBJECT

	enum clomun_e {
		eYear=0,
		eDate, //年份
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
	void slotTreeClicked(const QModelIndex &index);

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
	void bwLocate(const QString &code, CFinancialCalendar t = CFinancialCalendar());
	bool recordExist(const QString &val, CFinancialCalendar t = CFinancialCalendar());
	void bwClear(CFinancialCalendar t = CFinancialCalendar());
	void packQStandardItem(QList<QStandardItem *> &items, const CFinancialCalendar &val, const QList<int> cols);
	void updateChildNode(const CFinancialCalendar &val);
	QStandardItem *getItem(QMap<QString, QList<QStandardItem *>> &tree, const QString &code, int nVal, int role = 257);
	CFinancialCalendar getTFromDB(const QString &code, QString &parentCode, CFinancialCalendar t = CFinancialCalendar());
	//void addTree(QMap<QString, QList<QStandardItem *>> &tree, QStandardItemModel *pGoodsModel, const QString &curCode, const QString &parentCode, const CFinancialCalendar &val, const QList<int> cols);

private:
	Ui::FinancialCalendar ui;
	QStandardItemModel *m_pGoodsModel;
	QMap<QString, QList<QStandardItem *>> m_tree;
};
