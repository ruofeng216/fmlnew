#pragma once

#include <QThread>

class DemoCrawler : public QThread
{
	Q_OBJECT

public:
	DemoCrawler(QObject *parent = NULL);
	~DemoCrawler();

	void run();
 
};
