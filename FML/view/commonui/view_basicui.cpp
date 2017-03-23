
#include "view_basicui.h"
#include "ui_basic.h"
#include <QDebug>
#include <QPainter>
#include <QBitmap>
#include <QMenu>
#include <qt_windows.h>
#include "util/util.h"
#include "util/skin_config.h"
#include "title_widget.h"
#include "idbresource.h"
#include "../view_controller.h"
#include "message_box_widget.h"
#include "util/fml_style.h"

basicui::basicui(QWidget *parent, QWidget *contentWidget, const QString &wndid, const QString &title, int titlestyle)
	: QWidget(parent)
	, m_wndid(wndid)
	, m_contentWidget(contentWidget)
	, m_ui(new Ui::basic)
	, m_titlestyle(titlestyle)
	, m_closeIsHide(false)
	, m_forbidMove(false)
	, m_dir(MIDDLE)
	, m_title(title)
{
	this->setObjectName(wndid);
	m_ui->setupUi(this);
	
	bool isNoTitle = m_titlestyle & TS_NONE;
	if (!isNoTitle)
	{
		TitleWidget *ptitle = new TitleWidget(this);
		bool isLogoVisible = (m_titlestyle & TS_LOGO);
		bool isCenter = (m_titlestyle & TS_CENTER);
		ptitle->init(isLogoVisible ? QPixmap(qutil::skin("logo.png")).scaled(20, 20) : QPixmap(), m_title, isCenter);
		m_ui->titleLeftLayout->addWidget(ptitle);
	}
	
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::Window);

	//可获取鼠标跟踪效果
	setMouseTracking(true);
	//设置窗口透明度
	//setWindowOpacity(0.8);  
	// 背景色自动填充
	setAutoFillBackground(true);  
	// 设置窗口销毁策略
	setAttribute(Qt::WA_DeleteOnClose);
	// 设置body
	m_ui->gl_body->addWidget(m_contentWidget);
	m_contentWidget->setMouseTracking(true);
	setWindowTitle(m_title);

	// 最小化信号槽
	connect(m_ui->btn_min, SIGNAL(clicked()), this, SLOT(min()));
	// 最大化/恢复信号槽
	connect(m_ui->btn_max, SIGNAL(clicked()), this, SLOT(max()));
	connect(m_ui->btn_restore, SIGNAL(clicked()), this, SLOT(max()));
	// 关闭信号槽
	connect(m_ui->btn_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(m_ui->btn_skin, SIGNAL(clicked()), this, SLOT(skin()));
	connect(m_ui->btn_pushpin, SIGNAL(clicked()), this, SLOT(pushpin()));
	connect(ViewController::instance(), SIGNAL(sigSkinChange()), this, SLOT(skinchange()));
	
	m_blpressdown = false;
	m_bPin = false;
	setTitleStyle(m_titlestyle);
	initBtns();
}

basicui::~basicui()
{
	if (m_ui) {
		delete m_ui;
		m_ui = NULL;
	}
}

QString basicui::id() const
{
	return m_wndid;
}

// 设置标题功能按钮
void basicui::setTitleStyle(int titlestyle)
{
	m_titlestyle = titlestyle;
	m_ui->btns->setContentsMargins(0, 0, 0, 20);
	m_ui->btn_func->setVisible(m_titlestyle & TS_FUNC);
	m_ui->btn_min->setVisible(m_titlestyle & TS_MIN);
	m_ui->btn_skin->setVisible(m_titlestyle & TS_SKIN);
	m_ui->btn_pushpin->setVisible(m_titlestyle & TS_PUSHPIN);
	setMaxRestoreVisible();
	m_ui->btn_close->setVisible(m_titlestyle & TS_CLOSE);
	bool isLogoVisible = (m_titlestyle & TS_LOGO);

	if (TS_NONE == titlestyle) {
		m_ui->title->hide();
	}

}

void basicui::setCloseIsHide(bool isHide)
{
	m_closeIsHide = isHide;
}

void basicui::setForbidMove(bool forbidMove)
{
	m_forbidMove = forbidMove;
}

QWidget *basicui::getContentWidget() const
{
	return m_contentWidget;
}

