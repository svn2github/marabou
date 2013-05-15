/*_________________________________________________________________[C UTILITY]
//////////////////////////////////////////////////////////////////////////////
// @(#)Name:       mapping.c
// @(#)Purpose:    Perform VME and BLT mapping
// Description:    Functions to perform mapping, either statically or dynamically
// @(#)Author:     R. Lutter
// @(#)Revision:   SCCS:  %W%
// @(#)Date:       %G%
// URL:            
// Keywords:       
//
// Mapping                          RIO2          RIO3        RIO4/A32    RIO4/A24/A16
//                 single cycle     static        static      direct      static
//                 BLT              dynamic       static      dynamic     dynamic
//                 MBLT             dynamic       static      dynamic     dynamic
//
//                 direct:  mapping thru 0xf0000000+physAddr
//                 static:  mapping according to tables in mapping_database.h
//                 dynamic: mapping via xvme()
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <smem.h>

#include <allParam.h>
#include <ces/vmelib.h>
#include <ces/bmalib.h>

#include "root_env.h"
#include "unix_string.h"
#include "mapping_database.h"

#include "err_mask_def.h"
#include "errnum_def.h"

/*___________________________________________________________________[GLOBALS]
////////////////////////////////////////////////////////////////////////////*/
static struct pdparam_master s_param; 		/* vme segment params */

static struct s_mapDescr * firstDescr = NULL;	/* first module in list */
static struct s_mapDescr * lastDescr = NULL;	/* ... last module */

Char_t msg[256];


