#pragma once

#include <QWidget>

template <class T>
class CAction
{
public:
	CAction() {}
	~CAction() {}
	void setCurData(const T &val) { m_CurData = val; }
	const T &getCurData() const { return m_CurData; }
	bool isEqual(const T &val) { return m_CurData == val; }
	bool isKeyModify(const T &val) = 0;
private:
	T m_CurData;
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