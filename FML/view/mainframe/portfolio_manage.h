#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class PortfolioManage : public BodyWidget
{
	Q_OBJECT

public:
	PortfolioManage(QWidget *parent = Q_NULLPTR);
	~PortfolioManage();
	
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
private:
	Ui::PortfolioManage ui;
	QStandardItemModel *m_pGoodsModel;
};
