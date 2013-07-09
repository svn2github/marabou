#ifndef __SIS3302_BUS_TRAP_H__
#define __SIS3302_BUS_TRAP_H__

#ifdef BUS_TRAP_CHECK_ON
#define BUS_TRAP_FLAG	Bool_t bus_trap_flag = FALSE
#else
#define BUS_TRAP_FLAG
#endif

#ifdef BUS_TRAP_CHECK_ON
#define INIT_BUS_TRAP	signal(SIGBUS, sis3302_catchBusTrap)
#else
#define INIT_BUS_TRAP
#endif

#ifdef BUS_TRAP_CHECK_ON
#define CLEAR_BUS_TRAP_FLAG		bus_trap_flag = FALSE
#else
#define CLEAR_BUS_TRAP_FLAG
#endif

#ifdef BUS_TRAP_CHECK_ON
#define CHECK_BUS_TRAP(m,a,c)	sis3302_checkBusTrap(m, a, c)
#else
#define CHECK_BUS_TRAP(m,a,c)
#endif

#endif
