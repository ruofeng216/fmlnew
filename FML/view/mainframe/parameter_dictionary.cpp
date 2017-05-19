#include "parameter_dictionary.h"
#include "message_box_widget.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include <QTableView>

ParameterDictionary::ParameterDictionary(QWidget *parent)
	: BodyWidget(parent)
	, m_model(nullptr)
	, m_modelCombobox(nullptr)
{
	ui.setupUi(this);
	connect(ui.pbAdd, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(ui.pbModify, SIGNAL(clicked()), this, SLOT(slotModify()));
	connect(ui.pbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));

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
bool ParameterDictionary::isKeyModify(const CParaDict &newVal)
{
	CParaDict old = getCurrentData();
	if (old.getParaCode().isEmpty())
		return old.getTypeCode() != newVal.getTypeCode();
	else
		return old.getParaCode() != newVal.getParaCode();
}
// 提交时，检查相关控件值是否合法。
bool ParameterDictionary::checkValid()
{
	bool bValid = true;
	
	if (ui.leParaCode->text().isEmpty())
	{
		ui.leParaCode->setError(tr("ParaCode cant be empty!"));
		bValid = false;
	}

	if (ui.leParaName->text().isEmpty())
	{
		ui.leParaCode->setError(tr("ParaName cant be empty!"));
		bValid = false;
	}
	return bValid;
}

void ParameterDictionary::init()
{
	{
		ui.cbTypeCode->view()->setAlternatingRowColors(true);
		connect(ui.cbTypeCode, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
			[this](const QString &text) 
		{
			if (PARASETCTL->getParadict().contains(text))
				//ui.cbTypeName->setText(PARASETCTL->getParadict()[text].getTypeName());
				ui.cbTypeName->setText(PARASETCTL->getParadict()[text].getParaName());
			else
				ui.cbTypeName->setText("");
		});
	}
	{
		ui.treeView->setAlternatingRowColors(true);
		connect(ui.treeView, &QTreeView::clicked, [this](const QModelIndex &index) {
			QVariant s = index.sibling(index.row(), eParaCode).data();
			if (s.isValid())
			{
				if (PARASETCTL->getParadict().contains(s.toString()))
				{
					CParaDict dic = PARASETCTL->getParadict()[s.toString()];
					setViewData(dic);
				}
				setCurrentData(getViewData());
			}
		});
		initDateView();
	}
}

void ParameterDictionary::slotSkinChange()
{
	ui.pbAdd->setText(tr("add"));
	ui.pbAdd->setIcon(QIcon(qutil::skin("add-click-hover.png")));
	ui.pbModify->setText(tr("modify"));
	ui.pbModify->setIcon(QIcon(qutil::skin("revise-click-hover.png")));
	ui.pbDelete->setText(tr("delete"));
	ui.pbDelete->setIcon(QIcon(qutil::skin("delete-click-hover.png")));
}

void ParameterDictionary::slotAdd()
{
	if (!checkValid()) return;

	QList<int> cols = QList<int>{ 0,1,2,3,4,5 };
	QList<int> colsTable = QList<int>{ 0 };

	CParaDict val = getViewData();

	CParaDict oldVal;
	if (PARASETCTL->getParadict(val.getTypeCode(), val.getParaCode(), oldVal)) {
		ShowWarnMessage(tr("add"), tr("The para code already exists").arg(val.getTypeCode()).arg(val.getParaCode()), this);
		return;
	}
	QString k = val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode();
	if (PARASETCTL->getParadict().contains(k))
	{
		ShowWarnMessage(tr("add"), tr("type-code/paracode cant be equal to his-type-code/his-paracode!").arg(val.getTypeCode()).arg(val.getParaCode()), this);
		return;
	}
	
	if (!val.getParaCode().isEmpty())
	{// 新增参数
		/*if (val.getParaName().isEmpty())
		{
			ShowWarnMessage(tr("add"), tr("paraname cant be empty!"), this);
			return;
		}*/
		/*if (val.getTypeName().isEmpty())
		{
			ShowWarnMessage(tr("add"), tr("typename cant be empty!"), this);
			return;
		}*/
		/*if (!PARASETCTL->getParadict().contains(val.getTypeCode()))
		{
			ShowWarnMessage(tr("add"), tr("typecode dont exist!"), this);
			return;
		}*/
	}
	else
	{//新增类型
		if (!val.getParaName().isEmpty())
		{
			ShowWarnMessage(tr("add"), tr("when add type, paraname should be empty!"), this);
			return;
		}
	}
	QString err;
	if (PARASETCTL->setParadict(val, err)) {
		ShowSuccessMessage(tr("add"), tr("add success."), this);
		//addData(val);
		addTree(m_tree, m_model, val.getParaCode(), val.getTypeCode(), val, cols);
		if (val.getTypeCode().isEmpty() && !m_table.contains(val.getParaCode()))
			addRow(m_table, m_modelCombobox, val.getTypeCode(), val, colsTable);

		//locateData(val);
		locator(m_tree, val.getParaCode());
	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void ParameterDictionary::slotModify()
{
	if (!checkValid()) return;

	QList<int> cols = QList<int>{ 0,1,2,3,4,5 };
	QList<int> colsTable = QList<int>{ 0 };

	CParaDict val = getViewData();
	if (this->isEqual(val)) {
		ShowWarnMessage(tr("modify"), tr("Records do not change, do not need to modify!"), this);
		return;
	}
	if (this->isKeyModify(val)) {
		ShowWarnMessage(tr("modify"), tr("param code can not be modified!"), this);
		return;
	}
	if (getCurrentData().getParaCode().isEmpty())
	{// type
		if (!val.getParaCode().isEmpty() || !val.getParaName().isEmpty())
		{
			ShowWarnMessage(tr("modify"), tr("when modifying type, cant set para!"), this);
			return;
		}
	}
	else
	{// para
		if (val.getParaName().isEmpty())
		{
			ShowWarnMessage(tr("modify"), tr("when modifying para, para cant be empty!"), this);
			return;
		}
		if (!PARASETCTL->getParadict().contains(val.getTypeCode()))
		{
			ShowWarnMessage(tr("modify"), tr("typecode dont exist!"), this);
			return;
		}
	}

	QString err;
	if (PARASETCTL->setParadict(val, err)) {
		ShowSuccessMessage(tr("modify"), tr("modify success."), this);
	
		addTree(m_tree, m_model, val.getParaCode(), val.getTypeCode(), val, cols);
		if (val.getTypeCode().isEmpty() && !m_table.contains(val.getParaCode())) {
			addRow(m_table, m_modelCombobox, val.getTypeCode(), val, colsTable);
		}
			
	
		locator(m_tree, val.getParaCode());
	}
	else {
		ShowErrorMessage(tr("modify"), err.isEmpty()?tr("modify fail."):err, this);
	}
}

void ParameterDictionary::slotDelete()
{
	if (!checkValid()) return;

	if (MessageBoxWidget::Yes == ShowQuestionMessage(tr("delete"), tr("confirm to delete."), this)) {
		CParaDict val = getViewData();
		CParaDict oldVal;
		bool isFind = PARASETCTL->getParadict(val.getTypeCode(), val.getParaCode(), oldVal);
		if (!isFind) {
			ShowWarnMessage(tr("delete"), tr("The paradict is not existing!"), this);
			return;
		}
		if (val.getParaCode().isEmpty())
		{// 删除类型
			QMap<QString, CParaDict> cld;
			PARASETCTL->getAllParadict(val.getTypeCode(), cld);
			if (!cld.isEmpty())
			{
				ShowWarnMessage(tr("delete"), tr("The type has children, cant delete!"), this);
				return;
			}
		}
		
		QString err;
		if (PARASETCTL->removeParadict(val.getTypeCode(), val.getParaCode(), err)) {
			ShowSuccessMessage(tr("delete"), tr("delete success."), this);
			delTree(m_tree, m_model, val.getParaCode(), val.getTypeCode());
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void ParameterDictionary::initDateView()
{
	//************************************
	QTableView *tv = new QTableView(ui.cbTypeCode);
	tv->horizontalHeader()->setVisible(false);
	tv->verticalHeader()->setVisible(false);
	tv->horizontalHeader()->setStretchLastSection(true);

	if (nullptr==m_modelCombobox) m_modelCombobox=new QStandardItemModel(0,1,this);	
	ui.cbTypeCode->setModel(m_modelCombobox);
	ui.cbTypeCode->setView(tv);
	//*************************************
	QList<int> cols = QList<int>{ 0,1,2,3,4};
	QList<int> colsTable = QList<int>{ 0};
	if (m_model) m_model->clear();
	QStringList treeHeader;
	treeHeader << tr("paracode") << tr("paraname") << tr("typecode")<<tr("typename")<<tr("paraexplain");
	if (!m_model) m_model = new QStandardItemModel(0, treeHeader.size(), this);
	m_model->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_model);
	ui.treeView->setColumnWidth(0, 200);
	ui.treeView->setColumnWidth(1, 260);

	ui.treeView->hideColumn(2);
	ui.treeView->hideColumn(3);

	QMap<QString,CParaDict> val = PARASETCTL->getParadict();
	
	for (QMap<QString, CParaDict>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		addTree(m_tree, m_model, itor.value().getParaCode(), itor.value().getTypeCode(), itor.value(), cols);
		
		if (itor.value().getTypeCode().isEmpty() && !m_table.contains(itor.value().getParaCode())) {
			addRow(m_table, m_modelCombobox, itor.value().getTypeCode(), itor.value(), colsTable);
		}
	}
	if (!val.isEmpty())
	{
		locator(m_tree,val[val.keys().back()].getParaCode());
	}
	else
	{
		bwClear();
	}
}

CParaDict ParameterDictionary::getViewData()
{
	CParaDict result;
	result.setTypeCode(ui.cbTypeCode->currentText().trimmed());
	
	CParaDict t;

	if (PARASETCTL->getParaDict(ui.cbTypeCode->currentText().trimmed(),t))
		result.setTypeName(t.getParaName());
	else
		result.setTypeName(ui.cbTypeName->text().trimmed());
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
	ui.cbTypeName->setText(val.getTypeName());
	ui.pteParaExplain->setPlainText(val.getParaExplain());
	setCurrentData(getViewData());
}

//******************************************************************
//BWTreeOper pure virtual function  
//******************************************************************

void ParameterDictionary::bwLocate(const QString &code) {
	QModelIndexList findIndex = this->m_model->match(this->m_model->index(0, 0), Qt::DisplayRole, code, 1, Qt::MatchRecursive);
	if (findIndex.size() > 0)
	{
		this->ui.treeView->setCurrentIndex(findIndex[eParaCode]);
		this->ui.treeView->clicked(findIndex[eParaCode]);
	}
	else
	{
		this->bwClear();
	}
}

void ParameterDictionary::bwClear() {
	ui.leParaCode->setText("");
	ui.leParaName->setText("");
	ui.cbTypeCode->setCurrentText("");
	ui.cbTypeName->setText("");
	ui.pteParaExplain->setPlainText("");
}

bool ParameterDictionary::recordExist(const QString &val) {
	if (PARASETCTL->getParadict().contains(val))
		return true;
	return false;
}

CParaDict ParameterDictionary::getTFromDB(const QString &code, QString &parentCode) {
	CParaDict val;
	val = PARASETCTL->getParadict()[code];
	parentCode = val.getTypeCode();
	return val;
}

void ParameterDictionary::packQStandardItem(QList<QStandardItem *> &childItems, const CParaDict &val, const QList<int> cols) {
	
	for (int i = 0; i < eEnd; i++)
	{
		switch (i)
		{
		case eParaCode:
		{
			QStandardItem *iParaCode = new QStandardItem(val.getParaCode());
			if(cols.contains(eParaCode))childItems.push_back(iParaCode);
			break;
		}
		case eParaName:
		{
			QStandardItem *iParaName = new QStandardItem(val.getParaName());
			if (cols.contains(eParaName))childItems.push_back(iParaName);
			break;
		}

		case eTypeCode:
		{
			QStandardItem *iTypeCode = new QStandardItem(val.getTypeCode());
			if (cols.contains(eTypeCode))childItems.push_back(iTypeCode);
			break;
		}
		case eTypeName:
		{
			QStandardItem *iTypeName = new QStandardItem(val.getTypeName());
			if (cols.contains(eTypeCode))childItems.push_back(iTypeName);
			break;
		}
		case eParaExplain:
		{
			QStandardItem *explain = new QStandardItem(val.getParaExplain());
			if(cols.contains(eParaExplain))childItems.push_back(explain);
			break;
		}
		default:
			break;
		}
	}
}

void ParameterDictionary::updateChildNode(const CParaDict &val) {
	
	QList<int> cols = QList<int>{ 0,1,2,3,4};
	QList<int> colsTable = QList<int>{ 0 };
	
	if (m_tree.contains(val.getParaCode()))
	{
		if (val.getTypeCode() != m_tree[val.getParaCode()][eTypeCode]->text()) {

			QString code = val.getParaCode();

			CParaDict del(
				m_tree[val.getParaCode()][eTypeCode]->text(),
				m_tree[val.getParaCode()][eTypeName]->text(),
				m_tree[val.getParaCode()][eParaCode]->text(),
				m_tree[val.getParaCode()][eParaName]->text(),
				m_tree[val.getParaCode()][eParaExplain]->text()
			);
			delTree(m_tree, m_model, del.getParaCode(), val.getTypeCode());
			if (del.getTypeCode().isEmpty()) delRow(m_table, m_modelCombobox, del.getParaCode());

			addTree(m_tree, m_model, val.getParaCode(), val.getTypeCode(), val, cols);
			if (val.getTypeCode().isEmpty())addRow(m_table, m_modelCombobox, val.getParaCode(), val, colsTable);

		}
		else if (m_tree[val.getParaCode()].size() == eEnd) {
			m_tree[val.getParaCode()][eTypeCode]->setText(val.getTypeCode());
			m_tree[val.getParaCode()][eTypeName]->setText(val.getParaName());
			
			m_tree[val.getParaCode()][eParaName]->setText(val.getParaName());
			m_tree[val.getParaCode()][eParaExplain]->setText(val.getParaExplain());

			m_tree[val.getParaCode()][eTypeCode]->setToolTip(val.getTypeCode());
			m_tree[val.getParaCode()][eTypeName]->setToolTip(val.getParaName());
			
			m_tree[val.getParaCode()][eParaName]->setToolTip(val.getParaName());
			m_tree[val.getParaCode()][eParaExplain]->setToolTip(val.getParaExplain());

		}
	}

}