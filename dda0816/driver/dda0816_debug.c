/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#define __NO_VERSION__

#include <dda0816_P.h>


/* default debugging level */

#if DEBUG
#ifndef DEFAULT_DEBUG
unsigned int   dbgMask  = (DBG_ENTRY | DBG_EXIT | DBG_BRANCH | DBG_DATA | DBG_INTR | DBG_1PPL) & ~DBG_ALL;
#else
unsigned int   dbgMask  = 0;
#endif
#else
unsigned int   dbgMask  = 0;
#endif


/* static variables for debugging */

int   fidx     = 0;		/* index into function name stack		*/
char *fstk[32] =			/* function name stack (NO OVERFLOW PROTECTION)	*/
{
	"NULL"
};

/* format patterns for different function levels 
 * so debugging output is easier to read
 * this will cost a little memory of course but there is 
 * not much perfomance drop */

char *ffmt[32] = 
{
   "",
   " ",
   "  ",
   "   ",
   "    ",
   "     ",
   "      ",
   "       ",
   "        ",
   "         ",
   "          ",
   "           ",
   "            ",
   "             ",
   "              ",
   "               ",
   "*",
   "* ",
   "*  ",
   "*   ",
   "*    ",
   "*     ",
   "*      ",
   "*       ",
   "*        ",
   "*         ",
   "*          ",
   "*           ",
   "*            ",
   "*             ",
   "*              ",
   "*               "
};

