/*
 * |------------------------------------------------------|
 * | Copyright (c) 1991 , 1991 Ibex Computing SA          |
 * | All Rights Reserved                                  |
 * |------------------------------------------------------|
*/

/*
 * Maintainance : frank humbert (GSI)
 * Description  : CAMAC crate controller definitions
 * last update  : 15-Dec-1995 added new test functions
 * file         : camlib.h
 *
 * updates      : Mar 2002 [RL]
 *                add support for different controllers
 *                (CBV, CC32)
 */

/*
** CAMAC crate controller commands
*/

/* generate dataway initialize */
/* CBV: N28.A8.F26 */
#define N_Z_CBV 		28
#define A_Z_CBV 		8
#define F_Z_CBV 		F_ENB
/* CC32: N0.A1.F16 */
#define N_Z_CC32		0
#define A_Z_CC32		1
#define F_Z_CC32		F_WT1

/* generate dataway clear */
/* CBV: N28.A9.F26 */
#define N_C_CBV 		28
#define A_C_CBV 		8
#define F_C_CBV 		F_ENB
/* CC32: N0.A0.F16 */
#define N_C_CC32		0
#define A_C_CC32		0
#define F_C_CC32		F_WT1

/* read graded LAM */
#define N_GL			30
#define A_GL			0
#define F_GL			F_RD1

/* load SNR */
#define N_SNR			30
#define A_SNR   		8
#define F_SNR   		F_WT1

/* dataway inhibit */
/* CBV: N30.A9.F26/F24 */
#define N_I_CBV 		30
#define A_I_ENB_CBV 	9
#define A_I_DIS_CBV 	9
#define F_I_ENB_CBV 	F_ENB
#define F_I_DIS_CBV 	F_DIS
#define F_I_TST_CBV 	F_TST
/* CC32: N27.A0/A1.F16 */
#define N_I_CC32    	27
#define A_I_ENB_CC32	0
#define A_I_DIS_CC32	1
#define F_I_ENB_CC32	F_WT1
#define F_I_DIS_CC32	F_WT1
#define F_I_TST_CC32	F_RD1

/* branch demand output */
#define N_D     30
#define A_D     10
/*      F_D     F_DIS/F_ENB/F_TST */

/* test demands present */
#define N_DP    30
#define A_DP    11
#define F_DP    F_TST

/*** patch 8/11/93 1c -> 18 ***/
#define fcode_is_read(f)        (((unsigned int)(f) & 0x18) == 0)
#define fcode_is_write(f)       (((unsigned int)(f) & 0x18) == 0x10)
#define fcode_is_rstatus(f)     ((f == 8) || (f == 27))
/*** end patch ***/
#define IS_F_READ(s)    (((unsigned int)(s) & 0x18) == 0)
#define IS_F_WRITE(s)   (((unsigned int)(s) & 0x18) == 0x10)
#define IS_F_CNTL(s)    ((!IS_F_READ((s))) & (!IS_F_WRITE((s))))
#define IS_F_VALID(s)    (((unsigned int)(s) & 0xe0) == 0)

/*
  definition of a camac address
*/
/*
  CAUTION: the records of camacadd_lcl_t in camacrpc.h
  must match the first records of camacadd_t
*/
typedef struct camacadd
{
    int    n_path;
    char   n_cnaf [4];
    void   *n_client;
    struct camacadd *n_branch;
    int    n_valid;
} camacadd_t;

typedef struct camacadd_tmp
{
    int n_path;
    char n_cnaf [4];
} camacadd_tmp_t;

typedef struct camacent
{
    char *c_name;
    char *c_host;
    char *c_serv;
    char *c_dev;
    int  c_unit;
    char *c_prot;
} camacent_t;

#define n_crate         n_cnaf [3]
#define n_station       n_cnaf [2]
#define n_address       n_cnaf [1]
#define n_lamspec       n_cnaf [1]
#define n_function      n_cnaf [0]
#define n_lam           n_cnaf [0]

#define VALID_DEV       0xDEADCAFE

/*
  status register
*/
#define NOT_Q   (1<<0)
#define NOT_X   (1<<1)

/*
  Block transfer modes (coded in the control block)
*/
#ifndef Q_STOP
#include "camdef.h"
#endif
enum
{
    MODE_QIGNORE = Q_IGNORE,
    MODE_QSTOP = Q_STOP,
    MODE_QSTOPW = Q_STOP_ON_WORD,
    MODE_QREPEAT
};
