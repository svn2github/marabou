/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_download.c
// Purpose:        Download FPGA and DSP code to XIA DGF-4C modules
// Description:    Reads code files and downloads code to XIA modules.
//                 May use broadcasting if available.
//                 ===> To be used in CC32 standalone mode only (no MBS) <===
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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Useful macro defs
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define CC32_AUTOREAD_ON(ccar)		CC32_WRITE_R2B(ccar, A(0), F(3), CC32_READ_R2B(ccar, A(0), F(3)) | (0x1 << 12))
#define CC32_AUTOREAD_OFF(ccar)		CC32_WRITE_R2B(ccar, A(0), F(3), CC32_READ_R2B(ccar, A(0), F(3)) & ~(0x1 << 12))

#define CC32_READ_DWORD_ON(ccrdw)	CC32_WRITE_R2B(ccrdw, A(2), F(16), 0)
#define CC32_READ_DWORD_OFF(ccrdw)	CC32_WRITE_R2B(ccrdw, A(3), F(16), 0)

#define CC32_READ_STATUS(ccsts)		CC32_READ_R2B(ccsts, A(0), F(0))
#define CC32_READ_I(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x1)
#define CC32_READ_L(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x2)
#define CC32_READ_X(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x4)
#define CC32_READ_Q(ccsts)			(CC32_READ_R2B(ccsts, A(0), F(0)) & 0x8)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device-dependent definitions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* DGF-4C inline functions */
#define DGF_WRITE_ICSR(dgf, data)	CC32_WRITE_R2B(dgf, A(8), F(17), data)
#define DGF_READ_ICSR(dgf)		CC32_READ_R2B_16(dgf, A(8), F(1))
#define DGF_WRITE_CSR(dgf, data)	CC32_WRITE_R2B(dgf, A(0), F(17), data)
#define DGF_READ_CSR(dgf)		CC32_READ_R2B_16(dgf, A(0), F(1))
#define DGF_WRITE_TSAR(dgf, addr)	CC32_WRITE_R2B(dgf, A(1), F(17), addr)
#define DGF_READ_TSAR(dgf) 		CC32_READ_R2B_16(dgf, A(1), F(1))
#define DGF_WRITE_WCR(dgf, data)	CC32_WRITE_R2B(dgf, A(2), F(17), data)
#define DGF_READ_WCR(dgf)		CC32_READ_R2B_16(dgf, A(2), F(1))
#define DGF_WRITE_PSA(dgf, offset)	CC32_WRITE_R2B(dgf, A(1), F(17), DGF_INPAR_START_ADDR + offset)
#define DGF_READ_DSP(dgf)		CC32_READ_R2B_16(dgf, A(0), F(0))
#define DGF_WRITE_DSP(dgf, data)	CC32_WRITE_R2B(dgf, A(0), F(16), data)

#define DGF_L_CODE		50000

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
/ Global pointers
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern volatile unsigned long * cc32_baseAddr;

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


int dgf_read_fpga(char * fileSpec, unsigned short data[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_read_fpga
// Purpose:        load fpga code
// Arguments:      char * fileSpec      -- code file
//                 unsigned short data  -- array to hold data
// Results:        int size             -- number of 16 bit words read
// Exceptions:     
// Description:    Reads binary FPGA code from file. 
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	int fd;
	int nbytes;
	char tmp[DGF_L_CODE * sizeof(unsigned short)];

	memset(tmp, 0, DGF_L_CODE * sizeof(unsigned short));
	fd = open(fileSpec, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "dgf_read_fpga: Cannot open file %s\n", fileSpec);
		return(-1);
	}
	nbytes = read(fd, tmp, DGF_L_CODE * sizeof(unsigned short));
	close(fd);
	if (nbytes > 0) {
		for (i = 0; i < nbytes; i++) data[i] = (unsigned short) tmp[i];
		printf("dgf_read_fpga: Read %d bytes from file %s\n", nbytes, fileSpec);
	} else {
		printf("dgf_read_fpga: file %s is empty\n", fileSpec);
	}
	return(nbytes);
}

