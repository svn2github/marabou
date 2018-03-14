#ifndef __CAEN_785_LAYOUT_H__
#define __CAEN_785_LAYOUT_H__
/*___________________________________________________________[BIT DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           Caen785_Layout.h
// Purpose:        Register Layout
// Description:    Definitions for CAEN V785
// Keywords:
////////////////////////////////////////////////////////////////////////////*/

#define CAEN_V785_R_OUTPUT						0x0
#define CAEN_V785_R_FIRMWARE						0x1000
#define CAEN_V785_785_R_GEOADDR						0x1002
#define CAEN_V785_R_BITSET1						0x1006
#define CAEN_V785_R_BITCLEAR1					0x1008
#define CAEN_V785_R_STATUS1						0x100E
#define CAEN_V785_R_CONTROL1						0x1010
#define CAEN_V785_R_ADDR_HIGH					0x1012
#define CAEN_V785_R_ADDR_LOW						0x1014
#define CAEN_V785_R_EVT_TRIG						0x1020
#define CAEN_V785_R_STATUS2						0x1022
#define CAEN_V785_R_EVT_CNT_LOW					0x1024
#define CAEN_V785_R_EVT_CNT_HIGH 				0x1026
#define CAEN_V785_R_INCR_EVT						0x1028
#define CAEN_V785_R_INCR_OFFS					0x102A
#define CAEN_V785_R_BITSET2						0x1032
#define CAEN_V785_R_BITCLEAR2					0x1034
#define CAEN_V785_R_CRATE_SEL					0x103C
#define CAEN_V785_R_TEST_EVT_WRITE				0x103E
#define CAEN_V785_R_EVT_CNT_RESET				0x1040
#define CAEN_V785_R_THRESH 						0x1080
#define CAEN_V785_R_ROM	 						0x8000

#define CAEN_V785_B_BITSET1_SEL_ADDR_INTERN 		(0x1 << 4)
#define CAEN_V785_B_BITSET1_RESET			 		(0x1 << 7)
#define CAEN_V785_B_CONTROL1_PROG_RESET_MODULE	(0x1 << 4)
#define CAEN_V785_B_BITSET2_OFFLINE				(0x1 << 1)
#define CAEN_V785_B_BITSET2_CLEAR_DATA		(0x1 << 2)
#define CAEN_V785_B_BITSET2_OVER_RANGE_DIS		(0x1 << 3)
#define CAEN_V785_B_BITSET2_LOW_THRESH_DIS		(0x1 << 4)
#define CAEN_V785_B_BITSET2_SLIDE_ENA			(0x1 << 7)
#define CAEN_V785_B_BITSET2_STEP_TH 			(0x1 << 8)
#define CAEN_V785_B_BITSET2_AUTO_INCR_ENA 		(0x1 << 11)
#define CAEN_V785_B_BITSET2_EMPTY_PROG_ENA		(0x1 << 12)
#define CAEN_V785_B_BITSET2_ALL_TRIG_ENA			(0x1 << 14)
#define CAEN_V785_B_STATUS2_BUFFER_EMPTY			(0x1 << 1)
#define CAEN_V785_B_THRESH_KILL_CHANNEL 			(0x1 << 8)

#define CAEN_V785_SH_WC							8

#define CAEN_V785_D_HDR							0x02000000
#define CAEN_V785_D_DATA							0x00000000
#define CAEN_V785_D_EOB							0x04000000
#define CAEN_V785_D_INVALID						0x06000000

#define CAEN_V785_M_MSERIAL						0xFF
#define CAEN_V785_M_WC							0x3F
#define CAEN_V785_M_CHN							0xFF
#define CAEN_V785_M_ID							0x07000000
#define CAEN_V785_M_HDR							0x07FFFFFF
#define CAEN_V785_M_DATA						0x07FFFFFF
#define CAEN_V785_M_ADCDATA						0x00000FFF

#define CAEN_V785_B_OVERFLOW	 				(0x1 << 12)
#define CAEN_V785_B_UNDERTHRESH 				(0x1 << 13)

#define CAEN_V785_N_MAXEVENTS					32

