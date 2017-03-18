#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPixmap>
// 边界
#define PADDING 2
#define PADDING2 15

namespace Ui {
	class basic;
}
class basicui : public QWidget
{
	Q_OBJECT

public:
	enum TitleStyle 
	{
		TS_NONE = 1,		// 无标题
		TS_CLOSE = 1<<1,	// 关闭
		TS_MAX =1<<2 ,		// 最大化,双击标题最大化，不显示最大化按钮！
		TS_MIN = 1<<3,		// 最小化
		TS_FUNC = 1<<4,		// 功能
		TS_LEFT =1<<5,		// 标题在左边
		TS_CENTER= 1<<6,	// 标题居中
		TS_LOGO =1<<7,		// LOGO
		TS_SKIN = 1<<8,		// 换肤
		TS_PUSHPIN = 1<<9,	// 图钉
	};

	explicit basicui(QWidget *parent, QWidget *contentWidget, const QString &wndid, 
		const QString &title, int titlestyle = TS_LOGO|TS_MAX|TS_CLOSE|TS_MIN|TS_LEFT);

	virtual ~basicui();

	QString id() const;									// 唯一标示窗口的ID
	void setTitleStyle(int titlestyle);					// 设置标题样式
	void setCloseIsHide(bool isHide);					// 点击关闭按钮进行隐藏窗口
	void setForbidMove(bool forbidMove);				// 禁止窗口移动
	QWidget *getContentWidget() const;

	
signals:
	void sigClose(const QString &id="", bool isExit=true);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void closeEvent(QCloseEvent *event);
	bool nativeEvent(const QByteArray & eventType, void * message, long * result);

	void setMaxRestoreVisible();

public Q_SLOTS:
	bool close();
	void min();
	virtual void max();
	void skin();
	void pushpin();

protected:
	// 鼠标所在窗口区域：上，下，左，右，左上，左下，右下，右上，中间
	enum Direction { UP=0, DOWN, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, MIDDLE };
	// 鼠标目前的位置转换对应窗口所在区域
	void transRegion(const QPoint &cursorGlobalPoint);

	bool m_blpressdown; //鼠标按下
	
protected:
	Ui::basic *m_ui;
	QString m_wndid;
	QPoint m_movepoint; //移动的距离 
	Direction m_dir;    //鼠标所在区域
	int m_titlestyle;   //标题功能按钮配置
	bool m_closeIsHide;	//点击关闭按钮隐藏窗口
	bool m_forbidMove;  //禁止窗口移动
	QWidget *m_contentWidget; // 主窗口区域
	QString m_title; //标题内容
};


class MainWidget : public basicui
{
	Q_OBJECT
public:
	MainWidget(QWidget *parent, QWidget *contentWidget, const QString &wndid,
		const QString &title, int titlestyle = TS_LOGO | TS_MAX | TS_CLOSE | TS_MIN | TS_LEFT);
	~MainWidget();
};

class SubWidget : public basicui
{
	Q_OBJECT
public:
	SubWidget(QWidget *parent, QWidget *contentWidget, const QString &wndid,
		const QString &title, int titlestyle = TS_LOGO | TS_MAX | TS_CLOSE | TS_MIN | TS_LEFT);
	~SubWidget();
protected:
	void moveEvent(QMoveEvent *event);
	void timerEvent(QTimerEvent *event);
signals:
	void sigWndMove(const QString &id, QWidget *pWnd);
private:
	int m_move;
	QPoint m_pos;
};