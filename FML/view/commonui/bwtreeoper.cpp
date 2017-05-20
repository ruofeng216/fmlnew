#include"bwtreeoper.h"



template<class T>
bool BWTreeOper<T>::delRootNode(
		QMap<QString, QList<QStandardItem *>> tree, 
		QStandardItemModel *pGoodsModel,
		QString &val) 
{
	if (tree.contains(val))
	{
		if (pGoodsModel->removeRow(m_tree[val].front()->row()))
			tree.remove(val);
		return true;
	}
	return false;
}

template<class T>
bool BWTreeOper<T>::delChildNode(
	QMap<QString, QList<QStandardItem *>> tree,
	QStandardItemModel *pGoodsModel,
	QString &val)
{
	if (this->m_tree.contains(val))
	{
		if (this->m_pGoodsModel->removeRow(m_tree[val].front()->row(), m_tree[val].front()->parent()->index()))
			this->m_tree.remove(val);
		return true;
	}
	return false;
}

template<class T>
bool BWTreeOper<T>::insertRootNode(
	QMap<QString, QList<QStandardItem *>> tree,
	QStandardItemModel *pGoodsModel,
	const QString &code,
	const T &val)
{
	QList<QStandardItem *> items;
	this->packQStandardItem(items, val);

	if (items.size() > 0) {
		this->pGoodsModel->appendRow(items);
		this->tree[code] = items;
	}
}

template<class T>
bool BWTreeOper<T>::insertChildNode(
	QMap<QString, QList<QStandardItem *>> tree,
	QStandardItemModel *pGoodsModel,
	const QString &code, 
	const QString &parentCode, const T &val) {
	if (!this->tree.contains(code) && this->tree.contains(parentCode))
	{
		QList<QStandardItem *> items;
		this->packQStandardItem(items, val);

		if (items.size() > 0) {
			tree[parentCode].front()->appendRow(items);
			tree[code] = items;
		}
	}
}


template<class T>
void BWTreeOper<T>::locator(QMap<QString, QList<QStandardItem *>> tree,QStandardItem *item) {
	if (item)
	{
		QString code = item->text();
		locator(tree,code);
	}
}

template<class T>
void BWTreeOper<T>::locator(
	QMap<QString, QList<QStandardItem *>> tree,
	QString &curCode) 
{
	m_tree.contains(curCode)
		bwLocate(curCode);
}

template<class T>
int BWTreeOper<T>::lastRow(
	QMap<QString, QList<QStandardItem *>> tree,
	QString &curCode) 
{
	int curRow = tree[curCode].front()->row();
	int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
	return nearRow;
}

template<class T>
void BWTreeOper<T>::delTree(
	/*QMap<QString, QList<QStandardItem *>> tree,
	QStandardItemModel *pGoodsModel,
	const QString &curCode, 
	const QString &parentCode*/) 
{
	//if (parentCode.isEmpty())
	//{// ¸ù½Úµã
	//	if (pGoodsModel->rowCount() <= 1)
	//	{
	//		bwClear();
	//	}
	//	else
	//	{
	//		if (tree.contains(curCode))
	//		{
	//			int nearRow = lastRow(curCode);
	//			QStandardItem* p = pGoodsModel->item(nearRow);
	//			locator(p);
	//		}
	//	}
	//	delRootNode(tree,pGoodsModel,curCode);
	//}
	//else
	//{
	//	if (tree.contains(parentCode) && tree[parentCode].front()->rowCount() <= 1)
	//	{
	//		if (recordExist(parentCode))
	//			locator(parentCode);
	//	}
	//	else if (tree.contains(parentCode) && tree.contains(curCode))
	//	{
	//		int nearRow = lastRow(curCode);
	//		QStandardItem* p = tree[parentCode].front()->child(nearRow);
	//		locator(p);
	//	}
	//	delChildNode(tree,pGoodsModel,curCode);
	//}
}


template<class T>
void BWTreeOper<T>::addTree(
	QMap<QString, QList<QStandardItem *>> tree,
	QStandardItemModel *pGoodsModel,
	const QString &curCode, 
	const QString &parentCode, 
	const T &val) 
{
	if (parentCode.isEmpty())
	{// root node
		if (tree.contains(curCode))
		{
			if (tree[curCode].isEmpty())
			{
				insertRootNode(val);
			}
			else
			{
				updateChildNode(val);
			}
		}
		else
		{
			insertRootNode(val);
		}
	}
	else
	{//child node
		if (tree.contains(parentCode) && !tree[parentCode].isEmpty())
		{//parent node exist
			if (tree.contains(curCode))
				updateChildNode(val);
			else
				insertChildNode(val);
		}
		else
		{//parent node not exist
			if (recordExist(parentCode)) {
				T parentVal = getTFromDB(parentCode);
				addTree(curCode, parentCode, parentVal);
				insertChildNode(code, parentCode, val);
			}
			else
				qWarning() << "Do not exist parent-code: " << rootCode;

		}
	}
}