#include <stddef.h>

#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <signal.h>
#include <math.h>

#include "rc.h"

#define MAX_FES 6
#define MAX_PORTS 8
#define NCHANNELS 64

char rcsid[]= "@(#)$Header: /misc/hadesprojects/daq/cvsroot/debug/race/racedbg.c,v 1.2 2003/10/09 17:22:56 hadaq Exp $";
char version[]= "$Revision: 1.2 $";


static int resetRC(Rc *rc) {

  return 0;
}


static int resetFE(Rc *rc)
{
  int i;
  
  for (i = 0; i < MAX_PORTS; i++) {
    Rc_clrTkoutPort(rc, i);
  }
  Rc_setFc(rc, RFIFO);
  Rc_clrNstrb(rc);
  Rc_setNstrb(rc);
  Rc_setFc(rc, NOP);
  
  return 0;
}

static int stop(Rc *rc)
{
  /* PRUEFEN */
  if (!Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in STOP mode !\n");
    return -1;
  }
  
  Rc_clrStop(rc);
#if(0)
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
#endif
}

static int run(Rc *rc)
{
  /* PRUEFEN */
  if (Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in RUN mode !\n");
    return -1;
  }
  Rc_ClrCtrs(rc);
  Rc_setMemfull(rc, 0x1f4);
  Rc_clrMsel(rc);
  Rc_setStop(rc);
}


static int dump(Rc *rc)
{
  printf("RC99 register dump:\n");
  printf("Interface XC (V %d R %d)\n",
         (Rc_getInterfaceVersion(rc) & 0xf0) >> 4, 
         (Rc_getInterfaceVersion(rc) & 0x0f));
  printf("\tCtrlReg:     0x%04x\tTrgReg:     0x%04x\n", 
         Rc_read_iCtrlReg(rc), 
         Rc_read_iTrgReg(rc));
  printf("\tCtrlPad:     0x%04x\tTrgPad:     0x%04x\n", 
         Rc_read_iCtrlPad(rc), 
         Rc_read_iTrgPad(rc));
  printf("\tTrgCtr:      0x%04x\tPrdCtr:     0x%04x\n", 
         Rc_read_iTrgCtr(rc), 
         Rc_read_iPrdOutCtr(rc));
  printf("\tArdCtr:      0x%04x\tAdelCtr:    0x%04x\n", 
         Rc_read_iArdOutCtr(rc), 
         Rc_read_iAdelCtr(rc));
  printf("\tTagReg:      0x%04x\tLcReg:      0x%04x\n", 
         Rc_read_iTagReg(rc), 
         Rc_read_iLcReg(rc));
  printf("\n");
  printf("Memory XC (V %d R %d)\n", 
         (Rc_read_mRev(rc) & 0xff00) >> 8, 
         (Rc_read_mRev(rc) & 0x00ff));
  printf("\tStatusID:    0x%04x\tBug:        0x%04x\n", 
         Rc_read_mStatusId(rc), 
         Rc_read_mBugReg(rc));
  printf("\tMemReg:      0x%04x\tPages:      0x%04x\n", 
         Rc_read_mMemReg(rc), 
         Rc_read_mPages(rc));
  printf("\tSEDEC_high:  0x%04x\tSEDEC_low:  0x%04x\n", 
         Rc_read_mSedec_high(rc), 
         Rc_read_mSedec_low(rc));
  printf("\tSEID_high:   0x%04x\tSEID_low:   0x%04x\n", 
         Rc_read_mSeid_high(rc), 
         Rc_read_mSeid_low(rc));
  printf("\tVersion:     0x%04x\tSwCtr:      0x%04x\n", 
         Rc_read_mRev(rc), 
         Rc_read_mSwCtr(rc));

  if (!Rc_getStop(rc)) {
    int i;
    printf("\n");
    printf("Timing XC (V %d R %d)\n", 
           (Rc_read_tRevReg(rc) & 0xff00) >> 8, 
           (Rc_read_tRevReg(rc) & 0x00ff));
    printf("\tTagReg:      0x%04x\tBadReg:     0x%04x\n", 
           Rc_read_tTagReg(rc), 
           Rc_read_tBadReg(rc));
    printf("\tErrReg:      0x%04x\tSeqReg:     0x%04x\n",
           Rc_read_tErrorReg(rc), 
           Rc_read_tSeqReg(rc));
    printf("\tTERROR:      0x%04x\tMERROR:     0x%04x\n",
           Rc_read_tTctrReg(rc), 
           Rc_read_tMctrReg(rc));
    printf("\tSERROR:      0x%04x\tCYCLES:     0x%04x\n",
           Rc_read_tSctrReg(rc), 
           Rc_read_tCycleReg(rc));
    printf("\tNomRc:       0x%04x\tNomRCP:     0x%04x\n",
           Rc_read_tNomrcReg(rc), 
           Rc_read_tNomrcpReg(rc));
    printf("\tMaskReg:     0x%04x\tStwReg:     0x%04x\n",
           Rc_read_tMaskReg(rc), 
           Rc_read_tStwReg(rc));
    printf("\tDebugReg:    0x%04x\n", 0xffff);
    for (i = 0; i < MAX_PORTS; i++) {
      printf("\n");
      printf("Port#%d XC (V %d R %d)\n", i, 
             (Rc_read_pRevReg(rc, i) & 0xff00) >> 8, 
             (Rc_read_pRevReg(rc, i) & 0x00ff));
      printf("\tCtrlReg:     0x%04x\tStatReg:    0x%04x\n",
             Rc_read_pCtrlReg(rc, i), 
             Rc_read_pStatReg(rc, i));
      printf("\tRevReg:      0x%04x\n", 
             Rc_read_pRevReg(rc, i));
    }
  }
  
  return 0;
}

