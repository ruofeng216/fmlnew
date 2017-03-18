#include "login.h"
#include "view/commonui/message_box_widget.h"
#include "view/view_controller.h"
#include "controller/qcontrollermanager.h"
#include <QTimer>

login::login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.leUsername->setPlaceholderText(tr("input name"));
	ui.lePassword->setPlaceholderText(tr("input password"));

	connect(ui.btn_login, SIGNAL(clicked()), this, SLOT(loginSys()));
	connect(ui.btn_reg, SIGNAL(clicked()), this, SLOT(registerSys()));
	ui.btn_login->setShortcut(QKeySequence::InsertParagraphSeparator);


#if 0
	ui.leUsername->setText("hello");
	ui.lePassword->setText("hello");
	QTimer::singleShot(1000, [this]() {
		this->loginSys();
	});
#endif
}

login::~login()
{
}

void login::loginSys()
{
	QString username = ui.leUsername->text();
	QString password = ui.lePassword->text();

	if (username.isEmpty() || password.isEmpty())
	{
		ShowWarnMessage(tr("login"), tr("The user name or password cannot be empty"), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
	eERR er = LONGINCTL->chkLogin(username, password);
	if (e_Success == er)
	{
		ViewController::instance()->closewnd(LOGIN_WINDOW_ID);
		ViewController::instance()->openwnd(MAIN_WINDOW_ID, tr("Financial Market Leader"));
		return;
	}
	if (e_NoUser == er)
	{
		ShowWarnMessage(tr("login"), tr("no user. if you have none, please register."), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
	if (e_ErrPswd == er)
	{
		ShowWarnMessage(tr("login"), tr("user pswd error, no login."), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
}
void login::registerSys()
{
	QString username = ui.leUsername->text();
	QString password = ui.lePassword->text();
	if (username.isEmpty() || password.isEmpty())
	{
		ShowWarnMessage(tr("login"), tr("The user name or password cannot be empty"), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
	eERR er = LONGINCTL->regLogin(username, password);
	if (e_Success == er)
	{
		ViewController::instance()->closewnd(LOGIN_WINDOW_ID);
		ViewController::instance()->openwnd(MAIN_WINDOW_ID, tr("Financial Market Leader"));
		return;
	}
	if (e_Exist == er)
	{
		ShowWarnMessage(tr("login"), tr("user existed, please register another."), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
	if (e_RegErr == er)
	{
		ShowWarnMessage(tr("login"), tr("register error, call IT."), this);
		ViewController::instance()->showwnd(LOGIN_WINDOW_ID);
		ui.btn_login->setFocus();
		return;
	}
}

void login::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Enter ||
		e->key() == Qt::Key_Return)
	{
		if (ui.btn_reg->hasFocus())
			emit ui.btn_reg->clicked();
		else
			emit ui.btn_login->clicked();
	}
}