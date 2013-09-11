#ifndef XILD_H
#define XILD_H
#include <unistd.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include <lvme.h>

int ConfigXilinx(const char *name, LVme *lVme, unsigned long offset,
                 int prog, int din, int cclk, int init, int done, FILE *XRBT);

int ConfigXilinxD8(const char *name, LVme *lVme, unsigned long offset,
                   int prog, int din, int cclk, int init, int done,
                   FILE *XRBT);

#endif
