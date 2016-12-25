#include "basewebengineview.h"
#include "WebEnginePage.h"
#include "basewebchannel.h"
#include <QUuid>
#include <QDebug>
#include "util/util.h"

BaseWebEngineView::BaseWebEngineView(QWidget *parent)
	: QWebEngineView(parent)
	, m_pWebChannel(NULL)
	, m_pWebEnginePage(new WebEnginePage(this))
	, m_isWebLoadFinished(false)
{
	this->setPage(m_pWebEnginePage);
	connect(this->page(), SIGNAL(loadFinished(bool)), this, SLOT(finish(bool)));
	connect(this->page(), SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
	connect(this->page(), SIGNAL(sigLoadUrl(const QUrl &)), this, SLOT(hyperlinkClicked(const QUrl &)));
	setAcceptDrops(false);
	setMouseTracking(true);

	if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7) {
		page()->setBackgroundColor(Qt::transparent);
	}
	else {
		page()->setBackgroundColor(QColor("#0f0f10"));
	}
	setFocusPolicy(Qt::ClickFocus);
	// 设置名称
	setObjectName("webview_" + QUuid::createUuid().toString());
	// 页面初始化
	//page()->load(QUrl("about:blank"));

}

BaseWebEngineView::~BaseWebEngineView()
{
}
void BaseWebEngineView::loadHtml(const QString &url)
{
	m_isWebLoadFinished = false;
	if (!url.isEmpty() && url.startsWith(":/"))
	{
		page()->load(QUrl("qrc"+url));
	}
	else
	{
		// 加载内嵌网页
		if (url.isEmpty())
		{
			page()->load(QUrl("about:blank"));
		}
		else
		{
			page()->load(QUrl("file:///" + url));
		}
	}
}
// 加载成功
void BaseWebEngineView::finish(bool bsuccess)
{
	if (bsuccess)
	{
		m_isWebLoadFinished = true;

		for (int i = 0, count = m_loadFinishRunList.size(); i < count; i++) {
			runjs(m_loadFinishRunList[i].first, m_loadFinishRunList[i].second);
		}
		m_loadFinishRunList.clear();
	}
}
// 加载进度
void BaseWebEngineView::slotLoadProgress(int progress)
{
	qDebug() << "slotLoadProgress, url:" << this->url().toString() << " [" << progress << "]";
}
// 链接响应
void BaseWebEngineView::hyperlinkClicked(const QUrl &links)
{
	if (!links.isEmpty())
	{
		m_isWebLoadFinished = false;
		page()->load(links);
	}
}

// 包装runJavaScript，方便管理
void BaseWebEngineView::runjs(const QString &js, const JsResponseCb &cb)
{
	if (m_isWebLoadFinished) {
		if (cb) {
			page()->runJavaScript(js, cb);
		}
		else {
			page()->runJavaScript(js);
		}
	}
	else {
		bool isNextRepeat = false;
		if (!m_loadFinishRunList.isEmpty()) {
			const QPair<QString, JsResponseCb> &last = m_loadFinishRunList.last();
			isNextRepeat = last.first == js;
		}
		// 下一个命令是重复的
		if (isNextRepeat) {
			qDebug() << "runjs filter:" << js;
		}
		else {
			qDebug() << "runjs after load finished:" << js;
			m_loadFinishRunList.append(qMakePair(js, cb));
		}
	}
}


//////////////////////////////////////////////////////////////////
//DemoWebview
DemoWebview::DemoWebview(QWidget *parent)
	: BaseWebEngineView(parent)
{
	setWebChannel();
}
DemoWebview::~DemoWebview()
{

}

void DemoWebview::setWebChannel()
{
	m_pWebChannel = new DemolWebChannel(this);
}

void DemoWebview::pushDemoData(const demoStruct &val)
{
	demoStruct t = val;
	QString str = QString("updateData('%1')").arg(json::toString(t.toJson()));
	runjs(str);
}

QString DemoWebview::getTitle()
{
	return page()->title();
}

//DemoWebview
DemoWebview1::DemoWebview1(QWidget *parent)
	: BaseWebEngineView(parent)
{
	setWebChannel();
}
DemoWebview1::~DemoWebview1()
{

}

void DemoWebview1::setWebChannel()
{
	m_pWebChannel = new DemolWebChannel1(this);
}

QString DemoWebview1::getTitle()
{
	return page()->title();
}