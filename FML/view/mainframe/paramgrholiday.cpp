#include "paramgrholiday.h"
#include "util/datatype.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "util/util.h"

ParaMgrHoliday::ParaMgrHoliday(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.pushButton_add->setText(tr("add"));
	ui.pushButton_modify->setText(tr("modify"));
	ui.pushButton_delete->setText(tr("delete"));
	connect(ui.pushButton_add, &QPushButton::clicked, this, &ParaMgrHoliday::addHoliday);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &ParaMgrHoliday::modifyHoliday);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &ParaMgrHoliday::delHoliday);
	init();
}

ParaMgrHoliday::~ParaMgrHoliday()
{
}

void ParaMgrHoliday::init()
{
	{
		ui.treeView->setAlternatingRowColors(true);
		QMap<int, CFinancialCalendar> val;
		PARASETCTL->getFinancialCalendar(val);
		QStringList treeHeader;
		treeHeader << tr("hyear") << tr("holiday") << tr("holidayinfo");
		QStandardItemModel *goodsModel = new QStandardItemModel(0, treeHeader.size(), this);
		for (int i = 0; i < treeHeader.size(); i++)
			goodsModel->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
		ui.treeView->setModel(goodsModel);

		QList<QStandardItem *> items;
		for (QMap<int, CFinancialCalendar>::const_iterator itor = val.begin();
			itor != val.end(); itor++)
		{
			if (items.isEmpty() ||
				items[0]->text().toInt() != itor->getYear())
			{
				items.clear();
				items << new QStandardItem(itor->getYear());
				goodsModel->appendRow(items);
			}

			QList<QStandardItem *> childItems;
			QStandardItem *itemDate = new QStandardItem(QDate::fromJulianDay(itor->getDate()).toString("yyyy-MM-dd"));
			QStandardItem *itemHolidayType = new QStandardItem(itor->getHolidayTypeStr());
			QStandardItem *itemHolidayinfo = new QStandardItem(itor->getHolidayinfo());
			childItems.push_back(itemDate);
			childItems.push_back(itemHolidayType);
			childItems.push_back(itemHolidayinfo);
			items.at(0)->appendRow(childItems);
		}
	}
	

	{
		ui.comboBox->addItems(QStringList() << tr("holiday") << tr("workday"));
		ui.comboBox->view()->setAlternatingRowColors(true);
	}
}

void ParaMgrHoliday::addHoliday()
{
	QString _y = ui.lineEdit_year->text();
	qint64 _d = ui.dateEdit->date().toJulianDay();
}
void ParaMgrHoliday::modifyHoliday()
{

}
void ParaMgrHoliday::delHoliday()
{

}