/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/

	/* Register definitions */
#define	DDA08_DATAL	0x00
#define	DDA08_DATAH	0x01
#define DDA08_DP	0x02 /* Data Pointer */
#define DDA08_BCSR	0x04
#define DDA08_CCSR	0x05 /* Channel Control Status Reg. */
#define DDA08_SP	0x06 /* Status Pointer */

#define	DDA08_PACER	0x10 /* Pointer to Pacer Clock */
#define	DDA08_OCLCK	0x11 /* Pointer to Output Clock */

#define DDA08_CCSR_DAC01	0x0
#define DDA08_CCSR_DAC23	0x1
#define DDA08_CCSR_CVSR0	0x2
#define DDA08_CCSR_CVSR1	0x3

#define DDA08_BCSR_RSR	0x0
#define DDA08_BCSR_BSR	0x1
#define DDA08_BCSR_CSR	0x2
#define DDA08_BCSR_MISC	0x3

#define DDA08_CHANNEL	(MINOR(device)&0xf)
#define DDA08_NRCHAN	8

#define DDA08_BASE	0x300

#define DDA08_TG0	0x8
#define DDA08_TG1	0x80
#define DDA08_SP0	0x6
#define DDA08_SP1	0x60
#define	DDA08_5V	0x4
#define	DDA08_10V	0x2
#define	DDA08_20V	0x6
#define DDA08_UB0	0x1
#define DDA08_UB1	0x10

#define DDA08_TRE	0x80	/* Enable Trigger Input */
#define DDA08_GAE	0x40	/* Enable Gate Input */
#define DDA08_OTR	0x20	/* Output Timer on */
#define DDA08_OON	0x10	/* Enable Output Clock */
#define DDA08_RCT	0x08	/* Enbale Onboard Period Pacer Clock */
#define	DDA08_SWT	0x02	/* Start Update */
#define DDA08_UPD	0x01	/* Update happened */

#define DDA08_DOE	0x10	/* Enable Analog Output */
#define	DDA08_SEQ	0x8	/* Auto Increment Output channel */
#define	DDA08_IL	0x7	/* Interrupt Level */
#define DDA08_ILOFF	0x0	/* Interrupt Disabled */
#define DDA08_IL3	0x1	/* Interrupt Level 3 */
#define DDA08_IL5	0x2	/* Interrupt Level 5 */
#define DDA08_IL7	0x3	/* Interrupt Level 7 */
#define DDA08_IL10	0x4	/* Interrupt Level 10 */
#define DDA08_IL11	0x5	/* Interrupt Level 11 */
#define DDA08_IL15	0x6	/* Interrupt Level 15 */
#define DDA08_ILUNDEF	0x7	/* Interrupt Level 15 */

#define DDA08_OP	0x20	/* Output Clock Polarity */
#define DDA08_TRP	0x10	/* Trigger Polarity */
#define	DDA08_GAP	0x8	/* Gate Polarity */
#define	DDA08_CP	0x4	/* External Pacer Clock Polarity */
#define DDA08_CS	0x3	/* Clock Source */
#define DDA08_CS_OFF	0x0	/* Clock Source Off */
#define DDA08_CS_ONB	0x1	/* Clock Source Onboard */
#define DDA08_CS_EXT	0x2	/* Clock Source External */
#define DDA08_CS_SOFT	0x3	/* Clock Source Software */

#define DDA08_PS	0xe0	/* Prescaler Clock Period */
#define DDA08_PS_1HZ	0x00	/* Prescaler Clock Period */
#define DDA08_PS_10HZ	0x20	/* Prescaler Clock Period */
#define DDA08_PS_100HZ	0x40	/* Prescaler Clock Period */
#define DDA08_PS_1KHZ	0x60	/* Prescaler Clock Period */
#define DDA08_PS_10KHZ	0x80	/* Prescaler Clock Period */
#define DDA08_PS_100KHZ	0xa0	/* Prescaler Clock Period */
#define DDA08_PS_1MHZ	0xc0	/* Prescaler Clock Period */
#define	DDA08_CLI	0x08	/* Clock Input Line Level */
#define	DDA08_GAI	0x04	/* Gate Input Line Level */
#define	DDA08_TRI	0x02	/* Trigger Input Line Level */
#define DDA08_16	0x01	/* Indicates Board Type */

#define DDA08_MAXPOINTS	262144	/* Size of max Curve Field */

	struct dda0816_curve {
		int * points ;
		int size;
		int SoftScale;
	};

	struct dda0816_kernel {
		struct dda0816_curve	curve;
		int	scaler;
		int	step;
		int irq;
	};

