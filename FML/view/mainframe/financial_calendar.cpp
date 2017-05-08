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
	
	return bValid;
}

void FinancialCalendar::init()
{
	{
		ui.treeView->setAlternatingRowColors(true);
		connect(ui.treeView, &QTreeView::clicked, [this](const QModelIndex &index) {
			QVariant s = index.sibling(index.row(), eDate).data();
			if (s.isValid())
			{
				ui.dateEdit->setDate(QDate::fromString(s.toString(), YMD));
				QStringList slist = s.toString().split("-");
				if (slist.size() <= 1) return;
				ui.lineEdit_year->setText(slist[0]);
				QVariant sDayType = index.sibling(index.row(), eDateType).data();
				if (sDayType.isValid())
					ui.comboBox->setCurrentText(sDayType.toString());
				QVariant sDayinfo = index.sibling(index.row(), eInfo).data();
				if (sDayinfo.isValid())
					ui.textEdit_def->setText(sDayinfo.toString());

				setCurrentData(getViewData());
			}
		});
		initDateView();
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
		QString err;
		if (PARASETCTL->setFinancialCalendar(fc, err))
		{
			ShowSuccessMessage(tr("add"), tr("add success."), this);
			// 同步
			addDate(fc);
			// 定位
			locateDate(fc);
		}
		else
			ShowWarnMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
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
	QString err;
	if (PARASETCTL->setFinancialCalendar(fc, err))
	{
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		// 同步
		addDate(fc);
		// 定位
		locateDate(fc);
	} else {
		ShowWarnMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
}
void FinancialCalendar::delHoliday()
{
	if (!checkValid()) return;
	CFinancialCalendar fc = getViewData();
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this))
	{
		if (PARASETCTL->isExistFinancialCalendar(fc))
		{
			QString err;
			if (!PARASETCTL->removeFinancialCalendar(fc.getDate(), err))
			{
				ShowWarnMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
				return;
			}
		}
		else
		{
			QString errMsg = tr("No record in database.").arg(QDate::fromJulianDay(fc.getDate()).toString(YMD));
			ShowWarnMessage(tr("delete"), errMsg, this);
			return;
		}
		ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		// 同步
		delDate(fc);
		// 定位
		locateDate(fc);
	}
}

void FinancialCalendar::initDateView()
{
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
	for (QMap<int, CFinancialCalendar>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		addDate(itor.value());
	}
	if (!val.isEmpty()) {
		locateDate(val[val.keys().back()]);
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

void FinancialCalendar::addDate(const CFinancialCalendar & date)
{
	auto insertData = [this](QStandardItem *rd, const CFinancialCalendar &val) {
		QString dKey = QDate::fromJulianDay(val.getDate()).toString(YMD);
		QList<QStandardItem *> childItems;
		for (int i = 0; i < eEnd; i++)
		{
			switch (i)
			{
			case eDate:
				{
					QStandardItem *itemDate = new QStandardItem(dKey);
					childItems.push_back(itemDate);
					itemDate->setToolTip(dKey);
					break;
				}
			case eDateType:
				{
					QStandardItem *itemHolidayType = new QStandardItem(val.getHolidayTypeStr());
					childItems.push_back(itemHolidayType);
					break;
				}
			case eInfo:
				{
					QStandardItem *itemHolidayinfo = new QStandardItem(val.getHolidayinfo());
					childItems.push_back(itemHolidayinfo);
					itemHolidayinfo->setToolTip(qutil::splitTooltip(val.getHolidayinfo(), 200));
					break;
				}
			default:
				break;
			}
		}
		rd->appendRow(childItems);
		this->m_tree[dKey] = childItems;
	};
	QString ykey = QString::number(date.getYear());
	if (!m_tree.contains(ykey) || m_tree[ykey].isEmpty())
	{
		QStandardItem* pRoot = new QStandardItem(QString::number(date.getYear()));
		m_pGoodsModel->appendRow(pRoot);
		m_tree[ykey] = QList<QStandardItem*>() << pRoot;
		insertData(pRoot, date);
	}
	else
	{
		QString dKey = QDate::fromJulianDay(date.getDate()).toString(YMD);
		if (!m_tree.contains(dKey))
		{
			insertData(m_tree[ykey].front(), date);
		}
		else
		{
			if (m_tree[dKey].size() == eEnd)
			{
				m_tree[dKey][eDate]->setText(dKey);
				m_tree[dKey][eDateType]->setText(date.getHolidayTypeStr());
				m_tree[dKey][eInfo]->setText(date.getHolidayinfo());
				m_tree[dKey][eDateType]->setToolTip(dKey);
				m_tree[dKey][eInfo]->setToolTip(qutil::splitTooltip(date.getHolidayinfo(), 200));
			}
		}
	}
}
void FinancialCalendar::delDate(const CFinancialCalendar & date)
{
	QString ykey = QString::number(date.getYear());
	QString dKey = QDate::fromJulianDay(date.getDate()).toString(YMD);
	if (m_tree.contains(dKey))
	{
		m_pGoodsModel->removeRow(m_tree[dKey].front()->row(), m_tree[dKey].front()->parent()->index());
		m_tree.remove(dKey);
	}
	if (m_tree.contains(ykey))
	{
		QMap<QString, QList<QStandardItem *>>::iterator itor = m_tree.find(ykey);
		if (itor != m_tree.end())
		{
			QMap<QString, QList<QStandardItem *>>::const_iterator itortmp = itor+1;
			if (itortmp == m_tree.end() || itortmp.key().indexOf(ykey)==-1)
			{
				m_pGoodsModel->removeRow(m_tree[ykey].front()->row());
				// 不存在子节点
				m_tree.remove(ykey);
			}
		}
	}
}
void FinancialCalendar::locateDate(const CFinancialCalendar & date)
{
	ui.treeView->sortByColumn(eDate, Qt::AscendingOrder);
	auto locate = [this](const QString &dkey) {
		QModelIndexList findIndex = this->m_pGoodsModel->match(this->m_pGoodsModel->index(0, 0), Qt::DisplayRole, dkey, 1, Qt::MatchRecursive);
		if (findIndex.size() > 0)
		{
			this->ui.treeView->setCurrentIndex(findIndex[eDate]);
			this->ui.treeView->clicked(findIndex[eDate]);
		}
		else
		{
			this->clear();
		}
	};
	QString dKey = QDate::fromJulianDay(date.getDate()).toString(YMD);
	if (m_tree.contains(dKey))
	{
		locate(dKey);
	}
	else
	{
		QString nearKey = "";
		for (QMap<QString, QList<QStandardItem *>>::const_iterator itor = m_tree.begin();
			itor != m_tree.end(); itor++)
		{
			if (itor.key().compare(dKey) <= 0) nearKey = itor.key();
			if (nearKey.isEmpty())
			{
				if (itor.key().size() == dKey.size())
				{
					nearKey = itor.key();
					break;
				}
			}
			else if (nearKey == QString::number(date.getYear()))
				nearKey = "";
		}
		if (!nearKey.isEmpty()) locate(nearKey);
		else clear();
	}
}

void FinancialCalendar::clear()
{
	ui.dateEdit->setDate(QDate::currentDate());
	ui.lineEdit_year->setText("");
	ui.textEdit_def->setText("");

	setCurrentData(getViewData());
}