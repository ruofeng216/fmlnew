#pragma once

#include <QWidget>
#include "ui_login.h"

class login : public QWidget
{
	Q_OBJECT

public:
	login(QWidget *parent = Q_NULLPTR);
	~login();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private slots:
	void loginSys();
	void registerSys();

private:
	Ui::login ui;
};
