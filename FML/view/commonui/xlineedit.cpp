#include "xlineedit.h"
#include <QWidget>
#include <QStyle>
#include <QPropertyAnimation>

XLineEdit::XLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	connect(this, &QLineEdit::textChanged, [this] {
		this->restore();
	});
}

XLineEdit::~XLineEdit()
{
}

void XLineEdit::setError()
{
	if (this->property("property").toString() != "error")
	{
		this->setProperty("property", "error");
		this->style()->polish(this);
		/*QPropertyAnimation animation(this);
		animation.setStartValue(0);
		animation.setEndValue(100);
		animation.setDuration(1000);
		animation.setLoopCount(3);
		animation.setEasingCurve(QEasingCurve::Linear);
		animation.start();*/
	}
}

void XLineEdit::restore()
{
	if (this->property("property").toString() != "")
	{
		this->setProperty("property", "");
		this->style()->polish(this);
	}
}