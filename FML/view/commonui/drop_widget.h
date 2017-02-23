#pragma once

#include <QDialog>
#include <QMouseEvent>
#include <qmath.h>

class DropWidget : public QDialog
{
	Q_OBJECT
public:
	explicit DropWidget(QWidget *parent = 0);
	~DropWidget();

	void setTitle(const QString &title);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent( QPaintEvent * event);

private:
	QPoint m_movePoint; //�ƶ��ľ���
	bool m_mousePressed; //����������
	QString m_title;
};