#ifndef CAEN_V1290N_H
#define CAEN_V1290N_H

#include <lvme.h>


typedef struct{
  char name[128];
  LVme *lvme;
} CAEN_V1290N;

CAEN_V1290N* newV1290N(const char* name, uint32_t vmeBase);

void delV1290N(CAEN_V1290N*my);

void V1290N_reset(CAEN_V1290N*my);

void V1290N_enab_trigmatch(CAEN_V1290N*my);

void V1290N_setwindoww(CAEN_V1290N*my, u_int16_t arg);

void V1290N_setwindowoff(CAEN_V1290N*my, u_int16_t arg);

void V1290N_setresolution(CAEN_V1290N*my, u_int16_t arg);

int V1290N_readdatardy(CAEN_V1290N*my);

int V1290N_readstatus(CAEN_V1290N*my);

int V1290N_readmode(CAEN_V1290N*my);

u_int32_t V1290N_readdata(CAEN_V1290N*my, u_int16_t adr);



#endif