void basicui::initBtns()
{
	m_ui->btn_close->setPicName(qutil::skin("close.png"), qutil::skin("close-click-hover.png"), qutil::skin("close-click-hover.png"));
	m_ui->btn_max->setPicName(qutil::skin("maximized.png"), qutil::skin("maximized-click-hover.png"), qutil::skin("maximized-click-hover.png"));
	m_ui->btn_min->setPicName(qutil::skin("minimize.png"), qutil::skin("minimize-click-hover.png"), qutil::skin("minimize-click-hover.png"));
	m_ui->btn_pushpin->setPicName(qutil::skin("sub-pushpin.png"), qutil::skin("sub-pushpin-click-hover.png"), qutil::skin("sub-pushpin-click-hover.png"));
	m_ui->btn_restore->setPicName(qutil::skin("pluralized.png"), qutil::skin("pluralized-click-hover.png"), qutil::skin("pluralized-click-hover.png"));
	m_ui->btn_skin->setPicName(qutil::skin("skin.png"), qutil::skin("skin-click-hover.png"), qutil::skin("skin-click-hover.png"));
}

void basicui::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	// 窗口top圆角处理
	int radius = 5;
	QSize maskSize(this->width(), this->height() + radius);
	QBitmap mask(maskSize);
	QPainter maskPainter(&mask);
	maskPainter.setRenderHint(QPainter::Antialiasing);
	maskPainter.setRenderHint(QPainter::SmoothPixmapTransform);
	QColor color = QColor("#FFFFFF");
	maskPainter.fillRect(this->rect(), color);
	maskPainter.setBrush(QColor("#000000"));
	maskPainter.drawRoundedRect(QRect(QPoint(0, 0), maskSize), radius, radius);
	this->setMask(mask);
	
	QColor borderColor = m_ui->title->palette().color(QPalette::Background);
	painter.setPen(borderColor);
	painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
	QWidget::paintEvent(event);
}

//鼠标按下事件
void basicui::mousePressEvent(QMouseEvent *event)
{
	// 最大化后不支持窗口的移动拖拽
	if (this->isMaximized())
	{
		//鼠标事件向上抛
		event->ignore();
		return;
	}
	if(event->button() == Qt::LeftButton)
	{
		m_blpressdown = true;
		if (m_dir != MIDDLE)
		{
			// 捕捉鼠标
			this->mouseGrabber();
		} 
		else
		{
			//鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
			m_movepoint = event->globalPos() - this->pos();
		}
	}
	//鼠标事件向上抛
	event->ignore();
}
//鼠标释放事件
void basicui::mouseReleaseEvent(QMouseEvent *event)
{
	// 最大化后不支持窗口的移动拖拽
	if (this->isMaximized())
	{
		//鼠标事件向上抛
		event->ignore();
		return;
	}

	if(event->button() == Qt::LeftButton) {
		//设置鼠标为未被按下
		m_blpressdown = false;
		if(m_dir != MIDDLE) {
			// 鼠标释放
			this->releaseMouse();
			// 鼠标设置回手型
			this->setCursor(QCursor(Qt::ArrowCursor));
		}
	}
	//鼠标事件向上抛
	event->ignore();
}
//鼠标移动事件
void basicui::mouseMoveEvent(QMouseEvent *event)
{
	// 最大化后不支持窗口的移动拖拽
	if (this->isMaximized())
	{
		//鼠标事件向上抛
		event->ignore();
		return;
	}


	//若鼠标左键被按下
	if(m_blpressdown)
	{
		if (m_dir == MIDDLE && !m_forbidMove)
		{
			//移动主窗体位置
			this->move(event->globalPos() - m_movepoint);
		}
	}
	else
	{
		transRegion(event->globalPos());
	}
	// 事件上抛
	event->ignore();
}

//鼠标双击事件
void basicui::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::LeftButton && (m_titlestyle&TS_MAX)) //判断是否左键双击
	{
		QPoint gloPoint = event->globalPos();
		QPoint tl = mapToGlobal(m_ui->title->rect().topLeft());
		QPoint rb = mapToGlobal(m_ui->title->rect().bottomRight());
		QRect r = QRect(tl, rb);
		if (r.contains(gloPoint)) {
			max();
		}
	}
	// 事件上抛
	event->ignore();
}

void basicui::closeEvent(QCloseEvent *event)
{
	event->ignore();
	emit sigClose(m_wndid, !m_closeIsHide);
}

