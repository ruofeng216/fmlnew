#include "qmpreload.h"
#include <QApplication>
#include "view/view_controller.h"
#include <QTranslator>
#include <QFile>
#include <QIcon>

QmPreload::QmPreload(QObject *parent)
	: QObject(parent)
{

}

QmPreload::~QmPreload()
{

}

bool QmPreload::init(int argc, char *argv[])
{
	qutil::initSkin("dark");
	// 加载默认文字资源
	static QTranslator trans;
	bool isLoad = trans.load(QString(":/font/qt_zh_CN.qm"));
	if (isLoad) {
		qApp->installTranslator(&trans);
	} else {
		qDebug() << "load qt_zh_CN.qm failed";
	}

	static QTranslator widgetsTrans;
	isLoad = widgetsTrans.load(QString(":/syswnd/widgets.qm"));
	if (isLoad) {
		qApp->installTranslator(&widgetsTrans);
	} else {
		qDebug() << "load widgets.qm failed";
	}
	
	// 加载文字资源
	static QTranslator qtTranslator;
	if (qtTranslator.load(QString(":/font/fml_zh.qm")))
	{
		if (!qApp->installTranslator(&qtTranslator))
		{
			qDebug() << "install translator failed";
			return false;
		}
	}

	// qcss 加载
	QFile file(qutil::skin("sc.css"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QString str = file.readAll();
		qApp->setStyleSheet(str);
		file.close();
	}
	else
	{
		qDebug() << "open style file failed";
		return false;
	}

	qApp->setWindowIcon(QIcon(qutil::skin("logo")));

	// 拉起登录窗口
	ViewController::instance()->openwnd(LOGIN_WINDOW_ID, tr("Financial Market Leader"), false);
	return true;
}

void QmCrashEvents::onCrash(const std::string &dumpPath)
{
	//qutil::SendMailLog();
}