#include "portfolio_manage.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include <QCalendarWidget>
#include "util/util.h"
#include "view/commonui/message_box_widget.h"

PortfolioManage::PortfolioManage(QWidget *parent)
	: BodyWidget(parent)
	, m_pGoodsModel(NULL)
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

bool PortfolioManage::isEqual(const CPortfolio &newVal)
{
	return newVal == getCurrentData();
}

void PortfolioManage::init()
{
	{
		ui.treeView->setAlternatingRowColors(true);
		
		initDateView();
		connect(ui.treeView, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
			
			QVariant s = index.sibling(index.row(), 0).data();
			if (s.isValid()) ui.lineEdit_portcode->setText(s.toString());
			s = index.sibling(index.row(), 1).data();
			if (s.isValid()) ui.lineEdit_portname->setText(s.toString());
			s = index.sibling(index.row(), 4).data();
			if (s.isValid()) ui.dateEdit_datebegin->setDate(QDate::fromString(s.toString(), "yyyy-MM-dd"));
			s = index.sibling(index.row(), 5).data();
			if (s.isValid()) ui.dateEdit_dateend->setDate(QDate::fromString(s.toString(), "yyyy-MM-dd"));
			s = index.sibling(index.row(), 2).data();
			if (s.isValid()) ui.comboBox_parentcode->setCurrentText(s.toString());
			s = index.sibling(index.row(), 6).data();
			if (s.isValid()) ui.textEdit->setText(s.toString());
			setCurrentData(getViewData());
		});
	}

	{
		ui.comboBox_parentcode->view()->setAlternatingRowColors(true);
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
	qutil::setWeekendStyle(ui.dateEdit_datebegin);
	qutil::setWeekendStyle(ui.dateEdit_dateend);
}

void PortfolioManage::addPortfolio()
{
	CPortfolio cp = getViewData();
	if (checkNull({ ui.lineEdit_portcode,ui.lineEdit_portname})) { 
		ShowWarnMessage(tr("add"), tr("Red input box can not be empty!"), this);
		return; 
	}
	/*if (cp.getPortcode().isEmpty()) {
		ShowWarnMessage(tr("add"), tr("the code can not empty!"), this);
		return;
	}*/
	if (PARASETCTL->isExistCode(cp.getPortcode()))
		ShowWarnMessage(tr("add"), tr("the portfolio already exists.").arg(cp.getPortcode()), this);
	else
	{
		if (PARASETCTL->setPortfolio(cp))
		{
			ShowSuccessMessage(tr("add"), tr("add success."), this);
			// 同步
			initDateView();
		}
		else
			ShowWarnMessage(tr("add"), tr("add fail."), this);
	}
}
void PortfolioManage::modifyPortfolio()
{
	if (checkNull({ui.lineEdit_portcode})) {
		ShowWarnMessage(tr("modify"), tr("Please confirm the portfolio you want to modify first"), this);
		return;
	}

	CPortfolio cp = getViewData();
	if (PARASETCTL->isParentCode(cp.getPortcode(), cp.getParentcode()))
	{
		ShowWarnMessage(tr("modify"), tr("the code parent is invalid.do not choose its child or self."), this);
		return;
	}
	if (this->isKeyModify(cp)) {
		ShowWarnMessage(tr("modify"), tr("port code can not be modified!"), this);
		return;
	}

	if (this->isEqual(cp)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}

	if (PARASETCTL->setPortfolio(cp))
	{
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		// 同步
		initDateView();
	}
	else
		ShowWarnMessage(tr("modify"), tr("modify fail."), this);
}
void PortfolioManage::delPortfolio()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this))
	{
		if (checkNull({ ui.lineEdit_portcode })) {
			ShowWarnMessage(tr("delete"), tr("the code can not empty!"), this);
			return;
		}
		QString _portcode = ui.lineEdit_portcode->text().trimmed();
		/*if (_portcode.isEmpty()) {
			ShowWarnMessage(tr("delete"), tr("the code can not empty!"), this);
			return;
		}*/
		if (PARASETCTL->isExistCode(_portcode))
		{
			if (!PARASETCTL->removePortfolio(_portcode))
			{
				ShowWarnMessage(tr("delete"), tr("delete fail."), this);
				return;
			}
		}
		else
		{
			QString errMsg = tr("the portfolio doesn't exist!").arg(_portcode);
			ShowWarnMessage(tr("delete"), errMsg, this);
			return;
		}
		ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		setViewData(CPortfolio());
		// 同步
		initDateView();
	}
}

