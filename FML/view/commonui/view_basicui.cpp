
#include "view_basicui.h"
#include "ui_basic.h"
#include <QDebug>
#include <QPainter>
#include "util/util.h"
#include <QBitmap>
#include <qt_windows.h>
#include "title_widget.h"
#include "idbresource.h"

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
		ptitle->init(isLogoVisible ? QPixmap(qutil::skin("logo")) : QPixmap(), m_title, isCenter);
		m_ui->horizontalLayout_2->addWidget(ptitle);
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
	
	m_ui->btn_min->init(IDBResourceNS::BASICUI_TITLE_MIN);
	m_ui->btn_max->init(IDBResourceNS::BASICUI_TITLE_MAX);
	m_ui->btn_close->init(IDBResourceNS::BASICUI_TITLE_CLOSE);
	m_ui->btn_restore->init(IDBResourceNS::BASICUI_TITLE_RESTORE);
	m_ui->btn_func->init(IDBResourceNS::SINGLESESSION_MENU);

	// 最小化信号槽
	connect(m_ui->btn_min, SIGNAL(clicked()), this, SLOT(min()));
	// 最大化/恢复信号槽
	connect(m_ui->btn_max, SIGNAL(clicked()), this, SLOT(max()));
	connect(m_ui->btn_restore, SIGNAL(clicked()), this, SLOT(max()));
	// 关闭信号槽
	connect(m_ui->btn_close, SIGNAL(clicked()), this, SLOT(close()));
	
	m_blpressdown = false;
	setTitleStyle(m_titlestyle);

	m_bgImage.load(qutil::skin("bg_main"));
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
	m_ui->btn_func->setVisible(m_titlestyle & TS_FUNC);
	m_ui->btn_min->setVisible(m_titlestyle & TS_MIN);
	setMaxRestoreVisible();
	m_ui->btn_close->setVisible(m_titlestyle & TS_CLOSE);
	bool isLogoVisible = (m_titlestyle & TS_LOGO);

	if (m_titlestyle != basicui::TS_CENTER) {
		m_ui->horizontalLayout->setContentsMargins(isLogoVisible ? 8 : 18, 0, 15, 3); // 再加上边框2px
	}

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
void basicui::setBackground(const QPixmap &bg)
{
	m_bgImage = bg;
}

QWidget *basicui::getContentWidget() const
{
	return m_contentWidget;
}

void basicui::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (TS_NONE == m_titlestyle) {
		painter.drawPixmap(0, 0, this->width(), this->height(), m_bgImage.copy(0, m_bgImage.height()-1, m_bgImage.width(), 1));
		return;
	}
	// 原图
	painter.drawPixmap(0, 0, qMin(width(), m_bgImage.width()), qMin(height(), m_bgImage.height()), m_bgImage);
	// 横向填充 图片宽度
	if (m_bgImage.width() < width())
	{
		painter.drawPixmap(m_bgImage.width()/2, 0, width()- m_bgImage.width(), m_bgImage.height(), m_bgImage.copy(m_bgImage.width()/2, 0, 1, m_bgImage.height()));
		painter.drawPixmap(width()- m_bgImage.width()/2-1, 0, m_bgImage.width()/2+1, m_bgImage.height(), m_bgImage.copy(m_bgImage.width()/2, 0, m_bgImage.width()/2+1, m_bgImage.height()));
	}
	// 纵向填充 图片长度
	if (m_bgImage.height() < height())
	{
		painter.drawPixmap(1, m_bgImage.height()-1, width()-2, height()- m_bgImage.height(), m_bgImage.copy(m_bgImage.width()/2, m_bgImage.height()-2, 1, 1));
		painter.drawPixmap(0, m_bgImage.height()-1, 1, height()- m_bgImage.height(), m_bgImage.copy(0, m_bgImage.height()-1, 1, 1));
		painter.drawPixmap(width()-1, m_bgImage.height()-1, 1, height()- m_bgImage.height(), m_bgImage.copy(0, m_bgImage.height()-1, 1, 1));
		painter.drawPixmap(0, height()-1, width(), 1, m_bgImage.copy(0, m_bgImage.height()-1, 1, 1));
	}

	// 窗口top圆角处理
	int radius = 5;
	QSize maskSize(this->size().width(), this->size().height() + radius);
	QBitmap mask(maskSize);
	QPainter maskPainter(&mask);
	maskPainter.setRenderHint(QPainter::Antialiasing);
	maskPainter.setRenderHint(QPainter::SmoothPixmapTransform);
	QColor color=QColor("#FFFFFF");
	maskPainter.fillRect(this->rect(), color);
	maskPainter.setBrush(QColor("#000000"));
	maskPainter.drawRoundedRect(QRect(QPoint(0, 0), maskSize), radius, radius);
	this->setMask(mask);
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
	if(msg->message == WM_NCHITTEST) {
		int xPos = ((int)(short)LOWORD(msg->lParam)) - this->frameGeometry().x();
		int yPos = ((int)(short)HIWORD(msg->lParam)) - this->frameGeometry().y();
		if(this->childAt(xPos,yPos) == 0) {
			*result = HTCAPTION;
			return true;
		}
		if(xPos > 0 && xPos < HIT_BORDER) {
			*result = HTLEFT;
			return true;
		}
		if(xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0)) {
			*result = HTRIGHT;
			return true;
		}
		if(yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOP;
			return true;
		}
		if(yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOM;
			return true;
		}
		if(xPos > 0 && xPos < HIT_BORDER && yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOPLEFT;
			return true;
		}
		if(xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0) && yPos > 0 && yPos < HIT_BORDER) {
			*result = HTTOPRIGHT;
		}
		if(xPos > 0 && xPos < HIT_BORDER && yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOMLEFT;
			return true;
		}
		if(xPos > (this->width() - HIT_BORDER) && xPos < (this->width() - 0) && yPos > (this->height() - HIT_BORDER) && yPos < (this->height() - 0)) {
			*result = HTBOTTOMRIGHT;
			return true;
		}
		return false;
	}
	else if (msg->message == WM_LBUTTONUP) 
	{
		// 解决mouseReleaseEvent在特殊情况没有响应的问题，导致鼠标已经弹起了界面还跟着鼠标移动
		m_blpressdown = false;
	}
	return false;
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
	m_titlestyle = titlestyle;
	MainTitle *ptitle = new MainTitle(this);
	bool isLogoVisible = (m_titlestyle & TS_LOGO);
	ptitle->init(isLogoVisible ? QPixmap(qutil::skin("logo")) : QPixmap(), m_title);
	m_ui->horizontalLayout_2->addWidget(ptitle);
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
	m_pos = pos();
}
SubWidget::~SubWidget()
{
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