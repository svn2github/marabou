//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbModule.cxx
// Purpose:        MARaBOU configuration: modules
// Description:    Implements class methods to handle modules
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

#include "TEnv.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"

#include "TMrbCamacModule.h"
#include "TMrbVMEModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbModuleRegister.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbModule)

TMrbModule::TMrbModule(const Char_t * ModuleName, const Char_t * ModuleID, Int_t NofChannels, Int_t Range) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule
// Purpose:        Create a camac module
// Arguments:      Char_t * ModuleName      -- module name
//                 Char_t * ModuleID        -- module type
//                 Int_t NofChannels        -- number of channels
//                 Int_t Range              -- number of data points
// Results:        --
// Exceptions:
// Description:    Defines a new module having specified number of channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;
	TMrbNamedX * id;
	TString pos;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (gMrbConfig == NULL) {
		gMrbLog->Err() << "No config defined" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (gMrbConfig->NameNotLegal("module", ModuleName)) {
		this->MakeZombie();
	} else {
		if ((module = (TMrbModule *) gMrbConfig->FindModule(ModuleName)) != NULL) {
			gMrbLog->Err() << ModuleName << ": Name already in use - " << module->GetName();
			if (module->IsCamac()) {
				gMrbLog->Err() << " in " << ((TMrbCamacModule *) module)->GetPosition();
			} else if (module->IsVME()) {
				gMrbLog->Err() << " in "	<< setiosflags(ios::showbase) << setbase(16)
											<< ((TMrbVMEModule *) module)->GetBaseAddr()
											<< resetiosflags(ios::showbase) << setbase(10);
			}
			gMrbLog->Err() << " (" << module->GetTitle() << ")" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		}

        if ((id = gMrbConfig->fLofModuleIDs.FindByName(ModuleID)) == NULL) {
            gMrbLog->Err() << ModuleName << ": Unknown ID - " << ModuleID << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
        }

		if (!this->IsZombie()) {
			fModuleID.Set(id);
			SetName(ModuleName);

			fDataType = gMrbConfig->fLofDataTypes.FindByIndex(TMrbConfig::kDataUShort);	// data type is short per default
			fCrate = -1;	 								// crate number
			fNofChannels = NofChannels; 	 				// number of channels available
			fRange = Range; 								// range
			fPointsPerBin = 1;								// bin size defaults to 1
			fBinRange = Range;								// number of channels
			fBlockReadout = kFALSE; 						// turn off block mode
			fIsActive = kTRUE;								// module is active
			fExcludeFromReadout = kFALSE;					// module will be read out
			fNofShortsPerChannel = 1;						// 16 bit words per channel
			fTimeOffset = 0;								// time offset
			if (fModuleID.GetIndex() != TMrbConfig::kModuleSoftModule) fSerial = gMrbConfig->AssignModuleSerial();		// assign a unique module number
			fHistosToBeAllocated = kTRUE;					// create histograms for each channel
		}
	}
}

Int_t TMrbModule::GetNofChannelsUsed() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::GetNofChannelsUsed
// Purpose:        Calculate number of channels in use
// Arguments:      --
// Results:        Int_t NofUsed  -- number of channels used
// Exceptions:
// Description:    Calculates number of active channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TMrbModuleChannel * chn;
	Int_t nofChannelsUsed;

	nofChannelsUsed = 0;
	for (i = 0; i < fNofChannels; i++) {
		chn = (TMrbModuleChannel *) fChannelSpec[i];
		if (chn->IsUsed()) nofChannelsUsed++;
	}
	return(nofChannelsUsed);
}

UInt_t TMrbModule::GetPatternOfChannelsUsed() const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::GetPatternOfChannelsUsed
// Purpose:        Return pattern of channels in use
// Arguments:      --
// Results:        UInt_t PofChannelsUsed  -- pattern of channels used
// Exceptions:
// Description:    Build bit pattern of active channels
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	TMrbModuleChannel * chn;
	UInt_t pofChannelsUsed, bit;

	pofChannelsUsed = 0;
	bit = 1;
	for (i = 0; i < fNofChannels; i++) {
		chn = (TMrbModuleChannel *) fChannelSpec[i];
		if (chn->IsUsed()) pofChannelsUsed |= bit;
		bit <<= 1;
	}
	return(pofChannelsUsed);
}

