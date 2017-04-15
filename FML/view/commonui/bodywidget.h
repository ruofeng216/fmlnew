#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QDebug>

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

	bool checkNull(std::initializer_list<QLineEdit*> les);

private:
	T m_data;
	//all QLineEdits' color are the same
	//QString m_oldQLineEditBackgroundColor;
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

template<class T> 
inline bool CAction<T>::checkNull(std::initializer_list<QLineEdit*> les) {
	short nullNo = 0;
	for (auto le : les) {
		if (le->text().trimmed().isEmpty()) {
			//if(nullNo==0)m_oldQLineEditBackgroundColor = le->palette().color(QPalette::Background).name();
			le->setStyleSheet("background-color: #CD7054");
			nullNo++;
		}
		else {
			//QString colorStr = QString("background-color:%1").arg(m_oldQLineEditBackgroundColor);
			le->setStyleSheet("");
		}
	}

	if (nullNo > 0) return true;
	else return false;
}