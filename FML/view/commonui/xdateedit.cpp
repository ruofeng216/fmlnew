#include "xdateedit.h"
#include <QStyle>
#include <QPropertyAnimation>

xDateEdit::xDateEdit(QWidget *parent)
	: QDateEdit(parent)
	, m_pAnimation(NULL)
	, m_nWarning(0)
{
	connect(this, &QDateEdit::dateChanged, [this] {
		this->restore();
	});
}

xDateEdit::~xDateEdit()
{
}

void xDateEdit::setError(const QString &msg)
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

void xDateEdit::restore()
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

void xDateEdit::setWarning(int val)
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