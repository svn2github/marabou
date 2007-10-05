/*_________________________________________________________[C DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           GamsReadout.h
// Purpose:        MBS readout function
//                 gams: GAMS experiment
//
// Description:    Definitions to be used byt GamsReadout.c
//
// Author:         gams
// Revision:         
// Date:           Mon Aug 13 12:29:13 2007
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/


#define UXS_N_VALS	5
#define UXS_L_STR	128

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Struct used by unix_string functions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct {
	char name[UXS_L_STR];
	int nvals;
	char values[UXS_N_VALS][UXS_L_STR];
	char curval[UXS_L_STR];
} TUnixEntry;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device tables
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct {
	char * name;
	int index;
	int crate;
	int station;
	unsigned long lam;
	int nofParams;
	int nofParamsUsed;
	unsigned long paramPattern;
	volatile unsigned long * addr;
} ModuleDef;

typedef struct {
	char * name;
	int index;
	int nofModules;
	int firstModule;
	unsigned long lamPattern;
} SevtDef;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Useful macro defs
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef MBS_SIMUL_RDO
#define CC_AUTOREAD_ON(cc)
#define CC_AUTOREAD_OFF(cc)
#define CC_READ_DWORD_ON(cc)
#define CC_READ_DWORD_OFF(cc)
#else
#define CC_AUTOREAD_ON(cc)	cc += 0x0c; \
							cctmp = *((volatile unsigned short *) cc); \
							*((volatile unsigned short *) cc) = cctmp | (0x1 << 12)

#define CC_AUTOREAD_OFF(cc)	cc += 0x0c; \
							cctmp = *((volatile unsigned short *) cc); \
							*((volatile unsigned short *) cc) = cctmp & ~(0x1 << 12)

#define CC_READ_DWORD_ON(cc)	CIO_WRITE_R2B(cc, A(2), F(16), 0)
#define CC_READ_DWORD_OFF(cc)	CIO_WRITE_R2B(cc, A(3), F(16), 0)
#endif

#define WITH_SCALE_DOWN(n)	if ((total_event_count % n) == 0)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device-dependent definitions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

unsigned short cctmp;	/* temp for vc32/cc32 controller functions */

