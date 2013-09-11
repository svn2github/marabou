#ifndef CAEN_V775_H
#define CAEN_V775_H

#include <lvme.h>


typedef struct{
  char name[128];
  LVme *lvme;
} CAEN_V775;

CAEN_V775* newV775(const char* name, uint32_t vmeBase);

void delV775(CAEN_V775*my);

void V775_reset(CAEN_V775*my);

void v775_cleardata(CAEN_V775*my);

void v775_setfullrange(CAEN_V775*my, int arg);

void v775_setemptyprog(CAEN_V775*my, int arg);

void v775_setautoincrem(CAEN_V775*my, int arg);

void v775_enablecomstart(CAEN_V775*my);

void v775_enablecomstop(CAEN_V775*my);

void v775_gooffline(CAEN_V775*my);

void v775_goonline(CAEN_V775*my);

u_int32_t v775_getdata(CAEN_V775*my);



#endif
