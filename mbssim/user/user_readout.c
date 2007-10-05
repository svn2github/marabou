/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           GamsReadout.c
// Purpose:        MBS readout function
//                 gams: GAMS experiment
//
// Description:    This file provides any functions needed for data readout
//                 in a specific experiment.
//
// Modules:        f_user_get_virt_ptr  -- pass hardware offsets to user
//                 f_user_init          -- initialize user readout
//                 f_user_readout       -- user-defined readout
//                 write_setup          -- write setup description
//
// Author:         gams
// Revision:         
// Date:           Mon Aug 13 12:29:13 2007
// Keywords:       
//
//EXP_SETUP
////////////////////////////////////////////////////////////////////////////*/

#include <sys/stat.h>
#include <time.h>
#include "s_evhe.h"
#include "s_veshe.h"
#ifdef MBS_SIMUL_RDO
#	include <iostream>
#	include <iomanip>
#	include "Rtypes.h"
#	include "MbsSimRdo.h"
#else
#	include <stdio.h>
#	include "gd_cnaf.h"
#	include "gd_camio.h"
#	include "gd_readout.h"
#	include "gd_tstamp.h"
#	include "ces/vmelib.h"
#	include "err_mask_def.h"
#	include "errnum_def.h"
#endif

#include "root_env.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Common indices used by readout as well as analysis
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "user_indices.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Definitions depending on user's setup
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "user_readout.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Static variables
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static long clock_resolution;		/* clock resolution as given by clock_getres system call */
static long trigger_count; 		/* count user triggers */
static struct pdparam_master s_param; 		/* vme segment params */

static long total_event_count; 	/* events since last start */
static long dt_event_count;		/* events since last dead-time stamp */
static long dt_scaler_contents;	/* contents of dead-time scaler */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Global variables
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef MBS_SIMUL_RDO
extern MbsSimRdo * gMbsSimRdo; 		/* data base for mbs i/o simulation */
#endif





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Global variables, once per module class
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Prototypes
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int uct_init(volatile unsigned long * uct, int xgate); 			/* initialize scaler UCT_8904 */
unsigned long uct_read(volatile unsigned long * uct, int clear);	/* read scaler contents */

/* access to ROOT-style environment files */
int root_env_read(char * file);
int root_env_getval_i(char * resource, int ivalue);
double root_env_getval_f(char * resource, double dvalue);
const char * root_env_getval_s(char * resource, char * svalue);
int root_env_getval_b(char * resource, int bvalue);

