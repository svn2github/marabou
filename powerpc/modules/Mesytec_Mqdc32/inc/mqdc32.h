#ifndef __MQDC32_H__
#define __MQDC32_H__

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
//! \file			mqdc32.h
//! \brief			Definitions for Mesytec Madc32 ADC
//! \details		Contains definitions to operate a Mesytec Madc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.15 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/

/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			address map
////////////////////////////////////////////////////////////////////////////*/

#define MQDC32_DATA						0x0
#define MQDC32_THRESHOLD				0x4000

#define MQDC32_ADDR_SOURCE 				0x6000
#define MQDC32_ADDR_REG					0x6002
#define MQDC32_MODULE_ID				0x6004
#define MQDC32_SOFT_RESET				0x6008
#define MQDC32_FIRMWARE_REV				0x600E

#define MQDC32_IRQ_THRESH				0x6018
#define MQDC32_MAX_XFER_DATA			0x601A

#define MQDC32_BUFFER_DATA_LENGTH		0x6030
#define MQDC32_DATA_LENGTH_FORMAT		0x6032
#define MQDC32_READOUT_RESET			0x6034
#define MQDC32_MULTI_EVENT 				0x6036
#define MQDC32_MARKING_TYPE				0x6038
#define MQDC32_START_ACQUISITION		0x603A
#define MQDC32_FIFO_RESET				0x603C
#define MQDC32_DATA_READY				0x603E

#define MQDC32_BANK_OPERATION			0x6040
#define MQDC32_ADC_RESOLUTION			0x6042
#define MQDC32_OFFSET_BANK_0			0x6044
#define MQDC32_OFFSET_BANK_1			0x6046
#define MQDC32_SLIDING_SCALE_OFF		0x6048
#define MQDC32_SKIP_OUT_OF_RANGE		0x604A
#define MQDC32_IGNORE_THRESHOLDS		0x604C

#define MQDC32_GATE_LIMIT_BANK_0 		0x6050
#define MQDC32_GATE_LIMIT_BANK_1		0x6052

#define MQDC32_INPUT_COUPLING 			0x6060
#define MQDC32_ECL_TERMINATORS			0x6062
#define MQDC32_ECL_G1_OR_OSC			0x6064
#define MQDC32_ECL_FCL_OR_RES_TS		0x6066
#define MQDC32_GATE_SELECT				0x6068
#define MQDC32_NIM_G1_OR_OSC			0x606A
#define MQDC32_NIM_FCL_OR_RES_TS		0x606C
#define MQDC32_NIM_BUSY					0x606E

#define MQDC32_TEST_PULSER_STATUS		0x6070
#define MQDC32_TEST_PULSER_DAC			0x6072

#define MQDC32_CTRA_RESET_A_OR_B		0x6090
#define MQDC32_CTRA_EVENT_LOW			0x6092
#define MQDC32_CTRA_EVENT_HIGH 			0x6094
#define MQDC32_CTRA_TS_SOURCE			0x6096
#define MQDC32_CTRA_TS_DIVISOR	 		0x6098
#define MQDC32_CTRA_TS_CTR_LOW 			0x609C
#define MQDC32_CTRA_TS_CTR_HIGH			0x609E

#define MQDC32_CTRB_ADC_BUSY_LOW		0x60A0
#define MQDC32_CTRB_ADC_BUSY_HIGH		0x60A2
#define MQDC32_CTRB_G1_LOW				0x60A4
#define MQDC32_CTRB_G1_HIGH				0x60A6
#define MQDC32_CTRB_TIME_0 				0x60A8
#define MQDC32_CTRB_TIME_1 				0x60AA
#define MQDC32_CTRB_TIME_2 				0x60AC
#define MQDC32_CTRB_STOP	 			0x60AE

#define MQDC32_THRESHOLD_DEFAULT		0
#define MQDC32_THRESHOLD_MASK			0xFFF

#define MQDC32_ADDR_SOURCE_BOARD 		0
#define MQDC32_ADDR_SOURCE_REG 			1
#define MQDC32_ADDR_SOURCE_DEFAULT 		MQDC32_ADDR_SOURCE_BOARD
#define MQDC32_ADDR_SOURCE_MASK 		MQDC32_ADDR_SOURCE_REG

#define MQDC32_MODULE_ID_MASK			0xFF
#define MQDC32_MODULE_ID_DEFAULT		0xFF

#define MQDC32_BUFFER_DATA_LENGTH_MASK 0x3FFF

