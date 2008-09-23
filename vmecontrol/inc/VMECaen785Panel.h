#ifndef __VMECaen785Panel_h__
#define __VMECaen785Panel_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMECaen785Panel.h
// Purpose:        Class defs for VME modules
// Class:          VMECaen785Panel
// Description:    A GUI to control vme modules via tcp
// Author:         R. Lutter
// Revision:       $Id: VMECaen785Panel.h,v 1.1 2008-09-23 10:47:20 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-09-23 10:47:20 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "TGWindow.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGMenu.h"
#include "TGTextView.h"

#include "TMrbLofNamedX.h"
#include "TMrbNamedX.h"
#include "TMrbTail.h"

#include "TGMrbTextButton.h"
#include "TGMrbRadioButton.h"
#include "TGMrbLabelEntry.h"
#include "TGMrbFileEntry.h"
#include "TGMrbLabelCombo.h"
#include "TGMrbLofKeyBindings.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMECaen785Panel
// Purpose:        Panel to control a Caen V785 adc
// Constructors:   
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class VMECaen785Panel : public TGCompositeFrame {

	public:
		VMECaen785Panel(TGCompositeFrame * TabFrame);
		virtual ~VMECaen785Panel() {
			fFocusList.Clear();
			fHeap.Delete();
		};

		inline TMrbLofNamedX * GetLofModules() { return(&fLofModules); };
		inline Int_t GetNofModules() { return(fLofModules.GetEntries()); };

		Bool_t SetupModuleList();

	protected:
		TList fHeap;								//! list of objects created on heap

		TMrbLofNamedX fLofModules;	// list of CAEN V785 modules

		TGMrbFocusList fFocusList;

	ClassDef(VMECaen785Panel, 0) 	// [VMEControl] Panel to control a Caen V785 adc
};

#endif
