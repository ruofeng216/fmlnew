#pragma once

#include <QComboBox>

class QTreeView;
class QStandardItemModel;
class ComboxTree : public QComboBox
{
	Q_OBJECT

public:
	ComboxTree(QWidget *parent = nullptr);
	~ComboxTree();


protected:
	void showPopup();
	void hidePopup();

private:
	QTreeView *m_pTreeView;
	QStandardItemModel *m_pGoodsModel;
};
