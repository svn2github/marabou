/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           histoDump.c
// Purpose:        Dump histogram memory to file
// Description:    Writes mca data to ascii file
// Author:         R. Lutter
// Revision:       
// Date:           Nov 2002
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

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

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

#define DGF_INPAR_START_ADDR			0x4000
#define DGF_NOF_DSP_INPAR_MODULE		64
#define DGF_NOF_DSP_INPAR_CHANNEL	48

#define DGF_OFFS_RUNTASK				5
#define DGF_OFFS_CONTROLTASK			6
#define DGF_OFFS_HOSTIO					12
#define DGF_OFFS_AOUTBUFFER			304
#define DGF_OFFS_LOUTBUFFER			305

#define DGF_RUN_CONTROL					0
#define DGF_CTRL_READ_HISTO_FIRST	9
#define DGF_CTRL_READ_HISTO_NEXT		10

#define DGF_CSR_RUNENA 					(0x1 << 0)
#define DGF_CSR_NEWRUN 					(0x1 << 1)
#define DGF_CSR_ENALAM 					(0x1 << 3)
#define DGF_CSR_DSP_RESET				(0x1 << 4)
#define DGF_CSR_DSP_ERROR 				(0x1 << 12)
#define DGF_CSR_ACTIVE 					(0x1 << 13)
#define DGF_CSR_LAM_ACTIVE 			(0x1 << 14)

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
#define DGF_READ_DSP_FAST(dgf)		CC32_READ_R2B_16(dgf, A(0), F(5))
#define DGF_WRITE_DSP(dgf, data)	CC32_WRITE_R2B(dgf, A(0), F(16), data)

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

/* dgf modules */
unsigned short dgf_get_par_value(volatile unsigned long * dgf, int chn, int pidx);
void dgf_set_par_value(volatile unsigned long * dgf, int chn, int pidx, unsigned short pval);
void dgf_modify_csr(volatile unsigned long * dgf, unsigned short bits2clear, unsigned short bits2set);
int dgf_wait_active(volatile unsigned long * dgf);

/* camac controller */
void cc32_init(unsigned long * physAddrCrate1, int nofCrates);


