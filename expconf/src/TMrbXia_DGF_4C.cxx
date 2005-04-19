//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbXia_DGF_4C.cxx
// Purpose:        MARaBOU configuration: Xia ADCs
// Description:    Implements class methods to handle modules of type XIA DGF-4C
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbXia_DGF_4C.cxx,v 1.14 2005-04-19 08:32:16 rudi Exp $       
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

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbCamacChannel.h"
#include "TMrbCamacRegister.h"
#include "TMrbXia_DGF_4C.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbXia_DGF_4C)

static Bool_t printRelease = kTRUE;

TMrbXia_DGF_4C::TMrbXia_DGF_4C(const Char_t * ModuleName, const Char_t * ModulePosition) :
									TMrbCamacModule(ModuleName, "Xia_DGF_4C", ModulePosition, 0, 4, 1 << 16) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C
// Purpose:        Create a module of type Xia 4418/V
// Arguments:      Char_t * ModuleName      -- name of camac module
//                 Char_t * ModulePosition  -- B.C.N for this module
// Results:        --
// Exceptions:
// Description:    Creates a camac module of type Xia 4418/V.
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
			SetTitle("Xia DGF-4C Digital Gamma Finder"); 	// store module type
			mTypeBits = TMrbConfig::kModuleCamac | TMrbConfig::kModuleListMode;
			mType = gMrbConfig->GetLofModuleTypes()->Pattern2String(mType, mTypeBits);
			fModuleType.Set(mTypeBits, mType.Data());
			xiaRelString = gEnv->GetValue("TMrbDGF.XiaRelease", "");
			if (xiaRelString.CompareTo("v1.0") == 0)		fRelease = 100;
			else if (xiaRelString.CompareTo("v2.0") == 0)	fRelease = 200;
			else if (xiaRelString.CompareTo("v2.70") == 0)	fRelease = 270;
			else											fRelease = 0;
			dspFormat = gEnv->GetValue("TMrbDGF.DSPCode", "");
			fDSPLoadedFromBinary = (dspFormat.Index(".bin", 0) >= 0);
			if (fDSPLoadedFromBinary) dspFormat = "binary"; else dspFormat = "ascii/exe";
			if (printRelease) {
				gMrbLog->Out()	<< "Generating code for XIA Release "
								<< xiaRelString << " (" << gEnv->GetValue("TMrbDGF.XiaDate", "") << ") ("
								<< dspFormat << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "", setblue);
			}
			printRelease = kFALSE;
						
			if (!this->ReadNameTable()) {
				gMrbLog->Err() << ModuleName << ": No param names found" << endl;
				gMrbLog->Flush(this->ClassName());
				this->MakeZombie();
			} else {
				codeFile = fModuleID.GetName();
				codeFile += ".code";
				if (LoadCodeTemplates(codeFile)) {
					this->DefineRegisters();
					this->SetBlockReadout();					// block mode only
					gMrbConfig->AddModule(this);				// append to list of modules
					gDirectory->Append(this);
					fMaxEvents = 1;
					fTraceLength = 40;
					fRunTask = 0x100;
					fSwitchBusTerm = kFALSE;
					fSwitchBusIndiv = gEnv->GetValue("TMrbDGF.TerminateSwitchBusIndividually", kFALSE);
					fSwitchBusTermIfMaster = gEnv->GetValue("TMrbDGF.TerminateSwitchBusIfMaster", kTRUE);
					fSynchWait = 1;
					fActivateUserPSA = kFALSE;
					fInSynch = 0;
				} else {
					this->MakeZombie();
				}
			}
		}
	}
}

void TMrbXia_DGF_4C::DefineRegisters() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::DefineRegisters
// Purpose:        Define camac registers
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Defines status, common threshold, lower & upper thresholds, and offset.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

}

