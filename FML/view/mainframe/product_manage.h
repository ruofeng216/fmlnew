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
	void slotTreeDoubleClicked(const QModelIndex &index);
	void slotParentCodeChanged(int index);
	void slotParentNameChanged(int index);

private:
	// ����ÿ�е�items
	QList<QStandardItem*> createRowItems(const CProduct &val);
	// �ݹ�������к��ӽڵ�
	void appendChildrenProduct(QStandardItem *item, const QString &parentCode);
	// �������û�ȡ��Ʒ��Ϣ
	CProduct getViewProduct();
	void setViewProduct(const CProduct &val);

private:
	Ui::product_manage ui;
	QStandardItemModel *m_model;
};
