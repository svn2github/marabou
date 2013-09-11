static const char rcsId[] =
	"$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/prog_pz2.c,v 1.12 2003/08/20 16:52:34 hadaq Exp $";
#define _POSIX_C_SOURCE 199506L

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>

#include <stdio.h>
#include <syslog.h>
#include <time.h>

#include <lvme.h>

#define CLOCK(x)     (x)
#define DATA(x)      (x<<0x1)
#define PROGRAMN(x)  (x<<0x2)
#define RESETN(x)    (x<<0x3)

#define READ_BUFFER_SIZE  8192

#define NDEBUG

/* access by lvme library */
#ifndef NDEBUG
#define WRITE(p)   { LVme_setL(lvme, p , (unsigned long) output); syslog(LOG_DEBUG, "prog_pz2: WRITE_debug: %2.2x ", output); }
#else							/* !NDEBUG */
#define WRITE(p)   { LVme_setL(lvme, p , (unsigned long) output); }
#endif							/* NDEBUG */
#define READ(p)     { LVme_getL(lvme, p); }

int prog_pz2(char *filename, unsigned long base, int debug, const char *name, LVme *lvme)
{
	unsigned char bit;
	unsigned char output = 0;
	int buffer_count = 0;
	int read_bytes = 0;
	unsigned long offset = 0;

	FILE *fp;
	unsigned char buffer[READ_BUFFER_SIZE + 1];


	if (NULL == (fp = fopen(filename, "r"))) {
		syslog(LOG_ERR, "prog_pz2: Error loading input-file: %s", filename);
		return -1;
	} else {

		offset = base;

		LVme_nop(lvme);
		READ(offset);

		output = 0;
		LVme_nop(lvme);
		WRITE(offset);			/* resets philips */

		output = output | PROGRAMN(1);
		WRITE(offset);

		output = output | RESETN(1);
		WRITE(offset);

		output = output & ~PROGRAMN(1);
		WRITE(offset);

		output = output | PROGRAMN(1);
		WRITE(offset);

		while (!feof(fp)) {
			read_bytes = fread(buffer, sizeof(unsigned char), READ_BUFFER_SIZE, fp);
			/* if the last bit is included , write all but last */
			if (read_bytes < READ_BUFFER_SIZE) {
				read_bytes--;
			}

			for (buffer_count = 0; buffer_count <= read_bytes - 1; buffer_count++) {

#ifndef NDEBUG
				syslog(LOG_DEBUG, "prog_pz2: read: %1.1d", (char) buffer[buffer_count]);
#endif							/* !NDEBUG */
				if (buffer[buffer_count] == '1' || buffer[buffer_count] == '0') {
					if (buffer[buffer_count] == '1')
						bit = 1;
					else
						bit = 0;

					if (bit == 1) {
						output = output | DATA(1) | CLOCK(1);
						WRITE(offset);
					} else {
						output = (output & ~DATA(1)) | CLOCK(1);	/* 8 (immer) + 2 * bit + 1 */
						WRITE(offset);
					}

					output = output & ~CLOCK(1);
					WRITE(offset);

#ifndef NDEBUG
					syslog(LOG_DEBUG, "prog_pz2: status: %lx", status);
#endif							/* !NDEBUG */

				}
			}

		}
		/* write last bit, that means skip last clock ! */

		if (buffer[buffer_count] == '1')
			bit = 1;
		else
			bit = 0;

		if (bit == 1)
			output = output | DATA(1);
		else
			output = output & ~DATA(1);

		WRITE(offset);

		output = output | CLOCK(1);
		WRITE(offset);
		LVme_nop(lvme);
	}
	fclose(fp);

	return 0;
}

#if 0
static int wait_some_time(int amount)
{
	struct timespec t = { 0, 0 };

	/*  int rfds[100];
	   struct timeval tv;
	   int retval; 
	 */
	/* works only on unix systems! */

	/*  FD_ZERO(&rfds);
	   FD_SET(0, &rfds); */
	/* Wait up to five seconds. */
	/*tv.tv_sec = 0;
	   tv.tv_usec = amount;
	   retval = select(1, &rfds, NULL, NULL, &tv);
	 */

	/*usleep (amount); */
	t.tv_sec = (time_t) amount;
	nanosleep(&t, NULL);

	return 1;
}
#endif