int dgf_read_dsp(char * fileSpec, unsigned short data[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_read_dsp
// Purpose:        load dsp code
// Arguments:      char * fileSpec      -- code file
//                 unsigned short data  -- array to hold data
// Results:        int size             -- number of 16 bit words read
// Exceptions:     
// Description:    Reads binary DSP code from file. 
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	int fd;
	int nbytes, size;
	int byte0, byte1, byte2, byte3;
	unsigned short tmp[DGF_L_CODE];

	memset(tmp, 0, DGF_L_CODE * sizeof(unsigned short));
	fd = open(fileSpec, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "dgf_read_dsp: Cannot open file %s\n", fileSpec);
		return(-1);
	}
	nbytes = read(fd, tmp, DGF_L_CODE * sizeof(unsigned short));
	close(fd);

	size = nbytes / 2;
	if (size % 4) {
		fprintf(stderr, "dgf_read_dsp: File %s has ODD size - %d\n", fileSpec, size);
		return(-1);
	}
	printf("dgf_read_dsp: Read %d bytes from file %s\n", nbytes, fileSpec);

	if (size > 0) {
		for (i = 0; i < size; i += 2) {
			byte0 = tmp[i] & 0x00ff;
			byte1 = (tmp[i] >> 8) & 0x00ff;
			byte2 = tmp[i + 1] & 0x00ff;
			byte3 = (tmp[i + 1] >> 8) & 0x00ff;
			if (byte2 != 0) {
				fprintf(stderr, "dgf_read_dsp: Data inconsistent - byte#3 != 0\n");
			}
			data[i] = (byte3 << 8) + byte0;
			data[i + 1] = byte1;
		}
	} else {
		printf("dgf_read_dsp: file %s is empty\n", fileSpec);
	}
	return(size);
}

int dgf_download_fpga(int crate, int sorf, unsigned short data[], int size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_download_fpga
// Purpose:        download fpga code to DGF-4C
// Arguments:      int crate            -- crate number
//                 int sorf             -- system (0) or fippi (1) fpga
//                 unsigned short data  -- fpga code to be downloaded
//                 int size             -- number of words
// Results:        0/1
// Exceptions:     
// Description:    Downloads FPGA code.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	char resource[256];

	volatile unsigned long * bcm;		/* broadcast mask register */
	volatile unsigned long * bcw;		/* exec broadcast command (camac write only) */
	volatile unsigned long * m;
	unsigned short csr;
	unsigned long mbits;

	int station;
	int subAddr;
	unsigned int stationMask;
	unsigned int sts;
	int ok;
	char * sorfStr;

	bcm = CC32_SET_BASE(C(crate), N(26));
	bcw = CC32_SET_BASE(C(crate), N(25));

	if (sorf == 0) {										/* system of fippi fpga? */
		csr = 0x01;
		subAddr = A(10);
		sorfStr = "system";
	} else {
		csr = 0xF0;
		subAddr = A(9);
		sorfStr = "fippi";
	}

	sprintf(resource, "StationMask.Crate%d", crate); 		/* read station mask from environment */
	stationMask = root_env_getval_x(resource, 0);
	if (stationMask == 0) {
		fprintf(stderr, "dgf_download_fpga: (%s) No station bits given for crate C%d\n", sorfStr, crate);
		return(0);
	}


	CC32_WRITE_R2B(bcm, F(16), A(0), stationMask); 							/* set broadcast mask */

	DGF_WRITE_ICSR(bcw, csr);												/* broadcast to ICSR */
	usleep(50000);															/* wait for bcw complete */
	for (i = 0; i < size; i++) CC32_WRITE_R2B(bcw, F(17), subAddr, data[i]); /* download fpga data (system or fippi) */
	usleep(50000);															/* wait again */

	ok = 1;
	station = 1;										/* check fgpa status */
	mbits = stationMask;
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			sts = DGF_READ_ICSR(m) & csr;				/* read ICSR */
			if (sts != 0) { 							/* fpga bits should now be '0' */
				fprintf(stderr, "dgf_download_fpga: FGPA download (%s) failed for dgf in C%d.N%d - bits = %#x\n",
																				sorfStr, crate, station, sts);
				ok = 0;
			}
		}
		station++;
		mbits >>= 1;
	}
	if (ok == 0) {
		fprintf(stderr, "dgf_download_fpga: Error during FPGA download (%s)\n", sorfStr);
	} else {
		fprintf(stdout, "dgf_download_fpga: FPGA Download (%s) ok - %d words to crate C%d, broadcast mask = %#lx\n",
												sorfStr, size, crate, stationMask);
	}
	return(ok);
}

