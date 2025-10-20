#ifndef BGA_GLOBAL_H
#define BGA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BGA_LIBRARY)
#define BGA_EXPORT Q_DECL_EXPORT
#else
#define BGA_EXPORT Q_DECL_IMPORT
#endif

#endif // BGA_GLOBAL_H
