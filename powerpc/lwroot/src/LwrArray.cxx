//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArray                                                               //
//                                                                      //
// Abstract array base class. Used by TArrayC, TArrayS, TArrayI,        //
// TArrayL, TArrayF and TArrayD.                                        //
// Data member is public for historical reasons.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition                                  //
// R. Lutter                                                            //
//////////////////////////////////////////////////////////////////////////

#ifdef CPU_TYPE_RIO2
#include <iostream.h>
#include <iomanip.h>
#else
#include <iostream>
#include <iomanip>
#endif


#include "LwrArray.h"

//______________________________________________________________________________
Bool_t TArray::OutOfBoundsError(const char *where, Int_t i) const
{
   // Generate an out-of-bounds error. Always returns false.

   cerr << "Index " << i << " out of bounds (size: " << fN << endl;
   return kFALSE;
}
