#ifndef LVME_H
#define LVME_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stddef.h>
#include <stdint.h>
  
#if defined(LVME_SOFTSWAP)
#include <netinet/in.h>
#define HTOVW(a) htons(a)
#define HTOVL(a) htonl(a)
#define VTOHW(a) ntohs(a)
#define VTOHL(a) ntohl(a)
#else
#define HTOVW(a) (a)
#define HTOVL(a) (a)
#define VTOHW(a) (a)
#define VTOHL(a) (a)
#endif
  
  /******************************************************************
   * NAME
   *        lvme.h - Portable access to VME bus
   *
   * SYNOPSIS
   ********************************************************************/
  
  typedef uint8_t LVme_B;
  typedef uint16_t LVme_W;
  typedef uint32_t LVme_L;
  typedef uint32_t LVme_A;
  typedef volatile uint8_t LVme_VB;
  typedef volatile uint16_t LVme_VW;
  typedef volatile uint32_t LVme_VL;
  
  typedef enum LVME_ADDR_MOD {
    LVME_A16 = 0x29,
    LVME_A24 = 0x39,
    LVME_A32 = 0x09
  } LVME_ADDR_MOD;
  
  typedef struct LVmeS {
    uint8_t* virtBase;
    uint32_t vmeAddr;
    LVME_ADDR_MOD addrMod;    
    uint32_t size;
    int mapped;
  } LVme;
    
  LVme* newLVme(uint32_t vmeAddr, uint32_t size, LVME_ADDR_MOD addrMod);
  LVme* newLVme_MBS(uint32_t vmeAddr, uint32_t size,
                    LVME_ADDR_MOD addrMod, void* pl_loc_hwacc);
  void delLVme(LVme* my);
  
  static uint8_t LVme_getB(LVme* my, ptrdiff_t offset);
  static uint16_t LVme_getW(LVme* my, ptrdiff_t offset);
  static uint32_t LVme_getL(LVme* my, ptrdiff_t offset);
  
  static void LVme_setB(LVme* my, ptrdiff_t offset, uint8_t v);
  static void LVme_setW(LVme* my, ptrdiff_t offset, uint16_t v);
  static void LVme_setL(LVme* my, ptrdiff_t offset, uint32_t v);
  
  static LVme_VB* LVme_getPtrB(LVme* my, ptrdiff_t offset);
  static LVme_VW* LVme_getPtrW(LVme* my, ptrdiff_t offset);
 
  static LVme_VL* LVme_getPtrL(LVme* my, ptrdiff_t offset);

  static int LVme_tstBitB(LVme* my, ptrdiff_t offset, int bitNr);
  static int LVme_tstBitW(LVme* my, ptrdiff_t offset, int bitNr);
  static int LVme_tstBitL(LVme* my, ptrdiff_t offset, int bitNr);

  static void LVme_setBitB(LVme* my, ptrdiff_t offset, int bitNr);
  static void LVme_setBitW(LVme* my, ptrdiff_t offset, int bitNr);
  static void LVme_setBitL(LVme* my, ptrdiff_t offset, int bitNr);

  static void LVme_clrBitB(LVme* my, ptrdiff_t offset, int bitNr);
  static void LVme_clrBitW(LVme* my, ptrdiff_t offset, int bitNr);
  static void LVme_clrBitL(LVme* my, ptrdiff_t offset, int bitNr);

  static void LVme_nop(LVme* my);
  
  /*
   * DESCRIPTION
   *      This is the header file with declarations and definitions
   *      for the lvme library.  This library allows access to VME
   *      hardware in a portable way.
   *
   *      The *LVme_handleBusErr() function installs a standard signal
   *      handler for the bus error signal.  In case of a bus error,
   *      this handler sets a flag that can be checked using the
   *      *LVme_tstBusErr()* function and cleared by the
   *      *LVme_clrBusErr()* function.  Note that the flag stays pending,
   *      even if successful VME operations are performed after the bus
   *      error.  The *LVme_restoreBusErr()* function restores the signal
   *      handler to the state before *LVme_handleBusErr()* was called.
   *
   *
   *      LVme_getB(), LVme_getW(), LVme_getL()
   *                    Reads one data word of the specified access
   *                    size from _offset_.
   *
   *      LVme_setB(), LVme_setW(), LVme_setL()
   *                    Writes _v_ to _offset_ using the
   *                    specified access.
   *
   *      LVme_getPtrB(), LVme_getPtrW(), LVme_getPtrL()
   *                    Returns a pointer in the virtual address
   *                    space pointing to _offset_ in VME address
   *                    space.  This allows for fast access to
   *                    large VME areas, but provides no software
   *                    byte swapping.
   *
   *      LVme_clrBitB(), LVme_clrBitW(), LVme_clrBitL()
   *                     Clears (=0) bit number _bitNr_ at _offset_ using
   *                     the specified access size.
   *
   *      LVme_setBitB(), LVme_setBitW(), LVme_setBitL()
   *                     Sets (=1) bit number _bitNr_ at _offset_ using
   *                     the specified access size.
   *
   *      LVme_tstBitB(), LVme_tstBitW(), LVme_tstBitL()
   *                     Reads the value of bit number _bitNr_ at
   *                     _offset_ using the specified access size.
   *
   * NOTES
   *
   *      _offset_ gives the offset from _cardBase_ in bytes.
   *      _bitNr_ counts LSB = 0 and MSB = 7, 15 or 31
   *      respectively.
   *
   *      On systems that cannot do proper byte swapping VME<->CPU
   *      and vice versa by hardware (e.g. Linux with the Universe VME
   *      driver), the macro LVME_SOFTSWAP must be defined.  In that
   *      case, be very careful when using any of the LVme_getPtr()
   *      functions, the pointer gives you raw (unswapped) access
   *      to the hardware.
   *
   *      Many CPUs do not allow unaligned access, so _offset_ must
   *      be a multiple of 2 for D16 and 4 for D32 access.
   *
   *      The access functions are static in the header file.  This
   *      allows state of the art compilers to inline the functions
   *      and spare the function call.  If compilers show up that can
   *      not inline, it may change to '#defines', so be careful
   *      with side effects in the function parameters.
   *
   * RETURN VALUE
   *      The *LVme_tstBusErr()* function return 1 if a bus error
   *      occured since the last call to *LVme_clrBusErr()* and 0
   *      otherwise.
   *
   *      The *LVme_get?()* functions return the value that was read
   *      via the VME bus, the *LVme_tstBit?() functions return 1
   *      is the specified bit was set and 0 otherwise.
   *
   * SEE ALSO
   *      lbma.h, lint.h
   *
   *****************************************************************/

  static uint8_t LVme_getB(LVme *my, ptrdiff_t offset) {
    return *((volatile uint8_t *) (my->virtBase + offset));
  }

  static uint16_t LVme_getW(LVme *my, ptrdiff_t offset) {
    return VTOHW(*((volatile uint16_t *) (my->virtBase + offset)));
  }

  static uint32_t LVme_getL(LVme *my, ptrdiff_t offset) {
    return VTOHL(*((volatile uint32_t *) (my->virtBase + offset)));
  }

  static void LVme_setB(LVme *my, ptrdiff_t offset, uint8_t v) {
    *((volatile uint8_t *) (my->virtBase + offset)) = v;
  }

  static void LVme_setW(LVme *my, ptrdiff_t offset, uint16_t v) {
    *((volatile uint16_t *) (my->virtBase + offset)) = HTOVW(v);
  }

  static void LVme_setL(LVme *my, ptrdiff_t offset, uint32_t v) {
    *((volatile uint32_t *) (my->virtBase + offset)) = HTOVL(v);
  }

  static volatile uint8_t *LVme_getPtrB(LVme *my, ptrdiff_t offset) {
    return (volatile uint8_t *) (my->virtBase + offset);
  }

  static volatile uint16_t *LVme_getPtrW(LVme *my, ptrdiff_t offset) {
    return (volatile uint16_t *) (my->virtBase + offset);
  }

  static volatile uint32_t *LVme_getPtrL(LVme *my, ptrdiff_t offset) {
    return (volatile uint32_t *) (my->virtBase + offset);
  }

  static int LVme_tstBitB(LVme *my, ptrdiff_t offset, int bitNr) {
    return (*((volatile uint8_t *) (my->virtBase + offset)) & (1 << bitNr)) 
      != 0;
  }
  
  static int LVme_tstBitW(LVme *my, ptrdiff_t offset, int bitNr) {
    return 
      (*((volatile uint16_t *) (my->virtBase + offset)) & HTOVW(1 << bitNr))
      != 0;
  }

  static int LVme_tstBitL(LVme *my, ptrdiff_t offset, int bitNr) {
    return
      (*((volatile uint32_t *) (my->virtBase + offset)) & HTOVL(1 << bitNr))
      != 0;
  }

  static void LVme_setBitB(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint8_t *) (my->virtBase + offset)) |= (1UL << bitNr);
  }

  static void LVme_setBitW(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint16_t *) (my->virtBase + offset)) |= HTOVW(1UL << bitNr);
  }

  static void LVme_setBitL(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint32_t *) (my->virtBase + offset)) |= HTOVL(1UL << bitNr);
  }

  static void LVme_clrBitB(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint8_t *) (my->virtBase + offset)) &= ~(1UL << bitNr);
  }

  static void LVme_clrBitW(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint16_t *) (my->virtBase + offset)) &= HTOVW(~(1UL << bitNr));
  }

  static void LVme_clrBitL(LVme *my, ptrdiff_t offset, int bitNr) {
    *((volatile uint32_t *) (my->virtBase + offset)) &= HTOVL(~(1UL << bitNr));
  }

  
  static void LVme_nop(LVme *my) {
#if defined(__68k__)
    __asm__ volatile ("nop");
#elif defined(__powerpc__)
    __asm__ volatile ("eieio");
#elif defined(__386__) 
    __asm__ volatile ("push %eax");
    __asm__ volatile ("push %ebx");
    __asm__ volatile ("push %ecx");
    __asm__ volatile ("push %edx");
    __asm__ volatile ("CPUID");
    __asm__ volatile ("pop  %edx");
    __asm__ volatile ("pop  %ecx");
    __asm__ volatile ("pop  %ebx");
    __asm__ volatile ("pop  %eax");
    __asm__ volatile ("nop");
#endif
  }

