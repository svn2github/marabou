static const char rcsId[] =
  "$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/race/race_lib.c,v 1.41 2005/04/14 11:48:26 hadaq Exp $";
#define _POSIX_C_SOURCE 199309L

#if HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include <allParam.h>

#include "rc.h"
#include "race_lib.h"


#include "../support/xild.h"

#define MAXNFES 6
#define NCHANNELS 64
#define MAX_PORTS 8

static void markComment(char *line)
{
  char *pos;

  pos = strchr(line, '#');
  if (pos != NULL) {
    *pos = '\0';
  }
}

static int wrdpr(Rc * rc, FILE * f)
{
  int retVal = 0;
  int off;
  int val;
  char line[81];
  int nItems;

  while (NULL != fgets(line, 80, f)) {
    markComment(line);
    nItems = sscanf(line, "%i%i", &off, &val);
    if (nItems == 2) {
      Rc_writeDpr(rc, off, val);
      if (val != Rc_readDpr(rc, off)) {
        syslog(LOG_DEBUG, "race: 0x%04x written: %d read: %d", off, val,
               Rc_readDpr(rc, off));
        retVal = -1;
      }
    } else if (nItems != -1) {
      syslog(LOG_ERR, "race: Malformed input: %s", line);
      retVal = -1;
    }
  }
  return retVal;
}

static int feReg(int port)
{
  return port * 0x20 + 0x4;
}

static int wrthr(Rc * rc, const char *name, FILE * f, int port)
{
  int retVal = 0;
  int i;
  int ch;
  int val;
  char line[81];
  int nItems;
  int vals[NCHANNELS * MAXNFES];
  int nVals;

  /* read data from file to memory */
  nVals = 0;
  while (NULL != fgets(line, 80, f)) {
    markComment(line);
    nItems = sscanf(line, "%d%d", &ch, &val);
    if (nItems == 2) {
      vals[nVals++] = ((ch & 0x3f) << 10) | (val & 0x3ff);
    } else if (nItems != -1) {
      syslog(LOG_ERR, "race: Malformed input: %s", line);
      retVal = -1;
    }
  }

  /* write data to front ends */
  Rc_setFc(rc, NOP);
  Rc_setNstrb(rc);
  for (i = 0; i < MAX_PORTS; i++) {
    Rc_clrTkoutPort(rc, i);
  }
  Rc_setFc(rc, RFIFO);
  Rc_clrNstrb(rc);
  Rc_setNstrb(rc);
  Rc_setFc(rc, WCFG);
  Rc_setTkoutPort(rc, port);

  syslog(LOG_DEBUG, "%s: Write thresholds to front ends at port %d", name,
         port);
  for (i = 0; i < nVals; i++) {
    Rc_writeDB(rc, feReg(port), vals[i]);
    val = vals[i] & 0x3ff;
    ch = (vals[i] >> 10) & 0x3f;
    Rc_clrNstrb(rc);
    /* printf("W: Port %d: 0x%04x: 0x%04x\n", port, i, Rc_read_pStatReg(rc, port)); */
    Rc_setNstrb(rc);
  }
  Rc_setFc(rc, NOP);
  Rc_clrTkoutPort(rc, port);
  Rc_setFc(rc, NOP);

  Rc_setFc(rc, RFIFO);
  Rc_clrNstrb(rc);
  Rc_setNstrb(rc);

  /* read data from front ends and verify */
  Rc_setFc(rc, RCFG);
  Rc_setTkoutPort(rc, port);

  Rc_setPortTrans(rc, port);
  for (i = 0; i < nVals; i++) {
    Rc_clrNstrb(rc);
/*    Rc_clrTkoutPort(rc, port); */
/*    Rc_setTkoutPort(rc, port); */
    val = Rc_readDB(rc, feReg(port));
    /* printf("R: Port %d: 0x%04x: 0x%04x => 0x%04x channel %d \n", port, i, Rc_read_pStatReg(rc, port), val, ((val >> 10) & 0x3f)); */
    Rc_setNstrb(rc);
    if (val != vals[i]) {
      syslog(LOG_DEBUG, "%s: 0x%04x written: %d read: %d", name, i, vals[i],
             val);
      retVal = -1;
    }
  }
  Rc_clrPortTrans(rc, port);
  Rc_setFc(rc, NOP);
  Rc_clrTkoutPort(rc, port);
  Rc_setFc(rc, NOP);

  return retVal;
}

