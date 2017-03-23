#pragma once

#include <QWidget>
#include <QComboBox>

class QCompleter;
class SpeedSearch : public QWidget
{
	Q_OBJECT
public:
	explicit SpeedSearch(QWidget *parent = 0);
	void initData(const QStringList &strList);
	const QComboBox *getCombox() const;

signals:
	void sigItemSelected(const QString &name);

public slots:
	void slotActivated(const QString &str);

protected:
	void showEvent(QShowEvent *event);

private:
	QComboBox *m_comboBox;
	QCompleter *m_completer;
};