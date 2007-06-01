#ifndef __SIS_3300_H__
#define __SIS_3300_H__

#include <sys/types.h>
#include <stdint.h>

#include "block.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3300.h
// Purpose:        Definitions for SIS3300 digitizing ADC
// Description:    Contains definitions to operate a SIS 3300 digitizing ADC
// Author:         R. Lutter (adopted from code written by L. Maier)
// Revision:       
// Date:           
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

#define SIS_3300_VME_SIZE  0x800000

#define SIS_3300_STATUS                  0x00    /* status register */
#define SIS_3300_CONTROL                 0x00    /* control register */

#define SIS_3300_MODULE_ID               0x04    /* module id. and firmware revision */
#define SIS_3300_IRQ_CONFIG              0x08    /* interrupt configuration register */
#define SIS_3300_IRQ_CONTROL             0xC     /* interrupt control register */

#define SIS_3300_ACQ_STATUS              0x10    /* acquisition status register */
#define SIS_3300_ACQ_CONTROL             0x10    /* acquisition control register */
#define SIS_3300_EXT_START_DELAY         0x14    /* external start delay */
#define SIS_3300_EXT_STOP_DELAY          0x18    /* external stop delay */

#define SIS_3300_KEY_RESET               0x20    /* key address for reset */
#define SIS_3300_KEY_START               0x30    /* key address for VME start */
#define SIS_3300_KEY_STOP                0x34    /* key address for VME stop */

#define SIS_3300_BANK1_TIME_DIR    0x00001000    /* bank 1 time stamp directory */
#define SIS_3300_BANK2_TIME_DIR    0x00002000    /* bank 2 time stamp directory */
#define SIS_3300_EVT_CONFIG        0x00100000    /* event configuration register */

/* ADC group 1 registers */
#define SIS_3300_GROUP1_EVT_CONFIG         0x00200000    /* group 1 event configuration  */
#define SIS_3300_GROUP1_THRESHOLD          0x00200004    /* group 1 threshold register  */
#define SIS_3300_BANK1_GROUP1_ADR_COUNTER  0x00200008    /* group 1 bank 1 address counter */
#define SIS_3300_BANK2_GROUP1_ADR_COUNTER  0x0020000C    /* group 1 bank 2 address counter */
#define SIS_3300_BANK1_GROUP1_EVT_COUNTER  0x00200010    /* group 1 bank 1 event counter */
#define SIS_3300_BANK2_GROUP1_EVT_COUNTER  0x00200014    /* group 1 bank 2 event counter  */
#define SIS_3300_BANK1_GROUP1_EVT_DIR      0x00201000    /* group 1 bank 1 event counter */
#define SIS_3300_BANK2_GROUP1_EVT_DIR      0x00202000    /* group 1 bank 2 event counter  */
/* ADC group 2 registers */
#define SIS_3300_GROUP2_EVT_CONFIG         0x00280000    /* group 2 event configuration  */
#define SIS_3300_GROUP2_THRESHOLD          0x00280004    /* group 2 threshold register  */
#define SIS_3300_BANK1_GROUP2_ADR_COUNTER  0x00280008    /* group 2 bank 1 address counter */
#define SIS_3300_BANK2_GROUP2_ADR_COUNTER  0x0028000C    /* group 2 bank 2 address counter */
#define SIS_3300_BANK1_GROUP2_EVT_COUNTER  0x00280010    /* group 2 bank 1 event counter */
#define SIS_3300_BANK2_GROUP2_EVT_COUNTER  0x00280014    /* group 2 bank 2 event counter  */
#define SIS_3300_BANK1_GROUP2_EVT_DIR      0x00281000    /* group 2 bank 1 event counter */
#define SIS_3300_BANK2_GROUP2_EVT_DIR      0x00282000    /* group 2 bank 2 event counter  */
/* ADC group 3 registers */
#define SIS_3300_GROUP3_EVT_CONFIG         0x00300000    /* group 3 event configuration  */
#define SIS_3300_GROUP3_THRESHOLD          0x00300004    /* group 3 threshold register  */
#define SIS_3300_BANK1_GROUP3_ADR_COUNTER  0x00300008    /* group 3 bank 1 address counter */
#define SIS_3300_BANK2_GROUP3_ADR_COUNTER  0x0030000C    /* group 3 bank 2 address counter */
#define SIS_3300_BANK1_GROUP3_EVT_COUNTER  0x00300010    /* group 3 bank 1 event counter */
#define SIS_3300_BANK2_GROUP3_EVT_COUNTER  0x00300014    /* group 3 bank 2 event counter  */
#define SIS_3300_BANK1_GROUP3_EVT_DIR      0x00301000    /* group 3 bank 1 event counter */
#define SIS_3300_BANK2_GROUP3_EVT_DIR      0x00302000    /* group 3 bank 2 event counter  */
/* ADC group 4 registers */
#define SIS_3300_GROUP4_EVT_CONFIG         0x00380000    /* group 4 event configuration  */
#define SIS_3300_GROUP4_THRESHOLD          0x00380004    /* group 4 threshold register  */
#define SIS_3300_BANK1_GROUP4_ADR_COUNTER  0x00380008    /* group 4 bank 1 address counter */
#define SIS_3300_BANK2_GROUP4_ADR_COUNTER  0x0038000C    /* group 4 bank 2 address counter */
#define SIS_3300_BANK1_GROUP4_EVT_COUNTER  0x00380010    /* group 4 bank 1 event counter */
#define SIS_3300_BANK2_GROUP4_EVT_COUNTER  0x00380014    /* group 4 bank 2 event counter  */
#define SIS_3300_BANK1_GROUP4_EVT_DIR      0x00381000    /* group 4 bank 1 event counter */
#define SIS_3300_BANK2_GROUP4_EVT_DIR      0x00382000    /* group 4 bank 2 event counter  */

