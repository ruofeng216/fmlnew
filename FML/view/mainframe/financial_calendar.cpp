#include "financial_calendar.h"
#include "util/datatype.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include <QCalendarWidget>
#include "util/util.h"
#include "view/commonui/message_box_widget.h"

#define V_INDEX (Qt::UserRole + 1)
#define V_CODE (Qt::UserRole + 2)

#define FORMATDATE(T) QDate::fromJulianDay(T).toString(YMD)
#define STRTOJULIAN(T) QDate::fromString(T,YMD).toJulianDay()
#define INTTOQSTRING(T) QString::number(T)

#define COLSHOLIDAY QList<int>()<<eDate<<eDateType<<eInfo
#define COLSYEAR QList<int>()<<eYear

FinancialCalendar::FinancialCalendar(QWidget *parent)
	: BodyWidget(parent)
	, m_pGoodsModel(NULL)
{
	ui.setupUi(this);
	connect(ui.pushButton_add, &QPushButton::clicked, this, &FinancialCalendar::addHoliday);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &FinancialCalendar::modifyHoliday);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &FinancialCalendar::delHoliday);
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotTreeClicked(QModelIndex)));
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
bool FinancialCalendar::checkValid(bool)
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
		ui.comboBox->addItems(QStringList() << tr("holiday") << tr("workday"));
		ui.comboBox->view()->setAlternatingRowColors(true);
	}

	connect(ui.dateEdit, &QDateTimeEdit::dateChanged, [this](const QDate &date) {
		ui.lineEdit_year->setText(QString::number(date.year()));
	});

	{
		ui.treeView->setAlternatingRowColors(true);
		initDateView();
	}
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
		
			if (!m_tree.contains(INTTOQSTRING(fc.getYear())))
			{
				addTree(m_tree, m_pGoodsModel,INTTOQSTRING(fc.getYear()), "",fc, COLSYEAR);
			}

			addTree(m_tree, m_pGoodsModel,FORMATDATE(fc.getDate()),
				INTTOQSTRING(fc.getYear()),fc,COLSHOLIDAY);
			
			locator(m_tree, FORMATDATE(fc.getDate()));
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
		
		addTree(m_tree, m_pGoodsModel, FORMATDATE(fc.getDate()),
			INTTOQSTRING(fc.getYear()), fc, COLSHOLIDAY);
		locator(m_tree, FORMATDATE(fc.getDate()));

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
		delTree(m_tree, m_pGoodsModel, FORMATDATE(fc.getDate()), INTTOQSTRING(fc.getYear()));
		if (!hasChildren(m_pGoodsModel, INTTOQSTRING(fc.getYear()))) {
			delTree(m_tree, m_pGoodsModel, INTTOQSTRING(fc.getYear()), "");
		}
		
		locator(m_tree, FORMATDATE(fc.getDate()));
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
		//addDate(itor.value());
		if (!m_tree.contains(INTTOQSTRING(itor->getYear())))
		{
			addTree(m_tree, m_pGoodsModel,
				INTTOQSTRING(itor->getYear()), "",
				itor.value(), COLSYEAR);
		}
			
		addTree(m_tree, m_pGoodsModel,
			FORMATDATE(itor->getDate()),
			INTTOQSTRING(itor->getYear()),
			itor.value(),
			COLSHOLIDAY);
	}
	if (!val.isEmpty()) {
		QString dt = FORMATDATE(val[val.keys().back()].getDate());
		locator(m_tree, dt);
	}
}

