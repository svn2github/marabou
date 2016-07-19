#ifndef __MTDC32_H__
#define __MTDC32_H__

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			madc32.h
//! \brief			Definitions for Mesytec Mtdc32 TDC
//! \details		Contains definitions to operate a Mesytec Mtdc32
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.15 $
//! $Date: 2011-04-29 07:19:03 $
////////////////////////////////////////////////////////////////////////////*/

#define MTDC32_DATA						0x0
#define MTDC32_THRESHOLD				0x4000

#define MTDC32_ADDR_SOURCE 				0x6000
#define MTDC32_ADDR_REG					0x6002
#define MTDC32_MODULE_ID				0x6004
#define MTDC32_SOFT_RESET				0x6008
#define MTDC32_FIRMWARE_REV				0x600E

#define MTDC32_MAX_XFER_DATA			0x601A

#define MTDC32_CBLT_MCST_CONTROL		0x6020
#define MTDC32_CBLT_ADDRESS				0x6022
#define MTDC32_MCST_ADDRESS				0x6024

#define MTDC32_BUFFER_DATA_LENGTH		0x6030
#define MTDC32_DATA_LENGTH_FORMAT		0x6032
#define MTDC32_READOUT_RESET			0x6034
#define MTDC32_MULTI_EVENT 				0x6036
#define MTDC32_MARKING_TYPE				0x6038
#define MTDC32_START_ACQUISITION		0x603A
#define MTDC32_FIFO_RESET				0x603C
#define MTDC32_DATA_READY				0x603E

#define MTDC32_BANK_OPERATION			0x6040
#define MTDC32_TDC_RESOLUTION			0x6042
#define MTDC32_OUTPUT_FORMAT			0x6044

#define MTDC32_WIN_START_0	 			0x6050
#define MTDC32_WIN_START_1	 			0x6052
#define MTDC32_WIN_WIDTH_0 				0x6054
#define MTDC32_WIN_WIDTH_1 				0x6056
#define MTDC32_TRIG_SOURCE_0			0x6058
#define MTDC32_TRIG_SOURCE_1			0x605A
#define MTDC32_FIRST_HIT				0x605C

#define MTDC32_NEG_EDGE 				0x6060
#define MTDC32_ECL_TERMINATORS			0x6062
#define MTDC32_ECL_TRIG1_OSC			0x6064
#define MTDC32_TRIG_SELECT				0x6068
#define MTDC32_NIM_TRIG1_OSC			0x606A
#define MTDC32_NIM_BUSY					0x606E

#define MTDC32_PULSER_STATUS			0x6070
#define MTDC32_PULSER_PATTERN			0x6072

#define MTDC32_INPUT_THRESH_0			0x6078
#define MTDC32_INPUT_THRESH_1			0x607A

#define MTDC32_CTRA_RESET_A_OR_B		0x6090
#define MTDC32_CTRA_EVENT_LOW			0x6092
#define MTDC32_CTRA_EVENT_HIGH 			0x6094
#define MTDC32_CTRA_TS_SOURCE			0x6096
#define MTDC32_CTRA_TS_DIVISOR	 		0x6098
#define MTDC32_CTRA_TS_CTR_LOW 			0x609C
#define MTDC32_CTRA_TS_CTR_HIGH			0x609E

#define MTDC32_CTRB_TIME_0 				0x60A8
#define MTDC32_CTRB_TIME_1 				0x60AA
#define MTDC32_CTRB_TIME_2 				0x60AC
#define MTDC32_CTRB_STOP	 			0x60AE

#define MTDC32_MULT_HIGH_LIM_0	 		0x60B0
#define MTDC32_MULT_LOW_LIM_0	 		0x60B2
#define MTDC32_MULT_HIGH_LIM_1	 		0x60B4
#define MTDC32_MULT_LOW_LIM_1	 		0x60B6

#define MTDC32_ADDR_SOURCE_BOARD 		0
#define MTDC32_ADDR_SOURCE_REG 			1
#define MTDC32_ADDR_SOURCE_DEFAULT 		MTDC32_ADDR_SOURCE_BOARD
#define MTDC32_ADDR_SOURCE_MASK 		MTDC32_ADDR_SOURCE_REG

#define MTDC32_MODULE_ID_MASK			0xFF
#define MTDC32_MODULE_ID_DEFAULT		0xFF

#define MTDC32_BUFFER_DATA_LENGTH_MASK 0x3FFF

