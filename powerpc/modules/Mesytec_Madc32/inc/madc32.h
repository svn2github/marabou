#ifndef __MADC32_H__
#define __MADC32_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <allParam.h>
#include <ces/bmalib.h>

#include "block.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32.h
//! \brief			Definitions for Mesytec Madc32 ADC
//! \details		Contains definitions to operate a Mesytec Madc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.5 $     
//! $Date: 2009-05-06 07:23:45 $
////////////////////////////////////////////////////////////////////////////*/

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			address map
////////////////////////////////////////////////////////////////////////////*/

#define MADC32_DATA						0x0
#define MADC32_THRESHOLD				0x4000

#define MADC32_ADDR_SOURCE 				0x6000
#define MADC32_ADDR_REG					0x6002
#define MADC32_MODULE_ID				0x6004
#define MADC32_SOFT_RESET				0x6008
#define MADC32_FIRMWARE_REV				0x600E

#define MADC32_IRQ_THRESH				0x6018
#define MADC32_MAX_XFER_DATA			0x601A

#define MADC32_BUFFER_DATA_LENGTH		0x6030
#define MADC32_DATA_LENGTH_FORMAT		0x6032
#define MADC32_READOUT_RESET			0x6034
#define MADC32_MULTI_EVENT 				0x6036
#define MADC32_MARKING_TYPE				0x6038
#define MADC32_START_ACQUISITION		0x603A
#define MADC32_FIFO_RESET				0x603C
#define MADC32_DATA_READY				0x603E

#define MADC32_BANK_OPERATION			0x6040
#define MADC32_ADC_RESOLUTION			0x6042
#define MADC32_OUTPUT_FORMAT			0x6044
#define MADC32_ADC_OVERRIDE				0x6046
#define MADC32_SLIDING_SCALE_OFF		0x6048
#define MADC32_SKIP_OUT_OF_RANGE		0x604A

#define MADC32_GG_HOLD_DELAY_0 			0x6050
#define MADC32_GG_HOLD_DELAY_1 			0x6052
#define MADC32_GG_HOLD_WIDTH_0 			0x6054
#define MADC32_GG_HOLD_WIDTH_1 			0x6056
#define MADC32_USE_GATE_GENERATOR		0x6058

#define MADC32_INPUT_RANGE 				0x6060
#define MADC32_ECL_TERMINATORS			0x6062
#define MADC32_ECL_G1_OR_OSC			0x6064
#define MADC32_ECL_FCL_OR_RES_TS		0x6066
#define MADC32_ECL_BUSY					0x6068
#define MADC32_NIM_G1_OR_OSC			0x606A
#define MADC32_NIM_FCL_OR_RES_TS		0x606C
#define MADC32_NIM_BUSY					0x606E

#define MADC32_TEST_PULSER_STATUS		0x6070

#define MADC32_CTRA_RESET_A_OR_B		0x6090
#define MADC32_CTRA_EVENT_LOW			0x6092
#define MADC32_CTRA_EVENT_HIGH 			0x6094
#define MADC32_CTRA_TS_SOURCE			0x6096
#define MADC32_CTRA_TS_DIVISOR	 		0x6098
#define MADC32_CTRA_TS_CTR_LOW 			0x609C
#define MADC32_CTRA_TS_CTR_HIGH			0x609E

#define MADC32_CTRB_ADC_BUSY_LOW		0x60A0
#define MADC32_CTRB_ADC_BUSY_HIGH		0x60A2
#define MADC32_CTRB_G1_LOW				0x60A4
#define MADC32_CTRB_G1_HIGH				0x60A6
#define MADC32_CTRB_TIME_0 				0x60A8
#define MADC32_CTRB_TIME_1 				0x60AA
#define MADC32_CTRB_TIME_2 				0x60AC
#define MADC32_CTRB_STOP	 			0x60AE

#define MADC32_THRESHOLD_DEFAULT		0
#define MADC32_THRESHOLD_MASK			0xFFF

#define MADC32_ADDR_SOURCE_BOARD 		0
#define MADC32_ADDR_SOURCE_REG 			1
#define MADC32_ADDR_SOURCE_DEFAULT 		MADC32_ADDR_SOURCE_BOARD
#define MADC32_ADDR_SOURCE_MASK 		MADC32_ADDR_SOURCE_REG

#define MADC32_MODULE_ID_MASK			0xFF
#define MADC32_MODULE_ID_DEFAULT		0xFF

#define MADC32_BUFFER_DATA_LENGTH_MASK 0x3FFF

