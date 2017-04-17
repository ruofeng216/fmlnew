#pragma once

#include <QLineEdit>
class QWidget;
class XLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	XLineEdit(QWidget *parent);
	~XLineEdit();

	void setError();

	void restore();
};
