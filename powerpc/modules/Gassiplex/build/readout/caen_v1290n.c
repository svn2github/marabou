#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <lvme.h>

#include "caen_v1290n.h"

#define v1290_CTRL    0x1000
#define v1290_STAT    0x1002
#define v1290_RESET   0x1014
#define v1290_CLEAR   0x1016
#define v1290_EVRST   0x1018
#define v1290_MICRO   0x102e
#define v1290_MCRHS   0x1030

#define opcode_TRG_MATCH      0x0000
#define opcode_READ_ACQ_MOD   0x0200
#define opcode_SET_WIN_WIDTH  0x1000
#define opcode_SET_WIN_OFFS   0x1100
#define opcode_SET_SW_MARGIN  0x1200
#define opcode_SET_REJ_MARGIN 0x1300
#define opcode_READ_TRG_CONF  0x1600

#define opcode_SET_RESOLUT    0x2400

#define v1290_mask_WRITE_OK         0x01
#define v1290_mask_READ_OK          0x02
#define v1290_mask_DATA_RDY         0x01


CAEN_V1290N* newV1290N(const char* name, uint32_t vmeBase){
  CAEN_V1290N*my=NULL;

  if((my = (CAEN_V1290N*)malloc(sizeof(CAEN_V1290N))) == NULL) return NULL;
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

void delV1290N(CAEN_V1290N*my){
  if (my == NULL) return;
  desLVme(my->lvme);
  free(my->lvme);
  free(my);
}

void V1290N_reset(CAEN_V1290N*my){
  printf("resetting v1290n ...\n");
  LVme_setW(my->lvme, v1290_RESET, 0x0);
  printf("... done\n");
}



void V1290N_enab_trigmatch(CAEN_V1290N*my){
  int i=0;

  printf("trying to enable trigger matching mode\n");

  for(i=0;i<1000;i++){
    if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
      printf("WRITE_OK=1\n");
      LVme_setW(my->lvme, v1290_MICRO, opcode_TRG_MATCH);
      printf("enabling trigger matching mode\n");
      return;
    }else{
      usleep(1000);
    }
  }
  
  printf("ERROR: waited for 2 sec, WRITE_OK!=1\n");
}

void V1290N_setwindoww(CAEN_V1290N*my, u_int16_t arg){
  int i=0;
  int j=0;

  printf("trying to set window width to %i ns\n",arg*25);

  for(i=0;i<1000;i++){
    if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
      printf("WRITE_OK=1\n");
      LVme_setW(my->lvme, v1290_MICRO, opcode_SET_WIN_WIDTH);
      for(j=0;j<1000;j++){
	if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
	  printf("WRITE_OK=1\n");
	  LVme_setW(my->lvme, v1290_MICRO, arg);
	  return;
	}else{
	  usleep(1000);
	}
      }
      printf("ERROR: waited for 2 sec, 2nd WRITE_OK!=1\n");

      return;
    }else{
      usleep(1000);
    }
  }
  printf("ERROR: waited for 2 sec, WRITE_OK!=1\n");
}

void V1290N_setwindowoff(CAEN_V1290N*my, u_int16_t arg){
  int i=0;
  int j=0;
  int offset;
  
  if(arg>32768){
    offset=(arg-65536)*25;
  }else{
    offset=arg*25;
  }
 
  printf("trying to set window offset to %i ns\n",offset);
  
  for(i=0;i<1000;i++){
    if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
      printf("WRITE_OK=1\n");
      LVme_setW(my->lvme, v1290_MICRO, opcode_SET_WIN_OFFS);
      for(j=0;j<1000;j++){
	if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
	  printf("WRITE_OK=1\n");
	  LVme_setW(my->lvme, v1290_MICRO, arg);
	  return;
	}else{
	  usleep(1000);
	}
      }
      printf("ERROR: waited for 2 sec, 2nd WRITE_OK!=1\n");

      return;
    }else{
      usleep(1000);
    }
  }
  printf("ERROR: waited for 2 sec, WRITE_OK!=1\n");

}

void V1290N_setresolution(CAEN_V1290N*my, u_int16_t arg){
  /*arg = 0x00 ->800ps, 0x01->200ps, 0x02->100ps, 0x03->25ps*/
  int i=0;
  int j=0;

  for(i=0;i<1000;i++){
    if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
      printf("WRITE_OK=1\n");
      LVme_setW(my->lvme, v1290_MICRO, opcode_SET_RESOLUT);
      for(j=0;j<1000;j++){
	if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
	  printf("WRITE_OK=1\n");
	  LVme_setW(my->lvme, v1290_MICRO, arg);
	  return;
	}else{
	  usleep(1000);
	}
      }
      printf("ERROR: waited for 2 sec, 2nd WRITE_OK!=1\n");

      return;
    }else{
      usleep(1000);
    }
  }
  printf("ERROR: waited for 2 sec, WRITE_OK!=1\n");

}

int V1290N_readdatardy(CAEN_V1290N*my){
  if((LVme_getW(my->lvme,v1290_STAT)&v1290_mask_DATA_RDY) == v1290_mask_DATA_RDY) return 1;
  else return 0;
}

int V1290N_readstatus(CAEN_V1290N*my){
  return LVme_getW(my->lvme,v1290_STAT);
}


int V1290N_readmode(CAEN_V1290N*my){
  int i=0;
  int j=0;
  
  for(j=0;j<1000;j++){
    if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_WRITE_OK){
      printf("WRITE_OK=1\n");
      LVme_setW(my->lvme, v1290_MICRO, opcode_READ_ACQ_MOD);
      for(i=0;i<1000;i++){
	fprintf(stderr, "status is %i\n",LVme_getW(my->lvme,v1290_MCRHS));

	if(LVme_getW(my->lvme,v1290_MCRHS)==v1290_mask_READ_OK){
	  printf("READ_OK=1\n");

	  return LVme_getW(my->lvme, v1290_MICRO);
	}else{
	  usleep(100000);
	}
      }
      printf("ERROR: waited for 2 sec, READ_OK!=1\n");
      return -1;
    }else{
      usleep(1000);
    }
  }
  printf("ERROR: waited for 2 sec, WRITE_OK!=1\n");
  return -1;
}

u_int32_t V1290N_readdata(CAEN_V1290N*my, u_int16_t adr){
  return LVme_getL(my->lvme,adr);
}