#define MTDC32_DATA_LENGTH_FORMAT_8			0
#define MTDC32_DATA_LENGTH_FORMAT_16		1
#define MTDC32_DATA_LENGTH_FORMAT_32		2
#define MTDC32_DATA_LENGTH_FORMAT_64		3
#define MTDC32_DATA_LENGTH_FORMAT_DEFAULT	MTDC32_DATA_LENGTH_FORMAT_32
#define MTDC32_DATA_LENGTH_FORMAT_MASK		MTDC32_DATA_LENGTH_FORMAT_64

#define MTDC32_MULTI_EVENT_NO			0
#define MTDC32_MULTI_EVENT_YES			0x1
#define MTDC32_MULTI_EVENT_BERR 	 	0x4
#define MTDC32_MULTI_EVENT_LIM_XFER 	(MTDC32_MULTI_EVENT_YES | 0x2)
#define MTDC32_MULTI_EVENT_DEFAULT		MTDC32_MULTI_EVENT_NO
#define MTDC32_MULTI_EVENT_MASK			0xF

#define MTDC32_MAX_XFER_DATA_DEFAULT	0
#define MTDC32_MAX_XFER_DATA_MASK		0x3FF

#define MTDC32_MARKING_TYPE_EVT			0x0
#define MTDC32_MARKING_TYPE_TS 			0x1
#define MTDC32_MARKING_TYPE_XTS 		0x3
#define MTDC32_MARKING_TYPE_DEFAULT 	MTDC32_MARKING_TYPE_EVT
#define MTDC32_MARKING_TYPE_MASK 		MTDC32_MARKING_TYPE_XTS

#define MTDC32_BANK_OPERATION_CON		0
#define MTDC32_BANK_OPERATION_INDEP		1
#define MTDC32_BANK_OPERATION_DEFAULT	MTDC32_BANK_OPERATION_CON
#define MTDC32_BANK_OPERATION_MASK  	MTDC32_BANK_OPERATION_INDEP

#define MTDC32_TDC_RESOLUTION_500		9
#define MTDC32_TDC_RESOLUTION_250		8
#define MTDC32_TDC_RESOLUTION_125		7
#define MTDC32_TDC_RESOLUTION_62_5		6
#define MTDC32_TDC_RESOLUTION_31_3		5
#define MTDC32_TDC_RESOLUTION_15_6		4
#define MTDC32_TDC_RESOLUTION_7_8		3
#define MTDC32_TDC_RESOLUTION_3_9		2
#define MTDC32_TDC_RESOLUTION_DEFAULT	0
#define MTDC32_TDC_RESOLUTION_MASK		0x1F

#define MTDC32_OUTPUT_FORMAT_TDIFF			0
#define MTDC32_OUTPUT_FORMAT_SINGLE_HIT 	1
#define MTDC32_OUTPUT_FORMAT_DEFAULT	MTDC32_OUTPUT_FORMAT_TDIFF
#define MTDC32_OUTPUT_FORMAT_MASK		MTDC32_OUTPUT_FORMAT_SINGLE_HIT

#define MTDC32_WIN_START_DEFAULT		-16
#define MTDC32_WIN_START_MASK			0x7FFF
#define MTDC32_WIN_WIDTH_DEFAULT		32
#define MTDC32_WIN_WIDTH_MASK			0x3FFF

#define MTDC32_TRIG_SOURCE_DEFAULT		0x1
#define MTDC32_TRIG_SRC_TRIG_MASK		0x3
#define MTDC32_TRIG_SRC_CHAN_MASK		0x3F
#define MTDC32_TRIG_SRC_BANK_MASK		0x3
#define MTDC32_TRIG_SOURCE_MASK		 	0x3FF

#define MTDC32_FIRST_HIT_ONLY_0		 	0x1
#define MTDC32_FIRST_HIT_ONLY_1		 	0x2
#define MTDC32_FIRST_ALL_HITS		 	0x0
#define MTDC32_FIRST_HIT_DEFAULT		(MTDC32_FIRST_HIT_ONLY_0 | MTDC32_FIRST_HIT_ONLY_1)
#define MTDC32_FIRST_HIT_MASK			MTDC32_FIRST_HIT_DEFAULT

#define MTDC32_NEG_EDGE_0				0x1
#define MTDC32_NEG_EDGE_1				0x2
#define MTDC32_NEG_EDGE_DEFAULT			0x0
#define MTDC32_NEG_EDGE_MASK			(MTDC32_NEG_EDGE_0 | MTDC32_NEG_EDGE_1)

