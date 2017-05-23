#include "treecombobox.h"
#include <QWidget>
#include <QStyle>
#include <QPropertyAnimation>
#include<QTreeView>
#include<QMouseEvent>
#include<QModelIndex>

TreeComboBox::TreeComboBox(QWidget *parent) :XComboBox(parent) {}
TreeComboBox::~TreeComboBox() {}

bool TreeComboBox::eventFilter(QObject* obj, QEvent* evt) {

	if (evt->type() == QEvent::MouseButtonRelease && obj == view()->viewport())
	{
		return true;
	}

	if (evt->type() == QEvent::MouseButtonDblClick) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(evt);
		QModelIndex index = view()->indexAt(mouseEvent->pos());
		this->setEditText(index.sibling(index.row(), 0).data().toString());
		this->hidePopup();
		return true;
	}
	return false;
}