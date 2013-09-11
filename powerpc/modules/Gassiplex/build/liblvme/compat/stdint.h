/* $Header: /misc/hadaq/cvsroot/compat/stdint.h,v 1.5 2005/03/10 14:53:04 hadaq Exp $ */
#ifndef OUR_STDINT_H
#define OUR_STDINT_H

#ifdef HAVE_LIBCOMPAT

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;

#ifdef __cplusplus
}
#endif

#else
#include </usr/include/stdint.h>
#endif

#endif
