#include "comboxtree.h"
#include <QTreeView>
#include <QStandardItemModel>

ComboxTree::ComboxTree(QWidget *parent)
	: QComboBox(parent)
	, m_pTreeView(NULL)
	, m_pGoodsModel(NULL)
{
	m_pTreeView = new QTreeView(NULL);
	if (!m_pGoodsModel) m_pGoodsModel = new QStandardItemModel(0, 1, m_pTreeView);
	m_pTreeView->setModel(m_pGoodsModel);
	m_pTreeView->hide();
}

ComboxTree::~ComboxTree()
{
	if (m_pTreeView) m_pTreeView->deleteLater();
}

void ComboxTree::showPopup()
{
	m_pTreeView->setColumnWidth(0, qMax(200, m_pTreeView->width()));
	m_pTreeView->setMaximumWidth(qMax(200, m_pTreeView->width()));
	m_pTreeView->move(this->rect().x(), rect().y() + rect().height());
	m_pTreeView->show();
}
void ComboxTree::hidePopup()
{
	m_pTreeView->hide();
}