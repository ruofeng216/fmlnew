#include "key_point_definition.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include "viewsignalmanager/viewsignalmanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"
#include <QDebug>
#include <QTableView>

#define FORMATDATE(T) QDate::fromJulianDay(T).toString(YMD)
#define V_INDEX (Qt::UserRole + 1)
#define V_CODE (Qt::UserRole + 2)
#define COLSKP QList<int>()<<eKPCode<<eKPName<<eProductName<<eTenor<<eCalendar<<eConvention<<eDayCount<<eCouponfrequency<<eSdate<<eEdate
#define COLSMARKET QList<int>()<<eMarket
#define COLSPRODUCT QList<int>()<<eProductCode
#define PROCODE "code"


KeyPointDefinition::KeyPointDefinition(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
	, m_pProductModel(nullptr)
{
	ui.setupUi(this);

	connect(VIEWSIGNAL, &ViewSignalManager::sigProductChange, this, &KeyPointDefinition::initProduct, Qt::QueuedConnection);
	connect(VIEWSIGNAL, &ViewSignalManager::sigParameterChange, this, &KeyPointDefinition::initParameter, Qt::QueuedConnection);
	
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(slotTreeClicked(QModelIndex)));
	
	init();
	slotSkinChange();
}

KeyPointDefinition::~KeyPointDefinition()
{
}

QString KeyPointDefinition::getKey(const CKeypoint &newVal) const
{
	return newVal.getKpcode();
}