void FinancialCalendar::slotTreeClicked(const QModelIndex &index) {
	QString text, data; // text:UI上展示的名称，data：对应的代码
	CFinancialCalendar val;
	getParentData(m_pGoodsModel, index, text,data,V_CODE);
	val.setYear(text.toInt());

	if (index.parent().isValid())
	{
		QModelIndex parentIndex = index.parent();
		if (index.parent().isValid()) {
			parentIndex = parentIndex.sibling(parentIndex.row(), 0);
		}
		else {
			parentIndex = index.sibling(index.row(), 0);
		}
		
		QStandardItem *item = m_pGoodsModel->itemFromIndex(parentIndex);
		if (item)
		{
			int nIndex = 0;
			for (int i = 0; i < item->columnCount(); i++)
			{

				getChildData(m_pGoodsModel, index, i, nIndex, text, data, V_INDEX, V_CODE);
				if (nIndex == eYear)
				{
					val.setYear(text.toInt());
				}
				else if (nIndex == eDate)
				{
					val.setDate(STRTOJULIAN(text));
				}
				else if (nIndex == eDateType)
				{
					val.setHolidayType(text.toInt());
				}
				else if (nIndex == eInfo)
				{
					val.setHolidayinfo(text);
				}
			}
		}
	}
	setViewData(val);

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

void FinancialCalendar::bwLocate(const QString &code, CFinancialCalendar t) {
	QModelIndexList findIndex = this->m_pGoodsModel->match(this->m_pGoodsModel->index(0, 0), Qt::DisplayRole, code, 1, Qt::MatchRecursive);
	if (findIndex.size() > 0)
	{
		this->ui.treeView->setCurrentIndex(findIndex[0]);
		this->ui.treeView->clicked(findIndex[0]);
	}
	else
	{
		this->bwClear();
	}
}

bool FinancialCalendar::recordExist(const QString &val, CFinancialCalendar t) {
	if (PARASETCTL->getFinancialCalendar().contains(QDate::fromString(val).toJulianDay()))
		return true;
	else
		return false;
}

void FinancialCalendar::bwClear(CFinancialCalendar t) {
	ui.dateEdit->setDate(QDate::currentDate());
	ui.lineEdit_year->setText("");
	ui.textEdit_def->setText("");

	setCurrentData(getViewData());
}

void FinancialCalendar::packQStandardItem(QList<QStandardItem *> &childItems, const CFinancialCalendar &val, const QList<int> cols) {
	
	if (FORMATDATE(val.getDate()).isEmpty()) return;
	foreach(int i, cols)
	{
		switch (i)
		{
		case eYear:
		{
			QStandardItem *p = new QStandardItem(INTTOQSTRING(val.getYear()));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(eYear, V_INDEX);
			break;
		}
		case eDate:
		{
			QStandardItem *p = new QStandardItem(FORMATDATE(val.getDate()));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(eDate, V_INDEX);
			p->setData(val.getYear(), V_CODE);
			break;
		}
		case eDateType:
		{
			QStandardItem *p = new QStandardItem(val.getHolidayTypeStr());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(eDateType, V_INDEX);
			break;
		}
		case eInfo:
		{
			QStandardItem *p = new QStandardItem(val.getHolidayinfo());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(eInfo, V_INDEX);
			break;
		}
		default:
			break;
		}
	}
}

QStandardItem *FinancialCalendar::getItem(QMap<QString, QList<QStandardItem *>> &tree, const QString &code, int nVal, int role) {
	if (tree.contains(code))
	{
		for (QList<QStandardItem *>::iterator itor = tree[code].begin();
			itor != tree[code].end(); itor++)
		{
			auto p = *itor;
			if (p && p->data(role).toInt() == nVal)
				return p;
		}
	}
	return NULL;
}

void FinancialCalendar::updateChildNode(const CFinancialCalendar &val) {
	QStandardItem *pYear = getItem(m_tree, FORMATDATE(val.getDate()), eDate, V_INDEX);
	if (pYear && val.getYear() != pYear->data(V_CODE).toInt()) {
		QString strDate = FORMATDATE(val.getDate());
		QString oldYear = pYear->data(V_CODE).toString();

		delTree(m_tree, m_pGoodsModel, strDate, oldYear);
		if (m_tree.contains(oldYear) &&
			!m_tree[oldYear].isEmpty() &&
			m_tree[oldYear].front()->rowCount() == 0)
		{
			delTree(m_tree, m_pGoodsModel, strDate, "");
		}

		if (!m_tree.contains(INTTOQSTRING(val.getYear())) && recordExist(INTTOQSTRING(val.getYear())))
			addTree(m_tree, m_pGoodsModel, INTTOQSTRING(val.getYear()), "", val, COLSYEAR);
		addTree(m_tree, m_pGoodsModel,FORMATDATE(val.getDate()), INTTOQSTRING(val.getYear()), val, COLSHOLIDAY);
	}
	else if(pYear) {

		QStandardItem *p = getItem(m_tree, FORMATDATE(val.getDate()), eDateType, V_INDEX);
		if (p)
		{
			p->setText(val.getHolidayTypeStr());
			p->setToolTip(val.getHolidayTypeStr());
		}
		p = getItem(m_tree, FORMATDATE(val.getDate()), eInfo, V_INDEX);
		if (p)
		{
			p->setText(val.getHolidayinfo());
			p->setToolTip(val.getHolidayinfo());
		}
	}

}

CFinancialCalendar FinancialCalendar::getTFromDB(const QString &code, QString &parentCode, CFinancialCalendar t) {

	CFinancialCalendar val;
	if (PARASETCTL->getFinancialCalendar().contains(QDate::fromString(code).toJulianDay()))
		val = PARASETCTL->getFinancialCalendar()[QDate::fromString(code).toJulianDay()];
	parentCode = INTTOQSTRING(val.getYear());
	return val;
}