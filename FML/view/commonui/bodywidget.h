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
	virtual bool isKeyModify(const T &newVal) { return getKey(newVal) != getKey(m_data); };
	virtual QString getKey(const T &newVal) const = 0;
	bool isEqual(const T &newVal) { return newVal == m_data; }

	void setNotNullCtls(std::initializer_list<QLineEdit*> les);
	bool checkNull(std::initializer_list<QLineEdit*> les);
	void slotSetNotNull(QLineEdit* les);

	// 提交时，检查相关控件值是否合法。
	virtual bool checkValid(bool opr = true) = 0;

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
			if (le->property("property").toString() != "error")
			{
				le->setProperty("property", "error");
				le->style()->polish(le);
			}
			//if(nullNo==0)m_oldQLineEditBackgroundColor = le->palette().color(QPalette::Background).name();
			//le->setStyleSheet("background-color: #CD7054");
			nullNo++;
		}
		else {
			//QString colorStr = QString("background-color:%1").arg(m_oldQLineEditBackgroundColor);
			//le->setStyleSheet("");
		}
	}

	if (nullNo > 0) return true;
	else return false;
}

template<class T>
inline void CAction<T>::setNotNullCtls(std::initializer_list<QLineEdit*> les) {
	for (auto le : les) {
		QObject::connect(le, &QLineEdit::textChanged, [&le]() {
			if (le->property("property").toString() != "")
			{
				le->setProperty("property", "");
				le->style()->unpolish(le);
				le->style()->polish(le);
			}
		});
	}
}

template<class T>
inline void CAction<T>::slotSetNotNull(QLineEdit* le) {
	if (le->property("property").toString() != "")
	{
		le->setProperty("property", "");
		le->style()->unpolish(le);
		le->style()->polish(le);
	}
}
