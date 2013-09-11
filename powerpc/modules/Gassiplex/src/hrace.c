#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <xild.h>

#include "hrace.h"
#define HRACE_MEMREGION 0x100000
#define HRACE_MEMSIZE   0x100000
#define HRACE_REGREGION 0x300000

#define HRACE_PAGEREG 0x0106
#define HRACE_NODATASIZE 0x10
#define RCPAGESIZE 0x100

HRace* newHRace(uint32_t vmeBase, int bma)
{
  HRace* race = NULL;
  
  if ((race = malloc(sizeof(HRace))) == NULL) abort();
  race->memRegion = NULL;
  race->bma = NULL;
  
  if ((race->regRegion = (LVme*)malloc(sizeof(LVme))) == NULL) abort();
  race->regRegion = newLVme(vmeBase + HRACE_REGREGION, 0x10000, 0x09);
  
  if ((race->memRegion = malloc(sizeof(LVme))) == NULL) abort();
  race->memRegion = newLVme(vmeBase + HRACE_MEMREGION, HRACE_MEMSIZE, 0x09);

  if (bma != 0) { 
    /*if ((race->bma = newLBma(HRACE_MEMSIZE)) == NULL) abort();*/
    fprintf(stderr,"BMA is not yet implemented with the new LVME library\n");
    return NULL;
  }
  
  race->vmeBase = vmeBase;
  race->currBank = 0xffff;
  race->currAddr = 0;
  race->lastAddr = 0;
  
  return race;
}

void delHRace(HRace* my)
{
  if (my->regRegion != NULL) delLVme(my->regRegion);
  if (my->memRegion != NULL) delLVme(my->memRegion);
  if (my->bma != NULL) delLBma(my->bma);
  free(my->regRegion);
  free(my->memRegion);
  free(my);
}

unsigned int HRace_readSubEvt(HRace* my, uint32_t* data)
{
  unsigned int subEvtId = 0;
  uint16_t pageReg;
  uint32_t size;
  uint32_t totalSize;
  uint32_t* start;

  start = data;
    
  pageReg = LVme_getW(my->regRegion, HRACE_PAGEREG);
  /* check Bank switched status */
  if (!(((pageReg & 0xc000) == 0xc000) || ((pageReg & 0xc000) == 0))) {
    fprintf(stderr, "Error pageReg content: 0x%04x\n", pageReg);
    return 0;
  }
  if ((my->currBank != 0xffff) && (my->currBank == (pageReg & 0xc000))) {
    fprintf(stderr, "Error pageReg swap: cur: 0x%04x  page: 0x%04x\n",
            my->currBank, pageReg);
    return 0; 
  }
  my->currBank = pageReg & 0xc000;
  totalSize = (pageReg & 0x07ff) * RCPAGESIZE;

  /*fprintf(stderr, "totalSize = %lu\n",(unsigned long)totalSize);*/
  
  my->currAddr = 0;
  my->lastAddr = 0;  
  
  if (my->bma == NULL) {
    while(my->currAddr < totalSize) {
      /*fprintf(stderr, "SubEvt# = %d\n",subEvtId);*/

      size = LVme_getL(my->memRegion, my->currAddr);
      /*fprintf(stderr, "size = %lu\n",(unsigned long)size);*/

      my->lastAddr = my->currAddr + size;
      
      /* copy one sub evt from RC to memory */
      while (my->currAddr < my->lastAddr) {
        *data++ = LVme_getL(my->memRegion, my->currAddr);
        my->currAddr += 4;
      }
    
      /* fprintf(stderr, "read SubEvt# %d", subEvtId); */
    
      /* align currAddr to next page */
      my->currAddr = (my->lastAddr + (RCPAGESIZE - 1)) & ~(RCPAGESIZE - 1);
      subEvtId++;
    }
  } else {
    /* BLT */
    fprintf(stderr,"BMA is not yet implemented with the new LVME library\n");
    /*
    ptrdiff_t bmaOffset = 0;
    if (LBma_resetChain(my->bma) < 0) {
      fprintf(stderr, "LBma_resetChain failed.");
      return 0;
      }*/
    /*while(my->currAddr < totalSize) {*/
      /* syslo(LOG_DEBUG, "Start reading SubEvt# %d", subEvtId);*/
      /*fprintf(stderr, "SubEvt# = %d\n",subEvtId);*/

    /*      size = LVme_getL(my->memRegion, my->currAddr);*/
      /*fprintf(stderr, "size = %lu\n",(unsigned long)size);*/

    /* my->lastAddr = my->currAddr + size;*/
      
      /* Add subevent to bma-chain */
    /* if (LBma_readChain(my->bma,
                         bmaOffset, 
                         my->vmeBase + HRACE_MEMREGION + my->currAddr, 
                         size / 4, 
                         0x0b) < 0) {
        fprintf(stderr, "LBma_readChain Event failed");
        return 0;
	}*/
      /* align currAddr to next page */
      /*my->currAddr = (my->lastAddr + (RCPAGESIZE - 1)) & ~(RCPAGESIZE - 1);
      bmaOffset += size;
    }
      */
    /* submit BMA-chain */
    /* fprintf(stderr, "start chain chain\n");*/
    /*  if (LBma_startChain(my->bma) < 0) {
      fprintf(stderr, "LBma_startChain failed");
      return 0;
      }*/
    /* fprintf(stderr, "wait chain\n");*/
    /*if (LBma_waitChain(my->bma) < 0) {
      fprintf(stderr, "LBma_waitChain failed");
      return 0;
      }*/
    /*fprintf(stderr, "chain done\n");*/
    
    /* read data from virtual address space into data */
    /*  my->currAddr = 0;
    my->lastAddr = 0; 
    subEvtId=0;

    while(my->currAddr < totalSize) {*/
      /* syslog(LOG_DEBUG, "Start reading SubEvt# %d", subEvtId);*/
      /* fprintf(stderr, "SubEvt# = %d\n",subEvtId);*/
      
    /*      size = LVme_getL(my->memRegion, my->currAddr);*/
      /*  fprintf(stderr, "size = %lu\n",(unsigned long)size);*/
      /*
      my->lastAddr = my->currAddr + size;
      */
      /* copy one sub evt from RC to memory */
      /*while (my->currAddr < my->lastAddr) {
        *data++ = *(LBma_getPtrL(my->bma,my->currAddr));
        my->currAddr += 4;
      }
      */
      /* fprintf(stderr, "read SubEvt# %d", subEvtId); */
      
      /* align currAddr to next page */
    /* my->currAddr = (my->lastAddr + (RCPAGESIZE - 1)) & ~(RCPAGESIZE - 1);
      subEvtId++;
      }*/
   

  }
  
  return (data - start);
}
