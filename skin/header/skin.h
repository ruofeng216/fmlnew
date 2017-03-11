#pragma  once

#include "skin_global.h"
#include <QString>

class SKIN_EXPORT Skin
{
public:
	static Skin& instance();

	void setSkinName(const QString &skinName);
	const QString& skinName() const;
	QString path(const QString &fileName);

private:
	Skin();
	~Skin();

private:
	QString m_skinName;
};