#define MADC32_DATA_LENGTH_FORMAT_8			0
#define MADC32_DATA_LENGTH_FORMAT_16		1
#define MADC32_DATA_LENGTH_FORMAT_32		2
#define MADC32_DATA_LENGTH_FORMAT_64		3
#define MADC32_DATA_LENGTH_FORMAT_DEFAULT	MADC32_DATA_LENGTH_FORMAT_32
#define MADC32_DATA_LENGTH_FORMAT_MASK		MADC32_DATA_LENGTH_FORMAT_64

#define MADC32_MULTI_EVENT_NO			0
#define MADC32_MULTI_EVENT_YES			0x1
#define MADC32_MULTI_EVENT_BERR 	 	0x2
#define MADC32_MULTI_EVENT_LIM_XFER 	0x3
#define MADC32_MULTI_EVENT_DEFAULT		MADC32_MULTI_EVENT_NO
#define MADC32_MULTI_EVENT_MASK			MADC32_MULTI_EVENT_LIM_XFER

#define MADC32_MAX_XFER_DATA_DEFAULT	0
#define MADC32_MAX_XFER_DATA_MASK		0x3FF

#define MADC32_MARKING_TYPE_EVT			0x0
#define MADC32_MARKING_TYPE_TS 			0x1
#define MADC32_MARKING_TYPE_XTS 		0x3
#define MADC32_MARKING_TYPE_DEFAULT 	MADC32_MARKING_TYPE_EVT
#define MADC32_MARKING_TYPE_MASK 		MADC32_MARKING_TYPE_XTS

#define MADC32_BANK_OPERATION_CON		0
#define MADC32_BANK_OPERATION_INDEP 1
#define MADC32_BANK_OPERATION_TOGGLE	3
#define MADC32_BANK_OPERATION_DEFAULT	MADC32_BANK_OPERATION_CON
#define MADC32_BANK_OPERATION_MASK  	MADC32_BANK_OPERATION_TOGGLE

#define MADC32_ADC_RESOLUTION_2K		0
#define MADC32_ADC_RESOLUTION_4K		1
#define MADC32_ADC_RESOLUTION_4K_HIRES	2
#define MADC32_ADC_RESOLUTION_8K		3
#define MADC32_ADC_RESOLUTION_8K_HIRES	4
#define MADC32_ADC_RESOLUTION_DEFAULT	MADC32_ADC_RESOLUTION_4K_HIRES
#define MADC32_ADC_RESOLUTION_MASK		0x7

#define MADC32_ADC_OVERWRITE_DEFAULT	5

#define MADC32_OUTPUT_FORMAT_MESY		0
#define MADC32_OUTPUT_FORMAT_RESRVD 	1
#define MADC32_OUTPUT_FORMAT_DEFAULT	MADC32_OUTPUT_FORMAT_MESY
#define MADC32_OUTPUT_FORMAT_MASK		MADC32_OUTPUT_FORMAT_RESRVD

#define MADC32_SLIDING_SCALE_OFF_MASK	0x1
#define MADC32_SKIP_OUT_OF_RANGE_MASK	0x1

#define MADC32_GG_HOLD_DELAY_DEFAULT	20
#define MADC32_GG_HOLD_DELAY_MASK		0xFF
#define MADC32_GG_HOLD_WIDTH_DEFAULT	50
#define MADC32_GG_HOLD_WIDTH_MASK		0xFF

#define MADC32_USE_GATE_GENERATOR_0 		0x1
#define MADC32_USE_GATE_GENERATOR_1 		0x2
#define MADC32_USE_GATE_GENERATOR_BOTH		(MADC32_USE_GATE_GENERATOR_0 | MADC32_USE_GATE_GENERATOR_1)
#define MADC32_USE_GATE_GENERATOR_DEFAULT	MADC32_USE_GATE_GENERATOR_0
#define MADC32_USE_GATE_GENERATOR_MASK		MADC32_USE_GATE_GENERATOR_BOTH

#define MADC32_INPUT_RANGE_4V				0
#define MADC32_INPUT_RANGE_10V				1
#define MADC32_INPUT_RANGE_8V				2
#define MADC32_INPUT_RANGE_DEFAULT 			MADC32_INPUT_RANGE_4V
#define MADC32_INPUT_RANGE_MASK 			0x3

#define MADC32_ECL_TERMINATORS_G0			0x1
#define MADC32_ECL_TERMINATORS_G1			0x2
#define MADC32_ECL_TERMINATORS_BUSY			0x4
#define MADC32_ECL_TERMINATORS_FCL			0x8
#define MADC32_ECL_TERMINATORS_DEFAULT  	(MADC32_ECL_TERMINATORS_G0 | MADC32_ECL_TERMINATORS_G1 | MADC32_ECL_TERMINATORS_BUSY | MADC32_ECL_TERMINATORS_FCL)
#define MADC32_ECL_TERMINATORS_MASK 		MADC32_ECL_TERMINATORS_DEFAULT

