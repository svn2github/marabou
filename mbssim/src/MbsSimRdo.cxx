//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo.cxx
// Purpose:        Simulate MBS i/o offline
// Description:    Implements class methods to simulate MBS i/o
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: MbsSimRdo.cxx,v 1.3 2008-06-16 15:00:21 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-06-16 15:00:21 $
//
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"

#include "TMrbLogger.h"

#include "MbsSimRdo.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;

MbsSimRdo * gMbsSimRdo = NULL;

ClassImp(MbsSimRdo)
ClassImp(MbsSimModule)

const SMrbNamedXShort kMbsSimLofModuleTypes[] =
							{
								{MbsSimRdo::kMbsSimMtypeSilena4418V,	"TMrbSilena_4418V"	},
								{MbsSimRdo::kMbsSimMtypeSilena4418T,	"TMrbSilena_4418T"	},
								{MbsSimRdo::kMbsSimMtypeCaenV785,		"TMrbCaen_V785" 	},
								{MbsSimRdo::kMbsSimMtypeCaenV775,		"TMrbCaen_V775" 	},
								{MbsSimRdo::kMbsSimMtypeSis3300,		"TMrbSis_3300"		},
								{MbsSimRdo::kMbsSimMtypeSis3600,		"TMrbSis_3600"		},
								{MbsSimRdo::kMbsSimMtypeSis3801,		"TMrbSis_3801"		},
								{MbsSimRdo::kMbsSimMtypeSis3820,		"TMrbSis_3820"		},
								{MbsSimRdo::kMbsSimMtypeXiaDgf4C,		"TMrbXia_DGF_4C"	},
								{0, 									NULL				}
							};

MbsSimRdo::MbsSimRdo(const Char_t * ConfigFile) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::MbsSimRdo
// Purpose:        Main class to simulte MBS i/o
// Arguments:      Char_t * ConfigFile   -- config description file (.rc)
// Description:    Starts simulation of MBS i/o in offline mode.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger(this->ClassName(), "mbssim.log");

	if (gSystem->AccessPathName(ConfigFile)) {
		gMrbLog->Err() << "No such file - " << ConfigFile << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else {
		fConfigFile = ConfigFile;
		fConfig = new TEnv(ConfigFile);
		fLofModules.SetName("List of modules");
		fLofModules.Delete();
		fLofModuleTypes.SetName("List of module types");
		fLofModuleTypes.AddNamedX(kMbsSimLofModuleTypes);
	}
}

MbsSimModule * MbsSimRdo::FindModule(const Char_t * ModuleName, Bool_t CreateIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::FindModule
// Purpose:        Search for given module in list
// Arguments:      Char_t * ModuleName   -- module name
//                 Bool_t CreateIt       -- create a new entry if not found
// Results:        MbsSimModule * Module -- pointer to module specs
// Exceptions:     
// Description:    Searches for a given module and returns specs
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx = fLofModules.FindByName(ModuleName);
	if (nx == NULL) {
		if (CreateIt) {
			MbsSimModule * newModule = this->CreateModule(ModuleName);
			if (newModule) {
				TMrbNamedX * mtype = newModule->GetType();
				fLofModules.AddNamedX(mtype->GetIndex() + newModule->Serial(), ModuleName, mtype->GetTitle(), newModule);
			}
			return(newModule);
		}
		return(NULL);
	} else {	
		return((MbsSimModule *) nx->GetAssignedObject());
	}
}

MbsSimModule * MbsSimRdo::FindModule(Int_t Crate, Int_t Station, Bool_t CreateIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::FindModule
// Purpose:        Search for given (CAMAC) module in list
// Arguments:      Int_t Crate           -- crate number
//                 Int_t Station         -- station
//                 Bool_t CreateIt       -- create a new entry if not found
// Results:        MbsSimModule * Module -- pointer to module specs
// Exceptions:     
// Description:    Searches for a given camac module and returns specs
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TList * lofConfItems = (TList *) fConfig->GetTable();
	TIterator * cIter = lofConfItems->MakeIterator();
	TEnvRec * r;
	TString lofModuleNames = "";
	Bool_t first = kTRUE;
	while (r = (TEnvRec *) cIter->Next()) {
		TString resName = r->GetName();
		if (resName.Contains("LofModules")) {
			if (!first) lofModuleNames += ":";
			first = kFALSE;
			lofModuleNames += r->GetValue();
		}
	}
	Int_t from = 0;
	TString moduleName;
	Bool_t found = kFALSE;
	while (lofModuleNames.Tokenize(moduleName, from, ":")) {
		Int_t crate = this->GetConfig(moduleName.Data(), "Crate", 0);
		Int_t station = this->GetConfig(moduleName.Data(), "Station", 0);
		if (crate == Crate && station == Station) {
			found = kTRUE;
			break;
		}
	}
	if (!found) {
		gMrbLog->Err() << "Module not in config - " << moduleName << endl;
		gMrbLog->Flush(this->ClassName(), "FindModule");
		return(NULL);
	}
	TMrbNamedX * nx = fLofModules.FindByName(moduleName.Data());
	if (nx) {
		MbsSimModule * m = (MbsSimModule *) nx->GetAssignedObject();
		if (!m->IsCamac() || m->Crate() != Crate || m->Station() != Station) {
			gMrbLog->Err() << "Module already defined - " << moduleName << " (settings different)" << endl;
			gMrbLog->Flush(this->ClassName(), "FindModule");
			return(NULL);
		}
		return(m);
	} else if (CreateIt) {
		return(this->CreateModule(moduleName.Data()));
	} else {
		return(NULL);
	}
}

