#include "paramgrunion.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "util/util.h"
#include "view/commonui/message_box_widget.h"


ParaMgrUnion::ParaMgrUnion(QWidget *parent)
	: QWidget(parent)
	, m_pGoodsModel(NULL)
{
	ui.setupUi(this);
	ui.pushButton_add->setText(tr("add"));
	ui.pushButton_add->setIcon(QIcon(qutil::skin("addclickover.png")));
	ui.pushButton_modify->setText(tr("modify"));
	ui.pushButton_modify->setIcon(QIcon(qutil::skin("reviseclickover.png")));
	ui.pushButton_delete->setText(tr("delete"));
	ui.pushButton_delete->setIcon(QIcon(qutil::skin("deleteclickover.png")));
	connect(ui.pushButton_add, &QPushButton::clicked, this, &ParaMgrUnion::addPortfolio);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &ParaMgrUnion::modifyPortfolio);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &ParaMgrUnion::delPortfolio);
	init();
}

ParaMgrUnion::~ParaMgrUnion()
{
}

void ParaMgrUnion::init()
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
		});
	}

	{
		ui.comboBox_parentcode->view()->setAlternatingRowColors(true);
	}
}

void ParaMgrUnion::addPortfolio()
{
	QString _portcode = ui.lineEdit_portcode->text();
	QString _parentcode = ui.comboBox_parentcode->currentText();
	QString _portname = ui.lineEdit_portname->text();
	QString _parentname = PARASETCTL->isExistCode(_parentcode)? PARASETCTL->getPortfolio()[_parentcode].getPortname():"";
	int _sdate = ui.dateEdit_datebegin->date().toJulianDay();
	int _edate = ui.dateEdit_dateend->date().toJulianDay();
	QString _annotation = ui.textEdit->toPlainText();
	CPortfolio cp(_portcode, _portname, _parentcode, _parentname, _sdate, _edate, _annotation);
	if (PARASETCTL->isExistCode(_portcode))
		ShowWarnMessage(tr("add"), tr("the code is existing."), this);
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
void ParaMgrUnion::modifyPortfolio()
{
	QString _portcode = ui.lineEdit_portcode->text();
	QString _parentcode = ui.comboBox_parentcode->currentText();
	if (PARASETCTL->isParentCode(_portcode, _parentcode))
	{
		ShowWarnMessage(tr("modify"), tr("the code parent is invalid.do not choose its child or self."), this);
		return;
	}
	QString _portname = ui.lineEdit_portname->text();
	QString _parentname = PARASETCTL->isExistCode(_parentcode) ? PARASETCTL->getPortfolio()[_parentcode].getPortname() : "";
	int _sdate = ui.dateEdit_datebegin->date().toJulianDay();
	int _edate = ui.dateEdit_dateend->date().toJulianDay();
	QString _annotation = ui.textEdit->toPlainText();
	CPortfolio cp(_portcode, _portname, _parentcode, _parentname, _sdate, _edate, _annotation);
	if (!PARASETCTL->isExistCode(_portcode))
		ShowWarnMessage(tr("modify"), tr("the code is not existing."), this);
	else
	{
		if (PARASETCTL->setPortfolio(cp))
		{
			ShowSuccessMessage(tr("modify"), tr("modify success."), this);
			// 同步
			initDateView();
		}
		else
			ShowWarnMessage(tr("modify"), tr("modify fail."), this);
	}
}
void ParaMgrUnion::delPortfolio()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this))
	{
		QString _portcode = ui.lineEdit_portcode->text();
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
			ShowWarnMessage(tr("delete"), tr("no code."), this);
			return;
		}
		ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		// 同步
		initDateView();
	}
}

void ParaMgrUnion::initDateView()
{
	if (m_pGoodsModel) m_pGoodsModel->clear();
	QStringList treeHeader;
	treeHeader << tr("portcode") << tr("portname") << tr("parentcode")
		<< tr("parentname") << tr("sdate") << tr("edate") << tr("annotation");
	if (!m_pGoodsModel) m_pGoodsModel = new QStandardItemModel(0, treeHeader.size(), this);
	m_pGoodsModel->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_pGoodsModel->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_pGoodsModel);
	
	QStringList roots;
	PARASETCTL->getAllRootCodes(roots);
	for (QStringList::const_iterator itorRoot = roots.begin();
		itorRoot != roots.end(); itorRoot++)
	{
		QList<QStandardItem *> items;
		packItem(items, PARASETCTL->getPortfolio()[*itorRoot]);
		m_pGoodsModel->appendRow(items);
		
		packChild(items.front(), *itorRoot);
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
}

void ParaMgrUnion::packItem(QList<QStandardItem *> &childItems, const CPortfolio &val)
{
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

void ParaMgrUnion::packChild(QStandardItem *parent, const QString &curID)
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