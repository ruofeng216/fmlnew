#ifndef IDBIMAGEBUTTON_H
#define IDBIMAGEBUTTON_H

#include "idbresource.h"
#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>

class IDBImageButton : public QPushButton
{
    Q_OBJECT
public:
	explicit IDBImageButton(const IDBIconPosition &position, QWidget *parent = 0,const bool &checkedByClick=true);
	explicit IDBImageButton(QWidget *parent = 0);

	void init(const IDBIconPosition &position,const bool &checkedByClick=true);

	void setDisableEnterLeaveEvent(bool val);
	void handleEnter();
	void handleLeave();

signals:

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void leaveEvent(QEvent *);
    void enterEvent(QEvent *);
    void changeEvent(QEvent *);

public slots:
    void toggledHandler(bool);

private:
    IDBIconPosition _position;
	bool m_disableEnterLeaveEvent;

    void setEnabledStyle();

};


#endif // IDBIMAGEBUTTON_H