#define CAEN_V785_A_OUTPUT(module)			((unsigned int *) (module + CAEN_V785_R_OUTPUT))
#define CAEN_V785_A_FIRMWARE(module) 		((unsigned short *) (module + CAEN_V785_R_FIRMWARE))
#define CAEN_V785_A_GEOADDR(module)			((unsigned short *) (module + CAEN_V785_R_GEOADDR))
#define CAEN_V785_A_BITSET1(module)			((unsigned short *) (module + CAEN_V785_R_BITSET1))
#define CAEN_V785_A_BITCLEAR1(module)		((unsigned short *) (module + CAEN_V785_R_BITCLEAR1))
#define CAEN_V785_A_STATUS1(module)			((unsigned short *) (module + CAEN_V785_R_STATUS1))
#define CAEN_V785_A_CONTROL1(module) 		((unsigned short *) (module + CAEN_V785_R_CONTROL1))
#define CAEN_V785_A_ADDR_HIGH(module) 		((unsigned short *) (module + CAEN_V785_R_ADDR_HIGH))
#define CAEN_V785_A_ADDR_LOW(module)			((unsigned short *) (module + CAEN_V785_R_ADDR_LOW))
#define CAEN_V785_A_EVT_TRIG(module)			((unsigned short *) (module + CAEN_V785_R_EVT_TRIG))
#define CAEN_V785_A_STATUS2(module)			((unsigned short *) (module + CAEN_V785_R_STATUS2))
#define CAEN_V785_A_EVT_CNT_LOW(module)		((unsigned short *) (module + CAEN_V785_R_EVT_CNT_LOW))
#define CAEN_V785_A_EVT_CNT_HIGH(module) 	((unsigned short *) (module + CAEN_V785_R_EVT_CNT_HIGH))
#define CAEN_V785_A_INCR_EVT(module)			((unsigned short *) (module + CAEN_V785_R_INCR_EVT))
#define CAEN_V785_A_INCR_OFFS(module) 		((unsigned short *) (module + CAEN_V785_R_INCR_OFFS))
#define CAEN_V785_A_BITSET2(module)			((unsigned short *) (module + CAEN_V785_R_BITSET2))
#define CAEN_V785_A_BITCLEAR2(module)		((unsigned short *) (module + CAEN_V785_R_BITCLEAR2))
#define CAEN_V785_A_CRATE_SEL(module) 		((unsigned short *) (module + CAEN_V785_R_CRATE_SEL))
#define CAEN_V785_A_TEST_EVT_WRITE(module)	((unsigned short *) (module + CAEN_V785_R_TEST_EVT_WRITE))
#define CAEN_V785_A_EVT_CNT_RESET(module)	((unsigned short *) (module + CAEN_V785_R_EVT_CNT_RESET))
#define CAEN_V785_A_THRESH(module)			((unsigned short *) (module + CAEN_V785_R_THRESH))
#define CAEN_V785_A_ROM(module)				((unsigned short *) (module + CAEN_V785_R_ROM))

#define CAEN_V785_SET_THRESHOLD(module,chn,data) 	*(CAEN_V785_A_THRESH(module) + chn) = data
#define CAEN_V785_GET_THRESHOLD(module,chn) 	*(CAEN_V785_A_THRESH(module) + chn)

#define CAEN_V785_AUTO_INCR_ON(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_AUTO_INCR_ENA
#define CAEN_V785_AUTO_INCR_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_AUTO_INCR_ENA

#define CAEN_V785_ZERO_SUPPR_ON(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_LOW_THRESH_DIS
#define CAEN_V785_ZERO_SUPPR_OFF(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_LOW_THRESH_DIS

#define CAEN_V785_ALL_TRIG_ON(module)		*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_ALL_TRIG_ENA
#define CAEN_V785_ALL_TRIG_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_ALL_TRIG_ENA

#define CAEN_V785_THRESH_RES_COARSE(module) *CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_STEP_TH
#define CAEN_V785_THRESH_RES_FINE(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_STEP_TH

#define CAEN_V785_SLIDING_SCALE_ON(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_SLIDE_ENA
#define CAEN_V785_SLIDING_SCALE_OFF(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_SLIDE_ENA

#define CAEN_V785_OVER_RANGE_CHECK_ON(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_OVER_RANGE_DIS
#define CAEN_V785_OVER_RANGE_CHECK_OFF(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_OVER_RANGE_DIS

#define CAEN_V785_EMPTY_PROG_ENABLE(module)	*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_EMPTY_PROG_ENA
#define CAEN_V785_EMPTY_PROG_DISABLE(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_EMPTY_PROG_ENA

#define CAEN_V785_CLEAR_DATA(module)	*CAEN_V785_A_BITSET2(module) = CAEN_V785_B_BITSET2_CLEAR_DATA; \
											*CAEN_V785_A_BITCLEAR2(module) = CAEN_V785_B_BITSET2_CLEAR_DATA

#define CAEN_V785_RESET_EVENT_COUNTER(module)	*CAEN_V785_A_EVT_CNT_RESET(module) = 1

#define CAEN_V785_SET_KILL_BIT(module,chn)	*(CAEN_V785_A_THRESH(module) + chn) = CAEN_V785_B_THRESH_KILL_CHANNEL

#define CAEN_V785_READ_BITSET2(module)	*CAEN_V785_A_BITSET2(module)

