
/* 	------------------------------------------------------------
	- Header File for the Detector Trigger Unit (Shower-Crate) -
	------------------------------------------------------------
	
	Date: 20/01/1999			Author: Markus Petri
	
	Remarks:

	~RB_OP_MODE      'n' == Readout Board is switched into RUN MODE
                                and accepts calibration/normal LVL1 trigger
                         't' == Readout Board is switched into TEST MODE
        ~RB_ACCESS_MODE: 'n' == DTU-RB bus timing DTU controlled
        		 	requires Xilinx Conf-File: '..............'
                         't' == DTU-RB bus timing software controlled
                         	requires Xilinx Conf-File: 'showertest.rbt'
	~DTU_OP_MODE   : 'n' == DTU is trigger bus controlled
                         't' == DTU is software controlled
*/

#if !defined(DTU_BASIS)
#define DTU_BASIS

#define RB_OP_MODE                      'n'
#define RB_ACCESS_MODE			'n'
#define DTU_OP_MODE			'n'
#define DTU_BASE                        0x00000000
#define DTU_BASIS_ADR			0x44400000
#define DTU_IPC_BRCAST                  0x5c
#define DTU_IPC_ADDR_REG		0x58
#define DTU_IPC_DATA_REG		0x54
#define DTU_ACK 	                0x48
#define DTU_COMMAND     	        0x44
#define DTU_DATA               		0x40
#define DTU_LVL2_TMODE_REG		0x38
#define DTU_LVL1_TMODE_REG	        0x34
/* new register for trigger delay */
#define DTU_DELAY			0x4c
#define DTU_RB_MAP                      0x8
#define DTU_CTR_REG                     0x4
#define DTU_DATA_REG                    0x0
#define DTU_DEAD_TIME_LVL1		0x4
#define DTU_DEAD_TIME_LVL2		0x14
#define DTU_STATUS_LVL1			0x0
#define DTU_STATUS_LVL2			0x10


#define RB_M_RES			0x0111
#define RB_R_MODE			0x0122
#define RB_T_MODE			0x0133
#define RB_LVL1_TR			0x0144
#define RB_LVL1_CAL	        	0x0155
#define RB_SEL_F1			0x0161
#define RB_SEL_F2			0x0162
#define RB_SEL_F3			0x0164
#define RB_SEL_F4			0x0168
#define RB_SEL_F1234	        	0x016f
#define RB_LOAD_F			0x0177
#define RB_RES_F			0x0188
#define RB_LOAD_ID			0x0199

#define RB_TEST_VECTOR_SIZE             2046
#define RB_SIZE_OF_FIFO                 386

#endif
