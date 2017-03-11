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

void Skin::setSkinName(const QString &skinName)
{
	m_skinName = skinName;
}

const QString& Skin::skinName() const
{
	return m_skinName;
}

QString Skin::path(const QString &fileName)
{
	return QString(":/%1/%2").arg(m_skinName).arg(fileName);
}
