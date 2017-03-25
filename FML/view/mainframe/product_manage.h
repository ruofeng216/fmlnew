#pragma once

#include "ui_product_manage.h"
#include "bodywidget.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class ProductManage : public BodyWidget
{
	Q_OBJECT

public:
	ProductManage(QWidget *parent = Q_NULLPTR);
	~ProductManage();
	void init();

public slots:
	void slotSkinChange();
	void slotAdd();
	void slotModify();
	void slotDelete();

private:
	QList<QStandardItem*> createRowItems(const CProduct &val);
	void appendChildrenProduct(QStandardItem *item, const QString &parentCode);

private:
	Ui::product_manage ui;
	QStandardItemModel *m_model;
};
