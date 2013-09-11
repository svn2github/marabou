#ifndef HRACE_H
#define HRACE_H

#include <stdint.h>

#include <lvme.h>
#include <lbma.h>

typedef struct HRaceS {
  uint32_t vmeBase;
  LVme* regRegion;
  LVme* memRegion;
  LBma* bma;
  uint16_t currBank;
  uint32_t currAddr;
  uint32_t lastAddr;
} HRace;

HRace* newHRace(uint32_t vmeBase, int bma);

void delHRace(HRace* my);

unsigned int HRace_readSubEvt(HRace* my, uint32_t* data);

#endif
