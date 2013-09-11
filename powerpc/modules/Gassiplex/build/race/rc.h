#ifndef RC_H
#define RC_H

#include <stddef.h>
#include <stdio.h>
#include <lvme.h>

typedef struct RcS
{
  LVme *lvme;
  LVme *memRegion;
  LVme *busRegion;
  LVme *regRegion;
  LVme *xcRegion;
  LVme *dprRegion;
} Rc;

enum Rc_regions
{
  memRegion = 0x100000,
  busRegion = 0x200000,
  regRegion = 0x300000,
  xcRegion = 0x400000,
  dprRegion = 0x500000
};

enum Rc_addrSpaces
{
  memSpace = 0x80000,
  busSpace = 0x1000,
  regSpace = 0x1000,
  xcSpace = 0x1000,
  dprSpace = 0x1000
};


int Rc_invariant(Rc * my);
int conRc(Rc * my, const char *name, unsigned long vmeBase);
void desRc(Rc * my);


/* ---------------------------------------------------------------------- */

/***********************************************************************
 * TIMING2 Xilinx part                                                 *
 * based on verXrevY                                                   *
 *                                                                     *
 * Keep in mind: TIMING2 is a 2nd class Xilinx -> busRegion            * 
 *                                                                     *
 * Registers:                                                          *
 *                                                                     *
 * Offset       Name       Access                                      *
 * ------------------------------                                      *
 * 0x100        TAG        ro                                          *
 * 0x102        BAD        ro                                          *
 * 0x104        ERROR      ro                                          *
 * 0x106        SEQ        ro                                          *
 * 0x108        TCTR       ro                                          *
 * 0x10a        MCTR       ro                                          *
 * 0x10c        SCTR       ro                                          *
 * 0x10e        CYCLES     ro                                          *
 * 0x110        NOMRC      r/w                                         *
 * 0x112        NOMRCP     r/w                                         *
 * 0x114        MASK       r/w                                         *
 * 0x116        STW        r/w                                         *
 * 0x118        DBG0       ro                                          *
 * 0x11a        DBG1       ro                                          *
 * 0x11c        DBG2       ro                                          *
 * 0x11e        REV        ro                                          *
 *                                                                     *
 ***********************************************************************/

/* Offsets for TIMING2 registers */
static const ptrdiff_t tTagReg = 0x100;
static const ptrdiff_t tBadReg = 0x102;
static const ptrdiff_t tErrorReg = 0x104;
static const ptrdiff_t tSeqReg = 0x106;
static const ptrdiff_t tTctrReg = 0x108;
static const ptrdiff_t tMctrReg = 0x10a;
static const ptrdiff_t tSctrReg = 0x10c;
static const ptrdiff_t tCycleReg = 0x10e;
static const ptrdiff_t tNomrcReg = 0x110;
static const ptrdiff_t tNomrcpReg = 0x112;
static const ptrdiff_t tMaskReg = 0x114;
static const ptrdiff_t tStwReg = 0x116;
static const ptrdiff_t tDbg0Reg = 0x118;
static const ptrdiff_t tDbg1Reg = 0x11a;
static const ptrdiff_t tDbg2Reg = 0x11c;
static const ptrdiff_t tRevReg = 0x11e;


/***********************************************************************
 * PORT Xilinx part                                                    *
 *                                                                     *
 * Registers per XC:                                                   *
 *                                                                     *
 * Offset       Name       Access                                      *
 * ------------------------------                                      *
 * 0x00         CTRL       r/w                                         *
 * 0x02         STAT       ro                                          *
 * 0x04         PORT       r/w                                         *
 * 0x10         HIST0      r/o                                         *
 * 0x12         HIST1      r/o                                         *
 * 0x14         HIST2      r/o                                         *
 * 0x16         HIST3      r/o                                         *
 * 0x18         HIST4      r/o                                         *
 * 0x1e         REV        ro                                          *
 *                                                                     *
 ***********************************************************************/
static const ptrdiff_t pPortOffset = 0x020;
/* Offsets for PORT registers */
static const ptrdiff_t pCtrlReg = 0x00;
static const ptrdiff_t pStatReg = 0x02;
static const ptrdiff_t pPortReg = 0x04;
static const ptrdiff_t pHist0Reg = 0x10;
static const ptrdiff_t pHist1Reg = 0x12;
static const ptrdiff_t pHist2Reg = 0x14;
static const ptrdiff_t pHist3Reg = 0x16;
static const ptrdiff_t pHist4Reg = 0x18;
static const ptrdiff_t pRevReg = 0x1e;


/***********************************************************************
 * INTERFACE3 Xilinx part                                              *
 * based on ver3rev1, Feb. 2000                                        *
 *                                                                     *
 * Registers per XC:                                                   *
 *                                                                     *
 * Offset       Name       Access                                      *
 * ------------------------------                                      *
 * 0x000        CtrlReg    r/w                                         *
 * 0x002        CtrlPad    ro                                          *
 * 0x004        TrgReg     r/w                                         *
 * 0x006        TrgPad     ro                                          *
 * 0x008        TgrCtr     ro                                          *
 * 0x00a        PrdOutCtr  ro                                          *
 * 0x00c        ArdOutCtr  ro                                          *
 * 0x00e        AdelCtr    ro                                          *
 * 0x010        TagReg     ro                                          *
 * 0x012        LcReg      ro                                          *
 * 0x014        Unused1    --                                          *
 * 0x016        Unused2    --                                          *
 * 0x018        Unused3    --                                          *
 * 0x01a        Unused4    --                                          *
 * 0x01c        GiopReg    ro                                          *
 * 0x01e        GiopPad    ro                                          *
 *                                                                     *
 ***********************************************************************/
/* Offsets for INTERFACE registers */
static const ptrdiff_t iCtrlReg = 0x000;
static const ptrdiff_t iCtrlPad = 0x002;
static const ptrdiff_t iTrgReg = 0x004;
static const ptrdiff_t iTrgPad = 0x006;
static const ptrdiff_t iTgrCtr = 0x008;
static const ptrdiff_t iPrdOutCtr = 0x00a;
static const ptrdiff_t iArdOutCtr = 0x00c;
static const ptrdiff_t iAdelCtr = 0x00e;
static const ptrdiff_t iTagReg = 0x010;
static const ptrdiff_t iLcReg = 0x012;
#if 0
static const ptrdiff_t iUnused1 = 0x014;
static const ptrdiff_t iUnused2 = 0x016;
static const ptrdiff_t iUnused3 = 0x018;
static const ptrdiff_t iUnused4 = 0x01a;
#endif
static const ptrdiff_t iGpioReg = 0x01c;
static const ptrdiff_t iGpioPad = 0x01e;

