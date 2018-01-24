#ifndef __MESY_COMMON_H__
#define __MESY_COMMON_H__

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			mesy_common.h
//! \brief			Definitions for Mesytec modules
//! \details		Contains definitions common for all Mesytec modules
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: $
//! $Date: $
////////////////////////////////////////////////////////////////////////////*/

#define MESY_DATA					0x0

#define MESY_ADDR_SOURCE 			0x6000
#define MESY_ADDR_REG				0x6002
#define MESY_MODULE_ID				0x6004
#define MESY_SOFT_RESET				0x6008
#define MESY_FIRMWARE_REV			0x600E

#define MESY_MAX_XFER_DATA			0x601A

#define MESY_CBLT_MCST_CONTROL		0x6020
#define MESY_CBLT_ADDRESS			0x6022
#define MESY_MCST_ADDRESS			0x6024

#define MESY_BUFFER_DATA_LENGTH		0x6030
#define MESY_DATA_LENGTH_FORMAT		0x6032
#define MESY_READOUT_RESET			0x6034
#define MESY_MULTI_EVENT 			0x6036
#define MESY_MARKING_TYPE			0x6038
#define MESY_START_ACQUISITION		0x603A
#define MESY_FIFO_RESET				0x603C
#define MESY_DATA_READY				0x603E

#define MESY_CTRA_RESET_A_OR_B		0x6090
#define MESY_CTRA_EVENT_LOW			0x6092
#define MESY_CTRA_EVENT_HIGH 		0x6094
#define MESY_CTRA_TS_SOURCE			0x6096
#define MESY_CTRA_TS_DIVISOR	 	0x6098
#define MESY_CTRA_TS_CTR_LOW 		0x609C
#define MESY_CTRA_TS_CTR_HIGH		0x609E

#define MESY_CTRB_TIME_0 			0x60A8
#define MESY_CTRB_TIME_1 			0x60AA
#define MESY_CTRB_TIME_2 			0x60AC
#define MESY_CTRB_STOP	 			0x60AE

#define MESY_ADDR_SOURCE_BOARD 		0
#define MESY_ADDR_SOURCE_REG 		1
#define MESY_ADDR_SOURCE_DEFAULT 	MESY_ADDR_SOURCE_BOARD
#define MESY_ADDR_SOURCE_MASK 		MESY_ADDR_SOURCE_REG

#define MESY_MODULE_ID_MASK			0xFF
#define MESY_MODULE_ID_DEFAULT		0xFF

#define MESY_BUFFER_DATA_LENGTH_MASK 0x3FFF

#define MESY_DATA_LENGTH_FORMAT_8		0
#define MESY_DATA_LENGTH_FORMAT_16		1
#define MESY_DATA_LENGTH_FORMAT_32		2
#define MESY_DATA_LENGTH_FORMAT_64		3
#define MESY_DATA_LENGTH_FORMAT_DEFAULT	MESY_DATA_LENGTH_FORMAT_32
#define MESY_DATA_LENGTH_FORMAT_MASK	MESY_DATA_LENGTH_FORMAT_64

#define MESY_MULTI_EVENT_NO			0
#define MESY_MULTI_EVENT_YES		0x1
#define MESY_MULTI_EVENT_BERR 	 	0x4
#define MESY_MULTI_EVENT_LIM_XFER 	(MESY_MULTI_EVENT_YES | 0x2)
#define MESY_MULTI_EVENT_DEFAULT	MESY_MULTI_EVENT_NO
#define MESY_MULTI_EVENT_MASK		0xF

#define MESY_MAX_XFER_DATA_DEFAULT	0
#define MESY_MAX_XFER_DATA_MASK		0x3FF

#define MESY_MARKING_TYPE_EVT		0x0
#define MESY_MARKING_TYPE_TS 		0x1
#define MESY_MARKING_TYPE_XTS 		0x3
#define MESY_MARKING_TYPE_DEFAULT 	MESY_MARKING_TYPE_EVT
#define MESY_MARKING_TYPE_MASK 		MESY_MARKING_TYPE_XTS

#define MESY_CTRA_TS_SOURCE_VME			0
#define MESY_CTRA_TS_SOURCE_EXT			0x1
#define MESY_CTRA_TS_SOURCE_XRES_ENA	0x2
#define MESY_CTRA_TS_SOURCE_DEFAULT		MESY_CTRA_TS_SOURCE_VME
#define MESY_CTRA_TS_SOURCE_MASK		0x3

#define MESY_CTRA_TS_DIVISOR_DEFAULT 	1
#define MESY_CTRA_TS_DIVISOR_MASK 		0xFFFF

#define MESY_D_CHAN_INACTIVE				0x1FFF

#define MESY_M_SIGNATURE				0xC0000000
#define MESY_M_HEADER					0x40000000
#define MESY_M_TRAILER					0xC0000000
#define MESY_M_EOB						0x80000000
#define MESY_M_WC						0x00000FFF

#define MESY_MCST_ENA					(0x1 << 7)
#define MESY_MCST_DIS					(0x1 << 6)
#define MESY_CBLT_FIRST_ENA				(0x1 << 5)
#define MESY_CBLT_FIRST_DIS				(0x1 << 4)
#define MESY_CBLT_LAST_ENA				(0x1 << 3)
#define MESY_CBLT_LAST_DIS				(0x1 << 2)
#define MESY_CBLT_ENA					(0x1 << 1)
#define MESY_CBLT_DIS					(0x1 << 0)


#endif
