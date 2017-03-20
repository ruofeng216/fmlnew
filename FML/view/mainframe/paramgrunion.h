#pragma once

#include "ui_paramgrunion.h"
#include "util/datatype.h"
#include "bodywidget.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class ParaMgrUnion : public BodyWidget
{
	Q_OBJECT

public:
	ParaMgrUnion(QWidget *parent = Q_NULLPTR);
	~ParaMgrUnion();
	
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
	Ui::ParaMgrUnion ui;
	QStandardItemModel *m_pGoodsModel;
};
