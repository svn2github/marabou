/*_______________________________________________________[C LIBRARY FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
//! \file			gassiplex.c
//! \brief			Code for module GASSIPLEX
//! \details		Implements functions to handle modules of type GASSIPLEX
//!
//! $Author$
//! $Mail			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Id$
//! $Date$
////////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allParam.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>
#include <errno.h>
#include <sigcodes.h>

#include "gd_readout.h"

#include "gassiplex_database.h"
#include "gassiplex_protos.h"

#include "root_env.h"
#include "mapping_database.h"

#include "err_mask_def.h"
#include "errnum_def.h"

#define RACE_OFFSET	0x8000000

char msg[256];

s_mapDescr * mapGassiplex(char * moduleName, uint32_t physAddr, int addrMod) {
	s_mapDescr * md;
	s_gassiplex * gs;

	gs = (s_gassiplex *) calloc(1, sizeof(s_gassiplex));
	
	if ((md = _createMapDescr(moduleName)) == NULL) {
		sprintf(msg, "[mapGassiplex] %s: creating mapping struct failed", moduleName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	if (addrMod != 0x9) {
		sprintf(msg, "[mapGassiplex] %s: Wrong addr modfier - %#x (should be 0x9)", moduleName, addrMod);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	gs->hdtu = newHDTU("dtu", physAddr);
	if (gs->hdtu == NULL) {
		sprintf(msg, "[mapGassiplex] %s: mapping failed [hdtu]", moduleName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	gs->hrace = newHRace(physAddr + RACE_OFFSET, 0);
	if (gs->hrace == NULL) {
		sprintf(msg, "[mapGassiplex] %s: mapping failed [hrace]", moduleName);
		f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
		return NULL;
	}
	
	strcpy(gs->moduleName, moduleName);
	gs->verbose = FALSE;
	gs->md = md;

	md->mappingVME = kVMEMappingPrivate;
	md->addrModVME = 0x9;
	md->physAddrVME = physAddr;
	md->vmeBase = gs->hdtu->lvme->virtBase;
	md->handle = (void *) gs;
	
	sprintf(msg, "[mapGassiplex] %s: hdtu mapped to addr %#lx, hrace mapped to addr %#lx\n", gs->moduleName, gs->hdtu->lvme->virtBase, gs->hrace->regRegion->virtBase);
	f_ut_send_msg("m_read_meb", msg, ERR__MSG_INFO, MASK__PRTT);
	
	return md;
}

void gassiplex_initialize(s_gassiplex * s)
{
	HDTU_initDefaults(s->hdtu);
}

void gassiplex_startAcq(s_gassiplex * s) {
	HDTU_reset(s->hdtu);
	HDTU_arm(s->hdtu);
}

void gassiplex_stopAcq(s_gassiplex * s) {}

int gassiplex_readout(s_gassiplex * s, uint32_t * pointer) {
	int nofWords;
	
	HDTU_waitData(s->hdtu);
	HDTU_arm(s->hdtu);
	nofWords = HRace_readSubEvt(s->hrace, pointer);
	HDTU_release(s->hdtu);
	return nofWords;
}


