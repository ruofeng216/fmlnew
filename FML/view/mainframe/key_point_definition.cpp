#include "key_point_definition.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include "viewsignalmanager/viewsignalmanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"
#include <QDebug>
#include <QTableView>

KeyPointDefinition::KeyPointDefinition(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
	, m_pProductModel(nullptr)
{
	ui.setupUi(this);

	connect(VIEWSIGNAL, &ViewSignalManager::sigProductChange, this, &KeyPointDefinition::initProduct);

	
	ui.treeView->setAlternatingRowColors(true);
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotTreeDoubleClicked(QModelIndex)));
	init();
	slotSkinChange();
	ui.leTenor->setValidator(new QIntValidator(0, 99999, this));
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(QDate::currentDate());
}

KeyPointDefinition::~KeyPointDefinition()
{
}

QString KeyPointDefinition::getKey(const CKeypoint &newVal) const
{
	return newVal.getKpcode();
}

// 提交时，检查相关控件值是否合法。
bool KeyPointDefinition::checkValid()
{
	return true;
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

	{
		if (!m_pProductModel) m_pProductModel = new QStandardItemModel(0, 1, this);
		m_pProductModel->setColumnCount(1);
		ui.cbProductCode->setModel(m_pProductModel);
		connect(ui.cbProductCode, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
			[this](const QString &text) {
			if (PARASETCTL->getProduct().contains(text))
				ui.cbProductName->setText(PARASETCTL->getProduct()[text].getName());
			else
				ui.cbProductName->setText("");
		});
		// 初始化产品列表
		initProduct();
		// 市场初始化
		initParameter(MARKETTYPE);
		// 日历初始化
		initParameter(CALENDAR);
		// 计息日调整
		initParameter(CONVENTION);
		// 天数计数
		initParameter(DAYCOUNT);
		// 付息频率
		initParameter(COUPONFREQUENCY);
	}
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

	if (!isProductExist(val.getProductCode())) {
		return;
	}

	QString err;
	if (YIELDCURVECTL->setKeyPoint(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		init();
	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void KeyPointDefinition::slotModify()
{
	if (getCurrentData().getKpcode().isEmpty()) {
		ShowWarnMessage(tr("modify"), tr("No selected content can not be modified"), this);
		return;
	}

	CKeypoint val = getViewData();
	if (this->isKeyModify(val)) {
		ShowWarnMessage(tr("modify"), tr("keypoint code can not be modified"), this);
		return;
	}

	if (this->isEqual(val)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}

	if (!isProductExist(val.getProductCode())) {
		return;
	}
	QString err;
	if (YIELDCURVECTL->setKeyPoint(val,err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		init();
	} else {
		ShowErrorMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
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
		QString err;
		if (YIELDCURVECTL->removeKeyPoint(val.getKpcode(), err)) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			setViewData(CKeypoint());
			init();
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void KeyPointDefinition::slotTreeDoubleClicked(const QModelIndex &index)
{
	auto getChildData = [this, &index](int col, QString &text, QString &data) {
		text = ""; data = "";
		QModelIndex sibling = index.sibling(index.row(), col);
		QStandardItem *item = m_model->itemFromIndex(sibling);
		if (item) {
			data = item->data().toString();
			text = item->text();
		}
	};
	auto getParentData = [this, &index](QString &text, QString &data) {
		text = ""; data = "";
		QModelIndex parentIndex = index.parent();
		if (index.parent().isValid()) {
			parentIndex = parentIndex.sibling(parentIndex.row(), 0);
		} else {
			parentIndex = index.sibling(index.row(), 0);
		}
		QStandardItem *item = m_model->itemFromIndex(parentIndex);
		if (item) {
			text = item->text();
			data = item->data().toString();
		}
	};

	QString text, data; // text:UI上展示的名称，data：对应的代码
	CKeypoint val;
	QString startDate, endDate;
	getParentData(text, data);
	val.setMarketCode(data);
	val.setMarketName(text);
	if (index.parent().isValid()) {
		getChildData(0, text, data);
		val.setKpcode(text);
		getChildData(1, text, data);
		val.setKpname(text);
		getChildData(2, text, data);
		val.setProductCode(data);
		val.setProductName(text);
		getChildData(3, text, data);
		val.setTenor(text.toLower());
		getChildData(4, text, data);
		//val.setCalendar(data); // 存放code， 显示name
		getChildData(5, text, data);
		//val.setConvention(data);
		getChildData(6, text, data);
		//val.setDayCount(data);
		getChildData(7, text, data); // startDate
		startDate = text;
		getChildData(8, text, data); // endDate
		endDate = text;
	}

	setViewData(val);
	ui.deStart->setDate(QDate::fromString(startDate, YMD));
	ui.deEnd->setDate(QDate::fromString(endDate, YMD));
}

void KeyPointDefinition::setViewData(const CKeypoint &val)
{
	ui.leKPCode->setText(val.getKpcode());
	ui.leKPName->setText(val.getKpname());
	ui.cbProductCode->setCurrentText(val.getProductCode());
	ui.cbProductName->setText(val.getProductName());
	QPair<int, QString> tenor = parseTenor(val.getTenor());
	ui.leTenor->setText(QString::number(tenor.first));
	ui.rbDay->setChecked(tenor.second == "d");
	ui.rbMonth->setChecked(tenor.second == "m");
	ui.rbYear->setChecked(tenor.second == "y");

	CProduct product;
	if (PARASETCTL->getProduct(val.getProductCode(), product)) {
		ui.deStart->setDate(product.getSdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(product.getSdate()));
		ui.deEnd->setDate(product.getEdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(product.getEdate()));
	} else {
		qWarning() << "get product faild, code:" << val.getProductCode();
	}
	ui.cbMarket->setCurrentText(getParaNameFromCode("MarketType", val.getMarketCode()));
	//ui.cbCalendar->setCurrentText(getParaNameFromCode("Calendar", val.getCalendar()));
	//ui.cbConvention->setCurrentText(getParaNameFromCode("Convention", val.getConvention()));
	//ui.cbSpotlag->setCurrentText(getParaNameFromCode("CouponFrequency", val.getSpotlat()));
	//ui.cbDayCount->setCurrentText(getParaNameFromCode("DayCount", val.getDayCount()));
	setCurrentData(getViewData()); // 从UI获取，保持与UI一致
}

CKeypoint KeyPointDefinition::getViewData()
{
	CKeypoint result;
	result.setKpcode(ui.leKPCode->text().trimmed());
	result.setKpname(ui.leKPName->text().trimmed());
	result.setProductCode(ui.cbProductCode->currentText().trimmed());
	result.setProductName(ui.cbProductName->text().trimmed());
	QString tenor;
	if (ui.rbDay->isChecked()) {
		tenor = "d"; 
	} else if (ui.rbMonth->isChecked()) {
		tenor = "m";
	} else if (ui.rbYear->isChecked()) {
		tenor = "y";
	}
	result.setTenor(spliceTenor(ui.leTenor->text().toInt(), tenor));
	QString strStartDate = ui.deStart->date().toString(YMD);
	QString strEndDate = ui.deEnd->date().toString(YMD);
	result.setMarketCode(ui.cbMarket->currentData().toString().trimmed());
	result.setMarketName(ui.cbMarket->currentText().trimmed());
	//result.setCalendar(ui.cbCalendar->currentData().toString().trimmed());
	//result.setConvention(ui.cbConvention->currentData().toString().trimmed());
	//result.setSpotlat(ui.cbSpotlag->currentData().toString().trimmed());
	//result.setDayCount(ui.cbDayCount->currentData().toString().trimmed());
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
		strStartDate = QDate::fromJulianDay(product.getSdate()).toString(YMD);
		strEndDate = QDate::fromJulianDay(product.getEdate()).toString(YMD);
	} else {
		qWarning() << "get product failed, code:" << val.getProductCode();
	}

	QList<QStandardItem *> result;
	result.push_back(createItem(val.getKpcode()));
	result.push_back(createItem(val.getKpname()));
	result.push_back(createItem(val.getProductName(), val.getProductCode()));
	result.push_back(createItem(val.getTenor().toUpper()));
	//result.push_back(createItem(getParaNameFromCode("Calendar", val.getCalendar()), val.getCalendar()));
	//result.push_back(createItem(getParaNameFromCode("Convention", val.getConvention()), val.getConvention()));
	//result.push_back(createItem(getParaNameFromCode("DayCount", val.getDayCount()), val.getDayCount()));
	result.push_back(createItem(strStartDate));
	result.push_back(createItem(strEndDate));
	return result;
}

bool KeyPointDefinition::isProductExist(const QString &productCode)
{
	CProduct product;
	if (!PARASETCTL->getProduct(productCode, product)) {
		ShowWarnMessage(tr("warning"), tr("product is not existing!"), this);
		return false;
	}
	return true;
}

void KeyPointDefinition::initProduct()
{
	//m_pProductModel->clear();

	//QList<int> columnlst;

	/*for (int i = 0; i < eEnd; i++)
		columnlst << i;*/

	
	/*QMap<QString, CProduct> val = PARASETCTL->getProduct();
	for (QMap<QString, CProduct>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		QMap<QString, QList<QStandardItem *>> _treeCombobox;
		addTree(_treeCombobox, m_pProductModel, itor.value().getCode(), itor.value().getParentCode(), itor.value(), column);
	}
	if (!val.isEmpty())
	{
		locator(m_tree, val[val.keys().back()].getCode());
	}
	else
	{
		bwClear();
	}
	((QTreeView *)(ui.cbParentCode->view()))->expandAll();*/
}

void KeyPointDefinition::initParameter(const QString &paraCode)
{
	auto initParaList = [](QComboBox *cb, const QString &typeCode) {
		cb->clear();
		QMap<QString, CParaDict> paraList;
		if (PARASETCTL->getAllParadict(typeCode, paraList)) {
			foreach(const CParaDict &para, paraList) {
				cb->addItem(para.getParaName().trimmed(), para.getParaCode().trimmed());
			}
		}
	};
	
	if (paraCode == MARKETTYPE)
	{// 市场
		initParaList(ui.cbMarket, MARKETTYPE);
	}
	else if (paraCode == CALENDAR)
	{// 日历
		initParaList(ui.cbCalendar, CALENDAR);
	}
	else if (paraCode == CONVENTION)
	{// 计息日调整
		initParaList(ui.cbConvention, CONVENTION);
	}
	else if (paraCode == DAYCOUNT)
	{// 天数计数
		initParaList(ui.cbDayCount, DAYCOUNT);
	}
	else if (paraCode == COUPONFREQUENCY)
	{// 付息频率
		initParaList(ui.cbSpotlag, COUPONFREQUENCY);
	}
}

void KeyPointDefinition::bwLocate(const QString &code, CKeypoint t)
{
	QModelIndexList findIndex = this->m_model->match(this->m_model->index(0, 0), Qt::DisplayRole, code, 1, Qt::MatchRecursive);
	if (findIndex.size() > 0)
	{
		this->ui.treeView->setCurrentIndex(findIndex[eKPCode]);
		this->ui.treeView->clicked(findIndex[eKPCode]);
	}
	else
	{
		this->bwClear(CKeypoint());
	}
}
bool KeyPointDefinition::recordExist(const QString &val, CKeypoint t)
{
	if (YIELDCURVECTL->getKeyPoint().contains(val))
		return true;
	else
		return false;
}
void KeyPointDefinition::bwClear(CKeypoint t)
{
	ui.leKPCode->setText("");
	ui.leKPName->setText("");
	ui.cbProductCode->setCurrentText("");
	ui.cbProductName->setText("");
	ui.leTenor->setText("");
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(ui.deEnd->maximumDate());
	ui.cbMarket->setCurrentText("");
	ui.cbCalendar->setCurrentText("");
	ui.cbConvention->setCurrentText("");
	ui.cbSpotlag->setCurrentText("");
	ui.cbDayCount->setCurrentText("");

	setCurrentData(getViewData());
}
void KeyPointDefinition::packQStandardItem(QList<QStandardItem *> &childItems, const CKeypoint &val, const QList<int> cols)
{
	if (val.getKpcode().isEmpty()) return;
	foreach (int i, cols)
	{
		switch (i)
		{
		case eKPCode:
		{
			QStandardItem *p = new QStandardItem(val.getKpcode());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			p->setData(val.getMarketCode(), 1);
			break;
		}
		case eKPName:
		{
			QStandardItem *p = new QStandardItem(val.getKpname());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			break;
		}
		case eProductName:
		{
			QStandardItem *p = new QStandardItem(val.getProductName());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			p->setData(val.getProductCode(), 1);
			break;
		}
		case eTenor:
		{
			QStandardItem *p = new QStandardItem(val.getTenor());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			p->setData(val.getTenor(), 1);
			break;
		}
		case eSdate:
		{
			QStandardItem *p = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			break;
		}
		case eEdate:
		{
			QStandardItem *p = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString(YMD));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, 0);
			break;
		}
		case eMarket:
		{
			QStandardItem *p = new QStandardItem(val.getMarketName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getMarketName(), 200));
			p->setData(i, 0);
			p->setData(val.getMarketCode(), 1);
			break;
		}
		case eCalendar:
		{
			QStandardItem *p = new QStandardItem(val.getCalendarName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getCalendarName(), 200));
			p->setData(i, 0);
			p->setData(val.getCalendarCode(), 1);
			break;
		}
		case eConvention:
		{
			QStandardItem *p = new QStandardItem(val.getConventionName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getConventionName(), 200));
			p->setData(i, 0);
			p->setData(val.getConventionCode(), 1);
			break;
		}
		case eDayCount:
		{
			QStandardItem *p = new QStandardItem(val.getDayCountName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getDayCountName(), 200));
			p->setData(i, 0);
			p->setData(val.getDayCountCode(), 1);
			break;
		}
		case eCouponfrequency:
		{
			QStandardItem *p = new QStandardItem(val.getSpotlatName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getSpotlatName(), 200));
			p->setData(i, 0);
			p->setData(val.getSpotlatCode(), 1);
			break;
		}
		default:
			break;
		}
	}
}
void KeyPointDefinition::updateChildNode(const CKeypoint &val)
{
	QList<int> colsKp = QList<int>{ eKPCode,eKPName,eProductName,eTenor,eCalendar,eConvention,eDayCount,eCouponfrequency,eSdate,eEdate };
	QList<int> colsProduct = QList<int>{ 0 };

	if (m_tree.contains(val.getKpcode()))
	{
		QStandardItem *pMarket = getItem(m_tree, val.getKpcode(), eKPCode, 0);
		if (pMarket && val.getMarketCode() != pMarket->data(1).toString()) {

			QString code = val.getKpcode();
			QString oldMarket = pMarket->data(1).toString();
			CKeypoint del;
			del.setKpcode(code);

			delTree(m_tree, m_model, code, oldMarket, CKeypoint());
			
			addTree(m_tree, m_model, code, val.getMarketCode(), val, colsKp);
		}
		/*else if (pMarket && m_tree[val.getParaCode()].size() == eEnd) {
			m_tree[val.getParaCode()][eTypeCode]->setText(val.getTypeCode());
			m_tree[val.getParaCode()][eTypeName]->setText(val.getParaName());

			m_tree[val.getParaCode()][eParaName]->setText(val.getParaName());
			m_tree[val.getParaCode()][eParaExplain]->setText(val.getParaExplain());

			m_tree[val.getParaCode()][eTypeCode]->setToolTip(val.getTypeCode());
			m_tree[val.getParaCode()][eTypeName]->setToolTip(val.getParaName());

			m_tree[val.getParaCode()][eParaName]->setToolTip(val.getParaName());
			m_tree[val.getParaCode()][eParaExplain]->setToolTip(val.getParaExplain());

		}*/
	}
}
CKeypoint KeyPointDefinition::getTFromDB(const QString &code, QString &parentCode, CKeypoint t)
{
	CKeypoint val;
	if (YIELDCURVECTL->getKeyPoint().contains(code))
		val = YIELDCURVECTL->getKeyPoint()[code];
	parentCode = val.getMarketCode();
	return val;
}
void KeyPointDefinition::delTree(QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel, const QString &curCode,
	const QString &parentCode, CKeypoint t)
{
	KeypointTreeOper::delTree(tree, pGoodsModel, curCode, parentCode, t);
}
void KeyPointDefinition::addTree(QMap<QString, QList<QStandardItem *>> &tree, 
	QStandardItemModel *pGoodsModel, const QString &curCode, 
	const QString &parentCode, const CKeypoint &val, const QList<int> cols)
{
	KeypointTreeOper::addTree(tree, pGoodsModel, curCode, parentCode, val, cols);
}

