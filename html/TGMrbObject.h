#ifndef __TGMrbObject_h__
#define __TGMrbObject_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbObject.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbObject  - some basic methods common to all MARaBOU widgets
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TSystem.h"
#include "TGMrbLayout.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbObject
// Purpose:        Base class for MARaBOU widgets
// Methods:        
// Description:    Implements methods common to all MARaBOU widgets.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbObject {

	public:
		TGMrbObject() {};									// ctor

		TGMrbObject(const TGMrbObject &) {};				// default copy ctor

		virtual ~TGMrbObject() { fHeap.Delete(); };			// dtor

		TGMrbLayout * SetupGC(TGMrbLayout * GC, UInt_t FrameOptions);

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbObject.html&"); };

	protected:
		TList fHeap;							// store heap objects here

	ClassDef(TGMrbObject, 0)	// [GraphUtils] Some extension to TGObject
};

#endif
