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
#include <linux/mm.h>
#include <linux/sysctl.h>
#include <linux/ctype.h>
#include <dda0816_sysctl.h>

/*
 * This Template implements the SYSCTL basics, and handler/strategy routines
 * Users may implement own routines and hook them up with the 'handler'		
 * and 'strategy' methods of sysctl.
 * 
 *
 */

#define SYSCTL_dda0816 1

/* ----- Prototypes */

extern int dda0816_dointvec(ctl_table *table, int write, struct file *filp,
		  void *buffer, size_t *lenp);

extern int dda0816_dostring(ctl_table *table, int write, struct file *filp,
		  void *buffer, size_t *lenp);


extern int dda0816_sysctl_string(ctl_table *table, int *name, int nlen,
		  void *oldval, size_t *oldlenp,
		  void *newval, size_t newlen, void **context);


/* ----- global variables accessible through /proc/sys/dda0816 */



/* ----- the sysctl table */

ctl_table dda0816_sysctl_table[] = {
   { SYSCTL_DBGMASK, "dbgMask",(void *) &dbgMask, 1*sizeof(int), 
		 0644, NULL, &dda0816_dointvec , NULL  },
    
   {0}
};

/* ----- the main directory entry in /proc/sys */

ctl_table dda0816_sys_table[] = {
	    {SYSCTL_dda0816,"dda0816",      NULL, 0, 0555, 
                 &dda0816_sysctl_table},	
	    {0}	
};

/* ----- register and unregister entrys */

struct ctl_table_header *dda0816_systable;

        void register_systables(){
	    dda0816_systable = register_sysctl_table( &dda0816_sys_table, 0 );
        }

        void unregister_systables(){
		unregister_sysctl_table(dda0816_systable);
        }


/* ----- default proc handlers */

int dda0816_dointvec(ctl_table *table, int write, struct file *filp,
		  void *buffer, size_t *lenp)
         {
	int *i, vleft, first=1, len, left, neg, val;
	char dummy;

	#define TMPBUFLEN 20
	char buf[TMPBUFLEN], *p;
	
	if (!table->data || !table->maxlen || !*lenp ||
	    (filp->f_pos && !write)) {
		*lenp = 0;
		return 0;
	}
	
	i = (int *) table->data;
	vleft = table->maxlen / sizeof(int);
	left = *lenp;
	
	for (; left && vleft--; i++, first=0) {
		if (write) {
			while (left) {
				get_user(dummy, (char *) buffer);
				if (!isspace(dummy)) break;
				left--, ((char *) buffer)++;
			}
			if (!left)
				break;
			neg = 0;
			len = left;
			if (len > TMPBUFLEN-1)
				len = TMPBUFLEN-1;

#if LINUX_VERSION_CODE > 0x20200
			copy_from_user(buf, buffer, len);
#else
#if LINUX_VERSION_CODE > 0x20100
			memcpy_fromio(buf, buffer, len);
#else
			memcpy_fromfs(buf, buffer, len);
#endif
#endif

			buf[len] = 0;
			p = buf;
			if (*p == '-' && left > 1) {
				neg = 1;
				left--, p++;
			}
			if (*p < '0' || *p > '9')
				break;
			val = simple_strtoul(p, &p, 0);
			len = p-buf;
			if ((len < left) && *p && !isspace(*p))
				break;
			if (neg)
				val = -val;
			buffer += len;
			left -= len;
			*i = val;
		} else {
			p = buf;
			if (!first)
				*p++ = '\t';
			sprintf(p, "%d", *i);
			len = strlen(buf);
			if (len > left)
				len = left;
#if	LINUX_VERSION_CODE > 0x20200
			copy_to_user(buffer, buf, len);
#else
			memcpy_toio(buffer, buf, len);
#endif
			left -= len;
			buffer += len;
		}
	}

	if (!write && !first && left) {
		put_user('\n', (char *) buffer);
		left--, buffer++;
	}
	if (write) {
		p = (char *) buffer;
		while (left) {
			get_user(dummy, p++);
			if (!isspace(dummy)) break;
			left--;
		}
	}
	if (write && first)
		return -EINVAL;
	*lenp -= left;
	filp->f_pos += *lenp;
	return 0;
}


int dda0816_dostring(ctl_table *table, int write, struct file *filp,
		  void *buffer, size_t *lenp)
{
	int len;
	char *p, c;
	
	if (!table->data || !table->maxlen || !*lenp ||
	    (filp->f_pos && !write)) {
		*lenp = 0;
		return 0;
	}
	
	if (write) {
		len = 0;
		p = buffer;
		while (len < *lenp) {
			get_user(c, p++);
		        if (c == 0 || c == '\n')	break;
			len++;
		}
		if (len >= table->maxlen)
			len = table->maxlen-1;

#if LINUX_VERSION_CODE > 0x20200
		copy_from_user(table->data, buffer, len);
#else
#if LINUX_VERSION_CODE > 0x20100
		memcpy_fromio(table->data, buffer, len);
#else
		memcpy_fromfs(table->data, buffer, len);
#endif
#endif

		((char *) table->data)[len] = 0;
		filp->f_pos += *lenp;
	} else {
		len = strlen(table->data);
		if (len > table->maxlen)
			len = table->maxlen;
		if (len > *lenp)
			len = *lenp;
		if (len)
#if LINUX_VERSION_CODE > 0x20200
			copy_to_user(buffer, table->data, len);
#else
			memcpy_toio(buffer, table->data, len);
#endif
		if (len < *lenp) {
			put_user('\n', ((char *) buffer) + len);
			len++;
		}
		*lenp = len;
		filp->f_pos += len;
	}
	return 0;
}

/* ----- strategy handlers */

int dda0816_sysctl_string(ctl_table *table, int *name, int nlen,
		  void *oldval, size_t *oldlenp,
		  void *newval, size_t newlen, void **context)
{
	int l, len;
	
	if (!table->data || !table->maxlen) 
		return -ENOTDIR;
	
	if (oldval && oldlenp) {
		get_user(len, oldlenp);
		if (len) {
			l = strlen(table->data);
			if (len > l) len = l;
			if (len >= table->maxlen)
				len = table->maxlen;
#if LINUX_VERSION_CODE > 0x20200
			copy_to_user(oldval, table->data, len);
#else
			memcpy_toio(oldval, table->data, len);
#endif
			put_user(0, ((char *) oldval) + len);
			put_user(len, oldlenp);
		}
	}
	if (newval && newlen) {
		len = newlen;
		if (len > table->maxlen)
			len = table->maxlen;

#if LINUX_VERSION_CODE > 0x20200
		copy_from_user(table->data, newval, len);
#else
#if LINUX_VERSION_CODE > 0x20100
		memcpy_fromio(table->data, newval, len);
#else
		memcpy_fromfs(table->data, newval, len);
#endif
#endif

		if (len == table->maxlen)
			len--;
		((char *) table->data)[len] = 0;
	}
	return 0;
}

