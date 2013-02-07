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

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			Mapping_Database.h
//! \brief			Definition for VME/BLT mapping
//! \details		Structures describing vme mapping
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/


struct s_mapDb {
	Char_t name[STRLEN];					/* name, to be used with smem_create() etc */

	Bool_t staticMapping;					/* TRUE or FALSE */

	UInt_t addrModVME;						/* address modifier */
	UInt_t physAddrVME;						/* phys addr given by module switches */
	volatile Char_t * vmeBase;				/* mapped address, static or dynamic mapping */
	Int_t segSizeVME;						/* segment size */

	UInt_t addrModBLT;						/* address modifier */
	UInt_t physAddrBLT;						/* phys addr given by module switches */
	volatile Char_t * bltBase;				/* mapped address */
	Int_t segSizeBLT;						/* segment size */
	UInt_t modeId;							/* id from bma_create_mode() */

	void * prev;							/* linked list */
	void * next;

};
#endif