static void LVme_ignoreThisFunction()
{
  /*
    NEVER CALL THIS FUNCTION, IT'S JUST A DIRTY TRICK TO
    SUPPRESS COMPILER WARNINGS WHEN HANDING OVER -Wall, -pedantic
    OR SIMILAR OPTIONS TO THE COMPILER
  */
  return;
  
  LVme_getB(NULL, 0);
  LVme_getW(NULL, 0);
  LVme_getL(NULL, 0);
  LVme_setB(NULL, 0, 0);
  LVme_setW(NULL, 0, 0);
  LVme_setL(NULL, 0, 0);
  LVme_getPtrB(NULL, 0);
  LVme_getPtrW(NULL, 0);
  LVme_getPtrL(NULL, 0);
  LVme_tstBitB(NULL, 0, 0);
  LVme_tstBitW(NULL, 0, 0);
  LVme_tstBitL(NULL, 0, 0);
  LVme_setBitB(NULL, 0, 0);
  LVme_setBitW(NULL, 0, 0);
  LVme_setBitL(NULL, 0, 0);
  LVme_clrBitB(NULL, 0, 0);
  LVme_clrBitW(NULL, 0, 0);
  LVme_clrBitL(NULL, 0, 0);
  LVme_nop(NULL);
}

#ifdef __cplusplus
}
#endif

#endif
