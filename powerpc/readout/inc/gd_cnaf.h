/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       %M%
// @(#)Purpose:    Cnaf definitions
// Typedefs:       CnafSpec  -- define cnaf data
// Description:    Defines cnaf structure.
// @(#)Author:     R. Lutter 
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CRATE_MAX	7

/*_______________________________________________________________[C STRUCTURE]
//////////////////////////////////////////////////////////////////////////////
// Name:           CnafSpec
// Purpose:        Structure containing cnaf data
// Struct members: int crate           -- crate number (C)
//                 int station         -- station (N)
//                 int addr1           -- address (A)
//                 int addr2           -- ... 2 (if range of subadresses)
//                 int function        -- function (F)
//                 long data           -- data word if output (F16 .. F23)
//                 int repeat          -- repetition
//                 unsigned int type   -- cnaf type (CNAF_T_xxxx)
//                 insigned int label  -- label type (CNAF_LT_xxxx)
// Description:    Defines how a cnaf looks like.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

typedef struct {
	int crate;
	int station;
	int addr1;
	int addr2;
	int function;
	long data;
	int repeat;
	unsigned int type;
	unsigned int label;
} CnafSpec;

/*______________________________________________________________[BITS & BYTES]
//////////////////////////////////////////////////////////////////////////////
// Name:           CNAF_LT_xxxx
// Purpose:        Define label types
// Definitions:    CNAF_LT_INIT	-- init		CAMINIT:
//                 CNAF_LT_READ	-- read		CAMREAD:
// Description:    Describes possible labels used in MBS cnaf tables.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CNAF_LT_INIT		1
#define CNAF_LT_READ		2

/*______________________________________________________________[BITS & BYTES]
//////////////////////////////////////////////////////////////////////////////
// Name:           CNAF_T_xxxx
// Purpose:        Define cnaf types
// Definitions:    CNAF_T_CONTROL   -- camac control
//                 CNAF_T_READ      -- camac read
//                 CNAF_T_WRITE     -- camac write
// Description:    Describes cnaf i/o modes.
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define CNAF_T_CONTROL		1
#define CNAF_T_READ			2
#define CNAF_T_WRITE		4
