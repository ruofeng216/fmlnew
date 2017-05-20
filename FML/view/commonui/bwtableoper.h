#pragma once

#include<QStandardItemModel>

template<class T>
class BWTableOper {

public:
	BWTableOper() {}
	~BWTableOper() {}
public:
	virtual void packQStandardItem(QList<QStandardItem *> &items, const T &val, const QList<int> cols) = 0;
public:
	void addRow(QMap<QString, QList<QStandardItem *>> &table,
		QStandardItemModel *pTableModel,
		const QString &code,
		const T &val, QList<int> cols);

	void delRow(QMap<QString, QList<QStandardItem *>> &tree,
		QStandardItemModel *pGoodsModel,
		const QString &val);
};

template<class T>
void BWTableOper<T>::addRow(QMap<QString, QList<QStandardItem *>> &table,
	QStandardItemModel *pTableModel,
	const QString &code,
	const T &val, QList<int> cols) {

	QList<QStandardItem *> items;
	packQStandardItem(items, val, cols);

	if (items.size() > 0) {
		pTableModel->appendRow(items);
		table[code] = items;
	}
}

template<class T>
void BWTableOper<T>::delRow(QMap<QString, QList<QStandardItem *>> &table,
	QStandardItemModel *pTableModel,
	const QString &val){

	if (table.contains(val))
	{
		if (pTableModel->removeRow(table[val].front()->row()))
			table.remove(val);
	}
}