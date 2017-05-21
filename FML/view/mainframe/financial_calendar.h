#pragma once

#include "ui_financial_calendar.h"
#include "bodywidget.h"
#include "util/datatype.h"

// ��������
class QStandardItemModel;
class QStandardItem;
class FinancialCalendar : public BodyWidget, public CAction<CFinancialCalendar>
{
	Q_OBJECT
	enum clomun_e {
		eDate = 0, //���
		eDateType,     //����
		eInfo,     //����
		eEnd       
	};
public:
	FinancialCalendar(QWidget *parent = Q_NULLPTR);
	~FinancialCalendar();
	QString getKey(const CFinancialCalendar &newVal) const;
	// �ύʱ�������ؿؼ�ֵ�Ƿ�Ϸ���
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
