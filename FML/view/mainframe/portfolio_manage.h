#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"

class QStandardItemModel;
class QStandardItem;
// ��Ϲ���
class PortfolioManage : public BodyWidget, public CAction<CPortfolio>
{
	Q_OBJECT
	enum clomun_e {
		ePortcode = 0, //����
		ePortname,     //��������
		eParentcode,   //��������
		eParentname,   //������������
		eSdate,        //��ʼʱ��
		eEdate,        //����ʱ��
		eAnnotation,   //���˵��
		eEnd
	};
public:
	PortfolioManage(QWidget *parent = Q_NULLPTR);
	~PortfolioManage();
	QString getKey(const CPortfolio &newVal) const;

	// �ύʱ�������ؿؼ�ֵ�Ƿ�Ϸ���
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
	// ӳ��ڵ�
	QMap<QString, QList<QStandardItem *>> m_tree;
	QMap<QString, QList<QStandardItem *>> m_treeCombobox;
};
