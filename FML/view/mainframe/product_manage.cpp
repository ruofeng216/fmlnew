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
	connect(ui.cbParentCode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotParentCodeChanged(int)));
	connect(ui.cbParentName, SIGNAL(currentIndexChanged(int)), this, SLOT(slotParentNameChanged(int)));

	slotSkinChange();
	init();
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(QDate::currentDate());
}

ProductManage::~ProductManage()
{
}

void ProductManage::init()
{
	CProduct oldVal = getViewProduct();
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
	setViewProduct(oldVal);
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
	CProduct val = getViewProduct();
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
	CProduct val = getViewProduct();
	if (val.getCode().isEmpty() || val.getName().isEmpty()) {
		ShowWarnMessage(tr("modify"), tr("code or name is empty"), this);
		return;
	}

	CProduct oldVal;
	if (!PARASETCTL->getProduct(val.getCode(), oldVal)) {
		ShowWarnMessage(tr("modify"), tr("The product is not existing!"), this);
		return;
	}

	if (oldVal == val) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}

	if (PARASETCTL->setProduct(val)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		init();
	} else {
		ShowErrorMessage(tr("modify"), tr("modify fail."), this);
	}
}

void ProductManage::slotDelete()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CProduct val = getViewProduct();
		CProduct oldVal;
		if (!PARASETCTL->getProduct(val.getCode(), oldVal)) {
			ShowWarnMessage(tr("delete"), tr("The product is not existing!"), this);
			return;
		}
		if (PARASETCTL->removeProduct(val.getCode())) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
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
	int sdate = index.sibling(index.row(), 4).data().toInt();
	int edate = index.sibling(index.row(), 5).data().toInt();
	QString annotation = index.sibling(index.row(), 6).data().toString();
	CProduct val(code, name, parentCode, parentName, sdate, edate, annotation);
	setViewProduct(val);
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

CProduct ProductManage::getViewProduct()
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

void ProductManage::setViewProduct(const CProduct &val)
{
	ui.leCode->setText(val.getCode());
	ui.leName->setText(val.getName());
	ui.cbParentCode->setCurrentText(val.getParentCode());
	ui.cbParentName->setCurrentText(val.getParentName());
	ui.deStart->setDate(QDate::fromString(QString::number(val.getSdate()), "yyyy-MM-dd"));
	ui.deEnd->setDate(QDate::fromString(QString::number(val.getEdate()), "yyyy-MM-dd"));
	ui.pteAnnotation->setPlainText(val.getAnnotation());
}