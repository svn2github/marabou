#ifndef __LwrUtils_h__
#define __LwrUtils_h__

#include "LwrTypes.h"

UInt_t swapIt(UInt_t x);
ULong_t swapIt(ULong_t x);

inline Int_t swapIt(Int_t x) { return((Int_t) swapIt((UInt_t) x)); };
inline Long_t swapIt(Long_t x) { return((Long_t) swapIt((ULong_t) x)); };
inline Long_t swapIt(void * x) { return((Long_t) swapIt((ULong_t) x)); };

#endif
