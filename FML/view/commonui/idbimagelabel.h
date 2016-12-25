#ifndef IDBIMAGELABEL_H
#define IDBIMAGELABEL_H

#include "idbresource.h"
#include <QWidget>
#include <QLabel>

class IDBImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit IDBImageLabel(const IDBIconPosition &position, QWidget *parent = 0);
    ~IDBImageLabel(){}

private:
    IDBIconPosition _position;

signals:

public slots:

};

#endif // IDBIMAGELABEL_H
