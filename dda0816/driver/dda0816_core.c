/*
* Generated with LDDK Version 0.7-beta2 
*     (c) 1996,1997 Claus Schroeter (clausi@chemie.fu-berlin.de) all rights reserved,
*         lddk is availiable from the Linux Lab Project
*             http://www.llp.fu-berlin.de/
*         or by anonymus FTP
*             ftp://ftp.llp.fu-berlin.de/pub/linux/LINUX-LAB/LDDK
*
*/
#include <dda0816_P.h>
#include <linux/devfs_fs_kernel.h>

char kernel_version[] = UTS_RELEASE;

MODULE_AUTHOR("Klaus Steinberger <Klaus.Steinberger@Physik.Uni-Muenchen.DE>");
MODULE_DESCRIPTION("DDA08/16 Driver");
MODULE_LICENSE("GPL");


    int dda0816_major = dda0816_MAJOR; 
  
struct file_operations dda0816_fops = { 
write:		dda0816_write, 
ioctl:		dda0816_ioctl, 
open:		dda0816_open,
release:	dda0816_release
};


#ifdef __cplusplus
extern "C" {
#endif

	static devfs_handle_t devfs_handle;

int init_module(void)
{
  int ret;
  int irq_handler();
  extern struct dda0816_kernel dda0816_resource[];

  u8 dummy;
  int	n = 0;
  int 	nmax = 8;

  DBGin("init_module");
  	

  if ( devfs_register_chrdev(dda0816_major, "dda0816", &dda0816_fops))  {
    printk ("dda0816: unable to get major %d\n", dda0816_major);
    return -EIO;
  }

  devfs_handle = devfs_register(NULL, "dda0816", DEVFS_FL_DEFAULT,
	dda0816_major, 0, S_IFCHR | S_IRUGO | S_IWUSR, 
	&dda0816_fops, NULL);



  outb(DDA08_BCSR_MISC, DDA08_BASE+DDA08_SP);
  dummy = inb(DDA08_BASE+DDA08_BCSR);

  if ( dummy & DDA08_16 ) {
	printk("dda0816: Board is a DDA08\n");
	nmax = 8;
  } else {
	printk("dda0816: Board is a DDA16\n");
	nmax = 16;
  }
  outb(DDA08_BCSR_MISC, DDA08_BASE+DDA08_SP);
  dummy = 0;
  outb(dummy, DDA08_BASE+DDA08_BCSR);

  for (n=0; n < nmax; n++) {
	dda0816_resource[n].curve.points = (int *)NULL;	
	dda0816_resource[n].curve.size = 0;
	dda0816_resource[n].curve.SoftScale = 0;
	dda0816_resource[0].irq = 0;
  }
  for (n=0; n < 8; n++) {
	outb(n, DDA08_BASE+DDA08_DP);
	outb(0x00, DDA08_BASE+DDA08_DATAL);
	outb(0x08, DDA08_BASE+DDA08_DATAH);
  }
  outb(DDA08_CCSR_DAC01, DDA08_BASE+DDA08_SP);
  outb(0, DDA08_BASE+DDA08_CCSR);
  if (nmax == 16) {
	outb(DDA08_CCSR_DAC23, DDA08_BASE+DDA08_SP);
	outb(0, DDA08_BASE+DDA08_CCSR);
  }
  outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
  outb(0, DDA08_BASE+DDA08_BCSR);
  outb(DDA08_BCSR_CSR, DDA08_BASE+DDA08_SP);
  outb(0, DDA08_BASE+DDA08_BCSR);

  request_irq(11, irq_handler, SA_INTERRUPT, "DDA08/16", NULL);
  outb(DDA08_BCSR_BSR, DDA08_BASE+DDA08_SP);
  outb(DDA08_DOE|DDA08_IL11, DDA08_BASE+DDA08_BCSR);
			
  register_procinfo(); 
  register_systables();

  DBGout();
  return 0;
}

void cleanup_module(void)
{
  DBGin("cleanup_module");
  if (MOD_IN_USE) {
    printk("dda0816 : device busy, remove delayed\n");
  }

  {
			u8 dummy;
			int n;
			extern struct dda0816_kernel dda0816_resource[16];

			/* Switch off Analog Output + Interrupt */
			outb(DDA08_BCSR_BSR, DDA08_BASE+DDA08_SP);
			outb(0x0, DDA08_BASE+DDA08_BCSR);
			outb(DDA08_BCSR_RSR, DDA08_BASE+DDA08_SP);
			dummy = inb(DDA08_BASE+DDA08_BCSR) & !DDA08_RCT;
			outb(dummy, DDA08_BASE+DDA08_BCSR);
			free_irq(11, NULL);
			
			outb(0, DDA08_BASE+DDA08_SP);
			outb(0, DDA08_BASE+DDA08_CCSR);
			outb(1, DDA08_BASE+DDA08_SP);
			outb(0, DDA08_BASE+DDA08_CCSR);

			for (n=0; n<16; n++) {
				if (dda0816_resource[n].curve.points != NULL) {
					vfree(dda0816_resource[n].curve.points);
				}
			}
			
			printk("Have a nice day! \n");
		
  }



  devfs_unregister_chrdev(dda0816_major, "dda0816");
  devfs_unregister(devfs_handle);
  unregister_procinfo();
  unregister_systables();
  DBGout();
}


#ifdef __cplusplus
}
#endif

