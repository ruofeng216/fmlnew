#pragma once

#include "ui_product_manage.h"
#include "bodywidget.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class ProductManage : public BodyWidget, public CAction<CProduct>
{
	Q_OBJECT

public:
	ProductManage(QWidget *parent = Q_NULLPTR);
	~ProductManage();
	QString getKey(const CProduct &newVal) const;
	// 提交时，检查相关控件值是否合法。
	bool checkValid();

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
	// 创建每行的items
	QList<QStandardItem*> createRowItems(const CProduct &val);
	// 递归插入所有孩子节点
	void appendChildrenProduct(QStandardItem *item, const QString &parentCode);
	// 界面设置获取产品信息
	CProduct getViewData();
	void setViewData(const CProduct &val);

private:
	Ui::product_manage ui;
	QStandardItemModel *m_model;
};
