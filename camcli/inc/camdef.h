/*
 * |------------------------------------------------------|
 * | Copyright (c) 1991 , 1991 Ibex Computing SA          |
 * | All Rights Reserved                                  |
 * |------------------------------------------------------|
*/

/*
 * Maintainance : frank humbert (GSI)
 * Description  : CAMAC definition file, error codes
 * last update  : 24-May-1996 added new error codes
 *                14-Mar-2002 [RL] added controller codes
 *                                 added function "cdctrl"
 * file         : camdef.h
*/

#ifndef CAMDEF_H
#define CAMDEF_H

#define DFLT_CAMAC      "DFLT_CAMAC"

/* error messages */

#define M_EC_NORMAL           "OK"
#define M_EC_EBRANCH          "branch number out of bounds"
#define M_EC_ECRATE           "crate number out of bounds"
#define M_EC_ESTATION         "station number out of bounds"
#define M_EC_EADDRESS         "subaddress number out of bounds"
#define M_EC_EFUNCODE         "function code out of bounds"
#define M_EC_IFUNCODE         "illegal function code"
#define M_EC_ELAM	      "GL out of bounds"
#define M_EC_LAMLNK	      "LAM already linked"
#define M_EC_LAMNOTLNK        "operation performed on a LAM not linked"
#define M_EC_NOLAMINTR	      "task woke up but no LAM interrupt arrived"
#define M_EC_UNKNOWNDEV       "unknown device"
#define M_EC_DEVERR           "device access error"
#define M_EC_DEVTABLEFULL     "device table full"
#define M_EC_DEVBUSY          "device is busy (create new minor number)"
#define M_EC_ADDFAULT         "invalid parameter address"
#define M_EC_EPARAM           "invalid parameter"
#define M_EC_ECNAF            "invalid CNAF"
#define M_EC_SYSERR           "unspecified system error (check errno)"
#define M_EC_NODATABASE       "device database cannot be opened"
#define M_EC_REMOTEDEV        "device is controlled by remote host"
#define M_EC_BLKTIMEOUT       "block timeout in io_blk"
#define M_EC_ADDSCANTIMEOUT   "addscan timeout in io_addscan"
#define M_EC_EBLOCK           "transfer-block for CNAF's out of bounds"
#define M_EC_UNKNOWNRPC       "L - unknown RPC entry"
#define M_EC_UNKNOWNHOST      "L - unknown host name"
#define M_EC_UNKNOWNTRANSPORT "L - unknown transport protocol"
#define M_EC_RPCOPENERROR     "L - either unknown host name or server not available"
#define M_EC_NETXMITERR       "L - transmit error"
#define M_EC_NETRCVERR        "L - receive error"
#define M_EC_NETTIMEOUT       "L - timeout"
#define M_EC_NOPROC           "L - unsupported (remote) procedure"
#define M_EC_BADXMSGFMT       "L - bad (transmitted) message format"
#define M_EC_BADRMSGFMT       "L - bad (received) message format"
#define M_EC_SRVRBADMSGFMT    "R - bad (received) message format"
#define M_EC_RPCVERSION       "R - RPC version mismatch"
#define M_EC_SRVRVERSION      "R - server version mismatch"
#define M_EC_RPCNOPRIV        "R - authentication failure"
#define M_EC_NOSERVER         "R - server not available"
#define M_EC_SRVRNOPROC       "R - procedure not available"
#define M_EC_RPCPMAPERROR     "R - RPC portmap failure"
#define M_EC_UNKNOWNSRVR      "R - unknown remote server"
#define M_EC_SRVRERROR        "R - unspecified server error"
#define M_EC_RPCERROR         "R - unspecified network error"

/*
** error codes
*/
enum
{
    EC_NORMAL,
    EC_EBRANCH,             /* branch number out of bounds */
    EC_ECRATE,              /* crate number out of bounds */
    EC_ESTATION,            /* station number out of bounds */
    EC_EADDRESS,            /* subaddress number out of bounds */
    EC_EFUNCODE,            /* function code out of bounds */
    EC_IFUNCODE,            /* illegal function code */
    EC_ELAM,		    /* GL out of bounds */
    EC_LAMLNK,		    /* LAM already linked */
    EC_LAMNOTLNK,           /* operation performed on a LAM not linked */
    EC_NOLAMINTR,	    /* task woke up but no LAM interrupt arrived */
    EC_UNKNOWNDEV,          /* unknown device */
    EC_DEVERR,              /* device access error */
    EC_DEVTABLEFULL,        /* device table full */
    EC_DEVBUSY,		    /* device is busy (create new minor number) */
    EC_ADDFAULT,            /* invalid parameter address */
    EC_EPARAM,              /* invalid parameter */
    EC_ECNAF,               /* invalid CNAF */

    EC_SYSERR = 127,        /* unspecified system error (check errno) */

                            /* database errors */
    EC_NODATABASE = 128,    /* device database cannot be opened */
    EC_REMOTEDEV,           /* device is controlled by remote host */
    EC_BLKTIMEOUT,          /* 22/02/96 f.h. block timeout in io_blk */
    EC_ADDSCANTIMEOUT,      /* 26/02/96 f.h. addscan timeout in io_addscan */
    