static int feReg(int port)
{
  return port * 0x20 + 0x4;
}

static int analogPattern(Rc *rc, int port, int AnalogReadout)
{
  int i, j, Modul, PrintModul;
  short PStatus, PData, ch, data, nTags, nData;

  Modul = 0;
  PrintModul = 1;
  nTags = 0;
  nData = 0;
    
  if (port < 0 || port >= MAX_PORTS) {
    fprintf(stderr, "Error: port %d out of range\n", port);
    return -1;
  }
  
  /* /STOP mode */
  Rc_clrStop(rc);
  
  /* Reset daisy chain */
  /* ---- */
  Rc_setFc(rc, NOP);
  Rc_clrTkoutPort(rc, port);
  Rc_setFc(rc, NOP);
  
  Rc_setFc(rc, RDAISY);
  Rc_clrNstrb(rc);
  Rc_setNstrb(rc);
  /* ---- */
  
  if (AnalogReadout) {
    /*ARDOUT*/
    Rc_setFc(rc, ARDOUT);
    Rc_setTkoutPort(rc, port);
    printf(" analog readout: \n");
  } else {
    /* PRDOUT */
    Rc_setFc(rc, PRDOUT);
    Rc_setTkoutPort(rc, port);
    printf(" pattern readout: \n");
  }
  
  for (i = 0; i < (65 * 20 + 1); i++) {
    if (PrintModul) {
      printf(" Modul %d : \n", Modul);
      PrintModul = 0;
    }
    
    /* ---- */
    Rc_clrNstrb(rc);
    Rc_clrTkoutPort(rc, port);
    Rc_setTkoutPort(rc, port);
    PStatus =Rc_read_pStatReg(rc, port);
    PData = Rc_readDB(rc, feReg(port));
    Rc_setNstrb(rc);
    /* ---- */
    if ((PStatus & 0x0080) == 0x0080) {
      printf(" END\n");
      /* Reset daisy chain
         Rc_setFc(rc, RDAISY);
         Rc_StrobePort(rc, port); */
      break;
    }
    if ((PStatus & 0x0040) == 0x0040) {
      printf(" T  ");
      printf(" %04x \n", PData & 0xFFFF);
      nTags = nTags + 1;
      printf(" data: %d , tags: %d \n", nData, nTags);
      Modul = Modul + 1;
      PrintModul = 1;
    } else {
      if (AnalogReadout) {
    printf(" A  ");
      } else {
    printf(" P  ");
      }
      ch = (PData >> 10) & 0x3F;
      data = PData & 0x03FF;
      nData = nData + 1;
      printf(" %04x ( %d , %d ) \n", PData & 0xFFFF, ch, data);
    }
  }
  /* ---- */
  Rc_setFc(rc, NOP);
  Rc_clrTkoutPort(rc, port);
  Rc_setFc(rc, NOP);
  /* ---- */

  return 0;
}