void PortfolioManage::initDateView()
{
	CPortfolio oldVal = getViewData();
	if (m_pGoodsModel) m_pGoodsModel->clear();
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
	ui.treeView->setColumnWidth(1, 200);
	QStringList roots;
	PARASETCTL->getAllRootCodes(roots);
	for (QStringList::const_iterator itorRoot = roots.begin();
		itorRoot != roots.end(); itorRoot++)
	{
		QList<QStandardItem *> items;
		packItem(items, PARASETCTL->getPortfolio()[*itorRoot]);
		if (items.size() > 0) {
			m_pGoodsModel->appendRow(items);
			packChild(items.front(), *itorRoot);
		}
	}
	ui.treeView->expandAll();

	{
		QString last = ui.comboBox_parentcode->currentText();
		QStringList s;
		s << "" << PARASETCTL->getPortfolio().keys();
		ui.comboBox_parentcode->clear();
		ui.comboBox_parentcode->addItems(s);
		if (s.contains(last)) ui.comboBox_parentcode->setCurrentText(last);
	}

	setViewData(oldVal);
}

void PortfolioManage::packItem(QList<QStandardItem *> &childItems, const CPortfolio &val)
{
	if (val.getPortcode().isEmpty() || val.getPortname().isEmpty()) {
		return;
	}
	QStandardItem *iportcode = new QStandardItem(val.getPortcode());
	QStandardItem *iportname = new QStandardItem(val.getPortname());
	QStandardItem *iparentcode = new QStandardItem(val.getParentcode());
	QStandardItem *iparentname = new QStandardItem(val.getParentname());
	QStandardItem *isdate = new QStandardItem(QDate::fromJulianDay(val.getSdate()).toString("yyyy-MM-dd"));
	QStandardItem *iedate = new QStandardItem(QDate::fromJulianDay(val.getEdate()).toString("yyyy-MM-dd"));
	QStandardItem *iannotation = new QStandardItem(val.getAnnotation());
	childItems.push_back(iportcode);
	childItems.push_back(iportname);
	childItems.push_back(iparentcode);
	childItems.push_back(iparentname);
	childItems.push_back(isdate);
	childItems.push_back(iedate);
	childItems.push_back(iannotation);
	iportcode->setToolTip(val.getPortcode());
	iportcode->setToolTip(val.getPortname());
	iportcode->setToolTip(val.getParentcode());
	iportcode->setToolTip(val.getParentname());
	isdate->setToolTip(QDate::fromJulianDay(val.getSdate()).toString("yyyy-MM-dd"));
	iedate->setToolTip(QDate::fromJulianDay(val.getSdate()).toString("yyyy-MM-dd"));
	iannotation->setToolTip(qutil::splitTooltip(val.getAnnotation(), 200));
}

void PortfolioManage::packChild(QStandardItem *parent, const QString &curID)
{
	QStringList children;
	PARASETCTL->getChildren(curID, children);
	for (QStringList::const_iterator itorRoot = children.begin();
		itorRoot != children.end(); itorRoot++)
	{
		QList<QStandardItem *> items;
		packItem(items, PARASETCTL->getPortfolio()[*itorRoot]);
		parent->appendRow(items);

		packChild(items.front(), *itorRoot);
	}
}

CPortfolio PortfolioManage::getViewData()
{
	QString _portcode = ui.lineEdit_portcode->text().trimmed();
	QString _parentcode = ui.comboBox_parentcode->currentText().trimmed();
	QString _portname = ui.lineEdit_portname->text().trimmed();

	QString _parentname = PARASETCTL->isExistCode(_parentcode) ? PARASETCTL->getPortfolio()[_parentcode].getPortname() : "";
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