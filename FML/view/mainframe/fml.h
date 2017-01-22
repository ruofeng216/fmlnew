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
	QTabWidget *getMainTab();
	
private slots:
	void slotPopSignalWnd(int nIndex);

private:
	void init();
	void initMenuFunc();
	void setMenu(QMenu *menu, const QString funcid);

private:
    Ui::FMLClass ui;
};