/* ---- Function Codes -------------------------------------------------- */
static const int PRDOUT = 0x0;  /* Pattern readout (daisy chain)       */
static const int RCFG = 0x1;    /* Read configuration (daisy chain)    */
static const int ARDOUT = 0x2;  /* Analog readout (daisy chain)        */
static const int ADEL = 0x3;    /* Analog delete (buslike)             */
static const int RFIFO = 0x4;   /* Reset FIFO (buslike)                */
static const int RDAISY = 0x5;  /* Reset daisy chain (buslike)         */
static const int WCFG = 0x6;    /* Write configuration (daisy chain)   */
static const int NOP = 0x7;     /* No Operation - do nothing           */



/***********************************************************************
 * MEMORY3 Xilinx part                                                 *
 * based on ver7rev2 Feb. 2000                                         *
 *                                                                     *
 * Registers:                                                          *
 *                                                                     *
 * Offset       Name          Access                                   *
 * ---------------------------------                                   *
 * 0x100        StatusId     r/w                                       *
 * 0x102        BugReg       ro                                        *
 * 0x104        MemReg       r/w                                       *
 * 0x106        Pages        ro                                        *
 * 0x108        Sedec_high   r/w                                       *
 * 0x10a        Sedec_low    r/w                                       *
 * 0x10c        Seid_high    r/w                                       *
 * 0x10e        Seid_low     r/w                                       *
 * 0x110        Rev          ro                                        *
 * 0x112        SwCtr        ro                                        *
 * 0x114        CodeMem      r/w                                       *
 * 0x116        Unused1      --                                        *
 * 0x118        Unused2      --                                        *
 * 0x11a        Unused3      --                                        *
 * 0x11c        Unused4      --                                        *
 * 0x11e        Unused5      --                                        *
 *                                                                     *
 ***********************************************************************/

/* Offsets for MEMORY2 registers */
static const ptrdiff_t mStatusId = 0x100;
static const ptrdiff_t mBugReg = 0x102;
static const ptrdiff_t mMemReg = 0x104;
static const ptrdiff_t mPages = 0x106;
static const ptrdiff_t mSedec_high = 0x108;
static const ptrdiff_t mSedec_low = 0x10a;
static const ptrdiff_t mSeid_high = 0x10c;
static const ptrdiff_t mSeid_low = 0x10e;
static const ptrdiff_t mRev = 0x110;
static const ptrdiff_t mSwCtr = 0x112;
static const ptrdiff_t mCodeMem = 0x114;
#if 0
static const ptrdiff_t mUnused1 = 0x116;
static const ptrdiff_t mUnused2 = 0x118;
static const ptrdiff_t mUnused3 = 0x11a;
static const ptrdiff_t mUnused4 = 0x11c;
static const ptrdiff_t mUnused5 = 0x11e;
#endif

/* ---- PAGES register -------------------------------------------------- */

/* ---- Do not use this function alone ----*/
static LVme_W Rc_readPages(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mPages);
}

static void Rc_waitSwitchcomplete(Rc * my)
{
  LVme_W realPages = 0x0000;

  realPages = Rc_readPages(my); /* Altes Register retten         */
  realPages = realPages & 0xc000;       /* oberste zwei Bits anschauen   */

  /* Schleife, bis beide Bits gleich */
  while ((realPages != 0x0000) || (realPages != 0xc000)) {
    realPages = (Rc_readPages(my) & 0xc000);
  }
}
static int Rc_SwitchStatus(Rc * my)
{
  return (int)((Rc_readPages(my) >> 14) & 0x03);
  /* 0x03: Bank A requested and activated
     0x02: Bank B requested, request still pending
     0x01: Bank A requested, request still pending
     0x00: Bank B requested and activated          */
}
static int Rc_testMemfull(Rc * my)
{
  return (int)((Rc_readPages(my) >> 13) & 0x01);
}

static void Rc_setSwrq(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 12);
}

static void Rc_clrSwrq(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 12);
}

static unsigned long Rc_readMem(Rc * my, unsigned long offsetValue)
{
  LVme_nop(my->memRegion);
  return LVme_getL(my->memRegion, offsetValue);
}

static void Rc_writeMem(Rc * my, unsigned long offsetValue,
                        unsigned long value)
{
  LVme_nop(my->memRegion);
  LVme_setL(my->memRegion, offsetValue, value);
}

static void Rc_clearMem(Rc * my)
{
  long i;
  const long memSize = 0x80000;
  const long clearValue = 0x00000000;
  for (i = 0; i < memSize; i = i + 0x04) {
    Rc_writeMem(my, i, clearValue);
  }
}

static long Rc_testMem(Rc * my)
{
  long i;
  const long memSize = 0x80000;
  const long clearValue = 0x00000000;
  unsigned long different = 0x00000000;
  unsigned long memValue = 0x00000000;
  for (i = 0; i < memSize; i = i + 0x04) {
    memValue = Rc_readMem(my, i);
    if (memValue != clearValue) {
      different++;
      printf("A: 0x%08x: 0x%08x\n", (unsigned int)i, (unsigned int)memValue);
    }
  }
  return different;
}

static unsigned short Rc_getPages(Rc * my)
{
  return (unsigned short)(Rc_readPages(my) & 0x07ff);
}


/**************************************************************************
 * Internal DB bus part                                                   *
 **************************************************************************/

static void Rc_writeDB(Rc * my, ptrdiff_t offset, unsigned short data)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, offset, data);
}
static unsigned short Rc_readDB(Rc * my, ptrdiff_t offset)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, offset);
}

/* ---------------------------------------------------------------------- */
/* ---- TIMING-Xilinx Register direct access ---------------------------- */
/* ---------------------------------------------------------------------- */

static LVme_W Rc_read_tTagReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tTagReg);
}

static LVme_W Rc_read_tBadReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tBadReg);
}

static LVme_W Rc_read_tErrorReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tErrorReg);
}

static LVme_W Rc_read_tSeqReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tSeqReg);
}

static LVme_W Rc_read_tMaskReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tMaskReg);
}

static void Rc_write_tMaskReg(Rc * my, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, tMaskReg, value);
}

static LVme_W Rc_read_tRevReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tRevReg);
}

static LVme_W Rc_read_tStwReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tStwReg);
}

static void Rc_write_tStwReg(Rc * my, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, tStwReg, value);
}

static LVme_W Rc_read_tNomrcReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tNomrcReg);
}

static void Rc_write_tNomrcReg(Rc * my, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, tNomrcReg, value);
}

