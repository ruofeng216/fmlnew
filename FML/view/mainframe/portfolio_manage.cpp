#include "portfolio_manage.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include <QCalendarWidget>
#include <QFileSystemModel>
#include "util/util.h"
#include "view/commonui/message_box_widget.h"

PortfolioManage::PortfolioManage(QWidget *parent)
	: BodyWidget(parent)
	, m_pGoodsModel(NULL)
	, m_pGoodsModelCombobox(NULL)
{
	ui.setupUi(this);

	connect(ui.pushButton_add, &QPushButton::clicked, this, &PortfolioManage::addPortfolio);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &PortfolioManage::modifyPortfolio);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &PortfolioManage::delPortfolio);
	init();
	slotSkinChange();
}

PortfolioManage::~PortfolioManage()
{
}

QString PortfolioManage::getKey(const CPortfolio &newVal) const
{
	return newVal.getPortcode();
}

// 提交时，检查相关控件值是否合法。
bool PortfolioManage::checkValid()
{
	bool bValid = true;
	if (ui.lineEdit_portcode->text().isEmpty())
	{
		ui.lineEdit_portcode->setError(tr("code cant be empty!"));
		bValid = false;
	}
	if (ui.lineEdit_portname->text().isEmpty())
	{
		ui.lineEdit_portname->setError(tr("code-name cant be empty!"));
		bValid = false;
	}
	if (ui.dateEdit_dateend->date().toJulianDay() < ui.dateEdit_datebegin->date().toJulianDay())
	{
		ui.dateEdit_dateend->setError(tr("end-time cant be small than start-time!"));
		bValid = false;
	}
	if (ui.lineEdit_portcode->text() == ui.comboBox_parentcode->currentText()) 
	{
		ui.lineEdit_portcode->setError(tr("portfolio code equals parent code error!"));
		ui.comboBox_parentcode->setError(tr("portfolio code equals parent code error!"));
		bValid = false;
	}
	return bValid;
}

void PortfolioManage::init()
{
	{
		//QTreeView *pView = new QTreeView(ui.comboBox_parentcode);
		//QTreeView *pView = (QTreeView *)ui.comboBox_parentcode->view();
		if (!m_pGoodsModelCombobox) m_pGoodsModelCombobox = new QStandardItemModel(0, 1, this);
		m_pGoodsModelCombobox->setColumnCount(1);
		ui.comboBox_parentcode->setModel(m_pGoodsModelCombobox);
		//pView->setHeaderHidden(true);
		//pView->setAlternatingRowColors(true);
		//ui.comboBox_parentcode->setView(pView);
		//ui.comboBox_parentcode->view()->setAlternatingRowColors(true);
	}
	{
		ui.treeView->setAlternatingRowColors(true);
		connect(ui.treeView, &QTreeView::clicked, [this](const QModelIndex &index) {
			QVariant s = index.sibling(index.row(), ePortcode).data();
			if (s.isValid()) this->ui.lineEdit_portcode->setText(s.toString());
			s = index.sibling(index.row(), ePortname).data();
			if (s.isValid()) this->ui.lineEdit_portname->setText(s.toString());
			s = index.sibling(index.row(), eSdate).data();
			if (s.isValid()) this->ui.dateEdit_datebegin->setDate(QDate::fromString(s.toString(), YMD));
			s = index.sibling(index.row(), eEdate).data();
			if (s.isValid()) this->ui.dateEdit_dateend->setDate(QDate::fromString(s.toString(), YMD));
			s = index.sibling(index.row(), eParentcode).data();
			if (s.isValid()) this->ui.comboBox_parentcode->setCurrentText(s.toString());
			s = index.sibling(index.row(), eAnnotation).data();
			if (s.isValid()) this->ui.textEdit->setText(s.toString());
			this->setCurrentData(this->getViewData());
		});
		
		initDateView();
		
	}
}

void PortfolioManage::slotSkinChange()
{
	ui.pushButton_add->setText(tr("add"));
	ui.pushButton_add->setIcon(QIcon(qutil::skin("add-click-hover.png")));
	ui.pushButton_modify->setText(tr("modify"));
	ui.pushButton_modify->setIcon(QIcon(qutil::skin("revise-click-hover.png")));
	ui.pushButton_delete->setText(tr("delete"));
	ui.pushButton_delete->setIcon(QIcon(qutil::skin("delete-click-hover.png")));
	qutil::setWeekendStyle((QDateEdit *)ui.dateEdit_datebegin);
	qutil::setWeekendStyle((QDateEdit *)ui.dateEdit_dateend);
}

