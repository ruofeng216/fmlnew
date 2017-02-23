#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_fml.h"
#include "util/datatype.h"
#include "viewsignalmanager.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "view/webview/basewebengineview.h"

class FML : public QMainWindow
{
    Q_OBJECT

public:
    FML(QWidget *parent = Q_NULLPTR);
	~FML();
	QTabWidget *getMainTab();
	void init();

private slots:
	void slotPopSignalWnd(int nIndex);
	void slotPopSignalWndDBClk(int nIndex);
private:
	void initMenuFunc();
	void initWidget();
	void setMenu(QMenu *menu, const QString funcid);

	void saveWidget();

private:
    Ui::FMLClass ui;
	
};
