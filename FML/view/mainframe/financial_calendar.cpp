#include "financial_calendar.h"
#include "util/datatype.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include <QCalendarWidget>
#include "util/util.h"
#include "view/commonui/message_box_widget.h"

FinancialCalendar::FinancialCalendar(QWidget *parent)
	: BodyWidget(parent)
	, m_pGoodsModel(NULL)
{
	ui.setupUi(this);
	connect(ui.pushButton_add, &QPushButton::clicked, this, &FinancialCalendar::addHoliday);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &FinancialCalendar::modifyHoliday);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &FinancialCalendar::delHoliday);
	init();
	slotSkinChange();
}

FinancialCalendar::~FinancialCalendar()
{
}

QString FinancialCalendar::getKey(const CFinancialCalendar &newVal) const
{
	return newVal.getDate();
}

// 提交时，检查相关控件值是否合法。
bool FinancialCalendar::checkValid()
{
	bool bValid = true;
	if (ui.dateEdit->date().year() != ui.lineEdit_year->text().toInt())
	{
		ui.lineEdit_year->setError(tr("the year must equal to the date-year!"));
		bValid = false;
	}
	else
	{
		ui.lineEdit_year->restore();
	}
	return bValid;
}

void FinancialCalendar::init()
{
	{
		ui.treeView->setAlternatingRowColors(true);
		initDateView();
		connect(ui.treeView, &QTreeView::clicked, [this](const QModelIndex &index) {
			QVariant s = index.sibling(index.row(),0).data();
			if (s.isValid())
			{
				ui.dateEdit->setDate(QDate::fromString(s.toString(),"yyyy-MM-dd"));
				QStringList slist = s.toString().split("-");
				if (slist.length() > 0)
					ui.lineEdit_year->setText(slist[0]);
				QVariant sDayType = index.sibling(index.row(), 1).data();
				if (sDayType.isValid())
					ui.comboBox->setCurrentText(sDayType.toString());
				QVariant sDayinfo = index.sibling(index.row(), 2).data();
				if (sDayinfo.isValid())
					ui.textEdit_def->setText(sDayinfo.toString());

				setCurrentData(getViewData());
			}
		});
	}
	{
		ui.dateEdit->setDate(QDate::currentDate());
	}
	{
		ui.comboBox->addItems(QStringList() << tr("holiday") << tr("workday"));
		ui.comboBox->view()->setAlternatingRowColors(true);
	}

	connect(ui.dateEdit, &QDateTimeEdit::dateChanged, [this](const QDate &date) {
		ui.lineEdit_year->setText(QString::number(date.year()));
	});
}

void FinancialCalendar::slotSkinChange()
{
	ui.pushButton_add->setText(tr("add"));
	ui.pushButton_add->setIcon(QIcon(qutil::skin("add-click-hover.png")));
	ui.pushButton_modify->setText(tr("modify"));
	ui.pushButton_modify->setIcon(QIcon(qutil::skin("revise-click-hover.png")));
	ui.pushButton_delete->setText(tr("delete"));
	ui.pushButton_delete->setIcon(QIcon(qutil::skin("delete-click-hover.png")));
	qutil::setWeekendStyle(ui.dateEdit);
}

void FinancialCalendar::addHoliday()
{
	if (!checkValid()) return;
	CFinancialCalendar fc = getViewData();
	if (PARASETCTL->isExistFinancialCalendar(fc))
		ShowWarnMessage(tr("add"), tr("the time is existing."), this);
	else
	{
		if (PARASETCTL->setFinancialCalendar(fc))
		{
			ShowSuccessMessage(tr("add"), tr("add success."), this);
			// 同步
			initDateView();
			expand(fc.getYear());
		}
		else
			ShowWarnMessage(tr("add"), tr("add fail."), this);
	}
}
void FinancialCalendar::modifyHoliday()
{
	if (!checkValid()) return;

	if (getCurrentData().getDate() == 0 || getCurrentData().getYear() == 0) {
		ShowWarnMessage(tr("modify"), tr("Please confirm the date you want to modify first"), this);
		return;
	}

	CFinancialCalendar fc = getViewData();
	if (this->isKeyModify(fc)) {
		ShowWarnMessage(tr("modify"), tr("date can not be modified!"), this);
		return;
	}

	if (this->isEqual(fc)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}

	if (PARASETCTL->setFinancialCalendar(fc))
	{
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		initDateView();
		expand(fc.getYear());
	} else {
		ShowWarnMessage(tr("modify"), tr("modify fail."), this);
	}
}
void FinancialCalendar::delHoliday()
{
	if (!checkValid()) return;

	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this))
	{
		CFinancialCalendar fc = getViewData();
		if (PARASETCTL->isExistFinancialCalendar(fc))
		{
			if (!PARASETCTL->removeFinancialCalendar(fc.getDate()))
			{
				ShowWarnMessage(tr("delete"), tr("delete fail."), this);
				return;
			}
		}
		else
		{
			QString errMsg = tr("No record in database.").arg(QDate::fromJulianDay(fc.getDate()).toString("yyyy-MM-dd"));
			ShowWarnMessage(tr("delete"), errMsg, this);
			return;
		}
		ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		setViewData(CFinancialCalendar());
		// 同步
		initDateView();
		expand(fc.getYear());
	}
}

