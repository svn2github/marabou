#ifndef CAEN_V977_H
#define CAEN_V977_H

#include <lvme.h>


typedef struct{
  char name[128];
  LVme *lvme;
} CAEN_V977;

CAEN_V977* newV977(const char* name, uint32_t vmeBase);

void delV977(CAEN_V977* my);

void V977_reset(CAEN_V977* my);

void V977_setmode(CAEN_V977* my, u_int16_t arg);

void V977_setoutput(CAEN_V977* my, u_int16_t arg);

void V977_clear(CAEN_V977* my);

u_int16_t V977_readclear(CAEN_V977* my);

u_int16_t V977_multireadclear(CAEN_V977* my);

u_int16_t V977_read(CAEN_V977* my);


#endif
