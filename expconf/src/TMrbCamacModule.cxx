//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCamacModule.cxx
// Purpose:        MARaBOU configuration: CAMAC modules
// Description:    Implements class methods to handle CAMAC modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacModule.h"
#include "TMrbCamacChannel.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCamacModule)

TMrbCamacModule::TMrbCamacModule(const Char_t * ModuleName, const Char_t * ModuleID, const Char_t * ModulePosition,
									Int_t SubDevice, Int_t NofChannels, Int_t Range) :
												TMrbModule(ModuleName, ModuleID, NofChannels, Range) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacModule
// Purpose:        Create a camac module
// Arguments:      Char_t * ModuleName      -- module name
//                 Char_t * ModuleID        -- module type
//                 Char_t * ModulePosition  -- B.C.N for this module
//                 Int_t SubDevice          -- if modules houses more than 1 dev
//                 Int_t NofChannels        -- number of channels
//                 Int_t Range              -- number of data points
// Results:        --
// Exceptions:
// Description:    Defines a new camac module having specified number of channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nch;
	TMrbConfig::EMrbCrateType cType;
	TString mType;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {

		gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, TMrbConfig::kModuleCamac);
		fModuleType.Set(TMrbConfig::kModuleCamac, mType.Data());
		fSubDevice = SubDevice; 					// number of subdevices

		fCNAF.SetBit(TMrbCNAF::kCnafBranch | TMrbCNAF::kCnafCrate | TMrbCNAF::kCnafStation, TMrbCNAF::kCnafStation); 	// defined by B.C.N, but N is needed

		if (fCNAF.Ascii2Int(ModulePosition)) {					// decode position B.C.N
				if (!gMrbConfig->CheckModuleAddress(this)) this->MakeZombie(); 	// check position
		} else {
			this->MakeZombie();
		}

		if (!this->IsZombie()) {
			if (fCNAF.Get(TMrbCNAF::kCnafBranch) == -1) {						// check branch
				fCNAF.Set(TMrbCNAF::kCnafBranch, 0);							// default is B0
			} else {
				gMrbConfig->SetMultiBorC(TMrbCNAF::kCnafBranch);				// more than 1 branch
			}
			fCrate = fCNAF.Get(TMrbCNAF::kCnafCrate);							// check crate
			if (fCrate == -1) {
				fCrate = 0;
				fCNAF.Set(TMrbCNAF::kCnafCrate, 0);								// default is C0
			} else {
				gMrbConfig->SetMultiBorC(TMrbCNAF::kCnafCrate);
			}

			cType = gMrbConfig->GetCrateType(fCrate);
			if (cType == TMrbConfig::kCrateUnused || cType == TMrbConfig::kCrateCamac) {
				gMrbConfig->SetCrateType(fCrate, TMrbConfig::kCrateCamac);			// mark camac crate active
				for (nch = 0; nch < NofChannels; nch++) { 		// create array of params
					fChannelSpec.Add(new TMrbCamacChannel(this, nch, fCNAF));
				}
			} else {
				gMrbLog->Err()	<< ModuleName << ": Crate C" << fCrate
								<< " is not a CAMAC crate (type = " << cType << ")" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			}
		}
	}
}

void TMrbCamacModule::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacModule::Print
// Purpose:        Output current module definitions
// Arguments:      ofstream & OutStrm   -- output stream
//                 Char_t * Prefix 	    -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current module definition to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	register Int_t nch;
	TMrbCamacChannel * chn;
	Bool_t found;
	Bool_t arrayFlag;
	TString cnafAscii;

	OutStrm << Prefix << endl;
	OutStrm << Prefix << "Module Definition:" << endl;
	OutStrm << Prefix << "   Name          : " << GetName() << endl;
	OutStrm << Prefix << "   Type          : " << GetTitle() << endl;
	OutStrm << Prefix << "   Range         : " << fRange << endl;
	OutStrm << Prefix << "   Location      : " << fCNAF.Int2Ascii(cnafAscii) << endl;
	OutStrm << Prefix << "   Channels      : " << this->GetNofChannelsUsed() << " out of " << this->GetNofChannels() << endl;
	found = kFALSE;
	nch = 0;
	while (nch < fNofChannels) {
		chn = (TMrbCamacChannel *) fChannelSpec[nch];
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

Bool_t TMrbCamacModule::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbReadoutTag TagIndex, TMrbTemplate & Template, const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCamacModule::MakeReadoutCode
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
		Template.InitializeCode("%C%");
		Template.Substitute("$moduleNameLC", this->GetName());
		Template.Substitute("$moduleNameUC", moduleName.Data());
		Template.Substitute("$moduleTitle", this->GetTitle());
		Template.Substitute("$modNo", this->GetSerial() - 1);
		Template.Substitute("$mnemoLC", mnemoLC);
		Template.Substitute("$mnemoUC", mnemoUC);
		Template.Substitute("$branch", 0);
		Template.Substitute("$crateNo", this->GetCrate());
		Template.Substitute("$station", this->GetCNAF(TMrbCNAF::kCnafStation));
	}
	Template.WriteCode(RdoStrm);
	return(kTRUE);
}
