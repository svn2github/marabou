#ifndef __SIS_3302_LAYOUT_H__
#define __SIS_3302_LAYOUT_H__

/*___________________________________________________________[BIT DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           Sis3302_Layout.h
// Purpose:        Register Layout
// Description:    Definitions for SIS 3302 modules
// Keywords:
////////////////////////////////////////////////////////////////////////////*/

/* SIS3302 Standard */
#define SIS3302_CONTROL_STATUS                       0x0	  /* read/write; D32 */
#define SIS3302_MODID                                0x4	  /* read only; D32 */
#define SIS3302_IRQ_CONFIG                           0x8      /* read/write; D32 */
#define SIS3302_IRQ_CONTROL                          0xC      /* read/write; D32 */
#define SIS3302_ACQUISITION_CONTROL                  0x10      /* read/write; D32 */


#define SIS3302_CBLT_BROADCAST_SETUP                0x30      /* read/write; D32 */
#define SIS3302_ADC_MEMORY_PAGE_REGISTER            0x34      /* read/write; D32 */

#define SIS3302_DAC_CONTROL_STATUS                  0x50      /* read/write; D32 */
#define SIS3302_DAC_DATA                            0x54      /* read/write; D32 */

#define SIS3302_MCA_SCAN_NOF_HISTOGRAMS_PRESET             0x80      /* GAMMA, 14xx */
#define SIS3302_MCA_SCAN_HISTOGRAM_COUNTER                 0x84      /* GAMMA, 14xx */
#define SIS3302_MCA_SCAN_SETUP_PRESCALE_FACTOR             0x88      /* GAMMA, 14xx */
#define SIS3302_MCA_SCAN_CONTROL                           0x8C      /* GAMMA, 14xx */
#define SIS3302_MCA_MULTISCAN_NOF_SCANS_PRESET             0x90      /* GAMMA, 14xx */
#define SIS3302_MCA_MULTISCAN_SCAN_COUNTER                 0x94      /* GAMMA, 14xx */
#define SIS3302_MCA_MULTISCAN_LAST_SCAN_HISTOGRAM_COUNTER  0x98      /* GAMMA, 14xx */

#define SIS3302_KEY_RESET                           0x400	  /* write only; D32 */
#define SIS3302_KEY_SAMPLE_LOGIC_RESET_404          0x404	  /* write only; D32 */

/* Key Addresses (MCA=0) */
#define SIS3302_KEY_SAMPLE_LOGIC_RESET_410          0x410	  /* write only; D32 */
#define SIS3302_KEY_DISARM                         	0x414	  /* write only; D32 */
#define SIS3302_KEY_TRIGGER                         0x418	  /* write only; D32 */
#define SIS3302_KEY_TIMESTAMP_CLEAR                 0x41C	  /* write only; D32 */
#define SIS3302_KEY_DISARM_AND_ARM_BANK1           	0x420	  /* write only; D32 */
#define SIS3302_KEY_DISARM_AND_ARM_BANK2           	0x424	  /* write only; D32 */
#define SIS3302_KEY_RESET_DDR2_LOGIC                0x428	  /* write only; D32 */

/* Key Addresses (MCA=1) */
#define SIS3302_KEY_MCA_SCAN_LNE_PULSE                    0x410	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_SCAN_ARM                          0x414	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_SCAN_START                        0x418	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_SCAN_DISABLE                      0x41C	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_MULTISCAN_START_RESET_PULSE       0x420	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_MULTISCAN_ARM_SCAN_ARM            0x424	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_MULTISCAN_ARM_SCAN_ENABLE         0x428	  /* GAMMA, 14xx */
#define SIS3302_KEY_MCA_MULTISCAN_DISABLE                 0x42C	  /* GAMMA, 14xx */

/* all ADC FPGA groups */
#define SIS3302_EVENT_CONFIG_ALL_ADC                			0x01000000
#define SIS3302_END_ADDRESS_THRESHOLD_ALL_ADC       			0x01000004	    /* Gamma */
#define SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ALL_ADC  	0x01000008	    /* Gamma */
#define SIS3302_RAW_DATA_BUFFER_CONFIG_ALL_ADC         			0x0100000C	    /* Gamma */

