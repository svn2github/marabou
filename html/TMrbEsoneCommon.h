#ifndef __TMrbEsoneCommon_h__
#define __TMrbEsoneCommon_h__

#include "esone_protos.h"

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           esone/inc/TMrbEsoneCommon.h
// Purpose:        Common defs for ESONE library
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

//___________________________________________________________________________ C.N.A.F defs
#define C(x)    x
#define N(x)    x
#define A(x)    x
#define F(x)    x

//___________________________________________________________________________ definitions taken from camlib.h
#define IS_F_READ(s)    (((unsigned int)(s) & 0x18) == 0)
#define IS_F_WRITE(s)   (((unsigned int)(s) & 0x18) == 0x10)
#define IS_F_CNTL(s)    ((!IS_F_READ((s))) & (!IS_F_WRITE((s))))
#define IS_F_VALID(s)    (((unsigned int)(s) & 0xe0) == 0)

//___________________________________________________________________________ enums taken from camdef.h
typedef enum
{
    F_RD1,        /* read group 1 register */
    F_RD2,        /* read group 2 register */
    F_RC1,        /* read and clear group 1 register */
    F_RCM,        /* read complement of group 1 register */
    F_TLM = 8,    /* test LAM */
    F_CL1,        /* clear group 1 register */
    F_CLM,        /* clear LAM */
    F_CL2,        /* clear group 2 register */
    F_WT1 = 16,   /* write group 1 register */
    F_WT2,        /* write group 2 register */
    F_SS1,        /* selective set group 1 register */
    F_SS2,        /* selective set group 2 register */
    F_SC1 = 21,   /* selective clear group 1 register */
    F_SC2 = 23,   /* selective clear group 2 register */
    F_DIS,        /* disable */
    F_XEQ,        /* execute */
    F_ENB,        /* enable */
    F_TST         /* test */
} fcode_t;

typedef enum
{
	C_CBV = 0,
	C_CC32 = 1
} ccode_t;

//___________________________________________________________________________ declarations for TMrbEsone
typedef unsigned long CamacHost_t; 		// camac host address

typedef unsigned long CamacReg_t; 		// camac register

typedef unsigned int CamacFunction_t; 	// camac function
typedef unsigned int CamacData_t;	 	// camac data
typedef unsigned int CamacQX_t;		 	// camac Q/X

typedef TArrayL CamacRegArray;		 	// camac cnaf list
typedef TArrayI CamacFArray;		 	// camac function array
typedef TArrayI CamacDArray;		 	// camac data array
typedef TArrayI CamacQXArray;		 	// camac Q/X array

typedef unsigned int CamacStatus_t;	 	// camac status
typedef unsigned int CamacError_t;	 	// camac error code

enum					{	kMaxNofCrates		=	16	 		};
enum					{	kMaxRepeatCount		=	0x10000000	};
enum					{	kNoData 			=	0xffffffff 	};
enum					{	kFDisable			=	F_DIS	};
enum					{	kFEnable 			=	F_ENB	};

#endif
