#include "view_controller.h"
#include <QPointer>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QUuid>
#include <QThread>
#include <QVariantMap>
#include <QTimer>
#include <QEventLoop>
#include <QRect>

#include "viewsignalmanager/viewsignalmanager.h"
#include "controller/qcontrollermanager.h"
#include "commonui/view_basicui.h"
#include "winmodel.h"

#include "login/login.h"
#include "mainframe/fml.h"
#include "webview/webviewsever.h"
#include "view/commonui/message_box_widget.h"


ViewController::ViewController(QObject *parent)
	: QObject(parent)
{
	connect(qApp, &QCoreApplication::aboutToQuit, [this]{
		
	});
	connect(VIEWSIGNAL, &ViewSignalManager::sigExitProgramme, this, &ViewController::exitProgramme, Qt::QueuedConnection);

	connect(VIEWSIGNAL, &ViewSignalManager::callBackUI, this, &ViewController::slotCallBackUI, Qt::QueuedConnection);
	connect(this, &ViewController::sigGotoFunc, this, &ViewController::slotGotoFunc);
}

ViewController::~ViewController()
{
	
}

ViewController *ViewController::instance()
{
	static ViewController inst;
	return &inst;
}

void ViewController::openwnd(const QString &id, const QString &title, bool bModality, const QVariant &valin, QVariant &valout)
{
	if (m_widgets.contains(id)) {
		basicui *p = static_cast<basicui*>(m_widgets[id]);
		if (p) {
			p->getContentWidget()->setProperty("data", valin); // 设置初始化数据
		}

		if (bModality) {
			showWndModality(id, valout);
		} else {
			showTop(id);
		}
	} else { 
		basicui *pwnd = static_cast<basicui*>(create(id, title, valin, valout));
		if (pwnd) {
			if (bModality) {
				showWndModality(id, valout);
			} else {
				pwnd->show();
			}
		}
	}
	if (m_widgets.contains(id))
	{
		m_widgets[id]->raise();
		m_widgets[id]->activateWindow();
	}
}

void ViewController::openwndNonblocking(const QString& id, const QString& title, bool bModality, const QVariant& valin, int timeout)
{
	QTimer *p = new QTimer(this);
	p->setSingleShot(true);
	connect(p, &QTimer::timeout, [p, id, title, bModality, valin] {
		p->deleteLater();
		ViewController::instance()->openwnd(id, title, bModality, valin);
	});
	p->start(timeout);
}


QWidget* ViewController::create(const QString &id, const QString &title, const QVariant &valin, const QVariant &valout)
{
	basicui *pwnd = NULL;
	if (LOGIN_WINDOW_ID == id){ // 登录
		pwnd = new basicui(NULL, new login(), id, title, basicui::TS_CLOSE|basicui::TS_MIN|basicui::TS_LEFT|basicui::TS_LOGO);
		pwnd->resize(LOGIN_INIT_WIDTH, LOGIN_INIT_HEIGHT);
	} else if (MAIN_WINDOW_ID == id) { // 主窗口
		pwnd = new MainWidget(NULL, new FML(), id, title, basicui::TS_CLOSE | basicui::TS_MAX | basicui::TS_MIN | basicui::TS_LEFT | basicui::TS_LOGO);
		//pwnd->setCloseIsHide(true);
		//获取可用桌面大小
		QRect deskRect = QApplication::desktop()->availableGeometry();
		pwnd->resize(deskRect.width()*0.8>MAIN_INIT_WIDTH ? deskRect.width()*0.8 : MAIN_INIT_WIDTH,
			deskRect.height()*0.8>MAIN_INIT_HEIGHT ? deskRect.height()*0.8 : MAIN_INIT_HEIGHT);
	}

	if (pwnd) {
		m_widgets[id] = pwnd;
		// 设置初始化数据
		pwnd->getContentWidget()->setProperty("data", valin);
		if (LOGIN_WINDOW_ID == id || MAIN_WINDOW_ID == id) {
			connect(pwnd, &basicui::sigClose, this, &ViewController::slotloginexit, Qt::QueuedConnection);
		} else {
			connect(pwnd, &basicui::sigClose, this, &ViewController::closewnd);
		}
	}
	return pwnd;
}