static LVme_W Rc_read_tNomrcpReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tNomrcpReg);
}

static void Rc_write_tNomrcpReg(Rc * my, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, tNomrcpReg, value);
}

static LVme_W Rc_read_tTctrReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tTctrReg);
}

static LVme_W Rc_read_tMctrReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tMctrReg);
}

static LVme_W Rc_read_tSctrReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tSctrReg);
}

static LVme_W Rc_read_tCycleReg(Rc * my)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, tCycleReg);
}


/* ---------------------------------------------------------------------- */
/* ---- TIMING-Xilinx Register Functions -------------------------------- */
/* ---------------------------------------------------------------------- */

/* ---- TAG register definitions ---------------------------------------- */
static unsigned int Rc_getTriggerType(Rc * my)
{
  return ((Rc_read_tTagReg(my) >> 0x08) & 0x03);
}

static unsigned int Rc_getRefTag(Rc * my)
{
  return (Rc_read_tTagReg(my) & 0xff);
}

/* ---- BAD register definitions ---------------------------------------- */
static unsigned int Rc_getBadTag(Rc * my)
{
  return (Rc_read_tBadReg(my) & 0xff);
}

/* ---- ERROR register definitions -------------------------------------- */
static unsigned int Rc_getMackErrSrc(Rc * my)
{
  return ((Rc_read_tErrorReg(my) >> 8) & 0xff);
}

static unsigned int Rc_getTagErrSrc(Rc * my)
{
  return (Rc_read_tErrorReg(my) & 0xff);
}

/* ---- SEQ register definitions -------------------------------------- */
static unsigned int Rc_getSeqErrSrc(Rc * my)
{
  return (Rc_read_tSeqReg(my) & 0xff);
}

static unsigned int Rc_getNumOfErrors(Rc * my)
{
  return ((Rc_read_tSeqReg(my) >> 8) & 0xff);
}

/* ---- STW register definitions ---------------------------------------- */
/* ---- strobe-width[ns] = 32 * (3 + stw) ) ---- */
static void Rc_setStrobeWidth(Rc * my, unsigned int stw)
{
  LVme_W value;

  value = (Rc_read_tStwReg(my) & ~((LVme_W) 0x0f)) | (stw & 0x0f);
  Rc_write_tStwReg(my, value);
}
static unsigned int Rc_getStrobeWidth(Rc * my)
{
  return (Rc_read_tStwReg(my) & 0x0f);
}

static void Rc_setSloppy(Rc * my, unsigned int sloppy)
{
  LVme_W value;

  value = (Rc_read_tStwReg(my) & ~((LVme_W) 0xff00)) | ((sloppy & 0xff) << 8);
  Rc_write_tStwReg(my, value);
}
static unsigned int Rc_getSloppy(Rc * my)
{
  return ((Rc_read_tStwReg(my) >> 8) & 0xff);
}


/* ---- MASK register definitions --------------------------------------- */
static void Rc_setPortOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, tMaskReg, (port & 0x07) + 8);
}
static void Rc_clrPortOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, tMaskReg, (port & 0x07) + 8);
}
static int Rc_getPortOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, tMaskReg, (port & 0x07) + 8);
}

static void Rc_setErrorOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, tMaskReg, (port & 0x07));
}
static void Rc_clrErrorOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, tMaskReg, (port & 0x07));
}
static int Rc_getErrorOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, tMaskReg, (port & 0x07));
}

/* ---- NOMRC(P) register definitions ----------------------------------- */
static void Rc_setNumFe(Rc * my, unsigned int port, unsigned int numFe)
{
  LVme_W value;
  LVme_W mask;

  port = port & 0x07;
  numFe = (numFe & 0x07) << ((port / 2) * 4);
  mask = ~(0x0f << ((port / 2) * 4));
  /*printf("DEBUG: => mask 0x%04x numFe 0x%04x\n", mask, numFe);*/
  if (port % 2 == 0) {
    value = (Rc_read_tNomrcReg(my) & mask);
    /*    printf("DEBUG: => value read: 0x%04x new 0x%04x\n", value, (value | numFe));*/
    Rc_write_tNomrcReg(my, value | numFe);
  } else {
    value = Rc_read_tNomrcpReg(my) & mask;
    Rc_write_tNomrcpReg(my, value | numFe);
  }
}
static unsigned int Rc_getNumFe(Rc * my, unsigned int port)
{
  unsigned int shift;

  port = port & 0x07;
  shift = (port / 2) * 4;
  if (port % 2 == 0) {
    return (unsigned int)((Rc_read_tNomrcReg(my) >> shift) & 0x07);
  } else {
    return (unsigned int)((Rc_read_tNomrcReg(my) >> shift) & 0x07);
  }
}


/* ---------------------------------------------------------------------- */
/* ---- PORT-Xilinx Register direct access ------------------------------ */
/* ---------------------------------------------------------------------- */

static LVme_W Rc_read_pCtrlReg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg);
}

static void Rc_write_pCtrlReg(Rc * my, unsigned int port, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, value);
}

static LVme_W Rc_read_pStatReg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, (port & 0x07) * pPortOffset + pStatReg);
}

static LVme_W Rc_read_pPortReg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, ((port & 0x07) * pPortOffset + pPortReg));
}

static void Rc_write_pPortReg(Rc * my, unsigned int port, LVme_W value)
{
  LVme_nop(my->busRegion);
  LVme_setW(my->busRegion, (port & 0x07) * pPortOffset + pPortReg, value);
}

static LVme_W Rc_read_pRevReg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_getW(my->busRegion, (port & 0x07) * pPortOffset + pRevReg);
}

/* ---------------------------------------------------------------------- */
/* ---- PORT-Xilinx Register Functions ---------------------------------- */
/* ---------------------------------------------------------------------- */

/* ---- Ctrl register definitions --------------------------------------- */
static void Rc_setPortBsyOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 4);
}
static void Rc_clrPortBsyOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 4);
}
static int Rc_getPortBsyOff(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg,
                      4);
}

static void Rc_setFeMode(Rc * my, unsigned int port)
{
  fprintf(stderr, "setFeMode for port: %d\n", port);
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 3);
}
static void Rc_clrFeMode(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 3);
}
static int Rc_getFeMode(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg,
                      3);
}

static void Rc_setPortTrans(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 2);
}
static void Rc_clrPortTrans(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 2);
}
static int Rc_getPortTrans(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg,
                      2);
}

static void Rc_setPortOut(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 1);
}

static void Rc_clrPortOut(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 1);
}
static int Rc_getPortOut(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg,
                      1);
}

