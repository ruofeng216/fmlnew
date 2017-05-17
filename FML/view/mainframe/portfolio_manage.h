#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"
#include "bwtreeoper.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class PortfolioManage : public BodyWidget
	,public CAction<CPortfolio>
	,public BWTreeOper<CPortfolio>
{
	Q_OBJECT
	enum clomun_e {
		ePortcode = 0, //代码
		ePortname,     //代码名称
		eParentcode,   //父级代码
		eParentname,   //父级代码名称
		eSdate,        //开始时间
		eEdate,        //截至时间
		eAnnotation,   //组合说明
		eEnd
	};
public:
	PortfolioManage(QWidget *parent = Q_NULLPTR);
	~PortfolioManage();
	QString getKey(const CPortfolio &newVal) const;

	// 提交时，检查相关控件值是否合法。
	bool checkValid();

	void init();

public slots:
	virtual void slotSkinChange();

private slots:
	void addPortfolio(bool);
	void modifyPortfolio(bool);
	void delPortfolio(bool);

private:
	void initDateView();
	
	CPortfolio getViewData();
	void setViewData(const CPortfolio &val);

private:
	Ui::PortfolioManage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// 映射节点
	QMap<QString, QList<QStandardItem *>> m_tree;
	QMap<QString, QList<QStandardItem *>> m_treeCombobox;

	//*******************************************************************************************
public:
	void bwLocate(const QString &code);
	bool recordExist(const QString &val);
	CPortfolio getTFromDB(const QString &code, QString &parentCode);
	void packQStandardItem(QList<QStandardItem *> &items, const CPortfolio &val, const QList<int> cols);
	void updateChildNode(const CPortfolio &val);
	void bwClear();
};
