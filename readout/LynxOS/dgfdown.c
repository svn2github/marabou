/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgfdown.c
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
#include "cc32_camio.h"
#include "vmelib.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#define UXS_N_VALS	5
#define UXS_L_STR	128

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

#define DGF_L_CODE		50000

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
int dgf_upload_dsp(int crate, int station, unsigned short data[], int size);
int dgf_set_switchbus(int crate);

/* camac controller */
void cc32_init(unsigned long * physAddrCrate1, int nofCrates);


int main() {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgfdown
// Purpose:        Download FGPA & DSP code to DGF-4C
// Arguments:      
// Results:        
// Exceptions:     
// Description:      
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	char loadPath[256];
	char fileSpec[256];
	unsigned short sys[DGF_L_CODE];
	unsigned short fippi[DGF_L_CODE];
	unsigned short dsp[DGF_L_CODE];
	unsigned short data[DGF_L_CODE];
	int sysSize, fippiSize, dspSize;
	int nofCrates, crate;
	char * diff;
	int nofDiffs;

 	cc32_init((unsigned long *) 0xee550000L, 3);				/* vme addr mapping for crates 1 ... 3 */

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
		dgf_download_fpga(crate, 0, sys, sysSize);
		dgf_download_fpga(crate, 1, fippi, fippiSize);
		dgf_set_switchbus(crate);
		dgf_download_dsp(crate, dsp, dspSize);
	}
/*	dgf_upload_dsp(2, 10, data, dspSize);
	nofDiffs = 0;
	for (i = 0; i < dspSize; i++) {
		if (dsp[i] != data[i]) {
			nofDiffs++;
			diff = " <====";
		} else {
			diff = "";
		}
		printf("%4d: file=%#x <> dsp=%#x %s\n", i, dsp[i], data[i], diff);
	}
	printf("Diffs: %d\n", nofDiffs); */
	exit(0);
}