/* access files containing :-separated strings */
int unix_string_read(char * file);
TUnixEntry * unix_string_get_entry(int eno);
int unix_string_getval_i(TUnixEntry * e, char * name, int vnum, int ivalue);
double unix_string_getval_f(TUnixEntry * e, char * name, int vnum, double dvalue);
const char * unix_string_getval_s(TUnixEntry * e, char * name, int vnum, char *  svalue);
int unix_string_getval_b(TUnixEntry * e, const char * name, int vnum, int bvalue);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Define device pointers
/
/ Any pointers to camac or vme modules used in functions "f_user_init" and 
/ "f_user_readout" have to be of type
/                      >> static volatile unsigned long * << (CAMAC)
/                  or  >> static volatile char * <<          (VME)
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static volatile unsigned long * adc1; 			/* Camac: C1.N3: Silena 4418/V ADC 8 x 4096 */
static volatile unsigned long * adc2; 			/* Camac: C1.N5: Silena 4418/V ADC 8 x 4096 */
static volatile unsigned long * adc3; 			/* Camac: C1.N7: Silena 4418/V ADC 8 x 4096 */
static volatile unsigned long * adc4; 			/* Camac: C1.N10: Silena 4418/V ADC 8 x 4096 */
static volatile unsigned long * sca1; 			/* Camac: C1.N14: UCT Universal Timer/Counter N.Franz TUM */
static volatile unsigned long * sca2; 			/* Camac: C1.N15: UCT Universal Timer/Counter N.Franz TUM */
static volatile unsigned long * sca3; 			/* Camac: C1.N16: UCT Universal Timer/Counter N.Franz TUM */
static volatile unsigned long * cam_reset_C1; 		/* reset controller in crate C1 */
static volatile unsigned long * cam_dwinh_C1; 		/* dataway inhibit in crate C1 */
static volatile unsigned char * cam_auto_C1; 		/* auto read */
static volatile unsigned long * cam_rddw_C1; 		/* read double word */
static volatile unsigned long * cam_bcast_C1; 		/* broadcast */
static volatile unsigned long * cam_sreg_C1; 	/* X/Q enable in crate C1 */
static volatile unsigned long * cam_lam_C1;  	/* LAM pattern in crate C1 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ User tables:
/          moduleTable[]          -- module specs, indexed by module number
/          sevtTable[]            -- subevent specs, indexed by subevent number
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* module table, indexed by module number */
/* struct members: name, index, crate, station, lam, number of params, params used, param pattern, addr */
ModuleDef moduleTable[] = {
	{"adc1", kMrbModuleAdc1, 1, 3, 0x4, 8, 8, 0xff, NULL},
	{"adc2", kMrbModuleAdc2, 1, 5, 0x10, 8, 1, 0x1, NULL},
	{"adc3", kMrbModuleAdc3, 1, 7, 0x40, 8, 1, 0x1, NULL},
	{"adc4", kMrbModuleAdc4, 1, 10, 0x200, 8, 1, 0x1, NULL},
	{"sca1", kMrbModuleSca1, 1, 14, 0x2000, 1, 0, 0x0, NULL},
	{"sca2", kMrbModuleSca2, 1, 15, 0x4000, 1, 0, 0x0, NULL},
	{"sca3", kMrbModuleSca3, 1, 16, 0x8000, 1, 0, 0x0, NULL},
	NULL
};
/* subevent table, indexed by subevent number */
/* struct members: name, index, number of modules, index of first module, lam pattern */
SevtDef sevtTable[] = {
	{"det", kMrbSevtDet, 4, kMrbModuleAdc1, 0x254},
	NULL
};

