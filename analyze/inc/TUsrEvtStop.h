#ifndef __TUsrEvtStop_h__
#define __TUsrEvtStop_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop.h
// Purpose:        Class to describe a stop event
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrEvtStop.h,v 1.3 2007-08-08 11:15:59 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TFile.h"

#include "TUsrEvent.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStop
// Purpose:        Define an event for "Stop Acquisition"
// Description:    Event assigned to trigger 15 (Stop Acquisition)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEvtStop : public TUsrEvent {

	public:
		TUsrEvtStop();  				// ctor
		~TUsrEvtStop() {};				// default dtor

		Bool_t ExtractTimeStamp(const s_vehe *, MBSDataIO *);
		Bool_t CreateTree();
		Bool_t InitializeTree(TFile * RootFile);
		inline Int_t GetTime() const { return(fClockSecs); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TUsrEvtStop, 1)		// [Analyze] Event type "STOP ACQUISITION"
};

#endif
