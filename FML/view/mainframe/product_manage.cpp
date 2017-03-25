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

	slotSkinChange();
	init();
}

ProductManage::~ProductManage()
{
}

void ProductManage::init()
{
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
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(QDate::currentDate());

	connect(ui.treeView, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
		QVariant s = index.sibling(index.row(), 0).data();
		if (s.isValid()) ui.leCode->setText(s.toString());
		s = index.sibling(index.row(), 1).data();
		if (s.isValid()) ui.leName->setText(s.toString());
		s = index.sibling(index.row(), 2).data();
		if (s.isValid()) ui.leParentCode->setText(s.toString());
		s = index.sibling(index.row(), 3).data();
		if (s.isValid()) ui.leParentName->setText(s.toString());
		s = index.sibling(index.row(), 4).data();
		if (s.isValid()) ui.deStart->setDate(QDate::fromString(s.toString(), "yyyy-MM-dd"));
		s = index.sibling(index.row(), 5).data();
		if (s.isValid()) ui.deEnd->setDate(QDate::fromString(s.toString(), "yyyy-MM-dd"));
		s = index.sibling(index.row(), 6).data();
		if (s.isValid()) ui.pteAnnotation->setPlainText(s.toString());
	});
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
	QString code = ui.leCode->text();
	QString name = ui.leName->text();
	QString parentCode = ui.leParentCode->text();
	QString parentName = ui.leParentName->text();
	int sdate = ui.deStart->date().toJulianDay();
	int edate = ui.deEnd->date().toJulianDay();
	QString annotation = ui.pteAnnotation->toPlainText();
	if (code.isEmpty() || name.isEmpty()) {
		ShowWarnMessage(tr("add"), tr("code or name is empty"), this);
		return;
	}

	CProduct product(code, name, parentCode, parentName, sdate, edate, annotation);
	if (PARASETCTL->setProduct(product)) {
		qDebug() << "add product success";
	}
}

void ProductManage::slotModify()
{

}

void ProductManage::slotDelete()
{

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