/* memory banks 1 & 2 */
#define SIS_3300_BANK1_MEMORY      0x00400000    /* memory bank 1 base address */
#define SIS_3300_BANK1_GROUP1      0x00400000    /* memory bank 1 group 1 address */
#define SIS_3300_BANK1_GROUP2      0x00480000    /* memory bank 1 group 2 address */
#define SIS_3300_BANK1_GROUP3      0x00500000    /* memory bank 1 group 3 address */
#define SIS_3300_BANK1_GROUP4      0x00580000    /* memory bank 1 group 4 address */

#define SIS_3300_BANK2_MEMORY      0x00600000    /* memory bank 2 base address    */
#define SIS_3300_BANK2_GROUP1      0x00600000    /* memory bank 2 group 1 address */
#define SIS_3300_BANK2_GROUP2      0x00680000    /* memory bank 2 group 2 address */
#define SIS_3300_BANK2_GROUP3      0x00700000    /* memory bank 2 group 3 address */
#define SIS_3300_BANK2_GROUP4      0x00780000    /* memory bank 2 group 4 address */

/* control register bit assignments */
/* on */
#define SIS_3300_CTL_LED_ON           0x00000001
#define SIS_3300_CTL_USEROUT_ON       0x00000002
#define SIS_3300_CTL_ENABLE_TRIGOUT   0x00000004
#define SIS_3300_CTL_INVERSE_TRIGOUT  0x00000010
#define SIS_3300_CTL_TRIGOUT_ARMSTART 0x00000020
/* off (default) */
#define SIS_3300_CTL_LED_OFF          0x00010000
#define SIS_3300_CTL_USEROUT_OFF      0x00020000
#define SIS_3300_CTL_ENABLE_USEROUT   0x00040000
#define SIS_3300_CTL_STRAIGHT_TRIGOUT 0x00100000
#define SIS_3300_CTL_TRIGOUT_ARMED    0x00200000
/* example:  data = SIS_3300_CTL_LED_ON + SIS_3300_CTL_ENABLE_TRIGOUT + SIS_3300_CTL_TRIGOUT_ARMSTART; */

/* interrupt configuration register bit assignments */
#define SIS_3300_IRQ_ENABLE          0x00000800
#define SIS_3300_IRQ_ROAK            0x00001000
#define SIS_3300_IRQ_RORA            0x00000000
#define SIS_3300_IRQ_LEVELSHIFT               8
/* example: data = SIS_3300_IRQ_ROAK + SIS_3300_IRQ_ENABLE + irqlevel << SIS_3300_IRQ_LEVELSHIFT + irqvector; */

/* interrupt configuration register bit assignments */
/* source on/off bits */
#define SIS_3300_IRQ_ENABLE0         0x00000001  /* end of event IRQ source */
#define SIS_3300_IRQ_ENABLE1         0x00000002  /* end of last event IRQ source */
#define SIS_3300_IRQ_ENABLE2         0x00000004  /* reserved IRQ source */
#define SIS_3300_IRQ_ENABLE3         0x00000008  /* user input IRQ source */
#define SIS_3300_IRQ_DISABLE0        0x00010000  /* end of event IRQ source */
#define SIS_3300_IRQ_DISABLE1        0x00020000  /* end of last event IRQ source */
#define SIS_3300_IRQ_DISABLE2        0x00040000  /* reserved IRQ source */
#define SIS_3300_IRQ_DISABLE3        0x00080000  /* user input IRQ source */
/* status bit masks */
#define SIS_3300_IRQ_INTERNAL_STAT   0x04000000
#define SIS_3300_IRQ_STAT            0x08000000
#define SIS_3300_IRQ_SOURCE0_STAT    0x10000000
#define SIS_3300_IRQ_SOURCE1_STAT    0x20000000
#define SIS_3300_IRQ_SOURCE2_STAT    0x40000000
#define SIS_3300_IRQ_SOURCE3_STAT    0x80000000

