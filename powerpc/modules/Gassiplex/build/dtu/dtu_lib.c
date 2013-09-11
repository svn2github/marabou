static const char rcsId[] = "$Header: /misc/hadesprojects/daq/cvsroot/runctrl/daq_cas/vme/dtu/dtu_lib.c,v 1.78 2006/04/24 08:33:32 hadaq Exp $";

/************************************************/
/*                                              */
/* dtu_lib.c                                    */
/*                                              */
/* author: Erik Lins                            */
/*                                              */
/************************************************/

#define _BSD_SOURCE
#define _POSIX_C_SOURCE 199506L

#if HAVE_CONFIG_H
#include <config.h>
#endif							/* HAVE_CONFIG_H */

#include <unistd.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <time.h>
#include <lvme.h>

#include "../support/xild.h"

#include "dtu_defs.h"
#include "dtu_lib.h"
#include "dtu_basis.h"

#define INIT_CODE_LEN 64
unsigned char init_code1[] = { 0, 1, 4, 4, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,
	2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3
};

static int mapVme(LVme * lvme, const char *name, const Param * p)
{
	unsigned long int cardbase = 0;
	int rows = 0;
	int retVal = 0;

	if (Param_getInt(p, name, "cardbase", &rows, &cardbase) || (rows != 1)) {
		syslog(LOG_ERR, "%s: Parameter cardbase not found", name);
		retVal = -1;
	} else {
		if ((lvme = newLVme(cardbase, 0x100, 0x09)) == NULL) {
		retVal = -1;
		}
	}

	return retVal;
}

static void unmapVme(LVme * lvme)
{
	delLVme(lvme);
}

/*
 * CTU
 */
static int resetCtu(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;
	int i;
	int rows;
	unsigned long int match_flag = 0;
	unsigned long int delay_flag = 0;
	unsigned long int timeout_flag = 0;
	unsigned long int rate = 1;
	unsigned long int delay_on = 0;
	unsigned long int match_on = 0;
	unsigned long int timeout_on = 0;
	unsigned long int dead1 = 0x0a;
	unsigned long int dead2 = 0x0a;

	Param_getInt(p, name, "rate", &rows, &rate);
	Param_getInt(p, name, "delay_on", &rows, &delay_on);
	Param_getInt(p, name, "match_on", &rows, &match_on);
	Param_getInt(p, name, "timeout_on", &rows, &timeout_on);
	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		delay_flag = LVme_getL(lvme, STATUS1) & 0x10;
		timeout_flag = LVme_getL(lvme, STATUS1) & 0x20;
		match_flag = LVme_getL(lvme, DELAY2) & 0x40;

		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01 | delay_flag | timeout_flag);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00 | delay_flag | timeout_flag);

		LVme_setL(lvme, STATUS1, 0x0a | delay_flag | timeout_flag);
		LVme_nop(lvme);
		/* trigger code init only for old ctu design */
		if ((LVme_getL(lvme, MAGIC) & 0xff) == 0x51) {
			LVme_nop(lvme);
			for (i = 0; i < INIT_CODE_LEN; i++) {
				LVme_setL(lvme, TAG1, i);
				LVme_setL(lvme, CODE1, init_code1[i]);
			}
		}

		LVme_nop(lvme);
		LVme_setL(lvme, TAG1, 0x00);

		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, DELAY2, 0x8f | match_flag);
		LVme_setL(lvme, RATE2, 0x01);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x0a | delay_flag | timeout_flag);

		/* rateCtu */
		LVme_setL(lvme, RATE2, rate);

		/* int lvl2delayCtu(LVme *lvme, unsigned delay) */
		{
			unsigned long int delay = 0;
			unsigned int old_lvl2_delay;

			Param_getInt(p, name, "delay", &rows, &delay);

			LVme_nop(lvme);
			old_lvl2_delay = LVme_getL(lvme, DELAY2) & 0xc0;
			LVme_nop(lvme);
			LVme_setL(lvme, DELAY2, old_lvl2_delay | (delay & 0x0f));
			LVme_nop(lvme);
		}

		if (delay_on) {
			/* int delayonCtu(LVme *lvme) */
			unsigned long int match_flag;
			unsigned long int delay_flag;
			unsigned long int timeout_flag;
			unsigned long int old_status1;

			delay_flag = 0x10;
			LVme_nop(lvme);
			timeout_flag = LVme_getL(lvme, STATUS1) & 0x20;
			match_flag = LVme_getL(lvme, STATUS1) & 0x40;
			old_status1 = LVme_getL(lvme, STATUS1) & 0xff;

			LVme_nop(lvme);
			LVme_setL(lvme, STATUS1, old_status1 | 0x01);
			LVme_setL(lvme, STATUS1, old_status1 | delay_flag | timeout_flag | 0x01 | match_flag);
			LVme_setL(lvme, STATUS1, old_status1 | delay_flag | timeout_flag | match_flag);
			LVme_nop(lvme);
		} else {
			/* int delayoffCtu(LVme *lvme) */
			unsigned long int delay_flag;
			unsigned long int timeout_flag;
			unsigned long int old_status1;

			delay_flag = 0xef;
			LVme_nop(lvme);
			timeout_flag = LVme_getL(lvme, STATUS1) & 0x20;
			old_status1 = LVme_getL(lvme, STATUS1) & 0xff;

			LVme_nop(lvme);
			LVme_setL(lvme, STATUS1, (old_status1 | timeout_flag) & delay_flag);
			LVme_nop(lvme);
		}

		if (timeout_on) {
			/* int timeoutonCtu(LVme *lvme) */
			unsigned long int delay_flag;
			unsigned long int timeout_flag;
			unsigned long int old_status1;

			LVme_nop(lvme);
			delay_flag = LVme_getL(lvme, STATUS1) & 0x10;
			timeout_flag = 0x20;
			old_status1 = LVme_getL(lvme, STATUS1) & 0xff;

			LVme_nop(lvme);
			LVme_setL(lvme, STATUS1, old_status1 | delay_flag | timeout_flag);
			LVme_nop(lvme);
		} else {
			/* int timeoutoffCtu(LVme *lvme) */
			unsigned long int delay_flag;
			unsigned long int timeout_flag;
			unsigned long int old_status1;

			LVme_nop(lvme);
			delay_flag = LVme_getL(lvme, STATUS1) & 0x10;
			timeout_flag = 0xdf;
			old_status1 = LVme_getL(lvme, STATUS1) & 0xff;

			LVme_nop(lvme);
			LVme_setL(lvme, STATUS1, (old_status1 | delay_flag) & timeout_flag);
			LVme_nop(lvme);
		}

		if (match_on) {
			/* int matchonCtu(LVme *lvme) */
			unsigned long int old_delay;

			LVme_nop(lvme);
			old_delay = LVme_getL(lvme, DELAY2) & 0xcf;

			LVme_nop(lvme);
			LVme_setL(lvme, DELAY2, old_delay | 0x40);
			LVme_nop(lvme);
		} else {
			/* int matchoffCtu(LVme *lvme) */
			unsigned long int old_delay;

			LVme_nop(lvme);
			old_delay = LVme_getL(lvme, DELAY2) & 0xcf;

			LVme_nop(lvme);
			LVme_setL(lvme, DELAY2, old_delay & 0xbf);
			LVme_nop(lvme);
		}

		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}
	return retVal;
}