#define SIS3302_ENERGY_SETUP_GP_ALL_ADC   						0x01000040      /* Gamma */
#define SIS3302_ENERGY_GATE_LENGTH_ALL_ADC						0x01000044      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_LENGTH_ALL_ADC					0x01000048      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX1_ALL_ADC				0x0100004C      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX2_ALL_ADC				0x01000050      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX3_ALL_ADC				0x01000054      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC1357						0x01000058      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC2468						0x0100005C      /* Gamma */

#define SIS3302_MCA_ENERGY_TO_HISTO_ODD                         0x01000060      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_EVEN                        0x01000064      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ALL                         0x01000068      /* GAMMA, 14xx */

#define SIS3302_EVENT_EXTENDED_CONFIG_ALL                       0x01000070      /* GAMMA, 14xx */

/* ADC12 FPGA group */
#define SIS3302_EVENT_CONFIG_ADC12                				0x02000000 		/* Gamma */
#define SIS3302_END_ADDRESS_THRESHOLD_ADC12      	 			0x02000004	    /* Gamma */
#define SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC12  		0x02000008	    /* Gamma */
#define SIS3302_RAW_DATA_BUFFER_CONFIG_ADC12       				0x0200000C	    /* Gamma */

#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC1          			0x02000010
#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC2          			0x02000014
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC1   			0x02000018
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC2   			0x0200001C

#define SIS3302_ACTUAL_SAMPLE_VALUE_ADC12           			0x02000020
#define SIS3302_DDR2_TEST_REGISTER_ADC12						0x02000028

#define SIS3302_TRIGGER_SETUP_ADC1                  			0x02000030
#define SIS3302_TRIGGER_THRESHOLD_ADC1              			0x02000034
#define SIS3302_TRIGGER_SETUP_ADC2                  			0x02000038
#define SIS3302_TRIGGER_THRESHOLD_ADC2              			0x0200003C

#define SIS3302_ENERGY_SETUP_GP_ADC12   						0x02000040      /* Gamma */
#define SIS3302_ENERGY_GATE_LENGTH_ADC12						0x02000044      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_LENGTH_ADC12						0x02000048     /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC12				0x0200004C      /* Gamma */

#define SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC12				0x02000050      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC12				0x02000054      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC1							0x02000058      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC2							0x0200005C      /* Gamma */

#define SIS3302_MCA_ENERGY_TO_HISTO_ADC1                        0x02000060      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC2			0x02000064      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC12                       0x02000068      /* GAMMA, 14xx */

#define SIS3302_EVENT_EXTENDED_CONFIG_ADC12                     0x02000070      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC1                     0x02000078      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC2                     0x0200007C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC1                     0x02000080      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC1                          0x02000084      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC1                 0x02000088      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC1                  0x0200008C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC2                     0x02000090      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC2                          0x02000094      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC2                 0x02000098      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC2                  0x0200009C      /* GAMMA, 14xx */

/* ADC34 FPGA group */
#define SIS3302_EVENT_CONFIG_ADC34                				0x02800000 		/* Gamma */
#define SIS3302_END_ADDRESS_THRESHOLD_ADC34      	 			0x02800004	    /* Gamma */
#define SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC34  		0x02800008	    /* Gamma */
#define SIS3302_RAW_DATA_BUFFER_CONFIG_ADC34       				0x0280000C	    /* Gamma */

#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC3          			0x02800010
#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC4          			0x02800014
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC3   			0x02800018
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC4   			0x0280001C

#define SIS3302_ACTUAL_SAMPLE_VALUE_ADC34           			0x02800020
#define SIS3302_DDR2_TEST_REGISTER_ADC34						0x02800028

#define SIS3302_TRIGGER_SETUP_ADC3                  			0x02800030
#define SIS3302_TRIGGER_THRESHOLD_ADC3              			0x02800034
#define SIS3302_TRIGGER_SETUP_ADC4                  			0x02800038
#define SIS3302_TRIGGER_THRESHOLD_ADC4              			0x0280003C

#define SIS3302_ENERGY_SETUP_GP_ADC34   						0x02800040      /* Gamma */
#define SIS3302_ENERGY_GATE_LENGTH_ADC34						0x02800044      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_LENGTH_ADC34						0x02800048      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC34				0x0280004C      /* Gamma */

#define SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC34				0x02800050      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC34				0x02800054      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC3							0x02800058      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC4							0x0280005C      /* Gamma */