struct s_mapDescr * mapVME(const Char_t * DescrName, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, UInt_t Mapping) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapVME
// Purpose:        Map VME address
// Arguments:      Char_t * DescrName       -- descriptor name
//                 UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
//                 UInt_t AddrMod           -- address modifier
//                 UInt_t Mapping           -- available mapping modes
// Results:        s_mapDescr * Descr       -- pointer to database
// Description:    Performs VME mapping, either statically or dynamically
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct s_mapDescr * md;
	UInt_t staticBase;
	UInt_t dynamicAddr;
	UInt_t cpuBaseAddr;
	
	if ((md = _createMapDescr(DescrName)) == NULL) {
		sprintf(msg, "[mapVME] %s: mapping failed", DescrName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	if (Size == 0) Size = 4096;

	md->mappingModes = Mapping;
	md->mappingVME = kVMEMappingUndef;
	md->mappingBLT = kVMEMappingUndef;
	
#ifdef CPU_TYPE_RIO4
	if (Mapping & kVMEMappingDirect && AddrMod == kAM_A32) {	/* direct mapping for RIO4/A32 only */
		if (PhysAddr > 0x0FFFFFFF) {
			sprintf(msg, "[mapVME] %s: Direct mapping not possible - %#lx (max 0x0FFFFFFF)", DescrName, PhysAddr);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			return NULL;
		}
		md->vmeBase = PhysAddr | kAddr_A32Direct;
		md->mappingVME = kVMEMappingDirect;
		sprintf(msg, "[mapVME] %s: Direct mapping %#lx -> %#lx, addrMod=%#x", DescrName, PhysAddr, md->vmeBase, AddrMod);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	if (md->mappingVME == kVMEMappingUndef) {
		if (Mapping & kVMEMappingStatic) {
			if (PhysAddr > 0x0FFFFFFF) {
				sprintf(msg, "[mapVME] %s: Static mapping not possible - %#lx (max 0x0FFFFFFF)", DescrName, PhysAddr);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			switch (AddrMod) {
				case kAM_A32:
					staticBase = kAddr_A32;
					break;
				case kAM_A24:
					staticBase = kAddr_A24;
					if (PhysAddr > 0x00FFFFFF) {
						sprintf(msg, "[mapVME] %s: Not a A24 addr - %#lx", DescrName, PhysAddr);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
						return NULL;
					}
					break;
				case kAM_A16:
					staticBase = kAddr_A16;
					if (PhysAddr > 0x0000FFFF) {
						sprintf(msg, "[mapVME] %s: Not a A16 addr - %#lx", DescrName, PhysAddr);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
						return NULL;
					}
					break;
				default:
					sprintf(msg, "[mapVME] %s: Illegal addr modifier - %#lx", DescrName, AddrMod);
					f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
					return NULL;
			}

			md->vmeBase = smem_create(DescrName, (Char_t *) (staticBase | PhysAddr), Size, SM_READ|SM_WRITE);
			if (md->vmeBase == NULL) {
				sprintf(msg, "[mapVME] %s: Creating shared segment failed - %s (%d)", DescrName, sys_errlist[errno], errno);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			sprintf(msg, "[mapVME] %s: Static mapping %#lx -> %#lx, addrMod=%#x", DescrName, PhysAddr, md->vmeBase, AddrMod);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			md->mappingVME = kVMEMappingStatic;
		} else if (Mapping & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO4
			md->busId = bus_open("xvme_mas");
			dynamicAddr = bus_map(md->busId, PhysAddr, 0, Size, 0xa0, 0);
#else
 			s_param.iack = 1;
 			s_param.rdpref = 0;
 			s_param.wrpost = 0;
 			s_param.swap = SINGLE_AUTO_SWAP;
 			s_param.dum[0] = 0;
 			dynamicAddr = find_controller(PhysAddr, Size, AddrMod, 0, 0, &s_param);
#endif
			if (dynamicAddr == 0xFFFFFFFF) {
				sprintf(msg, "[mapVME] %s: Dynamic mapping failed - %s (%d)", DescrName, sys_errlist[errno], errno);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			} else {
				md->vmeBase = (volatile Char_t *) dynamicAddr;
			}
			sprintf(msg, "[mapVME] %s: Dynamic mapping %#lx -> %#lx, addrMod=%#x", DescrName, PhysAddr, md->vmeBase, AddrMod);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			md->mappingVME = kVMEMappingDynamic;
		} else {
			sprintf(msg, "[mapVME] %s: Illegal mapping mode - %#lx", DescrName, Mapping);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			return NULL;
		}
	}

	strcpy(md->mdName, DescrName);
	md->addrModVME = AddrMod;
	md->physAddrVME = PhysAddr;
	md->segSizeVME = Size;
	md->nofMappings++;
	return md;
}

volatile Char_t * mapAdditionalVME(struct s_mapDescr * mapDescr, UInt_t PhysAddr, Int_t Size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapAdditionalVME
// Purpose:        Add another VME page
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
//                 UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
// Results:        Char_t * MappedAddr      -- mapped address
// Description:    Maps an additional address.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	volatile Char_t * mappedAddr;
	UInt_t staticBase;
	UInt_t dynamicAddr;
	UInt_t cpuBaseAddr;

	Char_t segName[64];

	if (Size == 0) Size = mapDescr->segSizeVME;

	switch (mapDescr->mappingVME) {
#ifdef CPU_TYPE_RIO4
		case kVMEMappingDirect:
			if (PhysAddr > 0x0FFFFFFF) {
				sprintf(msg, "[mapAdditionalVME] %s: Direct mapping not possible - %#lx (max 0x0FFFFFFF)", mapDescr->mdName, PhysAddr);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			mapDescr->nofMappings++;
			return (volatile Char_t *) (PhysAddr | kAddr_A32Direct);
#endif
		case kVMEMappingStatic:
			if (PhysAddr > 0x0FFFFFFF) {
				sprintf(msg, "[mapAdditionalVME] %s: Static mapping not possible - %#lx (max 0x0FFFFFFF)", mapDescr->mdName, PhysAddr);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			switch (mapDescr->addrModVME) {
				case kAM_A32:
					staticBase = kAddr_A32;
					break;
				case kAM_A24:
					staticBase = kAddr_A24;
					if (PhysAddr > 0x00FFFFFF) {
						sprintf(msg, "[mapAdditionalVME] %s: Not a A24 addr - %#lx", mapDescr->mdName, PhysAddr);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
						return NULL;
					}
					break;
				case kAM_A16:
					staticBase = kAddr_A16;
					if (PhysAddr > 0x0000FFFF) {
						sprintf(msg, "[mapAdditionalVME] %s: Not a A16 addr - %#lx", mapDescr->mdName, PhysAddr);
						f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
						return NULL;
					}
					break;
			}
			sprintf(segName, "%s_%d", mapDescr->mdName, mapDescr->nofMappings + 1);
			mappedAddr = smem_create(segName, (Char_t *) (staticBase | PhysAddr), Size, SM_READ|SM_WRITE);
			if (mappedAddr == NULL) {
				sprintf(msg, "[mapAdditionalVME] %s: Creating shared segment failed - %s (%d)", mapDescr->mdName, sys_errlist[errno], errno);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			mapDescr->nofMappings++;
			return mappedAddr;

		case kVMEMappingDynamic:
#ifdef CPU_TYPE_RIO4
			dynamicAddr = bus_map(mapDescr->busId, PhysAddr, 0, Size, 0xa0, 0);
#else
 			s_param.iack = 1;
 			s_param.rdpref = 0;
 			s_param.wrpost = 0;
 			s_param.swap = SINGLE_AUTO_SWAP;
 			s_param.dum[0] = 0;
 			dynamicAddr = find_controller(PhysAddr, Size, mapDescr->addrModVME, 0, 0, &s_param);
#endif
			if (dynamicAddr == 0xFFFFFFFF) {
				sprintf(msg, "[mapAdditionalVME] %s: Dynamic mapping failed - %s (%d)", mapDescr->mdName, sys_errlist[errno], errno);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return NULL;
			}
			mapDescr->nofMappings++;
			return (volatile Char_t *) dynamicAddr;
	}
}

Bool_t mapBLT(struct s_mapDescr * mapDescr, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapBLT
// Purpose:        Map BLT address
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
//                 UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
//                 UInt_t AddrMod           -- address modifier
// Results:        TRUE/FALSE
// Description:    Performs mapping to be used with block xfer
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	UInt_t staticBase;
	UInt_t dynamicAddr;

	Char_t bltName[64];

	if (_findMapDescr(mapDescr->mdName) == NULL) {
		sprintf(msg, "[mapBLT] Mapping descriptor not found - %s", mapDescr->mdName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	}
	
	mapDescr->mappingBLT = kVMEMappingUndef;

#ifdef CPU_TYPE_RIO3
	if (mapDescr->mappingModes & kVMEMappingStatic) {
		switch (AddrMod) {
			case kAM_BLT:
				staticBase = kAddr_BLT;
				break;
			case kAM_MBLT:
				staticBase = kAddr_MBLT;
			default:
				sprintf(msg, "[mapBLT] %s: Illegal addr modifier - %#lx", mapDescr->mdName, AddrMod);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				return FALSE;
		}

		sprintf(bltName, "%s_blt", mapDescr->mdName);
		mapDescr->bltBase = smem_create(bltName, (Char_t *) (staticBase | PhysAddr), Size, SM_READ);
		if (mapDescr->bltBase == NULL) {
			sprintf(msg, "[mapBLT] %s: Creating shared segment for BLT failed - %s (%d)", mapDescr->mdName, sys_errlist[errno], errno);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			return FALSE;
		}
		sprintf(msg, "[mapBLT] %s: Static mapping %#lx -> %#lx, addrMod=%#x", mapDescr->mdName, PhysAddr, mapDescr->bltBase, AddrMod);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		mapDescr->mappingBLT = kVMEMappingStatic;
	}
#endif
#ifdef CPU_TYPE_RIO2
	if (mapDescr->mappingModes & kVMEMappingDynamic) {
 		s_param.iack = 1;
 		s_param.rdpref = 0;
 		s_param.wrpost = 0;
 		s_param.swap = SINGLE_AUTO_SWAP;
 		s_param.dum[0] = 0;
 		dynamicAddr = find_controller(PhysAddr, Size, AddrMod, 0, 0, &s_param);
		if (dynamicAddr == 0xFFFFFFFF) {
			sprintf(msg, "[mapBLT] %s: Dynamic mapping failed - %s (%d)", mapDescr->mdName, sys_errlist[errno], errno);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			mapDescr->bltBase = NULL;
		} else {
			mapDescr->bltBase = (volatile Char_t *) dynamicAddr;
			mapDescr->mappingBLT = kVMEMappingStatic;
			sprintf(msg, "[mapBLT] %s: Dynamic mapping %#lx -> %#lx, addrMod=%#x", mapDescr->mdName, PhysAddr, mapDescr->bltBase, AddrMod);
			f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		}
	}
#else
	if (mapDescr->mappingBLT == kVMEMappingUndef) {
		if (mapDescr->mappingModes & kVMEMappingDynamic) {
			dynamicAddr = xvme_map(PhysAddr, Size, AddrMod, 0);
			if (dynamicAddr == -1) {
				sprintf(msg, "[mapBLT] %s: Can't map XVME page", mapDescr->mdName);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
				mapDescr->bltBase = NULL;
			} else {
				mapDescr->bltBase = (volatile Char_t *) dynamicAddr;
				sprintf(msg, "[mapBLT] %s: Dynamic mapping %#lx -> %#lx, addrMod=%#x", mapDescr->mdName, PhysAddr, mapDescr->bltBase, AddrMod);
				f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
			}
		}
	}
#endif

	if (bma_create_mode(&mapDescr->bltModeId) != 0) {
		sprintf(msg, "[mapBLT] %s: Can't create BLT mode struct - %s (%d)", mapDescr->mdName, sys_errlist[errno], errno);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}

	mapDescr->addrModBLT = AddrMod;
	mapDescr->physAddrBLT = PhysAddr;
	mapDescr->segSizeBLT = Size;

	if (mapDescr->bltBase == NULL) {
		sprintf(msg, "[mapBLT] %s: Can't map BLT addr - phys addr %#lx, addrMod=%#lx", mapDescr->mdName, PhysAddr, AddrMod);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return FALSE;
	} else {
		sprintf(msg, "[mapBLT] %s: phys addr %#lx, mapped to BLT addr %#lx, addrMod=%#lx", mapDescr->mdName, PhysAddr, mapDescr->bltBase, AddrMod);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return TRUE;
	}
}

Bool_t setBLTMode(struct s_mapDescr * mapDescr, UInt_t VmeSize, UInt_t WordSize, Bool_t FifoMode) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           setBLTMode
// Purpose:        Set BLT mode
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
//                 UInt_t VmeSize           -- VME size
//                 UInt_t WordSize          -- word size
//                 UInt_t AddrMod           -- address modifier
//                 Bool_t FifoMode          -- TRUE if AutoIncr
// Results:        ---
// Description:    Fills BLT mode struct.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	Int_t sts;

	if ((sts = bma_set_mode(mapDescr->bltModeId, BMA_M_VMESize, VmeSize)) != 0) {
		sprintf(msg, "[setBLTMode] %s: Error while setting BLT mode (VMESize = %d) - error code %d", mapDescr->mdName, VmeSize, sts);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}
	if ((sts = bma_set_mode(mapDescr->bltModeId, BMA_M_WordSize, WordSize)) != 0) {
		sprintf(msg, "[setBLTMode] %s: Error while setting BLT mode (WordSize = %d) - error code %d", mapDescr->mdName, WordSize, sts);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}
	if ((sts = bma_set_mode(mapDescr->bltModeId, BMA_M_AmCode, mapDescr->addrModBLT)) != 0) {
		sprintf(msg, "[setBLTMode] %s: Error while setting BLT mode (AmCode = %#x) - error code %d", mapDescr->mdName, mapDescr->addrModBLT, sts);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#ifndef CPU_TYPE_RIO2
	if ((sts = bma_set_mode(mapDescr->bltModeId, BMA_M_VMEAdrInc, FifoMode ? BMA_M_VaiFifo : BMA_M_VaiNormal)) != 0) {
		sprintf(msg, "[setBLTMode] %s: Error while setting BLT mode (FifoMode = %d) - error code %d", mapDescr->mdName, FifoMode, sts);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	}
#endif
	return TRUE;
}

bool_t initBLT() {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           initBLT
// Purpose:        Initialize block xfer
// Arguments:      ---
// Results:        TRUE/FALSE
// Description:    Calls bma_open() etc
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	Int_t bmaError;

	bmaError = bma_open();
	if (bmaError != 0) {
		sprintf(msg, "[initBLT] Can't initialize BLT - %s (%d)", sys_errlist[errno], errno);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		bma_close();
		return FALSE;
	}
	f_ut_send_msg("m_read_meb", "[initBLT] Block xfer enabled", ERR__MSG_INFO, MASK__PRTT);
	return TRUE;
}

Bool_t unmapVME(struct s_mapDescr * mapDescr) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unmapVME
// Purpose:        Unmap a mapped segment
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
// Results:        TRUE/FALSE
// Description:    Removes a mapped segment, frees memory.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	UInt_t addr;

	if (mapDescr->mappingModes & kVMEMappingStatic) {
		smem_create("", mapDescr->vmeBase, 0, SM_DETACH);
		smem_remove(mapDescr->mdName);
		return TRUE;
	} else if (mapDescr->mappingModes & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO2
		addr = return_controller(mapDescr->vmeBase, mapDescr->segSizeVME);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO3
		addr = return_controller(mapDescr->vmeBase, mapDescr->segSizeVME);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO4
		bus_unmap(mapDescr->busId, mapDescr->vmeBase);
#endif
	}
}

Bool_t unmapBLT(struct s_mapDescr * mapDescr) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unmapBLT
// Purpose:        Unmap a BLT segment
// Arguments:      s_mapDescr * mapDescr    -- map descriptor
// Results:        TRUE/FALSE
// Description:    Removes a BMT segment, frees memory.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	Char_t bltName[64];
	Bool_t sts;
	UInt_t addr;

	if (mapDescr->bltBase == NULL) return kTRUE;

	if (mapDescr->mappingModes & kVMEMappingStatic) {
		smem_create("", mapDescr->bltBase, 0, SM_DETACH);
		sprintf(bltName, "%s_blt", mapDescr->mdName);
		smem_remove(bltName);
		return kTRUE;
	} else if (mapDescr->mappingModes & kVMEMappingDynamic) {
#ifdef CPU_TYPE_RIO2
		addr = return_controller(mapDescr->bltBase, mapDescr->segSizeBLT);
		return (addr != 0xFFFFFFFF);
#elif CPU_TYPE_RIO3
		addr = xvme_rel(mapDescr->bltBase, mapDescr->segSizeBLT);
		return (addr != 0xFFFFFFFF);
#endif
	}
}

Bool_t unmapAll() {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           unmapAll
// Purpose:        Unmap all mappings
// Arguments:      ---
// Results:        TRUE/FALSE
// Description:    Removes shared mem segments, frees memory.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct s_mapDescr * md;
	char bltName[64];
	
	md = firstDescr;
	if (md == NULL) return TRUE;
	
	while (md) {
		unmapVME(md);
		unmapBLT(md);
		md = md->nextDescr;
	}
	return TRUE;
}

struct s_mapDescr * _findMapDescr(const Char_t * DescrName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _findMapDescr
// Purpose:        Find map descriptor
// Arguments:      Char_t * DescrName       -- descriptor name
// Results:        s_mapDescr * Descr       -- pointer to database
// Description:    Searches thru linked list for a givcen descriptor
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct s_mapDescr * md;
	
	md = firstDescr;
	if (md == NULL) return NULL;
	
	while (md) {
		if (strcmp(md->mdName, DescrName) == 0) return(md);
		md = md->nextDescr;
	}
	return NULL;
}

struct s_mapDescr * _createMapDescr(const Char_t * DescrName) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           _createMapDescr
// Purpose:        Create a new descriptor
// Arguments:      Char_t * DescrName       -- descriptor name
// Results:        s_mapDescr * Descr       -- pointer to database
// Description:    Creates an empty descriptor
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct s_mapDescr * md;
	
	if (_findMapDescr(DescrName) != NULL) {
		sprintf(msg, "[_createMapDescr] Can't create descriptor %s - already allocated", DescrName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	md = calloc(1, sizeof(struct s_mapDescr));
	if (md == NULL) {
		sprintf(msg, "[_createMapDescr] Can't allocate descriptor %s - %s (%d)", DescrName,  sys_errlist[errno], errno);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	memset(md, 0, sizeof(struct s_mapDescr));
	strcpy(md->mdName, DescrName);
	
	if (firstDescr == NULL) {
		md->prevDescr = NULL;
		firstDescr = md;
	} else {
		lastDescr->nextDescr = md;
		md->prevDescr = lastDescr;
	}
	md->nextDescr = NULL;
	lastDescr = md;
	return md;
}

Char_t * getPhysAddr(Char_t * Addr, Int_t Size) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           getPhysAddr
// Purpose:        Get pysical address from virtual one
// Arguments:      UInt_t Addr              -- virtual address
//                 Int_t Size               -- size in bytes
// Results:        Int_t Offset             -- offset
// Description:    Converts virtual to physical address.
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct dmaChain {
		ULong_t address;
		Int_t count;
	};

	struct dmaChain chains[10];

	static Int_t addrOffset = 0;
	Int_t error;

	if (vmtopm(getpid(), chains, Addr, Size) == -1) {
		sprintf(msg, "[getPhysAddr] vmtopm call failed");
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	return (Char_t *) chains[0].address;
}

	
