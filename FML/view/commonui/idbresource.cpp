#include "idbresource.h"
#include <QDebug>
#include <QPainter>
#include "util/util.h"

IDBResource::IDBResource(void)
{
}

IDBResource::~IDBResource(void)
{
}

QMap<QString,QPixmap *> IDBResource::fullIcons = QMap<QString,QPixmap *>();
QMap<QString,QPixmap *> IDBResource::icons = QMap<QString,QPixmap *>();

QPixmap &IDBResource::icon(IDBIconPosition position,int column){
    QString url = QString(qutil::skin("%1x%2")).arg(position.width).arg(position.height);
    QString iconkey = QString("%1x%2_%3x%4").arg(position.width).arg(position.height).arg(position.row).arg(column);
    if(!fullIcons.contains(url)){
        fullIcons[url] = new QPixmap(url);
    }
    if(!icons.contains(iconkey)){
        icons[iconkey] = new QPixmap(position.width,position.height);
        *icons[iconkey] = fullIcons[url]->copy(column*position.width,position.row*position.height,position.width,position.height);
    }

    return *icons[iconkey];
}

void IDBResource::icon(const QList<IDBIconPosition> &positions,QPixmap &result){
    result.fill(Qt::transparent);
    QPainter painter(&result);
    foreach (IDBIconPosition position, positions) {
        painter.drawPixmap(position.x,position.y,position.width,position.height,icon(position,position.column));
    }
}

void IDBResource::icon(const IDBIconPosition &position,QPixmap &result){
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.drawPixmap(position.x,position.y,position.width,position.height,icon(position,position.column));
}

void IDBResource::flushCache(){
    foreach (QString key, fullIcons.keys()) {
        delete fullIcons[key];
        fullIcons.remove(key);
    }
    foreach (QString key, icons.keys()) {
        delete icons[key];
        icons.remove(key);
    }
}
