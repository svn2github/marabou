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

#define C(x)    x
#define N(x)    x
#define A(x)    x
#define F(x)    x

#define IS_F_READ(s)    (((unsigned int)(s) & 0x18) == 0)
#define IS_F_WRITE(s)   (((unsigned int)(s) & 0x18) == 0x10)
#define IS_F_CNTL(s)    ((!IS_F_READ((s))) && (!IS_F_WRITE((s))))
#define IS_F_RSTATUS(s) ((IS_F_CNTL((s))) && ((s == 8) || (s == 27)))
#define IS_F_VALID(s)   (((unsigned int)(s) & 0xe0) == 0)

enum					{	kEsoneNofCrates 		=	8 			};

enum					{	kEsoneError 			=	BIT(31) 	};
enum					{	kEsoneX 	 			=	BIT(30) 	};
enum					{	kEsoneQ 	 			=	BIT(29) 	};
enum					{	kEsoneNoData 	 		=	BIT(24) 	};

enum					{	kEsoneDataMask			=	BIT(24) - 1 };
enum					{	kEsoneNofRepeats		=	0x10000	 	};

enum 					{	kEsoneWaitForServer		=	15		 	};

#define IS_ERROR(s)		(((unsigned int)(s) & kEsoneError) == 0)
#define IS_X(s)			(((unsigned int)(s) & kEsoneX) != 0)
#define IS_Q(s)			(((unsigned int)(s) & kEsoneQ) != 0)

#endif
