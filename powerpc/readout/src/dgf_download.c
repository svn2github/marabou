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

#include "SetColor.h"

#include "unix_string.h"
#include "root_env.h"

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
#define DGF_READ_REVISION(dgf)	(CC32_READ_R2B_16(dgf, A(13), F(1)) & 0xF)

#define DGF_L_CODE		50000

#define FIPPI_REVD	3
#define FIPPI_REVE	4

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Global pointers
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
extern volatile unsigned long * cc32_baseAddr;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Prototypes
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* DGF-4C functions */
int dgf_read_fgpa(char * file, char * type, unsigned short data[]);
int dgf_read_dsp(char * file, unsigned short data[]);
int dgf_download_fpga(int crate, unsigned int smask, char * type, unsigned short data[], int size);
int dgf_download_dsp(int crate, unsigned short data[], int size);
int dgf_upload_dsp(int crate, int station, unsigned short data[], int size);
int dgf_set_switchbus(int crate);
unsigned int dgf_get_revision(int crate, int rev);

int dgf_read_fpga(char * fileSpec, char * type, unsigned short data[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_read_fpga
// Purpose:        load fpga code
// Arguments:      char * fileSpec      -- code file
//                 char * type          -- sys or fippi D/E
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
		fprintf(stderr, "%sdgf_read_fpga: Cannot open file %s%s\n", setred, fileSpec, setblack);
		return(-1);
	}
	nbytes = read(fd, tmp, DGF_L_CODE * sizeof(unsigned short));
	close(fd);
	if (nbytes > 0) {
		for (i = 0; i < nbytes; i++) data[i] = (unsigned short) tmp[i];
		printf("dgf_read_fpga: [%s] Read %d bytes from file %s\n", type, nbytes, fileSpec);
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
		fprintf(stderr, "%sdgf_read_dsp: Cannot open file %s%s\n", setred, fileSpec, setblack);
		return(-1);
	}
	nbytes = read(fd, tmp, DGF_L_CODE * sizeof(unsigned short));
	close(fd);

	if (nbytes % 4) {
		fprintf(stderr, "%sdgf_read_dsp: File %s has wrong size - %d (should be %%4) %s\n", setred, fileSpec, size, setblack);
		return(-1);
	}

	size = nbytes / 2;
	printf("dgf_read_dsp: Read %d bytes from file %s\n", nbytes, fileSpec);

	if (size > 0) {
		for (i = 0; i < size; i += 2) {
			byte0 = tmp[i] & 0x00ff;
			byte1 = (tmp[i] >> 8) & 0x00ff;
			byte2 = tmp[i + 1] & 0x00ff;
			byte3 = (tmp[i + 1] >> 8) & 0x00ff;
			if (byte2 != 0) {
				fprintf(stderr, "% sdgf_read_dsp: Data inconsistent - byte#3 != 0%s\n", setred, setblack);
			}
			data[i] = (byte3 << 8) + byte0;
			data[i + 1] = byte1;
		}
	} else {
		printf("dgf_read_dsp: file %s is empty\n", fileSpec);
	}
	return(size);
}

