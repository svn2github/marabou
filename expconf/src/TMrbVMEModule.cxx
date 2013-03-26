//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVMEModule.cxx
// Purpose:        MARaBOU configuration: VME modules
// Description:    Implements class methods to handle VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbVMEModule.cxx,v 1.13 2011-05-18 11:04:49 Marabou Exp $
// Date:
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"

#include "TMrbTemplate.h"
#include "TMrbConfig.h"
#include "TMrbLogger.h"
#include "TMrbVMEModule.h"
#include "TMrbVMEChannel.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbVMEModule)

TMrbVMEModule::TMrbVMEModule(const Char_t * ModuleName, const Char_t * ModuleID,
										UInt_t BaseAddr, UInt_t AddrMod, Int_t SegSize,
										Int_t SubDevice, Int_t NofChannels, Int_t Range) :
												TMrbModule(ModuleName, ModuleID, NofChannels, Range) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEModule
// Purpose:        Create a vme module
// Arguments:      Char_t * ModuleName      -- module name
//                 Char_t * ModuleID        -- module type
//                 UInt_t BaseAddr          -- vme base address
//                 UInt_t AddrMod           -- address modifier
//                 Int_t SegSize            -- size of segment
//                 Int_t SubDevice          -- subdevice
//                 Int_t NofChannels        -- number of channels
//                 Int_t Range              -- number of data points
// Results:        --
// Exceptions:
// Description:    Defines a new vme module having specified number of channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	Bool_t verboseMode = gEnv->GetValue("TMrbConfig.VerboseMode", kFALSE);

	if (!this->IsZombie()) {
		fModuleType.Set(gMrbConfig->GetLofModuleTypes()->FindByIndex(TMrbConfig::kModuleVME)); 	// is vme
		fBaseAddr = BaseAddr;
		fAddrModifier = AddrMod;
		TString am;
		if (fAddrModifier == 0) {
			if (BaseAddr & 0xFF000000) {
				fAddrModifier = 0x09;
				am = "A32";
			} else {
				fAddrModifier = 0x39;
				am = "A24";
			}
			if (verboseMode) {
				gMrbLog->Out() << ModuleName << ": Using address mode " << am << " at addr 0x" << setbase(16) << BaseAddr << setbase(10) << endl;
				gMrbLog->Flush(this->ClassName());
			}
		}
		fSegmentSize = SegSize;

		fVMEMapping = TMrbConfig::kVMEMappingDirect | TMrbConfig::kVMEMappingStatic | TMrbConfig::kVMEMappingDynamic;
		fVMEMapping = gEnv->GetValue("TMrbConfig.VMEMapping",  (Int_t) fVMEMapping);
		TString moduleNameUC = ModuleName;
		moduleNameUC(0,1).ToUpper();
		fVMEMapping = gEnv->GetValue(Form("TMrbConfig.%s.VMEMapping", moduleNameUC.Data()), (Int_t) fVMEMapping);
		
		
		fSubDevice = SubDevice;
		fCrate = 0;
		gMrbConfig->SetCrateType(0, TMrbConfig::kCrateVME); // mark vme crate active
		fPosition = "C0.";
		fPosition += Form("%#0lx", BaseAddr);

		if (!this->IsZombie()) {
			for (Int_t nch = 0; nch < NofChannels; nch++) { 		// create array of params
				TMrbVMEChannel * vc = new TMrbVMEChannel(this, nch);
				fChannelSpec.Add(vc);
				vc->SetOffset(nch);
			}
		}
	}
}

void TMrbVMEModule::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEModule::Print
// Purpose:        Output current module definitions
// Arguments:      ofstream & OutStrm   -- output stream
//                 Char_t * Prefix 	    -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current module definition to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register Int_t nch;
	TMrbVMEChannel * chn;
	Bool_t found;
	Bool_t arrayFlag;

	OutStrm << Prefix << endl;
	OutStrm << Prefix << "Module Definition:" << endl;
	OutStrm << Prefix << "   Name              : "	<< GetName() << endl;
	OutStrm << Prefix << "   Type              : "	<< GetTitle() << endl;
	OutStrm << Prefix << "   Range             : "	<< fRange << endl;
	OutStrm << Prefix << "   Base Address      : "	<< setiosflags(ios::showbase) << setbase(16)
												<< fBaseAddr
												<< resetiosflags(ios::showbase) << setbase(10)
												<< endl;
	OutStrm << Prefix << "   Channels          : "	<< this->GetNofChannelsUsed()
												<< " out of " << this->GetNofChannels() << endl;
	found = kFALSE;
	nch = 0;
	while (nch < fNofChannels) {
		chn = (TMrbVMEChannel *) fChannelSpec[nch];
		arrayFlag = (chn->GetStatus() == TMrbConfig::kChannelArray);
		if (chn->IsUsed()) {
			if (!found) OutStrm << Prefix << Form("%-23s%-18s%-15s%-15s", "", "Addr", "Name", "Subevent") << endl;
			found = kTRUE;
			chn->Print(OutStrm, arrayFlag, kTRUE, Prefix);
		}
		if (arrayFlag) nch += chn->GetIndexRange(); else nch++;
	}
}

Bool_t TMrbVMEModule::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEModule::MakeReadoutCode
// Purpose:        Generate a piece of readout code
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes readout code needed for this module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC, moduleName;

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	moduleName = this->GetName();
	moduleName(0,1).ToUpper();

	if (TagIndex == TMrbConfig::kRdoInitPointers) {
		if (this->GetNofSubDevices() <= 1) {
			Template.InitializeCode("%V%");
		} else {
			Template.InitializeCode("%VS%");
			Template.Substitute("$subDevice", this->GetSubDevice());
		}
		Template.Substitute("$moduleNameLC", this->GetName());
		Template.Substitute("$moduleNameUC", moduleName.Data());
		Template.Substitute("$moduleTitle", this->GetTitle());
		Template.Substitute("$modNo", this->GetSerial() - 1);
		Template.Substitute("$mnemoLC", mnemoLC);
		Template.Substitute("$mnemoUC", mnemoUC);
		Template.Substitute("$crate", this->GetCrate());
		Template.Substitute("$baseAddr", (Int_t) this->GetBaseAddr(), 16);
		Template.Substitute("$addrMod", (Int_t) this->GetAddrModifier(), 16);
		Template.Substitute("$segSize", this->GetSegmentSize(), 16);
		Template.Substitute("$mapping", this->GetVMEMapping(), 16);
		Template.WriteCode(RdoStrm);
	}
	return(kTRUE);
}
