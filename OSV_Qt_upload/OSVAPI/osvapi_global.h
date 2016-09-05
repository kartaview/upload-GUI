#ifndef OSVAPI_GLOBAL_H
#define OSVAPI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OSVAPI_LIBRARY)
#  define OSVAPISHARED_EXPORT Q_DECL_EXPORT
#else
#  define OSVAPISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // OSVAPI_GLOBAL_H
