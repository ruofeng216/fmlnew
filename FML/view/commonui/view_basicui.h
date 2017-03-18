#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPixmap>
// �߽�
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
		TS_NONE = 1,		// �ޱ���
		TS_CLOSE = 1<<1,	// �ر�
		TS_MAX =1<<2 ,		// ���,˫��������󻯣�����ʾ��󻯰�ť��
		TS_MIN = 1<<3,		// ��С��
		TS_FUNC = 1<<4,		// ����
		TS_LEFT =1<<5,		// ���������
		TS_CENTER= 1<<6,	// �������
		TS_LOGO =1<<7,		// LOGO
		TS_SKIN = 1<<8,		// ����
		TS_PUSHPIN = 1<<9,	// ͼ��
	};

	explicit basicui(QWidget *parent, QWidget *contentWidget, const QString &wndid, 
		const QString &title, int titlestyle = TS_LOGO|TS_MAX|TS_CLOSE|TS_MIN|TS_LEFT);

	virtual ~basicui();

	QString id() const;									// Ψһ��ʾ���ڵ�ID
	void setTitleStyle(int titlestyle);					// ���ñ�����ʽ
	void setCloseIsHide(bool isHide);					// ����رհ�ť�������ش���
	void setForbidMove(bool forbidMove);				// ��ֹ�����ƶ�
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
	// ������ڴ��������ϣ��£����ң����ϣ����£����£����ϣ��м�
	enum Direction { UP=0, DOWN, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, MIDDLE };
	// ���Ŀǰ��λ��ת����Ӧ������������
	void transRegion(const QPoint &cursorGlobalPoint);

	bool m_blpressdown; //��갴��
	
protected:
	Ui::basic *m_ui;
	QString m_wndid;
	QPoint m_movepoint; //�ƶ��ľ��� 
	Direction m_dir;    //�����������
	int m_titlestyle;   //���⹦�ܰ�ť����
	bool m_closeIsHide;	//����رհ�ť���ش���
	bool m_forbidMove;  //��ֹ�����ƶ�
	QWidget *m_contentWidget; // ����������
	QString m_title; //��������
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