#ifndef __TUsrEvtStart_h__
#define __TUsrEvtStart_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart.h
// Purpose:        Class to describe a start event
// Description:
// Author:         R. Lutter
// Revision:       $Id: TUsrEvtStart.h,v 1.3 2007-08-08 11:15:59 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TFile.h"

#include "TUsrEvent.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtStart
// Purpose:        Define an event for "Start Acquisition"
// Description:    Event assigned to trigger 14 (Start Acquisition)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TUsrEvtStart : public TUsrEvent {

	public:
		TUsrEvtStart(); 				// ctor
		~TUsrEvtStart() {};				// default dtor

		Bool_t ExtractTimeStamp(const s_vehe *, MBSDataIO *);
		Bool_t CreateTree();
		Bool_t InitializeTree(TFile * RootFile);
		inline Int_t GetTime() const { return(fClockSecs); };

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	ClassDef(TUsrEvtStart, 1)		// [Analyze] Event type "START ACQUISITION"
};

#endif
