#ifndef INJECTION_GLOBAL_H
#define INJECTION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(INJECTION_LIBRARY)
#define INJECTION_EXPORT Q_DECL_EXPORT
#else
#define INJECTION_EXPORT Q_DECL_IMPORT
#endif

#endif // INJECTION_GLOBAL_H
