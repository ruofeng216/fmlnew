#pragma once

#include "bodywidget.h"
#include "ui_parameter_dictionary.h"
#include "util/datatype.h"
#include "bwtreeoper.h"
#include "bwtableoper.h"

class QStandardItemModel;
class QStandardItem;
class ParameterDictionary : public BodyWidget
	, public CAction<CParaDict>
	, public BWTreeOper<CParaDict>
	, public BWTableOper<CParaDict>
{
	Q_OBJECT
	enum clomun_e {
		eParaCode = 0, //参数代码
		eParaName,     //参数名称
		eTypeCode,
		eTypeName,
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

	QStandardItemModel *m_modelCombobox;
	QMap<QString, QList<QStandardItem *>> m_table;
	/*
	 BWTreeOper pure virtual function
	*/
	void bwLocate(const QString &code, CParaDict t = CParaDict());
	bool recordExist(const QString &val, CParaDict t = CParaDict()) ;
	void bwClear(CParaDict t = CParaDict()) ;
	void packQStandardItem(QList<QStandardItem *> &items, const CParaDict &val, const QList<int> cols) ;
	void updateChildNode(const CParaDict &val) ;
	CParaDict getTFromDB(const QString &code, QString &parentCode, CParaDict t = CParaDict()) ;
};