QWidget* ViewController::getWidget(const QString &id)
{
	if (!id.isEmpty() && 
		m_widgets.contains(id)) {
		return m_widgets[id];
	}
	return NULL;
}

QString ViewController::getIdFromContentWidget(const QWidget *contentWidget)
{
	QMapIterator<QString, QWidget*> iter(m_widgets);
	while (iter.hasNext()) {
		iter.next();
		basicui *basicWidget = static_cast<basicui*>(iter.value());
		if (basicWidget && basicWidget->getContentWidget() == contentWidget) {
			return iter.key();
		}
	}
	return "";
}

void ViewController::movewnd(const QString &id, const QPoint &pos)
{
	if (m_widgets.contains(id))
	{
		QWidget* pwnd = m_widgets[id];
		pwnd->move(pos);
	}
}

void ViewController::movewndToCenter(const QString &id)
{
	movewndToCenter(id, NULL);
}

void ViewController::movewndToCenter(const QString &id, QWidget *toWidget)
{
	if (!m_widgets.contains(MAIN_WINDOW_ID)) {
		return;
	}

	if (m_widgets.contains(id)) {
		QWidget *mainWnd = m_widgets[MAIN_WINDOW_ID];
		if (toWidget) {
			mainWnd = toWidget;
		}

		QPoint mainPos = mainWnd->pos();
		QSize mainSize = mainWnd->size();
		QPoint mainCenter(mainPos.x() + mainSize.width() / 2, mainPos.y() + mainSize.height() / 2);

		QWidget *curWnd = m_widgets[id];
		QSize curSize = curWnd->size();
		QPoint curPos(mainCenter.x() - curSize.width() / 2, mainCenter.y() - curSize.height() / 2);

		curPos = qutil::GracePoint(curPos, curSize);
		curWnd->move(curPos);
	}
}

void ViewController::resizewnd(const QString &id, const QSize &size)
{
	if (m_widgets.contains(id))
	{
		QWidget* pwnd = m_widgets[id];
		pwnd->resize(size);
	}
}

void ViewController::alertwnd(const QString &id, bool isStart)
{
    if (m_widgets.contains(id))
	{
		if (isStart) {
			qutil::startAlert((HWND)m_widgets[id]->winId(), 1500, 500);
		} else {
			qutil::stopAlert((HWND)m_widgets[id]->winId());
		}
	}
}


QSize ViewController::wndsize(const QString &id)
{
	if (m_widgets.contains(id)) {
		return m_widgets[id]->size();
	}
	return QSize();
}

QRect ViewController::wndGlobalRect(const QString &id)
{
	QRect globalRect;
	if (m_widgets.contains(id)) {
		QWidget *wnd = m_widgets[id];
		globalRect.setX(wnd->pos().x());
		globalRect.setY(wnd->pos().y());
		globalRect.setWidth(wnd->width());
		globalRect.setHeight(wnd->height());
	}
	return globalRect;
}

void ViewController::showwnd(const QString &id)
{
	if (m_widgets.contains(id)) {
		basicui *p = static_cast<basicui*>(m_widgets[id]);
		p->show();
	}
}

void ViewController::hidewnd(const QString &id)
{
	if (m_widgets.contains(id)) {
		basicui *p = static_cast<basicui*>(m_widgets[id]);
		p->hide();
	}
}

// 窗口置顶展示
void ViewController::showTop(const QString &wid)
{
	if (m_widgets.contains(wid)) {
		if (m_widgets[wid]->isMinimized()) {
			qutil::showWnd((HWND)m_widgets[wid]->winId());
		}
		m_widgets[wid]->show();
		m_widgets[wid]->activateWindow();
		m_widgets[wid]->raise();
	}
}

void ViewController::showTopMost(const QString &wid)
{
	if (m_widgets.contains(wid)) {
		if (m_widgets[wid]->isMinimized()) {
			qutil::showWnd((HWND)m_widgets[wid]->winId());
		}
		qutil::showWndTopMost((HWND)m_widgets[wid]->winId());
	}
}

void ViewController::cancelShowTopMost( const QString &wid )
{
	if (m_widgets.contains(wid)) {
		qutil::cancelTopMost((HWND)m_widgets[wid]->winId());
	}
}

