#ifdef __KERNEL__
extern int dda0816_write (struct inode *, struct file *, char *, int); 
extern int dda0816_ioctl ( struct inode *, struct file *, unsigned int, unsigned long ); 
extern int dda0816_open (struct inode *, struct file *); 
void dda0816_release (struct inode *, struct file *); 
#endif 
