/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_download.c
// Purpose:        Download FPGA and DSP code to XIA DGF-4C modules
// Description:    Reads code files and downloads code to XIA modules.
//                 May use broadcasting if available.
// Author:         R. Lutter
// Revision:       
// Date:           Jun 2002
// Keywords:       
//
//EXP_SETUP
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "s_evhe.h"
#include "s_veshe.h"
#include "gd_cnaf.h"
#include "gd_camio.h"
#include "gd_readout.h"
#include "gd_tstamp.h"
#include "vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Useful macro defs
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define CC_AUTOREAD_ON(cc)	CIO_WRITE_R2B(cc, A(0), F(3), CIO_READ_R2B(cc, A(0), F(3)) | (0x1 << 12))
#define CC_AUTOREAD_OFF(cc)	CIO_WRITE_R2B(cc, A(0), F(3), CIO_READ_R2B(cc, A(0), F(3)) & ~(0x1 << 12))

#define CC_READ_DWORD_ON(cc)	CIO_WRITE_R2B(cc, A(2), F(16), 0)
#define CC_READ_DWORD_OFF(cc)	CIO_WRITE_R2B(cc, A(3), F(16), 0)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device-dependent definitions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* DGF-4C inline functions */
#define DGF_WRITE_ICSR(dgf, data)	CIO_WRITE_R2B(dgf, A(8), F(17), data)
#define DGF_READ_ICSR(dgf)		CIO_READ_R2B_16(dgf, A(8), F(1))
#define DGF_WRITE_CSR(dgf, data)	CIO_WRITE_R2B(dgf, A(0), F(17), data)
#define DGF_READ_CSR(dgf)		CIO_READ_R2B_16(dgf, A(0), F(1))
#define DGF_WRITE_TSAR(dgf, addr)	CIO_WRITE_R2B(dgf, A(1), F(17), addr)
#define DGF_READ_TSAR(dgf) 		CIO_READ_R2B_16(dgf, A(1), F(1))
#define DGF_WRITE_WCR(dgf, data)	CIO_WRITE_R2B(dgf, A(2), F(17), data)
#define DGF_READ_WCR(dgf)		CIO_READ_R2B_16(dgf, A(2), F(1))
#define DGF_WRITE_PSA(dgf, offset)	CIO_WRITE_R2B(dgf, A(1), F(17), DGF_INPAR_START_ADDR + offset)
#define DGF_READ_DSP(dgf)		CIO_READ_R2B_16(dgf, A(0), F(0))
#define DGF_WRITE_DSP(dgf, data)	CIO_WRITE_R2B(dgf, A(0), F(16), data)

#define DGF_L_CODE		20000

/* entry format to be used by unix-string functions */
#define UXS_N_VALS	5
#define UXS_L_STR	128

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Static variables
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Prototypes
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* access to ROOT-style environment files */
int root_env_read(char * file);
int root_env_getval_i(char * resource, int ivalue);
int root_env_getval_x(char * resource, int ivalue);
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

/* DGF-4C functions */
int dgf_read_fgpa(char * file, unsigned short data[]);
int dgf_read_dsp(char * file, unsigned short data[]);
int dgf_download_fpga(int crate, int sorf, unsigned short data[], int size);
int dgf_download_dsp(int crate, unsigned short data[], int size);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Define device pointers
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern volatile unsigned long * bcw;
extern volatile unsigned long * bc;

int f_user_get_virt_ptr(long *pl_loc_hwacc,
                        long *pl_rem_cam[]) { return(1); }	/* not used */

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
// Description:    Will be called on startup. Performs downloading of code.    
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	char loadPath[256];
	char fileSpec[256];
	unsigned short sys[DGF_L_CODE];
	unsigned short fippi[DGF_L_CODE];
	unsigned short dsp[DGF_L_CODE];
	int sysSize, fippiSize, dspSize;
	int nofCrates, crate;

	if (bh_crate_nr != 1) return(1);		/* should be called once only */

 	root_env_read(".DgfDownload.rc");		/* read ROOT's environment file */

	strcpy(loadPath, root_env_getval_s("LoadPath", ""));
	strcat(loadPath, "/");

	strcpy(fileSpec, loadPath);
	strcat(fileSpec, root_env_getval_s("SystemFPGACode", ""));
	sysSize = dgf_read_fpga(fileSpec, sys);

	strcpy(fileSpec, loadPath);
	strcat(fileSpec, root_env_getval_s("FippiFPGACode", ""));
	fippiSize = dgf_read_fpga(fileSpec, fippi);

	strcpy(fileSpec, loadPath);
	strcat(fileSpec, root_env_getval_s("DspCode", ""));
	dspSize = dgf_read_dsp(fileSpec, dsp);

	nofCrates = root_env_getval_i("NofCrates", 1);

	for (crate = 1; crate <= nofCrates; crate++) {
		bcw = CIO_SET_BASE(B(0), crate, N(26));
		bc = CIO_SET_BASE(B(0), crate, N(25));
		dgf_download_fpga(crate, 0, sys, sysSize);
		dgf_download_fpga(crate, 1, fippi, fippiSize);
		dgf_download_dsp(crate, dsp, dspSize);
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
                    long           *l_read_stat) { return(1); } 	/* not used */
