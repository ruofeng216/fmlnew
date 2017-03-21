#pragma once

#include "ui_product_manage.h"
#include "bodywidget.h"

class ProductManage : public BodyWidget
{
	Q_OBJECT

public:
	ProductManage(QWidget *parent = Q_NULLPTR);
	~ProductManage();

public slots:
	void slotSkinChange();

private:
	Ui::product_manage ui;
};
