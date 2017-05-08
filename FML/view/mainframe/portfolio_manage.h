#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"

class QStandardItemModel;
class QStandardItem;
// 组合管理
class PortfolioManage : public BodyWidget, public CAction<CPortfolio>
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
	void packItem(QList<QStandardItem *> &childItems, const CPortfolio &val);
	CPortfolio getViewData();
	void setViewData(const CPortfolio &val);

	void addPortfolioData(const CPortfolio & date);
	void delPortfolioData(const CPortfolio & date);
	void locatePortfolioData(const CPortfolio & date);
	void clear();
private:
	Ui::PortfolioManage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// 映射节点
	QMap<QString, QList<QStandardItem *>> m_tree;
	QMap<QString, QList<QStandardItem *>> m_treeCombobox;
};