#define MTDC32_ECL_TERM_TRIG0			0x1
#define MTDC32_ECL_TERM_TRIG1			0x2
#define MTDC32_ECL_TERM_RESET			0x4
#define MTDC32_ECL_TERM_DEFAULT			MTDC32_ECL_TERM_TRIG0
#define MTDC32_ECL_TERM_MASK			(MTDC32_ECL_TERM_TRIG0 | MTDC32_ECL_TERM_TRIG1 | MTDC32_ECL_TERM_RESET)

#define MTDC32_ECL_INPUT_TRIG1			0
#define MTDC32_ECL_INPUT_OSC			1
#define MTDC32_ECL_TRIG1_OSC_DEFAULT	MTDC32_ECL_INPUT_TRIG1
#define MTDC32_ECL_TRIG1_OSC_MASK		MTDC32_ECL_INPUT_OSC

#define MTDC32_TRIG_SELECT_NIM			0
#define MTDC32_TRIG_SELECT_ECL			1
#define MTDC32_TRIG_SELECT_DEFAULT		MTDC32_TRIG_SELECT_NIM
#define MTDC32_TRIG_SELECT_MASK			MTDC32_TRIG_SELECT_ECL

#define MTDC32_NIM_INPUT_TRIG1			0
#define MTDC32_NIM_INPUT_OSC			1
#define MTDC32_NIM_TRIG1_OSC_DEFAULT	MTDC32_NIM_INPUT_TRIG1
#define MTDC32_NIM_TRIG1_OSC_MASK		MTDC32_NIM_INPUT_OSC

#define MTDC32_NIM_BUSY_BUSY				0
#define MTDC32_NIM_BUSY_CBUS				3
#define MTDC32_NIM_BUSY_BUFFER_FULL			4
#define MTDC32_NIM_BUSY_ABOVE_THRESH		8
#define MTDC32_NIM_BUSY_DEFAULT				MTDC32_NIM_BUSY_BUSY
#define MTDC32_NIM_BUSY_MASK				0xF

#define MTDC32_PULSER_OFF					0
#define MTDC32_PULSER_ON					1
#define MTDC32_PULSER_STATUS_DEFAULT		MTDC32_PULSER_OFF
#define MTDC32_PULSER_STATUS_MASK			MTDC32_PULSER_ON

#define MTDC32_PULSER_PATTERN_DEFAULT		0x0
#define MTDC32_PULSER_PATTERN_MASK			0xFF

#define MTDC32_INPUT_THRESH_MASK			0xFF
#define MTDC32_INPUT_THRESH_DEFAULT			105

#define MTDC32_CTRA_TS_SOURCE_VME			0
#define MTDC32_CTRA_TS_SOURCE_EXT			0x1
#define MTDC32_CTRA_TS_SOURCE_XRES_ENA		0x2
#define MTDC32_CTRA_TS_SOURCE_DEFAULT		MTDC32_CTRA_TS_SOURCE_VME
#define MTDC32_CTRA_TS_SOURCE_MASK			0x3

#define MTDC32_CTRA_TS_DIVISOR_DEFAULT 		1
#define MTDC32_CTRA_TS_DIVISOR_MASK 		0xFFFF

#define MTDC32_HIGH_LIMIT_DEFAULT_0			32
#define MTDC32_HIGH_LIMIT_DEFAULT_1			16
#define MTDC32_LOW_LIMIT_DEFAULT			0
#define MTDC32_MULT_LIMIT_MASK				0x3F


#define MTDC32_M_SIGNATURE					0xC0000000
#define MTDC32_M_HEADER						0x40000000
#define MTDC32_M_TRAILER					0xC0000000
#define MTDC32_M_EOB						0x80000000
#define MTDC32_M_WC							0x00000FFF

#define MTDC32_MCST_ENA						(0x1 << 7)
#define MTDC32_MCST_DIS						(0x1 << 6)
#define MTDC32_CBLT_FIRST_ENA				(0x1 << 5)
#define MTDC32_CBLT_FIRST_DIS				(0x1 << 4)
#define MTDC32_CBLT_LAST_ENA				(0x1 << 3)
#define MTDC32_CBLT_LAST_DIS				(0x1 << 2)
#define MTDC32_CBLT_ENA						(0x1 << 1)
#define MTDC32_CBLT_DIS						(0x1 << 0)


#endif
