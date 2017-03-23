#include "speed_search.h"
#include <QtWidgets>

SpeedSearch::SpeedSearch(QWidget *parent)
	: QWidget(parent)
	, m_completer(nullptr)
{
	m_comboBox = new QComboBox(this);
	m_comboBox->setEditable(true);
	m_comboBox->lineEdit()->setPlaceholderText(tr("speed search"));
	m_comboBox->view()->setAlternatingRowColors(true);
	connect(m_comboBox, SIGNAL(activated(QString)), this, SLOT(slotActivated(QString)));
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);
	vLayout->addWidget(m_comboBox);

	this->setFixedSize(150, 24);
}

void SpeedSearch::initData(const QStringList &strList)
{
	if (m_completer) {
		delete m_completer;
	}
	m_completer = new QCompleter(strList, this);
	
	m_completer->setFilterMode(Qt::MatchContains);
	m_comboBox->setCompleter(m_completer);
	m_comboBox->clear();
	m_comboBox->addItems(strList);
}

const QComboBox *SpeedSearch::getCombox() const
{
	return m_comboBox;
}

void SpeedSearch::slotActivated(const QString &str)
{
	qDebug() << str;
	emit sigItemSelected(str);
}

void SpeedSearch::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	m_comboBox->setCurrentText("");
	m_comboBox->setFocus();
}
