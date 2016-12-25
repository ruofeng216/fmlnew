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
	
private slots:
	void slotPushDemoData(const demoStruct &val);

private:
    Ui::FMLClass ui;
	bool m_showFunc;
	DemoWebview *m_pWeball;
	DemoWebview1 *m_pWebAdd;
};
