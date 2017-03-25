#pragma once

#include "ui_key_point_definition.h"
#include "bodywidget.h"
#include "util/datatype.h"

class KeyPointDefinition : public BodyWidget
{
	Q_OBJECT

public:
	KeyPointDefinition(QWidget *parent = Q_NULLPTR);
	~KeyPointDefinition();

public slots:
	void slotSkinChange();

private:
	Ui::KeyPointDefinition ui;
};
