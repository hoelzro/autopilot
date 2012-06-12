#ifndef AP_PRIVATE_OS_H
#define AP_PRIVATE_OS_H

/* XXX make use of these */
#if unix
# define AP_OS_UNIX 1
#endif

#if linux
# define AP_OS_LINUX 1
#endif

#if __APPLE_CC__
# define AP_OS_OSX 1
# define AP_OS_UNIX 1
#endif

#if _WIN32
# define AP_OS_WINDOWS 1
#endif

#ifdef __HAIKU__
# define AP_OS_HAIKU 1
# define AP_OS_BEOS 1
#endif

#ifdef __FreeBSD__
# define AP_OS_FREEBSD 1
# define AP_OS_BSD 1
# define AP_OS_UNIX 1
#endif

#ifdef __NetBSD__
# define AP_OS_NETBSD 1
# define AP_OS_BSD 1
# define AP_OS_UNIX 1
#endif

#ifdef __OpenBSD__
# define AP_OS_OPENBSD 1
# define AP_OS_BSD 1
# define AP_OS_UNIX 1
#endif

#endif
