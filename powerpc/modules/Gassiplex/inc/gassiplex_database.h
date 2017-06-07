#ifndef __GASSIPLEX_DATABASE_H__
#define __GASSIPLEX_DATABASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"
#include "mapping_database.h"
#include "hdtu.h"
#include "hrace.h"


/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			gassiplex_database.h
//! \brief			Database for GASSIPLEX
//! \details		Structure describing GASSIPLEX
//! $Author$
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Id$
//! $Date$
////////////////////////////////////////////////////////////////////////////*/


typedef struct {
	char moduleName[100];

	s_mapDescr * md;			/* mapping descriptor */
	HDTU * hdtu;						/* pointer to dtu struct */
	HRace * hrace;						/* ... to race struct */

	bool_t verbose;
} s_gassiplex;

#endif

