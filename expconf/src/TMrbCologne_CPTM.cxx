//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbCologne_CPTM.cxx
// Purpose:        MARaBOU configuration: Xia ADCs
// Description:    Implements class methods to handle modules of type XIA DGF-4C
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbCologne_CPTM.cxx,v 1.3 2005-12-13 12:46:45 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"
#include "TEnv.h"

#include "TMrbSystem.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbCamacRegister.h"
#include "TMrbCologne_CPTM.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbCologne_CPTM)

TMrbCologne_CPTM::TMrbCologne_CPTM(const Char_t * ModuleName, const Char_t * ModulePosition) :
									TMrbCamacModule(ModuleName, "Cologne_CPTM", ModulePosition, 0, 1, 1 << 16) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM
// Purpose:        Create a module of type C_PTM
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type C_PTM.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString codeFile;
	UInt_t mTypeBits;
	TString mType;
	TString xiaRelString;
	TString dspFormat;
	
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (!this->IsZombie()) {
		if (gMrbConfig == NULL) {
			gMrbLog->Err() << "No config defined" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else if (gMrbConfig->FindModule(ModuleName)) {
			gMrbLog->Err() << ModuleName << ": Module name already in use" << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle("C_PTM Clock and Programmable Trigger Module"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			mType = gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());

			codeFile = fModuleID.GetName();
			codeFile += ".code";
			if (LoadCodeTemplates(codeFile)) {
				gMrbConfig->AddModule(this);				// append to list of modules
				gDirectory->Append(this);
			} else {
				this->MakeZombie();
			}
		}
	}
}

void TMrbCologne_CPTM::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::DefineRegisters
// Purpose:        Define camac registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, common threshold, lower & upper thresholds, and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

}

Bool_t TMrbCologne_CPTM::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::MakeReadoutCode
// Purpose:        Write a piece of code for a DGF module
// Arguments:      ofstream & RdoStrm         -- file output stream
//                 EMrbModuleTag TagIndex     -- index of tag word taken from template file
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a DGF-4C module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString mnemoLC, mnemoUC;
	TString moduleNameUC;
	TString sPath, seg;	
	TString pos;
	
	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	moduleNameUC = this->GetName();
	moduleNameUC(0,1).ToUpper();

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kModuleDefs:
		case TMrbConfig::kModuleInitCommonCode:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleInitModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$moduleNumber", this->GetSerial());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleReadModule:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$moduleNumber", this->GetSerial());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleClearModule:
		case TMrbConfig::kModuleFinishReadout:
		case TMrbConfig::kModuleStartAcquisition:
		case TMrbConfig::kModuleStopAcquisition:
		case TMrbConfig::kModuleStartAcquisitionGroup:
		case TMrbConfig::kModuleStopAcquisitionGroup:
		case TMrbConfig::kModuleUtilities:
		case TMrbConfig::kModuleDefineGlobals:
		case TMrbConfig::kModuleDefineGlobalsOnce:
		case TMrbConfig::kModuleDefineLocalVarsInit:
		case TMrbConfig::kModuleDefineLocalVarsReadout:
		case TMrbConfig::kModuleDefinePrototypes:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.Substitute("$nofModules", gMrbConfig->GetNofModules());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbCologne_CPTM::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::MakeReadoutCode
// Purpose:        Write a piece of code for a DGF module
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbModuleTag TagIndex       -- index of tag word taken from template file
//                 TObject * Channel            -- channel
//                 Int_t Value                  -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Writes code for readout of a DGF-4C module.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbCamacChannel * chn;

	TString mnemoLC, mnemoUC;
	TString moduleNameUC;
	moduleNameUC = this->GetName();
	moduleNameUC(0,1).ToUpper();

	mnemoLC = this->GetMnemonic();
	mnemoUC = mnemoLC;
	mnemoUC.ToUpper();

	chn = (TMrbCamacChannel *) Channel;

	if (!fCodeTemplates.FindCode(TagIndex)) {
		gMrbLog->Err()	<< "No code loaded for tag "
						<< gMrbConfig->fLofModuleTags.FindByIndex(TagIndex)->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "MakeReadoutCode");
		return(kFALSE);
	}

	switch (TagIndex) {
		case TMrbConfig::kModuleInitChannel:
			break;
		case TMrbConfig::kModuleWriteSubaddr:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$subaddr", chn->GetAddr() + 1);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleSetupReadout:
		case TMrbConfig::kModuleReadChannel:
		case TMrbConfig::kModuleIncrementChannel:
		case TMrbConfig::kModuleSkipChannels:
		case TMrbConfig::kModuleStoreData:
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$chnName", chn->GetName());
			fCodeTemplates.Substitute("$chnNo", chn->GetAddr());
			fCodeTemplates.Substitute("$data", Value);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}

