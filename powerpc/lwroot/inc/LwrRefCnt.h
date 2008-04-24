#ifndef __LwrRefCnt_h__
#define __LwrRefCnt_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TRefCnt                                                             //
//                                                                      //
//  Base class for reference counted objects.                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'Light Weight ROOT' edition for LynxOs                       //
// O. Schaile                                                           //
//////////////////////////////////////////////////////////////////////////

#include "LwrTypes.h"

class TRefCnt {

protected:
   UInt_t  fRefs;      // (1 less than) number of references

public:
   enum EReferenceFlag { kStaticInit };

   TRefCnt(Int_t initRef = 0) : fRefs((UInt_t)initRef-1) { }
   TRefCnt(EReferenceFlag) { }  // leave fRefs alone
   UInt_t   References()            { return fRefs+1; }
   void     SetRefCount(UInt_t r)   { fRefs = r-1; }
   void     AddReference()          { fRefs++; }
   UInt_t   RemoveReference()       { return fRefs--; }
};

#endif
