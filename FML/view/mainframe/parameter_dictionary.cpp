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
	connect(ui.cbTypeCode, SIGNAL(activated(int)), this, SLOT(slotTypeCodeChanged(int)));
	connect(ui.cbTypeName, SIGNAL(activated(int)), this, SLOT(slotTypeNameChanged(int)));

	init();
	slotSkinChange();
}

ParameterDictionary::~ParameterDictionary()
{
}

QString ParameterDictionary::getKey(const CParaDict &newVal) const
{
	return newVal.getParaCode();
}

// 提交时，检查相关控件值是否合法。
bool ParameterDictionary::checkValid()
{
	return true;
}

void ParameterDictionary::init()
{
	CParaDict oldVal = getViewData();
	QList<QPair<QString, QString>> typeList;
	{
		// 初始化treeView
		QStringList treeHeader;
		treeHeader << tr("paracode") << tr("paraname") << tr("paraexplain");
		if (m_model) {
			m_model->clear();
		} else {
			m_model = new QStandardItemModel(0, treeHeader.size(), this);
		}

		m_model->setColumnCount(treeHeader.size());
		for (int i = 0; i < treeHeader.size(); i++) {
			m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
		}
		const QMap<QString, QList<CParaDict>>& data = PARASETCTL->getParadict();
		QMap<QString, QStandardItem*> roots;
		/*foreach(const CParaDict &val, data) {
			if (val.isTypeData()) {
				QList<QStandardItem*> items = createRowItems(val, true);
				if (!items.isEmpty() && !roots.contains(val.getTypeCode())) {
					roots[val.getTypeCode()] = items[0];
					m_model->appendRow(items);
					typeList << qMakePair(val.getTypeCode(), val.getTypeName());
				}
			}
		}
		foreach(const CParaDict &val, data) {
			if (val.getTypeCode().isEmpty() || val.getParaCode().isEmpty()) {
				continue;
			}
			if (roots.contains(val.getTypeCode())) {
				QList<QStandardItem*> items = createRowItems(val, false);
				if (!items.isEmpty()) {
					roots[val.getTypeCode()]->appendRow(items);
				}
			}
		}*/

		ui.treeView->setModel(m_model);
		ui.treeView->setColumnWidth(0, 160);
		ui.treeView->setColumnWidth(1, 160);
		ui.treeView->expandAll();
	}

	{
		// 初始化类别列表
		ui.cbTypeCode->clear();
		ui.cbTypeName->clear();
		for (int i = 0; i < typeList.size(); i++) {
			ui.cbTypeCode->addItem(typeList[i].first);
			ui.cbTypeName->addItem(typeList[i].second);
		}
	}
	setViewData(oldVal);
	// 选中当前行
	QModelIndexList findIndex = m_model->match(m_model->index(0, 0), Qt::DisplayRole, oldVal.getParaCode(), 1, Qt::MatchRecursive);
	if (findIndex.size() > 0) {
		ui.treeView->setCurrentIndex(findIndex[0]);
	}
}

CParaDict ParameterDictionary::getViewData()
{
	CParaDict result;
	result.setTypeCode(ui.cbTypeCode->currentText().trimmed());
	result.setTypeName(ui.cbTypeName->currentText().trimmed());
	result.setParaCode(ui.leParaCode->text().trimmed());
	result.setParaName(ui.leParaName->text().trimmed());
	result.setParaExplain(ui.pteParaExplain->toPlainText().trimmed());
	return result;
}

void ParameterDictionary::setViewData(const CParaDict &val)
{
	ui.leParaCode->setText(val.getParaCode());
	ui.leParaName->setText(val.getParaName());
	ui.cbTypeCode->setCurrentText(val.getTypeCode());
	ui.cbTypeName->setCurrentText(val.getTypeName());
	ui.pteParaExplain->setPlainText(val.getParaExplain());
	setCurrentData(getViewData());
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
		ShowWarnMessage(tr("add"), tr("The para code already exists").arg(val.getTypeCode()).arg(val.getParaCode()), this);
		return;
	}
	QString err;
	if (PARASETCTL->setParadict(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		init();
	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void ParameterDictionary::slotModify()
{
	if (getCurrentData().getParaCode().isEmpty()) {
		ShowWarnMessage(tr("modify"), tr("No selected content can not be modified"), this);
		return;
	}

	CParaDict val = getViewData();
	if (this->isKeyModify(val)) {
		ShowWarnMessage(tr("modify"), tr("param code can not be modified!"), this);
		return;
	}
	if (this->isEqual(val)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}
	QString err;
	if (PARASETCTL->setParadict(val, err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
		init();
	}
	else {
		ShowErrorMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
}

void ParameterDictionary::slotDelete()
{
	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CParaDict val = getViewData();
		CParaDict oldVal;
		bool isFind = PARASETCTL->getParadict(val.getTypeCode(), val.getParaCode(), oldVal);
		if (!isFind) {
			ShowWarnMessage(tr("delete"), tr("The paradict is not existing!"), this);
			return;
		}
		QString err;
		if (PARASETCTL->removeParadict(val.getTypeCode(), val.getParaCode(), err)) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			setViewData(CParaDict());
			init();
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void ParameterDictionary::slotTreeDoubleClicked(const QModelIndex &index)
{
	QString typeCode, typeName;
	QString code = index.sibling(index.row(), 0).data().toString();
	QString name = index.sibling(index.row(), 1).data().toString();
	QString explain = index.sibling(index.row(), 2).data().toString();
	QModelIndex parentIndex = index.parent();
	if (parentIndex.isValid()) { // 子节点
		typeCode = parentIndex.sibling(parentIndex.row(), 0).data().toString();
		typeName = parentIndex.sibling(parentIndex.row(), 1).data().toString();
	} else { // 父节点
		typeCode = code;
		typeName = name;
		code = "";
		name = "";
	}
	CParaDict val(typeCode, typeName, code, name, explain);
	setViewData(val);
}

void ParameterDictionary::slotTypeCodeChanged(int index)
{
	if (ui.cbTypeName->currentIndex() != index) {
		ui.cbTypeName->setCurrentIndex(index);
	}
}

void ParameterDictionary::slotTypeNameChanged(int index)
{
	if (ui.cbTypeCode->currentIndex() != index) {
		ui.cbTypeCode->setCurrentIndex(index);
	}
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
