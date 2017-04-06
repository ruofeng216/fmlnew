#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class PortfolioManage : public BodyWidget, public CAction<CPortfolio>
{
	Q_OBJECT

public:
	PortfolioManage(QWidget *parent = Q_NULLPTR);
	~PortfolioManage();
	QString getKey(const CPortfolio &newVal) const;
	bool isEqual(const CPortfolio &newVal);
	void init();

public slots:
	virtual void slotSkinChange();

private slots:
	void addPortfolio();
	void modifyPortfolio();
	void delPortfolio();

private:
	void initDateView();
	void packItem(QList<QStandardItem *> &childItems, const CPortfolio &val);
	void packChild(QStandardItem *parent, const QString &curID);
	CPortfolio getViewData();
	void setViewData(const CPortfolio &val);

private:
	Ui::PortfolioManage ui;
	QStandardItemModel *m_pGoodsModel;
};
