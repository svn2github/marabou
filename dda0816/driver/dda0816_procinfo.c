/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#define __NO_VERSION__

#include <dda0816_P.h>

#include <linux/proc_fs.h>

extern int dda0816_procinfo (char *buf, char **start, off_t fpos, 
                                int length, int dummy);



void register_procinfo(void) {

  struct proc_dir_entry *ent;
  DBGin("register_procinfo");

  if ((ent = create_proc_entry("dda0816", S_IRUGO | S_IWUSR, NULL)) != NULL) {
	 ent->read_proc = dda0816_procinfo;
	 ent->write_proc = NULL;
  }


  DBGout();
}

void unregister_procinfo(void) {
  DBGin("unregister_procinfo");
  remove_proc_entry("dda0816", NULL);

  DBGout();
}

int dda0816_procinfo (char *buf, char **start, off_t fpos, 
                                int length, int dummy){

     int retval=0;
     DBGin("dda0816_procinfo");
     {
                char *p;
		int n, i, r;
		u8 dummy;
		extern struct dda0816_kernel dda0816_resource[16];
	
	        p = buf; /* buf is defined implicitely */
		p += sprintf(p,"Proc entry from dda0816\n");
		outb(DDA08_CCSR_DAC01, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_CCSR);
		p += sprintf(p,"CCSR_DAC01: 0x%x (DAC0: ", dummy);
		if (dummy & DDA08_TG0) p += sprintf(p,"Update ");
		if (dummy & DDA08_UB0) p += sprintf(p,"Bipolar ");
		switch (dummy & DDA08_SP0) {
		case 0x0:	p += sprintf(p,"Undefined Range ");
				break;
		case DDA08_10V:	p += sprintf(p,"10V span ");
				break;
		case DDA08_5V:	p += sprintf(p,"5V span or current ");
				break;
		case DDA08_20V:	p += sprintf(p,"20V span ");
				break;
		default:	p += sprintf(p,"What? ");
		}	
		p += sprintf(p,"DAC1:");
		if (dummy & DDA08_TG1) p += sprintf(p,"Update ");
		if (dummy & DDA08_UB1) p += sprintf(p,"Bipolar ");
		switch ((dummy & DDA08_SP1) >> 4 ) {
		case 0x0:	p += sprintf(p,"Undefined Range ");
				break;
		case DDA08_10V:	p += sprintf(p,"10V span ");
				break;
		case DDA08_5V:	p += sprintf(p,"5V span or current ");
				break;
		case DDA08_20V:	p += sprintf(p,"20V span ");
				break;
		default:	p += sprintf(p,"What? ");
		}	
		p += sprintf(p,")\n");
		outb(DDA08_PACER, DDA08_BASE+DDA08_DP);
		dummy = inb(DDA08_DATAL);
		p += sprintf(p, "Pacer Clock: %d\n", dummy);
		outb(DDA08_OCLCK, DDA08_BASE+DDA08_DP);
		dummy = inb(DDA08_DATAL);
		p += sprintf(p, "Output Clock: %d\n", dummy);
		outb(DDA08_CCSR_CVSR0, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_CCSR);
		p += sprintf(p,"CVSR0: 0x%x\n", dummy);
		outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_BCSR);
		p += sprintf(p,"RSR: 0x%x (", dummy);
		if (dummy & DDA08_TRE) p += sprintf(p, "TRE ");
		if (dummy & DDA08_GAE) p += sprintf(p, "GAE ");
		if (dummy & DDA08_OTR) p += sprintf(p, "OTR ");
		if (dummy & DDA08_OON) p += sprintf(p, "OON ");
		if (dummy & DDA08_RCT) p += sprintf(p, "RCT ");
		if (dummy & DDA08_UPD) p += sprintf(p, "UPD ");
		p += sprintf(p, ")\n");
		outb(DDA08_BCSR_BSR, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_BCSR);
		p += sprintf(p,"BSR: 0x%x (", dummy);
		if (dummy & DDA08_DOE) p += sprintf(p, "DOE ");
		if (dummy & DDA08_SEQ) p += sprintf(p, "SEQ ");
		switch (dummy & DDA08_IL) {
		case DDA08_ILOFF:	p += sprintf(p, "Interrupt disabled ");
			break;
		case DDA08_IL3:	p += sprintf(p, "Interrupt Level 3 ");
			break;
		case DDA08_IL5:	p += sprintf(p, "Interrupt Level 5 ");
			break;
		case DDA08_IL7:	p += sprintf(p, "Interrupt Level 7 ");
			break;
		case DDA08_IL10:	p += sprintf(p, "Interrupt Level 10 ");
			break;
		case DDA08_IL11:	p += sprintf(p, "Interrupt Level 11 ");
			break;
		case DDA08_IL15:	p += sprintf(p, "Interrupt Level 15 ");
			break;
		case DDA08_ILUNDEF:	p += sprintf(p, "Interrupt undefined ");
			break;
		}
		p += sprintf(p, ")\n");
		p += sprintf(p, "Interrupts: %d\n", dda0816_resource[0].irq);
		outb(DDA08_BCSR_CSR, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_BCSR);
		p += sprintf(p,"CSR: 0x%x (", dummy);
		if (dummy & DDA08_OP) p += sprintf(p, "OP ");
		if (dummy & DDA08_TRP) p += sprintf(p, "TRP ");
		if (dummy & DDA08_GAP) p += sprintf(p, "GAP ");
		if (dummy & DDA08_CP) p += sprintf(p, "CP ");
		switch (dummy & DDA08_CS) {
		case 0: p += sprintf(p, "Clock source disabled ");
			break;
		case 1: p += sprintf(p, "Onboard Clock ");
			break;
		case 2: p += sprintf(p, "External Clock ");
			break;
		case 3: p += sprintf(p, "Software Clock ");
		}
		p += sprintf(p, ")\n");
		outb(DDA08_BCSR_MISC, DDA08_BASE+DDA08_SP);
		dummy = inb (DDA08_BASE+DDA08_BCSR);
		p += sprintf(p,"MISC: 0x%x (", dummy);
		if (dummy & DDA08_CLI) p += sprintf(p, "CLI ");
		if (dummy & DDA08_GAI) p += sprintf(p, "GAI ");
		if (dummy & DDA08_TRI) p += sprintf(p, "TRI ");
		if (!dummy & DDA08_16) p += sprintf(p, "16channel ");
		switch ((dummy & DDA08_PS) >> 5) {
		case 0:	p += sprintf(p, "1Hz ");
			break;
		case 1:	p += sprintf(p, "10Hz ");
			break;
		case 2:	p += sprintf(p, "100Hz ");
			break;
		case 3:	p += sprintf(p, "1kHz ");
			break;
		case 4:	p += sprintf(p, "10kHz ");
			break;
		case 5:	p += sprintf(p, "100kHz ");
			break;
		case 6:	p += sprintf(p, "1MHz ");
			break;
		case 7:	p += sprintf(p, "undefined PSCL ");
			break;
		}
		p += sprintf(p, ")\n");

		p += sprintf(p, "Allokierte Curve Buffer:\n");
		for (n=0; n<16; n++) {
		  p += sprintf(p, "%d, Adr: 0x%lx Size: %d SoftScale: %d Scaler> %d Step: %d\n",
			 n, dda0816_resource[n].curve.points,
			 dda0816_resource[n].curve.size,
			 dda0816_resource[n].curve.SoftScale,
			 dda0816_resource[n].scaler,
			 dda0816_resource[n].step);
/*		  if (dda0816_resource[n].curve.size > 0) {
		    p += sprintf(p, "Points:\n");
		    for (i=0; i<dda0816_resource[n].curve.size; i=i+4) {
		      for (r=i; 
			   (r<dda0816_resource[n].curve.size) && (r< i+4);
		 	   r++) {
		        p += sprintf(p, "0x%x\t", dda0816_resource[n].curve.points[r]);
		      }
		      p += sprintf(p, "\n");
		    }
		  }
		  */
		}
		retval = p - buf;
        
     }
     DBGout();
     return retval;
}