#define MQDC32_DATA_LENGTH_FORMAT_8			0
#define MQDC32_DATA_LENGTH_FORMAT_16		1
#define MQDC32_DATA_LENGTH_FORMAT_32		2
#define MQDC32_DATA_LENGTH_FORMAT_64		3
#define MQDC32_DATA_LENGTH_FORMAT_DEFAULT	MQDC32_DATA_LENGTH_FORMAT_32
#define MQDC32_DATA_LENGTH_FORMAT_MASK		MQDC32_DATA_LENGTH_FORMAT_64

#define MQDC32_MULTI_EVENT_NO			0
#define MQDC32_MULTI_EVENT_YES			0x1
#define MQDC32_MULTI_EVENT_BERR 	 	0x4
#define MQDC32_MULTI_EVENT_LIM_XFER 	(MQDC32_MULTI_EVENT_YES | 0x2)
#define MQDC32_MULTI_EVENT_DEFAULT		MQDC32_MULTI_EVENT_NO
#define MQDC32_MULTI_EVENT_MASK			0xF

#define MQDC32_MAX_XFER_DATA_DEFAULT	0
#define MQDC32_MAX_XFER_DATA_MASK		0x3FF

#define MQDC32_MARKING_TYPE_EVT			0x0
#define MQDC32_MARKING_TYPE_TS 			0x1
#define MQDC32_MARKING_TYPE_XTS 		0x3
#define MQDC32_MARKING_TYPE_DEFAULT 	MQDC32_MARKING_TYPE_EVT
#define MQDC32_MARKING_TYPE_MASK 		MQDC32_MARKING_TYPE_XTS

#define MQDC32_BANK_OPERATION_CON		0
#define MQDC32_BANK_OPERATION_INDEP 	1
#define MQDC32_BANK_OPERATION_DEFAULT	MQDC32_BANK_OPERATION_CON
#define MQDC32_BANK_OPERATION_MASK  	MQDC32_BANK_OPERATION_INDEP

#define MQDC32_ADC_RESOLUTION_4K		0
#define MQDC32_ADC_RESOLUTION_DEFAULT	MQDC32_ADC_RESOLUTION_4K
#define MQDC32_ADC_RESOLUTION_MASK		0x1

#define MQDC32_BANK_OFFSET_DEFAULT		128
#define MQDC32_BANK_OFFSET_MASK			0xFF

#define MQDC32_GATE_LIMIT_DEFAULT		255
#define MQDC32_GATE_LIMIT_MASK			0xFF

#define MQDC32_SLIDING_SCALE_OFF_MASK	0x1
#define MQDC32_SKIP_OUT_OF_RANGE_MASK	0x1
#define MQDC32_IGNORE_THRESH_MASK		0x1

#define MQDC32_INPUT_COUPLING_AC			0
#define MQDC32_INPUT_COUPLING_DC			1
#define MQDC32_INPUT_COUPLING_DEFAULT 		MQDC32_INPUT_COUPLING_AC
#define MQDC32_INPUT_COUPLING_MASK 			MQDC32_INPUT_COUPLING_DC

#define MQDC32_ECL_TERMINATORS_G0			0x1
#define MQDC32_ECL_TERMINATORS_G1			0x2
#define MQDC32_ECL_TERMINATORS_FCL			0x4
#define MQDC32_ECL_TERMINATORS_BANK0_INDIV	0x8
#define MQDC32_ECL_TERMINATORS_BANK1_INDIV	0x10
#define MQDC32_ECL_TERMINATORS_DEFAULT  	(MQDC32_ECL_TERMINATORS_BANK0_INDIV|MQDC32_ECL_TERMINATORS_BANK1_INDIV)
#define MQDC32_ECL_TERMINATORS_MASK 		0x1F

#define MQDC32_ECL_INPUT_G1					0
#define MQDC32_ECL_INPUT_OSC				1
#define MQDC32_ECL_G1_OR_OSC_DEFAULT		MQDC32_ECL_INPUT_G1
#define MQDC32_ECL_G1_OR_OSC_MASK			MQDC32_ECL_INPUT_OSC

#define MQDC32_ECL_INPUT_FCL				0
#define MQDC32_ECL_INPUT_RES_TS				1
#define MQDC32_ECL_FCL_OR_RES_TS_DEFAULT	MQDC32_ECL_INPUT_FCL
#define MQDC32_ECL_FCL_OR_RES_TS_MASK		MQDC32_ECL_INPUT_RES_TS

#define MQDC32_GATE_SELECT_NIM				0
#define MQDC32_GATE_SELECT_ECL				1
#define MQDC32_GATE_SELECT_DEFAULT			MQDC32_GATE_SELECT_NIM
#define MQDC32_GATE_SELECT_MASK				MQDC32_GATE_SELECT_ECL

