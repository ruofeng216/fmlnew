#include "product_manage.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"

ProductManage::ProductManage(QWidget *parent)
	: BodyWidget(parent)
	, m_pGoodsModel(NULL)
	, m_pGoodsModelCombobox(NULL)
{
	ui.setupUi(this);
	connect(ui.pbAdd, &QPushButton::clicked, this, &ProductManage::slotAdd);
	connect(ui.pbModify, &QPushButton::clicked, this, &ProductManage::slotModify);
	connect(ui.pbDelete, &QPushButton::clicked, this, &ProductManage::slotDelete);
	init();
	slotSkinChange();
}

ProductManage::~ProductManage()
{
}

QString ProductManage::getKey(const CProduct &newVal) const
{
	return newVal.getCode();
}

// 提交时，检查相关控件值是否合法。
bool ProductManage::checkValid()
{
	bool bValid = true;
	if (ui.leCode->text().isEmpty())
	{
		ui.leCode->setError(tr("code cant be empty!"));
		bValid = false;
	}
	if (ui.leName->text().isEmpty())
	{
		ui.leName->setError(tr("code-name cant be empty!"));
		bValid = false;
	}
	if (ui.deEnd->date().toJulianDay() < ui.deStart->date().toJulianDay())
	{
		ui.deEnd->setError(tr("end-time cant be small than start-time!"));
		bValid = false;
	}
	if (ui.leCode->text() == ui.cbParentCode->currentText()) {
		ui.leCode->setError(tr("product code equal parent code error"));
		ui.cbParentCode->setError(tr("product code equal parent code error"));
		bValid = false;
	}
	return bValid;
}

void ProductManage::init()
{
	{
		//QTreeView *pView = new QTreeView(ui.cbParentCode);
		if (!m_pGoodsModelCombobox) m_pGoodsModelCombobox = new QStandardItemModel(0, 1, this);
		m_pGoodsModelCombobox->setColumnCount(1);
		ui.cbParentCode->setModel(m_pGoodsModelCombobox);
		//pView->setHeaderHidden(true);
		//ui.cbParentCode->setView(pView);
		//ui.cbParentCode->view()->setAlternatingRowColors(true);
		connect(ui.cbParentCode, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
			[this](const QString &text) {
			if (PARASETCTL->getProduct().contains(text))
				ui.lineEditParentName->setText(PARASETCTL->getProduct()[text].getName());
			else
				ui.lineEditParentName->setText("");
		});
	}
	{
		ui.treeView->setAlternatingRowColors(true);
		connect(ui.treeView, &QTreeView::clicked, [this](const QModelIndex &index) {
			QString code = index.sibling(index.row(), eProductCode).data().toString();
			QString name = index.sibling(index.row(), eProductName).data().toString();
			QString parentCode = index.sibling(index.row(), eParentcode).data().toString();
			QString parentName = index.sibling(index.row(), eParentname).data().toString();
			QString strStartDate = index.sibling(index.row(), eSdate).data().toString();
			QString strEndDate = index.sibling(index.row(), eEdate).data().toString();
			QString annotation = index.sibling(index.row(), eAnnotation).data().toString();
			int sdate = QDate::fromString(strStartDate, YMD).toJulianDay();
			int edate = QDate::fromString(strEndDate, YMD).toJulianDay();
			CProduct val(code, name, parentCode, parentName, sdate, edate, annotation);
			setViewData(val);
		});

		initDateView();
	}
}

void ProductManage::slotSkinChange()
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

