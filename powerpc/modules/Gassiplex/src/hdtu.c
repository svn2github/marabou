#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>
#include <errno.h>
#include <string.h> 

#include <xild.h>
#include <lvme.h>

#include "hdtu.h"

#define CTRL   0x00
#define STAT   0x04
#define DONE   0x08
#define TRAP   0x0c
#define TAG    0x10
#define OFF    0x14
#define RAMRD  0x18
#define RAMWR  0x1c
#define SM     0x20
#define CTR    0x24
#define TDL    0x28
#define DBG    0x2c
#define TRC    0x30
#define RDC    0x34
#define SWC    0x38
#define VER    0x3c

HDTU* newHDTU(const char* name, uint32_t vmeBase)
{
  HDTU* my = NULL;
  
  if ((my = (HDTU*)malloc(sizeof(HDTU))) == NULL) return NULL;
  if (name != NULL) {
    strncpy(my->name, name, 128);
  } else {
    strcpy(my->name, "UNKNOWN");
  }

  if ((my->lvme = newLVme(vmeBase, 64 * 1024, 0x09)) == NULL) {
    syslog(LOG_ERR, "%s: failed to create newLVme", my->name);
    return NULL;
  }
  
  return my;
}

void delHDTU(HDTU* my)
{
  if (my == NULL) return;
  delLVme(my->lvme);
  free(my->lvme);
  free(my);
}

int HDTU_init(HDTU* my, const char* fileName)
{
  FILE* file = NULL;
  int ret;
  
  if ((file = fopen(fileName, "r")) == NULL) {
    syslog(LOG_ERR, "%s: could not open firmwarefile %s: %s", 
            my->name, fileName, strerror(errno)); 
    return -1;
  }
  ret = ConfigXilinx("HDTU", my->lvme, 0xC004, 0, 2, 3, 1, 4, file);
  fclose(file);
  
  return ret;
}

void HDTU_arm(HDTU* my)
{
  LVme_getW(my->lvme, TRAP);
}

void HDTU_trigger(HDTU* my)
{
  LVme_setW(my->lvme, TRC, 0xff);
}

void HDTU_release(HDTU* my)
{
  LVme_getW(my->lvme, DONE);
}

void HDTU_enableUpperLemo(HDTU* my)
{
  LVme_setW(my->lvme, TRAP, 0x01);
}

void HDTU_enableLowerLemo(HDTU* my)
{
  LVme_setW(my->lvme, TRAP, 0x02);
}

void HDTU_setLemoLowActive(HDTU* my)
{
  LVme_setW(my->lvme, TRAP, 0x04);
}

void HDTU_setTestMode(HDTU* my)
{
  LVme_setW(my->lvme, TRAP, 0x10);
}

void HDTU_reset(HDTU* my)
{
  LVme_clrBitW(my->lvme, CTRL, 7);
  LVme_nop(my->lvme);
  LVme_setBitW(my->lvme, CTRL, 7);
  LVme_nop(my->lvme);
  LVme_clrBitW(my->lvme, CTRL, 7);
  LVme_nop(my->lvme);
}

int HDTU_waitData(HDTU* my)
{
  while((LVme_getW(my->lvme, STAT) & 0x01) == 0) {}
  return 1;
}

int HDTU_initDefaults(HDTU* my)
{
  /*  LVme_setBitW(my->lvme, CTRL, 0); */
  HDTU_enableUpperLemo(my);
 
  return 0;
}