bool basicui::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	if (eventType != "windows_generic_MSG" || !message) {
		return false;
	}

	const int HIT_BORDER = 6;
	const MSG *msg=static_cast<MSG*>(message);
	if(msg->message == WM_NCHITTEST && (m_titlestyle & TS_MAX)) {
		int xPos = ((int)(short)LOWORD(msg->lParam)) - this->frameGeometry().x();
		int yPos = ((int)(short)HIWORD(msg->lParam)) - this->frameGeometry().y();
		if(this->childAt(xPos,yPos) == 0) {
			*result = HTCAPTION;
			return true;
		}

		if(xPos > 0 && xPos < HIT_BORDER && yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOPLEFT;
			return true;
		}
		if(xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0) && yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOPRIGHT;
			return true;
		}
		if(xPos > 0 && xPos < HIT_BORDER && yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOMLEFT;
			return true;
		}
		if(xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0) && yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOMRIGHT;
			return true;
		}
		if (xPos > 0 && xPos < HIT_BORDER) {
			*result = HTLEFT;
			return true;
		}
		if (xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0)) {
			*result = HTRIGHT;
			return true;
		}
		if (yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOP;
			return true;
		}
		if (yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOM;
			return true;
		}
		return QWidget::nativeEvent(eventType, message, result);
	}
	else if (msg->message == WM_LBUTTONUP) 
	{
		// 解决mouseReleaseEvent在特殊情况没有响应的问题，导致鼠标已经弹起了界面还跟着鼠标移动
		m_blpressdown = false;
	}
	return QWidget::nativeEvent(eventType, message, result);
}

void basicui::setMaxRestoreVisible()
{
	if (m_titlestyle & TS_MAX) {
		m_ui->btn_max->setVisible(!this->isMaximized());
		m_ui->btn_restore->setVisible(this->isMaximized());
	} else {
		m_ui->btn_max->setVisible(false);
		m_ui->btn_restore->setVisible(false);
	}
}

bool basicui::close()
{
	emit sigClose(m_wndid, !m_closeIsHide);
	qutil::cacheclean();
	return true;
}

void basicui::min()
{
	showMinimized();
	qutil::cacheclean();
}

void basicui::max()
{
	if (!this->isMaximized()) {
		showMaximized();

	} else {
		showNormal();
	}

	setMaxRestoreVisible();
}

void basicui::skinchange()
{
	initBtns();
}
void basicui::skin()
{
	QList<QPair<QString, QString>> skinList = SkinConfig::list();
	if (skinList.isEmpty()) {
		return;
	}

	QString curSkin = SkinConfig::current();
	QMenu skinMenu;
	skinMenu.setObjectName("SkinMenu");
	for (int i = 0; i < skinList.size(); i++) {
		QString skinName = skinList[i].first;
		QString skinDesc = skinList[i].second;
		QAction *action = new QAction(QIcon(skinName == curSkin ? qutil::skin("dropdown.png") : ""), skinDesc);
		connect(action, &QAction::triggered, [=]() {
			if (skinName != curSkin && MessageBoxWidget::Yes == ShowQuestionMessage(
				DefaultTitle, QString(tr("skin changed to %1")).arg(skinDesc), this)) {
				qutil::initSkin(skinName);
				emit ViewController::instance()->sigSkinChange();
			}
		});
		skinMenu.addAction(action);
	}

	QPoint pos = m_ui->btn_skin->pos();
	pos = this->mapToGlobal(pos);
	pos.setY(pos.y() + m_ui->btn_skin->height() + 3);
	skinMenu.exec(pos);
}

void basicui::pushpin()
{
	QWidget *widget = ViewController::instance()->getWidget(m_wndid);
	if (!widget) {
		return;
	}

	if (qutil::isWndTopMost((HWND)widget->winId())) {
		ViewController::instance()->cancelShowTopMost(m_wndid);
		m_bPin = false;
		m_ui->btn_pushpin->setPicName(qutil::skin("sub-pushpin.png"), qutil::skin("sub-pushpin-click-hover.png"), qutil::skin("sub-pushpin-click-hover.png"));
	} else {
		ViewController::instance()->showTopMost(m_wndid);
		m_bPin = true;
		m_ui->btn_pushpin->setPicName(qutil::skin("sub-pin.png"), qutil::skin("sub-pin-click-hover.png"), qutil::skin("sub-pin-click-hover.png"));
	}
}