static void Rc_setXcfg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_setBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 0);
}
static void Rc_clrXcfg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  LVme_clrBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg, 0);
}
static int Rc_getXcfg(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pCtrlReg,
                      0);
}

/* ---- STAT register definitions --------------------------------------- */
static int Rc_getTKIN(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pStatReg,
                      7);
}
static int Rc_getMACK(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pStatReg,
                      5);
}
static int Rc_getTBSY(Rc * my, unsigned int port)
{
  LVme_nop(my->busRegion);
  return LVme_tstBitW(my->busRegion, (port & 0x07) * pPortOffset + pStatReg,
                      4);
}



/* ---------------------------------------------------------------------- */
/* ---- INTERFACE-Xilinx Register direct access ------------------------- */
/* ---------------------------------------------------------------------- */

static LVme_W Rc_read_iCtrlReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iCtrlReg);
}
static void Rc_write_iCtrlReg(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, iCtrlReg, value);
}

static LVme_W Rc_read_iCtrlPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iCtrlPad);
}

static LVme_W Rc_read_iTrgReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTrgReg);
}
static void Rc_write_iTrgReg(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, iTrgReg, value);
}

static LVme_W Rc_read_iTrgPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTrgPad);
}

static LVme_W Rc_read_iTrgCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTgrCtr);
}

static LVme_W Rc_read_iPrdOutCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iPrdOutCtr);
}

static LVme_W Rc_read_iArdOutCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iArdOutCtr);
}

static LVme_W Rc_read_iAdelCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iAdelCtr);
}

static LVme_W Rc_read_iTagReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTagReg);
}

static LVme_W Rc_read_iLcReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iLcReg);
}

static LVme_W Rc_read_iGpioReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iGpioReg);
}

static LVme_W Rc_read_iGpioPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iGpioPad);
}

/* ---------------------------------------------------------------------- */
/* ---- INTERFACE-Xilinx Register Functions ----------------------------- */
/* ---------------------------------------------------------------------- */

/* ---- CTRL register definitions --------------------------------------- */
static LVme_W Rc_getCtrlReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iCtrlReg);
}

static LVme_W Rc_getCtrlPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iCtrlPad);
}

static void Rc_setReset(Rc * my)
{
  LVme_setBitW(my->regRegion, iCtrlReg, 6);
}

static void Rc_clrReset(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 6);
}

static int Rc_getReset(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 6);
}

static int Rc_padReset(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 6);
}

static void Rc_setMwr(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 7);
}

static void Rc_clrMwr(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 7);
}

static int Rc_getMwr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 7);
}

static int Rc_padMwr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 7);
}

static void Rc_setMsel(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 8);
}

static void Rc_clrMsel(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 8);
}

static int Rc_getMsel(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 8);
}

static int Rc_padMsel(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 8);
}

static void Rc_setEvtend(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 9);
}

static void Rc_clrEvtend(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 9);
}

static int Rc_getEvtend(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 9);
}

static int Rc_padEvtend(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 9);
}

static void Rc_setEvtbeg(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 10);
}

static void Rc_clrEvtbeg(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 10);
}

static int Rc_getEvtbeg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 10);
}

static int Rc_padEvtbeg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 10);
}

static void Rc_setSysclk(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 11);
}

static void Rc_clrSysclk(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 11);
}

static int Rc_getSysclk(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 11);
}

static int Rc_padSysclk(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 11);
}

static void Rc_Cycle(Rc * my)
{
  if (Rc_getSysclk(my)) {
    Rc_clrSysclk(my);
    Rc_setSysclk(my);
  } else {
    Rc_setSysclk(my);
    Rc_clrSysclk(my);
  }
}

static int Rc_getSwrq(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 12);
}

static int Rc_padSwrq(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 12);
}

static void Rc_setStop(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 13);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
}

static void Rc_clrStop(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 13);
  LVme_nop(my->regRegion);
  LVme_nop(my->regRegion);
}

static int Rc_getStop(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 13);
}

static int Rc_padStop(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 13);
}

/* --- FC is only three bit wide in the new PHOENIX design. --- */
static void Rc_setFc(Rc * my, unsigned int fc)
{
  LVme_W value;
  value = (LVme_getW(my->regRegion, iCtrlReg) & 0xfff8) | (fc & 0x0007);
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, iCtrlReg, value);
}

static unsigned int Rc_getFc(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iCtrlReg) & 0x0007;
}

/* --- The new PHOENIX design has one common /STRB line. --- */
static void Rc_setNstrb(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 3);
}
static void Rc_clrNstrb(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 3);
}
static int Rc_getNstrb(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 3);
}
static int Rc_padNstrb(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 3);
}

static void Rc_setError(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 4);
}
static void Rc_clrError(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 4);
}
static int Rc_getError(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 4);
}
static int Rc_padError(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 4);
}

static void Rc_setRcbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iCtrlReg, 5);
}
static void Rc_clrRcbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 5);
}
static int Rc_getRcbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlReg, 5);
}
static int Rc_padRcbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iCtrlPad, 5);
}

/* ---- TRG register definitions ---------------------------------------- */
static LVme_W Rc_getTrgReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTrgReg);
}
static LVme_W Rc_getTrgPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTrgPad);
}

static void Rc_setTkout0(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 0);
}
static void Rc_clrTkout0(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 0);
}
static int Rc_getTkout0(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 0);
}
static int Rc_padTkout0(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 0);
}

static void Rc_setTkout1(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 1);
}
static void Rc_clrTkout1(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 1);
}
static int Rc_getTkout1(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 1);
}
static int Rc_padTkout1(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 1);
}

static void Rc_setTkout2(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 2);
}
static void Rc_clrTkout2(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 2);
}
static int Rc_getTkout2(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 2);
}
static int Rc_padTkout2(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 2);
}

static void Rc_setTkout3(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 3);
}
static void Rc_clrTkout3(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 3);
}
static int Rc_getTkout3(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 3);
}
static int Rc_padTkout3(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 3);
}

static void Rc_setTkout4(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 4);
}
static void Rc_clrTkout4(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 4);
}
static int Rc_getTkout4(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 4);
}
static int Rc_padTkout4(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 4);
}

static void Rc_setTkout5(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 5);
}
static void Rc_clrTkout5(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 5);
}
static int Rc_getTkout5(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 5);
}
static int Rc_padTkout5(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 5);
}

static void Rc_setTkout6(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 6);
}
static void Rc_clrTkout6(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 6);
}
static int Rc_getTkout6(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 6);
}
static int Rc_padTkout6(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 6);
}