void PortfolioManage::addPortfolio(bool)
{
	if (!checkValid()) return;
	CPortfolio cp = getViewData();
	if (PARASETCTL->isExistPortfolioCode(cp.getPortcode()))
		ShowWarnMessage(tr("add"), tr("the portfolio already exists.").arg(cp.getPortcode()), this);
	else
	{
		if (!cp.getParentcode().isEmpty() && !PARASETCTL->isExistPortfolioCode(cp.getParentcode()))
		{
			ShowWarnMessage(tr("add"), tr("the parent dont exists.").arg(cp.getParentcode()), this);
			return;
		}
		QString err;
		if (PARASETCTL->setPortfolio(cp, err))
		{
			ShowSuccessMessage(tr("add"), tr("add success."), this);
			// 同步
			addPortfolioData(cp);
			locatePortfolioData(cp);
		}
		else
			ShowWarnMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}
void PortfolioManage::modifyPortfolio(bool)
{
	if (!checkValid()) return;
	CPortfolio cp = getViewData();

	if (this->isKeyModify(cp)) {
		ShowWarnMessage(tr("modify"), tr("port code can not be modified!"), this);
		return;
	}
	if (this->isEqual(cp)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}

	QStringList childlst;
	PARASETCTL->getPortfolioChildren(cp.getPortcode(), childlst);
	if (!childlst.isEmpty())
	{
		if (getCurrentData().getParentcode() != cp.getParentcode())
			ShowWarnMessage(tr("delete"), tr("Cant modify parent-code, when it has children!"), this);
			return;
	}

	if (!cp.getParentcode().isEmpty() && !PARASETCTL->isExistPortfolioCode(cp.getParentcode()))
	{
		ShowWarnMessage(tr("add"), tr("the parent dont exists.").arg(cp.getParentcode()), this);
		return;
	}

	QString err;
	if (PARASETCTL->setPortfolio(cp, err))
	{
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		// 同步
		
		addPortfolioData(cp);
		locatePortfolioData(cp);
	}
	else
		ShowWarnMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
}
void PortfolioManage::delPortfolio(bool)
{
	if (!checkValid()) return;
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this))
	{
		CPortfolio cp = getViewData();
		QStringList childlst;
		PARASETCTL->getPortfolioChildren(cp.getPortcode(), childlst);
		if (!childlst.isEmpty())
		{
			ShowWarnMessage(tr("delete"), tr("Cant delete parent-code, when it has children!"), this);
			return;
		}
		if (PARASETCTL->isExistPortfolioCode(cp.getPortcode()))
		{
			QString err;
			if (!PARASETCTL->removePortfolio(cp.getPortcode(), err))
			{
				ShowWarnMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
				return;
			}
		}
		else
		{
			QString errMsg = tr("the portfolio doesn't exist!").arg(cp.getPortcode());
			ShowWarnMessage(tr("delete"), errMsg, this);
			return;
		}
		ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		// 同步
		delPortfolioData(cp);
	}
}

void PortfolioManage::initDateView()
{
	if (m_pGoodsModel) m_pGoodsModel->clear();
	if (m_pGoodsModelCombobox) m_pGoodsModelCombobox->clear();
	m_pGoodsModelCombobox->appendRow(new QStandardItem(""));

	QStringList treeHeader;
	treeHeader << tr("portcode") << tr("portname") << tr("parentcode")
		<< tr("parentname") << tr("sdate") << tr("edate") << tr("annotation");
	if (!m_pGoodsModel) m_pGoodsModel = new QStandardItemModel(0, treeHeader.size(), this);
	m_pGoodsModel->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_pGoodsModel->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_pGoodsModel);
	ui.treeView->setColumnHidden(2, true);
	ui.treeView->setColumnHidden(3, true);
	ui.treeView->setColumnWidth(0, 200);

	QMap<QString, CPortfolio> val = PARASETCTL->getPortfolio();
	for (QMap<QString, CPortfolio>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		addPortfolioData(itor.value());
	}
	if (!val.isEmpty()) 
	{
		locatePortfolioData(val[val.keys().back()]);
	}
	else
	{
		clear();
	}
}

