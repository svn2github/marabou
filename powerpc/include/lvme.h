#ifndef LVME_H
#define LVME_H

#ifdef __cplusplus
extern "C" {
#endif

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
       */
void LVme_registerLogMsg(void (*f) (int priority, const char *message, ...));

void LVme_handleBusErr(void);
void LVme_restoreBusErr(void);
int LVme_tstBusErr(void);
void LVme_clrBusErr(void);

typedef unsigned long LVme_A;
typedef unsigned char LVme_B;
typedef unsigned short LVme_W;
typedef unsigned int LVme_L;
typedef volatile unsigned char LVme_VB;
typedef volatile unsigned short LVme_VW;
typedef volatile unsigned int LVme_VL;

#define LVme_maxName 32

typedef struct LVmeS {
	LVme_B *virtBase;
	LVme_A size;
	union {
		char name[LVme_maxName];
		int fd;
		unsigned long pno;
	} specific;
} LVme;

int conLVme(LVme *my, LVme_A vmeBase, LVme_A size, LVme_B addrMod, LVme_A chkOffset, LVme_B chkSize);
int desLVme(LVme *my);
void LVme_cleanup(void);

static LVme_B LVme_getB(LVme *my, LVme_A offset);
static LVme_W LVme_getW(LVme *my, LVme_A offset);
static LVme_L LVme_getL(LVme *my, LVme_A offset);

static void LVme_setB(LVme *my, LVme_A offset, LVme_B v);
static void LVme_setW(LVme *my, LVme_A offset, LVme_W v);
static void LVme_setL(LVme *my, LVme_A offset, LVme_L v);

static LVme_VB *LVme_getPtrB(LVme *my, LVme_A offset);
static LVme_VW *LVme_getPtrW(LVme *my, LVme_A offset);
static LVme_VL *LVme_getPtrL(LVme *my, LVme_A offset);

static int LVme_tstBitB(LVme *my, LVme_A offset, int bitNr);
static int LVme_tstBitW(LVme *my, LVme_A offset, int bitNr);
static int LVme_tstBitL(LVme *my, LVme_A offset, int bitNr);

static void LVme_setBitB(LVme *my, LVme_A offset, int bitNr);
static void LVme_setBitW(LVme *my, LVme_A offset, int bitNr);
static void LVme_setBitL(LVme *my, LVme_A offset, int bitNr);

static void LVme_clrBitB(LVme *my, LVme_A offset, int bitNr);
static void LVme_clrBitW(LVme *my, LVme_A offset, int bitNr);
static void LVme_clrBitL(LVme *my, LVme_A offset, int bitNr);

static void LVme_nop(LVme *my);
       /*
* DESCRIPTION
*      This is the header file with declarations and definitions
*      for the lvme library.  This library allows access to VME
*      hardware in a portable way.
*
*      *LVme_registerLogMsg()* allows the user of the lvme library
*      to register a callback function for error and log messages.
*      Each time the liblvme encounters an error, the function _f_
*      is called with arguments describing the error.  The signature
*      of _f_ is that of syslog(3), so LVme_registerLogMsg(syslog)
*      is straightforward. Passing NULL for _f_ leads to undefined
*      behavior.
*
*      If *LVme_registerLogMsg()* is never called, a default function
*      is used. This prints _message_ to stderr depending on the
*      environment variable LVME_VERBOSITY.  If LVME_VERBOSITY is
*      unset, nothing is ever printed.  If it is set to any value,
*      error messages will be printed.  If it is set to "DEBUG",
*      all messages will be printed.
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
*      LVme_A provides a portable data type for VME A32 addresses.
*      LVme_B, LVme_W, LVme_L provide portable data types for the
*      VME D8 (byte), VME D16 (word) and VME D32 (longword) access
*      word sizes respectively.  The LVme_V? types do the same
*      with volatile storage classifier.  For the user the volatile
*      types are only of interest when using the LVme_Ptr?()
*      functions.
*
*      conLVme()     The constructor for the LVme data type.
*                    It does the actual mapping from the virtual
*                    process address space to the physical VME
*                    addresses.  After conLVme was successfully
*                    called, the other functions of the library
*                    can be used.
*
*                    _my_ points to allocated space for one LVme,
*                    _vmeBase_ is the address to use in the VME
*                    address space, aligned to a hardware
*                    dependent page boundary (64 MByte seem to
*                    be save), _size_ the size of the address
*                    area in bytes, _addrMod_ the VME address
*                    modifier.  Supported _addrMod_ values are
*                    0x09 (A32USER) and 0x39(A24USER).
*
*                    On some systems, the conLVme can check for
*                    the availability of the addressed
*                    controller by doing one read access to the
*                    specified address.  For this purpose
*                    _chkOffset_ gives the offset inside the VME
*                    area (counting from _cardBase_) where the
*                    addressed controller allows a read access
*                    and _chkSize_ the access size (0 = no
*                    check,1 = D8, 2 = D16, 4 = D32).
*
*      desLVme()     The destructor for the LVme data type.
*                    It returns the mapped address space and all
*                    resources back to the OS, so it is not
*                    accessible any more.
*                    _my_ points to an LVme.  If _my_ is NULL,
*                    no action takes place.
*
*      LVme_cleanup()  Removes all VME mappings on systems where
*                    the unmapping is not done automatically at
*                    process termination.
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
*      *conLVme()* returns 0 if the mapping and the test access
*      was sucessful and -1 if one of both failed.
*      
*      The *LVme_get?()* functions return the value that was read
*      via the VME bus, the *LVme_tstBit?() functions return 1
*      is the specified bit was set and 0 otherwise.
*
* SEE ALSO
*      lbma.h, lint.h
*
*****************************************************************/

static LVme_B LVme_getB(LVme *my, LVme_A offset) {
	return *(LVme_VB *) (my->virtBase + offset);
}

static LVme_W LVme_getW(LVme *my, LVme_A offset) {
	return VTOHW(*(LVme_VW *) (my->virtBase + offset));
}

static LVme_L LVme_getL(LVme *my, LVme_A offset) {
	return VTOHL(*(LVme_VL *) (my->virtBase + offset));
}

static void LVme_setB(LVme *my, LVme_A offset, LVme_B v) {
	*(LVme_VB *) (my->virtBase + offset) = v;
}

static void LVme_setW(LVme *my, LVme_A offset, LVme_W v) {
	*(LVme_VW *) (my->virtBase + offset) = HTOVW(v);
}

static void LVme_setL(LVme *my, LVme_A offset, LVme_L v) {
	*(LVme_VL *) (my->virtBase + offset) = HTOVL(v);
}

static LVme_VB *LVme_getPtrB(LVme *my, LVme_A offset) {
	return (LVme_VB *) (my->virtBase + offset);
}

static LVme_VW *LVme_getPtrW(LVme *my, LVme_A offset) {
	return (LVme_VW *) (my->virtBase + offset);
}

static LVme_VL *LVme_getPtrL(LVme *my, LVme_A offset) {
	return (LVme_VL *) (my->virtBase + offset);
}

static int LVme_tstBitB(LVme *my, LVme_A offset, int bitNr) {
	return (*(LVme_VB *) (my->virtBase + offset) & (1 << bitNr)) != 0;
}

static int LVme_tstBitW(LVme *my, LVme_A offset, int bitNr) {
	return (*(LVme_VW *) (my->virtBase + offset) & HTOVW(1 << bitNr)) != 0;
}

static int LVme_tstBitL(LVme *my, LVme_A offset, int bitNr) {
	return (*(LVme_VL *) (my->virtBase + offset) & HTOVL(1 << bitNr)) != 0;
}

static void LVme_setBitB(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VB *) (my->virtBase + offset) |= (1UL << bitNr);
}

static void LVme_setBitW(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VW *) (my->virtBase + offset) |= HTOVW(1UL << bitNr);
}

static void LVme_setBitL(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VL *) (my->virtBase + offset) |= HTOVL(1UL << bitNr);
}

static void LVme_clrBitB(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VB *) (my->virtBase + offset) &= ~(1UL << bitNr);
}

static void LVme_clrBitW(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VW *) (my->virtBase + offset) &= HTOVW(~(1UL << bitNr));
}

static void LVme_clrBitL(LVme *my, LVme_A offset, int bitNr) {
	*(LVme_VL *) (my->virtBase + offset) &= HTOVL(~(1UL << bitNr));
}

static void LVme_nop(LVme *my) {
#if defined(__68k__)
	__asm__ volatile ("nop");
#elif defined(__powerpc__)
	__asm__ volatile ("eieio");
#endif
}

#ifdef __cplusplus
}
#endif

#endif