int dgf_download_dsp(int crate, unsigned short data[], int size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_download_dsp
// Purpose:        download dsp code to DGF-4C
// Arguments:      int crate            -- crate number
//                 unsigned short data  -- fpga code to be downloaded
//                 int size             -- number of words
// Results:        int size             -- words actually written
// Exceptions:     
// Description:    Downloads DSP code.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	char resource[256];
	unsigned short tmp[DGF_L_CODE];
	volatile unsigned long * bcm;		/* broadcast mask register */
	volatile unsigned long * bcw;		/* exec broadcast command (camac write only) */
	volatile unsigned long * m;

	unsigned short csr;
	unsigned long mbits, stationMask;

	int station;
	unsigned int sts;
	int ok;
	unsigned short dspData;

	bcm = CC32_SET_BASE(C(crate), N(26));
	bcw = CC32_SET_BASE(C(crate), N(25));

	sprintf(resource, "StationMask.Crate%d", crate); 							/* get station mask from environment */
	stationMask = root_env_getval_x(resource, 0);
	if (stationMask == 0) {
		fprintf(stderr, "dgf_download_dsp: No station bits given for crate C%d\n", crate);
		return(0);
	}

	CC32_WRITE_R2B(bcm, F(16), A(0), stationMask); 								/* set broadcast mask */

	DGF_WRITE_CSR(bcw, 0x10);													/* reset dsp */
	usleep(50000);																	/* wait for bc to complete */
	DGF_WRITE_TSAR(bcw, 1);														/* start download at &data[2] */
	for (i = 2; i < size; i++) CC32_WRITE_R2B(bcw, F(16), A(0), data[i]);		/* download dsp data */
	DGF_WRITE_TSAR(bcw, 0);								/* write data[0]&[1] (will start dsp) */
	CC32_WRITE_R2B(bcw, F(16), A(0), data[0]);
	CC32_WRITE_R2B(bcw, F(16), A(0), data[1]);
	usleep(50000);															/* wait for download to complete */

	ok = 1;
	station = 1;										/* check dsp status */
	mbits = stationMask;
	DGF_WRITE_TSAR(bcw, 0); 							/* set dsp start addr for all modules *=at once */
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			sts = DGF_READ_CSR(m) & csr;				/* read CSR */
			if (sts & 0x1000) { 						/* check dsp error bit */
				fprintf(stderr, "dgf_download_dsp: DSP download failed for dgf in C%d.N%d\n", crate, station);
				ok = 0;
			} else {
				for (i = 0; i < size; i++) {
					dspData = DGF_READ_DSP(m);			/* compare dsp data with file contents */
					if (dspData != data[i]) {
						fprintf(stderr, "dgf_download_dsp: DSP data check failed for dgf in C%d.N%d at addr %#x - data = %#x, should be %#x\n",
												crate, station, i, dspData, data[i]);
						ok = 0;
						if (i > 10) break;
					}
				}			
			}
		}
		station++;
		mbits >>= 1;
	}

	if (ok == 0) {
		fprintf(stderr, "dgf_download_dsp: Error during DSP download\n");
	} else {
		fprintf(stdout, "dgf_download_dsp: DSP download ok - %d words to crate C%d, broadcast mask = %#lx\n",
																size, crate, stationMask);
	}
	return(ok);
}

int dgf_set_switchbus(int crate) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_set_switchbus
// Purpose:        Set switchbus register
// Arguments:      int crate            -- crate number
// Results:        0/1
// Exceptions:     
// Description:    Setse terminationbits in ICSR.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	char resource[256];
	int left;

	unsigned short icsr;
	unsigned long mbits;
	volatile unsigned long * m;

	int station;
	unsigned int sts;

	sprintf(resource, "StationMask.Crate%d", crate); 							/* get station mask from environment */
	mbits = root_env_getval_x(resource, 0);
	if (mbits == 0) return(0);

	station = 1;										/* check dsp status */
	left = 1;
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			if (left == 1) {
				icsr = 0;
				left = 0;
			} else {
				icsr = 0x2400;
				left = 1;
			}
			DGF_WRITE_ICSR(m, icsr);
		}
		station++;
		mbits >>= 1;
	}
	return(1);
}

int dgf_upload_dsp(int crate, int station, unsigned short data[], int size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_upload_dsp
// Purpose:        download dsp code to DGF-4C
// Arguments:      int crate            -- crate number
//                 unsigned short data  -- fpga code to be downloaded
//                 int size             -- number of words
// Results:        int size             -- words actually written
// Exceptions:     
// Description:    Downloads DSP code.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i;
	volatile unsigned long * m;

	m = CC32_SET_BASE(C(crate), N(station));
	DGF_WRITE_TSAR(m, 0);
	for (i = 0; i < size; i++) data[i] = DGF_READ_DSP(m);
	fprintf(stdout, "dgf_upload_dsp: Uploaded %d words from module in crate C%d.N%d\n", size, crate, station);
	return(size);
}
