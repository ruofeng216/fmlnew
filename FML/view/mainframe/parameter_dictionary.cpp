#include "parameter_dictionary.h"
#include "message_box_widget.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>

ParameterDictionary::ParameterDictionary(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
{
	ui.setupUi(this);
	ui.treeView->setAlternatingRowColors(true);
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
	connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotTreeDoubleClicked(QModelIndex)));

	init();
	slotSkinChange();
}

ParameterDictionary::~ParameterDictionary()
{
}

void ParameterDictionary::init()
{
	CParaDict oldVal = getViewData();
	{
		// ≥ı ºªØtreeView
		QStringList treeHeader;
		treeHeader << tr("paracode") << tr("paraname") << tr("paraexplain");
		if (m_model) {
			m_model->clear();
		} else {
			m_model = new QStandardItemModel(0, treeHeader.size(), this);
		}

		//m_model->setColumnCount(treeHeader.size());
		//for (int i = 0; i < treeHeader.size(); i++) {
		//	m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
		//}
		//const QList<CParaDict>& data = PARASETCTL->getParadict();
		//QList<CParaDict> roots, children;
		//for (auto iter = data.begin(); iter != data.end(); ++iter) {
		//	bool isTypeCodeFind = false;
		//	bool isParaCodeFind = false;
		//	for (auto rootIter = roots.begin(); rootIter != roots.end(); ++rootIter) {
		//		if (rootIter->getTypeCode() == iter->getTypeCode()) {
		//			isTypeCodeFind = true;
		//			break;
		//		}
		//	}
		//	if (!isTypeCodeFind) {
		//		QList<QStandardItem *> items = createRowItems(*iter, true);
		//		roots.push_back(*iter);
		//		m_model->appendRow(items);
		//	}
		//}

		ui.treeView->setModel(m_model);
		ui.treeView->setColumnWidth(0, 160);
		ui.treeView->expandAll();
	}

	setViewData(oldVal);
}

CParaDict ParameterDictionary::getViewData()
{
	CParaDict result;
	result.setTypeCode(ui.leTypeCode->text().trimmed());
	result.setTypeName(ui.leTypeName->text().trimmed());
	result.setParaCode(ui.cbParaCode->currentText().trimmed());
	result.setParaName(ui.cbParaName->currentText().trimmed());
	result.setParaExplain(ui.pteParaExplain->toPlainText().trimmed());
	return result;
}

void ParameterDictionary::setViewData(const CParaDict &val)
{
	ui.leTypeCode->setText(val.getTypeCode());
	ui.leTypeName->setText(val.getTypeName());
	ui.cbParaCode->setCurrentText(val.getParaCode());
	ui.cbParaName->setCurrentText(val.getParaName());
	ui.pteParaExplain->setPlainText(val.getParaExplain());
}

void ParameterDictionary::slotSkinChange()
{

}

void ParameterDictionary::slotAdd()
{
	CParaDict val = getViewData();
	if (val.getTypeCode().isEmpty() || val.getTypeName().isEmpty()) {
		ShowWarnMessage(tr("add"), tr("code or name is empty"), this);
		return;
	}

	CParaDict oldVal;
	if (PARASETCTL->getParadict(val.getTypeCode(), val.getParaCode(), oldVal)) {
		ShowWarnMessage(tr("add"), tr("The para code already exists"), this);
		return;
	}

	if (PARASETCTL->setParadict(val)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
	} else {
		ShowErrorMessage(tr("add"), tr("add fail."), this);
	}
}

void ParameterDictionary::slotModify()
{
	CParaDict val = getViewData();
	if (val.getTypeCode().isEmpty() || val.getTypeName().isEmpty()) {
		ShowWarnMessage(tr("modify"), tr("code or name is empty"), this);
		return;
	}

	//CProduct oldVal;
	//if (!PARASETCTL->getProduct(val.getCode(), oldVal)) {
	//	ShowWarnMessage(tr("modify"), tr("The product is not existing!"), this);
	//	return;
	//}

	//if (oldVal == val) {
	//	ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
	//	return;
	//}

	//if (PARASETCTL->setProduct(val)) {
	//	ShowSuccessMessage(tr("modify"), tr("modify success."), this);
	//	init();
	//}
	//else {
	//	ShowErrorMessage(tr("modify"), tr("modify fail."), this);
	//}
}

void ParameterDictionary::slotDelete()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CParaDict val = getViewData();
		//CProduct oldVal;
		//if (!PARASETCTL->getProduct(val.getCode(), oldVal)) {
		//	ShowWarnMessage(tr("delete"), tr("The product is not existing!"), this);
		//	return;
		//}
		//if (PARASETCTL->removeProduct(val.getCode())) {
		//	ShowSuccessMessage(tr("delete"), tr("delete success."), this);
		//	init();
		//}
		//else {
		//	ShowErrorMessage(tr("delete"), tr("delete fail."), this);
		//}
	}
}

void ParameterDictionary::slotTreeDoubleClicked(const QModelIndex &index)
{

}

QList<QStandardItem*> ParameterDictionary::createRowItems(const CParaDict &val, bool isRoot)
{
	QList<QStandardItem *> items;
	QStandardItem *code = new QStandardItem(isRoot ? val.getTypeCode() : val.getParaCode());
	QStandardItem *name = new QStandardItem(isRoot ? val.getTypeName() : val.getParaName());
	QStandardItem *explain = new QStandardItem(val.getParaExplain());
	code->setToolTip(code->text());
	name->setToolTip(name->text());
	explain->setToolTip(qutil::splitTooltip(explain->text(), 200));
	items.push_back(code);
	items.push_back(name);
	items.push_back(explain);
	return items;
}