void FinancialCalendar::initDateView()
{
	CFinancialCalendar oldVal = getViewData();
	if (m_pGoodsModel) m_pGoodsModel->clear();
	QStringList treeHeader;
	treeHeader << tr("hyear") << tr("holiday") << tr("holidayinfo");
	if (!m_pGoodsModel) m_pGoodsModel = new QStandardItemModel(0, treeHeader.size(), this);
	m_pGoodsModel->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_pGoodsModel->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_pGoodsModel);
	ui.treeView->header()->setDefaultSectionSize(130);
	QMap<int, CFinancialCalendar> val = PARASETCTL->getFinancialCalendar();
	QList<QStandardItem *> items;
	for (QMap<int, CFinancialCalendar>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		if (items.isEmpty() ||
			items.back()->text().toInt() != itor->getYear())
		{
			items << new QStandardItem(QString::number(itor->getYear()));
			m_pGoodsModel->appendRow(items.back());
		}

		QList<QStandardItem *> childItems;
		QStandardItem *itemDate = new QStandardItem(QDate::fromJulianDay(itor->getDate()).toString("yyyy-MM-dd"));
		QStandardItem *itemHolidayType = new QStandardItem(itor->getHolidayTypeStr());
		QStandardItem *itemHolidayinfo = new QStandardItem(itor->getHolidayinfo());
		childItems.push_back(itemDate);
		childItems.push_back(itemHolidayType);
		childItems.push_back(itemHolidayinfo);
		itemDate->setToolTip(QDate::fromJulianDay(itor->getDate()).toString("yyyy-MM-dd"));
		itemHolidayinfo->setToolTip(qutil::splitTooltip(itor->getHolidayinfo(),200));
		items.back()->appendRow(childItems);
	}
	if (!val.isEmpty()) {
		expand(QDate::fromJulianDay(val[val.keys().last()].getDate()).year());
	}
	setViewData(oldVal);
}

void FinancialCalendar::expand(int y)
{
	ui.treeView->expandAll();
	return;
	// fixme 有死循环
	QList<QStandardItem*> lst = m_pGoodsModel->findItems(QString::number(y));
	if (lst.isEmpty())
	{
		QMap<int, CFinancialCalendar> val = PARASETCTL->getFinancialCalendar();
		if (!val.isEmpty()) {
			expand(QDate::fromJulianDay(val[val.keys().last()].getDate()).year());
		}
		return;
	}
	for each (QStandardItem* var in lst)
	{
		ui.treeView->expand(var->index());
	}
}

CFinancialCalendar FinancialCalendar::getViewData()
{
	CFinancialCalendar result;
	result.setDate(ui.dateEdit->date().toJulianDay());
	result.setYear(ui.lineEdit_year->text().toInt());
	result.setHolidayType(CFinancialCalendar::getHolidayType(ui.comboBox->currentText()));
	result.setHolidayinfo(ui.textEdit_def->toPlainText().trimmed());
	return result;
}

void FinancialCalendar::setViewData(const CFinancialCalendar &val)
{
	ui.dateEdit->setDate(val.getDate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getDate()));
	ui.lineEdit_year->setText(val.getYear() == 0 ? QString::number(QDate::currentDate().year()) : QString::number(val.getYear()));
	ui.comboBox->setCurrentText(val.getHolidayTypeStr());
	ui.textEdit_def->setPlainText(val.getHolidayinfo());
	setCurrentData(getViewData());
}
