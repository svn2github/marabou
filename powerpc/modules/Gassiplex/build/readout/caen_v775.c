#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <lvme.h>

#include "caen_v775.h"

#define v775_OUTPUT    0x0000
#define v775_BITSET1   0x1006
#define v775_BITCLR1   0x1008
#define v775_STAT1     0x100e
#define v775_CTRL1     0x1010
#define v775_STAT2     0x1022
#define v775_ECTR_L    0x1024
#define v775_ECTR_H    0x1026
#define v775_INCR_E    0x1028
#define v775_INCR_O    0x102a
#define v775_FCLRWIN   0x102e
#define v775_BITSET2   0x1032
#define v775_BITCLR2   0x1034
#define v775_ECTRRES   0x1040
#define v775_FSCL      0x1060
#define v775_THRESH    0x1080


CAEN_V775* newV775(const char* name, uint32_t vmeBase){
  CAEN_V775*my=NULL;
  if((my = (CAEN_V775*)malloc(sizeof(CAEN_V775))) == NULL) return NULL;
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


void delV775(CAEN_V775*my){
  if (my == NULL) return;
  desLVme(my->lvme);
  free(my->lvme);
  free(my);
}

void V775_reset(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITSET1, 0x80);
  usleep(1000);
  LVme_setW(my->lvme, v775_BITCLR1, 0x80);
}

void v775_cleardata(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITSET2, 0x04);
  usleep(1000);
  LVme_setW(my->lvme, v775_BITCLR2, 0x04);

}

void v775_setemptyprog(CAEN_V775*my, int arg){
  /*arg = 0 -> disable writing header and trailer always to buffer, arg = 1 -> enable */
  if(arg==0){
    LVme_setW(my->lvme, v775_BITCLR2, 0x1000);
  }else if(arg==1){
    LVme_setW(my->lvme, v775_BITSET2, 0x1000);
  }else{
    printf("ERROR: arg must be = 0 or =1\n");
    return;
  }
}

void v775_setfullrange(CAEN_V775*my, int arg){
  /*arg=0xff->35ps, 0x1e->300ps, stronly non linear!*/
  LVme_setW(my->lvme, v775_FSCL, arg);
}

void v775_setautoincrem(CAEN_V775*my, int arg){
  /*arg = 0 -> disable, arg = 1 -> enable */
  if(arg==0){
    LVme_setW(my->lvme, v775_BITCLR2, 0x800);
  }else if(arg==1){
    LVme_setW(my->lvme, v775_BITSET2, 0x800);
  }else{
    printf("ERROR: arg must be = 0 or =1\n");
    return;
  }
}

void v775_enablecomstart(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITCLR2, 0x400);
}

void v775_enablecomstop(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITSET2, 0x400);
}

void v775_gooffline(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITSET2, 0x02);
}

void v775_goonline(CAEN_V775*my){
  LVme_setW(my->lvme, v775_BITCLR2, 0x02);
}

u_int32_t v775_getdata(CAEN_V775*my){
  return LVme_getL(my->lvme, v775_OUTPUT);
}

/*int v775_getdatablock(CAEN_V775*my, u_int32_t*data){
  if(data==NULL) return -1;
  data = LVme_getPtrL(my->lvme, v775_OUTPUT);
  if(data==NULL) return -2;
  return 0;
}
*/
