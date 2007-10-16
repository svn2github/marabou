#ifndef __TUsrMux_h__
#define __TUsrMux_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux.h
// Purpose:        Class to describe a hit
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrMux.h,v 1.1 2007-10-16 14:24:04 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>

#include "TObject.h"
#include "TString.h"
#include "TEnv.h"
#include "TH1F.h"

#include "TUsrHit.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux
// Purpose:        Class to store hit data
// Description:    Describes a single hit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrMuxEntry : public TObject {

	friend class TUsrMux;

	public:
		TUsrMuxEntry(TEnv * LkpEnv, TH1F * E0, TH1F * E1, TH1F * P0, TH1F * P1, TArrayF * LkpTable, TUsrHit * Hit = NULL) {
			fLkpEnv = LkpEnv;
			fE0 = E0;
			fE1 = E1;
			fP0 = P0;
			fP1 = P1;
			fLkpTable = LkpTable;
			fHit = Hit;
		};

		~TUsrMuxEntry() {};
		
	protected:		
		TEnv * fLkpFile;
		TH1F * fE0;
		TH1F * fE1;
		TH1F * fP0;
		TH1F * fP1;
		TArrayF * fLkpTable;
		TUsrHit * fHit;
		
	ClassDef(TUsrMuxEntry, 0)				// [Analyze] Multiplexer entry
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrMux
// Purpose:        Class to store hit data
// Description:    Describes a single hit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TUsrMux : public TNamed {

	public:
		TUsrMux();
		~TUsrMux() {};														// default dtor
		

	protected:		
		TEnv * fConfig;
		TObjArray fMuxTable;
		TObjArray fMuxEntries;
		
	ClassDef(TUsrMux, 1)				// [Analyze] Multiplexer
};

#endif
