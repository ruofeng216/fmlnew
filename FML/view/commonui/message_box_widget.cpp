#include "message_box_widget.h"
#include "ui_message_box_widget.h"
#include "view_basicui.h"
#include "util/util.h"
#include "view/view_controller.h"
#include <QPointer>
#include <QPainter>
#include <QBitmap>
// 图标大小

MessageBoxWidget::MessageBoxWidget(QWidget *parent)
	: DropWidget(parent), ui(new Ui::MessageBoxWidget)
	, m_iconType(Warning)
	, m_buttonType(Ok)
{
	ui->setupUi(this);
	ui->pbOk->setDefault(true);
	ui->pbCancel->setDefault(false);
	connect(ui->pbOk, &QPushButton::clicked, this, &MessageBoxWidget::onOk);
	connect(ui->pbCancel, &QPushButton::clicked, this, &MessageBoxWidget::onCancel);
}

MessageBoxWidget::~MessageBoxWidget()
{
	delete ui;
}

void MessageBoxWidget::setIconType(IconType type)
{
	m_iconType = type;
	QPixmap pixmap;
	if (Question == type)
	{
		pixmap.load(qutil::skin("msgbox_confirm.png"));
	}
	else if (Information == type)
	{
		pixmap.load(qutil::skin("msgbox_success.png"));
	}
	else if (Warning == type)
	{
		pixmap.load(qutil::skin("msgbox_caution.png"));
	}
	else if (Critical == type)
	{
		pixmap.load(qutil::skin("msgbox_error.png"));
	}
	//pixmap=pixmap.scaled(s_iconSize,Qt::KeepAspectRatio);
	ui->labelIcon->setPixmap(pixmap);
	ui->labelIcon->setFixedSize(pixmap.size());
}

MessageBoxWidget::IconType MessageBoxWidget::getIconType() const
{
	return m_iconType;
}

void MessageBoxWidget::setButtonType(ButtonType type)
{
	m_buttonType = type;
	ui->pbOk->setVisible(Ok & type);
	ui->pbCancel->setVisible(Cancel & type);
}

MessageBoxWidget::ButtonType MessageBoxWidget::getButtonType() const
{
	return m_buttonType;
}

void MessageBoxWidget::setContent(const QString &content)
{
	m_content = content;
	ui->labelContent->setText(content);
	ui->labelContent->setTextFormat(Qt::PlainText);
}

QString MessageBoxWidget::getContent() const
{
	return m_content;
}

void MessageBoxWidget::onOk()
{
	done(Yes);
}

void MessageBoxWidget::onCancel()
{
	done(No);
}

void MessageBoxWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) {
		onCancel();
	}
	DropWidget::keyPressEvent(event);
}

void MessageBoxWidget::paintEvent(QPaintEvent * event)
{
	// 浅色皮肤border radius在样式表中不生效
	QPainter painter(this);
	int radius = 5;
	QSize maskSize(this->size().width(), this->size().height());
	QBitmap mask(maskSize);
	QPainter maskPainter(&mask);
	maskPainter.setRenderHint(QPainter::Antialiasing);
	maskPainter.setRenderHint(QPainter::SmoothPixmapTransform);
	QColor color = QColor("#FFFFFF");
	maskPainter.fillRect(this->rect(), color);
	maskPainter.setBrush(QColor("#000000"));
	maskPainter.drawRoundedRect(QRect(QPoint(0, 0), maskSize), radius, radius);
	this->setMask(mask);
	QWidget::paintEvent(event);
}

static MessageBoxWidget::Result ShowMessage(const QString &title, const QString &content,
	MessageBoxWidget::IconType iconType, MessageBoxWidget::ButtonType buttonType, QWidget *parent = NULL)
{
	MessageBoxWidget msgBox(parent);
	msgBox.setAttribute(Qt::WA_ShowModal, true);
	msgBox.setIconType(iconType);
	msgBox.setButtonType(buttonType);
	msgBox.setWindowTitle(title);
	msgBox.setContent(content);
	msgBox.resize(290, 270);

	QPoint centerPos;
	QRect rect = ViewController::instance()->wndGlobalRect(MAIN_WINDOW_ID);
	if (!rect.isEmpty()) {
		centerPos.setX(rect.x() + rect.width() / 2);
		centerPos.setY(rect.y() + rect.height() / 2);
	}

	QWidget *mainWidget = ViewController::instance()->getWidget(MAIN_WINDOW_ID);
	if (parent && parent != mainWidget) {
		QWidget *parentWidget = parent->parentWidget();
		QPoint parentGlobalPos;
		if (parentWidget) {
			parentGlobalPos = parentWidget->mapToGlobal(parent->pos());
		} else {
			parentGlobalPos = parent->pos();
		}
		centerPos.setX(parentGlobalPos.x() + parent->width() / 2 - msgBox.width() / 2);
		centerPos.setY(parentGlobalPos.y() + parent->height() / 2 - msgBox.height() / 2);
		msgBox.move(centerPos);
	} else if (!centerPos.isNull()) {
		centerPos.setX(centerPos.x() - msgBox.width() / 2);
		centerPos.setY(centerPos.y() - msgBox.height() / 2);
		msgBox.move(centerPos);
	}
	int result = msgBox.exec();
	return (MessageBoxWidget::Result)result;
}

void ShowWarnMessage(const QString &title, const QString &content, QWidget *parent)
{
	ShowMessage(title, content, MessageBoxWidget::Warning, MessageBoxWidget::Ok, parent);
}

void ShowErrorMessage(const QString &title, const QString &content, QWidget *parent)
{
	ShowMessage(title, content, MessageBoxWidget::Critical, MessageBoxWidget::Ok, parent);
}

void ShowSuccessMessage(const QString &title, const QString &content, QWidget *parent)
{
	ShowMessage(title, content, MessageBoxWidget::Information, MessageBoxWidget::Ok, parent);
}

MessageBoxWidget::Result ShowQuestionMessage(const QString &title, const QString &content, QWidget *parent)
{
	
	return ShowMessage(title, content, MessageBoxWidget::Question, MessageBoxWidget::OkCancel, parent);
		
}

ReloginMessageBoxWidget::ReloginMessageBoxWidget(QWidget *parent) 
	: DropWidget(parent)
{
	QLabel *firstLabel = new QLabel(tr("in other terminal login"), this);
	QLabel *secondLabel = new QLabel(tr("switch to the computer terminal"), this);
	firstLabel->setAlignment(Qt::AlignHCenter);
	secondLabel->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *labelLayout = new QVBoxLayout();
	labelLayout->setContentsMargins(0, 0, 0, 0);
	labelLayout->setSpacing(3);
	labelLayout->addWidget(firstLabel);
	labelLayout->addWidget(secondLabel);

	QPushButton *loginButton = new QPushButton(tr("login"), this);
	loginButton->setFixedSize(130, 40);
	loginButton->setDefault(true);
	connect(loginButton, &QPushButton::clicked, [this]() { accept(); close(); });

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setSpacing(35);
	mainLayout->setContentsMargins(55, 25, 55, 25);
	mainLayout->addLayout(labelLayout);
	mainLayout->addWidget(loginButton);
	this->setLayout(mainLayout);
	this->setFixedSize(240, 160);
}

bool ShowReloginMessage(QWidget *parent)
{
	ReloginMessageBoxWidget widget(parent);
	int retCode = widget.exec();
	return retCode ? true : false;
}