static void Rc_setTkout7(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 7);
}
static void Rc_clrTkout7(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 7);
}
static int Rc_getTkout7(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 7);
}
static int Rc_padTkout7(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 7);
}

/* Range for port: 0...7; other values are 
   dangerous and no checking for 
   the correct value is done ! */
static void Rc_setTkoutPort(Rc * my, int port)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, port & 0x07);
}
static void Rc_clrTkoutPort(Rc * my, int port)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, port & 0x07);
}
static int Rc_getTkoutPort(Rc * my, int port)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, port & 0x07);
}

static int Rc_padTkoutPort(Rc * my, int port)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, port & 0x07);
}

static void Rc_Strobe(Rc * my)
{
  Rc_clrNstrb(my);
  Rc_setNstrb(my);
}

static void Rc_setDecision(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 8);
}
static void Rc_clrDecision(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 8);
}
static int Rc_getDecision(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 8);
}
static int Rc_padDecision(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 8);
}

static void Rc_setArdout(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 9);
}
static void Rc_clrArdout(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 9);
}
static int Rc_getArdout(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 9);
}
static int Rc_padArdout(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 9);
}

static void Rc_setPrdout(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 10);
}
static void Rc_clrPrdout(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 10);
}
static int Rc_getPrdout(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 10);
}
static int Rc_padPrdout(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 10);
}

static void Rc_setBegrun(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 11);
}
static void Rc_clrBegrun(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 11);
}
static int Rc_getBegrun(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 11);
}
static int Rc_padBegrun(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 11);
}

static void Rc_setTagclk(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 12);
}
static void Rc_clrTagclk(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 12);
}
static int Rc_getTagclk(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 12);
}
static int Rc_padTagclk(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 12);
}

static void Rc_setTagdata(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 13);
}
static void Rc_clrTagdata(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 13);
}
static int Rc_getTagdata(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 13);
}
static int Rc_padTagdata(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 13);
}

static void Rc_setTbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 14);
}
static void Rc_clrTbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 14);
}
static int Rc_getTbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 14);
}
static int Rc_padTbsy(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 14);
}

static void Rc_setTrg(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, iTrgReg, 15);
}
static void Rc_clrTrg(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iTrgReg, 15);
}
static int Rc_getTrg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgReg, 15);
}
static int Rc_padTrg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, iTrgPad, 15);
}

static void Rc_TriggerPort(Rc * my)
{
  Rc_clrTrg(my);
  Rc_setTrg(my);
}

/* ---- Status counter access ------------------------------------------- */
static LVme_W Rc_getTrgCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTgrCtr);
}
static LVme_W Rc_getPrdoutCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iPrdOutCtr);
}
static LVme_W Rc_getArdoutCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iArdOutCtr);
}
static LVme_W Rc_getAdelCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iAdelCtr);
}

static void Rc_setRctr(Rc * my)
{
  LVme_setBitW(my->regRegion, iCtrlReg, 15);
}
static void Rc_clrRctr(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, iCtrlReg, 15);
}

static void Rc_ClrCtrs(Rc * my)
{
  Rc_setRctr(my);
  Rc_clrRctr(my);
}

/* ---- TAG register definitions ---------------------------------------- */

/**************************************************************************
 * This register contains the last distributed SubEventTag                *
 **************************************************************************/

static LVme_W Rc_getTagReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iTagReg);
}

/* ---- LC register definitions ----------------------------------------- */

/*********************************************************************** 
 * This register contains information on the last command delivered    *
 * by the DTU_RICH as well as the acknowledge of the RC99 hardware.    *
 * by this register, a DTU_RICH lockup due to a missing acknowledge of * 
 * the RC99 hardware can be easily detected.                           * 
 ***********************************************************************/

/* ---- Do not use this function alone ! ---- */
static LVme_W Rc_getLcReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iLcReg);
}

static LVme_W Rc_getLcStatus(Rc * my)
{
  LVme_W LcStatus = 0x0000;
  return LcStatus = (Rc_getLcReg(my) & 0x001f);
}

/* ---- GIOP register definitions --------------------------------------- */

/*********************************************************************** 
 * This register allows access to five free Xilinx pins equipped with  *
 * a solder pad.                                                       *
 * By default, these pads are unconnected and can be used for test     *
 * purposes.                                                           *
 ***********************************************************************/

/* ---- Do not use these functions alone ! ---- */
static LVme_W Rc_getGpioReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iGpioReg);
}
static LVme_W Rc_getGpioPad(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, iGpioPad);
}

static LVme_W Rc_getInterfaceVersion(Rc * my)
{
  LVme_W giopPadValue;
  giopPadValue = (0xff00 & Rc_getGpioPad(my)) >> 8;
  return giopPadValue;
}


static LVme_W Rc_read_mStatusId(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mStatusId);
}
static void Rc_write_mStatusId(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mStatusId, value);
}

static LVme_W Rc_read_mBugReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mBugReg);
}

static LVme_W Rc_read_mMemReg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mMemReg);
}

static LVme_W Rc_read_mPages(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mPages);
}

static LVme_W Rc_read_mSedec_high(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSedec_high);
}
static void Rc_write_mSedec_high(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSedec_high, value);
}

static LVme_W Rc_read_mSedec_low(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSedec_low);
}
static void Rc_write_mSedec_low(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSedec_low, value);
}

static LVme_W Rc_read_mSeid_high(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSeid_high);
}
static void Rc_write_mSeid_high(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSeid_high, value);
}

static LVme_W Rc_read_mSeid_low(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSeid_low);
}
static void Rc_write_mSeid_low(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSeid_low, value);
}

static LVme_W Rc_read_mRev(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mRev);
}

static LVme_W Rc_read_mSwCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSwCtr);
}

static LVme_W Rc_read_mCodeMem(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mCodeMem);
}
static void Rc_write_mCodeMem(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mCodeMem, value);
}

/* ---------------------------------------------------------------------- */
/* ---- MEMORY-Xilinx Register Functions -------------------------------- */
/* ---------------------------------------------------------------------- */

/* ---- STATUSID register ----------------------------------------------- */
static unsigned int Rc_readStatusid(Rc * my)
{
  return (unsigned int)(LVme_getW(my->regRegion, mStatusId) & 0x00ff);
}
static void Rc_writeStatusid(Rc * my, unsigned char value)
{
  LVme_W id;
  LVme_nop(my->regRegion);
  id = (LVme_getW(my->regRegion, mStatusId) & 0xff00) | (value & 0x00ff);
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mStatusId, id);
}
static LVme_W Rc_getStatusID(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mStatusId);
}