/* bit definitions for VME ADC CAEN V775 */
#define CAEN_V775_R_OUTPUT						0x0
#define CAEN_V775_R_FIRMWARE						0x1000
#define CAEN_V775_775_R_GEOADDR						0x1002
#define CAEN_V775_R_BITSET1						0x1006
#define CAEN_V775_R_BITCLEAR1					0x1008
#define CAEN_V775_R_STATUS1						0x100E
#define CAEN_V775_R_CONTROL1						0x1010
#define CAEN_V775_R_ADDR_HIGH					0x1012
#define CAEN_V775_R_ADDR_LOW						0x1014
#define CAEN_V775_R_EVT_TRIG						0x1020
#define CAEN_V775_R_STATUS2						0x1022
#define CAEN_V775_R_EVT_CNT_LOW					0x1024
#define CAEN_V775_R_EVT_CNT_HIGH 				0x1026
#define CAEN_V775_R_INCR_EVT						0x1028
#define CAEN_V775_R_INCR_OFFS					0x102A
#define CAEN_V775_R_BITSET2						0x1032
#define CAEN_V775_R_BITCLEAR2					0x1034
#define CAEN_V775_R_CRATE_SEL					0x103C
#define CAEN_V775_R_TEST_EVT_WRITE				0x103E
#define CAEN_V775_R_EVT_CNT_RESET				0x1040
#define CAEN_V775_R_FULL_SCALE_RANGE			0x1060
#define CAEN_V775_R_THRESH 						0x1080
#define CAEN_V775_R_ROM	 						0x8000

#define CAEN_V775_B_BITSET1_SEL_ADDR_INTERN 		(0x1 << 4)
#define CAEN_V775_B_BITSET1_RESET			 		(0x1 << 7)
#define CAEN_V775_B_CONTROL1_PROG_RESET_MODULE	(0x1 << 4)
#define CAEN_V775_B_BITSET2_OFFLINE				(0x1 << 1)
#define CAEN_V775_B_BITSET2_CLEAR_DATA		(0x1 << 2)
#define CAEN_V775_B_BITSET2_OVER_RANGE_DIS		(0x1 << 3)
#define CAEN_V775_B_BITSET2_LOW_THRESH_DIS		(0x1 << 4)
#define CAEN_V775_B_BITSET2_VALID_CONTROL_DIS	(0x1 << 5)
#define CAEN_V775_B_BITSET2_SLIDE_ENA			(0x1 << 7)
#define CAEN_V775_B_BITSET2_COMMON_START_DIS 	(0x1 << 10)
#define CAEN_V775_B_BITSET2_AUTO_INCR_ENA 		(0x1 << 11)
#define CAEN_V775_B_BITSET2_EMPTY_PROG_ENA		(0x1 << 12)
#define CAEN_V775_B_BITSET2_ALL_TRIG_ENA			(0x1 << 14)
#define CAEN_V775_B_STATUS2_BUFFER_EMPTY			(0x1 << 1)
#define CAEN_V775_B_THRESH_KILL_CHANNEL 			(0x1 << 8)

#define CAEN_V775_SH_WC							8

#define CAEN_V775_D_HDR							0x02000000
#define CAEN_V775_D_DATA							0x00000000
#define CAEN_V775_D_EOB							0x04000000
#define CAEN_V775_D_INVALID						0x06000000

#define CAEN_V775_M_MSERIAL						0xFF
#define CAEN_V775_M_WC							0x3F
#define CAEN_V775_M_CHN							0xFF
#define CAEN_V775_M_ID							0x07000000
#define CAEN_V775_M_HDR							0x07FFFFFF
#define CAEN_V775_M_DATA							0x07FFFFFF
#define CAEN_V775_M_ADCDATA						0x00000FFF

#define CAEN_V775_B_OVERFLOW	 				(0x1 << 12)
#define CAEN_V775_B_UNDERTHRESH 				(0x1 << 13)

#define CAEN_V775_N_MAXEVENTS					32

