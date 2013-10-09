#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#include <assert.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include <ces/vmelib.h>
#include <ces/vmecmd.h>
#include <ces/bmalib.h>
#include <sys/types.h>
#include <ces/uiolib.h>
#include <ces/absolute.h>
#ifndef CPU_TYPE_RIO2
#include <ces/buslib.h>
#endif

#include "xvmelib.h"
#include "lvme.h"
#include "lbma.h"
#include "lint.h"


/* --- Normal contructors --- */

LVme *newLVme(uint32_t vmeAddr, uint32_t size, LVME_ADDR_MOD addrMod)
{
  LVme* my = NULL;
  int busFd = -1;               /* File descriptor returned by bus_open */
  uint32_t cpuBus = 0;          /* Physical address on CPU bus          */
  uint32_t vAddr = 0;           /* Virtual address                      */
  int mode = -1;                /* Access mode                          */


  if (vmeAddr % (64 * 1024) != 0) {
    fprintf(stderr, "newLVme: vmeAddr % (64 * 1024) != 0\n");
    return NULL;
  }
  
  /* Translate VME-Address-Modifier to Access-Mode:
     VME-Code:      bus_map-Mode
     ================================
     0x09      -->  0xa0      A32
     0x39      -->  0x80      A24
     0x29      -->  0x20      A16
  */
  
  /* Translate VME-ADDR-Modifier */
  switch (addrMod) {
  case LVME_A32:
    mode = 0xa0;
    break;
  case LVME_A24:
    mode = 0x80;
    break;
  case LVME_A16:
    mode = 0x20;
    break;
  default:
    fprintf(stderr, "newLVme: problem in switch statement\n");

    return NULL;
  }

  if ((my = malloc(sizeof(LVme))) == NULL) {
    fprintf(stderr, "newLVme: couldnot allocate LVme\n");

    return NULL;
  }
    
  my->virtBase = NULL;
  my->vmeAddr = 0;
  my->addrMod = 0;
  my->size = 0;
  my->mapped = 0;
  
  /* Map VME-Bus using new buslib.h */
  if ((busFd = bus_open("xvme_mas")) != -1) {
    cpuBus = bus_map(busFd, 
                     vmeAddr,   /* 32bit physical address on I/O bus */
                     0,         /* 64bit physical address on I/O bus */
                     size,      /* Window size                       */
                     mode,      /* Access Mode, see above            */
                     0);        /* 0 = shared, 1 = private access    */
    
    if (cpuBus == (uint32_t) - 1) {
      
    } else {
      if ((vAddr = bus_kernel_remap(busFd, cpuBus)) == (uint32_t) - 1) {
        /* We have to call mmap */
        int md = -1;
        if ((md = open("/dev/mem", O_RDWR)) == -1) {
        } else {
          
#ifdef RIO4
          my->virtBase = 
            (uint8_t *)mmap(0,
                            size,
                            PROT_READ | PROT_WRITE | PROT_UNCACHE, MAP_SHARED,
                            md,
                            cpuBus);
#else
          my->virtBase = 
            (uint8_t *)mmap(0,
                            size, 
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            md,
                            cpuBus);
#endif
          
          if ((void *)my->virtBase == MAP_FAILED) {
            my->virtBase = NULL;
          } else {
            my->mapped = 1;
          }
        }
        
        close(md);
      } else {
        /* No mmap needed */
        my->virtBase = (void *)vAddr;
        my->mapped = 0;
      }
    }
    
    bus_close(busFd);
  }
  /* Finish LVme object */
  my->vmeAddr = vmeAddr;
  my->addrMod = addrMod;
  my->size = size;
  
  return my;
}

/* --- LVme MBS contructors --- */

LVme* newLVme_MBS(uint32_t vmeAddr, uint32_t size,
                  LVME_ADDR_MOD addrMod, void* pl_loc_hwacc)
{
  LVme *my = NULL;
  
  if (vmeAddr % (64 * 1024) != 0) {
    return NULL;
  }
  
  if ((my = malloc(sizeof(LVme))) == NULL) {
    return NULL;
  }
  
  my->virtBase = pl_loc_hwacc + vmeAddr;
  my->vmeAddr = vmeAddr;
  my->addrMod = addrMod;
  my->size = size;
  my->mapped = 0;
  
  return my;
}

void delLVme(LVme* my)
{
  if (my == NULL) {
    return;
  }

  if (my->mapped == 1) {
    munmap((void *)my->virtBase, my->size);
  }

  free(my);
  my = NULL;
}

/* ------ VME-BMA Routines ---------------------------------------------- */