static int resetFe(Rc * rc)
{
  int retVal = 0;
  int i;

  for (i = 0; i < MAX_PORTS; i++) {
    Rc_clrTkoutPort(rc, i);
  }
  Rc_setFc(rc, RFIFO);
  Rc_clrNstrb(rc);
  Rc_setNstrb(rc);
  Rc_setFc(rc, NOP);

  return retVal;
}

static int loadAllFe(const char *name, Rc * rc, unsigned long cardBase,
                     const char *fname, unsigned long int *poff)
{
  int retVal = 0;
  LVme lvmeS, *lvme = &lvmeS;
  FILE *file;
  int i;

  for (i = 0; i < MAX_PORTS; i++) {
    if (poff[i]) {
      Rc_clrXcfg(rc, i);
    } else {
      Rc_setXcfg(rc, i);
    }
  }
  if (NULL != (lvme = newLVme(cardBase + xcRegion, xcSpace, 0x09))) {
    if (NULL != (file = fopen(fname, "r"))) {
      retVal = ConfigXilinxD8(name, lvme, 0x03, 1, 2, 3, 6, 7, file);
      fclose(file);
      for (i = 0; i < MAX_PORTS; i++) {
        Rc_clrXcfg(rc, i);
      }
      if (resetFe(rc)) {
        retVal = -1;
      }
    } else {
      retVal = -1;
    }
    delLVme(lvme);
  } else {
    retVal = -1;
  }
  return retVal;
}

static int loadFe(const char *name, Rc * rc, unsigned long cardBase, int port,
                  const char *fname)
{
  int retVal = 0;
  LVme lvmeS, *lvme = &lvmeS;
  FILE *file;
  int i;

  for (i = 0; i < MAX_PORTS; i++) {
    Rc_clrXcfg(rc, i);
  }
  Rc_setXcfg(rc, port);
  if (NULL != (lvme = newLVme(cardBase + xcRegion, xcSpace, 0x09))) {
    if (NULL != (file = fopen(fname, "r"))) {
      retVal = ConfigXilinxD8(name, lvme, 0x03, 1, 2, 3, 6, 7, file);
      fclose(file);
      Rc_clrXcfg(rc, port);
      if (resetFe(rc)) {
        retVal = -1;
      }
    } else {
      retVal = -1;
    }
    delLVme(lvme);
  } else {
    retVal = -1;
  }

  return retVal;
}

static int loadRc(const char *name, Rc * rc, unsigned long cardBase,
                  const char *fname)
{
  int retVal = 0;
  LVme lvmeS, *lvme = &lvmeS;
  FILE *file;

  if (NULL != (lvme = newLVme(cardBase + xcRegion, xcSpace, 0x09))) {
    if (NULL != (file = fopen(fname, "r"))) {
      retVal = ConfigXilinxD8(name, lvme, 0x03, 0, 2, 3, 4, 5, file);
      if(retVal!=0){
	syslog(LOG_ERR, "%s: ConfigXilinxD8 returns %i", name,retVal);
      }

      fclose(file);
    } else {
      syslog(LOG_ERR, "%s: could not open RcFPGA-file %s", name,fname);
      retVal = -1;
    }
    delLVme(lvme);
  } else {
    syslog(LOG_ERR, "%s: failed to create newLVme", name);
    retVal = -1;
  }

  return retVal;
}

