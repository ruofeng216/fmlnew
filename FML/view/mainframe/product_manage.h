#pragma once

#include "ui_product_manage.h"
#include "bodywidget.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class ProductManage : public BodyWidget, public CAction<CProduct>
{
	Q_OBJECT
	enum clomun_e {
		eProductCode = 0, //��Ʒ����
		eProductName,     //��Ʒ����
		eParentcode,   //��������
		eParentname,   //������������
		eSdate,        //��ʼʱ��
		eEdate,        //����ʱ��
		eAnnotation,   //���˵��
		eEnd
	};
public:
	ProductManage(QWidget *parent = Q_NULLPTR);
	~ProductManage();
	QString getKey(const CProduct &newVal) const;
	// �ύʱ�������ؿؼ�ֵ�Ƿ�Ϸ���
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
	void packItem(QList<QStandardItem *> &childItems, const CProduct &val);
	// �������û�ȡ��Ʒ��Ϣ
	CProduct getViewData();
	void setViewData(const CProduct &val);

	void addProductData(const CProduct & val);
	void delProductData(const CProduct & val);
	void locateProductData(const CProduct & val);
	void clear();

private:
	Ui::product_manage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// ӳ��ڵ�
	QMap<QString, QList<QStandardItem *>> m_tree;
	QMap<QString, QList<QStandardItem *>> m_treeCombobox;
};
