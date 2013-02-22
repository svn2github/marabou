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

struct s_mapDescr * mapVME(const Char_t * DescrName, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, UInt_t Mapping);

Bool_t mapBLT(struct s_mapDescr * mapDescr, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod);

Bool_t remapVME(struct s_mapDescr * mapDescr, UInt_t Offset, Int_t Size);

Bool_t unmapVME(struct s_mapDescr * mapDescr);

Bool_t unmapAll();

Bool_t initBLT();

Bool_t setBLTMode(struct s_mapDescr * mapDescr, UInt_t VmeSize, UInt_t WordSize, Bool_t FifoMode);

Char_t * getPhysAddr(Char_t * Addr, Int_t Size);

void printMapping();

struct s_mapDescr * _findMapDescr(const Char_t * DescrName);
struct s_mapDescr * _createMapDescr(const Char_t * DescrName);

extern int vmtopm (int, struct dmachain *, char *, long);

#endif
