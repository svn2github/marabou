/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/

#ifndef KERNEL
#define __NO_VERSION__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/processor.h>              /* Processor type for cache alignment. */
#include <asm/bitops.h>
#include <asm/io.h>

#include <asm/segment.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/dma.h>

#include <asm/uaccess.h>
#endif



#include <dda0816_debug.h>
#include <dda0816_Proto.h>
#include <dda0816_ioctl.h>



#include <dda0816_iomap.h>


#include <dda0816_types.h>


#define LDDK_USE_PROCINFO 1

#define LDDK_USE_SYSCTL 1
#ifdef __KERNEL__
#include <linux/sysctl.h>
#include <dda0816_sysctl.h>
#endif



#ifndef dda0816_MAJOR
#define dda0816_MAJOR 120
#endif

extern int dda0816_errno;

#ifdef USE_LDDK_RETURN
#define LDDK_RETURN(arg) DBGout();return(arg)
#else
#define LDDK_RETURN(arg) return(arg)
#endif

