#ifndef QSYSTEMDETECTION_H
#define QSYSTEMDETECTION_H

#if defined(__linux__) || defined(__linux)
#define Q_OS_LINUX
#else
#error "Qt has not been ported to this OS - see http://www.qt-project.org/"
#endif

#define Q_OS_UNIX

#ifdef __LSB_VERSION__
#if __LSB_VERSION__ < 40
#error "This version of the Linux Standard Base is unsupported"
#endif

#ifndef QT_LINUXBASE
#define QT_LINUXBASE
#endif

#endif

#endif
