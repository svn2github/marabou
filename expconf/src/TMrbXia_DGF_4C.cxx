//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbXia_DGF_4C.cxx
// Purpose:        MARaBOU configuration: Xia ADCs
// Description:    Implements class methods to handle modules of type XIA DGF-4C
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <fstream.h>

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
									TMrbCamacModule(ModuleName, "Xia_DGF_4C", ModulePosition, 0, 4, 1 << 15) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXia_DGF_4C
// Purpose:        Create an ADC of type Xia 4418/V
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
					fTraceLength = 0;
					fRunTask = 0x100;
					fSwitchBusTerm = kFALSE;
					fSwitchBusIndiv = kFALSE;
					fSynchWait = 1;
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

	TMrbNamedX * px;

	TString mnemoLC, mnemoUC;
	TString moduleNameUC;
	TString sPath, seg;	
	
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
			fCodeTemplates.InitializeCode();
			fCodeTemplates.Substitute("$moduleName", this->GetName());
			fCodeTemplates.Substitute("$moduleTitle", this->GetTitle());
			fCodeTemplates.Substitute("$mnemoLC", mnemoLC);
			fCodeTemplates.Substitute("$mnemoUC", mnemoUC);
			fCodeTemplates.Substitute("$modulePosition", this->GetPosition());
			px = this->FindParam("MODNUM");
			fCodeTemplates.Substitute("$parModuleNumber", (px) ? px->GetIndex() : 0);
			px = this->FindParam("MODCSRA");
			fCodeTemplates.Substitute("$parModCSRA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("MAXEVENTS");
			fCodeTemplates.Substitute("$parMaxEvents", (px) ? px->GetIndex() : 0);
			px = this->FindParam("SYNCHWAIT");
			fCodeTemplates.Substitute("$parSynchWait", (px) ? px->GetIndex() : 0);
			px = this->FindParam("INSYNCH");
			fCodeTemplates.Substitute("$parInSynch", (px) ? px->GetIndex() : 0);
			px = this->FindParam("RUNTASK");
			fCodeTemplates.Substitute("$parRunTask", (px) ? px->GetIndex() : 0);
			px = this->FindParam("COINCPATTERN");
			fCodeTemplates.Substitute("$parCoincPattern", (px) ? px->GetIndex() : 0);
			px = this->FindParam("COINCWAIT");
			fCodeTemplates.Substitute("$parCoincWait", (px) ? px->GetIndex() : 0);
			px = this->FindParam("CONTROLTASK");
			fCodeTemplates.Substitute("$parControlTask", (px) ? px->GetIndex() : 0);
			px = this->FindParam("AOUTBUFFER");
			fCodeTemplates.Substitute("$parAddrOfOutputBuffer", (px) ? px->GetIndex() : 0);
			px = this->FindParam("LOUTBUFFER");
			fCodeTemplates.Substitute("$parLengthOfOutputBuffer", (px) ? px->GetIndex() : 0);
			px = this->FindParam("TRACELENGTH0");
			fCodeTemplates.Substitute("$parTraceLength0", (px) ? px->GetIndex() : 0);
			px = this->FindParam("CHANCSRA0");
			fCodeTemplates.Substitute("$parChanCSRA0", (px) ? px->GetIndex() : 0);
			px = this->FindParam("REALTIMEA");
			fCodeTemplates.Substitute("$parRealTimeA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("REALTIMEB");
			fCodeTemplates.Substitute("$parRealTimeB", (px) ? px->GetIndex() : 0);
			px = this->FindParam("REALTIMEC");
			fCodeTemplates.Substitute("$parRealTimeC", (px) ? px->GetIndex() : 0);
			px = this->FindParam("RUNTIMEA");
			fCodeTemplates.Substitute("$parRunTimeA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("RUNTIMEB");
			fCodeTemplates.Substitute("$parRunTimeB", (px) ? px->GetIndex() : 0);
			px = this->FindParam("RUNTIMEC");
			fCodeTemplates.Substitute("$parRunTimeC", (px) ? px->GetIndex() : 0);
			px = this->FindParam("GSLTTIMEA");
			fCodeTemplates.Substitute("$parGSLTTimeA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("GSLTTIMEB");
			fCodeTemplates.Substitute("$parGSLTTimeB", (px) ? px->GetIndex() : 0);
			px = this->FindParam("GSLTTIMEC");
			fCodeTemplates.Substitute("$parGSLTTimeC", (px) ? px->GetIndex() : 0);
			px = this->FindParam("NUMEVENTSA");
			fCodeTemplates.Substitute("$parNumEventsA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("NUMEVENTSB");
			fCodeTemplates.Substitute("$parNumEventsB", (px) ? px->GetIndex() : 0);
			px = this->FindParam("LIVETIMEA0");
			fCodeTemplates.Substitute("$parLiveTimeA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("LIVETIMEB0");
			fCodeTemplates.Substitute("$parLiveTimeB", (px) ? px->GetIndex() : 0);
			px = this->FindParam("LIVETIMEC0");
			fCodeTemplates.Substitute("$parLiveTimeC", (px) ? px->GetIndex() : 0);
			px = this->FindParam("FASTPEAKSA0");
			fCodeTemplates.Substitute("$parFastPeaksA", (px) ? px->GetIndex() : 0);
			px = this->FindParam("FASTPEAKSB0");
			fCodeTemplates.Substitute("$parFastPeaksB", (px) ? px->GetIndex() : 0);
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
			} else {
				fCodeTemplates.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "TRUE" : "FALSE");
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
			fCodeTemplates.Substitute("$runTask", this->GetRunTask());
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

Bool_t TMrbXia_DGF_4C::CheckSubeventType(TObject * Subevent) {
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
	Char_t * fp;
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

	fp = gSystem->Which(dataPath.Data(), paramFile.Data());
	if (fp == NULL) {
		gMrbLog->Err() << "No such file - " << paramFile << ", searched on \"" << dataPath << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(kFALSE);
	}
 
	paramFile = fp;
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
		istrstream str(pLine.Data());
		str >> pOffset >> pName;
		pName = pName.Strip(TString::kBoth);
		if (pName.Length() > 0) {
			if (pName.Length() == 3 && pName(0) == 'U') {
				istrstream pstr(pName.Data());
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

	Char_t * fp;
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

	fp = NULL;
	tf = "Module_";
	tf += this->ClassName();
	tf.ReplaceAll("TMrb", "");
	tf += ".rc.code";
	fp = gSystem->Which(templatePath.Data(), tf.Data());
	if (fp == NULL) return(kTRUE);

	if (verboseMode) {
		gMrbLog->Out()  << "[" << this->GetName() << "] Using template file " << fp << endl;
		gMrbLog->Flush(this->ClassName(), "MakeRcFile");
	}
	
 	rcTemplateFile = fp;

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
				rcTmpl.Substitute("$clusterColor", this->GetClusterColor());
				rcTmpl.Substitute("$clusterSegments", this->GetClusterSegments());
				rcTmpl.Substitute("$maxEvents", this->GetMaxEvents());
				rcTmpl.Substitute("$traceLength", this->GetTraceLength());
				rcTmpl.Substitute("$runTask", this->GetRunTask());
				rcTmpl.Substitute("$synchWait", this->GetSynchWait());
				rcTmpl.Substitute("$inSynch", this->GetInSynch());
				rcTmpl.Substitute("$isActive", this->IsActive() ? "TRUE" : "FALSE");
				seg = this->GetClusterSegments();
				if (fSwitchBusIndiv) {
					rcTmpl.Substitute("$switchBusTerm", fSwitchBusTerm ? "TRUE" : "FALSE");
				} else {
					rcTmpl.Substitute("$switchBusTerm", (seg.Index("c", 0) >= 0) ? "TRUE" : "FALSE");
				}
				rcTmpl.WriteCode(RcStrm);
				return(kTRUE);
			}
		}
	}
	return(kTRUE);
}