void ProductManage::slotAdd(bool)
{
	if (!checkValid()) return;
	CProduct val = getViewData();
	if (PARASETCTL->getProduct().contains(val.getCode())) {
		ShowWarnMessage(tr("add"), tr("The product already exists"), this);
		return;
	}
	if (!val.getParentCode().isEmpty() && !PARASETCTL->getProduct().contains(val.getParentCode()))
	{
		ShowWarnMessage(tr("add"), tr("the parent dont exists.").arg(val.getParentCode()), this);
		return;
	}

	QList<int> cols = QList<int>{ 0,1,2,3,4,5,6,7 };
	QList<int> colsComboBox = QList<int>{ 0 };

	QString err;
	if (PARASETCTL->setProduct(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		// 同步
		//addProductData(val);
		addTree(m_tree, m_pGoodsModel, val.getCode(), val.getParentCode(), val, cols);
		addTree(m_treeCombobox, m_pGoodsModelCombobox, val.getCode(), val.getParentCode(), val, colsComboBox);

		//locateProductData(val);
		locator(m_tree, val.getCode());

	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void ProductManage::slotModify(bool)
{
	if (!checkValid()) return;

	CProduct newVal = getViewData();
	if (isKeyModify(newVal.getCode())) {
		ShowWarnMessage(tr("modify"), tr("product code can not be modified!"), this);
		return;
	}

	if (this->isEqual(newVal)) {
		ShowWarnMessage(tr("modify"), tr("records do not change, do not need to modify!"), this);
		return;
	}

	if (!PARASETCTL->getChildrenProduct(newVal.getCode()).isEmpty())
	{
		if (getCurrentData().getParentCode() != newVal.getParentCode())
			ShowWarnMessage(tr("delete"), tr("Cant modify parent-code, when it has children!"), this);
		return;
	}
	if (!newVal.getParentCode().isEmpty() && !PARASETCTL->getProduct().contains(newVal.getParentCode()))
	{
		ShowWarnMessage(tr("add"), tr("the parent dont exists.").arg(newVal.getParentCode()), this);
		return;
	}

	QList<int> cols = QList<int>{ 0,1,2,3,4,5,6,7 };
	QList<int> colsComboBox = QList<int>{ 0 };


	QString err;
	if (PARASETCTL->setProduct(newVal, err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		// 同步
		//addProductData(newVal);
		addTree(m_tree, m_pGoodsModel, newVal.getCode(), newVal.getParentCode(), newVal, cols);
		addTree(m_treeCombobox, m_pGoodsModelCombobox, newVal.getCode(), newVal.getParentCode(), newVal, colsComboBox);

		//locateProductData(newVal);
		locator(m_tree, newVal.getCode());
	} else {
		ShowErrorMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
}

void ProductManage::slotDelete(bool)
{
	if (!checkValid()) return;
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) 
	{
		CProduct val = getViewData();
		if (!PARASETCTL->getChildrenProduct(val.getCode()).isEmpty())
		{
			ShowWarnMessage(tr("delete"), tr("Cant delete parent-code, when it has children!"), this);
			return;
		}
		if (!PARASETCTL->getProduct().contains(val.getCode())) {
			ShowWarnMessage(tr("delete"), tr("The product is not existing!"), this);
			return;
		}
		QString err;
		if (PARASETCTL->removeProduct(val.getCode(), err)) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			// 同步
			//delProductData(val);
			delTree(m_tree, m_pGoodsModel, val.getCode(), val.getParentCode());
			delTree(m_treeCombobox, m_pGoodsModelCombobox, val.getCode(), val.getParentCode());
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void ProductManage::initDateView()
{
	QList<int> cols = QList<int>{ 0,1,2,3,4,5,6,7 };
	QList<int> colsComboBox = QList<int>{ 0 };

	if (m_pGoodsModel) m_pGoodsModel->clear();
	if (m_pGoodsModelCombobox) m_pGoodsModelCombobox->clear();
	m_pGoodsModelCombobox->appendRow(new QStandardItem(""));

	QStringList treeHeader;
	treeHeader << tr("productcode") << tr("productname") << tr("parentcode")
		<< tr("parentname") << tr("sdate") << tr("edate") << tr("annotation");
	if (!m_pGoodsModel) m_pGoodsModel = new QStandardItemModel(0, treeHeader.size(), this);
	m_pGoodsModel->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_pGoodsModel->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_pGoodsModel);
	ui.treeView->setColumnWidth(0, 200);

	QMap<QString, CProduct> val = PARASETCTL->getProduct();
	for (QMap<QString, CProduct>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		//addProductData(itor.value());
		addTree(m_tree, m_pGoodsModel, itor.value().getCode(), itor.value().getParentCode(), itor.value(), cols);
		addTree(m_treeCombobox, m_pGoodsModelCombobox, itor.value().getCode(), itor.value().getParentCode(), itor.value(), colsComboBox);
	}
	if (!val.isEmpty()) 
	{
		//locateProductData(val[val.keys().back()]);
		locator(m_tree, val[val.keys().back()].getCode());
	}
	else
	{
		bwClear();
	}
}


CProduct ProductManage::getViewData()
{
	QString code = ui.leCode->text().trimmed();
	QString name = ui.leName->text().trimmed();
	QString parentCode = ui.cbParentCode->currentText().trimmed();
	QString parentName = !parentCode.isEmpty() && PARASETCTL->getProduct().contains(parentCode)? PARASETCTL->getProduct()[parentCode].getName() : "";
	int sdate = ui.deStart->date().toJulianDay();
	int edate = ui.deEnd->date().toJulianDay();
	QString annotation = ui.pteAnnotation->toPlainText().trimmed();
	return CProduct(code, name, parentCode, parentName, sdate, edate, annotation);
}

void ProductManage::setViewData(const CProduct &val)
{
	ui.leCode->setText(val.getCode());
	ui.leName->setText(val.getName());
	ui.cbParentCode->setCurrentText(val.getParentCode());
	ui.lineEditParentName->setText(val.getParentName());

	ui.deStart->setDate(val.getSdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getSdate()));
	ui.deEnd->setDate(val.getEdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getEdate()));
	ui.pteAnnotation->setPlainText(val.getAnnotation());
	setCurrentData(getViewData());
}


//********************************************************************
//* defime BWTreeOper virtual function  here
//********************************************************************
void ProductManage::bwLocate(const QString &code) {
	QModelIndexList findIndex = this->m_pGoodsModel->match(this->m_pGoodsModel->index(0, 0), Qt::DisplayRole, code, 1, Qt::MatchRecursive | Qt::MatchExactly);
	if (findIndex.size() > 0)
	{
		this->ui.treeView->setCurrentIndex(findIndex[eProductCode]);
		this->ui.treeView->clicked(findIndex[eProductCode]);
	}
	else
	{
		this->bwClear();
	}
}

void ProductManage::bwClear() {
	ui.leCode->setText("");
	ui.leName->setText("");
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(ui.deEnd->maximumDate());
	ui.cbParentCode->setCurrentText("");
	ui.lineEditParentName->setText("");
	ui.pteAnnotation->setPlainText("");

	setCurrentData(getViewData());
}

bool ProductManage::recordExist(const QString &val) {
	if (PARASETCTL->getProduct().contains(val))
		return true;
	else
		return false;
}

CProduct ProductManage::getTFromDB(const QString &code, QString &parentCode) {
	CProduct val;
	val = PARASETCTL->getProduct()[code];
	parentCode = val.getParentCode();
	return val;
}

void ProductManage::packQStandardItem(QList<QStandardItem *> &childItems, const CProduct &val, const QList<int> cols) {

	if (val.getCode().isEmpty() || val.getName().isEmpty()) {
		return;
	}
	for (int i = 0; i < eEnd; i++)
	{
		switch (i)
		{
		case eProductCode:
		{
			QStandardItem *code = new QStandardItem(val.getCode());
			if(cols.contains(eProductCode))childItems.push_back(code);
			code->setToolTip(code->text());
			break;
		}
		case eProductName:
		{
			QStandardItem *name = new QStandardItem(val.getName());
			if(cols.contains(eProductName))childItems.push_back(name);
			name->setToolTip(name->text());
			break;
		}
		case eParentcode:
		{
			QStandardItem *parentCode = new QStandardItem(val.getParentCode());
			if(cols.contains(eParentcode))childItems.push_back(parentCode);
			parentCode->setToolTip(parentCode->text());
			break;
		}
		case eParentname:
		{
			QStandardItem *parentName = new QStandardItem(val.getParentName());
			if(cols.contains(eParentname))childItems.push_back(parentName);
			parentName->setToolTip(parentName->text());
			break;
		}
		case eSdate:
		{
			QStandardItem *sdate = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			if(cols.contains(eSdate))childItems.push_back(sdate);
			sdate->setToolTip(sdate->text());
			break;
		}
		case eEdate:
		{
			QStandardItem *edate = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString(YMD));
			if(cols.contains(eEdate))childItems.push_back(edate);
			edate->setToolTip(edate->text());
			break;
		}
		case eAnnotation:
		{
			QStandardItem *annotation = new QStandardItem(val.getAnnotation());
			if(cols.contains(eAnnotation))childItems.push_back(annotation);
			annotation->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
			break;
		}
		default:
			break;
		}
	}
}

void ProductManage::updateChildNode(const CProduct &val){
	
	QList<int> cols = QList<int>{ 0,1,2,3,4,5,6,7};
	QList<int> colsComboBox = QList<int>{ 0 };

	if (this->m_tree.contains(val.getCode()))
	{
		if (val.getParentCode() != this->m_tree[val.getCode()][eParentcode]->text())
		{
			CProduct del(this->m_tree[val.getCode()][eProductCode]->text(),
				this->m_tree[val.getCode()][eProductName]->text(),
				this->m_tree[val.getCode()][eParentcode]->text(),
				this->m_tree[val.getCode()][eParentname]->text());
			//this->delProductData(del);
			delTree(m_tree, m_pGoodsModel, del.getCode(), del.getParentCode());
			delTree(m_treeCombobox, m_pGoodsModelCombobox, del.getCode(), del.getParentCode());

			//this->addProductData(val);
			addTree(m_tree, m_pGoodsModel, val.getCode(), val.getParentCode(),val, cols);
			addTree(m_treeCombobox, m_pGoodsModelCombobox, val.getCode(), val.getParentCode(),val,colsComboBox);

		}
		else if (this->m_tree[val.getCode()].size() == eEnd) {
			this->m_tree[val.getCode()][eProductName]->setText(val.getName());
			this->m_tree[val.getCode()][eParentcode]->setText(val.getParentCode());
			this->m_tree[val.getCode()][eParentname]->setText(val.getParentName());
			this->m_tree[val.getCode()][eSdate]->setText(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			this->m_tree[val.getCode()][eEdate]->setText(QDate::fromJulianDay(val.getEdate()).toString(YMD));
			this->m_tree[val.getCode()][eAnnotation]->setText(val.getAnnotation());

			this->m_tree[val.getCode()][eProductName]->setToolTip(val.getName());
			this->m_tree[val.getCode()][eParentcode]->setToolTip(val.getParentCode());
			this->m_tree[val.getCode()][eParentname]->setToolTip(val.getParentName());
			this->m_tree[val.getCode()][eSdate]->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			this->m_tree[val.getCode()][eEdate]->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			this->m_tree[val.getCode()][eAnnotation]->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
		}
	}
}