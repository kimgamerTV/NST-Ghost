#ifndef QTLINGO_GLOBAL_H
#define QTLINGO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTLINGO_LIBRARY)
#define QTLINGO_EXPORT Q_DECL_EXPORT
#else
#define QTLINGO_EXPORT Q_DECL_IMPORT
#endif

#endif // QTLINGO_GLOBAL_H
