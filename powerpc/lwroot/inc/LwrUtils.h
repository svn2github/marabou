#ifndef __LwrUtils_h__
#define __LwrUtils_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrUtils.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: Utilities
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.2 $     
//! $Date: 2009-02-03 08:29:20 $
//////////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"

UInt_t swapIt(UInt_t x);
ULong_t swapIt(ULong_t x);

inline Int_t swapIt(Int_t x) { return((Int_t) swapIt((UInt_t) x)); };
inline Long_t swapIt(Long_t x) { return((Long_t) swapIt((ULong_t) x)); };
inline Long_t swapIt(void * x) { return((Long_t) swapIt((ULong_t) x)); };

#endif