Bool_t TMrbXia_DGF_4C::MakeReadoutCode(ofstream & RdoStrm, TMrbConfig::EMrbModuleTag TagIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::MakeReadoutCode
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
			fCodeTemplates.InitializeCode("%A%");
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			fCodeTemplates.WriteCode(RdoStrm);
			fCodeTemplates.InitializeCode("%B%");
			sPath = gSystem->ExpandPathName(gEnv->GetValue("TMrbDGF.SettingsPath", "./DgfSettings"));
			if (sPath.Index("./", 0) == 0) {
				sPath = sPath(1, sPath.Length() - 1);
				sPath.Prepend(gSystem->WorkingDirectory());
			}
			seg = this->GetClusterSegments();
			if (fSwitchBusIndiv) {
				fCodeTemplates.Substitute("$switchBusTerm", fSwitchBusTerm ? "TRUE" : "FALSE");
			} else if (fSwitchBusTermIfMaster) {
				fCodeTemplates.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "TRUE" : "FALSE");
			} else {
				fCodeTemplates.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "FALSE" : "TRUE");
			}
			fCodeTemplates.Substitute("$settingsPath", sPath.Data());
			fCodeTemplates.Substitute("$moduleNameLC", this->GetName());
			fCodeTemplates.Substitute("$moduleNameUC", moduleNameUC.Data());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$moduleNumber", this->GetSerial());
			fCodeTemplates.Substitute("$maxEvents", this->GetMaxEvents());
			fCodeTemplates.Substitute("$traceLength", this->GetTraceLength());
			fCodeTemplates.Substitute("$activatePSA", this->UserPSAIsActive() ? 1 : 0);
			fCodeTemplates.Substitute("$runTask", this->GetRunTask(), 16);
			fCodeTemplates.Substitute("$synchWait", this->GetSynchWait());
			fCodeTemplates.Substitute("$inSynch", this->GetInSynch());
			fCodeTemplates.WriteCode(RdoStrm);
			break;
		case TMrbConfig::kModuleReadModule:
			if (this->GetRelease() == 100)	fCodeTemplates.InitializeCode("%R==1.0%");
			else 							fCodeTemplates.InitializeCode("%R>=2.0%");
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
			fCodeTemplates.Substitute("$runTask", this->GetRunTask(), 16);
			fCodeTemplates.WriteCode(RdoStrm);
			break;
	}
	return(kTRUE);
}


Bool_t TMrbXia_DGF_4C::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbModuleTag TagIndex,
															TObject * Channel,
															Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::MakeReadoutCode
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

Bool_t TMrbXia_DGF_4C::CheckSubeventType(TObject * Subevent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::CheckSubeventType
// Purpose:        Check if calling subevent is applicable
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Makes sure that a subevent of type [10,2x] (DGF)
//                 is calling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbSubevent *sevt;

	sevt = (TMrbSubevent *) Subevent;
	if (sevt->GetType() != 10) return(kFALSE);
	if (sevt->GetSubtype() == 32) return(kTRUE);
	if (sevt->GetSubtype() < 21 || sevt->GetSubtype() > 29) return(kFALSE);
	else													return(kTRUE);
}

Bool_t TMrbXia_DGF_4C::ReadNameTable() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::ReadNameTable
// Purpose:        Read param names from file
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads param names and offsets from file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t pOffset;
	TString pName;
	TMrbNamedX * pKey;
	ifstream param;
	Int_t nofParams;
	TString dataPath;
	TString paramFile;
	TString pLine;

	TString charU;
	Int_t numU;

	dataPath = gEnv->GetValue("TMrbDGF.LoadPath", ".:$(MARABOU)/data/xiadgf");
	gSystem->ExpandPathName(dataPath);

	paramFile = gEnv->GetValue("TMrbDGF.ParamNames", "");
	if (paramFile.Length() == 0) {
		gMrbLog->Err() << "File name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(kFALSE);
	}

	TString fileSpec = gSystem->Which(dataPath.Data(), paramFile.Data());
	if (fileSpec.IsNull()) {
		gMrbLog->Err() << "No such file - " << paramFile << ", searched on \"" << dataPath << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(kFALSE);
	}
 
	paramFile = fileSpec;
	param.open(paramFile, ios::in);
	if (!param.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << paramFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(kFALSE);
	}
	fParamFile = paramFile;

	nofParams = 0;
	fParamNames.Delete();
	for (;;) {
		pLine.ReadLine(param, kFALSE);
		if (param.eof()) break;
		pLine = pLine.Strip(TString::kBoth);
		if (pLine.Length() == 0 || pLine(0) == '#') continue;
		istringstream str(pLine.Data());
		str >> pOffset >> pName;
		pName = pName.Strip(TString::kBoth);
		if (pName.Length() > 0) {
			if (pName.Length() == 3 && pName(0) == 'U') {
				istringstream pstr(pName.Data());
				numU = -1;
				pstr >> charU >> numU;
				if (numU == -1) continue;
			}
			pKey = new TMrbNamedX(pOffset, pName.Data());
			fParamNames.AddNamedX(pKey);
			nofParams++;
		}
	}
	param.close();
	fNofParams = nofParams;

	return(kTRUE);
}