void PortfolioManage::packItem(QList<QStandardItem *> &childItems, const CPortfolio &val)
{
	if (val.getPortcode().isEmpty() || val.getPortname().isEmpty()) {
		return;
	}

	for (int i = 0; i < eEnd; i++)
	{
		switch (i)
		{
		case ePortcode:
			{
				QStandardItem *iportcode = new QStandardItem(val.getPortcode());
				childItems.push_back(iportcode);
				iportcode->setToolTip(val.getPortcode());
				break;
			}
		case ePortname:
			{
				QStandardItem *iportname = new QStandardItem(val.getPortname());
				childItems.push_back(iportname);
				iportname->setToolTip(val.getPortname());
				break;
			}
		case eParentcode:
			{
				QStandardItem *iparentcode = new QStandardItem(val.getParentcode());
				childItems.push_back(iparentcode);
				iparentcode->setToolTip(val.getParentcode());
				break;
			}
		case eParentname:
			{
				QStandardItem *iparentname = new QStandardItem(val.getParentname());
				childItems.push_back(iparentname);
				iparentname->setToolTip(val.getParentname());
				break;
			}
		case eSdate:
			{
				QStandardItem *isdate = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				childItems.push_back(isdate);
				isdate->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				break;
			}
		case eEdate:
			{
				QStandardItem *iedate = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString(YMD));
				childItems.push_back(iedate);
				iedate->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				break;
			}
		case eAnnotation:
			{
				QStandardItem *iannotation = new QStandardItem(val.getAnnotation());
				childItems.push_back(iannotation);
				iannotation->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
				break;
			}
		default:
			break;
		}
	}
	
}

CPortfolio PortfolioManage::getViewData()
{
	QString _portcode = ui.lineEdit_portcode->text().trimmed();
	QString _parentcode = ui.comboBox_parentcode->currentText().trimmed();
	QString _portname = ui.lineEdit_portname->text().trimmed();

	QString _parentname = PARASETCTL->isExistPortfolioCode(_parentcode) ? PARASETCTL->getPortfolio()[_parentcode].getPortname() : "";
	int _sdate = ui.dateEdit_datebegin->date().toJulianDay();
	int _edate = ui.dateEdit_dateend->date().toJulianDay();
	QString _annotation = ui.textEdit->toPlainText();
	return CPortfolio(_portcode, _portname, _parentcode, _parentname, _sdate, _edate, _annotation);
}

void PortfolioManage::setViewData(const CPortfolio &val)
{
	ui.lineEdit_portcode->setText(val.getPortcode());
	ui.lineEdit_portname->setText(val.getPortname());
	ui.dateEdit_datebegin->setDate(val.getSdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getSdate()));
	ui.dateEdit_dateend->setDate(val.getEdate() == 0 ? QDate::currentDate() : QDate::fromJulianDay(val.getEdate()));
	ui.comboBox_parentcode->setCurrentText(val.getParentcode());
	ui.textEdit->setText(val.getAnnotation());
	setCurrentData(getViewData());
}