#define SIS3302_MCA_ENERGY_TO_HISTO_ADC3                        0x02800060      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC4                        0x02800064      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC34                       0x02800068      /* GAMMA, 14xx */

#define SIS3302_EVENT_EXTENDED_CONFIG_ADC34			0x02800070      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC3			0x02800078      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC4                     0x0280007C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC3                     0x02800080      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC3                          0x02800084      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC3                 0x02800088      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC3                  0x0280008C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC4                     0x02800090      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC4                          0x02800094      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC4                 0x02800098      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC4                  0x0280009C      /* GAMMA, 14xx */

/* ADC56 FPGA group */
#define SIS3302_EVENT_CONFIG_ADC56                				0x03000000 		/* Gamma */
#define SIS3302_END_ADDRESS_THRESHOLD_ADC56      	 			0x03000004	    /* Gamma */
#define SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC56  		0x03000008	    /* Gamma */
#define SIS3302_RAW_DATA_BUFFER_CONFIG_ADC56       				0x0300000C	    /* Gamma */

#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC5          			0x03000010
#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC6          			0x03000014
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC5   			0x03000018
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC6   			0x0300001C

#define SIS3302_ACTUAL_SAMPLE_VALUE_ADC56           			0x03000020
#define SIS3302_DDR2_TEST_REGISTER_ADC56						0x03000028

#define SIS3302_TRIGGER_SETUP_ADC5                  			0x03000030
#define SIS3302_TRIGGER_THRESHOLD_ADC5              			0x03000034
#define SIS3302_TRIGGER_SETUP_ADC6                  			0x03000038
#define SIS3302_TRIGGER_THRESHOLD_ADC6              			0x0300003C

#define SIS3302_ENERGY_SETUP_GP_ADC56   						0x03000040      /* Gamma */
#define SIS3302_ENERGY_GATE_LENGTH_ADC56						0x03000044      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_LENGTH_ADC56						0x03000048      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC56				0x0300004C      /* Gamma */

#define SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC56				0x03000050      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC56				0x03000054      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC5							0x03000058      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC6							0x0300005C      /* Gamma */

#define SIS3302_MCA_ENERGY_TO_HISTO_ADC5                        0x03000060      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC6                        0x03000064      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC56                       0x03000068      /* GAMMA, 14xx */

#define SIS3302_EVENT_EXTENDED_CONFIG_ADC56                     0x03000070      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC5			0x03000078      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC6                     0x0300007C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC5                     0x03000080      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC5                          0x03000084      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC5                 0x03000088      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC5                  0x0300008C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC6                     0x03000090      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC6                          0x03000094      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC6                 0x03000098      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC6                  0x0300009C      /* GAMMA, 14xx */

/* ADC78 FPGA group */
#define SIS3302_EVENT_CONFIG_ADC78                				0x03800000 		/* Gamma */
#define SIS3302_END_ADDRESS_THRESHOLD_ADC78      	 			0x03800004	    /* Gamma */
#define SIS3302_PRETRIGGER_DELAY_TRIGGERGATE_LENGTH_ADC78  		0x03800008	    /* Gamma */
#define SIS3302_RAW_DATA_BUFFER_CONFIG_ADC78       				0x0380000C	    /* Gamma */

#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC7          			0x03800010
#define SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC8          			0x03800014
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC7   			0x03800018
#define SIS3302_PREVIOUS_BANK_SAMPLE_ADDRESS_ADC8   			0x0380001C

#define SIS3302_ACTUAL_SAMPLE_VALUE_ADC78           			0x03800020
#define SIS3302_DDR2_TEST_REGISTER_ADC78						0x03800028

#define SIS3302_TRIGGER_SETUP_ADC7                  			0x03800030
#define SIS3302_TRIGGER_THRESHOLD_ADC7              			0x03800034
#define SIS3302_TRIGGER_SETUP_ADC8                  			0x03800038
#define SIS3302_TRIGGER_THRESHOLD_ADC8              			0x0380003C

#define SIS3302_ENERGY_SETUP_GP_ADC78   						0x03800040      /* Gamma */
#define SIS3302_ENERGY_GATE_LENGTH_ADC78						0x03800044      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_LENGTH_ADC78						0x03800048      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX1_ADC78				0x0380004C      /* Gamma */