int dgf_download_fpga(int crate, unsigned int smask, char * type, unsigned short data[], int size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_download_fpga
// Purpose:        download fpga code to DGF-4C
// Arguments:      int crate            -- crate number
//                 unsigned int smask   -- station mask
//                 char * type          -- system or fippi D/E
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
	unsigned int sts;
	int ok;

	bcm = CC32_SET_BASE(C(crate), N(26));
	bcw = CC32_SET_BASE(C(crate), N(25));

	if (*type == 'S' || *type == 's') {										/* system of fippi fpga? */
		csr = 0x01;
		subAddr = A(10);
	} else {
		csr = 0xF0;
		subAddr = A(9);
	}

	if (smask == 0xFFFFFFFF) {
		sprintf(resource, "StationMask.Crate%d", crate); 		/* read station mask from environment */
		smask = root_env_getval_x(resource, 0);
		if (smask == 0) return(1);
	}
	CC32_WRITE_R2B(bcm, F(16), A(0), smask); 							/* set broadcast mask */

	DGF_WRITE_ICSR(bcw, csr);												/* broadcast to ICSR */
	usleep(100000);															/* wait for bcw complete */
	for (i = 0; i < size; i++) CC32_WRITE_R2B(bcw, F(17), subAddr, data[i]); /* download fpga data (system or fippi) */
	usleep(100000);															/* wait again */
	ok = 1;
	station = 1;										/* check fgpa status */
	mbits = smask;
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			sts = DGF_READ_ICSR(m) & csr;				/* read ICSR */
			if (sts != 0) { 							/* fpga bits should now be '0' */
				fprintf(stderr, "%sdgf_download_fpga: [%s] FGPA download failed for dgf in C%d.N%d - bits = %#x%s\n",
																				setred, type, crate, station, sts, setblack);
				ok = 0;
			}
		}
		station++;
		mbits >>= 1;
	}
	if (ok == 0) {
		fprintf(stderr, "%sdgf_download_fpga: [%s] Error during FPGA download%s\n", setred, type, setblack);
	} else {
		printf("%sdgf_download_fpga: [%s] FPGA Download ok - %d words to crate C%d, broadcast mask = %#lx%s\n",
												setblue, type, size, crate, smask, setblack);
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
	unsigned long mbits, smask;

	int station;
	unsigned int sts;
	int ok;
	unsigned short dspData;

	if (size <= 0) {
		fprintf(stderr, "%sdgf_download_dsp: DSP download failed for crate C%d - code size is %d%s\n", setred, crate, size, setblack);
		return(0);
	}

	bcm = CC32_SET_BASE(C(crate), N(26));
	bcw = CC32_SET_BASE(C(crate), N(25));

	sprintf(resource, "StationMask.Crate%d", crate); 							/* get station mask from environment */
	smask = root_env_getval_x(resource, 0);
	if (smask == 0) return(1);

	CC32_WRITE_R2B(bcm, F(16), A(0), smask); 								/* set broadcast mask */

	DGF_WRITE_CSR(bcw, 0x10);													/* reset dsp */
	usleep(100000);																	/* wait for bc to complete */
	DGF_WRITE_TSAR(bcw, 1);														/* start download at &data[2] */
	usleep(1);
	for (i = 2; i < size; i++) CC32_WRITE_R2B(bcw, F(16), A(0), data[i]);		/* download dsp data */
	DGF_WRITE_TSAR(bcw, 0);								/* write data[0]&[1] (will start dsp) */
	usleep(1);
	CC32_WRITE_R2B(bcw, F(16), A(0), data[0]);
	CC32_WRITE_R2B(bcw, F(16), A(0), data[1]);
	usleep(100000);															/* wait for download to complete */

	ok = 1;
	station = 1;										/* check dsp status */
	mbits = smask;
/*	DGF_WRITE_TSAR(bcw, 0); 							/* set dsp start addr for all modules at once */
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			sts = DGF_READ_CSR(m);					/* read CSR */
			if (sts & 0x1000) { 						/* check dsp error bit */
				fprintf(stderr, "%sdgf_download_dsp: DSP download failed for dgf in C%d.N%d (sts = %#lx)%s\n", setred, crate, station, sts, setblack);
				ok = 0;
			} else {
				DGF_WRITE_TSAR(m, 0);
				usleep(1);
				for (i = 0; i < size; i++) {
					dspData = DGF_READ_DSP(m);			/* compare dsp data with file contents */
					if (dspData != data[i]) {
						fprintf(stderr, "%sdgf_download_dsp: DSP data check failed for dgf in C%d.N%d at addr %#x - data = %#x, should be %#x%s\n",
												setred, crate, station, i, dspData, data[i], setblack);
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
		fprintf(stderr, "%sdgf_download_dsp: Error during DSP download%s\n", setred, setblack);
	} else {
		printf("%sdgf_download_dsp: DSP download ok - %d words to crate C%d, broadcast mask = %#lx%s\n",
																setblue, size, crate, smask, setblack);
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
	printf("%sdgf_upload_dsp: Uploaded %d words from module in crate C%d.N%d%s\n", setblue, size, crate, station, setblack);
	return(size);
}

unsigned int dgf_get_revision(int crate, int rev) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgf_get_revision
// Purpose:        read dgf revisions
// Arguments:      int crate            -- crate number
//                 int rev              -- revision
// Results:        unsigned int smask   -- station mask
// Exceptions:     
// Description:    Reads revision for all dgfs in a crate.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	unsigned int smask, mbits, bit;
	int station;
	volatile unsigned long * m;
	char * srev;
	char resource[256];
	unsigned int data;

	sprintf(resource, "StationMask.Crate%d", crate); 		/* read station mask from environment */
	mbits = root_env_getval_x(resource, 0);
	if (mbits == 0) return(0);

	station = 1;										/* check fgpa status */
	smask = 0;
	bit = 1;
	while (mbits) { 									/* module by module */
		if (mbits & 1) {								/* bit set in station mask? */
			m = CC32_SET_BASE(C(crate), N(station));
			data = DGF_READ_REVISION(m);
			if (data != FIPPI_REVD && data != FIPPI_REVE) {
				fprintf(stderr, "%sdgf_get_revision: Wrong revision for module in C%d.N%d - %d%s\n",
																setred, crate, station, data, setblack);
			}
			if (data == rev) smask |= bit;
		}
		station++;
		bit <<= 1;
		mbits >>= 1;
	}
	srev = (rev == FIPPI_REVD) ? "D" : "E";
	printf("dgf_get_revision: Broadcast mask for Rev%s modules in crate C%d is %#lx\n", srev, crate, smask);
	return(smask);
}
