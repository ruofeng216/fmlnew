#include "product_manage.h"
#include "util/util.h"

ProductManage::ProductManage(QWidget *parent)
	: BodyWidget(parent)
{
	ui.setupUi(this);
	slotSkinChange();
}

ProductManage::~ProductManage()
{
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