#define SIS3302_ENERGY_SAMPLE_START_INDEX2_ADC78				0x03800050      /* Gamma */
#define SIS3302_ENERGY_SAMPLE_START_INDEX3_ADC78				0x03800054      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC7							0x03800058      /* Gamma */
#define SIS3302_ENERGY_TAU_FACTOR_ADC8							0x0380005C      /* Gamma */

#define SIS3302_MCA_ENERGY_TO_HISTO_ADC7                        0x03800060      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC8                        0x03800064      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HISTO_ADC78                       0x03800068      /* GAMMA, 14xx */

#define SIS3302_EVENT_EXTENDED_CONFIG_ADC78                     0x03800070      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC7                     0x03800078      /* GAMMA, 14xx */
#define SIS3302_TRIGGER_SETUP_EXTENDED_ADC8                     0x0380007C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC7                     0x03800080      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC7                          0x03800084      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC7                 0x03800088      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC7                  0x0380008C      /* GAMMA, 14xx */

#define SIS3302_MCA_TRIG_START_COUNTER_ADC8                     0x03800090      /* GAMMA, 14xx */
#define SIS3302_MCA_PILUP_COUNTER_ADC8                          0x03800094      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_HIGH_COUNTER_ADC8                 0x03800098      /* GAMMA, 14xx */
#define SIS3302_MCA_ENERGY_TO_LOW_COUNTER_ADC8                  0x0380009C      /* GAMMA, 14xx */

#define SIS3302_ADC1_OFFSET                         			0x04000000
#define SIS3302_ADC2_OFFSET                         			0x04800000
#define SIS3302_ADC3_OFFSET                         			0x05000000
#define SIS3302_ADC4_OFFSET                         			0x05800000
#define SIS3302_ADC5_OFFSET                         			0x06000000
#define SIS3302_ADC6_OFFSET                         			0x06800000
#define SIS3302_ADC7_OFFSET                         			0x07000000
#define SIS3302_ADC8_OFFSET                         			0x07800000

#define SIS3302_NEXT_ADC_OFFSET                     			0x00800000

/* define sample clock */
#define SIS3302_ACQ_SET_CLOCK_TO_100MHZ                 0x70000000  /* default after Reset  */
#define SIS3302_ACQ_SET_CLOCK_TO_50MHZ                  0x60001000
#define SIS3302_ACQ_SET_CLOCK_TO_25MHZ                  0x50002000
#define SIS3302_ACQ_SET_CLOCK_TO_10MHZ                  0x40003000
#define SIS3302_ACQ_SET_CLOCK_TO_1MHZ                   0x30004000
#define SIS3302_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN          0x10006000
#define SIS3302_ACQ_SET_CLOCK_TO_100MHZ_2               0x00007000	/* Gamma, 14xx */



#define SIS3302_ACQ_DISABLE_LEMO_TRIGGER            0x01000000 /* GAMMA, 091207 */
#define SIS3302_ACQ_ENABLE_LEMO_TRIGGER             0x00000100 /* GAMMA, 091207 */
#define SIS3302_ACQ_DISABLE_LEMO_TIMESTAMPCLR       0x02000000 /* GAMMA, 091207 */
#define SIS3302_ACQ_ENABLE_LEMO_TIMESTAMPCLR        0x00000200 /* GAMMA, 091207 */


/* gamma (from 3320 LLNL */
#define EVENT_CONF_ADC2_EXTERN_TRIGGER_ENABLE_BIT		0x800
#define EVENT_CONF_ADC2_INTERN_TRIGGER_ENABLE_BIT		0x400
#define EVENT_CONF_ADC2_INPUT_INVERT_BIT				0x100

#define EVENT_CONF_ADC1_EXTERN_TRIGGER_ENABLE_BIT		0x8
#define EVENT_CONF_ADC1_INTERN_TRIGGER_ENABLE_BIT		0x4
#define EVENT_CONF_ADC1_INPUT_INVERT_BIT				0x1



#define DECIMATION_DISABLE							0x00000000
#define DECIMATION_2								0x10000000
#define DECIMATION_4								0x20000000
#define DECIMATION_8								0x30000000

/* event header/trailer */
#define SIS3302_EVENT_HEADER	2
#define SIS3302_EVENT_MIN_MAX	2
#define SIS3302_EVENT_TRAILER	2

/* end thresh bit in acq control */
#define SIS3302_STATUS_END_THRESH					0x00080000

#endif