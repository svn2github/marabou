/* This central include file defines data types for all platforms */

#ifndef TYPEDEF_H
#define TYPEDEF_H

/* Platform independent definitions */

typedef          char   CHARS;
typedef unsigned char   CHARU;
typedef          char   INTS1;
typedef unsigned char   INTU1;
typedef          short  INTS2;
typedef unsigned short  INTU2;
typedef          int    INTS4;
typedef unsigned int    INTU4;
typedef          float  REAL4;
typedef          double REAL8;

/* Platform specific definitions */

#ifdef _AIX
typedef          long long INTS8;
typedef unsigned long long INTU8;
typedef unsigned long ADDRS;
#endif

#ifdef __osf__
#ifdef __alpha
#define AXP
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
#endif
#endif

#ifdef _HPUX_SOURCE
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
#endif

#ifdef Lynx
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
#endif

#ifdef ultrix
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
#endif

#ifdef VMS
typedef          long INTS8;
typedef unsigned long INTU8;
typedef unsigned long ADDRS;
#ifdef __alpha
#define AXP
#endif
#ifdef VAX
#endif
#endif


#endif