static void Rc_setIrqR(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, mStatusId, 15);
}
static void Rc_clrIrqR(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, mStatusId, 15);
}
static int Rc_getIrqR(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, mStatusId, 15);
}
static void Rc_setIrqS(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_setBitW(my->regRegion, mStatusId, 14);
}
static void Rc_clrIrqS(Rc * my)
{
  LVme_nop(my->regRegion);
  LVme_clrBitW(my->regRegion, mStatusId, 14);
}
static int Rc_getIrqS(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_tstBitW(my->regRegion, mStatusId, 14);
}

static void Rc_IrqRon(Rc * my)
{
  Rc_setIrqR(my);
}
static void Rc_IrqRoff(Rc * my)
{
  Rc_clrIrqR(my);
}
static void Rc_IrqSon(Rc * my)
{
  Rc_setIrqS(my);
}
static void Rc_IrqSoff(Rc * my)
{
  Rc_clrIrqS(my);
}

/* ---- BUG register ---------------------------------------------------- */
static LVme_W Rc_getBug(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mBugReg);
}

/* ---- MEMREG register ------------------------------------------------- */

/* ---- Do not use this function alone ---- */
static LVme_W Rc_readMemreg(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mMemReg);
}

/* ---- Do not use this function alone ---- */
static void Rc_writeMemreg(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mMemReg, value);
}

static void Rc_setMemfull(Rc * my, LVme_W pages)
{
  LVme_W realMemreg = 0x0000;
  pages = pages & 0x07FF;       /* PAGES auf untere 11 bits begrenzen */
  if (pages == 0x0000) {
    pages++;                    /* PAGES muss groesser 1 sein (RCBSY) */
  }
  realMemreg = Rc_readMemreg(my);       /* Altes Memreg retten        */
  realMemreg = realMemreg & 0xF800;     /* Alte PAGES loeschen        */
  realMemreg = realMemreg | pages;      /* Neue PAGES einblenden      */
  Rc_writeMemreg(my, realMemreg);       /* Memreg zurueckschreiben    */
}


/* ---- SEDEC_HIGH register --------------------------------------------- */
static LVme_W Rc_readSedec_high(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSedec_high);
}
static void Rc_writeSedec_high(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSedec_high, value);
}

/* ---- SEDEC_LOW register ---------------------------------------------- */
static LVme_W Rc_readSedec_low(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSedec_low);
}
static void Rc_writeSedec_low(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSedec_low, value);
}

/* ---- SEID_HIGH register ---------------------------------------------- */
static LVme_W Rc_readSeid_high(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSeid_high);
}
static void Rc_writeSeid_high(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSeid_high, value);
}

/* ---- SEID_LOW register ----------------------------------------------- */
static LVme_W Rc_readSeid_low(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSeid_low);
}
static void Rc_writeSeid_low(Rc * my, LVme_W value)
{
  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mSeid_low, value);
}

/* ---- VERSIONMEM register --------------------------------------------- */
static LVme_W Rc_readVersionMem(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mRev);
}

/* ---- SWCTR register -------------------------------------------------- */
static LVme_W Rc_readSwCtr(Rc * my)
{
  LVme_nop(my->regRegion);
  return LVme_getW(my->regRegion, mSwCtr);
}

/* ---- CODEMEM register ------------------------------------------------ */
static void Rc_HallOfMemory(Rc * my)
{
  int i = 0;
  char low, high;
  LVme_W word;
  char fame[256];

  LVme_nop(my->regRegion);
  LVme_setW(my->regRegion, mCodeMem, 0xc0de);

  do {
    LVme_nop(my->regRegion);
    word = LVme_getW(my->regRegion, mCodeMem);
    low = word & 0xff;
    high = (word >> 8) & 0xff;
    fame[i++] = high;
    fame[i++] = low;

  }
  while (high != 0 || low != 0);
  printf(fame);
}


/**************************************************************************
 * Dualported RAM part                                                    *
 **************************************************************************/

static unsigned short Rc_readDpr(Rc * my, unsigned long offsetValue)
{
  unsigned long realValue = 0x00000000;
  LVme_nop(my->dprRegion);
  realValue = LVme_getL(my->dprRegion, offsetValue);
  realValue = 0x0000FFFF & realValue;
  return realValue;
}
static void Rc_writeDpr(Rc * my, unsigned long offsetValue,
                        unsigned short value)
{
  unsigned long realValue;
  realValue = 0x0000FFFF & value;
  LVme_nop(my->dprRegion);
  LVme_setL(my->dprRegion, offsetValue, realValue);
}
static void Rc_clearDpr(Rc * my)
{
  unsigned short i;
  const unsigned short dprSize = 0x8000;
  const unsigned short clearValue = 0x0000;
  for (i = 0; i < dprSize; i = i + 0x04) {
    if (i % 256 == 0) {
      printf(".");
    }
    Rc_writeDpr(my, i, clearValue);
  }
  printf("\n");
}
static void Rc_fillDpr(Rc * my)
{
  unsigned short i;
  const unsigned short dprSize = 0x8000;
  const unsigned short clearValue = 0xffff;
  for (i = 0; i < dprSize; i = i + 0x04) {
    if (i % 256 == 0) {
      printf(".");
    }
    Rc_writeDpr(my, i, clearValue);
  }
  printf("\n");
}

static unsigned short Rc_testDpr(Rc * my)
{
  unsigned short i;
  const unsigned short dprSize = 0x8000;
  const unsigned short clearValue = 0x0000;
  unsigned short different = 0x0000;
  unsigned short memValue = 0x0000;
  for (i = 0; i < dprSize; i = i + 0x04) {
    memValue = Rc_readDpr(my, i);
    if (memValue != clearValue) {
      different++;
      printf("    -> A: 0x%08x: 0x%04x\n", i, memValue);
    }
  }
  return different;
}