#define MQDC32_NIM_INPUT_G1					0
#define MQDC32_NIM_INPUT_OSC				1
#define MQDC32_NIM_G1_OR_OSC_DEFAULT		MQDC32_NIM_INPUT_G1
#define MQDC32_NIM_G1_OR_OSC_MASK			MQDC32_NIM_INPUT_OSC

#define MQDC32_NIM_INPUT_FCL				0
#define MQDC32_NIM_INPUT_RES_TS				1
#define MQDC32_NIM_FCL_OR_RES_TS_DEFAULT	MQDC32_NIM_INPUT_FCL
#define MQDC32_NIM_FCL_OR_RES_TS_MASK		MQDC32_NIM_INPUT_RES_TS

#define MQDC32_NIM_BUSY_BUSY				0
#define MQDC32_NIM_BUSY_G0					1
#define MQDC32_NIM_BUSY_G1					2
#define MQDC32_NIM_BUSY_CBUS				3
#define MQDC32_NIM_BUSY_BUFFER_FULL			4
#define MQDC32_NIM_BUSY_ABOVE_THRESH		8
#define MQDC32_NIM_BUSY_DEFAULT				MQDC32_NIM_BUSY_BUSY
#define MQDC32_NIM_BUSY_MASK				MQDC32_NIM_BUSY_CBUS

#define MQDC32_TEST_PULSER_OFF				0
#define MQDC32_TEST_PULSER_A0				4
#define MQDC32_TEST_PULSER_ALOW				5
#define MQDC32_TEST_PULSER_AHIGH			6
#define MQDC32_TEST_PULSER_A0LH0			7
#define MQDC32_TEST_PULSER_STATUS_DEFAULT	MQDC32_TEST_PULSER_OFF
#define MQDC32_TEST_PULSER_STATUS_MASK		MQDC32_TEST_PULSER_A0LH0

#define MQDC32_CTRA_TS_SOURCE_VME			0
#define MQDC32_CTRA_TS_SOURCE_EXT			0x1
#define MQDC32_CTRA_TS_SOURCE_XRES_ENA		0x2
#define MQDC32_CTRA_TS_SOURCE_DEFAULT		MQDC32_CTRA_TS_SOURCE_VME
#define MQDC32_CTRA_TS_SOURCE_MASK			0x3

#define MQDC32_CTRA_TS_DIVISOR_DEFAULT 		1
#define MQDC32_CTRA_TS_DIVISOR_MASK 		0xFFFF

#define MQDC32_D_CHAN_INACTIVE				0x1FFF

#define MQDC32_M_SIGNATURE					0xC0000000
#define MQDC32_M_HEADER						0x40000000
#define MQDC32_M_TRAILER					0xC0000000
#define MQDC32_M_EOB						0x80000000
#define MQDC32_M_WC							0x00000FFF

#define MQDC32_BLT_OFF						0
#define MQDC32_BLT_NORMAL					1
#define MQDC32_BLT_CHAINED					2


/*____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//! \brief			data structure for Madc32
////////////////////////////////////////////////////////////////////////////*/


#define NOF_CHANNELS	32

struct dmachain {
        void *address;
        int count;
};

struct s_mqdc32 {
	unsigned long vmeAddr;					/* phys addr given by module switches */
	volatile unsigned char * baseAddr;		/* addr mapped via find_controller() */

	char moduleName[100];
	char prefix[100];						/* "m_read_meb" (default) or any other */
	char mpref[10]; 						/* "mqdc32: " or "" */

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
	uint16_t bankOffset[2];
	uint16_t gateLimit[2];
	bool_t slidingScaleOff;
	bool_t skipOutOfRange;
	bool_t ignoreThresh;
	uint16_t inputCoupling;
	uint16_t eclTerm;
	uint16_t eclG1OrOsc;
	uint16_t eclFclOrRts;
	uint16_t gateSelect;
	uint16_t nimG1OrOsc;
	uint16_t nimFclOrRts;
	uint16_t nimBusy;
	uint16_t testPulserStatus;
	uint16_t ctraTsSource;
	uint16_t ctraTsDivisor;

	int memorySize;

	struct s_bma * bma; 					/* block mode access */
	uint32_t bltBufferSize;
	uint32_t bltBlockSize;
	uint8_t * bltBuffer;
	uint32_t bltDestination;
	int blockXfer;

	uint32_t evtBuf[NOF_CHANNELS + 3];		/* 1 event = 32 channels + header + extended timestamp + trailer */
	uint32_t *evtp;
	bool_t skipData;
};

#endif
