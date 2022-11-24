#ifndef QNUMERIC_H
#define QNUMERIC_H

#include "qglobal.h"

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsInf(double d);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsNaN(double d);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsFinite(double d);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION int qFpClassify(double val);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsInf(float f);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsNaN(float f);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION bool qIsFinite(float f);
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION int qFpClassify(float val);

#if QT_CONFIG(signaling_nan)
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION double qSNaN();
#endif

Q_CORE_EXPORT Q_DECL_CONST_FUNCTION double qQNaN();
Q_CORE_EXPORT Q_DECL_CONST_FUNCTION double qInf();

Q_CORE_EXPORT quint32 qFloatDistance(float a, float b);
Q_CORE_EXPORT quint64 qFloatDistance(double a, double b);

#define Q_INFINITY      (QT_PREPEND_NAMESPACE(qInf)())

#if QT_CONFIG(signaling_nan)
#define Q_SNAN          (QT_PREPEND_NAMESPACE(qSNaN)())
#endif

#define Q_QNAN          (QT_PREPEND_NAMESPACE(qQNaN)())

QT_END_NAMESPACE

#endif
