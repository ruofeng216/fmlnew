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
		eKPCode = 0, //关键点代码
		eKPName,     //关键点名称
		eProductCode,   //产品代码
		eProductName,   //产品名称
		eTenor,        //期限
		eSdate,        //开始日期
		eEdate,        //截至日期
		eMarket,   //所属市场
		eCalendar,   //日历
		eConvention,   //假日调整 计息日调整
		eDayCount,   //天数计数
		eCouponfrequency,   //交割天数 付息频率
		eEnd
	};
public:
	KeyPointDefinition(QWidget *parent = Q_NULLPTR);
	~KeyPointDefinition();
	QString getKey(const CKeypoint &newVal) const;
	// 提交时，检查相关控件值是否合法。
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
	// 映射节点
	QMap<QString, QList<QStandardItem *>> m_tree;
};
