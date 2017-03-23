#pragma once

#include <QWidget>

class QComboBox;
class QCompleter;
class SpeedSearch : public QWidget
{
	Q_OBJECT
public:
	explicit SpeedSearch(QWidget *parent = 0);
	void initData(const QStringList &strList);

signals:
	void sigItemSelected(const QString &name);

public slots:
	void slotActivated(const QString &str);

protected:
	void showEvent(QShowEvent *event);
	bool eventFilter(QObject *watched, QEvent *event);

private:
	QComboBox *m_comboBox;
	QCompleter *m_completer;
};