static int startCtu(const char *name, const Param * p)
{
	int retVal = 0;
	struct timespec t1 = { 1, 0 };
	struct timespec t2 = { 0, 100000000 };
	int counter = 50;
	LVme lvmeS, *lvme = &lvmeS;
	unsigned long int delay_flag1;
	unsigned long int timeout_flag1;

	if (0 == (retVal = mapVme(lvme, name, p))) {
		/* new ctu start stuff */
		LVme_nop(lvme);
		delay_flag1 = LVme_getL(lvme, STATUS1) & 0x10;
		timeout_flag1 = LVme_getL(lvme, STATUS1) & 0x20;

		if ((LVme_getL(lvme, MAGIC) & 0xff) == 0x51) {

			/* old CTU design without addon card */
			LVme_nop(lvme);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_setL(lvme, TRIG1, 0x80 | 0x10);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_setL(lvme, TRIG1, 0x00);
			LVme_nop(lvme);

		} else if ((LVme_getL(lvme, MAGIC) & 0xff) == 0x52) {

			/* new CTU design with addon card */
			LVme_nop(lvme);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_nop(lvme);
			LVme_setL(lvme, TRIG1, 0x80 | 0x0d);	/* new begin run code */
			LVme_nop(lvme);

			LVme_setL(lvme, TRIG1, 0x80 | 0x0d | 0x40);	/* trigger! */
			LVme_nop(lvme);
			nanosleep(&t1, NULL);
			LVme_setL(lvme, TRIG1, 0x00);
			LVme_nop(lvme);
			LVme_nop(lvme);

		} else {

			syslog(LOG_WARNING, "%s: Unknown CTU design", name);

		}

		/* wait one second, better for some readout systems */
		nanosleep(&t1, NULL);

		LVme_nop(lvme);
		LVme_setL(lvme, STATUS1, 0x02 | delay_flag1 | timeout_flag1);
		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}

	while ((1 != Dtu_running(name, p)) && (0 != counter)) {
		nanosleep(&t2, NULL);
		counter--;
	}

	return retVal;
}

static int stopCtu(const char *name, const Param * p)
{
	int retVal = 0;
	struct timespec t1 = { 1, 0 };
	struct timespec t2 = { 0, 100000000 };
	int counter = 50;
	LVme lvmeS, *lvme = &lvmeS;
	unsigned long int delay_flag = 0;
	unsigned long int timeout_flag = 0;

	if (0 == (retVal = mapVme(lvme, name, p))) {
		/* inhibit trigger inputs */
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS1, 0x0a | delay_flag | timeout_flag);

		/* wait one second and hope that busy is gone */
		nanosleep(&t1, NULL);

		/* new ctu stuff */
		LVme_nop(lvme);
		delay_flag = LVme_getL(lvme, STATUS1) & 0x10;
		timeout_flag = LVme_getL(lvme, STATUS1) & 0x20;
		if ((LVme_getL(lvme, MAGIC) & 0xff) == 0x51) {
			/* old CTU design without addon card */
			LVme_nop(lvme);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_setL(lvme, TRIG1, 0x80 | 0x20);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_setL(lvme, TRIG1, 0x00);
			LVme_nop(lvme);
		} else if ((LVme_getL(lvme, MAGIC) & 0xff) == 0x52) {
			/* new CTU design with addon card */
			LVme_nop(lvme);
			LVme_setL(lvme, TRIG1, 0x80);
			LVme_setL(lvme, TRIG1, 0x80 | 0x0e);	/* new end run code */
			LVme_setL(lvme, TRIG1, 0x80 | 0x0e | 0x40);	/* trigger! */
			LVme_setL(lvme, TRIG1, 0x00);
			LVme_nop(lvme);
		} else {
			syslog(LOG_WARNING, "%s: Unknown CTU design", name);
		}
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}

	while ((0 != Dtu_running(name, p)) && (0 != counter)) {
		nanosleep(&t2, NULL);
		counter--;
	}

	return retVal;
}

/*
 * RICH
 */
static int readStatusRich(LVme * lvme, unsigned int offset)
{
	unsigned int value;

	LVme_nop(lvme);
	LVme_setL(lvme, RICH_OFFSET, offset);
	LVme_nop(lvme);
	value = (LVme_getL(lvme, RICH_DATA) & 0xff);
	LVme_nop(lvme);
	return value;
}

static int manslayerRich(const Param * p, const char *name, LVme * lvme)
{
	/* MANSLAYER register is set with units of 400ns deadtime.
	 * Additional deadtime generation is needed for defect analog part
	 * of RICH FE. Set this register to zero to avoid any additional
	 * deadtime. Hopefully this problem will be fixed anywhere in the
	 * future. 
	 */
	unsigned long int manslayer = 0;
	int rows;

	if ((Param_getInt(p, name, "manslayer", &rows, &manslayer) == 0)
		&& (rows == 1)) {
		LVme_nop(lvme);
		LVme_setL(lvme, RICH_DEAD, manslayer);
		LVme_nop(lvme);
	}
	return 0;
}

static int ipuOnRich(const Param * p, const char *name, LVme * lvme)
{
	unsigned long int ipu_on = 0;
	int rows;

	Param_getInt(p, name, "ipu_on", &rows, &ipu_on);
	LVme_nop(lvme);
	if (ipu_on) {
		LVme_setBitL(lvme, RICH_OFF, BIT_IPUON);
	} else {
		LVme_clrBitL(lvme, RICH_OFF, BIT_IPUON);
	}
	LVme_nop(lvme);
	return 0;
}

static int decisionRich(const Param * p, const char *name, LVme * lvme)
{
	/* CtuDtu register is used for DECISION source selection.
	 * D0 : NORM triggers
	 * Dx : SPECx triggers (with x element from [1..5])
	 *
	 * Decision register is used for setting DECISION.
	 * D0 : DECISION NORM trigger
	 * Dx : DECISION SPECx triggers (with x element from [1..5])
	 */
	int i;
	unsigned long int triggerdec[6];
	unsigned long int ctudtu = 0;
	unsigned long int decision = 0;
	int rows;

	if ((Param_getIntArray(p, name, "triggerdec", 6, &rows, triggerdec) == 0)) {
		for (i = 0; i < rows; i++) {
			if (triggerdec[i] == 0) {
				/* follow ctu decision */
			} else if (triggerdec[i] == 1) {
				/* always decide positive */
				ctudtu |= (0x1 << i);
			} else if (triggerdec[i] == 2) {
				/* always decide negative */
				ctudtu |= (0x1 << i);
				decision |= (0x1 << i);
			}
		}
	}
	LVme_nop(lvme);
	LVme_setL(lvme, RICH_CD, ctudtu);
	LVme_setL(lvme, RICH_DEC, decision);
	LVme_nop(lvme);
	return 0;
}

