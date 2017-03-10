#include "skin.h"

Skin::Skin()
{
}

Skin::~Skin()
{
}

Skin& Skin::instance()
{
	static Skin s_inst;
	return s_inst;
}

void Skin::setSkin(const QString &skinName)
{
	m_skinName = skinName;
}

QString Skin::path(const QString &fileName)
{
	return QString(":/%1/%2").arg(m_skinName).arg(fileName);
}
