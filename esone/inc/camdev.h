/*
 * |------------------------------------------------------|
 * | Copyright (c) 1991 , 1992 Ibex Computing SA          |
 * | All Rights Reserved                                  |
 * |------------------------------------------------------|
*/

/*
 * Maintainance : frank humbert (GSI)
 * Description  : definition of transfer struct's
 * last update  :  
 * file         : camdev.h
*/

#ifndef CAMDEV_H
#define CAMDEV_H

#define MAXGL           24

/*
  ioctl codes
*/
enum
{
    CIOINIT = 0x101,
    CIOACT,
    CIOBLK,
    CIOADDSCAN,
    CIOONL,
    CIOMACT,
    CIOLAMWAIT,
    CIOLAMKILL,
    CIOLAMLINK,
    CIOLAMUNLINK,
    CIOLAMRELINK,
    CIOGETVCCDESC = 0x201	/* Return descriptor for debugging tools */
};

/*
  CAMAC Init
*/
typedef void*   cioinit_t;

/*
  CAMAC Action
*/
enum accessflag
{
    CAMAC_CYCLE,	/* access CAMAC, than internal */
    INTERNAL_CYCLE,	/* access internal twice */
    SPECIAL_CYCLE	/* access CAMAC twice (special for GL read) */
};

typedef struct
{
    int status;
    char cnaf [4];
    int *data;
} cioact_t;

/*
  CAMAC Block Transfer
*/
typedef struct
{
    int status;
    char cnaf [4];
    int *data;
    int *cb;
} cioblk_t;

#define _crate         cnaf [3]
#define _station       cnaf [2]
#define _address       cnaf [1]
#define _lamspec       cnaf [1]
#define _function      cnaf [0]
#define _lam           cnaf [0]
#define _rpt_count     cb [0]
#define _tally         cb [1]
#define _lam_id        cb [2]
#define _mode          cb [3]

/*
  CAMAC Address Scan
*/
typedef struct
{
    int status;
    char cnaf_0 [4];
    char cnaf_1 [4];
    int *data;
    int *cb;
} cioaddscan_t;        

/*
  CAMAC Address Scan
*/
typedef struct
{
    char cnaf[4];
} cnaf_tmp;

typedef struct
{
    int status;
    cnaf_tmp *cnaf;
    int *func;
    int *data;
    int *cb;
    int *q;
} ciomact_t;

#define _crate_0         cnaf_0 [3]
#define _station_0       cnaf_0 [2]
#define _address_0       cnaf_0 [1]
#define _lamspec_0       cnaf_0 [1]
#define _function_0      cnaf_0 [0]
#define _lam_0           cnaf_0 [0]

#define _crate_1         cnaf_1 [3]
#define _station_1       cnaf_1 [2]
#define _address_1       cnaf_1 [1]
#define _lamspec_1       cnaf_1 [1]
#define _function_1      cnaf_1 [0]
#define _lam_1           cnaf_1 [0]

/*
  CAMAC Test branch online
*/
typedef struct
{
    int crate;
    int online;
} cioonl_t;

/*
** LAM subroutines
*/
/*
  LAM wait ioctl argument
*/
typedef struct lam_s
{
    int status;
    char cnaf [4];
} ciolam_t;
 
#define FLG_KILL        1 
 
/* GL kill ioctl argument */
typedef struct gl_s
{
    unsigned int gl;
} ciogl_t;

#endif /* CAMDEV_H */
