//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            DGFModule.cxx
// Purpose:        A GUI to control the XIA DGF-4C
// Description:    Module Definitions
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TList.h"

#include "DGFControlData.h"
#include "DGFModule.h"

ClassImp(DGFModule)

extern DGFControlData * gDGFControlData;

DGFModule::DGFModule(const Char_t * Name, const Char_t * Host, Int_t Crate, Int_t Station) : TNamed(Name, "") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFModule
// Purpose:        DGF specs
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	SetHost(Host);
	fAddr = NULL;
	this->SetClusterID(0, "", "", 0);
	if (!SetCrate(Crate)) this->MakeZombie();
	if (!SetStation(Station)) this->MakeZombie();
};

Bool_t DGFModule::SetCrate(Int_t Crate) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFModule::SetCrate
// Purpose:        Define crate number
// Arguments:      Int_t Crate       -- camac crate
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines the crate number.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF  * dgf;
	dgf = new TMrbDGF();
	if (!dgf->SetCrate(Crate)) {
		delete dgf;
		return(kFALSE);
	} else {
		delete dgf;
		fCrate = Crate;
		return(kTRUE);
	}
}

Bool_t DGFModule::SetStation(Int_t Station) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFModule::SetStation
// Purpose:        Define crate number
// Arguments:      Int_t Station       -- camac station
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Defines the station number.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbDGF * dgf;
	dgf = new TMrbDGF();
	if (!dgf->SetStation(Station)) {
		delete dgf;
		return(kFALSE);
	} else {
		delete dgf;
		fStation = Station;
		return(kTRUE);
	}
}