Bool_t TMrbModule::SetBlockReadout(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::SetBlockReadout
// Purpose:        Enable/disable block readout
// Arguments:      Bool_t Flag    -- block mode flag
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Enables block mode if channels are allocated continuously.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	Bool_t isUsed;
	Int_t nofUsed;
	TMrbModuleChannel * chn;

	if (Flag) { 				// enable block mode
		if (this->HasRandomReadout()) {
			nofUsed = 0;
			isUsed = kTRUE;
			for (i = 0; i < fNofChannels; i++) {
				chn = (TMrbModuleChannel *) fChannelSpec[i];
				if (chn->IsUsed()) {
					if (!isUsed) {
						gMrbLog->Err()
								<< this->GetName()
								<< ": can't set block mode - channels have to be contiguous starting with chn0"
								<< endl;
						gMrbLog->Flush(this->ClassName(), "SetBlockReadout");
						fBlockReadout = kFALSE;
						return(kFALSE);
					}
					nofUsed++;
				} else isUsed = kFALSE;
			}
			if (nofUsed == 0) {
				gMrbLog->Err() << this->GetName() << ": No channels in use" << endl;
				gMrbLog->Flush(this->ClassName(), "SetBlockReadout");
				fBlockReadout = kFALSE;
				return(kFALSE);
			}
		}
		fBlockReadout = kTRUE;
		return(kTRUE);
	} else {
		if (this->HasRandomReadout()) {
			fBlockReadout = kFALSE;
			return(kTRUE);
		} else {
			gMrbLog->Err() << this->GetName() << ": Can't be read randomly - block readout mandatory" << endl;
			gMrbLog->Flush(this->ClassName(), "SetBlockReadout");
			return(kFALSE);
		}
	}
}

void TMrbModule::SetType(UInt_t ModuleType, Bool_t OrFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::SetType
// Purpose:        Set module type
// Arguments:      UInt_t ModuleType    -- type bits
//                 Bool_t OrFlag        -- kTRUE if "or"
// Results:        --
// Exceptions:
// Description:    Sets module type bits.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mType;

	if (OrFlag) ModuleType |= this->GetType()->GetIndex();
	gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, ModuleType);
	fModuleType.Set(ModuleType, mType.Data());
}

TObject * TMrbModule::GetChannel(Int_t Channel) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::GetChannel
// Purpose:        Return addr of specified channel
// Arguments:      Int_t Channel     -- channel number
// Results:        TObject * ChAddr  -- channel addr or NULL on error
// Exceptions:     Channel out of range
// Description:    Checks if channel number is legal and return addr of class object
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (Channel < 0 || Channel >= fNofChannels) return(NULL);
	return(fChannelSpec[Channel]);
}

Bool_t TMrbModule::Set(const Char_t * RegName, Int_t Value, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Set
// Purpose:        Set register values
// Arguments:      Char_t * Regname  -- register name
//                 Int_t Value       -- value to be set
//                 Int_t Channel     -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified channel.
//                 Channel = -1: set values for all channels
//                               or set channel of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->IsCommon() ? rp->Set(Value) : rp->Set(-1, Value));
	} else if (this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	} else {
		return(rp->Set(Channel, Value));
	}
}

Bool_t TMrbModule::Set(Int_t RegIndex, Int_t Value, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Set
// Purpose:        Set register values
// Arguments:      Int_t RegIndex          -- register index
//                 Int_t Value             -- value to be set
//                 Int_t Channel           -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified channel.
//                 Channel = -1: set values for all channels
//                               or set channel of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegIndex)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->IsCommon() ? rp->Set(Value) : rp->Set(-1, Value));
	} else if (this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	} else {
		return(rp->Set(Channel, Value));
	}
}

