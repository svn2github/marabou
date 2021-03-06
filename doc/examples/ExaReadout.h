/*_________________________________________________________[C DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           ExaReadout.h
// Purpose:        MBS readout function
//                 exa: Example configuration
//
// Description:    Definitions to be used byt ExaReadout.c
//
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
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

#define CC_AUTOREAD_ON(cc)	CIO_WRITE_R2B(cc, A(0), F(3), CIO_READ_R2B(cc, A(0), F(3)) | (0x1 << 12))
#define CC_AUTOREAD_OFF(cc)	CIO_WRITE_R2B(cc, A(0), F(3), CIO_READ_R2B(cc, A(0), F(3)) & ~(0x1 << 12))

#define CC_READ_DWORD_ON(cc)	CIO_WRITE_R2B(cc, A(2), F(16), 0)
#define CC_READ_DWORD_OFF(cc)	CIO_WRITE_R2B(cc, A(3), F(16), 0)

#define WITH_SCALE_DOWN(n)	if ((total_event_count % n) == 0)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/ Device-dependent definitions
/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