static int invDecRich(const Param * p, const char *name, LVme * lvme)
{
	unsigned long int invdec = 0;
	int rows;

	if ((Param_getInt(p, name, "invdec", &rows, &invdec) == 0)
		&& (rows == 1)) {
		LVme_nop(lvme);
		if (invdec == 1) {
			LVme_setBitL(lvme, RICH_OFF, BIT_INVDEC);
		} else {
			LVme_clrBitL(lvme, RICH_OFF, BIT_INVDEC);
		}
		LVme_nop(lvme);
	}
	return 0;
}

static int internalResetRich(LVme * lvme)
{
	LVme_nop(lvme);
	LVme_setBitL(lvme, RICH_OFF, BIT_RESET);
	LVme_clrBitL(lvme, RICH_OFF, BIT_RESET);
	LVme_nop(lvme);
	return 0;
}

static int resetRich(const char *name, const Param * p)
{
	int retVal = 0;
	int rows = 0;
	unsigned long int dead1 = 0x04;
	unsigned long int dead2 = 0x04;
	LVme lvmeS, *lvme = &lvmeS;

	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00);
		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x02);

		LVme_setL(lvme, FIFO1, 0x0);
		LVme_setL(lvme, FIFO2, 0x0);
		LVme_nop(lvme);

		/* Set deadtime register */
		manslayerRich(p, name, lvme);

		/* Set the DECISION MODE _and_ DECISION itself */
		decisionRich(p, name, lvme);

		/* Set the INVDEC bit */
		invDecRich(p, name, lvme);

		/* Set the IPUON bit */
		ipuOnRich(p, name, lvme);

		/* Clear the STOP bit */
		LVme_nop(lvme);
		LVme_clrBitL(lvme, RICH_OFF, BIT_STOP);
		LVme_nop(lvme);
		/* Generate internal /RESET */
		internalResetRich(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}
	return retVal;
}

/*
 * MDC
 */
static int resetMdc(const char *name, const Param * p)
{
	int retVal = 0;
	int rows = 0;
	unsigned long int dead1 = 0x04;
	unsigned long int dead2 = 0x04;
	unsigned long int samdead = 0x10;

	LVme lvmeS, *lvme = &lvmeS;

	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);
	Param_getInt(p, name, "samdead", &rows, &samdead);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00);
		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, SAMDEAD, samdead);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x02);

		LVme_setL(lvme, FIFO1, 0x0);
		LVme_setL(lvme, FIFO2, 0x0);

		LVme_nop(lvme);
		if ((LVme_getL(lvme, FIFO1) & 1) != 1) {
			syslog(LOG_ERR, "%s: FIFO1 not propperly cleared", name);
			retVal = -1;
		}
		if ((LVme_getL(lvme, FIFO2) & 1) != 1) {
			syslog(LOG_ERR, "%s: FIFO1 not propperly cleared", name);
			retVal = -1;
		}

		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}
	return retVal;
}

/*
 * SHW
 */
static int initRb(const char *name, const Param * p)
{
	int retVal = 0;
	struct timespec t = { 0, 500000000 };
	unsigned long int rb_delay = 0;
	unsigned long int rb_ack = 0;
	int rows = 0;
	LVme lvmeS, *lvme = &lvmeS;

	if (0 == (retVal = mapVme(lvme, name, p))) {
		Param_getInt(p, name, "rb_delay", &rows, &rb_delay);
		Param_getInt(p, name, "rb_ack", &rows, &rb_ack);

		LVme_nop(lvme);
		LVme_setL(lvme, DTU_DELAY, 0xc0);
		syslog(LOG_DEBUG, "%s: DTU_DELAY", name);
		nanosleep(&t, NULL);
		LVme_setL(lvme, DTU_COMMAND, RB_M_RES);
		syslog(LOG_DEBUG, "%s: DTU_COMMAND RB_M_RES", name);
		nanosleep(&t, NULL);
		LVme_setL(lvme, DTU_COMMAND, RB_SEL_F1234);
		syslog(LOG_DEBUG, "%s: DTU_COMMAND RB_SEL_F1234", name);
		nanosleep(&t, NULL);
		LVme_setL(lvme, DTU_COMMAND, RB_RES_F);
		syslog(LOG_DEBUG, "%s: DTU_COMMAND RB_RES_F", name);
		nanosleep(&t, NULL);
		LVme_setL(lvme, DTU_COMMAND, RB_R_MODE);
		syslog(LOG_DEBUG, "%s: DTU_COMMAND RB_R_MODE", name);
		nanosleep(&t, NULL);
		LVme_setL(lvme, DTU_DELAY, 0x00);
		syslog(LOG_DEBUG, "%s: DTU_DELAY", name);
		nanosleep(&t, NULL);
		/* delayRb */
		LVme_setL(lvme, RB_DELAY, rb_delay);
		syslog(LOG_DEBUG, "%s: RB_DELAY 1", name);
		nanosleep(&t, NULL);
		syslog(LOG_DEBUG, "%s: RB_DELAY 2", name);
		/* ackRb */
		LVme_setL(lvme, RB_ACK, (rb_ack << 2));
		syslog(LOG_DEBUG, "%s: RB_ACK", name);
		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: initRb failed", name);
	}
	return retVal;
}

static int resetShw(const char *name, const Param * p)
{
	struct sigaction actS, *act = &actS;
	struct sigaction oactS, *oact = &oactS;
	int retVal = 0;
	int rows = 0;
	unsigned long int dead1 = 0x0a;
	unsigned long int dead2 = 0x08;
	LVme lvmeS, *lvme = &lvmeS;

	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00);
		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, DTU_IPC_BRCAST, 0x80);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x02);
		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}

	act->sa_handler = SIG_IGN;
	sigemptyset(&act->sa_mask);
	act->sa_flags = 0;

	/* initRb may cause a bus error doing no harm, so try to ignore signal */
	if (0 != sigaction(SIGBUS, act, oact)) {
		/* maybe it works without SIGBUS, so it's only a warning */
		syslog(LOG_WARNING, "%s: Cannot not change signal handler for SIGBUS", name);
	}
	retVal |= initRb(name, p);
	/* try to reinstall old handler */
	if (0 != sigaction(SIGBUS, oact, NULL)) {
		syslog(LOG_ERR, "%s: Cannot not change back signal handler for SIGBUS", name);
	}

	return retVal;
}

