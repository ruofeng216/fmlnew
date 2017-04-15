#include "product_manage.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"

ProductManage::ProductManage(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
{
	ui.setupUi(this);
	ui.treeView->setAlternatingRowColors(true);
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotTreeDoubleClicked(QModelIndex)));
	connect(ui.cbParentCode, SIGNAL(activated(int)), this, SLOT(slotParentCodeChanged(int)));
	connect(ui.cbParentName, SIGNAL(activated(int)), this, SLOT(slotParentNameChanged(int)));

	init();
	slotSkinChange();
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(QDate::currentDate());
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
	return true;
}

void ProductManage::init()
{
	CProduct oldVal = getViewData();
	{
		// 初始化treeView
		QStringList treeHeader;
		treeHeader << tr("productcode") << tr("productname") << tr("parentcode")
			<< tr("parentname") << tr("sdate") << tr("edate") << tr("annotation");

		if (m_model) {
			m_model->clear();
		} else {
			m_model = new QStandardItemModel(0, treeHeader.size(), this);
		}

		m_model->setColumnCount(treeHeader.size());
		for (int i = 0; i < treeHeader.size(); i++) {
			m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
		}
		QList<CProduct> roots = PARASETCTL->getRootProduct();
		foreach(const CProduct &root, roots) {
			QList<QStandardItem *> items = createRowItems(root);
			if (items.isEmpty()) {
				continue;
			}
			m_model->appendRow(items);
			appendChildrenProduct(items.front(), root.getCode());
		}
		
		ui.treeView->setModel(m_model);
		ui.treeView->setColumnWidth(0, 160);
		ui.treeView->expandAll();
	}

	{
		// 初始化上级产品
		ui.cbParentCode->clear();
		ui.cbParentName->clear();
		QStringList parentCodeList, parentNameList;
		parentCodeList << "";
		parentNameList << "";
		const QMap<QString, CProduct>& allProduct = PARASETCTL->getProduct();
		for (auto iter = allProduct.constBegin(); iter != allProduct.constEnd(); ++iter) {
			parentCodeList.push_back(iter.value().getCode());
			parentNameList.push_back(iter.value().getName());
		}
		ui.cbParentCode->addItems(parentCodeList);
		ui.cbParentName->addItems(parentNameList);
	}
	setViewData(oldVal);
	// 选中当前行
	QModelIndexList findIndex = m_model->match(m_model->index(0, 0), Qt::DisplayRole, oldVal.getCode(), 1, Qt::MatchRecursive);
	if (findIndex.size() > 0) {
		ui.treeView->setCurrentIndex(findIndex[0]);
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

void ProductManage::slotAdd()
{
	CProduct val = getViewData();
	if (val.getCode().isEmpty() || val.getName().isEmpty()) {
		ShowWarnMessage(tr("add"), tr("code or name is empty"), this);
		return;
	}

	CProduct oldVal;
	if (PARASETCTL->getProduct(val.getCode(), oldVal)) {
		ShowWarnMessage(tr("add"), tr("The product already exists"), this);
		return;
	}

	if (PARASETCTL->setProduct(val)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		init();
	} else {
		ShowErrorMessage(tr("add"), tr("add fail."), this);
	}
}

void ProductManage::slotModify()
{
	if (getCurrentData().getCode().isEmpty()) {
		ShowWarnMessage(tr("modify"), tr("No selected content can not be modified"), this);
		return;
	}

	CProduct newVal = getViewData();
	if (isKeyModify(newVal.getCode())) {
		ShowWarnMessage(tr("modify"), tr("product code can not be modified!"), this);
		return;
	}

	if (this->isEqual(newVal)) {
		ShowWarnMessage(tr("modify"), tr("records do not change, do not need to modify!"), this);
		return;
	}

	if (PARASETCTL->setProduct(newVal)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		init();
	} else {
		ShowErrorMessage(tr("modify"), tr("modify fail."), this);
	}
}

void ProductManage::slotDelete()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CProduct val = getViewData();
		CProduct oldVal;
		if (!PARASETCTL->getProduct(val.getCode(), oldVal)) {
			ShowWarnMessage(tr("delete"), tr("The product is not existing!"), this);
			return;
		}
		if (PARASETCTL->removeProduct(val.getCode())) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			setViewData(CProduct());
			init();
		} else {
			ShowErrorMessage(tr("delete"), tr("delete fail."), this);
		}
	}
}