static int analog(Rc *rc, int port)
{
  if (Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in RUN mode !\n");
    return -1;
  }
  
  return analogPattern(rc, port, 1);
}

static int pattern(Rc *rc, int port)
{
  if (Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in RUN mode !\n");
    return -1;
  }

  return analogPattern(rc, port, 1);
}

static void plc(Rc *rc)
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

static int status(Rc *rc)
{
  unsigned short trgc, prdc, ardc, adelc, asum;
  unsigned long sedec, seid, lo, hi;
  
  printf("Status of RC99:\n");
  printf("\tMode          :  %s\n", 
         Rc_getStop(rc) ? "RUN" : "/STOP");
  printf("\t/RC busy      :    %1x => %s\n", 
         Rc_padRcbsy(rc), 
         Rc_padRcbsy(rc) ? "inactive" : "active");
  printf("\t/Trigger busy :    %1x => %s\n", 
         Rc_padTbsy(rc), 
         Rc_padTbsy(rc) ? "inactive" : "active");
  printf("\t/Error        :    %1x => %s\n", 
         Rc_padError(rc), 
         Rc_padError(rc) ? "no error" : "ERROR");
  printf("\tMemory        :  %s\n", 
         Rc_testMemfull(rc) ? "full" : "not full");
  printf("\tBankSwitch    :  0x%04x\n", 
         Rc_SwitchStatus(rc));
  printf("\tLast tag      :  0x%04x\n", 
         Rc_getTagReg(rc));
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
  
  lo = (unsigned long) Rc_readSedec_low(rc);
  hi = (unsigned long) Rc_readSedec_high(rc);
  sedec = lo | (hi << 16);
  lo = Rc_readSeid_low(rc);
  hi = Rc_readSeid_high(rc);
  seid = lo | (hi << 16);
  
  printf("\tStatusID      :  0x%04x\n", Rc_getStatusID(rc));
  printf("\tBug           :  0x%04x\n", Rc_getBug(rc));
  printf("\tMemReg        :  0x%04x\n", Rc_readMemreg(rc));
  printf("\tPages         :  0x%04x\n", Rc_readPages(rc));
  printf("\tSubevent dec. :  0x%08x\n", sedec);
  printf("\tSubevent id   :  0x%08x\n", seid);
  
  return 0;
}