/*
 * TOF
 */
static int resetTof(const char *name, const Param * p)
{
	int retVal = 0;
	int rows = 0;
	unsigned long int dead1 = 0x10;
	unsigned long int dead2 = 0x10;
	LVme lvmeS, *lvme = &lvmeS;

	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00);
		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x02);
		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}
	return retVal;
}

/*
 * SEB
 */
static int resetSeb(const char *name, const Param * p)
{
	int retVal = 0;
	int rows = 0;
	unsigned long int dead1 = 0x0a;
	unsigned long int dead2 = 0x08;
	LVme lvmeS, *lvme = &lvmeS;

	Param_getInt(p, name, "dead1", &rows, &dead1);
	Param_getInt(p, name, "dead2", &rows, &dead2);

	if (0 == (retVal = mapVme(lvme, name, p))) {
		LVme_nop(lvme);
		LVme_setL(lvme, STATUS2, 0x01);
		LVme_setL(lvme, STATUS1, 0x01);
		LVme_setL(lvme, STATUS2, 0x00);
		LVme_setL(lvme, STATUS1, 0x00);
		LVme_setL(lvme, DEAD1, dead1);
		LVme_setL(lvme, DEAD2, dead2);
		LVme_setL(lvme, STATUS2, 0x02);
		LVme_setL(lvme, STATUS1, 0x02);
		LVme_nop(lvme);
		unmapVme(lvme);
	} else {
		syslog(LOG_ERR, "%s: mapVme", name);
	}
	return retVal;
}

static int pollSeb(LVme * lvme)
{
	unsigned long int code1;
	unsigned long int code2;
	unsigned long int tag1;
	unsigned long int tag2;
	unsigned long int cnt_tag1 = 0;
	unsigned long int cnt_tag2 = 0;

	while (1) {
		LVme_nop(lvme);
		if ((LVme_getL(lvme, FIFO1) & 0x01) != 0x01) {
			code1 = LVme_getL(lvme, CODE1) & 0x0f;
			tag1 = LVme_getL(lvme, TAG1) & 0xff;
			syslog(LOG_DEBUG, "dtu: CODE1[0x%02lx] = 0x%02lx  TAG1[0x%02lx] = 0x%02lx", cnt_tag1, code1, cnt_tag1, tag1);
			LVme_nop(lvme);
			LVme_setL(lvme, FIFO1, 0x00);
			LVme_nop(lvme);
			++cnt_tag1;
		}
		if ((LVme_getL(lvme, FIFO2) & 0x01) != 0x01) {
			code2 = LVme_getL(lvme, CODE2) & 0x0f;
			tag2 = LVme_getL(lvme, TAG2) & 0xff;
			syslog(LOG_DEBUG, "dtu: CODE2[0x%02lx] = 0x%02lx  TAG2[0x%02lx] = 0x%02lx", cnt_tag2, code2, cnt_tag2, tag2);
			LVme_nop(lvme);
			LVme_setL(lvme, FIFO2, 0x00);
			LVme_nop(lvme);
			++cnt_tag2;
		}
		LVme_nop(lvme);
	}
	return 0;
}