Bool_t TMrbModule::Set(const Char_t * RegName, const Char_t * Value, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Set
// Purpose:        Set register values
// Arguments:      Char_t * Regname  -- register name
//                 Char_t * Value    -- mnemonic value to be set
//                 Int_t Channel     -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified channel.
//                 Channel = -1: set values for all channels
//                               or set channel of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->IsCommon() ? rp->Set(Value) : rp->Set(-1, Value));
	} else if (this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	} else {
		return(rp->Set(Channel, Value));
	}
}

Bool_t TMrbModule::Set(Int_t RegIndex, const Char_t * Value, Int_t Channel) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Set
// Purpose:        Set register values
// Arguments:      Int_t RegIndex          -- register index
//                 Char_t * Value          -- mnemonic value to be set
//                 Int_t Channel           -- channel number
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified channel.
//                 Channel = -1: set values for all channels
//                               or set channel of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegIndex)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->IsCommon() ? rp->Set(Value) : rp->Set(-1, Value));
	} else if (this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	} else {
		return(rp->Set(Channel, Value));
	}
}

Int_t TMrbModule::Get(const Char_t * RegName, Int_t Channel) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Get
// Purpose:        Get register values
// Arguments:      Char_t * RegName  -- register name
//                 Int_t Channel     -- channel number
// Results:        Int_t Value       -- register value
// Exceptions:
// Description:    Gets register values of specified channel
//                 Channel = -1: set value of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->Get());
	} else if (this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	} else {
		return(rp->Get(Channel));
	}
}

Int_t TMrbModule::Get(Int_t RegIndex, Int_t Channel) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::Get
// Purpose:        Get register values
// Arguments:      Int_t RegIndex          -- register index
//                 Int_t Channel           -- channel number
// Results:        Int_t Value             -- register value
// Exceptions:
// Description:    Gets register values of specified channel
//                 Channel = -1: set value of a common register
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;

	if ((kp = this->FindRegister(RegIndex)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register index - " << RegIndex << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(kFALSE);
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	if (Channel == -1) {
		return(rp->Get());
	} else if(this->GetChannel(Channel) == NULL) {
		gMrbLog->Err() << this->GetName() << ": Illegal channel - " << Channel << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(kFALSE);
	} else {
		return(rp->Get(Channel));
	}
}

Bool_t TMrbModule::MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::MakeAnalyzeCode
// Purpose:        Write a piece of code for data analysis
// Arguments:      ofstream & AnaStrm       -- file output stream
//                 EMrbAnalyzeTag TagIndex  -- index of tag word from template file
//                 Char_t * Extension       -- file extensions to be appended (.h or .cxx)
// Results:
// Exceptions:
// Description:    Writes code needed for data analysis.
//                 Searches for following files:
//                      (1)  if module has private code
//                      (1a)    Module_<Name>.<Extension>.code
//                      (1b)    <PrivateCodeFile>.<Extension>.code
//                      (1c)    Module_<Class>.<Extension>.code
//                      (2)  standard:
//                              Module.<Extension>.code
//
//                 This method is optional and doesn't generate an error
//                 if no template file has been found.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC;
	TString moduleNameLC, moduleNameUC;

	Char_t * fp;
	TString templatePath;
	TString anaTemplateFile;

	TString className;
	TString tf;
	const Char_t * pcf;

	TString line;
	TMrbTemplate anaTmpl;
	TMrbNamedX * analyzeTag;
	
	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	moduleNameLC = this->GetName();
	moduleNameUC = moduleNameLC;
	moduleNameUC.ToUpper();

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	className = this->ClassName();
	className.ReplaceAll("TMrb", "");
	className(0,1).ToUpper();

	TString ext = Extension;
	if (ext(0) != '.') ext.Prepend("_");

	fp = NULL;
	if (this->HasPrivateCode()) {
		tf = "Module_";
		tf += moduleNameUC;
		tf += ext.Data();
		tf += ".code";
		fp = gSystem->Which(templatePath.Data(), tf.Data());
		if (fp == NULL) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += ext.Data();
				tf += ".code";
				fp = gSystem->Which(templatePath.Data(), tf.Data());
			}
			if (fp == NULL) {
				tf = "Module_";
				tf += className;
				tf += ext.Data();
				tf += ".code";
				fp = gSystem->Which(templatePath.Data(), tf.Data());
			}
		}
	}
	if (fp == NULL) {
		tf = "Module";
		tf += ext.Data();
		tf += ".code";
		fp = gSystem->Which(templatePath.Data(), tf.Data());
	}
	if (fp == NULL) return(kTRUE);
	
	if (verboseMode) {
		gMrbLog->Out()  << "[" << moduleNameLC << "] Using template file " << fp << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}
	
	anaTemplateFile = fp;

	if (!anaTmpl.Open(anaTemplateFile, &gMrbConfig->fLofAnalyzeTags)) return(kFALSE);

	for (;;) {
		analyzeTag = anaTmpl.Next(line);
		if (anaTmpl.IsEof()) break;
		if (anaTmpl.IsError()) continue;
		if (anaTmpl.Status() & TMrbTemplate::kNoTag) {
			if (line.Index("//-") != 0) AnaStrm << line << endl;
		} else if (TagIndex == analyzeTag->GetIndex()) {
			if (!gMrbConfig->ExecUserMacro(&AnaStrm, this, analyzeTag->GetName())) {
				switch (TagIndex) {
					case TMrbConfig::kAnaIncludesAndDefs:
						anaTmpl.InitializeCode();
						anaTmpl.Substitute("$moduleName", this->GetName());
						anaTmpl.Substitute("$moduleTitle", this->GetName());
						anaTmpl.Substitute("$mnemoLC", mnemoLC);
						anaTmpl.Substitute("$mnemoUC", mnemoUC);
						anaTmpl.WriteCode(AnaStrm);
						return(kTRUE);
				}
			}
		}
	}
	return(kFALSE);
}

