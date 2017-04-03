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
				cb->addItem(para.getParaName().trimmed(), para.getParaCode().trimmed());
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
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CKeypoint val = getViewData();
		CKeypoint oldVal;
		if (!YIELDCURVECTL->getKeyPoint(val.getKpcode(), oldVal)) {
			ShowWarnMessage(tr("delete"), tr("The kpcode is not existing!"), this);
			return;
		}

		if (YIELDCURVECTL->removeKeyPoint(val.getKpcode())) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			init();
		} else {
			ShowErrorMessage(tr("delete"), tr("delete fail."), this);
		}
	}
}

void KeyPointDefinition::slotTreeDoubleClicked(const QModelIndex &index)
{
	auto getTextData = [this, &index](int col, QString &text, QString &data) {
		text = "";
		data = "";
		QModelIndex sibling = index.sibling(index.row(), col);
		QStandardItem *item = m_model->itemFromIndex(sibling);
		if (item) {
			data = item->data().toString();
			text = item->text();
		}
	};

	QString text, data; // text:UI上展示的名称，data：对应的代码
	CKeypoint val;
	QString startDate, endDate;
	QModelIndex parentIndex = index.parent();
	if (parentIndex.isValid()) {
		getTextData(0, text, data);
		val.setKpcode(text);
		getTextData(1, text, data);
		val.setKpname(text);
		getTextData(2, text, data);
		val.setProductCode(data);
		val.setProductName(text);
		getTextData(3, text, data);
		val.setTenor(text);
		getTextData(4, text, data);
		val.setCalendar(data); // 存放code， 显示name
		getTextData(5, text, data);
		val.setConvention(data);
		getTextData(6, text, data);
		val.setDayCount(data);
		getTextData(7, text, data); // startDate
		startDate = text;
		getTextData(8, text, data); // endDate
		endDate = text;
	} else {
		getTextData(0, text, data);
		val.setMarketCode(data);
		val.setMarketName(text);
	}

	setViewData(val);
	ui.deStart->setDate(QDate::fromString(startDate, "yyyy-MM-dd"));
	ui.deEnd->setDate(QDate::fromString(endDate, "yyyy-MM-dd"));
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
	ui.cbMarket->setCurrentText(getParaNameFromCode("MarketType", val.getMarketCode()));
	ui.cbCalendar->setCurrentText(getParaNameFromCode("Calendar", val.getCalendar()));
	ui.cbConvention->setCurrentText(getParaNameFromCode("Convention", val.getConvention()));
	ui.cbSpotlag->setCurrentText(getParaNameFromCode("CouponFrequency", val.getSpotlat()));
	ui.cbDayCount->setCurrentText(getParaNameFromCode("DayCount", val.getDayCount()));
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

QString KeyPointDefinition::getParaNameFromCode(const QString &typecode, const QString &paracode)
{
	CParaDict paradict;
	if (PARASETCTL->getParadict(typecode, paracode, paradict)) {
		return paradict.getParaName();
	}
	return "";
}
QList<QStandardItem *> KeyPointDefinition::createParentRowItems(const CKeypoint &val)
{
	QList<QStandardItem *> result;
	QStandardItem *marketName = new QStandardItem(val.getMarketName());
	marketName->setData(val.getMarketCode());
	result.push_back(marketName);
	return result;
}

QList<QStandardItem *> KeyPointDefinition::createChildtRowItems(const CKeypoint &val)
{
	auto createItem = [](const QString &text, const QString &data = "") -> QStandardItem* {
		QStandardItem *item = new QStandardItem(text);
		item->setData(data);
		return item;
	};

	QString strStartDate, strEndDate;
	CProduct product;
	if (PARASETCTL->getProduct(val.getProductCode(), product)) {
		strStartDate = QDate::fromJulianDay(product.getSdate()).toString("yyyy-MM-dd");
		strEndDate = QDate::fromJulianDay(product.getEdate()).toString("yyyy-MM-dd");
	} else {
		qWarning() << "get product failed, code:" << val.getProductCode();
	}

	QList<QStandardItem *> result;
	result.push_back(createItem(val.getKpcode()));
	result.push_back(createItem(val.getKpname()));
	result.push_back(createItem(val.getProductName(), val.getProductCode()));
	result.push_back(createItem(val.getTenor()));
	result.push_back(createItem(getParaNameFromCode("Calendar", val.getCalendar()), val.getCalendar()));
	result.push_back(createItem(getParaNameFromCode("Convention", val.getConvention()), val.getConvention()));
	result.push_back(createItem(getParaNameFromCode("DayCount", val.getDayCount()), val.getDayCount()));
	result.push_back(createItem(strStartDate));
	result.push_back(createItem(strEndDate));
	return result;
}