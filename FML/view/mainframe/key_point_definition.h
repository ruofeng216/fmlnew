#pragma once

#include "ui_key_point_definition.h"
#include "bodywidget.h"
#include "util/datatype.h"
#include "bwtreeoper.h"

class QStandardItemModel;
class QStandardItem;
class KeyPointDefinition 
	: public BodyWidget
	, public CAction<CKeypoint>
	, public ProductTreeOper
	, public KeypointTreeOper
{
	Q_OBJECT
	enum clomun_e {
		eKPCode = 0, //�ؼ������
		eKPName,     //�ؼ�������
		eProductCode,   //��Ʒ����
		eProductName,   //��Ʒ����
		eTenor,        //����
		eSdate,        //��ʼ����
		eEdate,        //��������
		eMarket,   //�����г�
		eCalendar,   //����
		eConvention,   //���յ��� ��Ϣ�յ���
		eDayCount,   //��������
		eCouponfrequency,   //�������� ��ϢƵ��
		eEnd
	};
public:
	KeyPointDefinition(QWidget *parent = Q_NULLPTR);
	~KeyPointDefinition();
	QString getKey(const CKeypoint &newVal) const;
	// �ύʱ�������ؿؼ�ֵ�Ƿ�Ϸ���
	bool checkValid(bool opr = true);

	void init();

public slots:
	void slotSkinChange();
	void slotAdd();
	void slotModify();
	void slotDelete();
	void slotTreeClicked(const QModelIndex &index);

private slots:
	// init product
	void initProduct();
	void initParameter(const QString &paraCode);

private:
	

	void setViewData(const CKeypoint &val);
	CKeypoint getViewData();
	QPair<int, QString> parseTenor(const QString& tenor);
	QString spliceTenor(int num, QString unit);
	bool isProductExist(const QString &productCode);


	void bwLocate(const QString &code, CKeypoint t = CKeypoint());
	bool recordExist(const QString &val, CKeypoint t = CKeypoint());
	void bwClear(CKeypoint t = CKeypoint());
	void packQStandardItem(QList<QStandardItem *> &items, const CKeypoint &val, const QList<int> cols);
	void updateChildNode(const CKeypoint &val);
	CKeypoint getTFromDB(const QString &code, QString &parentCode, CKeypoint t = CKeypoint());
	QStandardItem *getItem(QMap<QString, QList<QStandardItem *>> &tree, const QString &code, int nVal, int role = 257, CKeypoint t = CKeypoint());
	void addTree(QMap<QString, QList<QStandardItem *>> &tree, QStandardItemModel *pGoodsModel, const QString &curCode, const QString &parentCode, const CKeypoint &val, const QList<int> cols);

	void bwLocate(const QString &code, CProduct t = CProduct());
	bool recordExist(const QString &val, CProduct t = CProduct());
	void bwClear(CProduct t = CProduct());
	void packQStandardItem(QList<QStandardItem *> &items, const CProduct &val, const QList<int> cols);
	void updateChildNode(const CProduct &val);
	CProduct getTFromDB(const QString &code, QString &parentCode, CProduct t = CProduct());
	void addTree(QMap<QString, QList<QStandardItem *>> &tree, QStandardItemModel *pGoodsModel, const QString &curCode, const QString &parentCode, const CProduct &val, const QList<int> cols);

private:
	Ui::KeyPointDefinition ui;
	QStandardItemModel *m_model;
	QStandardItemModel *m_pProductModel;
	// ӳ��ڵ�
	QMap<QString, QList<QStandardItem *>> m_tree;
};
