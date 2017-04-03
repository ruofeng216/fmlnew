#include "key_point_definition.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"
#include <QDebug>

KeyPointDefinition::KeyPointDefinition(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
{
	ui.setupUi(this);
	ui.treeView->setAlternatingRowColors(true);
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotTreeDoubleClicked(QModelIndex)));

	init();
	slotSkinChange();
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(QDate::currentDate());
}

KeyPointDefinition::~KeyPointDefinition()
{
}

void KeyPointDefinition::init()
{
	CKeypoint oldVal = getViewData();
	{
		// 初始化treeView
		QStringList treeHeader;
		treeHeader << tr("kpcode") << tr("kpname") << tr("productname") << tr("tenor") << tr("calendar")
			<< tr("convention") << tr("daycount") << tr("startdate") << tr("enddate");
		if (m_model) {
			m_model->clear();
		} else {
			m_model = new QStandardItemModel(0, treeHeader.size(), this);
		}

		m_model->setColumnCount(treeHeader.size());
		for (int i = 0; i < treeHeader.size(); i++) {
			m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
		}
		const QMap<QString, CKeypoint>& data = YIELDCURVECTL->getKeyPoint();
		QMap<QString, QStandardItem*> roots;
		for (auto iter = data.constBegin(); iter != data.constEnd(); ++iter) {
			if (!roots.contains(iter->getMarketCode())) {
				QList<QStandardItem*> items = createParentRowItems(*iter);
				if (items.size() > 0) {
					m_model->appendRow(items);
					roots[iter->getMarketCode()] = items[0];
				}
			}
		}
		for (auto iter = data.constBegin(); iter != data.constEnd(); ++iter) {
			if (roots.contains(iter->getMarketCode())) {
				QList<QStandardItem*> items = createChildtRowItems(*iter);
				if (items.size() > 0) {
					roots[iter->getMarketCode()]->appendRow(items);
				}
			}
		}

		ui.treeView->setModel(m_model);
		ui.treeView->setColumnWidth(0, 160);
		ui.treeView->expandAll();
	}

	auto initParaList = [](QComboBox *cb, const QString &typeCode) {
		cb->clear();
		QList<CParaDict> paraList;
		if (PARASETCTL->getAllParadict(typeCode, paraList)) {
			foreach(const CParaDict &para, paraList) {
				cb->addItem(para.getParaName(), para.getParaCode());
			}
		}
	};

	{
		// 市场初始化
		initParaList(ui.cbMarket, "MarketType");
		// 日历初始化
		initParaList(ui.cbCalendar, "Calendar");
		// 假日调整
		initParaList(ui.cbConvention, "Convention");
		// 天数计数
		initParaList(ui.cbDayCount, "DayCount");
		// 交割天数
		initParaList(ui.cbSpotlag, "CouponFrequency");
	}
	setViewData(oldVal);
}

void KeyPointDefinition::slotSkinChange()
{
	ui.pbAdd->setText(tr("add"));
	ui.pbAdd->setIcon(QIcon(qutil::skin("add-click-hover.png")));
	ui.pbModify->setText(tr("modify"));
	ui.pbModify->setIcon(QIcon(qutil::skin("revise-click-hover.png")));
	ui.pbDelete->setText(tr("delete"));
	ui.pbDelete->setIcon(QIcon(qutil::skin("delete-click-hover.png")));
	qutil::setWeekendStyle(ui.deStart);
	qutil::setWeekendStyle(ui.deEnd);
}

void KeyPointDefinition::slotAdd()
{
	CKeypoint val = getViewData();
	if (val.getKpcode().trimmed().isEmpty()) {
		ShowWarnMessage(tr("add"), tr("kpcode is empty"), this);
		return;
	}

	CKeypoint oldVal;
	if (YIELDCURVECTL->getKeyPoint(val.getKpcode(), oldVal)) {
		ShowWarnMessage(tr("add"), tr("The kpcode already exists"), this);
		return;
	}

	if (YIELDCURVECTL->setKeyPoint(val)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		init();
	} else {
		ShowErrorMessage(tr("add"), tr("add fail."), this);
	}
}

void KeyPointDefinition::slotModify()
{

}

void KeyPointDefinition::slotDelete()
{

}

void KeyPointDefinition::slotTreeDoubleClicked(const QModelIndex &index)
{
	CKeypoint val;
	val.setKpcode(index.sibling(index.row(), 0).data().toString());
	val.setKpname(index.sibling(index.row(), 1).data().toString());
	val.setProductName(index.sibling(index.row(), 2).data().toString());
	val.setTenor(index.sibling(index.row(), 3).data().toString());
	val.setCalendar(index.sibling(index.row(), 4).data().toString());
	val.setConvention(index.sibling(index.row(), 5).data().toString());
	val.setDayCount(index.sibling(index.row(), 6).data().toString());
}

