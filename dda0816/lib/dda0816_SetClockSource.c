/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#include <dda0816_P.h>

        dda0816_SetClockSource(int fd,int source)
        {
          extern int errno;
          dda0816_int_par_t arg;
             arg.param = source;
		ioctl(fd,SETCLOCKSOURCE,(void *) &arg );
          errno = arg.error;
          return( arg.retval );          
        }


        
