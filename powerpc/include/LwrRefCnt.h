#ifndef __LwrRefCnt_h__
#define __LwrRefCnt_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			LwrRefCnt.h
//! \brief			Light Weight ROOT
//! \details		Class definitions for ROOT under LynxOs: TRefCnt<br>
//! 				Base class for reference counted objects.
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $     
//! $Date: 2009-02-03 13:30:30 $
//////////////////////////////////////////////////////////////////////////////

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