static int error(Rc *rc)
{
  unsigned int x1, x2;
  
  if (Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in RUN mode !\n");
    return -1;
  }
  printf("Error traceback information\n");
  printf("===========================\n");
  printf("Number of errors in this RC:\n");
  printf("#TERRORs:      %d\n", Rc_read_tTctrReg(rc));
  printf("#MERRORs:      %d\n", Rc_read_tMctrReg(rc));
  printf("#SERRORs:      %d\n", Rc_read_tSctrReg(rc));
  printf("#total  :      %d  in   %d cycles ( %d allowed)\n",
         (Rc_read_tSeqReg(rc) >> 8) & 0xff, 
         Rc_read_tCycleReg(rc),
         Rc_getSloppy(rc));
  printf("Port error traceback (last readout cycle):\n");
  printf("TERROR: 0x%02x\n", (Rc_read_tErrorReg(rc) & 0xff));
  printf("MERROR: 0x%02x\n", (Rc_read_tErrorReg(rc) >> 8) & 0xff);
  printf("SERROR: 0x%02x\n", (Rc_read_tSeqReg(rc) & 0xff));
  printf("Tag information:\n");
  printf("Reference tag: 0x%02x\n", Rc_getRefTag(rc));
  printf("Offending tag: 0x%02x\n", Rc_getBadTag(rc));
  printf("Port MACK reports:\n");

  x1 = Rc_read_tNomrcReg(rc);
  x2 = Rc_read_tNomrcpReg(rc);
  
  printf("P0: %d (set %d)  P1: %d (set %d)\n", 
         (Rc_read_pStatReg(rc, 0) >> 12) & 0x0f, (x1 >> 0) & 0x0f, 
         (Rc_read_pStatReg(rc, 1) >> 12) & 0x0f, (x2 >> 0) & 0x0f);
  printf("P2: %d (set %d)  P3: %d (set %d)\n",
         (Rc_read_pStatReg(rc, 2) >> 12) & 0x0f, (x1 >> 4) & 0x0f, 
         (Rc_read_pStatReg(rc, 3) >> 12) & 0x0f, (x2 >> 4) & 0x0f);
  printf("P4: %d (set %d)  P5: %d (set %d)\n",
         (Rc_read_pStatReg(rc, 4) >> 12) & 0x0f, (x1 >> 8) & 0x0f,
         (Rc_read_pStatReg(rc, 5) >> 12) & 0x0f, (x2 >> 8) & 0x0f);
  printf("P6: %d (set %d)  P7: %d (set %d)\n",
         (Rc_read_pStatReg(rc, 6) >> 12) & 0x0f, (x1 >> 12) & 0x0f,
         (Rc_read_pStatReg(rc, 7) >> 12) & 0x0f, (x2 >> 12) & 0x0f);

  return 0;
}

static int swrq(Rc *rc)
{
  if (!Rc_getStop(rc)) {
    fprintf(stderr, "RC99 is in STOP mode !\n");
    return -1;
  }
  Rc_setSwrq(rc);
  Rc_clrSwrq(rc);
  
  return 0;
}

static int switchstatus(Rc *rc)
{
  printf("Status of memory banks:\n");
  switch (Rc_SwitchStatus(rc)) {
  
  case 0x03:
    printf("\tBank A requested and activated\n");  
    break;
  case 0x02:
    printf("\tBank B requested, request still pending\n");
    break;
  case 0x01:
    printf("\tBank A requested, request still pending\n");
    break;
  case 0x00:
    printf("\tBank B requested and activated\n");
    break;
  default: 
    printf("\tStatus unknown\n");
    return -1;
  }
  
  printf("\tSelected memorybank has %d pages used and is %s\n",
          (int)Rc_getPages(rc), Rc_testMemfull(rc) ? "full" : "not full");
  
  return 0;
}

static int dumpmem(Rc *rc, int numPages)
{
  int pageSize = 256;
  int addr = 0;
  int size = (numPages > 0 && numPages < 512) ? pageSize * numPages : 0;
  
  for (addr = 0; addr < size; addr += 4) {
    if ((addr % 16) == 0)printf("\n0x%08x:", addr);
    Rc_readMem(rc, addr);
    printf(" %08x", Rc_readMem(rc, addr));
  } 
  printf("\n");
  
  return 0;
}


