#pragma once

#include <QWidget>
#include "ui_paramgrunion.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class ParaMgrUnion : public QWidget
{
	Q_OBJECT

public:
	ParaMgrUnion(QWidget *parent = Q_NULLPTR);
	~ParaMgrUnion();
	
	void init();

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