void KeyPointDefinition::setViewData(const CKeypoint &val)
{
	ui.leKPCode->setText(val.getKpcode());
	ui.leKPName->setText(val.getKpname());
	ui.leProductCode->setText(val.getProductCode());
	ui.leProductName->setText(val.getProductName());
	QPair<int, QString> tenor = parseTenor(val.getTenor());
	ui.leTenor->setText(QString::number(tenor.first));
	ui.rbDay->setChecked(tenor.second == "d");
	ui.rbMonth->setChecked(tenor.second == "m");
	ui.rbYear->setChecked(tenor.second == "y");

	CProduct product;
	if (PARASETCTL->getProduct(val.getProductCode(), product)) {
		ui.deStart->setDate(QDate::fromJulianDay(product.getSdate()));
		ui.deEnd->setDate(QDate::fromJulianDay(product.getEdate()));
	} else {
		qWarning() << "get product faild, code:" << val.getProductCode();
	}
	ui.cbMarket->setCurrentText(getParaNameFromCode(val.getMarketCode()));
	ui.cbCalendar->setCurrentText(getParaNameFromCode(val.getCalendar()));
	ui.cbConvention->setCurrentText(getParaNameFromCode(val.getConvention()));
	ui.cbSpotlag->setCurrentText(getParaNameFromCode(val.getSpotlat()));
	ui.cbDayCount->setCurrentText(getParaNameFromCode(val.getDayCount()));
}

CKeypoint KeyPointDefinition::getViewData()
{
	CKeypoint result;
	result.setKpcode(ui.leKPCode->text().trimmed());
	result.setKpname(ui.leKPName->text().trimmed());
	result.setProductCode(ui.leProductCode->text().trimmed());
	result.setProductName(ui.leProductName->text().trimmed());
	QString tenor;
	if (ui.rbDay->isChecked()) {
		tenor = "d"; 
	} else if (ui.rbMonth->isChecked()) {
		tenor = "m";
	} else if (ui.rbYear->isChecked()) {
		tenor = "y";
	}
	result.setTenor(spliceTenor(ui.leTenor->text().toInt(), tenor));
	QString strStartDate = ui.deStart->date().toString("yyyy-MM-dd");
	QString strEndDate = ui.deEnd->date().toString("yyyy-MM-dd");
	result.setMarketCode(ui.cbMarket->currentData().toString().trimmed());
	result.setMarketName(ui.cbMarket->currentText().trimmed());
	result.setCalendar(ui.cbCalendar->currentData().toString().trimmed());
	result.setConvention(ui.cbConvention->currentData().toString().trimmed());
	result.setSpotlat(ui.cbSpotlag->currentData().toString().trimmed());
	result.setDayCount(ui.cbDayCount->currentData().toString().trimmed());
	return result;
}

QPair<int, QString> KeyPointDefinition::parseTenor(const QString& tenor)
{
	int count = tenor.size();
	if (count > 1) {
		return qMakePair(tenor.left(count - 1).toInt(), tenor.right(1).toLower());
	}
	return qMakePair(0, QString(""));
}

QString KeyPointDefinition::spliceTenor(int num, QString unit)
{
	return QString("%1%2").arg(num).arg(unit.toLower());
}

QString KeyPointDefinition::getParaNameFromCode(const QString &code)
{
	CProduct product;
	if (PARASETCTL->getProduct(code, product)) {
		return product.getName();
	}
	return "";
}
QList<QStandardItem *> KeyPointDefinition::createParentRowItems(const CKeypoint &val)
{
	QList<QStandardItem *> result;
	QStandardItem *marketName = new QStandardItem(val.getMarketName());
	marketName->setData(val.getMarketCode(), Qt::UserRole);
	result.push_back(marketName);
	return result;
}

QList<QStandardItem *> KeyPointDefinition::createChildtRowItems(const CKeypoint &val)
{
	QList<QStandardItem *> result;
	QStandardItem *kpcode = new QStandardItem(val.getKpcode());
	QStandardItem *kpname = new QStandardItem(val.getKpname());
	QStandardItem *productName = new QStandardItem(val.getProductName());
	QStandardItem *tendor = new QStandardItem(val.getTenor());
	QStandardItem *calendar = new QStandardItem(val.getCalendar());
	QStandardItem *convention = new QStandardItem(val.getConvention());
	QStandardItem *dayCount = new QStandardItem(val.getDayCount());
	QStandardItem *spotlat = new QStandardItem(val.getSpotlat());
	QString strStartDate, strEndDate;
	CProduct product;
	if (PARASETCTL->getProduct(val.getProductCode(), product)) {
		strStartDate = QDate::fromJulianDay(product.getSdate()).toString("yyyy-MM-dd");
		strEndDate = QDate::fromJulianDay(product.getEdate()).toString("yyyy-MM-dd");
	} else {
		qWarning() << "get product failed, code:" << val.getProductCode();
	}
	QStandardItem *startDate = new QStandardItem(strStartDate);
	QStandardItem *endDate = new QStandardItem(strEndDate);
	result.push_back(kpcode);
	result.push_back(kpname);
	result.push_back(productName);
	result.push_back(tendor);
	result.push_back(calendar);
	result.push_back(convention);
	result.push_back(dayCount);
	result.push_back(spotlat);
	result.push_back(startDate);
	result.push_back(endDate);
	return result;
}