/* acquisiton control register bit assignments */
/* on */
#define SIS_3300_ACQ_BANK1_CLOCK_ON  0x00000001  /* enable sample clock to bank 1 */
#define SIS_3300_ACQ_BANK2_CLOCK_ON  0x00000002  /* enable sample clock to bank 2 */
#define SIS_3300_ACQ_AUTOSTART_ON    0x00000010  /* auto start on */
#define SIS_3300_ACQ_MULTIEVENT_ON   0x00000020  /* multi event mode on */
#define SIS_3300_ACQ_STOPDELAY_ON    0x00000080  /* external stop delay on  */
#define SIS_3300_ACQ_STARTDELAY_ON   0x00000040  /* external start delay on */
#define SIS_3300_ACQ_FRONTSTART_ON   0x00000100  /* front panel start/stop on */
#define SIS_3300_ACQ_P2START_ON      0x00000200  /* enable P2 start/stop */
#define SIS_3300_ACQ_GATE_ON         0x00000400  /* enable front panel gate mode */
#define SIS_3300_ACQ_CLKSRC0         0x70000000  /* enable internal 100 MHz */
#define SIS_3300_ACQ_CLKSRC1         0x00001000  /* enable internal 50 MHz */
#define SIS_3300_ACQ_CLKSRC2         0x00002000  /* enable internal 25 MHz */
#define SIS_3300_ACQ_CLKSRC3         0x00003000  /* enable internal 12.5 MHz */
#define SIS_3300_ACQ_CLKSRC4         0x00004000  /* enable internal 6.25 MHz */
#define SIS_3300_ACQ_CLKSRC5         0x00005000  /* enable internal 3.125 MHz */
#define SIS_3300_ACQ_CLKSRC6         0x00006000  /* enable front panel clock */
#define SIS_3300_ACQ_CLKSRC7         0x00007000  /* enable P2 clock */
/* off */
#define SIS_3300_ACQ_BANK1_CLOCK_OFF 0x00010000  /* disable sample clock to bank 1 */
#define SIS_3300_ACQ_BANK2_CLOCK_OFF 0x00020000  /* disable sample clock to bank 2 */
#define SIS_3300_ACQ_AUTOSTART_OFF   0x00100000  /* auto start off */
#define SIS_3300_ACQ_MULTIEVENT_OFF  0x00200000  /* multi event mode off */
#define SIS_3300_ACQ_STOPDELAY_OFF   0x00400000  /* external stop delay off */
#define SIS_3300_ACQ_STARTDELAY_OFF  0x00800000  /* external start delay off  */
#define SIS_3300_ACQ_FRONTSTART_OFF  0x01000000  /* front panel start/stop off */
#define SIS_3300_ACQ_P2START_OFF     0x02000000  /* disable P2 start/stop */
#define SIS_3300_ACQ_GATE_OFF        0x04000000  /* enable front panel start/stop mode */
#define SIS_3300_ACQ_CLEAR_CLKSRC    0x70000000  /* clear all clock sources */

/* event configuration register bit assignments */
#define SIS_3300_EVT_128KSAMPLES     0x00000000
#define SIS_3300_EVT_16KSAMPLES      0x00000001
#define SIS_3300_EVT_4KSAMPLES       0x00000002
#define SIS_3300_EVT_2KSAMPLES       0x00000003
#define SIS_3300_EVT_1KSAMPLES       0x00000004
#define SIS_3300_EVT_512SAMPLES      0x00000005
#define SIS_3300_EVT_256SAMPLES      0x00000006
#define SIS_3300_EVT_128SAMPLES      0x00000007
#define SIS_3300_EVT_WRAP_ON         0x00000008
#define SIS_3300_EVT_WRAP_OFF        0x00000000

