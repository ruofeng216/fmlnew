#include "key_point_definition.h"
#include "util/util.h"
#include "controller/qcontrollermanager.h"
#include <QStandardItemModel>
#include "view/commonui/message_box_widget.h"

KeyPointDefinition::KeyPointDefinition(QWidget *parent)
	: BodyWidget(parent)
{
	ui.setupUi(this);
	init();
	slotSkinChange();
}

KeyPointDefinition::~KeyPointDefinition()
{
}

void KeyPointDefinition::init()
{

}

void KeyPointDefinition::slotSkinChange()
{
	ui.pbAdd->setText(tr("add"));
	ui.pbAdd->setIcon(QIcon(qutil::skin("add-click-hover.png")));
	ui.pbModify->setText(tr("modify"));
	ui.pbModify->setIcon(QIcon(qutil::skin("revise-click-hover.png")));
	ui.pbDelete->setText(tr("delete"));
	ui.pbDelete->setIcon(QIcon(qutil::skin("delete-click-hover.png")));
	qutil::setWeekendStyle(ui.deStart);
	qutil::setWeekendStyle(ui.deEnd);
	qutil::setWeekendStyle(ui.deCalendar);
}