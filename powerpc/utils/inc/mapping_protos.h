#ifndef __MAPPING_PROTOS_H__
#define __MAPPING_PROTOS_H__

#include "LwrTypes.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mapping_protos.h
//! \brief			Definition for VME/BLT mapping
//! \details		Prototypes
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

s_mapDescr * mapVME(const Char_t * DescrName, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, UInt_t VMEMapping, UInt_t BLTMapping);

volatile Char_t * mapAdditionalVME(s_mapDescr * mapDescr, UInt_t PhysAddr, Int_t Size);

Bool_t mapBLT(s_mapDescr * mapDescr, UInt_t AddrMod);

Bool_t remapVME(s_mapDescr * mapDescr, UInt_t Offset, Int_t Size);

Bool_t unmapVME(s_mapDescr * mapDescr);
Bool_t unmapBLT(s_mapDescr * mapDescr);

Bool_t unmapAll();

Bool_t initBLT();

Bool_t setBLTMode(s_mapDescr * mapDescr, UInt_t VmeSize, UInt_t WordSize, Bool_t FifoMode);

Char_t * getPhysAddr(Char_t * Addr, Int_t Size);

void printMapping();

s_mapDescr * _findMapDescr(const Char_t * DescrName);
s_mapDescr * _createMapDescr(const Char_t * DescrName);
#endif
