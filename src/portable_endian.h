/*
 * Public domain, stripped down version of:
 * https://gist.github.com/panzi/6856583
 */

#ifndef __PORTABLE_ENDIAN_H
#define __PORTABLE_ENDIAN_H

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#include <endian.h>

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>

#define htole32(x) OSSwapHostToLittleInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#elif defined(__OpenBSD__)

#include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#include <sys/endian.h>

#define le32toh(x) letoh32(x)

#elif defined(__WINDOWS__)

#include <winsock2.h>
#include <sys/param.h>

#if BYTE_ORDER == LITTLE_ENDIAN

#define htole32(x) (x)
#define le32toh(x) (x)

#elif BYTE_ORDER == BIG_ENDIAN

/* that would be xbox 360 */

#define htole32(x) __builtin_bswap32(x)
#define le32toh(x) __builtin_bswap32(x)

#else

#error byte order not supported

#endif

#else

#error platform not supported

#endif

#endif /* __PORTABLE_ENDIAN_H */
