#ifndef __MAPPING_DATABASE_H__
#define __MAPPING_DATABASE_H__

#include "LwrTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/vmelib.h>
#include <ces/bmalib.h>

#define STRLEN	64

#ifdef CPU_TYPE_RIO2
static Char_t * cpuType = "RIO2";
#elif CPU_TYPE_RIO3
static Char_t * cpuType = "RIO3";
#else CPU_TYPE_RIO4
static Char_t * cpuType = "RIO4";
#endif

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			Mapping_Database.h
//! \brief			Definition for VME/BLT mapping
//! \details			Structures describing vme mapping
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

/* address modifiers */
enum	EAddrMod	{	kAM_A32		=	0x09,
						kAM_A24		=	0x39,
						kAM_BLT		=	0x0b,
						kAM_MBLT	=	0x08
					};

/* static mapping tables */
#ifdef CPU_TYPE_RIO2
enum	EMapAddr	{	kAddr_A32	=	0xE0000000,
						kAddr_A24	=	0xEE000000
					};
enum	EMapSize	{	kSize_A32	=	0x0E000000,
						kSize_A24	=	0x01000000
						kAddr_BLT	=	0xFFFFFFFF,
						kAddr_MBLT	=	0xFFFFFFFF
					};
#else
enum	EMapAddr	{	kAddr_A32	=	0x50000000,
						kAddr_A24	=	0x4F000000,
						kAddr_BLT	=	0x50000000,
						kAddr_MBLT	=	0x60000000
					};
enum	EMapSize	{	kSize_A32	=	0x10000000,
						kSize_A24	=	0x01000000,
						kSize_BLT	=	0x10000000,
						kSize_MBLT	=	0x10000000
					};
#endif
					
struct s_mapDescr {
	Char_t name[STRLEN];				/* name, to be used with smem_create() etc */

	Bool_t staticMapping;				/* TRUE if STATIC mapping has to be used */
	
	Bool_t mayUseBLT;					/* TRUE if module is capable of using BLT */
	Bool_t bltEnabled;					/* TRUE if BLT is enabled */

	UInt_t addrModVME;					/* address modifier */
	UInt_t physAddrVME;					/* phys addr given by module switches */
	volatile Char_t * vmeBase;			/* mapped address, static or dynamic mapping */
	Int_t segSizeVME;					/* segment size */

	UInt_t addrModBLT;					/* address modifier */
	UInt_t physAddrBLT;					/* phys addr given by module switches */
	volatile Char_t * bltBase;			/* mapped address */
	Int_t segSizeBLT;					/* segment size */
	UInt_t modeId;						/* id from bma_create_mode() */

	void * prevDescr;					/* linked list */
	void * nextDescr;

};
#endif