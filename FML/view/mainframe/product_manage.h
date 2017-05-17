#pragma once

#include "ui_product_manage.h"
#include "bodywidget.h"
#include "util/datatype.h"
#include "bwtreeoper.h"

class QStandardItemModel;
class QStandardItem;
class ProductManage : public BodyWidget
	,public CAction<CProduct>
	,public BWTreeOper<CProduct>
{
	Q_OBJECT
	enum clomun_e {
		eProductCode = 0, //产品代码
		eProductName,     //产品名称
		eParentcode,   //父级代码
		eParentname,   //父级代码名称
		eSdate,        //开始时间
		eEdate,        //截至时间
		eAnnotation,   //组合说明
		eEnd
	};
public:
	ProductManage(QWidget *parent = Q_NULLPTR);
	~ProductManage();
	QString getKey(const CProduct &newVal) const;
	// 提交时，检查相关控件值是否合法。
	bool checkValid();

	void init();

public slots:
	void slotSkinChange();

private:
	void slotAdd(bool);
	void slotModify(bool);
	void slotDelete(bool);

private:
	void initDateView();
	
	// 界面设置获取产品信息
	CProduct getViewData();
	void setViewData(const CProduct &val);


private:
	Ui::product_manage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// 映射节点
	QMap<QString, QList<QStandardItem *>> m_tree;
	QMap<QString, QList<QStandardItem *>> m_treeCombobox;

	//*********************************************************************
	//* BWTreeOper pure virtual function
	//*********************************************************************
public:
	void bwLocate(const QString &code);
	void bwClear();
	bool recordExist(const QString &val);
	CProduct getTFromDB(const QString &code, QString &parentCode);
	void packQStandardItem(QList<QStandardItem *> &items, const CProduct &val, const QList<int> cols);
	void updateChildNode(const CProduct &val);
	
};
