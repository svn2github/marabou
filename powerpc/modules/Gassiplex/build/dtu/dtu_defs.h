/****	Headerfile for DTU registers */
/****	E.Lins 16.6.99 */

/* $Id: dtu_defs.h,v 1.12 2003/06/23 10:49:12 hadaq Exp $ */
/* $Source: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/dtu/dtu_defs.h,v $ */

/*	define certain detector system: _RICH, _MDC, _TOF, _SHOWER */

#define _SHOWER
#define _RICH
#define _MDC
#define _TOF

/*****	CTU registers*/

#define STATUS1		0x00
#define	DEAD1		0x04
#define CODE1		0x08
#define TAG1		0x0c
#define STATUS2		0x10
#define	DEAD2		0x14
#define CODE2		0x18
#define TAG2		0x1c
#define TRIG1		0x20
#define DELAY2		0x24
#define RATE2		0x28
#define FIFO		0x2c
#define MEM_DATA1	0x30
#define MEM_DATA2	0x34
#define MEM_RESET	0x38
#define MAGIC		0x3c		/* allways read as 0x51 */


/****	common DTU registers */

#define STATUS1		0x00
#define	DEAD1		0x04
#define CODE1		0x08
#define TAG1		0x0c
#define STATUS2		0x10
#define	DEAD2		0x14
#define CODE2		0x18
#define TAG2		0x1c
#define	FIFO1		0x20
#define FIFO2		0x24
#define RELEASE1	0x28
#define RELEASE2	0x2c
#define COUNT1		0x30
#define COUNT2    	0x34
#define MAGIC		0x3c


/****  SHOWER registers*/

#ifdef _SHOWER

#define RB_DATA		0x40
#define RB_COM		0x44
#define RB_ACK		0x48
#define RB_DELAY	0x4c
#define IPC_DATA	0x50
#define IPC_DATA_BCAST	0x54
#define IPC_ADDR	0x58
#define IPC_ADDR_BCAST	0x5c

#endif

/**** RICH registers */

#ifdef _RICH

/* These offsets are only valid for the new FINALDTU design.
 * They won't work with the old ones anymore.
 * Using old designs with these offsets is not recommended.
 */

#define RICH_OFF        0x40	/* reset, IPU_ON et. al.               */
#define RICH_DEAD       0x44	/* Deadtime for AK problem             */
#define RICH_MM         0x48	/* Mode and Lemo out control           */
#define RICH_CD         0x4c	/* CTU/DTU as decision source          */
#define RICH_DEC        0x50	/* decision bits for triggers          */
#define RICH_MODE		0x54	/* Mode register                       */
#define RICH_UNUSED0    0x58	/* Reserved for future use             */
#define RICH_UNUSED1    0x5c	/* Reserved for future use             */
#define RICH_UNUSED2    0x60	/* Reserved for future use             */
#define RICH_UNUSED3    0x64	/* Reserved for future use             */
#define RICH_UNUSED4    0x68	/* Reserved for future use             */
#define RICH_UNUSED5    0x6c	/* Reserved for future use             */
#define RICH_UNUSED6    0x70	/* Reserved for future use             */
#define RICH_UNUSED7 	0x74	/* Reserved for future use             */
#define RICH_DATA		0x78	/* Data register                       */
#define RICH_OFFSET		0x7c	/* Offset register for banks           */

/* The finaldtu design has from v2r8 and up an offset structure     
 * for information (readonly) registers, as due to the D32/D8 accesses
 * the Xilinx runs out of TBUFs.
 * There's no other way to implement these registers. 
 * That's not a software but a hardware limitation. Sorry.
 */

#define OFFSET_VER	0x00		/* Version and Revision                */
#define OFFSET_QUP      0x01	/* LVL1 queue status                   */
#define OFFSET_QUA      0x02	/* LVL2 queue status                   */
#define OFFSET_BE       0x03	/* Busy and Error register             */
#define OFFSET_TCS	0x04		/* last TCs and RCs into DTU           */
#define OFFSET_MSM	0x05		/* Arbitration states                  */

/* Bit definitions */

/* OFF register bit definitions */
#define BIT_IPUON 		7		/* IPUON bit                    */
#define BIT_BEGRUNON	6		/* Switch /BEGRUN signal on     */
#define BIT_INVDEC		5		/* Invert Decision              */
#define BIT_STOP		1		/* STOP bit                     */
#define BIT_RESET		0		/* DTU internal RESET signal    */

/* Frontend mode definitions */
#define FE_MODE_NORMAL	0		/* normal event with thresholds */
#define FE_MODE_EMPTY	1		/* empty event with thr=1023    */
#define FE_MODE_FULL	2		/* full event with thr=0        */
#define FE_MODE_TEST	3		/* test mode - DO NOT USE !     */

#endif

/**** MDC registers */

#ifdef _MDC
#define SAMDEAD   0x38
#endif

/**** TOF registers */

#ifdef _TOF

#define TOF_BUSY  0x38

#endif
