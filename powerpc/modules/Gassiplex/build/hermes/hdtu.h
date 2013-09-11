#ifndef HDTU_H
#define HDTU_H

#include <lvme.h>

typedef struct {
  char name[128];
  LVme *lvme;
} HDTU;

HDTU* newHDTU(const char* name, uint32_t vmeBase);

void delHDTU(HDTU* my);

void HDTU_status(HDTU* my);

void HDTU_reset(HDTU* my);

int HDTU_init(HDTU* my, const char* fileName);

int HDTU_initDefaults(HDTU* my);

int HDTU_waitData(HDTU* my);

void HDTU_arm(HDTU* my);

void HDTU_trigger(HDTU* my);

void HDTU_release(HDTU* my);

void HDTU_enableUpperLemo(HDTU* my);

void HDTU_enableLowerLemo(HDTU* my);

void HDTU_setLemoLowActive(HDTU* my);

void HDTU_setTestMode(HDTU* my);

#endif
