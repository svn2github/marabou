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
////////////////////////////////////////////////////////////////////////////*/

#include "mapping_database.h"
#include <errno.h>

/*________________________________________________________________[PROTOTYPES]
////////////////////////////////////////////////////////////////////////////*/
Char_t * strcmp(Char_t *, Char_t *);
Char_t * calloc(Int_t, Int_t);

/*___________________________________________________________________[GLOBALS]
////////////////////////////////////////////////////////////////////////////*/
static struct s_mapDescr * firstDescr = NULL;
static struct s_mapDescr * lastDescr = NULL;

Char_t msg[256];


struct s_mapDescr * mapVME(const Char_t * DescrName, UInt_t PhysAddr, Int_t Size, UInt_t AddrMod, Bool_t StaticFlag) {
/*________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mapVME
// Purpose:        Map VME address
// Arguments:      Char_t * DescrName       -- descriptor name
//                 UInt_t PhysAddr          -- physical address
//                 Int_t Size               -- segment size
//                 UInt_t AddrMod           -- address modifier
//                 Bool_t StaticFlag        -- TRUE if static mapping
// Results:        s_mapDescr * Descr       -- pointer to database
// Description:    Performs VME mapping, either statically or dynamically
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

	struct s_mapDescr * md;
	
	if ((md = _createMapDescr(DescrName)) == NULL) {
		sprintf(msg, "[mapVME] %s: mapping failed", DescrName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	if (StaticFlag) {
	} else {
	}
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
		if (strcmp(md->name, DescrName) == 0) return(md);
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
	
	strcpy(md->name, DescrName);
	
	if (firstDescr == NULL) {
		md->prevDescr = NULL;
		md->nextDescr = NULL;
		firstDescr = md;
		lastDescr = md;
		return(md);
	} else {
		md->prevDescr = lastDescr;
		md->nextDescr = NULL;
		lastDescr = md;
		return md;
	}
}
	