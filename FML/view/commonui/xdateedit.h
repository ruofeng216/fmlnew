#pragma once

#include <QDateEdit>


class QWidget;
class QPropertyAnimation;
class xDateEdit : public QDateEdit
{
	Q_OBJECT
	Q_PROPERTY(int warning READ warning WRITE setWarning)
public:
	xDateEdit(QWidget *parent);
	~xDateEdit();
	void setError(const QString &msg);

	void restore();

private:
	inline int warning() { return m_nWarning; }
	void setWarning(int);
private:
	QPropertyAnimation *m_pAnimation;
	int m_nWarning;
};