int main(int argc, char * argv[]) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           histoDump
// Purpose:        Test parameters of a DGF-4C module
// Arguments:      int crate             -- crate number
//                 int station           -- station number
//                 int chnPattern        -- channel pattern
//                 char * file           -- file name for output
//                 char * mode           -- output mode ("b" or "a")
// Results:        
// Exceptions:     
// Description:      
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	int i, idx;
	int crate;
	int station;
	unsigned int chnPattern;
	unsigned int addr;
	int nofWords, nofChannels, chn, page, cTask, wc;
	char fileName[128], handshake[128];
	char mode, quiet;
	int ascii, beQuiet;
	FILE * f;
	int errCnt;
	unsigned int data[8192], data32[4 * 8192], d;

	volatile unsigned long * dgf;

	if (argc < 6) {
		fprintf(stderr, "%shistoDump: Illegal number of arguments - %d%s\n", setred, argc - 1, setblack);
		fprintf(stderr, "\n           Usage: histoDump <crate> <station> <chnPattern> <file> <mode> <quiet>\n\n");
		exit(1);
	}

	crate = strtol(argv[1], NULL, 0);
	station = strtol(argv[2], NULL, 0);
	chnPattern = strtol(argv[3], NULL, 0);

	strcpy(fileName, argv[4]);

	strcpy(handshake, ".");						/* handshake with file .<filename>.ok */
	strcat(handshake, fileName);
	strcat(handshake, ".ok");
		
	errCnt = 0;

	f = fopen(fileName, "w");
	if (f == NULL) {
		fprintf(stderr, "%s?histoDump: Can't open file - %s%s\n", setred, fileName, setblack);
		errCnt++;
		histoDump_write_hs(handshake, errCnt);
		exit(1);
	}

	mode = *argv[5];
	switch (mode) {
		case 'a':
		case 'A':	ascii = 1; break;
		case 'b':
		case 'B':	ascii = 0; break;
		default:	fprintf(stderr, "%s?histoDump: Illegal file mode - %s%s\n", setred, argv[5], setblack);
					errCnt++;
					histoDump_write_hs(handshake, errCnt);
					exit(1);
	}

	quiet = *argv[5];
	beQuiet = 0;
	switch (quiet) {
		case 'q':
		case 'Q':	beQuiet = 1; break;
	}

 	cc32_init((unsigned long *) 0xee550000L, crate);
	dgf = CC32_SET_BASE(C(crate), N(station));

	addr = dgf_get_par_value(dgf, -1, DGF_OFFS_AOUTBUFFER);
	wc = dgf_get_par_value(dgf, -1, DGF_OFFS_LOUTBUFFER);

	nofWords = 0;
	nofChannels = 0;
	for (chn = 0; chn < 4; chn++) {
		if (chnPattern & (1 << chn)) {
			memset(data32, 0, sizeof(int) * 8 * 1024);
			idx = 0;
			nofChannels++;
			dgf_set_par_value(dgf, -1, DGF_OFFS_HOSTIO, chn);
			for (page = 0; page < 8; page++) {
				dgf_set_par_value(dgf, -1, DGF_OFFS_RUNTASK, DGF_RUN_CONTROL);
				cTask = (page == 0) ? DGF_CTRL_READ_HISTO_FIRST : DGF_CTRL_READ_HISTO_NEXT;
				dgf_set_par_value(dgf, -1, DGF_OFFS_CONTROLTASK, cTask);
				dgf_modify_csr(dgf, 0, DGF_CSR_RUNENA);
				dgf_wait_active(dgf);
				DGF_WRITE_TSAR(dgf, addr);
				for (i = 0; i < wc; i++) data[i] = DGF_READ_DSP_FAST(dgf);
				nofWords += wc / 2;
				for (i = 0; i < wc; i++) {
					if (i & 1) {
						d |= data[i];
						data32[idx] = d;
						idx++;
					} else {
						d = data[i] << 8;
					}
				}
			}
			if (ascii == 0) {
				fwrite(data32, sizeof(unsigned int), idx, f);
			} else {
				for (i = 0; i < idx; i++) fprintf(f, "%6d%10d\n", i, data32[i]);
			}
		}
	}

	if (beQuiet == 0) {
		printf("histoDump: DGF-4C in C%d.N%d --> %s (%d channels, %d words, mode %c)\n",
								crate, station, fileName, nofChannels, nofWords, mode);
	}
	
	fclose(f);
	
	f = fopen(handshake, "w");					/* write handshake file */
	fprintf(f, "HistoDump.Errors: %d\n", errCnt);
	fprintf(f, "HistoDump.Crate: %d\n", crate);
	fprintf(f, "HistoDump.Station: %d\n", station);
	fprintf(f, "HistoDump.ChnPattern: %#x\n", chnPattern);
	fprintf(f, "HistoDump.File: %s\n", fileName);
	fprintf(f, "HistoDump.Mode: %s\n", (ascii == 0) ? "binary" : "ascii");
	fprintf(f, "HistoDump.NofDataWords: %d\n", nofWords);
	fclose(f);
	
	exit(0);
}

int histoDump_write_hs(char * hs, int errCnt) {
	FILE * f;
	f = fopen(hs, "w");					/* write handshake file */
	fprintf(f, "HistoDump.Errors: %d\n", errCnt);
	fclose(f);
}

unsigned short dgf_get_par_value(volatile unsigned long * dgf, int chn, int pidx) {	/* read param value from dsp */
	unsigned short pval;
	if (chn >= 0) pidx += chn * DGF_NOF_DSP_INPAR_CHANNEL; /* add channel offset if param belongs to a single channel */
	DGF_WRITE_PSA(dgf, pidx);								/* write param start addr */
	DGF_READ_TSAR(dgf);												/* settle down */
	pval = DGF_READ_DSP(dgf);								/* fetch param value from dsp memory */
	return(pval);
}

void dgf_set_par_value(volatile unsigned long * dgf, int chn, int pidx, unsigned short pval) {	/* set param value in dsp */
	if (chn >= 0) pidx += chn * DGF_NOF_DSP_INPAR_CHANNEL; /* add channel offset if param belongs to a single channel */
	DGF_WRITE_PSA(dgf, pidx);								/* write param start addr */
	DGF_READ_TSAR(dgf);												/* settle down */
	DGF_WRITE_DSP(dgf, pval);								/* write param value to dsp memory */
}

void dgf_modify_csr(volatile unsigned long * dgf, unsigned short bits2clear, unsigned short bits2set) {
	unsigned int csr;
	csr = DGF_READ_CSR(dgf);
	if (bits2clear) csr &= ~bits2clear;
	if (bits2set) csr |= bits2set;
	DGF_WRITE_CSR(dgf, csr);
}

int dgf_wait_active(volatile unsigned long * dgf) {
	unsigned int csr;
	csr = DGF_READ_CSR(dgf);
	while ((csr & (DGF_CSR_ACTIVE | DGF_CSR_RUNENA)) != 0) csr = DGF_READ_CSR(dgf);
	return(1);
}

