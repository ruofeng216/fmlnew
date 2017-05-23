#pragma once

#include <QComboBox>
class QWidget;
class QPropertyAnimation;
class XComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(int warning READ warning WRITE setWarning)
public:
	XComboBox(QWidget *parent);
	~XComboBox();

	void setError(const QString &msg);

	void restore();
private:
	inline int warning() { return m_nWarning; }
	void setWarning(int);
private:
	QPropertyAnimation *m_pAnimation;
	int m_nWarning;
protected:
	//bool eventFilter(QObject* obj, QEvent* evt);
};
