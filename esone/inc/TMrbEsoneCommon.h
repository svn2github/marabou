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

enum					{	kEsoneNofCrates 		=	8 			};

enum EMrbEsoneStatus	{	kEsoneError 			=	BIT(31),			// status bits
							kEsoneX 	 			=	BIT(30),
							kEsoneQ 	 			=	BIT(29),
							kEsoneNoData 	 		=	BIT(24)
						};

enum EMrbEsoneCnafType	{	kCnafTypeUndefined	=	0,						// cnaf types
							kCnafTypeRead		=	BIT(0),
							kCnafTypeControl	=	BIT(1),
							kCnafTypeWrite		=	BIT(2),
							kCnafTypeReadStatus =	BIT(3),
							kCnafTypeAny		=	kCnafTypeRead | kCnafTypeControl | kCnafTypeWrite
						};

enum EMrbEsoneCamacAction
						{	kCANone 					= 	0x0,
							kCASingle					=	BIT(0),									
							kCAMultiple					=	BIT(1),									
							kCAAddrScan 				=	BIT(2),
							kCABlockXfer 				=	BIT(3),
							kCAQDriven					=	BIT(4),
							kCA16Bit	 				=	BIT(5),
							kCA24Bit			 		=	BIT(6),
							kCA_cfsa					=	kCASingle | kCA24Bit,
							kCA_cssa					=	kCASingle | kCA16Bit,
							kCA_cfga					=	kCAMultiple | kCA24Bit,
							kCA_csga					=	kCAMultiple | kCA16Bit,
							kCA_cfmad					=	kCAAddrScan | kCA24Bit,
							kCA_csmad					=	kCAAddrScan | kCA16Bit,
							kCA_cfubc					=	kCABlockXfer | kCA24Bit,
							kCA_csubc					=	kCABlockXfer | kCA16Bit,
							kCA_cfubr					=	kCABlockXfer | kCAQDriven | kCA24Bit,
							kCA_csubr					=	kCABlockXfer | kCAQDriven | kCA16Bit
						};

enum EMrbEsoneCamacFunction
						{	kCF_RD1,        /* read group 1 register */
							kCF_RD2,        /* read group 2 register */
							kCF_RC1,        /* read and clear group 1 register */
							kCF_RCM,        /* read complement of group 1 register */
							kCF_TLM = 8,    /* test LAM */
							kCF_CL1,        /* clear group 1 register */
							kCF_CLM,        /* clear LAM */
							kCF_CL2,        /* clear group 2 register */
							kCF_WT1 = 16,   /* write group 1 register */
							kCF_WT2,        /* write group 2 register */
							kCF_SS1,        /* selective set group 1 register */
							kCF_SS2,        /* selective set group 2 register */
							kCF_SC1 = 21,   /* selective clear group 1 register */
							kCF_SC2 = 23,   /* selective clear group 2 register */
							kCF_DIS,        /* disable */
							kCF_XEQ,        /* execute */
							kCF_ENB,        /* enable */
							kCF_TST         /* test */
						};

enum					{	kEsoneDataMask			=	BIT(24) - 1 };
enum					{	kEsoneNofRepeats		=	0x10000	 	};
enum 					{	kEsoneWaitForServer		=	15		 	};

#define C(x)    x
#define N(x)    x
#define A(x)    x
#define F(x)    x

#define IS_F_READ(s)    (((unsigned int)(s) & 0x18) == 0)
#define IS_F_WRITE(s)   (((unsigned int)(s) & 0x18) == 0x10)
#define IS_F_CNTL(s)    ((!IS_F_READ((s))) && (!IS_F_WRITE((s))))
#define IS_F_RSTATUS(s) ((IS_F_CNTL((s))) && ((s == 8) || (s == 27)))
#define IS_F_VALID(s)   (((unsigned int)(s) & 0xe0) == 0)

#define IS_ERROR(s)		(((unsigned int)(s) & kEsoneError) == 0)
#define IS_X(s)			(((unsigned int)(s) & kEsoneX) != 0)
#define IS_Q(s)			(((unsigned int)(s) & kEsoneQ) != 0)

#endif