/* misc */
#define BLT_BUFFER_SIZE                        0x202000
#define SIS_3300_TIMESTAMP_PREDIVIDER           0x1c
#define SIS_3300_KEY_START_AUTO_BANK_SWITCH     0x40
#define SIS_3300_KEY_STOP_AUTO_BANK_SWITCH      0x44
#define SIS_3300_KEY_BANK1_FULL_FLAG            0x48
#define SIS_3300_KEY_BANK2_FULL_FLAG            0x4c
#define SIS_3300_TRIGGER_FLAG_CLR_CNT_ALL_ADC   0x10001c
#define SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC12     0x20001c
#define SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC34     0x28001c
#define SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC56     0x30001c
#define SIS_3300_TRIGGER_FLAG_CLR_CNT_ADC78     0x38001c
#define SIS_3300_CLOCK_PREDIVIDER_ALL_ADC       0x100020
#define SIS_3300_CLOCK_PREDIVIDER_ADC12         0x200020
#define SIS_3300_CLOCK_PREDIVIDER_ADC34         0x280020
#define SIS_3300_CLOCK_PREDIVIDER_ADC56         0x300020
#define SIS_3300_CLOCK_PREDIVIDER_ADC78         0x380020
#define SIS_3300_NO_OF_SAMPLE_ALL_ADC           0x100024
#define SIS_3300_NO_OF_SAMPLE_ADC12             0x200024
#define SIS_3300_NO_OF_SAMPLE_ADC34             0x280024
#define SIS_3300_NO_OF_SAMPLE_ADC56             0x300024
#define SIS_3300_NO_OF_SAMPLE_ADC78             0x380024
#define SIS_3300_TRIGGER_SETUP_ALL_ADC          0x100028
#define SIS_3300_TRIGGER_SETUP_ADC12            0x200028
#define SIS_3300_TRIGGER_SETUP_ADC34            0x280028
#define SIS_3300_TRIGGER_SETUP_ADC56            0x300028
#define SIS_3300_TRIGGER_SETUP_ADC78            0x380028
#define SIS_3300_MAX_NUMBER_OF_EVENTS_ALL_ADC   0x10002c
#define SIS_3300_MAX_NUMBER_OF_EVENTS_ADC12     0x20002c
#define SIS_3300_MAX_NUMBER_OF_EVENTS_ADC34     0x28002c
#define SIS_3300_MAX_NUMBER_OF_EVENTS_ADC56     0x30002c
#define SIS_3300_MAX_NUMBER_OF_EVENTS_ADC78     0x38002c
#define SIS_3300_EVENT_DIRECTORY_BANK1_ALL_ADC  0x101000
#define SIS_3300_EVENT_DIRECTORY_BANK2_ALL_ADC  0x102000
#define SIS_3300_ACTUAL_SAMPLE_VALUE_ADC12      0x200018
#define SIS_3300_ACTUAL_SAMPLE_VALUE_ADC34      0x280018
#define SIS_3300_ACTUAL_SAMPLE_VALUE_ADC56      0x300018
#define SIS_3300_ACTUAL_SAMPLE_VALUE_ADC78      0x380018

/* data structure for sis3300 */

typedef enum {
  SIS_3300_SINGLE_EVT = 0,
  SIS_3300_MULTI_EVT = 1,
  SIS_3300_GATE_CHAINING = 2
} SIS_3300_ACQMODE;

#define NOF_CHANNELS	8
#define NOF_GROUPS		4

struct s_sis_3300 {
	unsigned long vmeAddr;					/* phys addr given by module switches */
	volatile unsigned char * baseAddr;		/* addr mapped via find_controller() */

	char moduleName[100];
	int serial; 							/* MARaBOU's serial number */

	uint8_t verbose;
	uint8_t dumpRegsOnInit; 				/* dump register contents to stdout after init */

	struct s_bma * bma; 					/* block mode access */
	uint32_t bltBufferSize;
	uint8_t *bltBuffer;
	int blockTransOn;
	
	uint32_t subEvtId;
	uint8_t acqMode;						/* params to be set via settings file */
	uint8_t pageSize;
	uint8_t clockSource;
	uint16_t startDelay;
	uint16_t stopDelay;
	uint8_t frontStartStop;	
	uint8_t autoStart;
	uint8_t autoBankSwitch;
	uint8_t wrapAround;
	uint8_t triggerArmedStart;
	uint8_t triggerOut;
	uint8_t inverseTriggerOut;
	uint8_t triggerRouting;
	uint8_t bankFullLemo;
	
	uint16_t maxNumEvts;
	uint8_t channelGrpOn[NOF_GROUPS];
	uint8_t triggerOn[NOF_CHANNELS];
	uint8_t peakingTime[NOF_GROUPS];
	uint8_t gapTime[NOF_GROUPS];
	uint16_t threshold[NOF_CHANNELS];
	uint8_t triggerMode[NOF_GROUPS];
	uint8_t pulsMode[NOF_GROUPS];
	uint8_t pulsLength[NOF_GROUPS];
};

#endif
