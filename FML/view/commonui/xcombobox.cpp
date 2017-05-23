#include "xcombobox.h"
#include <QWidget>
#include <QStyle>
#include <QPropertyAnimation>
#include<QTreeView>
#include<QMouseEvent>
#include<QModelIndex>

XComboBox::XComboBox(QWidget *parent)
	: QComboBox(parent)
	, m_pAnimation(NULL)
	, m_nWarning(0)
{
	connect(this, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
		[=](const QString &text)
	{
		this->restore();
	});

	QTreeView* tv = new QTreeView(this);
	tv->setHeaderHidden(true);
	tv->setAlternatingRowColors(true);
	this->setView(tv);
	tv->viewport()->installEventFilter(this);
}

XComboBox::~XComboBox()
{
}
void XComboBox::setError(const QString &msg)
{
	if (this->property("property").toString() != "error")
	{
		if (!m_pAnimation)
		{
			m_pAnimation = new QPropertyAnimation(this, "warning");
			m_pAnimation->setStartValue(0);
			m_pAnimation->setEndValue(100);
			m_pAnimation->setDuration(1000);
			m_pAnimation->setLoopCount(3);
			m_pAnimation->setEasingCurve(QEasingCurve::Linear);
			connect(m_pAnimation, &QPropertyAnimation::finished, [this] {
				if (this->property("property").toString() != "error")
				{
					this->setProperty("property", "error");
					this->style()->polish(this);
				}
			});
		}
		m_pAnimation->start();
	}
	this->setToolTip(msg);
}

void XComboBox::restore()
{
	if (m_pAnimation && m_pAnimation->state() == QPropertyAnimation::Running)
		m_pAnimation->stop();
	if (this->property("property").toString() != "")
	{
		this->setProperty("property", "");
		this->style()->polish(this);
	}
	this->setToolTip("");
}

void XComboBox::setWarning(int val)
{
	if (val < 50 && this->property("property").toString() != "error")
	{
		this->setProperty("property", "error");
		this->style()->polish(this);
	}
	if (val > 50 && this->property("property").toString() != "")
	{
		this->setProperty("property", "");
		this->style()->polish(this);
	}
}

//bool XComboBox::eventFilter(QObject* obj, QEvent* evt) {
//
//	if (evt->type() == QEvent::MouseButtonRelease && obj == view()->viewport())
//	{
//		return true;
//	}
//
//	if (evt->type() == QEvent::MouseButtonDblClick) {
//		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(evt);
//		QModelIndex index = view()->indexAt(mouseEvent->pos());
//		this->setEditText(index.sibling(index.row(), 0).data().toString());
//		this->hidePopup();
//		return true;
//	}
//	return false;
//}