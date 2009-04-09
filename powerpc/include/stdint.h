#ifndef OUR_STDINT_H
#define OUR_STDINT_H

/* There is some amount of overlap with <sys/types.h> as known by inet code */
/* To avoid conflicts, first include <sys/types.h> then this stdint.h  */
#ifndef  __sys_types_h
typedef signed char			int8_t;
typedef short int			int16_t;
typedef int					int32_t;
#endif

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;

typedef unsigned char		bool_t;

#endif
