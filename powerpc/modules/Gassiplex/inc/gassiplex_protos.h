#ifndef __GASSIPLEX_PROTOS_H__
#define __GASSIPLEX_PROTOS_H__

#include "mapping_database.h"
#include "gassiplex_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			gassiplex_protos.h
//! \brief			Definitions for GASSIPLEX
//! \details		Prototypes for GASSIPLEX
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Id$
//! $Date$
////////////////////////////////////////////////////////////////////////////*/

struct s_mapDescr * mapGassiplex(char * moduleName, uint32_t hdtuAddr, int addrMod);

void gassiplex_initialize(struct s_gassiplex * s);

void gassiplex_startAcq(struct s_gassiplex * s);
void gassiplex_stopAcq(struct s_gassiplex * s);

int gassiplex_readout(struct s_gassiplex * s, uint32_t * pointer);
#endif
