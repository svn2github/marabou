/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#ifndef __DEBUG_INCLUDED
#if DEBUG       

#define DRIVER_NAME "dda0816"

#define DBG_ALL      (1 << 0)
#define DBG_ENTRY   ((1 << 1) | DBG_ALL)
#define DBG_EXIT    ((1 << 2) | DBG_ALL)
#define DBG_BRANCH  ((1 << 3) | DBG_ALL)
#define DBG_DATA    ((1 << 4) | DBG_ALL)
#define DBG_INTR    ((1 << 5) | DBG_ALL)
#define DBG_REG     ((1 << 6) | DBG_ALL)
#define DBG_SPEC    ((1 << 7) | DBG_ALL)
#define DBG_1PPL     (1 << 8)		/* one DBG print statement/line */

extern unsigned int dbgMask;


/* class of debug statements allowed		*/

extern int   fidx;     
extern char *fstk[];
extern char *ffmt[];

#define DBGprint(ms,ar)	{ if (dbgMask && (dbgMask & ms)) { printk("dda0816: - %s%s:",ffmt[fidx], fstk[fidx]); printk ar; printk("\n"); } }
#define DBGin(id)	{ fstk[++fidx] = id; DBGprint(DBG_ENTRY, ("in  ")); }
#define DBGout()	{ DBGprint(DBG_EXIT, ("out  ")); fidx--; }

#else					
#define DBGprint(ms,ar)	{ }
#define DBGin(id)	{ }
#define DBGout()	{ }
extern unsigned int dbgMask;

#endif					

#define __DEBUG_INCLUDED
#endif



