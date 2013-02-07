#ifndef __MAPPING_PROTOS_H__
#define __MAPPING_PROTOS_H__

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

struct s_mapDb * mapVME(const Char_t * Name, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, Bool_t StaticFlag);

Bool_t mapBLT(struct s_mapDb * mapDb, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod);

Bool_t remapVME(struct s_mapDb * mapDb, UInt_t Offset, Int_t Size);

Bool_t unmapVME(struct s_mapDb * mapDb);

Bool_t unmapAll();

#endif