static int resetRc(Rc * rc,
                   unsigned long int *poff,
                   unsigned long int *eoff, unsigned long int *numFe,
                   unsigned long int stw, unsigned long int sloppy)
{
  struct timespec t = { 0, 1000000 };
  int retVal = 0;
  int i;

  Rc_clrStop(rc);
  nanosleep(&t, NULL);
  Rc_clrReset(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_setReset(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_Cycle(rc);
  Rc_setMemfull(rc, 0x01e0);
  Rc_writeSedec_high(rc, 0x0002);
  Rc_writeSedec_low(rc, 0x0001);
  Rc_writeSeid_high(rc, 0x0);
  Rc_writeSeid_low(rc, 100);
  Rc_setStrobeWidth(rc, stw);
  Rc_setSloppy(rc, sloppy);


  for (i = 0; i < MAX_PORTS; i++) {
    if (poff[i]) {
      Rc_setPortOff(rc, i);
      Rc_setPortBsyOff(rc, i);
    } else {
      Rc_clrPortOff(rc, i);
      Rc_clrPortBsyOff(rc, i);
    }
  }

  for (i = 0; i < MAX_PORTS; i++) {
    if (eoff[i]) {
      Rc_setErrorOff(rc, i);
    } else {
      Rc_clrErrorOff(rc, i);
    }
  }

  for (i = 0; i < MAX_PORTS; i++) {
    Rc_setNumFe(rc, i, numFe[i]);
    if (numFe[i] == 1) {
      Rc_setFeMode(rc, i);
    } else {
      Rc_clrFeMode(rc, i);
    }
  }

  return retVal;
}

static void setId(Rc * rc, unsigned long id)
{
  Rc_writeSeid_high(rc, id >> 16);
  Rc_writeSeid_low(rc, id & 0xffff);
}

static int ourInit(const char *name, const Param * param)
{
  Rc rcS, *rc = &rcS;
  FILE *f;
  int i;
  int port;
  int retVal = 0;
  int rows;
  char file[PARAM_MAX_VALUE_LEN];
  char *fefpgafile[MAX_PORTS];
  unsigned long int poff[MAX_PORTS];
  unsigned long int cardbase = 0;

  if (Param_getInt(param, name, "cardbase", &rows, &cardbase)
      || (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found.", name);
    retVal = -1;
  } else {
    if (conRc(rc, name, cardbase)) {
      syslog(LOG_ERR, "%s: conRc failed", name);

      retVal = -1;
    } else if (Param_getFilename(param, name, "rcfpgafile", &rows, file)
               || (rows != 1)) {
      syslog(LOG_ERR, "%s: Parameter rcfpgafile not found.", name);
      retVal = -1;
      desRc(rc);
    } else {
      if (loadRc(name, rc, cardbase, file)) {
	syslog(LOG_ERR, "%s: loadRc failed", name);

        retVal = -1;
      }

      for (i = 0; i < MAX_PORTS; i++) {
        poff[i] = 0;
      }
      if (Param_getIntArray(param, name, "poff", MAX_PORTS, &rows, poff)
          || (rows != MAX_PORTS)) {
        syslog(LOG_NOTICE, "%s: Parameter poff not found.", name);
      }
      if ((Param_getFilename(param, name, "allfefile", &rows, file) 
           == 0) && (rows == 1)) {
        if (loadAllFe(name, rc, cardbase, file, poff)) {
	  syslog(LOG_ERR, "%s: loadAllFe failed", name);

          retVal = -1;
        }
      } else {
        for (port = 0; port < MAX_PORTS; port++) {
          fefpgafile[port] = malloc(PARAM_MAX_VALUE_LEN);
        }
        if ((Param_getFilenameArray
             (param, name, "fefpgafile", MAX_PORTS, &rows, fefpgafile) == 0)
            && (rows == MAX_PORTS)) {
          for (port = 0; port < MAX_PORTS; port++) {
            if (!poff[port]) {
              if (loadFe(name, rc, cardbase, port, fefpgafile[port])) {
		syslog(LOG_ERR, "%s: loadFe failed", name);

                retVal = -1;
              }
            }
          }
        } else {
          syslog(LOG_ERR, "%s: Cannot determine all fefpgafiles", name);
          retVal = -1;
        }
        for (port = 0; port < MAX_PORTS; port++) {
          free(fefpgafile[port]);
        }
      }

      if (Param_getFilename(param, name, "dprfile", &rows, file)
          || (rows != 1)) {
        syslog(LOG_ERR, "%s: Parameter dprfile not found.", name);
        retVal = -1;
      } else {
        if (NULL != (f = fopen(file, "r"))) {
          if (wrdpr(rc, f)) {
            retVal = -1;
          }
          fclose(f);
        } else {
          syslog(LOG_ERR, "%s: Cannot open %s", name, file);
          retVal = -1;
        }
      }

      desRc(rc);
    }
  }    
 
  
  /* Load Thresholds */
  if (retVal == 0) {
    if ((retVal = Race_reset(name, param)) == 0) {
      retVal = Race_loadThresholds(name, param);
    }
  }
  
  return retVal;
}

int Race_init(const char *name, const Param * param)
{
  int retVal;
  int retries;

  retries = 0;
  do {
    retVal = ourInit(name, param);
  } while (retVal != 0 && retries++ != 10);

  if (retVal != 0) {
    syslog(LOG_ERR, "%s: Init failed after %d retries", name, retries);
  }
  return retVal;
}


int Race_loadThresholds(const char *name, const Param * param)
{
  Rc rcS, *rc = &rcS;
  FILE *f;
  int rows;
  int port;
  int retVal = 0;
  unsigned long int poff[MAX_PORTS];
  char *thrfile[MAX_PORTS];
  unsigned long int cardbase = 0;
  unsigned long int thr_id;

  for (port = 0; port < MAX_PORTS; port++) {
    poff[port] = 0;
    thrfile[port] = malloc(PARAM_MAX_VALUE_LEN);
  }

  if (Param_getInt(param, name, "cardbase", &rows, &cardbase)
      || (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found.", name);
    retVal = -1;
  } else {
    if (Param_getInt(param, name, "thr_id", &rows, &thr_id)
        || (rows != 1)) {
      syslog(LOG_ERR, "%s: Parameter thr_id not found.", name);
      retVal = -1;
    } else {
      Param_storeInt(param, name, "start_thr_id", thr_id);
      if (Param_getIntArray(param, name, "poff", MAX_PORTS, &rows, poff)
          || (rows != MAX_PORTS)) {
        syslog(LOG_NOTICE, "%s: Parameter poff not found.", name);
      }

      if ((Param_getFilenameArray
           (param, name, "thrfile", MAX_PORTS, &rows, thrfile) == 0)
          && (rows == MAX_PORTS)) {
        if (conRc(rc, name, cardbase)) {
          retVal = -1;
        } else {
          for (port = 0; port < MAX_PORTS; port++) {
            if (!poff[port]) {
              if (NULL != (f = fopen(thrfile[port], "r"))) {
                if (wrthr(rc, name, f, port)) {
                  retVal = -1;
                }
                fclose(f);
              } else {
                syslog(LOG_ERR, "%s: Cannot open %s", name, thrfile[port]);
                retVal = -1;
              }
            }
          }
          desRc(rc);
        }
      } else {
        syslog(LOG_ERR, "%s: Cannot determine all thrfiles", name);
        retVal = -1;
      }
      for (port = 0; port < MAX_PORTS; port++) {
        free(thrfile[port]);
      }

      if (retVal != 0) {
        Param_storeInt(param, name, "fail_thr_id", thr_id);
      } else {
        Param_storeInt(param, name, "succ_thr_id", thr_id);
      }
    }
  }
  return retVal;
}

int Race_reset(const char *name, const Param * param)
{
  int retVal = 0;
  Rc rcS, *rc = &rcS;
  int rows;
  int i;
  unsigned long int cardbase = 0;
  unsigned long int se_id = 0;
  unsigned long int numFe[MAX_PORTS];
  unsigned long int poff[MAX_PORTS];
  unsigned long int eoff[MAX_PORTS];
  unsigned long int irq_vector = 0;
  unsigned long int irq_on = 0;
  unsigned long int stw = 3;
  unsigned long int sloppy = 0;

  if (Param_getInt(param, name, "cardbase", &rows, &cardbase)
      || (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found.", name);
    retVal = -1;
  } else {
    if (conRc(rc, name, cardbase)) {
      retVal = -1;
    } else {


      for (i = 0; i < MAX_PORTS; i++) {
        poff[i] = 0;
        eoff[i] = 0;
        numFe[i] = 5;
      }
      if (Param_getIntArray(param, name, "poff", MAX_PORTS, &rows, poff)
          || (rows != MAX_PORTS)) {
        syslog(LOG_NOTICE, "%s: Parameter poff not found, setting all 0",
               name);
      }
      if (Param_getIntArray(param, name, "numfe", MAX_PORTS, &rows, numFe)
          || (rows != MAX_PORTS)) {
        syslog(LOG_NOTICE, "%s: Parameter numfe not found, setting all 5",
               name);
      }
      if (Param_getIntArray(param, name, "eoff", MAX_PORTS, &rows, eoff)
          || (rows != MAX_PORTS)) {
        syslog(LOG_NOTICE, "%s: Parameter eoff not found, setting all 0",
               name);
      }
      if (Param_getInt(param, name, "stw", &rows, &stw)
          || (rows != 1)) {
        syslog(LOG_NOTICE, "%s: Parameter stw not found, setting 3", name);
        retVal = -1;
      }
      if (Param_getInt(param, name, "sloppy", &rows, &sloppy)
          || (rows != 1)) {
        syslog(LOG_NOTICE, "%s: Parameter sloppy not found, setting 0", name);
        retVal = -1;
      }
      if (resetRc(rc, poff, eoff, numFe, stw, sloppy)) {
        retVal = -1;
      }
      if (Param_getInt(param, name, "se_id", &rows, &se_id)
          || (rows != 1)) {
        syslog(LOG_ERR, "%s: Parameter se_id not found", name);
        retVal = -1;
      } else {
        setId(rc, se_id);
      }

      if (Param_getInt(param, name, "irq_vector", &rows, &irq_vector)
          || (rows != 1)) {
        syslog(LOG_NOTICE, "%s: Parameter irq_vector not found, setting %lu",
               name, irq_vector);
      }
      Rc_writeStatusid(rc, irq_vector);

      if (Param_getInt(param, name, "irq_on", &rows, &irq_on)
          || (rows != 1)) {
        syslog(LOG_NOTICE, "%s: Parameter irq_on not found, setting %lu",
               name, irq_on);
      }
      if (irq_on) {
        Rc_IrqRon(rc);
      } else {
        Rc_IrqRoff(rc);
      }
      if (resetFe(rc)) {
        retVal = -1;
      }

      desRc(rc);
    }
  }
  return retVal;
}

int Race_start(const char *name, const Param * param)
{
  int retVal = 0;
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;

  if (Param_getInt(param, name, "cardbase", &rows, &cardbase)
      || (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found.", name);
    retVal = -1;
  } else {
    if (conRc(rc, name, cardbase)) {
      retVal = -1;
    } else {

      Rc_ClrCtrs(rc);
      Rc_setMemfull(rc, 0x1f4);
      /* Rc_setMsel(rc); */
      /* Rc_writeMskreg(rc, 0xff); */
      Rc_clrMsel(rc);
      Rc_setStop(rc);

      if (Rc_getStop(rc) != 1) {
        retVal = -1;
      }

      desRc(rc);
    }
  }
  return retVal;
}

int Race_stop(const char *name, const Param * param)
{
  int retVal = 0;
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;

  if (Param_getInt(param, name, "cardbase", &rows, &cardbase)
      || (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found.", name);
    retVal = -1;
  } else {
    if (conRc(rc, name, cardbase)) {
      retVal = -1;
    } else {

      Rc_clrStop(rc);
      Rc_clrReset(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_Cycle(rc);
      Rc_setReset(rc);

      desRc(rc);
    }
  }
  return retVal;
}

int Race_initialized(const char *n, const Param * p)
{
  int retVal = -1;

  return retVal;
}

int Race_running(const char *n, const Param * p)
{
  Rc rcS, *rc = &rcS;
  int rows;
  int retVal = -1;
  unsigned long int cardbase = 0;

  if ((Param_getInt(p, n, "cardbase", &rows, &cardbase) == 0)
      && (rows == 1)) {
    if (0 == conRc(rc, n, cardbase)) {
      retVal = Rc_getStop(rc);
      desRc(rc);
    }
  }

  return retVal;
}

int Race_lvl1Busy(const char *n, const Param * p)
{
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;
  int retVal = 0;

  if ((Param_getInt(p, n, "cardbase", &rows, &cardbase) == 0)
      && (rows == 1)) {
    if (conRc(rc, n, cardbase)) {
      retVal = 0;
    } else {
      desRc(rc);
    }
  }

  return retVal;
}

int Race_lvl2Busy(const char *n, const Param * p)
{
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;
  int retVal = 0;

  if ((Param_getInt(p, n, "cardbase", &rows, &cardbase) == 0)
      && (rows == 1)) {
    if (conRc(rc, n, cardbase)) {
      retVal = 0;
    } else {
      desRc(rc);
    }
  }

  return retVal;
}

int Race_error(const char *n, const Param * p)
{
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;
  int retVal = 0;

  if ((Param_getInt(p, n, "cardbase", &rows, &cardbase) == 0)
      && (rows == 1)) {
    if (conRc(rc, n, cardbase)) {
      retVal = 0;
    } else {
      desRc(rc);
    }
  }

  return retVal;
}

int Race_statusRegister(const char *n, const Param * p, const char *r)
{
  unsigned long int retVal = 0;
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;

  if ((0 != Param_getInt(p, n, "cardbase", &rows, &cardbase)) || (0 == rows)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found", n);
  } else if (0 != conRc(rc, n, cardbase)) {
    syslog(LOG_ERR, "%s: conRc() failed", n);
  } else {
    if (0 == strcmp("memFull", r)) {
      retVal = Rc_testMemfull(rc) ? 1 : 0;
    } else if (0 == strcmp("bankSwitch", r)) {
      retVal = Rc_SwitchStatus(rc);
    } else if (0 == strcmp("lastTag", r)) {
      retVal = Rc_getTagReg(rc);
    } else if (0 == strcmp("trigger", r)) {
      retVal = Rc_getTrgCtr(rc);
    } else if (0 == strcmp("pReadout", r)) {
      retVal = Rc_getPrdoutCtr(rc);
    } else if (0 == strcmp("aReadout", r)) {
      retVal = Rc_getArdoutCtr(rc);
    } else if (0 == strcmp("aDelete", r)) {
      retVal = Rc_getAdelCtr(rc);
    } else if (0 == strcmp("aSum", r)) {
      retVal = Rc_getArdoutCtr(rc) + Rc_getAdelCtr(rc);
    }

    desRc(rc);
  }

  return retVal;
}

#ifndef EPICS_RUNCTRL

static void plc(Rc * rc)
{
  unsigned short lc, rcbsy;

  lc = Rc_getLcReg(rc);
  rcbsy = lc >> 3;
  printf("\tlast cycle    :  0x%04x, ", lc);
  switch (lc & 0x7) {
  case 1:
    printf("P readout");
    break;
  case 2:
    printf("A readout");
    break;
  case 4:
    printf("A readout");
    break;
  default:
    printf("ERROR: wrong cycle %x", lc);
    break;
  }
  printf(", %s\n", (rcbsy == 1) ? "acknowledge ok" : "no acknowledge");
}

static void pstatus(Rc * rc)
{
  unsigned short trgc, prdc, ardc, adelc, asum;
  unsigned long sedec, seid, lo, hi;

  printf("Status of RC99:\n");
  printf("\tMode          :  %s\n", Rc_getStop(rc) ? "RUN" : "/STOP");
  printf("\t/RC busy      :    %1x => %s\n", Rc_padRcbsy(rc),
         Rc_padRcbsy(rc) ? "inactive" : "active");
  printf("\t/Trigger busy :    %1x => %s\n", Rc_padTbsy(rc),
         Rc_padTbsy(rc) ? "inactive" : "active");
  printf("\t/Error        :    %1x => %s\n", Rc_padError(rc),
         Rc_padError(rc) ? "no error" : "ERROR");
  printf("\tMemory        :  %s\n", Rc_testMemfull(rc) ? "full" : "not full");
  printf("\tBankSwitch    :  0x%04x\n", Rc_SwitchStatus(rc));
  printf("\tLast tag      :  0x%04x\n", Rc_getTagReg(rc));
  plc(rc);

  trgc = Rc_getTrgCtr(rc);
  prdc = Rc_getPrdoutCtr(rc);
  ardc = Rc_getArdoutCtr(rc);
  adelc = Rc_getAdelCtr(rc);
  asum = ardc + adelc;

  printf("\tTrigger       :  %6d (0x%04x)\n", trgc, trgc);
  printf("\tP-readout     :  %6d (0x%04x)\n", prdc, prdc);
  printf("\tA-readout     :  %6d (0x%04x)\n", ardc, ardc);
  printf("\tA-delete      :  %6d (0x%04x)\n", adelc, adelc);
  printf("\tA-sum         :  %6d (0x%04x)\n", asum, asum);

  lo = (unsigned long)Rc_readSedec_low(rc);
  hi = (unsigned long)Rc_readSedec_high(rc);
  sedec = lo | (hi << 16);
  lo = Rc_readSeid_low(rc);
  hi = Rc_readSeid_high(rc);
  seid = lo | (hi << 16);

  printf("\tStatusID      :  0x%04x\n", Rc_getStatusID(rc));
  printf("\tBug           :  0x%04x\n", Rc_getBug(rc));
  printf("\tMemReg        :  0x%04x\n", Rc_readMemreg(rc));
  printf("\tPages         :  0x%04x\n", Rc_readPages(rc));
  printf("\tSubevent dec. :  0x%08lx\n", sedec);
  printf("\tSubevent id   :  0x%08lx\n", seid);

  printf("\n------------------------------------------------\n\n");

  printf("RC99 register dump:\n");
  printf("Interface XC (V %d R %d)\n",
         (Rc_getInterfaceVersion(rc) & 0xf0) >> 4,
         (Rc_getInterfaceVersion(rc) & 0x0f));
  printf("\tCtrlReg:   0x%04x\tTrgReg:    0x%04x\n", Rc_read_iCtrlReg(rc),
         Rc_read_iTrgReg(rc));
  printf("\tCtrlPad:   0x%04x\tTrgPad:    0x%04x\n", Rc_read_iCtrlPad(rc),
         Rc_read_iTrgPad(rc));
  printf("\tTrgCtr:    0x%04x\tPrdCtr:    0x%04x\n", Rc_read_iTrgCtr(rc),
         Rc_read_iPrdOutCtr(rc));
  printf("\tArdCtr:    0x%04x\tAdelCtr:   0x%04x\n", Rc_read_iArdOutCtr(rc),
         Rc_read_iAdelCtr(rc));
  printf("\tTagReg:    0x%04x\tLcReg:     0x%04x\n", Rc_read_iTagReg(rc),
         Rc_read_iLcReg(rc));
  printf("\tGpioReg:   0x%04x\tGpioPad:   0x%04x\n", Rc_getGpioReg(rc),
         Rc_getGpioPad(rc));
  printf("\n");
  printf("Memory XC (V %d R %d)\n", (Rc_read_mRev(rc) & 0xff00) >> 8,
         (Rc_read_mRev(rc) & 0x00ff));
  printf("\tStatusID:  0x%04x\tBug:       0x%04x\n", Rc_read_mStatusId(rc),
         Rc_read_mBugReg(rc));
  printf("\tMemReg:    0x%04x\tPages:     0x%04x\n", Rc_read_mMemReg(rc),
         Rc_read_mPages(rc));
  printf("\tSEDEC_high:0x%04x\tSEDEC_low: 0x%04x\n", Rc_read_mSedec_high(rc),
         Rc_read_mSedec_low(rc));
  printf("\tSEID_high: 0x%04x\tSEID_low:  0x%04x\n", Rc_read_mSeid_high(rc),
         Rc_read_mSeid_low(rc));
  printf("\tVersion:   0x%04x\tSwCtr:     0x%04x\n", Rc_read_mRev(rc),
         Rc_read_mSwCtr(rc));
}

int Race_status(const char *name, const Param * param)
{
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;

  Param_getInt(param, name, "cardbase", &rows, &cardbase);
  if (0 == conRc(rc, name, cardbase)) {
    pstatus(rc);

    desRc(rc);
  }

  return 0;
}

static void pdump(Rc * rc)
{
  printf("RC99 register dump:\n");
  printf("Interface XC (V %d R %d)\n",
         (Rc_getInterfaceVersion(rc) & 0xf0) >> 4,
         (Rc_getInterfaceVersion(rc) & 0x0f));
  printf("\tCtrlReg:   0x%04x\tTrgReg:    0x%04x\n", Rc_read_iCtrlReg(rc),
         Rc_read_iTrgReg(rc));
  printf("\tCtrlPad:   0x%04x\tTrgPad:    0x%04x\n", Rc_read_iCtrlPad(rc),
         Rc_read_iTrgPad(rc));
  printf("\tTrgCtr:    0x%04x\tPrdCtr:    0x%04x\n", Rc_read_iTrgCtr(rc),
         Rc_read_iPrdOutCtr(rc));
  printf("\tArdCtr:    0x%04x\tAdelCtr:   0x%04x\n", Rc_read_iArdOutCtr(rc),
         Rc_read_iAdelCtr(rc));
  printf("\tTagReg:    0x%04x\tLcReg:     0x%04x\n", Rc_read_iTagReg(rc),
         Rc_read_iLcReg(rc));
  printf("\n");
  printf("Memory XC (V %d R %d)\n", (Rc_read_mRev(rc) & 0xff00) >> 8,
         (Rc_read_mRev(rc) & 0x00ff));
  printf("\tStatusID:  0x%04x\tBug:       0x%04x\n", Rc_read_mStatusId(rc),
         Rc_read_mBugReg(rc));
  printf("\tMemReg:    0x%04x\tPages:     0x%04x\n", Rc_read_mMemReg(rc),
         Rc_read_mPages(rc));
  printf("\tSEDEC_high:0x%04x\tSEDEC_low: 0x%04x\n", Rc_read_mSedec_high(rc),
         Rc_read_mSedec_low(rc));
  printf("\tSEID_high: 0x%04x\tSEID_low:  0x%04x\n", Rc_read_mSeid_high(rc),
         Rc_read_mSeid_low(rc));
  printf("\tVersion:   0x%04x\tSwCtr:     0x%04x\n", Rc_read_mRev(rc),
         Rc_read_mSwCtr(rc));
}

int Race_dump(const char *name, const Param * param)
{
  Rc rcS, *rc = &rcS;
  int rows;
  unsigned long int cardbase = 0;

  Param_getInt(param, name, "cardbase", &rows, &cardbase);
  if (0 == conRc(rc, name, cardbase)) {
    pdump(rc);

    desRc(rc);
  }

  return 0;
}

#endif