MbsSimModule * MbsSimRdo::FindModule(Int_t VmeAddr, Bool_t CreateIt) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::FindModule
// Purpose:        Search for given (VME) module in list
// Arguments:      Int_t VmeAddr         -- vme address
//                 Bool_t CreateIt       -- create a new entry if not found
// Results:        MbsSimModule * Module -- pointer to module specs
// Exceptions:     
// Description:    Searches for a given vme module and returns specs
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TList * lofConfItems = (TList *) fConfig->GetTable();
	TIterator * cIter = lofConfItems->MakeIterator();
	TEnvRec * r;
	TString lofModuleNames = "";
	Bool_t first = kTRUE;
	while (r = (TEnvRec *) cIter->Next()) {
		TString resName = r->GetName();
		if (resName.Contains("LofModules")) {
			if (!first) lofModuleNames += ":";
			first = kFALSE;
			lofModuleNames += r->GetValue();
		}
	}
	Int_t from = 0;
	TString moduleName;
	Bool_t found = kFALSE;
	while (lofModuleNames.Tokenize(moduleName, from, ":")) {
		TString addr = this->GetConfig(moduleName.Data(), "Address", "");
		if (addr.BeginsWith("C0.0x")) {
			addr = addr(5, 1000);
			Int_t iaddr;
			sscanf(addr.Data(), "%x", &iaddr);
			if (iaddr == VmeAddr) {
				found = kTRUE;
				break;
			}
		}
	}
	if (!found) {
		gMrbLog->Err() << "Module not in config - " << moduleName << endl;
		gMrbLog->Flush(this->ClassName(), "FindModule");
		return(NULL);
	}
	TMrbNamedX * nx = fLofModules.FindByName(moduleName.Data());
	if (nx) {
		MbsSimModule * m = (MbsSimModule *) nx->GetAssignedObject();
		if (m->IsCamac() || m->VmeAddr() != VmeAddr) {
			gMrbLog->Err() << "Module already defined - " << moduleName << " (settings different)" << endl;
			gMrbLog->Flush(this->ClassName(), "FindModule");
			return(NULL);
		}
		return(m);
	} else if (CreateIt) {
		return(this->CreateModule(moduleName.Data()));
	} else {
		return(NULL);
	}
}