Bool_t TMrbCologne_CPTM::CheckSubeventType(TObject * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Makes sure that a subevent of type [10,61] (CPTM) is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent *sevt;

	sevt = (TMrbSubevent *) Subevent;
	if (sevt->GetType() != 10) return(kFALSE);
	if (sevt->GetSubtype() != 61) return(kFALSE);
	return(kTRUE);
}

Bool_t TMrbCologne_CPTM::MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::MakeAnalyzeCode
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

	TString fileSpec = "";
	TMrbSystem ux;
	TObjArray err;
	err.Delete();
	if (this->HasPrivateCode()) {
		tf = "Module_";
		tf += moduleNameUC;
		tf += ext.Data();
		tf += ".code";
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
		if (fileSpec.IsNull()) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += ext.Data();
				tf += ".code";
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
				if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
			if (fileSpec.IsNull()) {
				tf = "Module_";
				tf += className;
				tf += ext.Data();
				tf += ".code";
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
				if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
		}
		if (fileSpec.IsNull() && verboseMode) {
			gMrbLog->Wrn() << "Can't find code file(s):";
			for (Int_t i = 0; i < err.GetLast(); i++) {
				gMrbLog->Wrn() << ((i == 0) ? " neither " : " nor ");
				gMrbLog->Wrn() << ((TObjString *) err[i])->GetString();
			}
			gMrbLog->Wrn() << endl;
			gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		}
	}
	if (fileSpec.IsNull()) {
		tf = "Module";
		tf += ext.Data();
		tf += ".code";
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
	}
	if (fileSpec.IsNull()) return(kTRUE);
	
	if (verboseMode) {
		gMrbLog->Out()  << "[" << moduleNameLC << "] Using template file " << fileSpec << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
	}
	
	anaTemplateFile = fileSpec;

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

Bool_t TMrbCologne_CPTM::MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbCologne_CPTM::MakeRcFile
// Purpose:        Generate module-specific entries to resource file
// Arguments:      ofstream & RcStrm           -- where to output code
//                 EMrbRcFileTag TagIndex      -- tag word index
//                 Char_t * ResourceName       -- resource prefix
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates module-specific entries to resource file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString line;

	TMrbNamedX * rcFileTag;

	TString rcTemplateFile;
	TString templatePath;
	TString moduleNameUC;
	TString iniTag;
	TString seg;		
	TString tf;
	
	TMrbTemplate rcTmpl;
	
	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetRcFileOptions() & TMrbConfig::kRcOptVerbose) != 0);

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	tf = "Module_";
	tf += this->ClassName();
	tf.ReplaceAll("TMrb", "");
	tf += ".rc.code";
	TString fileSpec;
	TMrbSystem ux;
	ux.Which(fileSpec, templatePath.Data(), tf.Data());
	if (fileSpec.IsNull()) return(kTRUE);

	if (verboseMode) {
		gMrbLog->Out()  << "[" << this->GetName() << "] Using template file " << fileSpec << endl;
		gMrbLog->Flush(this->ClassName(), "MakeRcFile");
	}
	
 	rcTemplateFile = fileSpec;

	if (!rcTmpl.Open(rcTemplateFile, &gMrbConfig->fLofRcFileTags)) return(kFALSE);

	moduleNameUC = this->GetName();
	moduleNameUC(0,1).ToUpper();

	iniTag = (gMrbConfig->GetRcFileOptions() & TMrbConfig::kRcOptByName) ? "%NAM%" : "%NUM%";
	
	for (;;) {
		rcFileTag = rcTmpl.Next(line);
		if (rcTmpl.IsEof()) break;
		if (rcTmpl.IsError()) continue;
		if (rcTmpl.Status() & TMrbTemplate::kNoTag) continue;
		if (TagIndex == rcFileTag->GetIndex()) {
			if (!gMrbConfig->ExecUserMacro(&RcStrm, this, rcFileTag->GetName())) {
				rcTmpl.InitializeCode(iniTag.Data());
				rcTmpl.Substitute("$resource", ResourceName);
				rcTmpl.Substitute("$moduleNameLC", this->GetName());
				rcTmpl.Substitute("$moduleNameUC", moduleNameUC.Data());
				rcTmpl.WriteCode(RcStrm);
				return(kTRUE);
			}
		}
	}
	return(kTRUE);
}
