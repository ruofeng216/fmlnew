#pragma once

#include <QWidget>
#include "drop_widget.h"
#include "winparameter.h"

namespace Ui {
	class MessageBoxWidget;
}

class MessageBoxWidget : public DropWidget
{
	Q_OBJECT
public:
	enum IconType
	{
		Question,
		Information,
		Warning,
		Critical,
	};

	enum ButtonType
	{
		Ok = 1 << 0,
		Cancel = 1 << 1,
		OkCancel = Ok | Cancel,
	};

	enum Result
	{
		Yes,
		No
	};

	explicit MessageBoxWidget(QWidget *parent = 0);
	~MessageBoxWidget();

	void setIconType(IconType type);
	IconType getIconType() const;

	void setButtonType(ButtonType type);
	ButtonType getButtonType() const;

	void setContent(const QString &content);
	QString getContent() const;

	Result getResult() const;

signals:
	void sigOk();
	void sigCancel();
	
private slots:
	void onOk();
	void onCancel();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	Ui::MessageBoxWidget *ui;
	WinParameter m_allowSetForeground;
	IconType m_iconType;
	ButtonType m_buttonType;
	QString m_title;
	QString m_content;
	Result m_result;
};

const QString DefaultTitle(QStringLiteral("ÌבÊ¾"));
void ShowWarnMessage(const QString &title, const QString &content, QWidget *parent=NULL);
void ShowErrorMessage(const QString &title, const QString &content, QWidget *parent=NULL);
void ShowSuccessMessage(const QString &title, const QString &content, QWidget *parent=NULL);
MessageBoxWidget::Result ShowQuestionMessage(const QString &title, const QString &content, QWidget *parent=NULL);

class ReloginMessageBoxWidget : public DropWidget
{
	Q_OBJECT
public:
	ReloginMessageBoxWidget(QWidget *parent = 0);
};

bool ShowReloginMessage(QWidget *parent=NULL);