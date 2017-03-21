#include "main_home_page.h"
#include "util/pro.h"

MainHomePage::MainHomePage(QWidget *parent)
	: BodyWidget(parent)
{
	ui.setupUi(this);
	setProperty("subwndid", Main_HomePage);
}

MainHomePage::~MainHomePage()
{
}

void MainHomePage::slotSkinChange()
{

}