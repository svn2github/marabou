#ifndef __MESY_DATABASE_H__
#define __MESY_DATABASE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <ces/uiocmd.h>
#include <ces/bmalib.h>

#include "block.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mesy_database.h
//! \brief			Common database for Mesytec modules
//! \details		Structure describing common elements of mesytec modules
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/


typedef struct {
	char moduleName[100];
	char prefix[100];			/* "m_read_meb" (default) or any other */
	char mpref[10];

	s_mapDescr * md;			/* mapping descriptor */

	int serial; 				/* MARaBOU's serial number */

	bool_t verbose;				/* TRUE if verbose mode */
	bool_t dumpRegsOnInit;		/* TRUE if dump of registers required */

	int memorySize;				/* module's memory size */

	bool_t blockXfer;			/* TRUE if block xfer enabled */
	bool_t repairRawData;		/* TRUE if raw data should be repaired (missing EOEs) */

	int16_t reportReadErrors;	/* number of errors to be reported */
	int16_t nofReadErrors;		/* actual number of errors */
	int16_t nofReads;			/* number of read requests */
	
	unsigned long mcstSignature;	/* MCST signature */
	volatile char * mcstAddr;		/* ... after mapping */
	bool_t mcstMaster;				/* TRUE if MCST master */
	unsigned long cbltSignature;	/* CBLT signature */
	volatile char * cbltAddr;		/* ... after mapping */
	bool_t firstInCbltChain;		/* TRUE if head of CBLT chain */
	bool_t lastInCbltChain;			/* TRUE if end of CBLT chain */
	
/* common registers for all Mesytec modules */
	uint16_t addrSource;
	uint16_t addrReg;
	uint16_t moduleId;
	uint16_t dataWidth;
	uint16_t xferData;
	uint16_t multiEvent;
	uint16_t markingType;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;
} s_mesy;

#endif

