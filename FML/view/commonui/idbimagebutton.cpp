#include "idbimagebutton.h"
#include <QAction>

IDBImageButton::IDBImageButton(const IDBIconPosition &position, QWidget *parent,const bool &checkedByClick) :
    QPushButton(parent), m_disableEnterLeaveEvent(false) {
	init(position,checkedByClick);
}

IDBImageButton::IDBImageButton(QWidget *parent):QPushButton(parent), m_disableEnterLeaveEvent(false) {
}

void IDBImageButton::init(const IDBIconPosition &position,const bool &checkedByClick){
	_position = position;
	setText("");

	setFixedSize(_position.width,_position.height);
	setIconSize(QSize(_position.width,_position.height));
	setIcon(IDBResource::icon(_position,0));
	if(checkedByClick){
		connect(this,SIGNAL(toggled(bool)),this,SLOT(toggledHandler(bool)));
	}
	setFocusPolicy(Qt::NoFocus);
	setAttribute(Qt::WA_NoSystemBackground);
}

void IDBImageButton::setDisableEnterLeaveEvent(bool val)
{
	m_disableEnterLeaveEvent = val;
}

void IDBImageButton::handleEnter()
{
	int pos =0;
	if(isEnabled())
	{
		pos = isChecked()?5:1;
	}else{
		pos = isChecked()?7:3;
	}
	setIcon(IDBResource::icon(_position,pos));
}

void IDBImageButton::handleLeave()
{
	int pos =0;
	if(isEnabled())
	{
		pos = isChecked()?4:0;
	}else{
		pos = isChecked()?7:3;
	}
	setIcon(IDBResource::icon(_position,pos));
}

void IDBImageButton::mousePressEvent(QMouseEvent *e){
    int pos = 0;
    if(isEnabled())
    {
        pos = isChecked()?6:2;
    }else{
        pos = isChecked()?7:3;
    }
    setIcon(IDBResource::icon(_position,pos));
    QPushButton::mousePressEvent(e);
}

void IDBImageButton::mouseReleaseEvent(QMouseEvent *e){
	int pos =0;
	if(isEnabled())
	{
		if(this->rect().contains(e->pos())){
			pos = isChecked()?5:1;
		}else{
			pos = isChecked()?4:0;
		}
	}else{
		pos = isChecked()?7:3;
	}
	setIcon(IDBResource::icon(_position,pos));
    QPushButton::mouseReleaseEvent(e);
}

void IDBImageButton::leaveEvent(QEvent *e){
	if (!m_disableEnterLeaveEvent) {
		handleLeave();
	}
    QPushButton::leaveEvent(e);
}

void IDBImageButton::enterEvent(QEvent *e){
	if (!m_disableEnterLeaveEvent) {
		handleEnter();
	}
    QPushButton::enterEvent(e);
}

void IDBImageButton::toggledHandler(bool checked){
    int pos =0;
    if(isEnabled())
    {
        pos = checked?4:0;
    }else{
        pos = checked?7:3;
    }
    setIcon(IDBResource::icon(_position,pos));
}

void IDBImageButton::changeEvent(QEvent *e){

    if(e->type()==QEvent::EnabledChange||e->type()==QEvent::PaletteChange)
    {
        setEnabledStyle();
    }else{
        QPushButton::changeEvent(e);
    }
}

void IDBImageButton::setEnabledStyle()
{
    int pos =0;
    if(isEnabled())
    {
        pos = isChecked()?4:0;
    }else{
        pos = isChecked()?7:3;

    }
    setIcon(IDBResource::icon(_position,pos));
    setAttribute(Qt::WA_NoSystemBackground);
}

