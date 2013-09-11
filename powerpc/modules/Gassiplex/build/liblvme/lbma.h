#ifndef LBMA_H
#define LBMA_H

#ifdef __cplusplus
extern "C"
{
#endif
  
#include <stddef.h>
#include <stdint.h>
#include <ces/uiolib.h>
#include <ces/bmalib.h>
  
  typedef enum LBMA_ADDR_MOD {
    LBMA_A24 = BMA_M_AmA24U,
    LBMA_A32 = BMA_M_AmA32U
  } LBMA_ADDR_MOD;
  
  typedef enum LBMA_DTYPE {
    LBMA_D32 = 0,
    LBMA_D64 = 1
  } LBMA_DTYPE;
   
  typedef struct LBmaS {
    uint8_t* virtBase;
    uint32_t vmeAddr;
    LBMA_ADDR_MOD addrMod;
    LBMA_DTYPE dtype;
    uint32_t size;    
    uint32_t xvme;
    uio_mem_t buf_dsc;
    uint32_t* data;
  } LBma;

 
  
  /******************************************************************
   * NAME
   *        lbma.h - Portable access to VME bus in block mode access
   *
   * SYNOPSIS
   ******************************************************************/
  
  LBma* newLBma(size_t size, uint32_t vmeAddr,
                LBMA_ADDR_MOD addrMod, LBMA_DTYPE dtype, int fifoMode);
  void delLBma(LBma *my);
  int LBma_read(LBma *my, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
