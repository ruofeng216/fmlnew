#include "idbimagelabel.h"

IDBImageLabel::IDBImageLabel(const IDBIconPosition &position, QWidget *parent) :
    QLabel(parent)
{
    _position = position;
    setPixmap(IDBResource::icon(_position,0));
    setSizeIncrement(QSize(_position.width,_position.height));
    setFixedSize(_position.width,_position.height);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_NoSystemBackground);
}
