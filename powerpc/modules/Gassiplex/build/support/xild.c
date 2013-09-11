static const char rcsId[] =
    "$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/xild.c,v 1.10 2003/07/06 23:05:01 hadaq Exp $";
#define _POSIX_C_SOURCE 199506L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>

#include <lvme.h>

/* --------------------------------- */
/* Put a string of bits to the FPGA. */
/* --------------------------------- */
static void Str2Bit(LVme *lVme, unsigned long offset, int prog, int din,
                    int cclk, int init, int done, char *Str)
{
  int Pos;
  unsigned reg;
  unsigned hh;
  unsigned hl;
  unsigned lh;
  unsigned ll;

  LVme_nop(lVme);
  reg = LVme_getB(lVme, offset);
  hh = (reg | (1 << cclk)) | (1 << din);
  hl = (reg | (1 << cclk)) & ~(1 << din);
  lh = (reg & ~(1 << cclk)) | (1 << din);
  ll = (reg & ~(1 << cclk)) & ~(1 << din);


  for (Pos = 0; Str[Pos] == '1' || Str[Pos] == '0'; Pos++) {
    LVme_nop(lVme);
    if (Str[Pos] == '0') {
      LVme_setB(lVme, offset, ll);
      LVme_setB(lVme, offset, hl);
    } else {
      LVme_setB(lVme, offset, lh);
      LVme_setB(lVme, offset, hh);
    }
    LVme_nop(lVme);
    LVme_getB(lVme, offset);
    LVme_getB(lVme, offset);
    LVme_getB(lVme, offset);
    LVme_getB(lVme, offset);
    LVme_nop(lVme);
  }
}

/* -----------------------------------------------*/
/* Load a bitfile into the Xlinx-FPGAs and check. */
/* ---------------------------------------------- */
int ConfigXilinx(const char *name, LVme *lVme, unsigned long offset,
                 int prog, int din, int cclk, int init, int done, FILE *XRBT)
{
  int i, ExitCode;
  char Line[255];
  struct timespec t = { 0, 20000000 };

  syslog(LOG_DEBUG, "%s: xild: Start config cycle", name);
  LVme_nop(lVme);
  LVme_clrBitW(lVme, offset, cclk);
  LVme_clrBitW(lVme, offset, prog);
  LVme_setBitW(lVme, offset, prog);

  LVme_nop(lVme);
  for (i = 0; ((i < 50) && (!LVme_tstBitW(lVme, offset, init)))
       || LVme_tstBitW(lVme, offset, done); i++) {
    nanosleep(&t, NULL);
  }

  if (LVme_tstBitW(lVme, offset, init)
      && !LVme_tstBitW(lVme, offset, done)) {
    syslog(LOG_DEBUG, "%s: xild: Start of bit transfer", name);
    i = 0;
    while (fgets(Line, 255, XRBT) != 0 && LVme_tstBitW(lVme, offset, init)) {
      switch (Line[0]) {
      case '0':
      case '1':
        Str2Bit(lVme, offset, prog, din, cclk, init, done, Line);
        break;
      default:
        break;
      }
    }
    for (i = 0; i < 50 && !LVme_tstBitW(lVme, offset, init); i++) {
      nanosleep(&t, NULL);
    }

    if (LVme_tstBitW(lVme, offset, init)) {
      Str2Bit(lVme, offset, prog, din, cclk, init, done,
              "1111111111111111111111111111111111111111"
              "1111111111111111111111111111111111111111\n");
      syslog(LOG_DEBUG, "%s: xild: End of bit transfer", name);
      if (LVme_tstBitW(lVme, offset, init)
          && LVme_tstBitW(lVme, offset, done)) {
        syslog(LOG_INFO, "%s: xild: Configuration successful", name);
        ExitCode = 0;
      } else {
        syslog(LOG_ERR, "%s: xild: Configuration failed (done not H)", name);
        ExitCode = 4;           /* Done geht nicht auf H */
      }
    } else {
      syslog(LOG_ERR, "%s: xild: Configuration failed (init H to early)",
             name);
      ExitCode = 3;             /* Uebertragungsfehler */
    }
  } else {
    syslog(LOG_ERR, "%s: xild: Configuration failed (error at config start)",
           name);
    ExitCode = 1;               /* Fehler bei Config-Start */
  }
  return ExitCode;
}

int ConfigXilinxD8(const char *name, LVme *lVme, unsigned long offset,
                   int prog, int din, int cclk, int init, int done, FILE *XRBT)
{
  int i, ExitCode;
  char Line[255];
  struct timespec t = { 0, 20000000 };

  syslog(LOG_DEBUG, "%s: xild: Start config cycle", name);
  LVme_nop(lVme);
  LVme_clrBitB(lVme, offset, cclk);
  LVme_clrBitB(lVme, offset, prog);
  LVme_setBitB(lVme, offset, prog);

  LVme_nop(lVme);
  for (i = 0; ((i < 50) && (!LVme_tstBitB(lVme, offset, init)))
       || LVme_tstBitB(lVme, offset, done); i++) {
    nanosleep(&t, NULL);
  }

  if (LVme_tstBitB(lVme, offset, init)
      && !LVme_tstBitB(lVme, offset, done)) {
    syslog(LOG_DEBUG, "%s: xild: Start of bit transfer", name);
    i = 0;
    while (fgets(Line, 255, XRBT) != 0 && LVme_tstBitB(lVme, offset, init)) {
      switch (Line[0]) {
      case '0':
      case '1':
        Str2Bit(lVme, offset, prog, din, cclk, init, done, Line);
        break;
      default:
        break;
      }
    }
    for (i = 0; i < 50 && !LVme_tstBitB(lVme, offset, init); i++) {
      nanosleep(&t, NULL);
    }

    if (LVme_tstBitB(lVme, offset, init)) {
      Str2Bit(lVme, offset, prog, din, cclk, init, done,
              "1111111111111111111111111111111111111111"
              "1111111111111111111111111111111111111111\n");
      syslog(LOG_DEBUG, "%s: xild: End of bit transfer", name);
      if (LVme_tstBitB(lVme, offset, init)
          && LVme_tstBitB(lVme, offset, done)) {
        syslog(LOG_INFO, "%s: xild: Configuration successful", name);
        ExitCode = 0;
      } else {
        syslog(LOG_ERR, "%s: xild: Configuration failed (done not H)", name);
        ExitCode = 4;           /* Done geht nicht auf H */
      }
    } else {
      syslog(LOG_ERR, "%s: xild: Configuration failed (init H to early)",
             name);
      ExitCode = 3;             /* Uebertragungsfehler */
    }
  } else {
    syslog(LOG_ERR, "%s: xild: Configuration failed (error at config start)",
           name);
    ExitCode = 1;               /* Fehler bei Config-Start */
  }
  return ExitCode;
}
