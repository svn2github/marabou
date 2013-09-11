static const char rcsId[] =
	"$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/prog_sh.c,v 1.31 2003/08/20 16:52:34 hadaq Exp $";
#define _POSIX_C_SOURCE 199506L

/*
  Programming the Sharc

  $Id: prog_sh.c,v 1.31 2003/08/20 16:52:34 hadaq Exp $

  $Source: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/prog_sh.c,v $

*/

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>

#include <lvme.h>

#define CLOCK(x)     (x)
#define DATA(x)      (x<<0x1)
#define PROGRAMN(x)  (x<<0x2)
#define RESETN(x)    (x<<0x3)

#define READ_BUFFER_SIZE  4096
#define DMAC6_address  (0x1C * 0x4)	/* address of external port DMA control */

#define NDEBUG

/* access by lvme library */
#ifndef NDEBUG
#define WRITE(x,value)    LVme_setL (plvme, x , (unsigned long int) value) ; syslog(LOG_DEBUG, "prog_sh: WRITE_debug: %2.2lx to offset: %8.8lx", (unsigned long int) value, (unsigned long int) x);
#define READ(x)           LVme_getL(plvme, x); syslog(LOG_DEBUG, "prog_sh: READ_debug: offset: %8.8lx", x);
#else							/* !NDEBUG */
#define WRITE(x,value)    LVme_setL (plvme, x , (unsigned long int) value);
#define READ(x)           LVme_getL(plvme, x);
#endif							/* NDEBUG */

static void my_usleep(int i);
#if 0
static void my_usleep_too_slow(int i);
#endif

int prog_sh(char *filename, unsigned long base, int level, const char *name, LVme *plvme)
{
	FILE *fp;
	char buffer[READ_BUFFER_SIZE + 1];
	int debug = 0;

	int slow = 0;
	int singlestep = 0;
	unsigned long absolute_count = 0;
	unsigned int DMAC6_contents = 0;
	unsigned long int output;

	int runden;
	char dummy[100];

	syslog(LOG_INFO, "prog_sh: Program: %s at base %lx", filename, base);


	if (level == 0)				/* s s */
		slow = 1;

	if (level == 1)				/* s e */
		slow = 2;

	if (level == 2)				/* s s s */
		singlestep = 1;

	if (level == 3)				/* d d */
		debug = 1;

	if (level == 4)				/* e7, twice as fast */
		slow = 3;

	if (level == 5)				/* e7, twice as fast */
		slow = 4;



	if (NULL == (fp = fopen(filename, "r"))) {
		syslog(LOG_ERR, "prog_sh: Error loading input-file: %s", filename);
		return -1;
	} else {

		while (!feof(fp)) {

			if (!fgets(buffer, 20, fp)) {
				continue;
			}
			output = strtoul(buffer, 0, 16);

#ifndef NDEBUG
			if ((absolute_count & 0xff) == 0xff)
				syslog(LOG_DEBUG, "prog_sh: Still prog at %x", absolute_count);

			syslog(LOG_DEBUG, "prog_sh: count: %ld, read bytes: 0x%8.8lx", absolute_count, output);
#endif							/* !NDEBUG */

			LVme_nop(plvme);
			DMAC6_contents = READ(DMAC6_address + base);
			LVme_nop(plvme);
			READ(DMAC6_address + base);
			if (DMAC6_contents == 0xffffffff) {
				syslog(LOG_WARNING, "prog_sh: board crashed");
				return -1;
			}
			if ((absolute_count % 3) != ((DMAC6_contents & 0x18) >> 3))
				syslog(LOG_DEBUG, "prog_sh: STAGE: Buffer status: %x, absolute address: %lx (should be %lx)",
					   DMAC6_contents, absolute_count, absolute_count % 3);

			if ((DMAC6_contents & 0xc0) == 0) {
				syslog(LOG_DEBUG, "prog_sh: PMODE: Buffer status: %x, absolute address: %lx ", DMAC6_contents, absolute_count);
				my_usleep(100000);
				LVme_nop(plvme);
				DMAC6_contents = READ(DMAC6_address + base);
				READ(DMAC6_address + base);
				LVme_nop(plvme);
				syslog(LOG_DEBUG, "prog_sh: PMODE: Buffer status: %x, absolute address: %lx ", DMAC6_contents, absolute_count);
			}
#if 0
			syslog(LOG_DEBUG, "prog_sh: Buffer status: %x, absolute address: %x", DMAC6_contents, absolute_count);
#endif

			if (((DMAC6_contents & 0xc000) != 0)
				&& ((DMAC6_contents & 0x18) == 0x00)) {
				syslog(LOG_DEBUG, "prog_sh: Buffer status: %x, absolute address: %lx, programming too fast", DMAC6_contents,
					   absolute_count);

				runden = 0;
				while (((DMAC6_contents & 0xc000) != 0)
					   && ((DMAC6_contents & 0x18) == 0x00)) {
					my_usleep(10000);
					runden++;
					LVme_nop(plvme);
					DMAC6_contents = READ(DMAC6_address + base);	/* read lower 16 bit */
					READ(DMAC6_address + base);	/* read upper 16 bit */
					LVme_nop(plvme);
					my_usleep(10000);
					if (runden > 10) {
						syslog
							(LOG_DEBUG, "prog_sh: status: new stat %x, absolute address: %lx, programming too fast, fatal error",
							 DMAC6_contents, absolute_count);
						if (level < 2)
							return -1;
					}
					syslog(LOG_DEBUG, "prog_sh: Buffer status: %x, absolute address: %lx, (RETRY)", DMAC6_contents, absolute_count);
				}
			}

			LVme_nop(plvme);
			WRITE(base + 0x10, output);	/* 0x10 is the Boot-Fifo  */
			LVme_nop(plvme);

			if (singlestep == 1) {
				dummy[0] = getchar();
			}
			if ((absolute_count == 256) || (absolute_count == 512)) {
				my_usleep(100);
#if 0
				slow = 1;
				singlestep = 1;
				debug = 1;
#endif
			}
			if (slow == 1) {
				my_usleep(50);
			}
			if (slow == 2) {
				/* E7 slow */
				my_usleep(20);
			}
			if (slow == 3) {
				/* E7 slow, twice as fast as slow 2 */
				my_usleep(10);
			}
			if (slow == 4) {
				/* E7 slow, twice as fast as slow 2 */
				my_usleep(5);
			}
			absolute_count++;

		}
		syslog(LOG_DEBUG, "prog_sh: wrote %ld words", absolute_count);
	}

	fclose(fp);

	return 0;
}

static void my_usleep(int i)
{
	int a = 0;
	int b = 1;

	/*  usleep(i); */

	for (a = 0; a < i * 100; a++) {
		b = b + 1;
	}
}

#if 0
static void my_usleep_too_slow(int i)
{
	struct timespec t = { 0, 0 };

	t.tv_nsec = 1000 * (long int) i;
	nanosleep(&t, NULL);
}

#endif