int f_user_get_virt_ptr(long *pl_loc_hwacc,
                        long *pl_rem_cam[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           f_user_get_virt_ptr
// Purpose:        Pass hardware offsets to user
// Arguments:      long *pl_loc_hwacc   -- hardware offset to local camac crate
//                 long *pl_rem_cam[]   -- offset to remote crate(s),
//                                         indexed by crate no.
// Results:        TRUE(1)
// Exceptions:     
// Description:    This function assigns values to all pointers defined above.
//                 It should be called once on startup of event builder task.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	static long ptrs_passed = 0L;	/* once only code */
	if ( ptrs_passed ) return(1);
	ptrs_passed = 1L;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Initialize pointers to camac and/or vme modules
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 	adc1 = CIO_SET_BASE(B(0), C(1), N(3)); 			/* adc1: Silena 4418/V ADC 8 x 4096 */
 	moduleTable[kMrbModuleAdc1 - 1].addr = adc1;
 	adc2 = CIO_SET_BASE(B(0), C(1), N(5)); 			/* adc2: Silena 4418/V ADC 8 x 4096 */
 	moduleTable[kMrbModuleAdc2 - 1].addr = adc2;
 	adc3 = CIO_SET_BASE(B(0), C(1), N(7)); 			/* adc3: Silena 4418/V ADC 8 x 4096 */
 	moduleTable[kMrbModuleAdc3 - 1].addr = adc3;
 	adc4 = CIO_SET_BASE(B(0), C(1), N(10)); 			/* adc4: Silena 4418/V ADC 8 x 4096 */
 	moduleTable[kMrbModuleAdc4 - 1].addr = adc4;
 	sca1 = CIO_SET_BASE(B(0), C(1), N(14)); 			/* sca1: UCT Universal Timer/Counter N.Franz TUM */
 	moduleTable[kMrbModuleSca1 - 1].addr = sca1;
 	sca2 = CIO_SET_BASE(B(0), C(1), N(15)); 			/* sca2: UCT Universal Timer/Counter N.Franz TUM */
 	moduleTable[kMrbModuleSca2 - 1].addr = sca2;
 	sca3 = CIO_SET_BASE(B(0), C(1), N(16)); 			/* sca3: UCT Universal Timer/Counter N.Franz TUM */
 	moduleTable[kMrbModuleSca3 - 1].addr = sca3;
 	cam_reset_C1 = CIO_SET_BASE(B(0), C(1), N(31));	/* reset in crate C1 */
 	cam_dwinh_C1 = CIO_SET_BASE(B(0), C(1), N(27));	/* dataway inhibit in crate C1 */
 	cam_auto_C1 = (volatile unsigned char *) CIO_SET_BASE(B(0), C(1), N(0)); 	/* auto read in crate C1 */
 	cam_rddw_C1 = CIO_SET_BASE(B(0), C(1), N(27));	/* read double word in crate C1 */
 	cam_bcast_C1 = CIO_SET_BASE(B(0), C(1), N(25));	/* broadcast in crate C1 */
 	cam_lam_C1 = CIO_SET_BASE(B(0), C(1), N(28));		/* LAM pattern in crate C1 */

	return(1);
}	/* f_user_get_virt_ptr */

int f_user_init (unsigned char  bh_crate_nr,
                 long           *pl_loc_hwacc,
                 long           *pl_rem_cam[],
                 long           *pl_stat) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           f_user_init
// Purpose:        Initialize user readout
// Arguments:      unsigned char bh_crate_nr    -- crate number
//                 long *pl_loc_hwacc           -- offset (local crate)
//                 long *pl_rem_cam[]           -- offset (remote crates)
//                 long *pl_stat                -- status (unused)
// Results:        TRUE(1)
// Exceptions:     
// Description:    This function performs initialization of modules.
//                 It is called for every begin of run ( start of acquisition )
//                 and for every crate that is expected to be read out by
//                 event builder - as specified in the 'setup.usf' file.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct timespec res;
	char msg[256];								/* (error) message */
	int sil_4418vData;					/* silena 4418V:	data */


	clock_getres(CLOCK_REALTIME, &res); 	/* get resolution of the real-time clock */
	clock_resolution = res.tv_nsec; 		/* and save it for WRITE_TIME_STAMP macro */

	root_env_read("/o/exp/gams/Aug07-53Mn/.rootrc");			/* read ROOT's environment file */
 	root_env_read("/o/exp/gams/Aug07-53Mn/.GamsConfig.rc");				/* and experiment's configuration data */




 	switch (bh_crate_nr) {
 		case 1:						/* init modules in crate C1 (CAMAC) */
			CIO_WRITE_R2B(cam_reset_C1, F(16), A(0), 0L);	/* crate C1: reset */
			CC_AUTOREAD_OFF(cam_auto_C1);					/* crate C1: no auto read */
			CC_READ_DWORD_OFF(cam_rddw_C1);					/* crate C1: no double word read */
			CIO_WRITE_R2B(cam_dwinh_C1, F(16), A(0), 0L);	/* crate C1: dataway disable */
			CIO_WRITE_R2B(cam_lam_C1, F(16), A(0), 0L);		/* crate C1: clear lam ff */

									/* init module adc1 in C1.N3 */
					f_ut_send_msg("m_read_meb", "Initializing module adc1 in C1.N3: Silena 4418/V ADC 8 x 4096\n", ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL_R2B(adc1, F(9), A(0));	/* clear module */
					CIO_CTRL_R2B(adc1, F(2), A(7));	/* clear data */
					CIO_CTRL_R2B(adc1, F(10), A(0)); /* clear lam */
					CIO_WRITE_R2B(adc1, F(20), A(14), 0x201L);	/* set status */
					sil_4418vData = CIO_READ_R2B(adc1, F(4), A(14));	/* check status */
					sil_4418vData &= 0xFFFF;
					if (sil_4418vData != 0x201L) {
						sprintf(msg, "Module adc1 in C1.N3: Status error - %#lx (should be 0x201L)\n", sil_4418vData);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
					}
					CIO_WRITE_R2B(adc1, F(20), A(9), 40L); 	/* common thresh */

					CIO_WRITE_R2B(adc1, F(17), A(0 + 8), 0L);	/* lower limit et:A(0) */
					CIO_WRITE_R2B(adc1, F(17), A(0), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(0), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(1 + 8), 0L);	/* lower limit e1l:A(1) */
					CIO_WRITE_R2B(adc1, F(17), A(1), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(1), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(2 + 8), 0L);	/* lower limit e1r:A(2) */
					CIO_WRITE_R2B(adc1, F(17), A(2), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(2), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(3 + 8), 0L);	/* lower limit e2l:A(3) */
					CIO_WRITE_R2B(adc1, F(17), A(3), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(3), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(4 + 8), 0L);	/* lower limit e2r:A(4) */
					CIO_WRITE_R2B(adc1, F(17), A(4), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(4), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(5 + 8), 0L);	/* lower limit e3:A(5) */
					CIO_WRITE_R2B(adc1, F(17), A(5), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(5), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(6 + 8), 0L);	/* lower limit e4:A(6) */
					CIO_WRITE_R2B(adc1, F(17), A(6), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(6), 140L); 		/* offset */

					CIO_WRITE_R2B(adc1, F(17), A(7 + 8), 0L);	/* lower limit e5:A(7) */
					CIO_WRITE_R2B(adc1, F(17), A(7), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc1, F(20), A(7), 140L); 		/* offset */

									/* init module adc2 in C1.N5 */
					f_ut_send_msg("m_read_meb", "Initializing module adc2 in C1.N5: Silena 4418/V ADC 8 x 4096\n", ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL_R2B(adc2, F(9), A(0));	/* clear module */
					CIO_CTRL_R2B(adc2, F(2), A(7));	/* clear data */
					CIO_CTRL_R2B(adc2, F(10), A(0)); /* clear lam */
					CIO_WRITE_R2B(adc2, F(20), A(14), 0x202L);	/* set status */
					sil_4418vData = CIO_READ_R2B(adc2, F(4), A(14));	/* check status */
					sil_4418vData &= 0xFFFF;
					if (sil_4418vData != 0x202L) {
						sprintf(msg, "Module adc2 in C1.N5: Status error - %#lx (should be 0x202L)\n", sil_4418vData);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
					}
					CIO_WRITE_R2B(adc2, F(20), A(9), 40L); 	/* common thresh */

					CIO_WRITE_R2B(adc2, F(17), A(0 + 8), 0L);	/* lower limit dt:A(0) */
					CIO_WRITE_R2B(adc2, F(17), A(0), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc2, F(20), A(0), 140L); 		/* offset */

									/* init module adc3 in C1.N7 */
					f_ut_send_msg("m_read_meb", "Initializing module adc3 in C1.N7: Silena 4418/V ADC 8 x 4096\n", ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL_R2B(adc3, F(9), A(0));	/* clear module */
					CIO_CTRL_R2B(adc3, F(2), A(7));	/* clear data */
					CIO_CTRL_R2B(adc3, F(10), A(0)); /* clear lam */
					CIO_WRITE_R2B(adc3, F(20), A(14), 0x203L);	/* set status */
					sil_4418vData = CIO_READ_R2B(adc3, F(4), A(14));	/* check status */
					sil_4418vData &= 0xFFFF;
					if (sil_4418vData != 0x203L) {
						sprintf(msg, "Module adc3 in C1.N7: Status error - %#lx (should be 0x203L)\n", sil_4418vData);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
					}
					CIO_WRITE_R2B(adc3, F(20), A(9), 40L); 	/* common thresh */

					CIO_WRITE_R2B(adc3, F(17), A(0 + 8), 0L);	/* lower limit tof:A(0) */
					CIO_WRITE_R2B(adc3, F(17), A(0), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc3, F(20), A(0), 140L); 		/* offset */

									/* init module adc4 in C1.N10 */
					f_ut_send_msg("m_read_meb", "Initializing module adc4 in C1.N10: Silena 4418/V ADC 8 x 4096\n", ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL_R2B(adc4, F(9), A(0));	/* clear module */
					CIO_CTRL_R2B(adc4, F(2), A(7));	/* clear data */
					CIO_CTRL_R2B(adc4, F(10), A(0)); /* clear lam */
					CIO_WRITE_R2B(adc4, F(20), A(14), 0x204L);	/* set status */
					sil_4418vData = CIO_READ_R2B(adc4, F(4), A(14));	/* check status */
					sil_4418vData &= 0xFFFF;
					if (sil_4418vData != 0x204L) {
						sprintf(msg, "Module adc4 in C1.N10: Status error - %#lx (should be 0x204L)\n", sil_4418vData);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
					}
					CIO_WRITE_R2B(adc4, F(20), A(9), 40L); 	/* common thresh */

					CIO_WRITE_R2B(adc4, F(17), A(0 + 8), 0L);	/* lower limit pup:A(0) */
					CIO_WRITE_R2B(adc4, F(17), A(0), 255L);		/* upper limit */
					CIO_WRITE_R2B(adc4, F(20), A(0), 140L); 		/* offset */

					uct_init(sca1, 0);	/* init scaler sca1 in C1.N14 */
					f_ut_send_msg("m_read_meb", "Initializing scaler sca1 in C1.N14: UCT Universal Timer/Counter N.Franz TUM\n", ERR__MSG_INFO, MASK__PRTT);
					if (0) f_ut_send_msg("m_read_meb", "Scaler sca1 in C1.N14: external gate ENABLED\n", ERR__MSG_INFO, MASK__PRTT);

					uct_init(sca2, 0);	/* init scaler sca2 in C1.N15 */
					f_ut_send_msg("m_read_meb", "Initializing scaler sca2 in C1.N15: UCT Universal Timer/Counter N.Franz TUM\n", ERR__MSG_INFO, MASK__PRTT);
					if (0) f_ut_send_msg("m_read_meb", "Scaler sca2 in C1.N15: external gate ENABLED\n", ERR__MSG_INFO, MASK__PRTT);

					uct_init(sca3, 0);	/* init scaler sca3 in C1.N16 */
					f_ut_send_msg("m_read_meb", "Initializing scaler sca3 in C1.N16: UCT Universal Timer/Counter N.Franz TUM\n", ERR__MSG_INFO, MASK__PRTT);
					if (0) f_ut_send_msg("m_read_meb", "Scaler sca3 in C1.N16: external gate ENABLED\n", ERR__MSG_INFO, MASK__PRTT);

 			break;

  		default:
 			sprintf(msg, "user_init: Unexpected crate %d\n", bh_crate_nr);
 			f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
 			break;
 	}

	return(1);
}	/* f_user_init */


int f_user_readout (unsigned char  bh_trig_typ,
                    unsigned char  bh_crate_nr,
                    long           *pl_loc_hwacc,
                    long           *pl_rem_cam[],
                    long           *pl_dat,
                    s_veshe        *ps_veshe,
                    long           *l_se_read_len,
                    long           *l_read_stat) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           f_user_readout
// Purpose:        Read event data out of camac modules
// Arguments:      unsigned char bh_trig_typ  -- trigger type
//                 unsigned char bh_crate_nr  -- crate number
//                 long *pl_loc_hwacc         -- offset (local crate)
//                 long *pl_rem_cam[]         -- offset (remote crates)
//                 long *pl_dat               -- pointer to event buffer
//                 s_veshe *ps_veshe          -- pointer to subevent header
//                 long *l_se_read_len        -- actual length of subevent
//                 long *l_read_stat          -- status (unused)
// Results:        TRUE(1)
//                 l_se_read_len              -- actual subevent length
//                 l_read_stat                -- status (not yet implemented)
// Exceptions:     
// Description:    This is the user's readout routine.
//                 It is called for every event trigger and for every crate
//                 that is expected to be read out by event builder
//                 (as specified in 'setup.usf' file).
//
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	register int i;

	register unsigned long data;				/* temp data storage */
	int wc;										/* word count */
	char msg[256];								/* (error) message */

	int sil_4418vData;					/* silena 4418V:	data */
	int sil_4418vWC;						/*					wc */


/* define pointers for input and output (to be used in macros): */
/*		rdoinp	-- will be mapped to a device pointer */
/*		evt		-- points to event buffer starting at "pl_dat" */

	register unsigned long *rdoinp; 			/* pointer to camac base address */
	unsigned short *pmodh;						/* pointer to module header on output */

	union {										/* pointer to event buffer */
		unsigned long *out32;					/* (long), (short), or (char) */
		unsigned short *out16;
		unsigned char *out8;
	} evt;

	evt.out32 = ((unsigned long *) pl_dat);			/* init pointer to event buffer */
	*l_read_stat = 1L;								/* set initial readout status */

/* Switch according to trigger first */
/* then select crate number */

	switch (bh_trig_typ)
	{
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Perform device specific actions on trigger #14 (start acquisition)
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case 14:							/* start acquisition */
			switch (bh_crate_nr) {
				case 1:				/* start modules in crate C1 (CAMAC) */
					CIO_WRITE_R2B(cam_dwinh_C1, F(16), A(0), 0L);	/* crate C1: dataway disable */
 	 				WRITE_TIME_STAMP(); 	/* include time stamp for this trigger */




					uct_init(sca1, 0);	/* clear scaler sca1 */
					CIO_CTRL2_R2B(sca1, F(26), A(5)); /* start scaler sca1 */
					uct_init(sca2, 0);	/* clear scaler sca2 */
					CIO_CTRL2_R2B(sca2, F(26), A(5)); /* start scaler sca2 */
					uct_init(sca3, 0);	/* clear scaler sca3 */
					CIO_CTRL2_R2B(sca3, F(26), A(5)); /* start scaler sca3 */
					CIO_WRITE_R2B(cam_dwinh_C1, F(16), A(1), 0L);	/* crate C1: dataway enable */
					break;
 				 default:
 					sprintf(msg, "user_readout: Unexpected crate %d\n", bh_crate_nr);
 					f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
 					break;
 			}
 			total_event_count = 0;		/* reset event counters */
 			dt_event_count = 0;
			break;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Perform device specific actions on trigger #15 (stop acquisition)
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case 15:							/* stop acquisition */
			switch (bh_crate_nr) {
				case 1:				/* stop modules in crate C1 (CAMAC) */
					CIO_WRITE_R2B(cam_dwinh_C1, F(16), A(0), 0L);	/* crate C1: dataway disable */
 	 				WRITE_TIME_STAMP(); 	/* include time stamp for this trigger */
					CIO_CTRL_R2B(adc1, F(2), A(7));	/* module adc1: clear data */
					CIO_CTRL_R2B(adc2, F(2), A(7));	/* module adc2: clear data */
					CIO_CTRL_R2B(adc3, F(2), A(7));	/* module adc3: clear data */
					CIO_CTRL_R2B(adc4, F(2), A(7));	/* module adc4: clear data */
					CIO_CTRL2_R2B(sca1, F(24), A(5));	/* stop scaler sca1 */
					sprintf(msg, "Scaler sca1: contents = %ld\n", uct_read(sca1, 0));
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL2_R2B(sca2, F(24), A(5));	/* stop scaler sca2 */
					sprintf(msg, "Scaler sca2: contents = %ld\n", uct_read(sca2, 0));
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					CIO_CTRL2_R2B(sca3, F(24), A(5));	/* stop scaler sca3 */
					sprintf(msg, "Scaler sca3: contents = %ld\n", uct_read(sca3, 0));
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					break;
 				default:
 					sprintf(msg, "user_readout: Unexpected crate %d\n", bh_crate_nr);
 					f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
 					break;
 			}
 			dt_event_count = 0;			/* reset event count */
			sleep(1);					/* needed to keep m_transport happy (!?) */
										/* (otherwise soft trigger #15 won't be delivered to ROOT) */
			break;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Read event data according to trigger and crate numbers
/ (one "case" per trigger each containing a "switch" over crate number)
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

 		case kMrbTriggerDet:		/* on trigger Det (1) */
			switch (bh_crate_nr) {
				case 1:			/* include data from crate C1 (CAMAC) */
		 			total_event_count++;		/* count this event (once per trigger!) */
		 			dt_event_count++;			/* dead-time limit reached? */
					if (dt_event_count >= 1000) {	/* include time-stamp and dead-time data */
						dt_scaler_contents = uct_read(sca1, 1);	/* read and clear dead-time scaler */
						WRITE_DEAD_TIME();					/* write dead time subevent */
						dt_event_count = 0;				/* reset counters */
						dt_scaler_contents = 0;
					}
					else { WRITE_TIME_STAMP(); }		/* include time stamp */
					BEGIN_SUBEVENT(10, 11, 0, 1, kMrbSevtDet); /* subevent det (type=[10,11], crate C1, serial=1) */
					RDO_SETUP(adc1, F(2), A(0));	/* setup read module adc1, start with param et */
					RDO_C2MI_16_C();		/* read et:A(0) */
					RDO_C2MI_16_C();		/* read e1l:A(1) */
					RDO_C2MI_16_C();		/* read e1r:A(2) */
					RDO_C2MI_16_C();		/* read e2l:A(3) */
					RDO_C2MI_16_C();		/* read e2r:A(4) */
					RDO_C2MI_16_C();		/* read e3:A(5) */
					RDO_C2MI_16_C();		/* read e4:A(6) */
					RDO_C2MI_16_C();		/* read e5:A(7) */
					RDO_SETUP(adc2, F(2), A(0));	/* setup read module adc2, start with param dt */
					RDO_C2MI_16_C();		/* read dt:A(0) */
					RDO_SETUP(adc3, F(2), A(0));	/* setup read module adc3, start with param tof */
					RDO_C2MI_16_C();		/* read tof:A(0) */
					RDO_SETUP(adc4, F(2), A(0));	/* setup read module adc4, start with param pup */
					RDO_C2MI_16_C();		/* read pup:A(0) */
					CIO_CTRL_R2B(adc1, F(9), A(0));		/* clear module adc1 */
					CIO_CTRL_R2B(adc2, F(9), A(0));		/* clear module adc2 */
					CIO_CTRL_R2B(adc3, F(9), A(0));		/* clear module adc3 */
					CIO_CTRL_R2B(adc4, F(9), A(0));		/* clear module adc4 */
					RDO_2MEM_16(0xffff);	/* + 1 word to align to 32 bit boundary */
					END_SUBEVENT(); 	/* end of subevent $sevtName */
					break;
 			 	default:
 					sprintf(msg, "user_readout: Unexpected crate %d\n", bh_crate_nr);
 					f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
 					break;
 			}
 			break;


		default:
	  		sprintf(msg, "user_readout: Unexpected trigger %d\n", bh_trig_typ);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_ERROR, MASK__PRTT);
			break;
	}

/* Set actual length of data */

	*l_se_read_len = ((unsigned char *) evt.out32) - ((unsigned char *) pl_dat);
	return(1);
}	/* f_user_readout */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Utility functions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


int uct_init(volatile unsigned long * uct, int xgate) {	/* initialize uct scaler */
	CIO_CTRL_R2B(uct, F(9), A(15));						/* clear status */
	CIO_CTRL2_R2B(uct, F(xgate ? 26 : 24), A(11));		/* enable/disable external gate */
	CIO_CTRL_R2B(uct, F(2), A(2));						/* read & clear MSB */
	CIO_CTRL_R2B(uct, F(2), A(3));						/* ... LSB and clear scaler */
	return(1);
}

unsigned long uct_read(volatile unsigned long * uct, int clear) { /* read contents (and clear module) */
	unsigned long int lsb, msb, data;
	unsigned int fct;
	fct = (clear == 0) ? F(0) : F(2);	/* read and clear? */
	msb = CIO_READ_R2B(uct, fct, A(2));	/* read MSB */
	lsb = CIO_READ_R2B(uct, fct, A(3));	/* read LSB and clear scaler */
	data = (((msb & 0xffffL) << 16) & 0xffff0000L) | (lsb & 0xffffL);
	return(data);
}