#if 0
static LVme_W Rc_read_tTagReg(Rc * my);
static LVme_W Rc_read_tErrorReg(Rc * my);
static LVme_W Rc_read_tMaskReg(Rc * my);
static void Rc_write_tMaskReg(Rc * my, LVme_W value);
static LVme_W Rc_read_tRevReg(Rc * my);
static LVme_W Rc_read_tStwReg(Rc * my);
static void Rc_write_tStwReg(Rc * my, LVme_W value);
static LVme_W Rc_read_tNomrcReg(Rc * my);
static void Rc_write_tNomrcReg(Rc * my, LVme_W value);
static LVme_W Rc_read_tNomrcpReg(Rc * my);
static void Rc_write_tNomrcpReg(Rc * my, LVme_W value);
static int Rc_testTagError(Rc * my);
static int Rc_testMackError(Rc * my);
static unsigned int Rc_getTriggerType(Rc * my);
static unsigned int Rc_getRefTag(Rc * my);
static unsigned int Rc_getMackErrSrc(Rc * my);
static unsigned int Rc_getTagErrSrc(Rc * my);
static void Rc_setStrobeWidth(Rc * my, unsigned int stw);
static unsigned int Rc_getStrobeWidth(Rc * my);
static void Rc_setPortOff(Rc * my, unsigned int port);
static void Rc_clrPortOff(Rc * my, unsigned int port);
static int Rc_getPortOff(Rc * my, unsigned int port);
static void Rc_setErrorOff(Rc * my, unsigned int port);
static void Rc_clrErrorOff(Rc * my, unsigned int port);
static int Rc_getErrorOff(Rc * my, unsigned int port);
static void Rc_setNumFe(Rc * my, unsigned int port, unsigned int numFe);
static unsigned int Rc_getNumFe(Rc * my, unsigned int port);
static LVme_W Rc_read_pCtrlReg(Rc * my, unsigned int port);
static void Rc_write_pCtrlReg(Rc * my, unsigned int port, LVme_W value);
static LVme_W Rc_read_pStatReg(Rc * my, unsigned int port);
static LVme_W Rc_read_pPortReg(Rc * my, unsigned int port);
static void Rc_write_pPortReg(Rc * my, unsigned int port, LVme_W value);
static LVme_W Rc_read_pRevReg(Rc * my, unsigned int port);
static void Rc_setFeMode(Rc * my, unsigned int port);
static void Rc_clrFeMode(Rc * my, unsigned int port);
static int Rc_getFeMode(Rc * my, unsigned int port);
static void Rc_setPortTrans(Rc * my, unsigned int port);
static void Rc_clrPortTrans(Rc * my, unsigned int port);
static int Rc_getPortTrans(Rc * my, unsigned int port);
static void Rc_setPortOut(Rc * my, unsigned int port);
static void Rc_clrPortOut(Rc * my, unsigned int port);
static int Rc_getPortOut(Rc * my, unsigned int port);
static void Rc_setXcfg(Rc * my, unsigned int port);
static void Rc_clrXcfg(Rc * my, unsigned int port);
static int Rc_getXcfg(Rc * my, unsigned int port);
static int Rc_getTKIN(Rc * my, unsigned int port);
static int Rc_getMACK(Rc * my, unsigned int port);
static int Rc_getTBSY(Rc * my, unsigned int port);
static LVme_W Rc_read_iCtrlReg(Rc * my);
static void Rc_write_iCtrlReg(Rc * my, LVme_W value);
static LVme_W Rc_read_iCtrlPad(Rc * my);
static LVme_W Rc_read_iTrgReg(Rc * my);
static void Rc_write_iTrgReg(Rc * my, LVme_W value);
static LVme_W Rc_read_iTrgPad(Rc * my);
static LVme_W Rc_read_iTgrCtr(Rc * my);
static LVme_W Rc_read_iPrdOutCtr(Rc * my);
static LVme_W Rc_read_iArdOutCtr(Rc * my);
static LVme_W Rc_read_iAdelCtr(Rc * my);
static LVme_W Rc_read_iTagReg(Rc * my);
static LVme_W Rc_read_iLcReg(Rc * my);
static LVme_W Rc_read_iGiopReg(Rc * my);
static LVme_W Rc_read_iGiopPad(Rc * my);
static LVme_W Rc_getCtrlReg(Rc * my);
static LVme_W Rc_getCtrlPad(Rc * my);
static void Rc_setReset(Rc * my);
static void Rc_clrReset(Rc * my);
static int Rc_getReset(Rc * my);
static int Rc_padReset(Rc * my);
static void Rc_setMwr(Rc * my);
static void Rc_clrMwr(Rc * my);
static int Rc_getMwr(Rc * my);
static int Rc_padMwr(Rc * my);
static void Rc_setMsel(Rc * my);
static void Rc_clrMsel(Rc * my);
static int Rc_getMsel(Rc * my);
static int Rc_padMsel(Rc * my);
static void Rc_setEvtend(Rc * my);
static void Rc_clrEvtend(Rc * my);
static int Rc_getEvtend(Rc * my);
static int Rc_padEvtend(Rc * my);
static void Rc_setEvtbeg(Rc * my);
static void Rc_clrEvtbeg(Rc * my);
static int Rc_getEvtbeg(Rc * my);
static int Rc_padEvtbeg(Rc * my);
static void Rc_setSysclk(Rc * my);
static void Rc_clrSysclk(Rc * my);
static int Rc_getSysclk(Rc * my);
static int Rc_padSysclk(Rc * my);
static void Rc_Cycle(Rc * my);
static int Rc_getSwrq(Rc * my);
static int Rc_padSwrq(Rc * my);
static void Rc_setStop(Rc * my);
static void Rc_clrStop(Rc * my);
static int Rc_getStop(Rc * my);
static int Rc_padStop(Rc * my);
static void Rc_setFc(Rc * my, unsigned int fc);
static unsigned int Rc_getFc(Rc * my);
static void Rc_setError(Rc * my);
static void Rc_clrError(Rc * my);
static int Rc_getError(Rc * my);
static int Rc_padError(Rc * my);
static void Rc_setRcbsy(Rc * my);
static void Rc_clrRcbsy(Rc * my);
static int Rc_getRcbsy(Rc * my);
static int Rc_padRcbsy(Rc * my);
static LVme_W Rc_getTrgReg(Rc * my);
static LVme_W Rc_getTrgPad(Rc * my);
static void Rc_setStrb0(Rc * my);
static void Rc_clrStrb0(Rc * my);
static int Rc_getStrb0(Rc * my);
static int Rc_padStrb0(Rc * my);
static void Rc_setStrb1(Rc * my);
static void Rc_clrStrb1(Rc * my);
static int Rc_getStrb1(Rc * my);
static int Rc_padStrb1(Rc * my);
static void Rc_setStrb2(Rc * my);
static void Rc_clrStrb2(Rc * my);
static int Rc_getStrb2(Rc * my);
static int Rc_padStrb2(Rc * my);
static void Rc_setStrb3(Rc * my);
static void Rc_clrStrb3(Rc * my);
static int Rc_getStrb3(Rc * my);
static int Rc_padStrb3(Rc * my);
static void Rc_setStrb4(Rc * my);
static void Rc_clrStrb4(Rc * my);
static int Rc_getStrb4(Rc * my);
static int Rc_padStrb4(Rc * my);
static void Rc_setStrb5(Rc * my);
static void Rc_clrStrb5(Rc * my);
static int Rc_getStrb5(Rc * my);
static int Rc_padStrb5(Rc * my);
static void Rc_setStrb6(Rc * my);
static void Rc_clrStrb6(Rc * my);
static int Rc_getStrb6(Rc * my);
static int Rc_padStrb6(Rc * my);
static void Rc_setStrb7(Rc * my);
static void Rc_clrStrb7(Rc * my);
static int Rc_getStrb7(Rc * my);
static int Rc_padStrb7(Rc * my);
static void Rc_setStrbPort(Rc * my, int port);
static void Rc_clrStrbPort(Rc * my, int port);
static int Rc_getStrbPort(Rc * my, int port);
static int Rc_padStrbPort(Rc * my, int port);
static void Rc_StrobePort(Rc * my, int port);
static void Rc_setDecision(Rc * my);
static void Rc_clrDecision(Rc * my);
static int Rc_getDecision(Rc * my);
static int Rc_padDecision(Rc * my);
static void Rc_setArdout(Rc * my);
static void Rc_clrArdout(Rc * my);
static int Rc_getArdout(Rc * my);
static int Rc_padArdout(Rc * my);
static void Rc_setPrdout(Rc * my);
static void Rc_clrPrdout(Rc * my);
static int Rc_getPrdout(Rc * my);
static int Rc_padPrdout(Rc * my);
static void Rc_setBegrun(Rc * my);
static void Rc_clrBegrun(Rc * my);
static int Rc_getBegrun(Rc * my);
static int Rc_padBegrun(Rc * my);
static void Rc_setTagclk(Rc * my);
static void Rc_clrTagclk(Rc * my);
static int Rc_getTagclk(Rc * my);
static int Rc_padTagclk(Rc * my);
static void Rc_setTagdata(Rc * my);
static void Rc_clrTagdata(Rc * my);
static int Rc_getTagdata(Rc * my);
static int Rc_padTagdata(Rc * my);
static void Rc_setTbsy(Rc * my);
static void Rc_clrTbsy(Rc * my);
static int Rc_getTbsy(Rc * my);
static int Rc_padTbsy(Rc * my);
static void Rc_setTrg(Rc * my);
static void Rc_clrTrg(Rc * my);
static int Rc_getTrg(Rc * my);
static int Rc_padTrg(Rc * my);
static void Rc_TriggerPort(Rc * my);
static LVme_W Rc_getTrgCtr(Rc * my);
static LVme_W Rc_getPrdoutCtr(Rc * my);
static LVme_W Rc_getArdoutCtr(Rc * my);
static LVme_W Rc_getAdelCtr(Rc * my);
static void Rc_setRctr(Rc * my);
static void Rc_clrRctr(Rc * my);
static void Rc_ClrCtrs(Rc * my);
static LVme_W Rc_getTagReg(Rc * my);
static LVme_W Rc_getLcReg(Rc * my);
static LVme_W Rc_getLcStatus(Rc * my);
static LVme_W Rc_getGiopReg(Rc * my);
static LVme_W Rc_getGiopPad(Rc * my);
static LVme_W Rc_getInterfaceVersion(Rc * my);
static LVme_W Rc_read_mStatusId(Rc * my);
static void Rc_write_mStatusId(Rc * my, LVme_W value);
static LVme_W Rc_read_mBugReg(Rc * my);
static LVme_W Rc_read_mMemReg(Rc * my);
static LVme_W Rc_read_mPages(Rc * my);
static LVme_W Rc_read_mSedec_high(Rc * my);
static void Rc_write_mSedec_high(Rc * my, LVme_W value);
static LVme_W Rc_read_mSedec_low(Rc * my);
static void Rc_write_mSedec_low(Rc * my, LVme_W value);
static LVme_W Rc_read_mSeid_high(Rc * my);
static void Rc_write_mSeid_high(Rc * my, LVme_W value);
static LVme_W Rc_read_mSeid_low(Rc * my);
static void Rc_write_mSeid_low(Rc * my, LVme_W value);
static LVme_W Rc_read_mRev(Rc * my);
static LVme_W Rc_read_mSwCtr(Rc * my);
static LVme_W Rc_read_mCodeMem(Rc * my);
static void Rc_write_(Rc * my, LVme_W value);
static unsigned int Rc_readStatusid(Rc * my);
static void Rc_writeStatusid(Rc * my, unsigned char value);
static LVme_W Rc_getStatusID(Rc * my);
static void Rc_setIrqR(Rc * my);
static void Rc_clrIrqR(Rc * my);
static int Rc_getIrqR(Rc * my);
static void Rc_setIrqS(Rc * my);
static void Rc_clrIrqS(Rc * my);
static int Rc_getIrqS(Rc * my);
static void Rc_IrqRon(Rc * my);
static void Rc_IrqRoff(Rc * my);
static void Rc_IrqSon(Rc * my);
static void Rc_IrqSoff(Rc * my);
static LVme_W Rc_getBug(Rc * my);
static LVme_W Rc_readMemreg(Rc * my);
static void Rc_writeMemreg(Rc * my, LVme_W value);
static void Rc_setMemfull(Rc * my, LVme_W pages);
static void Rc_waitSwitchcomplete(Rc * my);
static int Rc_SwitchStatus(Rc * my);
static int Rc_testMemfull(Rc * my);
static LVme_W Rc_readSedec_high(Rc * my);
static void Rc_writeSedec_high(Rc * my, LVme_W value);
static LVme_W Rc_readSedec_low(Rc * my);
static void Rc_writeSedec_low(Rc * my, LVme_W value);
static LVme_W Rc_readSeid_high(Rc * my);
static void Rc_writeSeid_high(Rc * my, LVme_W value);
static LVme_W Rc_readSeid_low(Rc * my);
static void Rc_writeSeid_low(Rc * my, LVme_W value);
static LVme_W Rc_readVersionMem(Rc * my);
static LVme_W Rc_readSwCtr(Rc * my);
static void Rc_HallOfMemory(Rc * my);
static unsigned short Rc_readDpr(Rc * my, unsigned long offsetValue);
static void Rc_writeDpr(Rc * my, unsigned long offsetValue,
                        unsigned short value);
static void Rc_clearDpr(Rc * my);
static void Rc_fillDpr(Rc * my);
static unsigned short Rc_testDpr(Rc * my);
static void Rc_writeDB(Rc * my, ptrdiff_t offset, unsigned short data);
static unsigned short Rc_readDB(Rc * my, ptrdiff_t offset);
#endif
#endif