void KeyPointDefinition::bwLocate(const QString &code, CProduct t)
{
}
bool KeyPointDefinition::recordExist(const QString &val, CProduct t)
{
	if (PARASETCTL->getProduct().contains(val))
		return true;
	else
		return false;
}
void KeyPointDefinition::bwClear(CProduct t)
{
}
void KeyPointDefinition::packQStandardItem(QList<QStandardItem *> &childItems, const CProduct &val, const QList<int> cols)
{
	if (val.getCode().isEmpty() || val.getName().isEmpty()) {
		return;
	}
	if (cols.contains(0))
	{
		QStandardItem *p = new QStandardItem(val.getCode());
		childItems.push_back(p);
		p->setToolTip(p->text());
		p->setData(0, 0);
	}
}
void KeyPointDefinition::updateChildNode(const CProduct &val)
{
}
CProduct KeyPointDefinition::getTFromDB(const QString &code, QString &parentCode, CProduct t)
{
	CProduct val;
	if (PARASETCTL->getProduct().contains(code))
		val = PARASETCTL->getProduct()[code];
	parentCode = val.getParentCode();
	return val;
}
void KeyPointDefinition::delTree(QMap<QString, QList<QStandardItem *>> &tree,
	QStandardItemModel *pGoodsModel, const QString &curCode, 
	const QString &parentCode, CProduct t)
{
	ProductTreeOper::delTree(tree, pGoodsModel, curCode, parentCode, t);
}
void KeyPointDefinition::addTree(QMap<QString, QList<QStandardItem *>> &tree, 
	QStandardItemModel *pGoodsModel, const QString &curCode, 
	const QString &parentCode, const CProduct &val, const QList<int> cols)
{
	ProductTreeOper::addTree(tree, pGoodsModel, curCode, parentCode, val, cols);
}

QStandardItem *KeyPointDefinition::getItem(const QMap<QString, QList<QStandardItem *>> &tree,
	const QString &code, int nVal, int role, CKeypoint t)
{
	if (tree.contains(code))
	{
		foreach(QStandardItem *p, tree[code])
		{
			if (p && p->data(role).toInt() == nVal)
				return p;
		}
	}
	return NULL;
}