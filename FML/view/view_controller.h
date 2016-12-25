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
	// ���촰�ڴ�ʱ��idΪTAKL_WINDOW_ID + QString::number(userid/roomid)
	void openwnd(const QString &id /* ����ID */, 
				 const QString &title = "" /* �������ƣ��������� */, 
				 bool bModality = false /* �Ƿ�ģ̬ */, 
				 const QVariant &valin = QVariant()/*��������*/, 
				 QVariant &valout = QVariant()/*��������*/);
	// �������򿪴���
	void openwndNonblocking(const QString& id /* ����ID */, 
							const QString& title = "" /* �������ƣ��������� */, 
							bool bModality = false /* �Ƿ�ģ̬ */, 
							const QVariant& valin = QVariant()/*��������*/,
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
	// �����ö�չʾ
	void showTop(const QString &wid);
	void showTopMost(const QString &wid);
	void cancelShowTopMost(const QString &wid);

	// ��ȡָ�����ڵĽ�ͼ
	QPixmap render(const QString &id);

	// �ж��������Ƿ����� true:hide
	bool isMainWidgetHide(const QString id=MAIN_WINDOW_ID);
	bool isMainWidgetMinimized(const QString id=MAIN_WINDOW_ID);
	bool isMainWidgetMaximized(const QString id=MAIN_WINDOW_ID);
	void setMainWidgetMinimum(QSize size,const QString id=MAIN_WINDOW_ID);
	void setMainWidgetMaximum(QSize size,const QString id=MAIN_WINDOW_ID);

	
signals:
	// DemoData
	void pushDemoData(const demoStruct &val);

	// ���ܵ���
	void sigGotoFunc(const QString &funcid);

public slots:
	void closewnd(const QString &id /* ����ID */, bool bdestory = true);
	
private slots:
	// �˳�
	void exitProgramme(bool btips = true);
	// ��½�˳�
	void slotloginexit(const QString &id = "", bool isExit = true);
	// ��Ӧ��̨���ͼ��ص�����
	void slotCallBackUI(const CMyBasePtr val);
	// ���ܵ���
	void slotGotoFunc(const QString &funcid);

private:
	ViewController(QObject *parent = 0);
	~ViewController();

	// ģ̬��ʾ����
	void showWndModality(const QString &id /* ����ID */, QVariant &valout = QVariant());

private:
	QMap<QString, QWidget*> m_widgets;
	// ���滺������
	QMap<QString, QVariant> m_UiCache;
	QMutex m_mutex;
};

