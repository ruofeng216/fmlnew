#pragma once

#include "ui_portfolio_manage.h"
#include "util/datatype.h"
#include "bodywidget.h"
#include "bwtreeoper.h"

class QStandardItemModel;
class QStandardItem;
// ��Ϲ���
class PortfolioManage : public BodyWidget
	,public CAction<CPortfolio>
	,public BWTreeOper<CPortfolio>
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
	
	CPortfolio getViewData();
	void setViewData(const CPortfolio &val);

private:
	Ui::PortfolioManage ui;
	QStandardItemModel *m_pGoodsModel;
	QStandardItemModel *m_pGoodsModelCombobox;
	// ӳ��ڵ�
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