int Dtu_reset(const char *name, const Param * p)
{
	int retVal = 0;
	char dtuType[PARAM_MAX_VALUE_LEN];
	int rows;

	if (Param_getString(p, name, "dtu_type", &rows, dtuType)
		|| (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter dtu_type not found", name);
		retVal = -1;
	} else {
		if (strcmp(dtuType, "show") == 0) {
			retVal = resetShw(name, p);
		} else if (strcmp(dtuType, "mdc") == 0) {
			retVal = resetMdc(name, p);
		} else if (strcmp(dtuType, "rich") == 0) {
			retVal = resetRich(name, p);
		} else if (strcmp(dtuType, "seb") == 0) {
			retVal = resetSeb(name, p);
		} else if (strcmp(dtuType, "tof") == 0) {
			retVal = resetTof(name, p);
		} else if (strcmp(dtuType, "ctu") == 0) {
			retVal = resetCtu(name, p);
		}
	}
	return retVal;
}

int Dtu_init(const char *name, const Param * p)
{
	FILE *fs;
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;
	char filename[PARAM_MAX_VALUE_LEN];
	char dtuType[PARAM_MAX_VALUE_LEN];
	unsigned long int cardbase;
	int rows;

	if (Param_getString(p, name, "dtu_type", &rows, dtuType)
		|| (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter dtu_type not found", name);
		retVal = -1;
	} else if (Param_getInt(p, name, "cardbase", &rows, &cardbase)
			   || (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter cardbase not found", name);
		retVal = -1;
	} else if (Param_getFilename(p, name, "filename", &rows, filename)
			   || (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter filename not found", name);
		retVal = -1;
	} else {
		if (NULL == (lvme = newLVme(cardbase, 0x10000, 0x09))) {
			if (NULL != (fs = fopen(filename, "r"))) {
			    retVal = ConfigXilinx(name, lvme, 0xC004, 0, 2, 3, 1, 4, fs);
   				fclose(fs);
			} else {
				retVal = -1;
			}
			delLVme(lvme);
		} else {
			retVal = -1;
		}
	}
	retVal |= Dtu_reset(name, p);
	return retVal;
}

int Dtu_loadThresholds(const char *n, const Param * p)
{
	return 0;
}


int Dtu_start(const char *name, const Param * p)
{
	int retVal = 0;
	char dtuType[PARAM_MAX_VALUE_LEN];
	int rows;

	if (Param_getString(p, name, "dtu_type", &rows, dtuType)
		|| (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter dtu_type not found", name);
		retVal = -1;
	} else {
		if (strcmp(dtuType, "ctu") == 0) {
			retVal = startCtu(name, p);
		}
	}

	return retVal;
}

int Dtu_stop(const char *name, const Param * p)
{
	int retVal = 0;
	char dtuType[PARAM_MAX_VALUE_LEN];
	int rows;

	if (Param_getString(p, name, "dtu_type", &rows, dtuType)
		|| (rows == 0)) {
		syslog(LOG_ERR, "%s: Parameter dtu_type not found", name);
		retVal = -1;
	} else {
		if (strcmp(dtuType, "ctu") == 0) {
			retVal = stopCtu(name, p);
		}
	}

	return retVal;
}

int Dtu_initialized(const char *name, const Param * p)
{
	return -1;
}

int Dtu_running(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;
	int rows = 0;
	char dtuType[PARAM_MAX_VALUE_LEN];

	if (Param_getString(p, name, "dtu_type", &rows, dtuType)
		|| (rows == 0)) {
		syslog(LOG_DEBUG, "%s: Parameter dtu_type not found", name);
		retVal = -1;
	} else {
		if (strcmp(dtuType, "ctu") || mapVme(lvme, name, p)) {
			retVal = -1;
		} else {
			LVme_nop(lvme);
			retVal = !LVme_tstBitL(lvme, STATUS1, 3);
			LVme_nop(lvme);
			unmapVme(lvme);
		}
	}

	return retVal;
}

int Dtu_lvl1Busy(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;

	if (0 == mapVme(lvme, name, p)) {
		LVme_nop(lvme);
		retVal = (LVme_getL(lvme, STATUS1) & 0x40) >> 6;
		LVme_nop(lvme);
		unmapVme(lvme);
	}

	return retVal;
}

int Dtu_lvl2Busy(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;

	if (0 == mapVme(lvme, name, p)) {
		LVme_nop(lvme);
		retVal = (LVme_getL(lvme, STATUS2) & 0x40) >> 6;
		LVme_nop(lvme);
		unmapVme(lvme);
	}

	return retVal;
}

int Dtu_error(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	int retVal = 0;

	if (0 == mapVme(lvme, name, p)) {
		LVme_nop(lvme);
		retVal = (LVme_getL(lvme, STATUS1) & 0x80) >> 7;
		retVal |= (LVme_getL(lvme, STATUS2) & 0x80) >> 7;
		LVme_nop(lvme);
		unmapVme(lvme);
	}

	return retVal;
}

int Dtu_statusRegister(const char *name, const Param * p, const char *r)
{
	enum { nRegs = 8 };

	if (strcmp(r, "regs") == 0) {
		int i;
		LVme lvmeS, *lvme = &lvmeS;
		static unsigned long regs[nRegs];

			mapVme(lvme, name, p);

			i = 0;
			regs[i++] = LVme_getL(lvme, STATUS1) & 0xff;
			regs[i++] = LVme_getL(lvme, DEAD1) & 0xff;
			regs[i++] = LVme_getL(lvme, CODE1) & 0xff;
			regs[i++] = LVme_getL(lvme, TAG1) & 0xff;
			regs[i++] = LVme_getL(lvme, STATUS2) & 0xff;
			regs[i++] = LVme_getL(lvme, DEAD2) & 0xff;
			regs[i++] = LVme_getL(lvme, CODE2) & 0xff;
			regs[i++] = LVme_getL(lvme, TAG2) & 0xff;
			assert(i == nRegs);

			unmapVme(lvme);

		return (unsigned long) regs;
	} else if (strcmp(r, "nreg") == 0) {
		return nRegs;
	} else {
	unsigned long int retVal = 0;
	LVme lvmeS, *lvme = &lvmeS;
	char dtuType[PARAM_MAX_VALUE_LEN];
	int rows;

	if ((0 != Param_getString(p, name, "dtu_type", &rows, dtuType)) || (rows == 0)) {
		syslog(LOG_WARNING, "%s: Parameter dtu_type not found", name);
		strcpy(dtuType, "undefined");
	}

	if (0 != mapVme(lvme, name, p)) {
		syslog(LOG_ERR, "%s: mapVme failed", name);
	} else {
		LVme_nop(lvme);
		if (0 == strcmp("dead1", r)) {	/* General */
			retVal = LVme_getL(lvme, DEAD1) & 0xff;
		} else if (0 == strcmp("dead2", r)) {
			retVal = LVme_getL(lvme, DEAD2) & 0xff;
		} else if ((0 != strcmp("ctu", dtuType)) && (0 == strcmp("fifo_lvl1", r))) {
			retVal = LVme_getL(lvme, FIFO1) & 0xf3;
		} else if ((0 != strcmp("ctu", dtuType)) && (0 == strcmp("fifo_lvl2", r))) {
			retVal = LVme_getL(lvme, FIFO2) & 0xf3;
		} else if (0 == strcmp("count1", r)) {
			retVal = LVme_getL(lvme, COUNT1) & 0xff;
		} else if (0 == strcmp("count2", r)) {
			retVal = LVme_getL(lvme, COUNT2) & 0xff;
		} else if (0 == strcmp("ctu", dtuType)) {	/* CTU specific */
			if (0 == strcmp("fifo_ctu", r)) {
				retVal = LVme_getL(lvme, FIFO) & 0xf3;
			} else if (0 == strcmp("delay2", r)) {
				retVal = LVme_getL(lvme, DELAY2) & 0xcf;
			} else if (0 == strcmp("rate2", r)) {
				retVal = LVme_getL(lvme, RATE2) & 0xff;
			}
		} else if (0 == strcmp("show", dtuType)) {	/* SHOW specific */
			if (0 == strcmp("show_ack", r)) {
				retVal = LVme_getL(lvme, RB_ACK) & 0x0c;
			} else if (0 == strcmp("rb_delay", r)) {
				retVal = LVme_getL(lvme, RB_DELAY) & 0x0f;
			}
		} else if (0 == strcmp("mdc", dtuType)) {	/* MDC specific */
			if (0 == strcmp("samdead", r)) {
				retVal = LVme_getL(lvme, SAMDEAD) & 0xff;
			}
		} else if (0 == strcmp("rich", dtuType)) {	/* RICH specific */
			if (0 == strcmp("rich_off", r)) {
				retVal = LVme_getL(lvme, RICH_OFF) & 0xff;
			} else if (0 == strcmp("rich_dead", r)) {
				retVal = LVme_getL(lvme, RICH_DEAD) & 0xff;
			} else if (0 == strcmp("rich_mm", r)) {
				retVal = LVme_getL(lvme, RICH_MM) & 0xff;
			} else if (0 == strcmp("rich_cd", r)) {
				retVal = LVme_getL(lvme, RICH_CD) & 0xff;
			} else if (0 == strcmp("rich_dec", r)) {
				retVal = LVme_getL(lvme, RICH_DEC) & 0xff;
			} else if (0 == strcmp("rich_mode", r)) {
				retVal = LVme_getL(lvme, RICH_MODE) & 0xff;
			} else if (0 == strcmp("rich_ver", r)) {
				retVal = readStatusRich(lvme, OFFSET_VER);
			} else if (0 == strcmp("rich_qup", r)) {
				retVal = readStatusRich(lvme, OFFSET_QUP);
			} else if (0 == strcmp("rich_qua", r)) {
				retVal = readStatusRich(lvme, OFFSET_QUA);
			} else if (0 == strcmp("rich_be", r)) {
				retVal = readStatusRich(lvme, OFFSET_BE);
			} else if (0 == strcmp("rich_tcs", r)) {
				retVal = readStatusRich(lvme, OFFSET_TCS);
			} else if (0 == strcmp("rich_msn", r)) {
				retVal = readStatusRich(lvme, OFFSET_MSM);
			}
		}
		LVme_nop(lvme);
		unmapVme(lvme);
	}

	return retVal;
	}
}

#ifndef EPICS_RUNCTRL
int Dtu_status(const char *name, const Param * p)
{
	LVme lvmeS, *lvme = &lvmeS;
	unsigned long int status_lvl1;
	unsigned long int status_lvl2;
	unsigned long int fifo_lvl1;
	unsigned long int fifo_lvl2;
	unsigned long int deadtime_lvl1;
	unsigned long int deadtime_lvl2;
	unsigned long int count1;
	unsigned long int count2;
	unsigned long int fifo_ctu;
	unsigned long int delay2;
	unsigned long int rate2;
	unsigned long int show_ack;
	unsigned long int rb_delay;

	/* RICH specific registers */
	unsigned long int rich_off;
	unsigned long int rich_dead;
	unsigned long int rich_mm;
	unsigned long int rich_cd;
	unsigned long int rich_dec;
	unsigned long int rich_mode;
	unsigned long int rich_ver;
	unsigned long int rich_qup;
	unsigned long int rich_qua;
	unsigned long int rich_be;
	unsigned long int rich_tcs;
	unsigned long int rich_msm;
	unsigned long int tof_busy;
	unsigned long trash;

	/* MDC specific registers */
	unsigned long int samdead;
	char dtuType[PARAM_MAX_VALUE_LEN];
	int rows;

	if ((0 != Param_getString(p, name, "dtu_type", &rows, dtuType))
		|| (rows == 0)) {
		syslog(LOG_WARNING, "%s: Parameter dtu_type not found", name);
	}

	mapVme(lvme, name, p);

	status_lvl1 = LVme_getL(lvme, STATUS1) & 0xff;
	status_lvl2 = LVme_getL(lvme, STATUS2) & 0xff;
	deadtime_lvl1 = LVme_getL(lvme, DEAD1) & 0xff;
	deadtime_lvl2 = LVme_getL(lvme, DEAD2) & 0xff;
	fifo_lvl1 = LVme_getL(lvme, FIFO1) & 0xf3;
	fifo_lvl2 = LVme_getL(lvme, FIFO2) & 0xf3;
	count1 = LVme_getL(lvme, COUNT1) & 0xff;
	count2 = LVme_getL(lvme, COUNT2) & 0xff;
	fifo_ctu = LVme_getL(lvme, FIFO) & 0xf3;
	delay2 = LVme_getL(lvme, DELAY2) & 0xcf;
	rate2 = LVme_getL(lvme, RATE2) & 0xff;
	show_ack = LVme_getL(lvme, RB_ACK) & 0x0c;
	rb_delay = LVme_getL(lvme, RB_DELAY) & 0x0f;

	/* Read MDC specific DTU registers */
	samdead = LVme_getL(lvme, SAMDEAD) & 0xff;

	/* Read TOF specific DTU registers */
	if (strcmp(dtuType, "tof") == 0) {
		tof_busy = LVme_getL(lvme, TOF_BUSY) & 0x3f;
	}

	/* Read RICH specific DTU registers */
	if (strcmp(dtuType, "rich") == 0) {
		/************************************
		 * Get all registers out of the DTU *
		 ************************************/
		rich_off = LVme_getL(lvme, RICH_OFF) & 0xff;
		rich_dead = LVme_getL(lvme, RICH_DEAD) & 0xff;
		rich_mm = LVme_getL(lvme, RICH_MM) & 0xff;
		rich_cd = LVme_getL(lvme, RICH_CD) & 0xff;
		rich_dec = LVme_getL(lvme, RICH_DEC) & 0xff;
		rich_mode = LVme_getL(lvme, RICH_MODE) & 0xff;
		rich_ver = readStatusRich(lvme, OFFSET_VER);
		rich_qup = readStatusRich(lvme, OFFSET_QUP);
		rich_qua = readStatusRich(lvme, OFFSET_QUA);
		rich_be = readStatusRich(lvme, OFFSET_BE);
		rich_tcs = readStatusRich(lvme, OFFSET_TCS);
		rich_msm = readStatusRich(lvme, OFFSET_MSM);
	}

	if (strcmp(dtuType, "ctu") == 0) {
		fprintf(stderr,
				" 0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx\n",
				status_lvl1, status_lvl2, deadtime_lvl1, deadtime_lvl2, delay2, rate2, fifo_ctu, count1, count2);
		fprintf(stderr, "STAT1 STAT2 DEAD1 DEAD2 DELY2 RATE2 FIFO2 COUN1 COUN2\n");
	} else {
		fprintf(stderr,
				" 0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx  0x%02lx",
				status_lvl1, status_lvl2, deadtime_lvl1, deadtime_lvl2, fifo_lvl1, fifo_lvl2, count1, count2);
		if (strcmp(dtuType, "show") == 0) {
			fprintf(stderr, "  0x%02lx  0x%02lx", show_ack, rb_delay);
		}
		fprintf(stderr, "\n");
		fprintf(stderr, "STAT1 STAT2 DEAD1 DEAD2 FIFO1 FIFO2 COUNT1 COUNT2");
		if (strcmp(dtuType, "show") == 0) {
			fprintf(stderr, " RBACK RBDLY");
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, ":-) don't like hex numbers ?\n");

	if (status_lvl1 & 0xc0) {
		if (status_lvl1 & 0x40) {
			fprintf(stderr, "LVL1 is BUSY  ");
		}
		if (status_lvl1 & 0x80) {
			fprintf(stderr, "LVL1 has ERROR");
		}
	} else {
		fprintf(stderr, "LVL1 is idle  ");
	}
	fprintf(stderr, "                ");
	if (status_lvl2 & 0xc0) {
		if (status_lvl2 & 0x40) {
			fprintf(stderr, "LVL2 is BUSY  \n");
		}
		if (status_lvl2 & 0x80) {
			fprintf(stderr, "LVL2 has ERROR\n");
		}
	} else {
		fprintf(stderr, "LVL2 is idle  \n");
	}

	if (status_lvl1 & 0x02) {
		fprintf(stderr, "LVL1 is not inhibited         ");
	} else {
		fprintf(stderr, "LVL1 is inhibited             ");
	}
	if (status_lvl2 & 0x02) {
		fprintf(stderr, "LVL2 is not inhibited         \n");
	} else {
		fprintf(stderr, "LVL2 is inhibited             \n");
	}

	fprintf(stderr, "LVL1 deadtime is %4.1f us      ", (float) deadtime_lvl1 * 1.6);
	fprintf(stderr, "LVL2 deadtime is %4.1f us\n", (float) deadtime_lvl2 * 1.6);
	if (strcmp(dtuType, "ctu") != 0) {
		if (fifo_lvl1 & 0x07) {
			if (fifo_lvl1 & 0x01) {
				fprintf(stderr, "LVL1 Fifo is empty     ");
			}
			if (fifo_lvl1 & 0x06) {
				fprintf(stderr, "LVL1 Fifo is full      ");
			}
		} else {
			fprintf(stderr, "LVL1 Fifo contains data");
		}
		fprintf(stderr, "       ");
		if (fifo_lvl2 & 0x07) {
			if (fifo_lvl2 & 0x01) {
				fprintf(stderr, "LVL2 Fifo is empty\n");
			}
			if (fifo_lvl2 & 0x06) {
				fprintf(stderr, "LVL2 Fifo is full\n");
			}
		} else {
			fprintf(stderr, "LVL2 Fifo contains data\n");
		}
		fprintf(stderr, "LVL1 Fifo position is %2ld      ", (fifo_lvl1 & 0xf0) >> 4);
		fprintf(stderr, "LVL2 Fifo position is %2ld      \n", (fifo_lvl2 & 0xf0) >> 4);

		fprintf(stderr, "LVL1 Counter is 0x%02lx\n", count1);
		fprintf(stderr, "LVL2 Counter is 0x%02lx\n", count2);
		/* RICH specific stuff */
		if (strcmp(dtuType, "mdc") == 0) {
			fprintf(stderr, "SAM Deadtime = %f usec\n", (float) samdead * 1.6);
		}
		fprintf(stderr, "\n");

		if (strcmp(dtuType, "tof") == 0) {
			if (LVme_getL(lvme, STATUS1) & 0x40) {
				/* busy is enabled */
				fprintf(stderr, "TOF Busy Information\n");
				fprintf(stderr, "====================\n");
				fprintf(stderr, "register: 0x%x\n", tof_busy);
				if (tof_busy & 0x2)
					fprintf(stderr, "TDCs: static busy\n");
				else if (tof_busy & 0x20)
					fprintf(stderr, "DTU internal cycle not finished\n");
				else if (tof_busy & 0x10)
					fprintf(stderr, "TIP: static busy\n");
				else if (tof_busy & 0x8)
					fprintf(stderr, "TIP: waiting for handshake\n");
				else if (tof_busy & 0x4)
					fprintf(stderr, "TIP: linkport action\n");
				else if (tof_busy == 0)
					fprintf(stderr, "DTU internal cycle not started (no TDC convertion?)\n");
				else
					fprintf(stderr, "Unknown BUSY\n");
			}

		}

		if (strcmp(dtuType, "rich") == 0) {
			fprintf(stderr, "General Setup Information\n");
			fprintf(stderr, "=========================\n");
			fprintf(stderr, " DTU design V%ld R%ld\n", ((rich_ver & 0xf0) >> 4), (rich_ver & 0x0f));
			/* STOP bit status */
			fprintf(stderr, " DTU is ");
			if ((rich_off & 0x02) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "stopped.\n");
			/* RESET bit status */
			fprintf(stderr, " DTU is ");
			if ((rich_off & 0x01) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "in /Reset mode.\n");
			/* IPUON bit status */
			fprintf(stderr, " DTU arbitration ");
			if ((rich_off & 0x80) == 0) {
				fprintf(stderr, "ex");
			} else {
				fprintf(stderr, "in");
			}
			fprintf(stderr, "cludes IPU signals.\n");
			/* BEGRUNON bit status */
			fprintf(stderr, " DTU generates ");
			if ((rich_off & 0x40) == 0) {
				fprintf(stderr, "no ");
			}
			fprintf(stderr, "/BEGRUN signal.\n");
			/* INVDEC bit status */
			fprintf(stderr, " Decision is ");
			if ((rich_off & 0x20) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "inverted by DTU.\n");
			/* TC setup */
			fprintf(stderr, "LVL1 TC setup\n");
			fprintf(stderr, "=============\n");
			/* TC7 mode */
			trash = (rich_mode & 0x03);
			fprintf(stderr, " SPEC2 (TC7): %ld - ", trash);
			if (trash == 0) {
				fprintf(stderr, "normal event\n");
			} else if (trash == 1) {
				fprintf(stderr, "empty event\n");
			} else if (trash == 2) {
				fprintf(stderr, "full event\n");
			} else {
				fprintf(stderr, "test event (DO NOT USE !)\n");
			}
			/* TC9 mode */
			trash = (rich_mode & 0x0c) >> 2;
			fprintf(stderr, " SPEC4 (TC9): %ld - ", trash);
			if (trash == 0) {
				fprintf(stderr, "normal event\n");
			} else if (trash == 1) {
				fprintf(stderr, "empty event\n");
			} else if (trash == 2) {
				fprintf(stderr, "full event\n");
			} else {
				fprintf(stderr, "test event (DO NOT USE !)\n");
			}
			/* TCA mode */
			trash = (rich_mode & 0x30) >> 4;
			fprintf(stderr, " SPEC5 (TCA): %ld - ", trash);
			if (trash == 0) {
				fprintf(stderr, "normal event\n");
			} else if (trash == 1) {
				fprintf(stderr, "empty event\n");
			} else if (trash == 2) {
				fprintf(stderr, "full event\n");
			} else {
				fprintf(stderr, "test event (DO NOT USE !)\n");
			}
			/* Decision */
			fprintf(stderr, "Decision Setup\n");
			fprintf(stderr, "==============\n");
			/* CD and DEC register */
			fprintf(stderr, " LVL2 NORMAL: ");
			if (((rich_cd & 0x01) >> 0) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x01) >> 0) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, " LVL2 SPEC1 : ");
			if (((rich_cd & 0x02) >> 1) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x02) >> 1) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, " LVL2 SPEC2 : ");
			if (((rich_cd & 0x04) >> 2) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x04) >> 2) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, " LVL2 SPEC3 : ");
			if (((rich_cd & 0x08) >> 3) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x08) >> 3) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, " LVL2 SPEC4 : ");
			if (((rich_cd & 0x10) >> 4) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x10) >> 4) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, " LVL2 SPEC5 : ");
			if (((rich_cd & 0x20) >> 5) == 0) {
				fprintf(stderr, "CTU\n");
			} else {
				fprintf(stderr, "DTU ");
				if (((rich_dec & 0x20) >> 5) == 0) {
					fprintf(stderr, "postive\n");
				} else {
					fprintf(stderr, "negative\n");
				}
			}
			fprintf(stderr, "Queue Status Information\n");
			fprintf(stderr, "========================\n");
			/* LVL1 QUEUE status report */
			fprintf(stderr, " LVL1: %ld events queued", (rich_qup & 0x0f));
			if (((rich_qup & 0x10) >> 4) == 1) {
				fprintf(stderr, ", queue empty");
			}
			if (((rich_qup & 0x20) >> 5) == 1) {
				fprintf(stderr, ", last position");
			}
			if (((rich_qup & 0x40) >> 6) == 1) {
				fprintf(stderr, " queue full");
			}
			if (((rich_qup & 0x80) >> 7) == 1) {
				fprintf(stderr, " and PRDOUTs are pending");
			}
			fprintf(stderr, ".\n");
			/* LVL1 last TC Status */
			fprintf(stderr, "       Last TC on LVL1 bus: 0x%01x\n", (rich_tcs & 0x0f));
			/* RICH LVL1 Opcode Status */
			fprintf(stderr, "       Illegal Opcode has ");
			if (((rich_be & 0x02) >> 1) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "been detected.\n");
			/* LVL2 QUEUE status report */
			fprintf(stderr, " LVL2: %ld events queued", (rich_qua & 0x0f));
			if (((rich_qua & 0x10) >> 4) == 1) {
				fprintf(stderr, ", queue empty");
			}
			if (((rich_qua & 0x20) >> 5) == 1) {
				fprintf(stderr, ", queue last");
			}
			if (((rich_qua & 0x40) >> 6) == 1) {
				fprintf(stderr, ", queue full");
			}
			if (((rich_qua & 0x80) >> 7) == 1) {
				fprintf(stderr, " and ARDOUTs/ADELETEs are pending");
			}
			fprintf(stderr, "\n");
			/* LVL2 last RC Status */
			fprintf(stderr, "       Last RC on LVL2 bus: 0x%01x\n", ((rich_tcs & 0xf0) >> 4));
			/* RICH LVL2 Opcode Status */
			fprintf(stderr, "       Illegal Opcode has ");
			if (((rich_be & 0x04) >> 2) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "been detected.\n");
			/* Readout Status */
			fprintf(stderr, "Readout Status Information\n");
			fprintf(stderr, "==========================\n");
			/* Busy Information */
			/* Readout Controller */
			fprintf(stderr, " RCs : ");
			if (((rich_be & 0x80) >> 7) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "busy, ");
			if (((rich_be & 0x10) >> 4) == 0) {
				fprintf(stderr, "no ");
			}
			fprintf(stderr, "error.\n");
			/* Frontends */
			fprintf(stderr, " FEs : ");
			if (((rich_be & 0x40) >> 6) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "busy.\n");
			/* Image Processing Unit */
			fprintf(stderr, " IPUs: ");
			if (((rich_be & 0x20) >> 5) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "busy, ");
			if (((rich_be & 0x08) >> 3) == 0) {
				fprintf(stderr, "no ");
			}
			fprintf(stderr, "error.\n");
			/* GO signal */
			fprintf(stderr, " Readout cycles are ");
			if ((rich_be & 0x01) == 0) {
				fprintf(stderr, "not ");
			}
			fprintf(stderr, "possible (GO).\n");
			/* Lemo connectors */
			fprintf(stderr, "Lemo Out Connectors\n");
			fprintf(stderr, "===================\n");
			fprintf(stderr, " Connectors are ");
			if (((rich_mm & 0x08) >> 3) == 0) {
				fprintf(stderr, "enabled.\n");
				fprintf(stderr, " Upper LEMO signal: ");
				switch (((rich_mm & 0x30) >> 4)) {
				case 0:
					fprintf(stderr, "TBQ - inverted and sampled /TBSY\n");
					break;
				case 1:
					fprintf(stderr, "/TAGCLK - tag transmission clock\n");
					break;
				case 2:
					fprintf(stderr, "10MHZ - DTU master clock\n");
					break;
				case 3:
					fprintf(stderr, "/PRDOUT - start signal for pattern readout\n");
					break;
				}
				fprintf(stderr, " Lower LEMO signal: ");
				switch (((rich_mm & 0xc0) >> 6)) {
				case 0:
					fprintf(stderr, "RCBQ - inverted and sampled /RCBSY\n");
					break;
				case 1:
					fprintf(stderr, "TAGDTA - tag transmission data\n");
					break;
				case 2:
					fprintf(stderr, "IBSY - inverted and sampled /IPUBSY\n");
					break;
				case 3:
					fprintf(stderr, "/ARDOUT - start signal for analog readout\n");
					break;
				}
			}
			fprintf(stderr, "For experts only\n");
			fprintf(stderr, "================\n");
			fprintf(stderr, "OFF  DEAD MM   CD   DEC  MODE VER  QUP  QUA  BE   TCS  MSM   \n");
			fprintf(stderr,
					"0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
					rich_off, rich_dead, rich_mm, rich_cd, rich_dec,
					rich_mode, rich_ver, rich_qup, rich_qua, rich_be, rich_tcs, rich_msm);
		}

		if (strcmp(dtuType, "show") == 0) {
			if (show_ack & 0x04) {
				fprintf(stderr, "Acknowledge Low is masked\n");
			} else {
				fprintf(stderr, "Acknowledge Low is not masked\n");
			}
			if (show_ack & 0x08) {
				fprintf(stderr, "Acknowledge High is masked\n");
			} else {
				fprintf(stderr, "Acknowledge High is not masked\n");
			}
			fprintf(stderr, "Readout board LVL1 delay is %4.1f ns\n", (float) rb_delay * 100);
		}
	} else {
		fprintf(stderr, "LVL2 trigger rate is 1:%ld\n", rate2);
		fprintf(stderr, "LVL2 trigger delay is %4.1f us\n", (float) (delay2 & 0x0f) * 1.25);
		if (delay2 & 0x80) {
			fprintf(stderr, "LVL2 triggers are enabled\n");
		} else {
			fprintf(stderr, "LVL2 triggers are disabled\n");
		}
		if (delay2 & 0x40) {
			fprintf(stderr, "Matching Unit is enabled\n");
		} else {
			fprintf(stderr, "Matching Unit is disabled\n");
		}
		if (status_lvl1 & 0x10) {
			fprintf(stderr, "CTU lvl1 delay module is enabled\n");
		} else {
			fprintf(stderr, "CTU lvl1 delay module is disabled\n");
		}
		if (status_lvl1 & 0x20) {
			fprintf(stderr, "CTU busy timeout is enabled\n");
		} else {
			fprintf(stderr, "CTU busy timeout is disabled\n");
		}
		if (fifo_ctu & 0x07) {
			if (fifo_ctu & 0x01) {
				fprintf(stderr, "CTU Fifo is empty\n");
			}
			if (fifo_ctu & 0x06) {
				fprintf(stderr, "CTU Fifo is full\n");
			}
		} else {
			fprintf(stderr, "CTU Fifo contains data\n");
		}
		fprintf(stderr, "CTU Fifo position is %2ld      \n", (fifo_ctu & 0xf0) >> 4);
		if (status_lvl1 & 0x08) {
			fprintf(stderr, "CTU is stopped\n");
		} else {
			fprintf(stderr, "CTU is started\n");
		}
		fprintf(stderr, "LVL1 counter is 0x%2.2lx\n", count1);
		fprintf(stderr, "LVL2 counter is 0x%2.2lx\n", count2);
	}
	unmapVme(lvme);
	return 0;
}

#endif