LBma* newLBma(size_t size, uint32_t vmeAddr,
              LBMA_ADDR_MOD addrMod, LBMA_DTYPE dtype, int fifoMode)
{
  LBma* my = NULL;
  uint32_t vz = 6; /* limit VME block size according to <VOPS>:
                      6: default (32 words) 7: max (8062) */
  int status;

  if (!((addrMod == LBMA_A24) || 
        (addrMod == LBMA_A32))) {
    return NULL;
  }
  if (!((dtype == LBMA_D32) || (dtype == LBMA_D64))) {
    return NULL;
  }
  if ((my = malloc(sizeof(LBma))) == NULL) {
    return NULL;
  }
  
  /* Init LBma */
  my->virtBase = NULL;
  my->vmeAddr = 0;
  my->addrMod = 0;
  my->dtype = 0;
  my->xvme = 0;
  my->size = 0;
  my->data = NULL;

  /*
   * connect to BMA and User I/O drivers
   */
  if ((status = bma_open())) {
    bma_perror("newLBma: bma_open", status);
    free(my);
    return NULL;
  }
  
  /*
   * allocate contiguous memory, initialize it
   */
  if ((status = uio_calloc(&my->buf_dsc, size * sizeof(uint32_t)))) {
    uio_perror("newLBma: uio_calloc", status);
    delLBma(my);
    return NULL;
  }
  my->size = size;
  
  /*
   * configure AM code
   */
  if ((status = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_AmCode, addrMod))) {
    bma_perror("newLBma: bma_set_mode AM code", status);
    delLBma(my);
    return NULL;
  }
  
  /*
   * configure VME block size
   */
  if ((status = bma_set_mode(BMA_DEFAULT_MODE, BMA_M_VMESize, vz))) {
    bma_perror("newLBma: bma_set_mode VME block size", status);
    delLBma(my);
    return NULL;
  }
  
  /*
   * configure word size 
   */
  if ((status = bma_set_mode(BMA_DEFAULT_MODE,
                             BMA_M_WordSize,
                             (dtype == LBMA_D64 ? BMA_M_WzD64 : BMA_M_WzD32)
                             ))) {
    bma_perror("newLBma: bma_set_mode Word Size", status);
    delLBma(my);
    return NULL;
  }
  
  /*
   * configure 'fifo' mode
   */
#ifndef CPU_TYPE_RIO2
  if ((status = bma_set_mode(BMA_DEFAULT_MODE,
                             BMA_M_VMEAdrInc,
                             (fifoMode == 1 ? BMA_M_VaiFifo : BMA_M_VaiNormal)
                             ))) {
    bma_perror("newLBma: bma_set_mode FIFO mode", status);
    delLBma(my);
    return NULL;
  }
#endif
  
  /*
   * map XVME page for RIO3
   */
  if ((my->xvme = xvme_map(vmeAddr, size, addrMod, dtype)) == -1) {
    fprintf(stderr, "newLBma: cannot map XVME page\n");
    delLBma(my);
    return NULL;
  }

  my->virtBase = (uint8_t*)my->buf_dsc.uaddr;
  my->data = (uint32_t*)my->buf_dsc.uaddr;
  my->vmeAddr = vmeAddr;
  my->addrMod = addrMod;
  my->dtype = dtype;

  return my;
}

void delLBma(LBma *my)
{
  if (my == NULL) {
    return;
  }
  
  /* Free BMA Mapping */
  if (my->xvme != 0) {
    xvme_rel(my->xvme, my->size);
  }
  if (my->size > 0) {
    uio_cfree(&my->buf_dsc); 
  }
  bma_close();
  free(my);
}

int LBma_read(LBma *my, uint32_t size)
{
  int status = 0;
  
  if (size > my->size) {
    return -1;
  }
  status = bma_read_count(my->xvme,
                          bma_mem2loc(my->buf_dsc.paddr),
                          size,
                          BMA_DEFAULT_MODE);
  if (status <= 0 ) {
    return -1;
  }
  
  return size;
}

/* ----- VME-Interrupt Functions ---------------------------------------- */

LInt *newLInt(int vector)
{
  LInt *my = NULL;
  vme_int_cmd_t intCmd;

  if ((my = malloc(sizeof(LInt))) == NULL) {
    return NULL;
  }
  
  my->fd = open("/dev/vme", O_RDWR);
  if (my->fd == -1) {
    delLInt(my);
    return NULL;
  }

  my->vector = vector;
  intCmd.vector = my->vector;
  if (ioctl(my->fd, VME_INT_LINK, &intCmd) == -1) {
    delLInt(my);
    return NULL;
  }
  
  return my;
}

void delLInt(LInt *my)
{
  vme_int_cmd_t intCmd;

  if (my == NULL) {
    return;
  }
  
  intCmd.vector = my->vector;
  ioctl(my->fd, VME_INT_ULNK, &intCmd);
  if (my->fd != -1) {
    close(my->fd);
  }  
  
  free(my);
  my = NULL;
}

int LInt_wait(LInt *my)
{
  vme_int_cmd_t intCmd;

  intCmd.vector = my->vector;
  return ioctl(my->fd, VME_INT_WAIT, &intCmd);
}

int LInt_enable(LInt *my, int level)
{
  int status;
  struct vme_conf vmeConf;

  status = ioctl(my->fd, VME_GET_CONF, &vmeConf);
  if (status != -1) {
    vmeConf.ilev = 1 << level;

    status = ioctl(my->fd, VME_SET_CONF, &vmeConf);
  }

  return status;
}