QPixmap ViewController::render(const QString &id)
{
	if (m_widgets.contains(id)) {
		QPixmap pixmap(m_widgets[id]->size());
		m_widgets[id]->render(&pixmap);
		return pixmap;
	}
	return QPixmap();
}

void ViewController::closewnd(const QString &id /* 窗口ID */, bool bdestory)
{
	QMutexLocker locker(&m_mutex);
	if (m_widgets.contains(id)) {
		basicui *p = static_cast<basicui*>(m_widgets[id]);
		if (bdestory) {
			m_widgets.remove(id);
			if (p->getContentWidget() && !p->getContentWidget()->property("data").isNull())
			{
				m_UiCache[id] = p->getContentWidget()->property("data");
			}
			p->deleteLater();
		} else {
			p->hide();
		}
	}
}

// 登陆退出
void ViewController::slotloginexit(const QString &id, bool isExit)
{
	emit ViewSignalManager::instance()->sigExitProgramme();
}

// 响应后台推送及回掉处理
void ViewController::slotCallBackUI(const CMyBasePtr val)
{
	if (!val) return;
	QString classname = val->getClassName();
	if (classname == CLASSNAME_DEMOSTRUCT)
	{
		demoStruct *s = static_cast<demoStruct *>(val.data());
		emit pushDemoData(*s);
	}
}

// 功能导航
void ViewController::slotGotoFunc(const QString &funcid)
{
	CFuncInfo cf;
	GLBSETCTL->getFuncInfo(funcid, cf);
	ShowWarnMessage(tr("func"), cf.getFuncName().getVal().toString(), NULL);
}


void ViewController::exitProgramme(bool btips)
{
	if (btips)
	{
	}

	// 删除所有信号槽
	disconnect(VIEWSIGNAL, 0, 0, 0);
	for (auto iter=m_widgets.begin(); iter != m_widgets.end(); ++iter) {
		QPointer<QWidget> p(*iter);
		if (!p.isNull()) {
			p->hide();
		}
	}
	QControllerManager::instance()->release(); // 删除控制层
	WEBVIEWSEVER->release();
	// 销毁所有窗口
	QMap<QString, QWidget*>::iterator iter = m_widgets.begin();
	for (; iter != m_widgets.end(); ++iter) {
		basicui *p = static_cast<basicui*>(iter.value());
		p->deleteLater();
	}
	qApp->quit();
}

// 模态显示窗口
void ViewController::showWndModality(const QString &id /* 窗口ID */, QVariant &valout)
{
	if (m_widgets.contains(id))
	{
		QEventLoop eventLoop;
		m_widgets[id]->setAttribute(Qt::WA_ShowModal, true);
		m_widgets[id]->showNormal();
		m_widgets[id]->activateWindow();

		connect((basicui *)m_widgets[id], &QObject::destroyed,  &eventLoop, &QEventLoop::quit, Qt::DirectConnection);
		eventLoop.exec();
		if (m_UiCache.contains(id))
		{
			valout = m_UiCache[id];
			m_UiCache.remove(id);
		}
	}
}

// 判断主界面是否隐藏 true:hide
bool ViewController::isMainWidgetHide(const QString id)
{
	if (m_widgets.contains(id) && m_widgets[id]->isHidden())
	{
		return true;
	}
	return false;
}

bool ViewController::isMainWidgetMinimized(const QString id)
{
	if (m_widgets.contains(id) && m_widgets[id]->isMinimized())
	{
		return true;
	}
	return false;
}

bool ViewController::isMainWidgetMaximized(const QString id)
{
	if (m_widgets.contains(id)) {
		return m_widgets[id]->isMaximized();
	}
	return false;
}

void ViewController::setMainWidgetMinimum(QSize size,const QString id)
{
	if (m_widgets.contains(id)) {
		m_widgets[id]->setMinimumSize(size);
	}
}

void ViewController::setMainWidgetMaximum(QSize size,const QString id)
{
	if (m_widgets.contains(id)) {
		m_widgets[id]->setMaximumSize(size);
	}
}

QPoint ViewController::wndpos( const QString &id )
{
	if (m_widgets.contains(id))
	{
		return m_widgets[id]->pos();
	}
	return QPoint(0,0);
}

bool ViewController::IsExist( const QString &id )
{
	if (m_widgets.contains(id))
	{
		return true;
	}
	return false;
}
