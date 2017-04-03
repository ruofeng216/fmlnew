#pragma once

#include "ui_key_point_definition.h"
#include "bodywidget.h"
#include "util/datatype.h"

class QStandardItemModel;
class QStandardItem;
class KeyPointDefinition : public BodyWidget
{
	Q_OBJECT

public:
	KeyPointDefinition(QWidget *parent = Q_NULLPTR);
	~KeyPointDefinition();
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
	QString getParaNameFromCode(const QString &code);
	QList<QStandardItem *> createParentRowItems(const CKeypoint &val);
	QList<QStandardItem *> createChildtRowItems(const CKeypoint &val);

private:
	Ui::KeyPointDefinition ui;
	QStandardItemModel *m_model;
};