Bool_t TMrbModule::LoadCodeTemplates(const Char_t * TemplateFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::LoadCodeTemplates
// Purpose:        Load code from template file
// Arguments:      TString & TemplateFile   -- file name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads code from template file for further expansion
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString templatePath;
	Char_t * fp;
	TString codeFile;

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/modules");

	gSystem->ExpandPathName(templatePath);
	fp = gSystem->Which(templatePath.Data(), TemplateFile);
	if (fp == NULL) {
		gMrbLog->Err() << "No such file - " << templatePath << "/" << TemplateFile << endl;
		gMrbLog->Flush(this->ClassName(), "LoadCodeTemplates");
		return(kFALSE);
	}
	codeFile = fp;

	if (!fCodeTemplates.ReadCodeFromFile(codeFile, &gMrbConfig->fLofModuleTags)) {
		gMrbLog->Err() << TemplateFile << " - Error while loading code" << endl;
		gMrbLog->Flush(this->ClassName(), "LoadCodeTemplates");
		return(kFALSE);
	}

	return(kTRUE);
}

void TMrbModule::DeleteRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::DeleteRegisters()
// Purpose:        Empty list of registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Removes registers.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;
	kp = (TMrbNamedX *) fLofRegisters.First();
	while (kp) {
		rp = (TMrbModuleRegister *) kp->GetAssignedObject();
		rp->Delete();
		delete rp;
		kp = (TMrbNamedX *) fLofRegisters.After(kp);
	}
	fLofRegisters.Delete();
}

void TMrbModule::PrintRegister(ostream & OutStrm, const Char_t * RegName, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbModule::PrintRegister()
// Purpose:        Print register settings
// Arguments:      ostream * OutStrm    -- output stream
//                 Char_t * RegName     -- register name
//                 Char_t * Prefix      -- prefix
// Results:        --
// Exceptions:
// Description:    Output register settings
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * kp;
	TMrbModuleRegister * rp;
	TString prefix;

	if ((kp = this->FindRegister(RegName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such register - " << RegName << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRegister");
		return;
	}

	rp = (TMrbModuleRegister *) kp->GetAssignedObject();

	OutStrm << Prefix << "Module " << this->GetName() << " -" << endl;

	prefix = Prefix;
	prefix.Append("   ");
	rp->Print(OutStrm, prefix.Data());
}
