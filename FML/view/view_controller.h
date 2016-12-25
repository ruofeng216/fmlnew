#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QMutex>
#include "winmodel.h"
#include "util/util.h"
#include "util/datatype.h"
 
class ViewController : public QObject
{
	Q_OBJECT
public:
	static ViewController *instance();

public:
	// 聊天窗口打开时，id为TAKL_WINDOW_ID + QString::number(userid/roomid)
	void openwnd(const QString &id /* 窗口ID */, 
				 const QString &title = "" /* 窗口名称，附属属性 */, 
				 bool bModality = false /* 是否模态 */, 
				 const QVariant &valin = QVariant()/*传入数据*/, 
				 QVariant &valout = QVariant()/*传出数据*/);
	// 非阻塞打开窗口
	void openwndNonblocking(const QString& id /* 窗口ID */, 
							const QString& title = "" /* 窗口名称，附属属性 */, 
							bool bModality = false /* 是否模态 */, 
							const QVariant& valin = QVariant()/*传入数据*/,
							int timeout = 0);
	QWidget* create(const QString &id, const QString &title, const QVariant &valin, const QVariant &valout);
	
	QWidget* getWidget(const QString &id);
	QString getIdFromContentWidget(const QWidget *contentWidget);

	void movewnd(const QString &id, const QPoint &pos);
	void movewndToCenter(const QString &id);
	void movewndToCenter(const QString &id, QWidget *toWidget);
	void resizewnd(const QString &id, const QSize &size);

	void alertwnd(const QString &id, bool isStart = true);

	QSize wndsize(const QString &id);
	QRect wndGlobalRect(const QString &id);
	QPoint wndpos(const QString &id);

	bool IsExist(const QString &id);

	void showwnd(const QString &id);
	void hidewnd(const QString &id);
	// 窗口置顶展示
	void showTop(const QString &wid);
	void showTopMost(const QString &wid);
	void cancelShowTopMost(const QString &wid);

	// 获取指定窗口的截图
	QPixmap render(const QString &id);

	// 判断主界面是否隐藏 true:hide
	bool isMainWidgetHide(const QString id=MAIN_WINDOW_ID);
	bool isMainWidgetMinimized(const QString id=MAIN_WINDOW_ID);
	bool isMainWidgetMaximized(const QString id=MAIN_WINDOW_ID);
	void setMainWidgetMinimum(QSize size,const QString id=MAIN_WINDOW_ID);
	void setMainWidgetMaximum(QSize size,const QString id=MAIN_WINDOW_ID);

	
signals:
	// DemoData
	void pushDemoData(const demoStruct &val);

	// 功能导航
	void sigGotoFunc(const QString &funcid);

public slots:
	void closewnd(const QString &id /* 窗口ID */, bool bdestory = true);
	
private slots:
	// 退出
	void exitProgramme(bool btips = true);
	// 登陆退出
	void slotloginexit(const QString &id = "", bool isExit = true);
	// 响应后台推送及回掉处理
	void slotCallBackUI(const CMyBasePtr val);
	// 功能导航
	void slotGotoFunc(const QString &funcid);

private:
	ViewController(QObject *parent = 0);
	~ViewController();

	// 模态显示窗口
	void showWndModality(const QString &id /* 窗口ID */, QVariant &valout = QVariant());

private:
	QMap<QString, QWidget*> m_widgets;
	// 界面缓存数据
	QMap<QString, QVariant> m_UiCache;
	QMutex m_mutex;
};