void PortfolioManage::addPortfolioData(const CPortfolio & val)
{
	auto insertRoot = [this](const CPortfolio &val) {
		QList<QStandardItem *> items;
		QList<QStandardItem *> itemsCombobox;
		this->packItem(items, val);
		
		if (items.size() > 0) {
			this->m_pGoodsModel->appendRow(items);
			this->m_tree[val.getPortcode()] = items;
		}
		this->packItem(itemsCombobox, val);
		if (itemsCombobox.size() > 0) {
			this->m_pGoodsModelCombobox->appendRow(itemsCombobox.front());
			this->m_treeCombobox[val.getPortcode()] = itemsCombobox;
		}
	};
	auto insertChild = [this](const CPortfolio &val) {
		if (!this->m_tree.contains(val.getPortcode()) && this->m_tree.contains(val.getParentcode()))
		{
			QList<QStandardItem *> items;
			this->packItem(items, val);
			
			if (items.size() > 0) {
				this->m_tree[val.getParentcode()].front()->appendRow(items);
				this->m_tree[val.getPortcode()] = items;
			}
			QList<QStandardItem *> itemsCombobox;
			this->packItem(itemsCombobox, val);
			if (itemsCombobox.size() > 0) {
				this->m_treeCombobox[val.getParentcode()].front()->appendRow(itemsCombobox.front());
				this->m_treeCombobox[val.getPortcode()] = itemsCombobox;
			}
		}
	};
	auto updateChild = [this](const CPortfolio &val) {
		if (this->m_tree.contains(val.getPortcode()))
		{
			if (val.getParentcode() != this->m_tree[val.getPortcode()][eParentcode]->text())
			{
				CPortfolio del(this->m_tree[val.getPortcode()][ePortcode]->text(),
					this->m_tree[val.getPortcode()][ePortname]->text(),
					this->m_tree[val.getPortcode()][eParentcode]->text(),
					this->m_tree[val.getPortcode()][eParentname]->text());
				this->delPortfolioData(del);
				this->addPortfolioData(val);
			}
			else if (this->m_tree[val.getPortcode()].size() == eEnd) {
				this->m_tree[val.getPortcode()][ePortname]->setText(val.getPortname());
				this->m_tree[val.getPortcode()][eParentcode]->setText(val.getParentcode());
				this->m_tree[val.getPortcode()][eParentname]->setText(val.getParentname());
				this->m_tree[val.getPortcode()][eSdate]->setText(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				this->m_tree[val.getPortcode()][eEdate]->setText(QDate::fromJulianDay(val.getEdate()).toString(YMD));
				this->m_tree[val.getPortcode()][eAnnotation]->setText(val.getAnnotation());
				
				this->m_tree[val.getPortcode()][ePortname]->setToolTip(val.getPortname());
				this->m_tree[val.getPortcode()][eParentcode]->setToolTip(val.getParentcode());
				this->m_tree[val.getPortcode()][eParentname]->setToolTip(val.getParentname());
				this->m_tree[val.getPortcode()][eSdate]->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				this->m_tree[val.getPortcode()][eEdate]->setToolTip(QDate::fromJulianDay(val.getSdate()).toString(YMD));
				this->m_tree[val.getPortcode()][eAnnotation]->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
			}
		}
	};
	QString rootCode = val.getParentcode();
	QString curCode = val.getPortcode();
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
			if (PARASETCTL->getPortfolio().contains(rootCode))
			{// 父节点有效
				CPortfolio parentCode = PARASETCTL->getPortfolio()[rootCode];
				addPortfolioData(parentCode);
				insertChild(val);
			}
			else
			{
				qWarning() << "Do not exist parent-code: " << rootCode;
			}
		}
	}
}
void PortfolioManage::delPortfolioData(const CPortfolio & val)
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
	if (val.getParentcode().isEmpty())
	{// 根节点
		if (m_pGoodsModel->rowCount() <= 1)
		{
			clear();
		}
		else
		{
			if (m_tree.contains(val.getPortcode()))
			{
				int curRow = m_tree[val.getPortcode()].front()->row();
				int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
				QStandardItem* p = m_pGoodsModel->item(nearRow);
				if (p)
				{
					QString nearCode = p->text();
					if (PARASETCTL->getPortfolio().contains(nearCode))
						locatePortfolioData(PARASETCTL->getPortfolio()[nearCode]);
				}
			}
		}
		delRoot(val.getPortcode());
	}
	else
	{
		if (m_tree.contains(val.getParentcode()) && m_tree[val.getParentcode()].front()->rowCount() <= 1)
		{
			if (PARASETCTL->getPortfolio().contains(val.getParentcode()))
				locatePortfolioData(PARASETCTL->getPortfolio()[val.getParentcode()]);
		}
		else if (m_tree.contains(val.getParentcode()) && m_tree.contains(val.getPortcode()))
		{
			int curRow = m_tree[val.getPortcode()].front()->row();
			int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
			QStandardItem* p = m_tree[val.getParentcode()].front()->child(nearRow);
			if (p)
			{
				QString nearCode = p->text();
				if (PARASETCTL->getPortfolio().contains(nearCode))
					locatePortfolioData(PARASETCTL->getPortfolio()[nearCode]);
			}
		}
		delChild(val.getPortcode());
	}
}
void PortfolioManage::locatePortfolioData(const CPortfolio & val)
{
	((QTreeView *)(ui.comboBox_parentcode->view()))->expandAll();

	auto locate = [this](const QString &dkey) {
		QModelIndexList findIndex = this->m_pGoodsModel->match(this->m_pGoodsModel->index(0, 0), Qt::DisplayRole, dkey, 1, Qt::MatchRecursive| Qt::MatchExactly);
		if (findIndex.size() > 0)
		{
			this->ui.treeView->setCurrentIndex(findIndex[ePortcode]);
			this->ui.treeView->clicked(findIndex[ePortcode]);
		}
		else
		{
			this->clear();
		}
	};
	QString curCode = val.getPortcode();
	if (m_tree.contains(curCode))
	{
		locate(curCode);
	}
	else
	{
		locate(val.getParentcode());
	}
}
void PortfolioManage::clear()
{
	ui.lineEdit_portcode->setText("");
	ui.lineEdit_portname->setText("");
	ui.dateEdit_datebegin->setDate(QDate::currentDate());
	ui.dateEdit_dateend->setDate(ui.dateEdit_dateend->maximumDate());
	ui.comboBox_parentcode->setCurrentText("");
	ui.textEdit->setText("");

	setCurrentData(getViewData());
}