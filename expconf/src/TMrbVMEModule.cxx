//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbVMEModule.cxx
// Purpose:        MARaBOU configuration: VME modules
// Description:    Implements class methods to handle VME modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbTemplate.h"
#include "TMrbConfig.h"
#include "TMrbVMEModule.h"
#include "TMrbVMEChannel.h"

extern TMrbConfig * gMrbConfig;

ClassImp(TMrbVMEModule)

TMrbVMEModule::TMrbVMEModule(const Char_t * ModuleName, const Char_t * ModuleID,
										UInt_t BaseAddr, UInt_t AddrMod, Int_t SegSize,
										Int_t SubDevice, Int_t NofChannels, Int_t Range) :
												TMrbModule(ModuleName, ModuleID, NofChannels, Range) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbVMEModule
// Purpose:        Create a camac module
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
// Description:    Defines a new camac module having specified number of channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fModuleType.Set(gMrbConfig->GetLofModuleTypes()->FindByIndex(TMrbConfig::kModuleVME)); 	// is vme
		fBaseAddr = BaseAddr;
		fAddrModifier = AddrMod;
		fSegmentSize = SegSize;
		fSubDevice = SubDevice;
		fCrate = 0;
		gMrbConfig->SetCrateType(0, TMrbConfig::kCrateVME); // mark vme crate active
		fPosition = "C0.";
		fPosition.AppendInteger(BaseAddr, 0, 0, 16);

		for (Int_t nch = 0; nch < NofChannels; nch++) { 		// create array of params
			fChannelSpec.Add((TObject *) new TMrbVMEChannel(this, nch));
		}
	}
}

void TMrbVMEModule::Print(ostream & OutStrm, const Char_t * Prefix) {
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
	OutStrm << Prefix << "   Name          : "	<< GetName() << endl;
	OutStrm << Prefix << "   Type          : "	<< GetTitle() << endl;
	OutStrm << Prefix << "   Range         : "	<< fRange << endl;
	OutStrm << Prefix << "   Base Address  : "	<< setiosflags(ios::showbase) << setbase(16)
												<< fBaseAddr
												<< resetiosflags(ios::showbase) << setbase(10)
												<< endl;
	OutStrm << Prefix << "   Channels      : "	<< this->GetNofChannelsUsed()
												<< " out of " << this->GetNofChannels() << endl;
	found = kFALSE;
	nch = 0;
	while (nch < fNofChannels) {
		chn = (TMrbVMEChannel *) fChannelSpec[nch];
		arrayFlag = (chn->GetStatus() == TMrbConfig::kChannelArray);
		if (chn->IsUsed()) {
			if (!found) {
				OutStrm << Prefix << "                       Addr          Name       Subevent"
						<< endl;
			}
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
	}
	Template.WriteCode(RdoStrm);
	return(kTRUE);
}
