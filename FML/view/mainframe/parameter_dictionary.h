#pragma once

#include "bodywidget.h"
#include "ui_parameter_dictionary.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class ParameterDictionary : public BodyWidget, public CAction<CParaDict>
{
	Q_OBJECT

public:
	ParameterDictionary(QWidget *parent = Q_NULLPTR);
	~ParameterDictionary();
	QString getKey(const CParaDict &newVal) const;
	bool isEqual(const CParaDict &newVal);

	void init();
	CParaDict getViewData();
	void setViewData(const CParaDict &val);

public slots:
	void slotSkinChange();
	void slotAdd();
	void slotModify();
	void slotDelete();
	void slotTreeDoubleClicked(const QModelIndex &index);
	void slotTypeCodeChanged(int index);
	void slotTypeNameChanged(int index);

private:
	QList<QStandardItem*> createRowItems(const CParaDict &val, bool isRoot);

private:
	Ui::ParameterDictionary ui;
	QStandardItemModel *m_model;
};