void HDTU_status(HDTU* my)
{
  uint8_t ctrl;
  uint8_t stat;
  uint8_t dbg;
  uint8_t tag;
  uint8_t off;
  uint8_t ramrd;
  uint8_t ramwr;
  uint8_t sm;
  uint8_t ctr;
  uint8_t tdl;
  uint8_t trc;
  uint8_t rdc;
  uint8_t swc;
  uint8_t ver;
  
  ctrl = (uint8_t)LVme_getW(my->lvme, CTRL);
  stat = (uint8_t)LVme_getW(my->lvme, STAT);
  dbg = (uint8_t)LVme_getW(my->lvme, DBG);
  tag = (uint8_t)LVme_getW(my->lvme, TAG);
  off = (uint8_t)LVme_getW(my->lvme, OFF);
  ramrd = (uint8_t)LVme_getW(my->lvme, RAMRD);
  ramwr = (uint8_t)LVme_getW(my->lvme, RAMWR);
  sm = (uint8_t)LVme_getW(my->lvme, SM);
  ctr = (uint8_t)LVme_getW(my->lvme, CTR);
  tdl = (uint8_t)LVme_getW(my->lvme, TDL);
  trc = (uint8_t)LVme_getW(my->lvme, TRC);
  rdc = (uint8_t)LVme_getW(my->lvme, RDC);
  swc = (uint8_t)LVme_getW(my->lvme, SWC);
  ver = (uint8_t)LVme_getW(my->lvme, VER);
   
  /* CTRL */
  fprintf(stdout, "\nControl Register (CTRL):\n");
  fprintf(stdout, "========================\n");
  fprintf(stdout, "Value: 0x%02x\n", ctrl);
  fprintf(stdout, "  Operation (RST)                            :  %s\n", 
          ((ctrl & 0x80) == 0 ? "normal" : "reset"));
  fprintf(stdout, "  Lemo Out Signal (GIO)                      :  %s\n", 
          ((ctrl & 0x40) == 0 ? "low" : "high"));
  fprintf(stdout, "  Test (TST)                                 :  %s\n", 
          ((ctrl & 0x10) == 0 ? "normal" : "\033[1;34mtest mode\033[0m"));
  fprintf(stdout, "  Lemo In (POL)                              :  %s\n", 
          ((ctrl & 0x04) == 0 ? "high active" : "low active"));
  fprintf(stdout, "  Upper Lemo Input (EN1)                     :  %s\n", 
          ((ctrl & 0x01) == 0 ? "disabled" : "enabled"));
  fprintf(stdout, "  Lower Lemo Input (EN2)                     :  %s\n", 
          ((ctrl & 0x02) == 0 ? "disabled" : "enabled")); 
  
  /* STAT */
  fprintf(stdout, "\nStatus Register (STAT):\n");
  fprintf(stdout, "=======================\n");
  fprintf(stdout, "Value: 0x%02x\n", stat);
  fprintf(stdout, "  Error Status (ERR)                         :  %s\n", 
          ((stat & 0x80) == 0 ? "none" : "\033[1;31merror\033[0m"));
  fprintf(stdout, "  RC Error Status (NRE)                      :  %s\n", 
          ((stat & 0x40) == 0 ? "\033[1;31merror\033[0m" : "none"));
  fprintf(stdout, "  RC Busy (NRB)                              :  %s\n", 
          ((stat & 0x20) == 0 ? "\033[1;34mbusy\033[0m" : "not busy"));
  fprintf(stdout, "  FE Busy (NTB)                              :  %s\n", 
          ((stat & 0x10) == 0 ? "\033[1;34mbusy\033[0m" : "not busy"));
  fprintf(stdout, "  Error Readout Ctr (RCE)                    :  %s\n", 
          ((stat & 0x08) == 0 ? "none" : "\033[1;31merror\033[0m"));
  fprintf(stdout, "  Error Trigger Ctr (TCE)                    :  %s\n", 
          ((stat & 0x04) == 0 ? "none" : "\033[1;31merror\033[0m"));
  fprintf(stdout, "  Trigger Status (TRP)                       :  %s\n", 
          ((stat & 0x02) == 0 ? "\033[1;34mdeny\033[0m" : "accept"));
  fprintf(stdout, "  Data Status (INT)                          :  %s\n", 
          ((stat & 0x01) == 0 ? 
           "no data" : "\033[1;34mdata available\033[0m"));

  /* TRAP */
  fprintf(stdout, "\nTRAP Register (DBG):\n");
  fprintf(stdout, "====================\n");
  fprintf(stdout, "Value: 0x%02x\n", dbg);
  fprintf(stdout, "  Number of Reads (RD[2:0])                  :  %d\n", 
          (dbg >> 4) & 0x07); 
  fprintf(stdout, "  Number of Bad Triggers (BT[2:0])           :  %s"
          "%d\033[0m\n", (dbg & 0x07) > 0 ? "\033[1;34m" : "", (dbg & 0x07)); 
  fprintf(stdout, "  Overflow BadTrigger Counter (OVF)          :  %s\n", 
          ((dbg & 0x08) == 0 ? "off" : "\033[1;34mon\033[0m"));
  
 
  fprintf(stdout, "\nCounter Registers:\n");
  fprintf(stdout, "==================\n");
  /* TAG */
  fprintf(stdout, "  Next Tag to be sent to FEs (TAG)           :  0x%02x\n", 
          tag);

  /* OFF */
  fprintf(stdout, "  Offset inside SRAM Block ro r/w (OFF)      :  0x%02x\n", 
          off);

  /* RAMRD */
  fprintf(stdout, "  Number of read Data from SRAM (RAMRD)      :  0x%02x\n", 
          ramrd);
  
  /* RAMWR */
  fprintf(stdout, "  Number of written Data to SRAM RAMWR()     :  0x%02x\n", 
          ramwr);
  
  /* TRC */
  fprintf(stdout, "  Number of Triggers sent to FEs (TRC)       :  0x%02x\n", 
          trc);
  
  /* RDC */
  fprintf(stdout, "  Number of Triggers sent to RCs (RDC)       :  0x%02x\n", 
          rdc);
  /* SWC */
  fprintf(stdout, "  Number of SwitchRequest sent to RCs (SWC)  :  0x%02x\n", 
          swc);
  

  /* All Registers */
  fprintf(stdout, "\nOverview of All Registers:\n");
  fprintf(stdout, "=======================\n");
  fprintf(stdout, "  CTRL: 0x%02x\n", ctrl);
  fprintf(stdout, "  STAT: 0x%02x\n", stat);
  fprintf(stdout, "  DBG:  0x%02x\n", dbg);
  fprintf(stdout, "  TAG:  0x%02x\n", tag);
  fprintf(stdout, "  SM:   0x%02x\n", sm);
  fprintf(stdout, "  CTR:  0x%02x\n", ctr);
  fprintf(stdout, "  TDL:  0x%02x\n", tdl);
  fprintf(stdout, "  TRC:  0x%02x\n", trc);
  fprintf(stdout, "  RDC:  0x%02x\n", rdc);
  fprintf(stdout, "  SWC:  0x%02x\n", swc);
  fprintf(stdout, "  VER:  0x%02x\n\n", ver);

}