// 鼠标目前的位置转换对应窗口所在区域
void basicui::transRegion(const QPoint &cursorGlobalPoint)
{
	// 获取窗体在屏幕上的位置区域，tl为topleft点，rb为rightbottom点
	QPoint tl = mapToGlobal(this->rect().topLeft());
	QPoint rb = mapToGlobal(this->rect().bottomRight());
	// 当前点横坐标与纵坐标
	int x = cursorGlobalPoint.x();
	int y = cursorGlobalPoint.y();


	// 区域分析
	if(tl.x() + PADDING >= x && tl.x() <= x && tl.y() + PADDING >= y && tl.y() <= y) {
		m_dir = LEFTTOP; // 左上角
	} else if (x >= rb.x() - PADDING && x <= rb.x() && y >= rb.y() - PADDING && y <= rb.y()) {
		m_dir = RIGHTBOTTOM; // 右下角
	} else if (x <= tl.x() + PADDING && x >= tl.x() && y >= rb.y() - PADDING && y <= rb.y()) {
		m_dir = LEFTBOTTOM; //左下角
	} else if (x <= rb.x() && x >= rb.x() - PADDING && y >= tl.y() && y <= tl.y() + PADDING) {
		m_dir = RIGHTTOP; // 右上角
	} else if (x <= tl.x() + PADDING && x >= tl.x() && y >= tl.y() + PADDING && y <= rb.y() - PADDING) {
		m_dir = LEFT; // 左边
	} else if (x <= rb.x() && x >= rb.x() - PADDING  && y >= tl.y() + PADDING && y <= rb.y() - PADDING) {
		m_dir = RIGHT; // 右边
	} else if (y >= tl.y() && y <= tl.y() + PADDING  && x >= tl.x() + PADDING && x <= rb.x() - PADDING){
		m_dir = UP; // 上边
	} else if (y <= rb.y() && y >= rb.y() - PADDING  && x >= tl.x() + PADDING && x <= rb.x() - PADDING) {
		m_dir = DOWN; // 下边
	}else if (this->childAt(x,y)!=NULL){
		m_dir = MIDDLE; // 默认
	} else {
		m_dir = MIDDLE; // 默认
	}
}


////////////////////////////////////////////////////////////////////
MainWidget::MainWidget(QWidget *parent, QWidget *contentWidget, const QString &wndid,
	const QString &title, int titlestyle)
	: basicui(parent, contentWidget, wndid, title, titlestyle|TS_NONE)
{
	this->setObjectName("MainWidget");
	m_titlestyle = titlestyle;
	MainTitle *ptitle = new MainTitle(this);
	bool isLogoVisible = (m_titlestyle & TS_LOGO);
	ptitle->init(isLogoVisible ? QPixmap(qutil::skin("logo.png")).scaled(20, 20) : QPixmap(), m_title);
	m_ui->titleLeftLayout->addWidget(ptitle);
}
MainWidget::~MainWidget()
{
}

///////////////////////////////////////////////////////////////////
SubWidget::SubWidget(QWidget *parent, QWidget *contentWidget, const QString &wndid,
	const QString &title, int titlestyle)
	: basicui(parent, contentWidget, wndid, title, titlestyle)
{
	m_contentWidget->show();
	m_move = startTimer(500);
	m_pos = QPoint(-9999, -9999);
	initBtns();
}
SubWidget::~SubWidget()
{
}

void SubWidget::initBtns()
{
	m_ui->btn_close->setPicName(qutil::skin("sub-close.png"), qutil::skin("sub-close-click-hover.png"), qutil::skin("sub-close-click-hover.png"));
	m_ui->btn_max->setPicName(qutil::skin("sub-maximized.png"), qutil::skin("sub-maximized-click-hover.png"), qutil::skin("sub-maximized-click-hover.png"));
	if (m_bPin)
		m_ui->btn_pushpin->setPicName(qutil::skin("sub-pin.png"), qutil::skin("sub-pin-click-hover.png"), qutil::skin("sub-pin-click-hover.png"));
	else
		m_ui->btn_pushpin->setPicName(qutil::skin("sub-pushpin.png"), qutil::skin("sub-pushpin-click-hover.png"), qutil::skin("sub-pushpin-click-hover.png"));
	m_ui->btn_restore->setPicName(qutil::skin("sub-pluralized.png"), qutil::skin("sub-pluralized-click-hover.png"), qutil::skin("sub-pluralized-click-hover.png"));
	m_ui->btn_min->setPicName(qutil::skin("sub-minimize.png"), qutil::skin("sub-minimize-click-hover.png"), qutil::skin("sub-minimize-click-hover.png"));
	m_ui->btns->setContentsMargins(10, 8, 10, 8);
	m_ui->btns->setSpacing(8);
}

void SubWidget::moveEvent(QMoveEvent *event)
{
	m_pos = pos();
	QWidget::moveEvent(event);
}
void SubWidget::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_move)
	{
		if (pos() == m_pos)
		{
			m_pos = QPoint(-9999, -9999);
			emit sigWndMove(m_wndid, m_contentWidget);
		}
	}
	QWidget::timerEvent(event);
}