#pragma once

#include<QStandardItemModel>

template<class T>
class BWTreeOper
{

public:
	BWTreeOper() {}
	~BWTreeOper() {}
	
public:
	bool delRootNode(QMap<QString, QList<QStandardItem *>> &tree, QStandardItemModel *pGoodsModel,const QString &val);
	bool delChildNode(QMap<QString, QList<QStandardItem *>> &tree, QStandardItemModel *pGoodsModel, const QString &val);
	
	void insertRootNode(
			QMap<QString, QList<QStandardItem *>> &tree, 
			QStandardItemModel *pGoodsModel,
			const QString &code, 
			const T &val,
			QList<int> cols
		);
	
	void insertChildNode(
			QMap<QString, QList<QStandardItem *>> &tree, 
			QStandardItemModel *pGoodsModel, 
			const QString &code, 
			const QString &parentCode, 
			const T &val,
			QList<int> cols
		);

public:
	void delTree(QMap<QString,QList<QStandardItem *>> &tree,QStandardItemModel *pGoodsModel,const QString &curCode, const QString &parentCode);
	void addTree(QMap<QString, QList<QStandardItem *>> &tree,QStandardItemModel *pGoodsModel, const QString &curCode, const QString &parentCode, const T &val, const QList<int> cols);

public:
	virtual void bwLocate(const QString &code) = 0;
	virtual bool recordExist(const QString &val) = 0;
	virtual void bwClear() = 0;
	virtual void packQStandardItem(QList<QStandardItem *> &items, const T &val,const QList<int> cols) = 0;
	virtual void updateChildNode(const T &val) = 0;
	virtual T getTFromDB(const QString &code,QString &parentCode) = 0;

public:
	void locator(QMap<QString, QList<QStandardItem *>> tree,const QString &code);
	void locatorItem(QMap<QString, QList<QStandardItem *>> tree,QStandardItem *item);
	int lastRow(QMap<QString, QList<QStandardItem *>> tree,const QString &curcode);

};


template<class T>
bool BWTreeOper<T>::delRootNode(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &val)
{
	if (tree.contains(val))
	{
		if (pGoodsModel->removeRow(tree[val].front()->row()))
			tree.remove(val);
		return true;
	}
	return false;
}

template<class T>
bool BWTreeOper<T>::delChildNode(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &val)
{
	if (tree.contains(val))
	{
		if (pGoodsModel->removeRow(tree[val].front()->row(), tree[val].front()->parent()->index()))
			tree.remove(val);
		return true;
	}
	return false;
}

template<class T>
void BWTreeOper<T>::insertRootNode(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &code,
	const T &val,
	const QList<int> cols)
{
	QList<QStandardItem *> items;
	packQStandardItem(items, val,cols);
	
	if (items.size() > 0) {
		pGoodsModel->appendRow(items);
		tree[code] = items;
	}
}

template<class T>
void BWTreeOper<T>::insertChildNode(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &code,
	const QString &parentCode, 
	const T &val,
	const QList<int> cols) 

{
	if (!tree.contains(code) && tree.contains(parentCode))
	{
		QList<QStandardItem *> items;
		packQStandardItem(items, val,cols);

		if (items.size() > 0) {
			tree[parentCode].front()->appendRow(items);
			tree[code] = items;
		}
	}
}


template<class T>
void BWTreeOper<T>::locatorItem(QMap<QString, QList<QStandardItem *>> tree, QStandardItem *item) {
	if (item)
	{
		QString code = item->text();
		locator(tree, code);
	}
}

template<class T>
void BWTreeOper<T>::locator(
	QMap<QString, QList<QStandardItem *>> tree,
	const QString &curCode)
{
	if(tree.contains(curCode))
		bwLocate(curCode);
}

template<class T>
int BWTreeOper<T>::lastRow(
	QMap<QString, QList<QStandardItem *>> tree,
	const QString &curCode)
{
	int curRow = tree[curCode].front()->row();
	int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
	return nearRow;
}

template<class T>
void BWTreeOper<T>::delTree(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &curCode,
	const QString &parentCode)
{
	if (parentCode.isEmpty())
	{// ¸ù½Úµã
		if (pGoodsModel->rowCount() <= 1)
		{
			bwClear();
		}
		else
		{
			if (tree.contains(curCode))
			{
				int nearRow = lastRow(tree,curCode);
				QStandardItem* p = pGoodsModel->item(nearRow);
				locatorItem(tree,p);
			}
		}
		delRootNode(tree,pGoodsModel,curCode);
	}
	else
	{
		if (tree.contains(parentCode) && tree[parentCode].front()->rowCount() <= 1)
		{
			if (recordExist(parentCode))
				locator(tree,parentCode);
		}
		else if (tree.contains(parentCode) && tree.contains(curCode))
		{
			int nearRow = lastRow(tree,curCode);
			QStandardItem* p = tree[parentCode].front()->child(nearRow);
			locatorItem(tree,p);
		}
		delChildNode(tree,pGoodsModel,curCode);
	}
}


template<class T>
void BWTreeOper<T>::addTree(
	QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel,
	const QString &curCode,
	const QString &parentCode,
	const T &val,
	const QList<int> cols)
{
	if (parentCode.isEmpty())
	{// root node
		if (tree.contains(curCode))
		{
			if (tree[curCode].isEmpty())
			{
				insertRootNode(tree, pGoodsModel,curCode,val,cols);
			}
			else
			{
				updateChildNode(val);
			}
		}
		else
		{
			insertRootNode(tree,pGoodsModel,curCode,val,cols);
		}
	}
	else
	{//child node
		if (tree.contains(parentCode) && !tree[parentCode].isEmpty())
		{//parent node exist
			if (tree.contains(curCode))
				updateChildNode(val);
			else
				insertChildNode(tree,pGoodsModel,curCode,parentCode,val,cols);
		}
		else
		{//parent node not exist
			QString ppCode;
			if (recordExist(parentCode)) {
				
				T parentVal = getTFromDB(parentCode,ppCode);
				addTree(tree,pGoodsModel, parentCode, ppCode, parentVal,cols);
				insertChildNode(tree,pGoodsModel,curCode, parentCode, val,cols);
			}
			else
				qWarning() << "Do not exist parent-code: " << parentCode;

		}
	}
}