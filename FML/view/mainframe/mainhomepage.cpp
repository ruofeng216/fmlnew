#include "mainhomepage.h"
#include "util/pro.h"

MainHomePage::MainHomePage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setProperty("subwndid", Main_HomePage);
}

MainHomePage::~MainHomePage()
{
}
