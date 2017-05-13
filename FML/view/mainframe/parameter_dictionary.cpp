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
	if (ui.cbTypeCode->currentText().isEmpty())
	{
		ui.cbTypeCode->setError(tr("Typecode cant be empty!"));
		bValid = false;
	}
	if (ui.cbTypeName->text().isEmpty())
	{
		ui.cbTypeName->setError(tr("Typecode-name cant be empty!"));
		bValid = false;
	}
	if (!ui.leParaCode->text().isEmpty() && ui.leParaName->text().isEmpty())
	{
		ui.leParaName->setError(tr("ParaName cant be empty!"));
		bValid = false;
	}
	if (ui.leParaCode->text().isEmpty() && !ui.leParaName->text().isEmpty())
	{
		ui.leParaCode->setError(tr("ParaCode cant be empty!"));
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
				ui.cbTypeName->setText(PARASETCTL->getParadict()[text].getTypeName());
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
	QString k = val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode();
	if (PARASETCTL->getParadict().contains(k))
	{
		ShowWarnMessage(tr("add"), tr("type-code/paracode cant be equal to his-type-code/his-paracode!").arg(val.getTypeCode()).arg(val.getParaCode()), this);
		return;
	}
	
	if (!val.getParaCode().isEmpty())
	{// 新增参数
		if (val.getParaName().isEmpty())
		{
			ShowWarnMessage(tr("add"), tr("paraname cant be empty!"), this);
			return;
		}
		if (val.getTypeName().isEmpty())
		{
			ShowWarnMessage(tr("add"), tr("typename cant be empty!"), this);
			return;
		}
		if (!PARASETCTL->getParadict().contains(val.getTypeCode()))
		{
			ShowWarnMessage(tr("add"), tr("typecode dont exist!"), this);
			return;
		}
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
		addData(val);
		locateData(val);
	} else {
		ShowErrorMessage(tr("add"), err.isEmpty()?tr("add fail."):err, this);
	}
}

void ParameterDictionary::slotModify()
{
	if (!checkValid()) return;
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
		addData(val);
		locateData(val);
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
			delData(val);
		} else {
			ShowErrorMessage(tr("delete"), err.isEmpty()?tr("delete fail."):err, this);
		}
	}
}