void ProductManage::slotTreeDoubleClicked(const QModelIndex &index)
{
	QString code = index.sibling(index.row(), 0).data().toString();
	QString name = index.sibling(index.row(), 1).data().toString();
	QString parentCode = index.sibling(index.row(), 2).data().toString();
	QString parentName = index.sibling(index.row(), 3).data().toString();
	QString strStartDate = index.sibling(index.row(), 4).data().toString();
	QString strEndDate = index.sibling(index.row(), 5).data().toString();
	QString annotation = index.sibling(index.row(), 6).data().toString();
	int sdate = QDate::fromString(strStartDate, "yyyy-MM-dd").toJulianDay();
	int edate = QDate::fromString(strEndDate, "yyyy-MM-dd").toJulianDay();
	CProduct val(code, name, parentCode, parentName, sdate, edate, annotation);
	setViewData(val);
}

void ProductManage::slotParentCodeChanged(int index)
{
	if (ui.cbParentName->currentIndex() != index) {
		ui.cbParentName->setCurrentIndex(index);
	}
}

void ProductManage::slotParentNameChanged(int index)
{
	if (ui.cbParentCode->currentIndex() != index) {
		ui.cbParentCode->setCurrentIndex(index);
	}
}

QList<QStandardItem*> ProductManage::createRowItems(const CProduct &val)
{
	QList<QStandardItem *> items;
	if (val.getCode().isEmpty() || val.getName().isEmpty()) {
		return items;
	}
	QStandardItem *code = new QStandardItem(val.getCode());
	QStandardItem *name = new QStandardItem(val.getName());
	QStandardItem *parentCode = new QStandardItem(val.getParentCode());
	QStandardItem *parentName = new QStandardItem(val.getParentName());
	QStandardItem *sdate = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString("yyyy-MM-dd"));
	QStandardItem *edate = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString("yyyy-MM-dd"));
	QStandardItem *annotation = new QStandardItem(val.getAnnotation());
	code->setToolTip(code->text());
	name->setToolTip(name->text());
	parentCode->setToolTip(parentCode->text());
	parentName->setToolTip(parentName->text());
	sdate->setToolTip(sdate->text());
	edate->setToolTip(edate->text());
	annotation->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
	items.push_back(code);
	items.push_back(name);
	items.push_back(parentCode);
	items.push_back(parentName);
	items.push_back(sdate);
	items.push_back(edate);
	items.push_back(annotation);
	return items;
}

void ProductManage::appendChildrenProduct(QStandardItem *item, const QString &parentCode)
{
	if (!item || parentCode.isEmpty()) {
		return;
	}

	QList<CProduct> children = PARASETCTL->getChildrenProduct(parentCode);
	foreach(const CProduct &child, children) {
		QList<QStandardItem *> childItems = this->createRowItems(child);
		if (childItems.isEmpty()) {
			continue;
		}
		item->appendRow(childItems);
		appendChildrenProduct(childItems.front(), child.getCode());
	} 
}

CProduct ProductManage::getViewData()
{
	QString code = ui.leCode->text().trimmed();
	QString name = ui.leName->text().trimmed();
	QString parentCode = ui.cbParentCode->currentText().trimmed();
	QString parentName = ui.cbParentName->currentText().trimmed();
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
	ui.cbParentName->setCurrentText(val.getParentName());

	ui.deStart->setDate(val.getSdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getSdate()));
	ui.deEnd->setDate(val.getEdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getEdate()));
	ui.pteAnnotation->setPlainText(val.getAnnotation());
	setCurrentData(getViewData());
}
