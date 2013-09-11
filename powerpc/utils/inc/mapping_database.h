#ifndef __MAPPING_DATABASE_H__
#define __MAPPING_DATABASE_H__

#include "LwrTypes.h"

#define MAP_STRLEN	64

#ifdef CPU_TYPE_RIO2
static Char_t * rioType = "RIO2";
#elif CPU_TYPE_RIO3
static Char_t * rioType = "RIO3";
#else CPU_TYPE_RIO4
static Char_t * rioType = "RIO4";
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

/* mapping modes */
/* !!! has to be same as defined in TMrbConfig.h !!! */
enum EMrbVMEMapping	 {	kVMEMappingUndef	=	0,			/* vme mapping */
				kVMEMappingDirect	=	BIT(0),
				kVMEMappingStatic	=	BIT(1),
				kVMEMappingDynamic	=	BIT(2),
				kVMEMappingHasBLT	=	BIT(10),
				kVMEMappingHasMBLT	=	BIT(11),
				kVMEMappingPrivate	=	BIT(12)
			};

/* address modifiers */
enum	EAddrMod	{	kAM_A32		=	0x09,
				kAM_A24		=	0x39,
				kAM_A16		=	0x29,
				kAM_BLT		=	0x0b,
				kAM_MBLT	=	0x08
			};

/* static mapping tables */
#ifdef CPU_TYPE_RIO2
enum	EMapAddr	{	kAddr_A32	=	0xE0000000,
				kAddr_A24	=	0xEE000000,
				kAddr_A16	=	0xEFFE0000
			};
enum	EMapSize	{	kSize_A32	=	0x0E000000,
				kSize_A24	=	0x01000000,
				kSize_A16	=	0x00010000
			};
#elif CPU_TYPE_RIO3
enum	EMapAddr	{	kAddr_A32	=	0x50000000,
				kAddr_A24	=	0x4F000000,
				kAddr_A16	=	0x4E000000,
				kAddr_BLT	=	0x50000000,
				kAddr_MBLT	=	0x60000000
			};
enum	EMapSize	{	kSize_A32	=	0x10000000,
				kSize_A24	=	0x01000000,
				kSize_A16	=	0x00010000,
				kSize_BLT	=	0x10000000,
				kSize_MBLT	=	0x10000000
			};
#else
enum	EMapAddr	{	kAddr_A32	=	0x50000000,
				kAddr_A24	=	0x4F000000,
				kAddr_A16	=	0x4E000000,
				kAddr_BLT	=	0x50000000,
				kAddr_MBLT	=	0x60000000,
				kAddr_A32Direct	=	0xF0000000
			};
enum	EMapSize	{	kSize_A32	=	0x06000000,
				kSize_A24	=	0x01000000,
				kSize_A16	=	0x00010000
			};
#endif


struct s_mapDescr {
	Char_t mdName[MAP_STRLEN];		/* name, to be used with smem_create() etc */

	UInt_t mappingModes;			/* possible VME mapping modes */

	UInt_t mappingVME;				/* used VME mapping */
	UInt_t addrModVME;				/* address modifier */
	UInt_t physAddrVME;				/* phys addr given by module switches */
	volatile Char_t * vmeBase;		/* mapped address, static or dynamic mapping */
	Int_t segSizeVME;				/* segment size */
	Int_t busId;					/* identifier from bus_open() */

	Bool_t bltEnabled;				/* TRUE if BLT is enabled */
	UInt_t mappingBLT;				/* used BLT mapping */
	UInt_t addrModBLT;				/* address modifier */
	UInt_t physAddrBLT;				/* phys addr given by module switches */
	volatile Char_t * bltBase;			/* mapped address */
	Int_t segSizeBLT;				/* segment size */
	Int_t bltModeId;				/* id from bma_create_mode() */

	Int_t nofMappings;
	
	void * handle;					/* pointer to device specific data */

	void * prevDescr;				/* linked list */
	void * nextDescr;

};
#endif
