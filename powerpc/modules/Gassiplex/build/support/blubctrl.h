#ifndef BLUBCTRL_H
#define BLUBCTRL_H

static enum {
	init,
	reset,
	start,
	stop,
	loadThresholds,
	initialized,
	running,
	lvl1Busy,
	lvl2Busy,
	error,
	statusRegister,
	status,
	FunTab_size
} FunTab_names;

int blubctrl(int argc, char *argv[], const char *buildDate, int (*funtab[])());

#endif