MbsSimModule * MbsSimRdo::CreateModule(const Char_t * ModuleName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::CreateModule
// Purpose:        Create a new module entry
// Arguments:      Char_t * ModuleName   -- module name
// Results:        MbsSimModule * Module -- pointer to module specs
// Exceptions:     
// Description:    Creates a new module
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t serial = this->GetConfig(ModuleName, "Serial", 0);
	if (serial == 0) {
		gMrbLog->Err() << "Module not in config - " << ModuleName << endl;
		gMrbLog->Flush(this->ClassName(), "CreateModule");
		return(NULL);
	}
	TString moduleClass = this->GetConfig(ModuleName, "ClassName", "");
	TMrbNamedX * mtype = fLofModuleTypes.FindByName(moduleClass.Data());
	if (mtype == NULL) {
		gMrbLog->Err() << "[" << ModuleName << "] Unknown module class - " << moduleClass << endl;
		gMrbLog->Flush(this->ClassName(), "CreateModule");
		return(NULL);
	}
	mtype->SetTitle(this->GetConfig(ModuleName, "Title", ""));
	TString interFace = this->GetConfig(ModuleName, "Interface", "");
	if (interFace.CompareTo("CAMAC") == 0) {
		Int_t crate = this->GetConfig(ModuleName, "Crate", 0);
		Int_t station = this->GetConfig(ModuleName, "Station", 0);
		if (crate <= 0 || station <= 0) {
			gMrbLog->Err() << "[" << ModuleName << "] Wrong CAMAC addr - C" << crate << ".N" << station << endl;
			gMrbLog->Flush(this->ClassName(), "CreateModule");
			return(NULL);
		}
		MbsSimModule * newModule = new MbsSimModule(ModuleName, mtype, serial, crate, station);
		return(newModule);
	} else if (interFace.CompareTo("VME") == 0){
		TString addr = this->GetConfig(ModuleName, "Address", "");
		if (!addr.BeginsWith("C0.0x")) {
			gMrbLog->Err() << "[" << ModuleName << "] Wrong VME addr - " << addr << endl;
			gMrbLog->Flush(this->ClassName(), "CreateModule");
			return(NULL);
		}
		addr = addr(5, 1000);
		Int_t iaddr;
		sscanf(addr.Data(), "%x", &iaddr);
		MbsSimModule * newModule = new MbsSimModule(ModuleName, mtype, serial, iaddr);
		return(newModule);
	} else {
		gMrbLog->Err() << "[" << ModuleName << "] Wrong interface - " << interFace << endl;
		gMrbLog->Flush(this->ClassName(), "CreateModule");
		return(NULL);
	}
}

Int_t MbsSimRdo::GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, Int_t Default) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MbsSimRdo::GetConfig
// Purpose:        Get item from config
// Arguments:      Char_t * ModuleName   -- module name
//                 Char_t * ConfigItem   -- item name
//                 Int_t Default         -- default value
// Results:        Int_t Value           -- item value
// Exceptions:     
// Description:    Performs a config lookup
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString mname = ModuleName;
	mname(0,1).ToUpper();
	return(fConfig->GetValue(Form("TMrbConfig.Module.%s", ConfigItem), Default));
}

Bool_t MbsSimRdo::GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, Bool_t Default) {
	TString mname = ModuleName;
	mname(0,1).ToUpper();
	return(fConfig->GetValue(Form("TMrbConfig.Module.%s", ConfigItem), Default));
}

const Char_t * MbsSimRdo::GetConfig(const Char_t * ModuleName, const Char_t * ConfigItem, const Char_t * Default) {
	TString mname = ModuleName;
	mname(0,1).ToUpper();
	return(fConfig->GetValue(Form("TMrbConfig.Module.%s", ConfigItem), Default));
}

unsigned int * MbsSimRdo::BcnafAddr(Int_t branch, Int_t crate, Int_t nstation, Int_t addr, Int_t function) { return(NULL); };
volatile unsigned int * MbsSimRdo::CioSetBase(Int_t branch, Int_t crate, Int_t nstation) { return(NULL); };
void MbsSimRdo::CioCtrl(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr) {};
void MbsSimRdo::CioCtrlR2b(unsigned int * base, Int_t function, Int_t addr) {};
Int_t MbsSimRdo::CioRead(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t mask) { return(-1); };
Int_t MbsSimRdo::CioReadR2b(unsigned int * base, Int_t function, Int_t addr, Int_t mask) { return(-1); };
void MbsSimRdo::CioWrite(Int_t branch, Int_t crate, Int_t station, Int_t function, Int_t addr, Int_t data) {};
void MbsSimRdo::CioWriteR2b(unsigned int * base, Int_t function, Int_t addr, Int_t data) {};
void MbsSimRdo::RdoSetup(unsigned int *  base, Int_t function, Int_t addr) {};
void MbsSimRdo::RdoSetupVme(unsigned int * base, Int_t offset) {};
void MbsSimRdo::RdoC2M(Int_t mask, Bool_t shortflag) {};
void MbsSimRdo::RdoBlt(Int_t wc, Int_t mask, Bool_t shortflag) {};
void MbsSimRdo::RdoBltSw(Int_t wc, Int_t mask) {};
Int_t MbsSimRdo::RdoRead(Int_t mask, Bool_t shortflag) { return(-1); };
void MbsSimRdo::RdoIncrAddr(Int_t naddr) {};
void MbsSimRdo::RdoAlign(Bool_t shortflag) {};
void MbsSimRdo::Rdo2Mem(Int_t data, Int_t mask, Bool_t shortflag) {};

void f_ut_send_msg(const Char_t * hdr, const Char_t * msg, Int_t x1, Int_t x2) {
	gMrbLog->Out() << msg << endl;
	gMrbLog->Flush("MbsSimRdo", hdr);
}
