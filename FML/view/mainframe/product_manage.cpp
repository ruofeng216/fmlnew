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
	return bValid;
}

void ProductManage::init()
{
	{
		QTreeView *pView = new QTreeView(ui.cbParentCode);
		if (!m_pGoodsModelCombobox) m_pGoodsModelCombobox = new QStandardItemModel(0, 1, this);
		m_pGoodsModelCombobox->setColumnCount(1);
		ui.cbParentCode->setModel(m_pGoodsModelCombobox);
		pView->setHeaderHidden(true);
		ui.cbParentCode->setView(pView);
		ui.cbParentCode->view()->setAlternatingRowColors(true);
		connect(ui.cbParentCode, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
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
	QString err;
	if (PARASETCTL->setProduct(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		// 同步
		addProductData(val);
		locateProductData(val);
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
	QString err;
	if (PARASETCTL->setProduct(newVal, err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		// 同步
		addProductData(newVal);
		locateProductData(newVal);
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
			delProductData(val);
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void ProductManage::initDateView()
{
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
		addProductData(itor.value());
	}
	if (!val.isEmpty()) 
	{
		locateProductData(val[val.keys().back()]);
	}
	else
	{
		clear();
	}
}

void ProductManage::packItem(QList<QStandardItem *> &childItems, const CProduct &val)
{
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
			childItems.push_back(code);
			code->setToolTip(code->text());
			break;
		}
		case eProductName:
		{
			QStandardItem *name = new QStandardItem(val.getName());
			childItems.push_back(name);
			name->setToolTip(name->text());
			break;
		}
		case eParentcode:
		{
			QStandardItem *parentCode = new QStandardItem(val.getParentCode());
			childItems.push_back(parentCode);
			parentCode->setToolTip(parentCode->text());
			break;
		}
		case eParentname:
		{
			QStandardItem *parentName = new QStandardItem(val.getParentName());
			childItems.push_back(parentName);
			parentName->setToolTip(parentName->text());
			break;
		}
		case eSdate:
		{
			QStandardItem *sdate = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString(YMD));
			childItems.push_back(sdate);
			sdate->setToolTip(sdate->text());
			break;
		}
		case eEdate:
		{
			QStandardItem *edate = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString(YMD));
			childItems.push_back(edate);
			edate->setToolTip(edate->text());
			break;
		}
		case eAnnotation:
		{
			QStandardItem *annotation = new QStandardItem(val.getAnnotation());
			childItems.push_back(annotation);
			annotation->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
			break;
		}
		default:
			break;
		}
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

void ProductManage::addProductData(const CProduct & val)
{
	auto insertRoot = [this](const CProduct &val) {
		QList<QStandardItem *> items;
		QList<QStandardItem *> itemsCombobox;
		this->packItem(items, val);
		if (items.size() > 0) {
			this->m_pGoodsModel->appendRow(items);
			this->m_tree[val.getCode()] = items;
		}
		this->packItem(itemsCombobox, val);
		if (itemsCombobox.size() > 0) {
			this->m_pGoodsModelCombobox->appendRow(itemsCombobox.front());
			this->m_treeCombobox[val.getCode()] = itemsCombobox;
		}
	};
	auto insertChild = [this](const CProduct &val) {
		if (!this->m_tree.contains(val.getCode()) && this->m_tree.contains(val.getParentCode()))
		{
			QList<QStandardItem *> items;
			this->packItem(items, val);
			if (items.size() > 0) {
				this->m_tree[val.getParentCode()].front()->appendRow(items);
				this->m_tree[val.getCode()] = items;
			}
			QList<QStandardItem *> itemsCombobox;
			this->packItem(itemsCombobox, val);
			if (itemsCombobox.size() > 0) {
				this->m_treeCombobox[val.getParentCode()].front()->appendRow(itemsCombobox.front());
				this->m_treeCombobox[val.getCode()] = itemsCombobox;
			}
		}
	};
	auto updateChild = [this](const CProduct &val) {
		if (this->m_tree.contains(val.getCode()))
		{
			if (val.getParentCode() != this->m_tree[val.getCode()][eParentcode]->text())
			{
				CProduct del(this->m_tree[val.getCode()][eProductCode]->text(),
					this->m_tree[val.getCode()][eProductName]->text(),
					this->m_tree[val.getCode()][eParentcode]->text(),
					this->m_tree[val.getCode()][eParentname]->text());
				this->delProductData(del);
				this->addProductData(val);
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
	};
	QString rootCode = val.getParentCode();
	QString curCode = val.getCode();
	if (rootCode.isEmpty())
	{// 根节点
		if (m_tree.contains(curCode))
		{
			if (m_tree[curCode].isEmpty())
			{
				insertRoot(val);
			}
			else
			{
				updateChild(val);
			}
		}
		else
		{
			insertRoot(val);
		}
	}
	else
	{// 子节点
		if (m_tree.contains(rootCode) && !m_tree[rootCode].isEmpty())
		{// 父节点存在
			if (m_tree.contains(curCode))
			{
				updateChild(val);
			}
			else
			{
				insertChild(val);
			}
		}
		else
		{// 父节点不存在
			if (PARASETCTL->getProduct().contains(rootCode))
			{// 父节点有效
				CProduct parentCode = PARASETCTL->getProduct()[rootCode];
				addProductData(parentCode);
				insertChild(val);
			}
			else
			{
				qWarning() << "Do not exist parent-code: " << rootCode;
			}
		}
	}
}
void ProductManage::delProductData(const CProduct & val)
{
	auto delRoot = [this](const QString &val) {
		if (this->m_tree.contains(val))
		{
			this->m_pGoodsModel->removeRow(m_tree[val].front()->row());
			this->m_tree.remove(val);
		}
		if (this->m_treeCombobox.contains(val))
		{
			this->m_pGoodsModelCombobox->removeRow(m_treeCombobox[val].front()->row());
			this->m_treeCombobox.remove(val);
		}
	};
	auto delChild = [this](const QString &val) {
		if (this->m_tree.contains(val))
		{
			this->m_pGoodsModel->removeRow(m_tree[val].front()->row(), m_tree[val].front()->parent()->index());
			this->m_tree.remove(val);
		}
		if (this->m_treeCombobox.contains(val))
		{
			this->m_pGoodsModelCombobox->removeRow(m_treeCombobox[val].front()->row(), m_treeCombobox[val].front()->parent()->index());
			this->m_treeCombobox.remove(val);
		}
	};
	if (val.getParentCode().isEmpty())
	{// 根节点
		if (m_pGoodsModel->rowCount() <= 1)
		{
			clear();
		}
		else
		{
			if (m_tree.contains(val.getCode()))
			{
				int curRow = m_tree[val.getCode()].front()->row();
				int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
				QStandardItem* p = m_pGoodsModel->item(nearRow);
				if (p)
				{
					QString nearCode = p->text();
					if (PARASETCTL->getProduct().contains(nearCode))
						locateProductData(PARASETCTL->getProduct()[nearCode]);
				}
			}
		}
		delRoot(val.getCode());
	}
	else
	{
		if (m_tree.contains(val.getParentCode()) && m_tree[val.getParentCode()].front()->rowCount() <= 1)
		{
			if (PARASETCTL->getProduct().contains(val.getParentCode()))
				locateProductData(PARASETCTL->getProduct()[val.getParentCode()]);
		}
		else if (m_tree.contains(val.getParentCode()) && m_tree.contains(val.getCode()))
		{
			int curRow = m_tree[val.getCode()].front()->row();
			int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
			QStandardItem* p = m_tree[val.getParentCode()].front()->child(nearRow);
			if (p)
			{
				QString nearCode = p->text();
				if (PARASETCTL->getProduct().contains(nearCode))
					locateProductData(PARASETCTL->getProduct()[nearCode]);
			}
		}
		delChild(val.getCode());
	}
}
void ProductManage::locateProductData(const CProduct & val)
{
	((QTreeView *)(ui.cbParentCode->view()))->expandAll();

	auto locate = [this](const QString &dkey) {
		QModelIndexList findIndex = this->m_pGoodsModel->match(this->m_pGoodsModel->index(0, 0), Qt::DisplayRole, dkey, 1, Qt::MatchRecursive | Qt::MatchExactly);
		if (findIndex.size() > 0)
		{
			this->ui.treeView->setCurrentIndex(findIndex[eProductCode]);
			this->ui.treeView->clicked(findIndex[eProductCode]);
		}
		else
		{
			this->clear();
		}
	};
	QString curCode = val.getCode();
	if (m_tree.contains(curCode))
	{
		locate(curCode);
	}
	else
	{
		locate(val.getParentCode());
	}
}
void ProductManage::clear()
{
	ui.leCode->setText("");
	ui.leName->setText("");
	ui.deStart->setDate(QDate::currentDate());
	ui.deEnd->setDate(ui.deEnd->maximumDate());
	ui.cbParentCode->setCurrentText("");
	ui.lineEditParentName->setText("");
	ui.pteAnnotation->setPlainText("");

	setCurrentData(getViewData());
}