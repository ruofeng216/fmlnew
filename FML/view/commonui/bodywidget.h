#pragma once

#include <QWidget>

template <class T>
class CAction
{
public:
	CAction() {}
	virtual ~CAction() {}
	void setCurrentData(const T& data) {  m_data = data;  }
	const T& getCurrentData() const { return m_data;  }
	bool isKeyModify(const T &newVal) { return getKey(newVal) != getKey(m_data); };
	virtual QString getKey(const T &newVal) const = 0;
	virtual bool isEqual(const T &newVal) = 0;
private:
	T m_data;
};

class BodyWidget : public QWidget
{
	Q_OBJECT

public:
	BodyWidget(QWidget *parent = NULL);
	virtual ~BodyWidget();

	
public slots:
	virtual void slotSkinChange() = 0;
};

// 临时的，为了构造BodyWidget对象
class EmptyWidget : public BodyWidget
{
	Q_OBJECT
public:
	EmptyWidget(QWidget *parent = NULL) : BodyWidget(parent) {}

public slots:
	void slotSkinChange() {}
};