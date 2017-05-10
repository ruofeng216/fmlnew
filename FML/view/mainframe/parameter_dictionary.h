#pragma once

#include "bodywidget.h"
#include "ui_parameter_dictionary.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class ParameterDictionary : public BodyWidget, public CAction<CParaDict>
{
	Q_OBJECT
	enum clomun_e {
		eParaCode = 0, //参数代码
		eParaName,     //参数名称
		eParaExplain,  //类别说明
		eEnd
	};
public:
	ParameterDictionary(QWidget *parent = Q_NULLPTR);
	~ParameterDictionary();
	QString getKey(const CParaDict &newVal) const;
	bool isKeyModify(const CParaDict &newVal);
	// 提交时，检查相关控件值是否合法。
	bool checkValid();

	void init();

public slots:
	void slotSkinChange();
	void slotAdd();
	void slotModify();
	void slotDelete();

private:
	void initDateView();
	CParaDict getViewData();
	void setViewData(const CParaDict &val);
	void packItem(QList<QStandardItem *> &childItems, const CParaDict &val);
	void addData(const CParaDict & date);
	void delData(const CParaDict & date);
	void locateData(const CParaDict & date);
	void clear();

private:
	Ui::ParameterDictionary ui;
	QStandardItemModel *m_model;
	QMap<QString, QList<QStandardItem *>> m_tree;
};
