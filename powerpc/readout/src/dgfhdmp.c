/*____________________________________________________________[C LIBRARY FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgfhdmp.c
// Purpose:        Dump histgram buffer to ascii file
// Description:    Reads up to 8 pages from mca memory
//                 and writes it file (format (x,y))
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

#define FIPPI_REVD	3
#define FIPPI_REVE	4

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

/* camac controller */
void cc32_init(unsigned long * physAddrCrate1, int nofCrates);


int main() {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           dgfhdmp
// Purpose:        Dump histogram memory to ascii file
// Arguments:      
// Results:        
// Exceptions:     
// Description:      
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

 	cc32_init((unsigned long *) 0xee550000L, 3);				/* vme addr mapping for crates 1 ... 3 */

	addr = this->GetParValue("AOUTBUFFER");
	wc = this->GetParValue("LOUTBUFFER");

	nofWords = TMrbDGFData::kMCAPageSize * nofChannels;
	Buffer.SetESize(nofWords);		// size of energy region
	Buffer.Set(nofWords);			// reset data region
	
	Int_t fc = fCamac.HasFastCamac();
	if (fc == -1) fc = 0;

	k = 0;			// buffer index
	nofWords = 0;
	ofstream f;
	f.open("hist.dat", ios::out);
	for (chn = 0; chn < TMrbDGFData::kNofChannels; chn++) {
		if (Buffer.IsActive(chn)) {
			this->SetParValue("HOSTIO", chn, kTRUE);
			for (j = 0; j < 8; j++) {
				this->SetParValue("RUNTASK", TMrbDGFData::kRunControl, kTRUE);		// reprogram DACs
				cTask = (j == 0) ? TMrbDGFData::kReadHistoFirstPage : TMrbDGFData::kReadHistoNextPage;
				this->SetParValue("CONTROLTASK", cTask, kTRUE); 	// xfer histo page to i/o buffer
				this->StartRun();
				this->WaitActive(1);								// wait for xfer to complete
				this->WriteTSAR(addr); 							// read data from i/o buffer
				if (fCamac.BlockXfer(fCrate, fStation, A(0), F(5), wc, kTRUE) == -1) {	// xfer energies, 2 * 16 bits
					gMrbLog->Err()	<< fName << " in C" << fCrate << ".N" << fStation
									<< ".A0.F0: Reading histogram energies failed - DSPAddr=0x" << setbase(16) << addr
									<< setbase(10) << ", wc=" << wc << ", ActionCount=-1" << endl;
					gMrbLog->Flush(this->ClassName(), "ReadHistogramBuffer");
					return(-1);
				}
				nofWords += wc / 2;
				dp = (Int_t *) fCamac.GetDAddr();
				data = 0;
				for (i = 0; i < wc; i++, dp++) {
					if (i & 1) {
						data |= *dp;
						Buffer[k] = (Int_t) data;
						f << setw(8) << k << setw(10) << Buffer[k] << endl;
						k++;
					} else {
						data = *dp << 8;
					}
				}
			}
		}
	}
	f.close();
}