#define MADC32_ECL_INPUT_G1					0
#define MADC32_ECL_INPUT_OSC				1
#define MADC32_ECL_G1_OR_OSC_DEFAULT		MADC32_ECL_INPUT_G1
#define MADC32_ECL_G1_OR_OSC_MASK			MADC32_ECL_INPUT_OSC

#define MADC32_ECL_INPUT_FCL				0
#define MADC32_ECL_INPUT_RES_TS				1
#define MADC32_ECL_FCL_OR_RES_TS_DEFAULT	MADC32_ECL_INPUT_FCL
#define MADC32_ECL_FCL_OR_RES_TS_MASK		MADC32_ECL_INPUT_RES_TS

#define MADC32_NIM_INPUT_G1					0
#define MADC32_NIM_INPUT_OSC				1
#define MADC32_NIM_G1_OR_OSC_DEFAULT		MADC32_NIM_INPUT_G1
#define MADC32_NIM_G1_OR_OSC_MASK			MADC32_NIM_INPUT_OSC

#define MADC32_NIM_INPUT_FCL				0
#define MADC32_NIM_INPUT_RES_TS				1
#define MADC32_NIM_FCL_OR_RES_TS_DEFAULT	MADC32_NIM_INPUT_FCL
#define MADC32_NIM_FCL_OR_RES_TS_MASK		MADC32_NIM_INPUT_RES_TS

#define MADC32_NIM_BUSY_BUSY				0
#define MADC32_NIM_BUSY_G0					1
#define MADC32_NIM_BUSY_G1					2
#define MADC32_NIM_BUSY_CBUS				3
#define MADC32_NIM_BUSY_DEFAULT				MADC32_NIM_BUSY_BUSY
#define MADC32_NIM_BUSY_MASK				MADC32_NIM_BUSY_CBUS

#define MADC32_TEST_PULSER_OFF				0
#define MADC32_TEST_PULSER_A0				0x4
#define MADC32_TEST_PULSER_ALOW				0x5
#define MADC32_TEST_PULSER_AHIGH			0x6
#define MADC32_TEST_PULSER_A0LH0			0x7
#define MADC32_TEST_PULSER_STATUS_DEFAULT	MADC32_TEST_PULSER_OFF
#define MADC32_TEST_PULSER_STATUS_MASK		MADC32_TEST_PULSER_A0LH0

#define MADC32_CTRA_TS_SOURCE_VME			0
#define MADC32_CTRA_TS_SOURCE_EXT			0x1
#define MADC32_CTRA_TS_SOURCE_XRES_ENA		0x2
#define MADC32_CTRA_TS_SOURCE_DEFAULT		MADC32_CTRA_TS_SOURCE_VME
#define MADC32_CTRA_TS_SOURCE_MASK			0x3

#define MADC32_CTRA_TS_DIVISOR_DEFAULT 		1
#define MADC32_CTRA_TS_DIVISOR_MASK 		0xFFFF

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			data structure for Madc32
////////////////////////////////////////////////////////////////////////////*/


#define NOF_CHANNELS	32

struct s_madc32 {
	unsigned long vmeAddr;					/* phys addr given by module switches */
	volatile unsigned char * baseAddr;		/* addr mapped via find_controller() */

	char moduleName[100];
	int serial; 							/* MARaBOU's serial number */

	bool_t verbose;
	bool_t dumpRegsOnInit;

	bool_t updSettings;
	int updInterval;
	int updCountDown;

	uint16_t threshold[NOF_CHANNELS];
	uint16_t addrSource;
	uint16_t addrReg;
	uint16_t moduleId;
	uint16_t fifoLength;
	uint16_t dataWidth;
	uint16_t xferData;
	uint16_t multiEvent;
	uint16_t markingType;
	uint16_t bankOperation;
	uint16_t adcResolution;
	uint16_t outputFormat;
	uint16_t adcOverride;
	bool_t slidingScaleOff;
	bool_t skipOutOfRange;
	uint16_t ggHoldDelay[2];
	uint16_t ggHoldWidth[2];
	uint16_t useGG;
	uint16_t inputRange;
	uint16_t eclTerm;
	uint16_t eclG1OrOsc;
	uint16_t eclFclOrRts;
	uint16_t nimG1OrOsc;
	uint16_t nimFclOrRts;
	uint16_t nimBusy;
	uint16_t testPulserStatus;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;

	struct s_bma * bma; 					/* block mode access */
	uint32_t bltBufferSize;
	uint32_t bltBlockSize;
	uint8_t * bltBuffer;
};

#endif