// 提交时，检查相关控件值是否合法。
bool KeyPointDefinition::checkValid(bool opr)
{
	bool bValid = true;
	CKeypoint kp = getViewData();
	if (kp.getKpcode().isEmpty())
	{
		ui.leKPCode->setError(tr("KeyPointCode cant be empty!"));
		bValid = false;
	}
	if (kp.getKpname().isEmpty())
	{
		ui.leKPName->setError(tr("KeyPointName cant be empty!"));
		bValid = false;
	}

	CProduct cp;
	bool bexist = PARASETCTL->getProduct(ui.cbProductCode->currentText().trimmed(), cp);
	if (kp.getProductCode().isEmpty()) 
	{
		ui.cbProductCode->setError(tr("ProductCode cant be empty!"));
		bValid = false;
	}
	if (!bexist && opr)
	{
		ui.cbProductCode->setError(tr("ProductCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}
	if (kp.getProductName().isEmpty())
	{
		ui.cbProductName->setError(tr("ProductName cant be empty!"));
		bValid = false;
	}
	if (bexist && cp.getName() != kp.getProductName())
	{
		ui.cbProductName->setError(tr("ProductName cant be changed!"));
		bValid = false;
	}
	if (ui.leTenor->text().isEmpty())
	{
		ui.leTenor->setError(tr("ProductCode cant be empty!"));
		bValid = false;
	}
	if (kp.getEdate() < kp.getSdate())
	{
		ui.deEnd->setError(tr("end-time cant be small than start-time!"));
		bValid = false;
	}
	if (!PARASETCTL->getParadict().contains(kp.getMarketCode()) && opr)
	{
		ui.cbMarket->setError(tr("MarketCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}
	if (!PARASETCTL->getParadict().contains(kp.getCalendarCode()) && opr)
	{
		ui.cbCalendar->setError(tr("CalendarCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}
	if (!PARASETCTL->getParadict().contains(kp.getConventionCode()) && opr)
	{
		ui.cbConvention->setError(tr("ConventionCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}
	if (!PARASETCTL->getParadict().contains(kp.getDayCountCode()) && opr)
	{
		ui.cbDayCount->setError(tr("DayCountCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}
	if (!PARASETCTL->getParadict().contains(kp.getSpotlatCode()) && opr)
	{
		ui.cbSpotlag->setError(tr("SpotlatCode dont exist now, cant add/modify with this code!"));
		bValid = false;
	}

	return bValid;
}

void KeyPointDefinition::init()
{
	ui.leTenor->setValidator(new QIntValidator(0, 99999, this));
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
	{
		ui.treeView->setAlternatingRowColors(true);
		// 初始化treeView
		QStringList treeHeader;
		treeHeader << tr("kpcode") << tr("kpname") << tr("productname") << tr("tenor") << tr("calendar")
			<< tr("convention") << tr("daycount") << tr("Couponfrequency") << tr("startdate") << tr("enddate");
		if (m_model) {
			m_model->clear();
		} else {
			m_model = new QStandardItemModel(0, treeHeader.size(), this);
		}
		m_model->setColumnCount(treeHeader.size());
		for (int i = 0; i < treeHeader.size(); i++) 
			m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);

		const QMap<QString, CKeypoint>& data = YIELDCURVECTL->getKeyPoint();
		for (auto iter = data.constBegin(); iter != data.constEnd(); ++iter) 
		{
			if (!m_tree.contains(iter->getMarketCode()))
				addTree(m_tree, m_model, iter->getMarketCode(), "", iter.value(), COLSMARKET);
			addTree(m_tree, m_model, iter->getKpcode(), iter->getMarketCode(), iter.value(), COLSKP);
		}
		ui.treeView->setModel(m_model);
		ui.treeView->setColumnWidth(0, 160);
		ui.treeView->expandAll();
		bwClear(CKeypoint());
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
	if (!checkValid()) return;

	CKeypoint val = getViewData();

	CKeypoint oldVal;
	if (YIELDCURVECTL->getKeyPoint(val.getKpcode(), oldVal)) {
		ShowWarnMessage(tr("add"), tr("The kpcode already exists"), this);
		return;
	}

	QString err;
	if (YIELDCURVECTL->setKeyPoint(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		// 同步
		if (!m_tree.contains(val.getMarketCode()))
			addTree(m_tree, m_model, val.getMarketCode(), "", val, COLSMARKET);
		addTree(m_tree, m_model, val.getKpcode(), val.getMarketCode(), val, COLSKP);
		KeypointTreeOper::locator(m_tree, val.getKpcode());
	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void KeyPointDefinition::slotModify()
{
	if (!checkValid()) return;

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

	QString err;
	if (YIELDCURVECTL->setKeyPoint(val,err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		if (!m_tree.contains(val.getMarketCode()))
			addTree(m_tree, m_model, val.getMarketCode(), "", val, COLSMARKET);
		addTree(m_tree, m_model, val.getKpcode(), val.getMarketCode(), val, COLSKP);
		KeypointTreeOper::locator(m_tree, val.getKpcode());
	} else {
		ShowErrorMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
}

void KeyPointDefinition::slotDelete()
{
	if (!checkValid(false)) return;

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
			KeypointTreeOper::delTree(m_tree, m_model, val.getKpcode(), val.getMarketCode());
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
		
	}
}

void KeyPointDefinition::slotTreeClicked(const QModelIndex &index)
{
	auto getChildData = [this, &index](int col, int &nIndex, QString &text, QString &data) {
		text = ""; data = ""; nIndex = 0;
		QModelIndex sibling = index.sibling(index.row(), col);
		QStandardItem *item = m_model->itemFromIndex(sibling);
		if (item) {
			nIndex = item->data(V_INDEX).toInt();
			data = item->data(V_CODE).toString();
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
			data = item->data(V_CODE).toString();
		}
	};

	QString text, data; // text:UI上展示的名称，data：对应的代码
	CKeypoint val;
	QString startDate, endDate;
	getParentData(text, data);
	val.setMarketCode(data);
	val.setMarketName(text);
	if (index.parent().isValid()) 
	{
		QModelIndex parentIndex = index.parent();
		if (index.parent().isValid()) {
			parentIndex = parentIndex.sibling(parentIndex.row(), 0);
		}
		else {
			parentIndex = index.sibling(index.row(), 0);
		}
		QStandardItem *item = m_model->itemFromIndex(parentIndex);
		if (item)
		{
			int nIndex = 0;
			for (int i = 0; i < item->columnCount(); i++)
			{
				getChildData(i, nIndex, text, data);
				if (nIndex == eKPCode)
				{
					val.setKpcode(text);
				}
				else if (nIndex == eKPName)
				{
					val.setKpname(text);
				}
				else if (nIndex == eProductName)
				{
					val.setProductName(text);
					val.setProductCode(data);
				}
				else if (nIndex == eTenor)
				{
					val.setTenor(text);
				}
				else if (nIndex == eCalendar)
				{
					val.setCalendarName(text);
					val.setCalendarCode(data);
				}
				else if (nIndex == eConvention)
				{
					val.setConventionName(text);
					val.setConventionCode(data);
				}
				else if (nIndex == eDayCount)
				{
					val.setDayCountName(text);
					val.setDayCountCode(data);
				}
				else if (nIndex == eCouponfrequency)
				{
					val.setSpotlatName(text);
					val.setSpotlatCode(data);
				}
				else if (nIndex == eSdate)
				{
					int sdate = QDate::fromString(text, YMD).toJulianDay();
					val.setSdate(sdate);
				}
				else if (nIndex == eEdate)
				{
					int edate = QDate::fromString(text, YMD).toJulianDay();
					val.setEdate(edate);
				}
			}
		}
	}
	setViewData(val);
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
	ui.deStart->setDate(val.getSdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getSdate()));
	ui.deEnd->setDate(val.getEdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getEdate()));
	ui.cbMarket->setCurrentText(val.getMarketName());
	ui.cbMarket->setProperty(PROCODE, val.getMarketCode());
	ui.cbCalendar->setCurrentText(val.getCalendarName());
	ui.cbCalendar->setProperty(PROCODE, val.getCalendarCode());
	ui.cbConvention->setCurrentText(val.getConventionName());
	ui.cbConvention->setProperty(PROCODE, val.getConventionCode());
	ui.cbSpotlag->setCurrentText(val.getSpotlatName());
	ui.cbSpotlag->setProperty(PROCODE, val.getSpotlatCode());
	ui.cbDayCount->setCurrentText(val.getDayCountName());
	ui.cbDayCount->setProperty(PROCODE, val.getDayCountCode());

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
	QString code = ui.cbMarket->property(PROCODE).toString();
	if (PARASETCTL->getParadict().contains(code)) code = "";
	result.setMarketCode(code.isEmpty()?ui.cbMarket->currentData().toString().trimmed():code);
	result.setMarketName(ui.cbMarket->currentText().trimmed());
	code = ui.cbCalendar->property(PROCODE).toString();
	if (PARASETCTL->getParadict().contains(code)) code = "";
	result.setCalendarCode(code.isEmpty() ?ui.cbCalendar->currentData().toString().trimmed() :code);
	result.setCalendarName(ui.cbCalendar->currentText().trimmed());
	code = ui.cbConvention->property(PROCODE).toString();
	if (PARASETCTL->getParadict().contains(code)) code = "";
	result.setConventionCode(code.isEmpty() ?ui.cbConvention->currentData().toString().trimmed() :code);
	result.setConventionName(ui.cbConvention->currentText().trimmed());
	code = ui.cbSpotlag->property(PROCODE).toString();
	if (PARASETCTL->getParadict().contains(code)) code = "";
	result.setSpotlatCode(code.isEmpty() ?ui.cbSpotlag->currentData().toString().trimmed() :code);
	result.setSpotlatName(ui.cbSpotlag->currentText().trimmed());
	code = ui.cbDayCount->property(PROCODE).toString();
	if (PARASETCTL->getParadict().contains(code)) code = "";
	result.setDayCountCode(code.isEmpty() ?ui.cbDayCount->currentData().toString().trimmed() :code);
	result.setDayCountName(ui.cbDayCount->currentText().trimmed());
	result.setSdate(ui.deStart->date().toJulianDay());
	result.setEdate(ui.deEnd->date().toJulianDay());
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
	m_pProductModel->clear();

	QMap<QString, QList<QStandardItem *>> _treeCombobox;

	QMap<QString, CProduct> val = PARASETCTL->getProduct();
	for (QMap<QString, CProduct>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		addTree(_treeCombobox, m_pProductModel, itor.value().getCode(), itor.value().getParentCode(), itor.value(), COLSPRODUCT);
	}
	
	((QTreeView *)(ui.cbProductCode->view()))->expandAll();
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
			p->setData(i, V_INDEX);
			p->setData(val.getMarketCode(), V_CODE);
			break;
		}
		case eKPName:
		{
			QStandardItem *p = new QStandardItem(val.getKpname());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			break;
		}
		case eProductCode:
		{
			QStandardItem *p = new QStandardItem(val.getProductCode());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			break;
		}
		case eProductName:
		{
			QStandardItem *p = new QStandardItem(val.getProductName());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			p->setData(val.getProductCode(), V_CODE);
			break;
		}
		case eTenor:
		{
			QStandardItem *p = new QStandardItem(val.getTenor());
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			p->setData(val.getTenor(), V_CODE);
			break;
		}
		case eSdate:
		{
			QStandardItem *p = new QStandardItem(FORMATDATE(val.getSdate()));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			break;
		}
		case eEdate:
		{
			QStandardItem *p = new QStandardItem(FORMATDATE(val.getEdate()));
			childItems.push_back(p);
			p->setToolTip(p->text());
			p->setData(i, V_INDEX);
			break;
		}
		case eMarket:
		{
			QStandardItem *p = new QStandardItem(val.getMarketName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getMarketName(), 200));
			p->setData(i, V_INDEX);
			p->setData(val.getMarketCode(), V_CODE);
			break;
		}
		case eCalendar:
		{
			QStandardItem *p = new QStandardItem(val.getCalendarName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getCalendarName(), 200));
			p->setData(i, V_INDEX);
			p->setData(val.getCalendarCode(), V_CODE);
			break;
		}
		case eConvention:
		{
			QStandardItem *p = new QStandardItem(val.getConventionName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getConventionName(), 200));
			p->setData(i, V_INDEX);
			p->setData(val.getConventionCode(), V_CODE);
			break;
		}
		case eDayCount:
		{
			QStandardItem *p = new QStandardItem(val.getDayCountName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getDayCountName(), 200));
			p->setData(i, V_INDEX);
			p->setData(val.getDayCountCode(), V_CODE);
			break;
		}
		case eCouponfrequency:
		{
			QStandardItem *p = new QStandardItem(val.getSpotlatName());
			childItems.push_back(p);
			p->setToolTip(qutil::splitTooltip(val.getSpotlatName(), 200));
			p->setData(i, V_INDEX);
			p->setData(val.getSpotlatCode(), V_CODE);
			break;
		}
		default:
			break;
		}
	}
}
void KeyPointDefinition::updateChildNode(const CKeypoint &val)
{
	if (m_tree.contains(val.getKpcode()))
	{
		QStandardItem *pMarket = getItem(m_tree, val.getKpcode(), eKPCode, V_INDEX);
		if (pMarket && val.getMarketCode() != pMarket->data(V_CODE).toString()) {

			QString code = val.getKpcode();
			QString oldMarket = pMarket->data(V_CODE).toString();
			
			KeypointTreeOper::delTree(m_tree, m_model, code, oldMarket);
			if (m_tree.contains(oldMarket) &&
				!m_tree[oldMarket].isEmpty() &&
				m_tree[oldMarket].front()->rowCount() == 0)
			{
				KeypointTreeOper::delTree(m_tree, m_model, code, "");
			}
			
			if (!m_tree.contains(val.getMarketCode()) && recordExist(val.getMarketCode(), CKeypoint()))
				addTree(m_tree, m_model, val.getMarketCode(), "", val, COLSMARKET);
			addTree(m_tree, m_model, code, val.getMarketCode(), val, COLSKP);
		}
		else if (pMarket) {
			QStandardItem *p = getItem(m_tree, val.getKpcode(), eKPName, V_INDEX);
			if (p)
			{
				p->setText(val.getKpname());
				p->setToolTip(val.getKpname());
			}
			p = getItem(m_tree, val.getKpcode(), eProductName, V_INDEX);
			if (p)
			{
				p->setText(val.getProductName());
				p->setToolTip(val.getProductName());
				p->setData(val.getProductCode(), V_CODE);
			}
			p = getItem(m_tree, val.getKpcode(), eTenor, V_INDEX);
			if (p)
			{
				p->setText(val.getTenor());
				p->setToolTip(val.getTenor());
			}
			p = getItem(m_tree, val.getKpcode(), eCalendar, V_INDEX);
			if (p)
			{
				p->setText(val.getCalendarName());
				p->setToolTip(val.getCalendarName());
			}
			p = getItem(m_tree, val.getKpcode(), eConvention, V_INDEX);
			if (p)
			{
				p->setText(val.getConventionName());
				p->setToolTip(val.getConventionName());
			}
			p = getItem(m_tree, val.getKpcode(), eDayCount, V_INDEX);
			if (p)
			{
				p->setText(val.getDayCountName());
				p->setToolTip(val.getDayCountName());
			}
			p = getItem(m_tree, val.getKpcode(), eCouponfrequency, V_INDEX);
			if (p)
			{
				p->setText(val.getSpotlatName());
				p->setToolTip(val.getSpotlatName());
			}
			p = getItem(m_tree, val.getKpcode(), eSdate, V_INDEX);
			if (p)
			{
				p->setText(FORMATDATE(val.getSdate()));
				p->setToolTip(FORMATDATE(val.getSdate()));
			}
			p = getItem(m_tree, val.getKpcode(), eEdate, V_INDEX);
			if (p)
			{
				p->setText(FORMATDATE(val.getEdate()));
				p->setToolTip(FORMATDATE(val.getEdate()));
			}
		}
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
	if (cols.contains(eProductCode))
	{
		QStandardItem *p = new QStandardItem(val.getCode());
		childItems.push_back(p);
		p->setToolTip(p->text());
		p->setData(eProductCode, V_CODE);
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

void KeyPointDefinition::addTree(QMap<QString, QList<QStandardItem *>> &tree, 
	QStandardItemModel *pGoodsModel, const QString &curCode, 
	const QString &parentCode, const CProduct &val, const QList<int> cols)
{
	ProductTreeOper::addTree(tree, pGoodsModel, curCode, parentCode, val, cols);
}

QStandardItem *KeyPointDefinition::getItem(QMap<QString, QList<QStandardItem *>> &tree,
	const QString &code, int nVal, int role, CKeypoint t)
{
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