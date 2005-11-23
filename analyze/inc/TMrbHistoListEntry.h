#ifndef __TMrbHistoListEntry_h__
#define __TMrbHistoListEntry_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistoListEntry.h
// Purpose:        Class to manage a list of histograms
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbHistoListEntry.h,v 1.1 2005-11-23 11:51:53 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TMrbNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbHistoListEntry
// Purpose:        An entry in user's list of histograms
// Description:    Bookkeeping: Connects histograms to modules and params
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbHistoListEntry : public TObject {

	public:
		TMrbHistoListEntry( TMrbNamedX * Module = NULL,								// ctor
							TMrbNamedX * Param = NULL,
							TH1 * Address = NULL) :	fModule(Module),
													fParam(Param),
													fAddress(Address) {};
		virtual ~TMrbHistoListEntry() {};  											// dtor

		inline TMrbNamedX * GetModule() const { return(fModule); };
		inline void SetModule(TMrbNamedX * Module) { fModule = Module; };
		inline TMrbNamedX * GetParam() const { return(fParam); };
		inline void SetParam(TMrbNamedX * Param) { fParam = Param; };
		inline TH1 * GetAddress() const { return(fAddress); };
		inline void SetAddress(TH1 * Address) { fAddress = Address; };

	protected:
		TMrbNamedX * fModule;
		TMrbNamedX * fParam;
		TH1 * fAddress;

	ClassDef(TMrbHistoListEntry, 0) 	// [Analyze] List of histograms
};

#endif
