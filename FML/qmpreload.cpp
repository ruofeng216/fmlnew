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
	// ����Ĭ��������Դ
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
	
	// ����������Դ
	static QTranslator qtTranslator;
	if (qtTranslator.load(QString(":/font/fml_zh.qm")))
	{
		if (!qApp->installTranslator(&qtTranslator))
		{
			qDebug() << "install translator failed";
			return false;
		}
	}

	// qcss ����
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

	// �����¼����
	ViewController::instance()->openwnd(LOGIN_WINDOW_ID, tr("Financial Market Leader"), false);
	return true;
}

void QmCrashEvents::onCrash(const std::string &dumpPath)
{
	//qutil::SendMailLog();
}