void ParameterDictionary::initDateView()
{
	if (m_model) m_model->clear();
	QStringList treeHeader;
	treeHeader << tr("paracode") << tr("paraname") << tr("paraexplain");
	if (!m_model) m_model = new QStandardItemModel(0, treeHeader.size(), this);
	m_model->setColumnCount(treeHeader.size());
	for (int i = 0; i < treeHeader.size(); i++)
		m_model->setHeaderData(i, Qt::Horizontal, treeHeader[i]);
	ui.treeView->setModel(m_model);
	ui.treeView->setColumnWidth(0, 400);

	QMap<QString,CParaDict> val = PARASETCTL->getParadict();
	for (QMap<QString, CParaDict>::const_iterator itor = val.begin();
		itor != val.end(); itor++)
	{
		addData(itor.value());
	}
	if (!val.isEmpty())
	{
		locateData(val[val.keys().back()]);
	}
	else
	{
		clear();
	}
}
CParaDict ParameterDictionary::getViewData()
{
	CParaDict result;
	result.setTypeCode(ui.cbTypeCode->currentText().trimmed());
	CParaDict t;
	if (PARASETCTL->getParadict(ui.cbTypeCode->currentText().trimmed(), "", t))
		result.setTypeName(t.getTypeName());
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
void ParameterDictionary::packItem(QList<QStandardItem *> &childItems, const CParaDict &val)
{
	if (val.getTypeCode().isEmpty()) return;
	for (int i = 0; i < eEnd; i++)
	{
		switch (i)
		{
		case eParaCode:
		{
			QStandardItem *code = new QStandardItem(val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode());
			childItems.push_back(code);
			code->setToolTip(val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode());
			break;
		}
		case eParaName:
		{
			QStandardItem *name = new QStandardItem(val.getParaCode().isEmpty() ? val.getTypeName() : val.getParaName());
			childItems.push_back(name);
			name->setToolTip(val.getParaCode().isEmpty() ? val.getTypeName() : val.getParaName());
			break;
		}
		case eParaExplain:
		{
			QStandardItem *explain = new QStandardItem(val.getParaExplain());
			childItems.push_back(explain);
			explain->setToolTip(qutil::splitTooltip(explain->text(), 200));
			break;
		}
		default:
			break;
		}
	}
}
void ParameterDictionary::addData(const CParaDict & val)
{
	auto insertRoot = [this](const CParaDict &val) {
		QList<QStandardItem *> items;
		this->packItem(items, val);
		if (items.size() > 0) {
			this->m_model->appendRow(items);
			this->m_tree[val.getTypeCode()] = items;
		}
		this->ui.cbTypeCode->addItems(QStringList() << val.getTypeCode());
	};
	auto insertChild = [this](const CParaDict &val) {
		if (!val.getTypeCode().isEmpty() &&
			!val.getParaCode().isEmpty() &&
			!this->m_tree.contains(val.getParaCode()) && 
			this->m_tree.contains(val.getTypeCode()))
		{
			QList<QStandardItem *> items;
			this->packItem(items, val);
			if (items.size() > 0) {
				this->m_tree[val.getTypeCode()].front()->appendRow(items);
				this->m_tree[val.getParaCode()] = items;
			}
		}
	};
	auto updateChild = [this](const CParaDict &val) {
		QString k = val.getParaCode().isEmpty() ? val.getTypeCode() : val.getParaCode();
		if (!val.getTypeCode().isEmpty() && 
			this->m_tree.contains(k))
		{
			if (val.getParaCode().isEmpty())
			{// 类型
				this->m_tree[k][eParaCode]->setText(k);
				this->m_tree[k][eParaName]->setText(val.getTypeName());
				this->m_tree[k][eParaExplain]->setText(val.getParaExplain());
			}
			else
			{// 参数
				if (val.getTypeCode() != this->m_tree[k][eParaCode]->parent()->text())
				{
					this->delData(val);
					this->addData(val);
				}
				else if (this->m_tree[k].size() == eEnd) {
					this->m_tree[k][eParaCode]->setText(k);
					this->m_tree[k][eParaName]->setText(val.getParaName());
					this->m_tree[k][eParaExplain]->setText(val.getParaExplain());
					
					this->m_tree[k][eParaCode]->setToolTip(k);
					this->m_tree[k][eParaName]->setToolTip(val.getParaName());
					this->m_tree[k][eParaExplain]->setToolTip(qutil::splitTooltip(val.getParaExplain(), 200));
				}
			}
		}
	};
	if (val.getParaCode().isEmpty())
	{// 根节点
		if (m_tree.contains(val.getTypeCode()))
		{
			if (m_tree[val.getTypeCode()].isEmpty())
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
		if (m_tree.contains(val.getTypeCode()) && !m_tree[val.getTypeCode()].isEmpty())
		{// 父节点存在
			if (m_tree.contains(val.getParaCode()))
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
			if (PARASETCTL->getParadict().contains(val.getTypeCode()))
			{// 父节点有效
				CParaDict parentCode;
				if (PARASETCTL->getParadict(val.getTypeCode(), "", parentCode))
				{
					addData(parentCode);
					insertChild(val);
				}
			}
			else
			{
				qWarning() << "Do not exist parent-code: " << val.getTypeCode();
			}
		}
	}
}
void ParameterDictionary::delData(const CParaDict & val)
{
	auto delRoot = [this](const QString &val) {
		if (this->m_tree.contains(val))
		{
			this->m_model->removeRow(m_tree[val].front()->row());
			this->m_tree.remove(val);
		}
	};
	auto delChild = [this](const QString &val) {
		if (this->m_tree.contains(val))
		{
			this->m_model->removeRow(m_tree[val].front()->row(), m_tree[val].front()->parent()->index());
			this->m_tree.remove(val);
		}
	};
	if (!val.getTypeCode().isEmpty() && val.getParaCode().isEmpty())
	{// 根节点
		if (m_model->rowCount() <= 1)
		{
			clear();
		}
		else
		{
			if (m_tree.contains(val.getTypeCode()))
			{
				int curRow = m_tree[val.getTypeCode()].front()->row();
				int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
				QStandardItem* p = m_model->item(nearRow);
				if (p)
				{
					QString nearCode = p->text();
					CParaDict nearP;
					if (PARASETCTL->getParadict(nearCode, "", nearP))
						locateData(nearP);
				}
			}
		}
		delRoot(val.getTypeCode());
	}
	else
	{
		if (m_tree.contains(val.getParaCode()) && m_tree[val.getParaCode()].front()->rowCount() <= 1)
		{
			CParaDict nearP;
			if (PARASETCTL->getParadict(val.getTypeCode(), "", nearP))
				locateData(nearP);
		}
		else if (m_tree.contains(val.getParaCode()) && m_tree.contains(val.getTypeCode()))
		{
			int curRow = m_tree[val.getParaCode()].front()->row();
			int nearRow = curRow - 1 >= 0 ? curRow - 1 : curRow + 1;
			QStandardItem* p = m_tree[val.getTypeCode()].front()->child(nearRow);
			if (p)
			{
				QString nearCode = p->text();
				CParaDict nearP;
				if (PARASETCTL->getParadict(val.getTypeCode(), nearCode, nearP))
					locateData(nearP);
			}
		}
		delChild(val.getParaCode());
	}
}
void ParameterDictionary::locateData(const CParaDict & val)
{
	auto locate = [this](const QString &dkey) {
		QModelIndexList findIndex = this->m_model->match(this->m_model->index(0, 0), Qt::DisplayRole, dkey, 1, Qt::MatchRecursive);
		if (findIndex.size() > 0)
		{
			this->ui.treeView->setCurrentIndex(findIndex[eParaCode]);
			this->ui.treeView->clicked(findIndex[eParaCode]);
		}
		else
		{
			this->clear();
		}
	};
	if (!val.getTypeCode().isEmpty() &&
		val.getParaCode().isEmpty() &&
		m_tree.contains(val.getTypeCode()))
	{// 根结点 - 类型
		locate(val.getTypeCode());
	}
	else if (!val.getTypeCode().isEmpty() &&
		!val.getParaCode().isEmpty() &&
		m_tree.contains(val.getParaCode()))
	{// 子节点 - 参数节点
		locate(val.getParaCode());
	}
	else if (!val.getTypeCode().isEmpty() &&
		!val.getParaCode().isEmpty() &&
		!m_tree.contains(val.getParaCode()))
	{// 子节点 - 参数节点不存在
		locate(val.getTypeCode());
	}
	else
		clear();
}
void ParameterDictionary::clear()
{
	ui.leParaCode->setText("");
	ui.leParaName->setText("");
	ui.cbTypeCode->setCurrentText("");
	ui.cbTypeName->setText("");
	ui.pteParaExplain->setPlainText("");
}