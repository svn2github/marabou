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
////////////////////////////////////////////////////////////////////////////*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "cc32_camio.h"
#include "vmelib.h"

#include "SetColor.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#define UXS_N_VALS	5
#define UXS_L_STR	128

#define FIPPI_REVD	3
#define FIPPI_REVE	4

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

#define DGF_L_CODE		50000
#define DGF_A_VC32_BASE 0x550000

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
int dgf_read_fgpa(char * file, char * type, unsigned short data[]);
int dgf_read_dsp(char * file, unsigned short data[]);
int dgf_download_fpga(int crate, unsigned int smask, char * type, unsigned short data[], int size);
int dgf_download_dsp(int crate, unsigned short data[], int size);
int dgf_upload_dsp(int crate, int station, unsigned short data[], int size);
int dgf_set_switchbus(int crate);
unsigned int dgf_get_revision(int crate, int rev);

/* camac controller */
void cc32_init(unsigned long * physAddrCrate1, int nofCrates);


int main(int argc, char * argv[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgfdown
// Purpose:        Download FGPA & DSP code to DGF-4C
// Arguments:      CfgFile     -- configuration file
// Results:        
// Exceptions:     
// Description:      
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	char cfgFile[256];
	char loadPath[256];
	char fileSpec[256];
	char loadOptions[10];
	unsigned short sys[DGF_L_CODE];
	unsigned short fippiRevD[DGF_L_CODE];
	unsigned short fippiRevE[DGF_L_CODE];
	unsigned short dsp[DGF_L_CODE];
	unsigned short data[DGF_L_CODE];
	int sysSize, fippiDsize, fippiEsize, dspSize;
	int nofCrates, crate;
	int n;
	unsigned int smaskD, smaskE;
	unsigned long * baseAddr;
	int errCnt;
	FILE *fsts;

	if (argc >= 2) {
		strcpy(cfgFile, argv[1]);
	} else {
		strcpy(cfgFile, ".DgfDownload.rc");
	}

	errCnt = 0;

 	n = root_env_read(cfgFile);			/* read ROOT's environment file */
	if (n <= 0) {
		printf("%sdgfdown: Cannot open file %s%s\n", setred, cfgFile, setblack);
		errCnt++;
		dgfdown_write_hs(errCnt);
		exit(1);
	}

	nofCrates = root_env_getval_i("NofCrates", 1);
	baseAddr = (unsigned long *) root_env_getval_i("BaseAddr", DGF_A_VC32_BASE);
 	cc32_init(baseAddr, nofCrates);				/* vme addr mapping  */

	strcpy(loadOptions, root_env_getval_s("DownloadOptions", "SFD"));
	for (i = 0; i < 10; i++) loadOptions[i] = toupper(loadOptions[i]);

	strcpy(loadPath, root_env_getval_s("LoadPath", ""));
	strcat(loadPath, "/");

	if (strchr(loadOptions, 'S') != NULL) {
		strcpy(fileSpec, loadPath);
		strcat(fileSpec, root_env_getval_s("SystemFPGACode", ""));
		sysSize = dgf_read_fpga(fileSpec, "System", sys);
		if (sysSize <= 0) {
			printf("%sdgfdown: Error reading file %s [System]%s\n", setred, fileSpec, setblack);
			errCnt++;
			dgfdown_write_hs(errCnt);
			exit(1);
		}
	}

	if (strchr(loadOptions, 'F') != NULL) {
		strcpy(fileSpec, loadPath);
		strcat(fileSpec, root_env_getval_s("FippiFPGACode.RevD", ""));
		fippiDsize = dgf_read_fpga(fileSpec, "Fippi(D)", fippiRevD);
		if (fippiDsize <= 0) {
			printf("%sdgfdown: Error reading file %s [Fippi(D)]%s\n", setred, fileSpec, setblack);
			errCnt++;
		}
	
		strcpy(fileSpec, loadPath);
		strcat(fileSpec, root_env_getval_s("FippiFPGACode.RevE", ""));
		fippiEsize = dgf_read_fpga(fileSpec, "Fippi(E)", fippiRevE);
		if (fippiEsize <= 0) {
			printf("%sdgfdown: Error reading file %s [Fippi(E)]%s\n", setred, fileSpec, setblack);
			errCnt++;
		}
	}

	if (strchr(loadOptions, 'D') != NULL) {
		strcpy(fileSpec, loadPath);
		strcat(fileSpec, root_env_getval_s("DspCode", ""));
		dspSize = dgf_read_dsp(fileSpec, dsp);
	}

	for (crate = 1; crate <= nofCrates; crate++) {
		printf("dgfdown: Starting download for crate %d ...\n", crate);

		if (strchr(loadOptions, 'S') != NULL) {
			if (dgf_download_fpga(crate, -1, "System", sys, sysSize) == 0) {
				printf("%sdgfdown: Skipping further download for crate %d ...%s\n", setred, crate, setblack);
				errCnt++;
				continue;
			}
		}
		if (strchr(loadOptions, 'F') != NULL) {
			smaskD = dgf_get_revision(crate, FIPPI_REVD);
			smaskE = dgf_get_revision(crate, FIPPI_REVE);
			if (smaskD) {
				if (fippiDsize <= 0) {
					printf("%sdgfdown: No data [Fippi(D)]%s\n", setred, setblack);
					errCnt++;
					dgfdown_write_hs(errCnt);
					exit(1);
				}
				if (dgf_download_fpga(crate, smaskD, "Fippi(D)", fippiRevD, fippiDsize) == 0) {
					printf("%sdgfdown: Skipping further download for crate %d ...%s\n", setred, crate, setblack);
					errCnt++;
					continue;
				}
			}
			if (smaskE) {
				if (fippiEsize <= 0) {
					printf("%sdgfdown: No data [Fippi(E)]%s\n", setred, setblack);
					errCnt++;
					dgfdown_write_hs(errCnt);
					exit(1);
				}
				if (dgf_download_fpga(crate, smaskE, "Fippi(E)", fippiRevE, fippiEsize) == 0) {
					printf("%sdgfdown: Skipping further download for crate %d ...%s\n", setred, crate, setblack);
					errCnt++;
					continue;
				}
			}
		}
		if (strchr(loadOptions, 'D') != NULL) {
			dgf_set_switchbus(crate);
			if (dgf_download_dsp(crate, dsp, dspSize) == 0) {
					printf("%sdgfdown: Skipping further download for crate %d ...%s\n", setred, crate, setblack);
					errCnt++;
					continue;
			}
		}
	}

	dgfdown_write_hs(errCnt);
	if (errCnt == 0) {
		printf("\n%sdgfdown: Download ok - no errors%s\n\n", setblue, setblack);
		exit(0);
	} else {
		printf("\n%sdgfdown: Download finished - %d error(s)%s\n\n", setred, errCnt, setblack);
		exit(1);
	}
}

int dgfdown_write_hs(int errCnt) {
	FILE * fsts;
	fsts = fopen(".dgfdown.ok", "w");
	if (fsts) {
		fprintf(fsts, "Download.Errors:	%d\n", errCnt);
		close(fsts);
	}
	return(errCnt);
}
