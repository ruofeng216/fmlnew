#pragma once

#include "xcombobox.h"

class TreeComboBox : public XComboBox
{
	Q_OBJECT
	
public:
	TreeComboBox(QWidget *parent);
	~TreeComboBox();

protected:
	bool eventFilter(QObject* obj, QEvent* evt);
};
