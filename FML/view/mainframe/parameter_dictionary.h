#pragma once

#include "bodywidget.h"
#include "ui_parameter_dictionary.h"

class ParameterDictionary : public BodyWidget
{
	Q_OBJECT

public:
	ParameterDictionary(QWidget *parent = Q_NULLPTR);
	~ParameterDictionary();
	void init();

public slots:
	void slotSkinChange();

private:
	Ui::ParameterDictionary ui;
};
