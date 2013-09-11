#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#include <lvme.h>

#include "caen_v977.h"


#define OUTSET   0x0a
#define READ     0x04
#define CLEAR    0x10
#define SGLRCL   0x16
#define MLTRCL   0x18
#define CTRL     0x28
#define RESET    0x2e

CAEN_V977* newV977(const char* name, uint32_t vmeBase){
  CAEN_V977*my=NULL;

  if((my = (CAEN_V977*)malloc(sizeof(CAEN_V977))) == NULL) return NULL;
  if(name!=NULL){
    strncpy(my->name, name, 128);
  }else{
    strcpy(my->name, "UNKNOWN");
  }
 
  if((my->lvme = (LVme*)malloc(sizeof(LVme))) == NULL) return NULL;

  if(conLVme(my->lvme, vmeBase, 64 * 1024, 0x09, 0, 0) != 0) {
    syslog(LOG_ERR, "%s: conLVme failed", my->name);
    return NULL;
  }

  return my;
}

void delV977(CAEN_V977*my){
  if (my == NULL) return;
  desLVme(my->lvme);
  free(my->lvme);
  free(my);
}

void V977_reset(CAEN_V977* my){
  LVme_setW(my->lvme, RESET, 0x1);
}

void V977_setmode(CAEN_V977* my, u_int16_t arg){
/*arg = 0x0 -> io-register, arg=0x1 -> pattern unit*/

  LVme_setW(my->lvme, CTRL, arg);
}

void V977_setoutput(CAEN_V977* my, u_int16_t arg){
  LVme_setW(my->lvme, OUTSET, arg);
}

void V977_clear(CAEN_V977* my){
  LVme_setW(my->lvme, CLEAR, 0x0);
}


u_int16_t V977_readclear(CAEN_V977* my){
  return LVme_getW(my->lvme,SGLRCL);
}

u_int16_t V977_multireadclear(CAEN_V977* my){
  return LVme_getW(my->lvme,MLTRCL);
}

u_int16_t V977_read(CAEN_V977* my){
  return LVme_getW(my->lvme,READ);
}
