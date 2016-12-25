#pragma once

#include <QWidget>
#include <QPixmap>

class QPushButton;
class QLineEdit;
class QLabel;
class QMenu;
class TitleWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TitleWidget(QWidget *parent=NULL);
	~TitleWidget();

	void init(const QPixmap &logoPixmap, const QString &title, bool isCenter = false);
private:
	QLabel *m_logo;
	QLabel *m_title;
};

class  MainTitle : public QWidget
{
	Q_OBJECT
public:
	explicit MainTitle(QWidget *parent = NULL);
	~ MainTitle();
	void init(const QPixmap &logoPixmap, const QString &title);

private slots:
	void slotBook();

private:
	void setMenu(QMenu *menu, const QString funcid);
	
private:
	QLabel *m_logo;
	QLabel *m_title;
	QPushButton *m_book;
	QLineEdit *m_Search;
};