TMrbNamedX * TMrbXia_DGF_4C::FindParam(Int_t Channel, const Char_t * ParamName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbDGFData::FindParam
// Purpose:        Search a param for a given channel
// Arguments:      Int_t Channel         -- channel number
//                 Char_t * ParamName    -- param name
// Results:        TMrbNamedX * Param       -- param name & index
// Exceptions:
// Description:    Searches a param for a given channel.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString paramName;

	paramName = ParamName;
	paramName += Channel;
	return(this->FindParam(paramName.Data()));
}

Bool_t TMrbXia_DGF_4C::MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::MakeAnalyzeCode
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
	
	TMrbNamedX * px;

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
	if (this->HasPrivateCode()) {
		tf = "Module_";
		tf += moduleNameUC;
		tf += ext.Data();
		tf += ".code";
		fileSpec = gSystem->Which(templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += ext.Data();
				tf += ".code";
				fileSpec = gSystem->Which(templatePath.Data(), tf.Data());
			}
			if (fileSpec.IsNull()) {
				tf = "Module_";
				tf += className;
				tf += ext.Data();
				tf += ".code";
				fileSpec = gSystem->Which(templatePath.Data(), tf.Data());
			}
		}
	}
	if (fileSpec.IsNull()) {
		tf = "Module";
		tf += ext.Data();
		tf += ".code";
		fileSpec = gSystem->Which(templatePath.Data(), tf.Data());
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
					case TMrbConfig::kAnaModuleSpecialEnum:
						anaTmpl.InitializeCode();
						px = this->FindParam("MODNUM");
						anaTmpl.Substitute("$parModuleNumber", (px) ? px->GetIndex() : 0);
						px = this->FindParam("MODCSRA");
						anaTmpl.Substitute("$parModCSRA", (px) ? px->GetIndex() : 0);
						px = this->FindParam("MODCSRB");
						anaTmpl.Substitute("$parModCSRB", (px) ? px->GetIndex() : 0);
						px = this->FindParam("MAXEVENTS");
						anaTmpl.Substitute("$parMaxEvents", (px) ? px->GetIndex() : 0);
						px = this->FindParam("SYNCHWAIT");
						anaTmpl.Substitute("$parSynchWait", (px) ? px->GetIndex() : 0);
						px = this->FindParam("INSYNCH");
						anaTmpl.Substitute("$parInSynch", (px) ? px->GetIndex() : 0);
						px = this->FindParam("SYNCHDONE");
						anaTmpl.Substitute("$parSynchDone", (px) ? px->GetIndex() : 0);
						px = this->FindParam("RUNTASK");
						anaTmpl.Substitute("$parRunTask", (px) ? px->GetIndex() : 0);
						px = this->FindParam("COINCPATTERN");
						anaTmpl.Substitute("$parCoincPattern", (px) ? px->GetIndex() : 0);
						px = this->FindParam("COINCWAIT");
						anaTmpl.Substitute("$parCoincWait", (px) ? px->GetIndex() : 0);
						px = this->FindParam("CONTROLTASK");
						anaTmpl.Substitute("$parControlTask", (px) ? px->GetIndex() : 0);
						px = this->FindParam("AOUTBUFFER");
						anaTmpl.Substitute("$parAddrOfOutputBuffer", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LOUTBUFFER");
						anaTmpl.Substitute("$parLengthOfOutputBuffer", (px) ? px->GetIndex() : 0);
						px = this->FindParam("TRACELENGTH0");
						anaTmpl.Substitute("$parTraceLength0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("CHANCSRA0");
						anaTmpl.Substitute("$parChanCSRA0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("CHANCSRB0");
						anaTmpl.Substitute("$parChanCSRB0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("REALTIMEA");
						anaTmpl.Substitute("$parRealTimeA", (px) ? px->GetIndex() : 0);
						px = this->FindParam("REALTIMEB");
						anaTmpl.Substitute("$parRealTimeB", (px) ? px->GetIndex() : 0);
						px = this->FindParam("REALTIMEC");
						anaTmpl.Substitute("$parRealTimeC", (px) ? px->GetIndex() : 0);
						px = this->FindParam("RUNTIMEA");
						anaTmpl.Substitute("$parRunTimeA", (px) ? px->GetIndex() : 0);
						px = this->FindParam("RUNTIMEB");
						anaTmpl.Substitute("$parRunTimeB", (px) ? px->GetIndex() : 0);
						px = this->FindParam("RUNTIMEC");
						anaTmpl.Substitute("$parRunTimeC", (px) ? px->GetIndex() : 0);
						px = this->FindParam("GSLTTIMEA");
						anaTmpl.Substitute("$parGSLTTimeA", (px) ? px->GetIndex() : 0);
						px = this->FindParam("GSLTTIMEB");
						anaTmpl.Substitute("$parGSLTTimeB", (px) ? px->GetIndex() : 0);
						px = this->FindParam("GSLTTIMEC");
						anaTmpl.Substitute("$parGSLTTimeC", (px) ? px->GetIndex() : 0);
						px = this->FindParam("NUMEVENTSA");
						anaTmpl.Substitute("$parNumEventsA", (px) ? px->GetIndex() : 0);
						px = this->FindParam("NUMEVENTSB");
						anaTmpl.Substitute("$parNumEventsB", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEA0");
						anaTmpl.Substitute("$parLiveTimeA0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEB0");
						anaTmpl.Substitute("$parLiveTimeB0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEC0");
						anaTmpl.Substitute("$parLiveTimeC0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSA0");
						anaTmpl.Substitute("$parFastPeaksA0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSB0");
						anaTmpl.Substitute("$parFastPeaksB0", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEA1");
						anaTmpl.Substitute("$parLiveTimeA1", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEB1");
						anaTmpl.Substitute("$parLiveTimeB1", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEC1");
						anaTmpl.Substitute("$parLiveTimeC1", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSA1");
						anaTmpl.Substitute("$parFastPeaksA1", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSB1");
						anaTmpl.Substitute("$parFastPeaksB1", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEA2");
						anaTmpl.Substitute("$parLiveTimeA2", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEB2");
						anaTmpl.Substitute("$parLiveTimeB2", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEC2");
						anaTmpl.Substitute("$parLiveTimeC2", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSA2");
						anaTmpl.Substitute("$parFastPeaksA2", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSB2");
						anaTmpl.Substitute("$parFastPeaksB2", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEA3");
						anaTmpl.Substitute("$parLiveTimeA3", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEB3");
						anaTmpl.Substitute("$parLiveTimeB3", (px) ? px->GetIndex() : 0);
						px = this->FindParam("LIVETIMEC3");
						anaTmpl.Substitute("$parLiveTimeC3", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSA3");
						anaTmpl.Substitute("$parFastPeaksA3", (px) ? px->GetIndex() : 0);
						px = this->FindParam("FASTPEAKSB3");
						anaTmpl.Substitute("$parFastPeaksB3", (px) ? px->GetIndex() : 0);
						anaTmpl.WriteCode(AnaStrm);
						return(kTRUE);
				}
			}
		}
	}
	return(kFALSE);
}

Bool_t TMrbXia_DGF_4C::MakeRcFile(ofstream & RcStrm, TMrbConfig::EMrbRcFileTag TagIndex, const Char_t * ResourceName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C::MakeRcFile
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
	TString fileSpec = gSystem->Which(templatePath.Data(), tf.Data());
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
				rcTmpl.Substitute("$clusterSerial", this->GetClusterSerial());
				rcTmpl.Substitute("$clusterHexNum", this->GetClusterHexNum(), 16);
				rcTmpl.Substitute("$clusterColor", this->GetClusterColor());
				rcTmpl.Substitute("$clusterSegments", this->GetClusterSegments());
				rcTmpl.Substitute("$maxEvents", this->GetMaxEvents());
				rcTmpl.Substitute("$traceLength", this->GetTraceLength());
				rcTmpl.Substitute("$activatePSA", this->UserPSAIsActive() ? "TRUE" : "FALSE");
				rcTmpl.Substitute("$runTask", this->GetRunTask());
				rcTmpl.Substitute("$synchWait", this->GetSynchWait());
				rcTmpl.Substitute("$inSynch", this->GetInSynch());
				rcTmpl.Substitute("$isActive", this->IsActive() ? "TRUE" : "FALSE");
				seg = this->GetClusterSegments();
				if (fSwitchBusIndiv) {
					rcTmpl.Substitute("$switchBusTerm", fSwitchBusTerm ? "TRUE" : "FALSE");
				} else if (fSwitchBusTermIfMaster) {
					rcTmpl.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "TRUE" : "FALSE");
				} else {
					rcTmpl.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "FALSE" : "TRUE");
				}
				rcTmpl.WriteCode(RcStrm);
				return(kTRUE);
			}
		}
	}
	return(kTRUE);
}
