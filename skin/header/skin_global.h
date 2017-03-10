#ifndef SKIN_GLOBAL_H
#define SKIN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SKIN_LIB
# define SKIN_EXPORT Q_DECL_EXPORT
#else
# define SKIN_EXPORT Q_DECL_IMPORT
#endif

#endif // SKIN_GLOBAL_H