static void usage(const char *progName)
{
  fprintf(stderr, "%s: %s\n", progName, version);
  fprintf(stderr, "Usage: %s -b base command\n", progName);
  fprintf(stderr, "where command may be one of the following:\n");
  
  /*fprintf(stderr, "reset             (S/R) resets readout controller and "
    "front end modules\n");
    fprintf(stderr, "reset RC          (S/R) resets readout controller\n"); */
  fprintf(stderr, "resetFE           (S/R) resets front end modules\n");
  fprintf(stderr, "dump              (S/R) dump readout controller "
          "registers\n");
  fprintf(stderr, "mode stop         (-/R) set stop mode\n");
  fprintf(stderr, "mode run          (S/-) set run mode\n");
  fprintf(stderr, "analog #port      (S/-) analog data of port #port for "
          "one trigger\n");
  fprintf(stderr, "pattern #port     (S/-) pattern data of port #port for "
          "one trigger\n");
  fprintf(stderr, "status            (S/R) readout controller status "
          "summary\n");
  fprintf(stderr, "error             (S/-) display error traceback and "
          "statistics\n");
  fprintf(stderr, "swrq              (-/R) sends a SwitchRequest\n");
  fprintf(stderr, "switchstatus      (S/R) shows the status of the memory "
          "banks\n");
  fprintf(stderr, "dumpmem [pages]   (S/R) dumps [pages] of memory to "
          "stdout\n");
}


int main(int argc, char *argv[])
{
  char progName[200];
  Rc rcS, *rc = &rcS;
  unsigned long cardBase = 0xFFFFFFFF;
  char *command = NULL;
  char *commandOpt = NULL;
  extern char *optarg;
  extern int optind;
  int i;
  
  sprintf(progName, basename(argv[0]));
  while ((i = getopt(argc, argv, "+hb:")) != -1) {
    switch (i) {
    case '?':
      usage(progName);
      exit(EXIT_FAILURE);
    case 'h':
      usage(progName);
      exit(EXIT_SUCCESS);
    case 'b':
      cardBase = strtoul(optarg, NULL, 0);
      break;
    }
  }
  
  if (cardBase == 0xFFFFFFFF || argc - optind < 1) {
    /* no -b flag or no command given */
    usage(progName);
    exit(EXIT_FAILURE);
  }
    
  command = argv[optind++];
  if (argc - optind > 0) commandOpt = argv[optind++];
  
  if (conRc(rc, "RC99", cardBase) == -1) {
    fprintf(stderr, "conRc\n");
    exit(EXIT_FAILURE);
  }
  
  if (strcmp(command, "resetFE") == 0) {
    resetFE(rc);
  } else if (strcmp(command, "dump") == 0) {
    dump(rc);
  } else if (strcmp(command, "mode") == 0) {
    if(commandOpt != NULL) {
      if (strcmp(commandOpt, "stop") == 0) {
        stop(rc);
      } else if (strcmp(commandOpt, "run") == 0) {
        run(rc);
      } else {
        usage(progName);
        exit(EXIT_FAILURE);
      }
    } else {
      usage(progName);
      exit(EXIT_FAILURE);
    }
  } else if (strcmp(command, "analog") == 0) {
    if (commandOpt == NULL) {
      usage(progName);
      exit(EXIT_FAILURE);
    } 
    analog(rc, atoi(commandOpt));
  } else if (strcmp(command, "pattern") == 0) {
    if (commandOpt == NULL) {
      usage(progName);
      exit(EXIT_FAILURE);
    }
    pattern(rc, atoi(commandOpt));
  } else if (strcmp(command, "status") == 0) {
    status(rc);
  } else if (strcmp(command, "error") == 0) {
    error(rc);
  } else if (strcmp(command, "swrq") == 0) {
    swrq(rc);
  } else if (strcmp(command, "switchstatus") == 0) {
    switchstatus(rc);
  } else if (strcmp(command, "dumpmem") == 0) {
    if (commandOpt != NULL) {
      dumpmem(rc, atoi(commandOpt));
    } else {
      dumpmem(rc, 1);
    }
  } else {
    usage(progName);
  }
  
  desRc(rc);
  
  exit(EXIT_SUCCESS);
}
