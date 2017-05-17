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
	
	// �������û�ȡ��Ʒ��Ϣ
	CProduct getViewData();
	void setViewData(const CProduct &val);


private:
	Ui::product_manage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// ӳ��ڵ�
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