                            /* RPC/network errors - L = local, R = remote */
    EC_EBLOCK,              /* transfer-block for CNAF's out of bounds */
    EC_UNKNOWNRPC = 256,    /* L - unknown RPC entry */
    EC_UNKNOWNHOST,         /* L - unknown host name */
    EC_UNKNOWNTRANSPORT,    /* L - unknown transport protocol */
    EC_RPCOPENERROR,        /* L - either EC_UNKNOWNHOST or
                                   EC_UNKNOWNTRANSPORT */
    EC_NETXMITERR,          /* L - transmit error */
    EC_NETRCVERR,           /* L - receive error */
    EC_NETTIMEOUT,          /* L - timeout */
    EC_NOPROC,              /* L - unsupported (remote) procedure */
    EC_BADXMSGFMT,          /* L - bad (transmitted) message format */
    EC_BADRMSGFMT,          /* L - bad (received) message format */
    EC_SRVRBADMSGFMT,       /* R - bad (received) message format */
    EC_RPCVERSION,          /* R - RPC version mismatch */
    EC_SRVRVERSION,         /* R - server version mismatch */
    EC_RPCNOPRIV,           /* R - authentication failure (not used) */
    EC_NOSERVER,            /* R - server not available */
    EC_SRVRNOPROC,          /* R - procedure not available */
    EC_RPCPMAPERROR,        /* R - RPC portmap failure */
    EC_UNKNOWNSRVR,         /* R - unknown remote server */
    EC_SRVRERROR,           /* R - unspecified server error */
    EC_RPCERROR             /* R - unspecified network error */
};

/*
** function-code mnemonics
*/
typedef enum
{
    F_RD1,        /* read group 1 register */
    F_RD2,        /* read group 2 register */
    F_RC1,        /* read and clear group 1 register */
    F_RCM,        /* read complement of group 1 register */
    F_TLM = 8,    /* test LAM */
    F_CL1,        /* clear group 1 register */
    F_CLM,        /* clear LAM */
    F_CL2,        /* clear group 2 register */
    F_WT1 = 16,   /* write group 1 register */
    F_WT2,        /* write group 2 register */
    F_SS1,        /* selective set group 1 register */
    F_SS2,        /* selective set group 2 register */
    F_SC1 = 21,   /* selective clear group 1 register */
    F_SC2 = 23,   /* selective clear group 2 register */
    F_DIS,        /* disable */
    F_XEQ,        /* execute */
    F_ENB,        /* enable */
    F_TST         /* test */
} fcode_t;

/*
** controller codes [RL, 14-Mar-2002, modified 10-Dec-2002]
*/
typedef enum
{
	C_CBV = 5,
	C_CC32 = 11
} ccode_t;

#define MAX_CRATES	16  /* 23/02/96 f.h. changed from 7 to 16 */
#define MAX_STATIONS	31
#define MAX_ADDRESSES	16
#define MAX_LAMS        24
#define MAX_BLOCK       0x80000000

#define Q_IGNORE	0
#define Q_STOP		1
#define Q_STOP_ON_WORD	2

/*
** control block for block transfers
*/
typedef struct ctrlblk
{
    unsigned int rpt_count,
		 tally,
		 lam_id,
		 mode;
} ctrlblk_t;

/* f.h. 15/01/96 change default camac-access from ioctl(system) to sym_ioctl */
#ifndef _NOSYM_IOCTL
#define ioctl sym_ioctl
#endif

#ifndef _NO_PROTOS_    
#if (defined (__STDC__) && __STDC__) || defined (__cplusplus)
#define	__P(args)	args
#else	/* Not ANSI C or C++.  */
#define	__P(args)	()	/* No prototypes.  */
#endif	/* ANSI C or C++.  */

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef	__cplusplus
#define	__BEGIN_DECLS	extern "C" {
#define	__END_DECLS	}
#else
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif

__BEGIN_DECLS
extern int ccopen __P ((char*, void*));
extern int cdreg __P ((void*, long, int, int, int));
extern int cfsa __P ((int, long, int*, int*));
extern int cssa __P ((int, long, int*, int*));
extern int ctstat __P ((int*));
extern char *cerror __P ((int, int));
extern int cccz __P ((long));
extern int cccc __P ((long));
extern int ccci __P ((long, int));
extern int ctci __P ((long, int*));
extern int cccd __P ((long, int));
extern int ctcd __P ((long, int*));
extern int ctgl __P ((long, int*));
extern int cgreg __P ((long, void*, int*, int*, int*));
extern int cfga __P ((int*, void*, int*, int*, void*));
extern int csga __P ((int*, void*, int*, int*, void*));
extern int cfmad __P ((int, void*, int*, void*));
extern int csmad __P ((int, void*, int*, void*));
extern int cfubc __P ((int, long, int*, void*));
extern int csubc __P ((int, long, int*, void*));
extern int cfubr __P ((int, long, int*, void*));
extern int csubr __P ((int, long, int*, void*));
/* function cdctrl: declare controller [RL, 14-Mar-2002] */
extern int cdctrl __P ((int));
__END_DECLS
#endif  /* _NO_PROTOS_ */

/*
  to dispose of memory associated with register and LAM identifiers
*/
#define cureg( ext)	free ((void*)(ext))
#define culam( lam)	free ((void*)(lam))

#endif /* CAMDEF_H */
