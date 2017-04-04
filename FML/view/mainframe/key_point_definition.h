#pragma once

#include "ui_key_point_definition.h"
#include "bodywidget.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class KeyPointDefinition : public BodyWidget, public CAction<CKeypoint>
{
	Q_OBJECT

public:
	KeyPointDefinition(QWidget *parent = Q_NULLPTR);
	~KeyPointDefinition();
	bool isEqual(const CKeypoint &newVal);
	bool isKeyModify(const CKeypoint &newVal);
	void init();

public slots:
	void slotSkinChange();
	void slotAdd();
	void slotModify();
	void slotDelete();
	void slotTreeDoubleClicked(const QModelIndex &index);

private:
	void setViewData(const CKeypoint &val);
	CKeypoint getViewData();
	QPair<int, QString> parseTenor(const QString& tenor);
	QString spliceTenor(int num, QString unit);
	QString getParaNameFromCode(const QString &typecode, const QString &paracode);
	QList<QStandardItem *> createParentRowItems(const CKeypoint &val);
	QList<QStandardItem *> createChildtRowItems(const CKeypoint &val);

private:
	Ui::KeyPointDefinition ui;
	QStandardItemModel *m_model;
};
