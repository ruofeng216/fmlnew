#pragma once

#include <QLineEdit>
class QWidget;
class QPropertyAnimation;
class XLineEdit : public QLineEdit
{
	Q_OBJECT
		Q_PROPERTY(int warning READ warning WRITE setWarning)
public:
	XLineEdit(QWidget *parent);
	~XLineEdit();

	void setError(const QString &msg);

	void restore();

private:
	inline int warning() { return m_nWarning; }
	void setWarning(int);
private:
	QPropertyAnimation *m_pAnimation;
	int m_nWarning;
};