#define CAEN_V775_A_OUTPUT(module)			((unsigned int *) (module + CAEN_V775_R_OUTPUT))
#define CAEN_V775_A_FIRMWARE(module) 		((unsigned short *) (module + CAEN_V775_R_FIRMWARE))
#define CAEN_V775_A_GEOADDR(module)			((unsigned short *) (module + CAEN_V775_R_GEOADDR))
#define CAEN_V775_A_BITSET1(module)			((unsigned short *) (module + CAEN_V775_R_BITSET1))
#define CAEN_V775_A_BITCLEAR1(module)		((unsigned short *) (module + CAEN_V775_R_BITCLEAR1))
#define CAEN_V775_A_STATUS1(module)			((unsigned short *) (module + CAEN_V775_R_STATUS1))
#define CAEN_V775_A_CONTROL1(module) 		((unsigned short *) (module + CAEN_V775_R_CONTROL1))
#define CAEN_V775_A_ADDR_HIGH(module) 		((unsigned short *) (module + CAEN_V775_R_ADDR_HIGH))
#define CAEN_V775_A_ADDR_LOW(module)			((unsigned short *) (module + CAEN_V775_R_ADDR_LOW))
#define CAEN_V775_A_EVT_TRIG(module)			((unsigned short *) (module + CAEN_V775_R_EVT_TRIG))
#define CAEN_V775_A_STATUS2(module)			((unsigned short *) (module + CAEN_V775_R_STATUS2))
#define CAEN_V775_A_EVT_CNT_LOW(module)		((unsigned short *) (module + CAEN_V775_R_EVT_CNT_LOW))
#define CAEN_V775_A_EVT_CNT_HIGH(module) 	((unsigned short *) (module + CAEN_V775_R_EVT_CNT_HIGH))
#define CAEN_V775_A_INCR_EVT(module)			((unsigned short *) (module + CAEN_V775_R_INCR_EVT))
#define CAEN_V775_A_INCR_OFFS(module) 		((unsigned short *) (module + CAEN_V775_R_INCR_OFFS))
#define CAEN_V775_A_BITSET2(module)			((unsigned short *) (module + CAEN_V775_R_BITSET2))
#define CAEN_V775_A_BITCLEAR2(module)		((unsigned short *) (module + CAEN_V775_R_BITCLEAR2))
#define CAEN_V775_A_CRATE_SEL(module) 		((unsigned short *) (module + CAEN_V775_R_CRATE_SEL))
#define CAEN_V775_A_TEST_EVT_WRITE(module)	((unsigned short *) (module + CAEN_V775_R_TEST_EVT_WRITE))
#define CAEN_V775_A_EVT_CNT_RESET(module)	((unsigned short *) (module + CAEN_V775_R_EVT_CNT_RESET))
#define CAEN_V775_A_FULL_SCALE_RANGE(module)	((unsigned short *) (module + CAEN_V775_R_FULL_SCALE_RANGE))
#define CAEN_V775_A_THRESH(module)			((unsigned short *) (module + CAEN_V775_R_THRESH))
#define CAEN_V775_A_ROM(module)				((unsigned short *) (module + CAEN_V775_R_ROM))

#define CAEN_V775_SET_THRESHOLD(module,chn,data) 	*(CAEN_V775_A_THRESH(module) + chn) = data
#define CAEN_V775_GET_THRESHOLD(module,chn) 		*(CAEN_V775_A_THRESH(module) + chn)

#define CAEN_V775_SET_FULL_SCALE_RANGE(module,data) 	*CAEN_V775_A_FULL_SCALE_RANGE(module) = data

#define CAEN_V775_AUTO_INCR_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_AUTO_INCR_ENA
#define CAEN_V775_AUTO_INCR_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_AUTO_INCR_ENA

#define CAEN_V775_ZERO_SUPPR_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_LOW_THRESH_DIS
#define CAEN_V775_ZERO_SUPPR_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_LOW_THRESH_DIS

#define CAEN_V775_VALID_CONTROL_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_VALID_CONTROL_DIS
#define CAEN_V775_VALID_CONTROL_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_VALID_CONTROL_DIS

#define CAEN_V775_COMMON_START_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_COMMON_START_DIS
#define CAEN_V775_COMMON_STOP_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_COMMON_START_DIS

#define CAEN_V775_ALL_TRIG_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_ALL_TRIG_ENA
#define CAEN_V775_ALL_TRIG_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_ALL_TRIG_ENA

#define CAEN_V775_SLIDING_SCALE_ON(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_SLIDE_ENA
#define CAEN_V775_SLIDING_SCALE_OFF(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_SLIDE_ENA

#define CAEN_V775_OVER_RANGE_CHECK_ON(module)	*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_OVER_RANGE_DIS
#define CAEN_V775_OVER_RANGE_CHECK_OFF(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_OVER_RANGE_DIS

#define CAEN_V775_CLEAR_DATA(module)	*CAEN_V775_A_BITSET2(module) = CAEN_V775_B_BITSET2_CLEAR_DATA; \
											*CAEN_V775_A_BITCLEAR2(module) = CAEN_V775_B_BITSET2_CLEAR_DATA

#define CAEN_V775_RESET_EVENT_COUNTER(module)	*CAEN_V775_A_EVT_CNT_RESET(module) = 1

#define CAEN_V775_SET_KILL_BIT(module,chn)	*(CAEN_V775_A_THRESH(module) + chn) = CAEN_V775_B_THRESH_KILL_CHANNEL

#define CAEN_V775_READ_BITSET2(module)	*CAEN_V775_A_BITSET2(module)

#endif
