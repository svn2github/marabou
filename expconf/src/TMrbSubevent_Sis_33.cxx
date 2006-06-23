//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent_Sis_33.cxx
// Purpose:        MARaBOU configuration: subevents of type [10,54] - SIS data
// Description:    Implements class methods to handle [10,54] subevents
//                 reflecting data structure of XIA DGF-4C modules
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent_Sis_33.cxx,v 1.1 2006-06-23 09:05:11 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TDirectory.h"

#include "TMrbTemplate.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSis_3300.h"
#include "TMrbSubevent_Sis_33.h"

#include "SetColor.h"

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent_Sis_33)

TMrbSubevent_Sis_33::TMrbSubevent_Sis_33(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate)
																: TMrbSubevent(SevtName, SevtTitle, Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_33
// Purpose:        Create a subevent type [10,54]
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Create a new subevent of type [10,54]
//                 used to store data of SIS33xx digitizing adcs
//
//                 Data format:
//                 31                                    0
//                 |=====================================|
//                 |1| module ID (14:8)     serial (7:0) |
//                 |-------------------------------------|
//                 |      word count including header    |
//                 |-------------------------------------|
//                 |           acquisition mode          |
//                 |-------------------------------------|
//                 |           number of eventes         |
//                 |-------------------------------------|
//                 |               page size             |
//                 |-------------------------------------|
//                 |             module status           |
//                 |-------------------------------------|
//                 |           acquisition status        |
//                 |-------------------------------------|
//                 |      length of time stamp block     |
//                 |-------------------------------------|
//                 |              time stamps            | numEvts * 32 bit
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |        length of trigger block      |
//                 |-------------------------------------|
//                 |             trigger data            | numEvts * 32 bit
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |         length of event block       |
//                 |-------------------------------------|
//                 |              event data             | numEvts * pageSize
//                 |                ...                  |
//                 |                ...                  |
//                 |-------------------------------------|
//                 |1| module ID (14:8)     serial (7:0) |
//                 |-------------------------------------|
//                 |                ...                  |
//                 |                ...                  |
//                 31====================================0
//
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsZombie()) {
		fSevtDescr = "SIS trace data, multi-event";
		fSevtType = 10; 	 							// set subevent type & subtype
		fSevtSubtype = 54;
		if (*SevtTitle == '\0') this->SetTitle(Form("Subevent [%d,%d]: %s", fSevtType, fSevtSubtype, fSevtDescr.Data()));
		fLegalDataTypes = TMrbConfig::kDataUShort;		// only 16 bit words
		gDirectory->Append(this);
	}
}

Bool_t TMrbSubevent_Sis_33::MakeReadoutCode(ofstream & RdoStrm,	TMrbConfig::EMrbReadoutTag TagIndex,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent_Sis_33::MakeReadoutCode
// Purpose:        Write a piece of code for subevent [10,54]
// Arguments:      ofstream & RdoStrm           -- file output stream
//                 EMrbReadoutTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code needed for subevent handling.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;
	TString sevtName;
	TString moduleNameUC;

	switch (TagIndex) {
		case TMrbConfig::kRdoOnTriggerXX:
			Template.InitializeCode("%SB%");
			Template.Substitute("$sevtNameLC", this->GetName());
			sevtName = this->GetName();
			sevtName(0,1).ToUpper();
			Template.Substitute("$sevtNameUC", sevtName.Data());
			Template.Substitute("$sevtType", (Int_t) fSevtType);
			Template.Substitute("$sevtSubtype", (Int_t) fSevtSubtype);
			Template.Substitute("$sevtSerial", fSerial);
			Template.Substitute("$crateNo", this->GetCrate());
			Template.WriteCode(RdoStrm);

			module = (TMrbModule *) fLofModules.First();
			while (module) {
				Template.InitializeCode("%SMB%");
				moduleNameUC = module->GetName();
				moduleNameUC(0,1).ToUpper();

				TString mnemoLC = module->GetMnemonic();
				TString mnemoUC = mnemoLC;
				mnemoUC(0,1).ToUpper();

				Template.Substitute("$lng", 32);
				Template.Substitute("$moduleNameLC", module->GetName());
				Template.Substitute("$moduleNameUC", moduleNameUC);
				Template.Substitute("$moduleType", mnemoUC);
				Template.Substitute("$moduleSerial", module->GetSerial());
				Template.WriteCode(RdoStrm);

				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleReadModule);

				Template.InitializeCode("%SME%");
				Template.Substitute("$lng", 32);
				Template.Substitute("$moduleNameLC", module->GetName());
				Template.Substitute("$moduleSerial", module->GetSerial());
				Template.WriteCode(RdoStrm);

				Template.InitializeCode("%SE%");
				Template.Substitute("$sevtNameLC", this->GetName());
				Template.WriteCode(RdoStrm);
				module = (TMrbModule *) fLofModules.After(module);
			}
			break;

		case TMrbConfig::kRdoIgnoreTriggerXX:
			module = (TMrbModule *) fLofModules.First();
			while (module) {
				module->MakeReadoutCode(RdoStrm, TMrbConfig::kModuleClearModule);
				module = (TMrbModule *) fLofModules.After(module);
			}
			break;
	}
	return(kTRUE);
}

Bool_t TMrbSubevent_Sis_33::MakeSpecialAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, TMrbTemplate & AnaTmpl) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::MakeSpecialAnalyzeCode
// Purpose:        Write a piece of code for data analysis
// Arguments:      ofstream & AnaStrm           -- file output stream
//                 EMrbAnalyzeTag TagIndex      -- index of tag word from template file
//                 TMrbTemplate & AnaTmpl       -- template
// Results:
// Exceptions:
// Description:    Writes special analyze code for sis subevent type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString sevtNameLC = GetName();
	TString sevtNameUC = sevtNameLC;
	sevtNameUC(0,1).ToUpper();

	switch (TagIndex) {
		case TMrbConfig::kAnaSevtBookHistograms:
		case TMrbConfig::kAnaSevtBookParams:
		case TMrbConfig::kAnaSevtFillHistograms:
			{
				AnaTmpl.InitializeCode("%B%");
				AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
				AnaTmpl.Substitute("$sevtNameUC", sevtNameUC);
				AnaTmpl.WriteCode(AnaStrm);
				TMrbModuleChannel * param = (TMrbModuleChannel *) fLofParams.First();
				Int_t pIdx = 0;
				while (param) {
					TString paramNameLC = param->GetName();
					TString paramNameUC = paramNameLC;
					paramNameUC(0,1).ToUpper();
					TMrbSis_3300 * module = (TMrbSis_3300 *) param->Parent();
					TString moduleNameLC = module->GetName();
					TString moduleNameUC = moduleNameLC;
					moduleNameUC(0,1).ToUpper();
					Int_t trigGroup = param->GetAddr();
					TString type, type1, type2;
					TString tname, tname1, tname2;
					Int_t pPerChn;
					switch (module->GetTriggerOn(trigGroup)) {
						case TMrbSis_3300::kTriggerOnDisabled:
						case TMrbSis_3300::kTriggerOnEnabled:
							type1 = "S";	// sample only
							tname1 = "sample";
							pPerChn = 1;
							break;
						case TMrbSis_3300::kTriggerOnEvenFIR:
							type1 = "S";	// sample
							type2 = "T";	// trigger
							tname1 = "sample";
							tname2 = "trigger";
							pPerChn = 2;
							break;
						case TMrbSis_3300::kTriggerOnOddFIR:
							type1 = "T";	// trigger
							type2 = "S";	// sample
							tname1 = "trigger";
							tname2 = "sample";
							pPerChn = 2;
							break;
					}
					type = type1;
					tname = tname1;
					for (Int_t i = 0; i < pPerChn; i++) {
						AnaTmpl.InitializeCode("%C%");
						AnaTmpl.Substitute("$paramNameLC", paramNameLC);
						AnaTmpl.Substitute("$paramNameUC", paramNameUC);
						AnaTmpl.Substitute("$moduleNameLC", moduleNameLC);
						AnaTmpl.Substitute("$moduleNameUC", moduleNameUC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$pageSize", module->GetChansPerPage());
						AnaTmpl.Substitute("$nofBins", module->GetBinRange());
						AnaTmpl.Substitute("$paramIndex", pIdx++);
						AnaTmpl.Substitute("$type", type);
						AnaTmpl.Substitute("$xtype", tname);
						AnaTmpl.WriteCode(AnaStrm);
						type = type2;
						tname = tname2;
					}
					param = (TMrbModuleChannel *) fLofParams.After(param);
				}
				AnaTmpl.InitializeCode("%E%");
				AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
				AnaTmpl.Substitute("$sevtNameUC", sevtNameUC);
				AnaTmpl.WriteCode(AnaStrm);
				return(kTRUE);
			}
		case TMrbConfig::kAnaSevtPrivateData:
		case TMrbConfig::kAnaSevtDefineAddr:
			{
				TMrbModuleChannel * param = (TMrbModuleChannel *) fLofParams.First();
				Int_t pIdx = 0;
				while (param) {
					TString paramNameLC = param->GetName();
					TString paramNameUC = paramNameLC;
					paramNameUC(0,1).ToUpper();
					TMrbSis_3300 * module = (TMrbSis_3300 *) param->Parent();
					TString moduleNameLC = module->GetName();
					TString moduleNameUC = moduleNameLC;
					moduleNameUC(0,1).ToUpper();
					Int_t trigGroup = param->GetAddr();
					TString type, type1, type2;
					TString tname, tname1, tname2;
					Int_t pPerChn;
					switch (module->GetTriggerOn(trigGroup)) {
						case TMrbSis_3300::kTriggerOnDisabled:
						case TMrbSis_3300::kTriggerOnEnabled:
							type1 = "S";	// sample only
							tname1 = "sample";
							pPerChn = 1;
							break;
						case TMrbSis_3300::kTriggerOnEvenFIR:
							type1 = "S";	// sample
							type2 = "T";	// trigger
							tname1 = "sample";
							tname2 = "trigger";
							pPerChn = 2;
							break;
						case TMrbSis_3300::kTriggerOnOddFIR:
							type1 = "T";	// trigger
							type2 = "S";	// sample
							tname1 = "trigger";
							tname2 = "sample";
							pPerChn = 2;
							break;
					}
					type = type1;
					tname = tname1;
					for (Int_t i = 0; i < pPerChn; i++) {
						AnaTmpl.InitializeCode();
						AnaTmpl.Substitute("$paramNameLC", paramNameLC);
						AnaTmpl.Substitute("$paramNameUC", paramNameUC);
						AnaTmpl.Substitute("$moduleNameLC", moduleNameLC);
						AnaTmpl.Substitute("$moduleNameUC", moduleNameUC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$pageSize", module->GetChansPerPage());
						AnaTmpl.Substitute("$nofBins", module->GetBinRange());
						AnaTmpl.Substitute("$paramIndex", pIdx++);
						AnaTmpl.Substitute("$type", type);
						AnaTmpl.Substitute("$xtype", tname);
						AnaTmpl.WriteCode(AnaStrm);
						type = type2;
						tname = tname2;
					}
					param = (TMrbModuleChannel *) fLofParams.After(param);
				}
				return(kTRUE);
			}
		case TMrbConfig::kAnaHistoDefinePointers:
			{
				TMrbModuleChannel * param = (TMrbModuleChannel *) fLofParams.First();
				Int_t pIdx = 0;
				while (param) {
					TMrbSis_3300 * module = (TMrbSis_3300 *) param->Parent();
					TString moduleNameLC = module->GetName();
					TString moduleNameUC = moduleNameLC;
					moduleNameUC(0,1).ToUpper();
					Int_t trigGroup = param->GetAddr();
					TString type, type1, type2;
					TString tname, tname1, tname2;
					Int_t pPerChn;
					switch (module->GetTriggerOn(trigGroup)) {
						case TMrbSis_3300::kTriggerOnDisabled:
						case TMrbSis_3300::kTriggerOnEnabled:
							type1 = "S";	// sample only
							tname1 = "sample";
							pPerChn = 1;
							break;
						case TMrbSis_3300::kTriggerOnEvenFIR:
							type1 = "S";	// sample
							type2 = "T";	// trigger
							tname1 = "sample";
							tname2 = "trigger";
							pPerChn = 2;
							break;
						case TMrbSis_3300::kTriggerOnOddFIR:
							type1 = "T";	// trigger
							type2 = "S";	// sample
							tname1 = "trigger";
							tname2 = "sample";
							pPerChn = 2;
							break;
					}
					type = type1;
					tname = tname1;
					for (Int_t i = 0; i < pPerChn; i++) {
						TString paramNameLC = param->GetName();
						TString paramNameUC = paramNameLC;
						paramNameUC(0,1).ToUpper();
						paramNameLC += type;
						paramNameUC += type;
						AnaTmpl.InitializeCode("%SN%");
						AnaTmpl.Substitute("$paramNameLC", paramNameLC);
						AnaTmpl.Substitute("$paramNameUC", paramNameUC);
						AnaTmpl.Substitute("$moduleNameLC", moduleNameLC);
						AnaTmpl.Substitute("$moduleNameUC", moduleNameUC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						AnaTmpl.Substitute("$pageSize", module->GetChansPerPage());
						AnaTmpl.Substitute("$nofBins", module->GetBinRange());
						AnaTmpl.Substitute("$paramIndex", pIdx++);
						AnaTmpl.Substitute("$type", type);
						AnaTmpl.Substitute("$xtype", tname);
						AnaTmpl.WriteCode(AnaStrm);
						type = type2;
						tname = tname2;
					}
					param = (TMrbModuleChannel *) fLofParams.After(param);
				}
				return(kTRUE);
			}
		case TMrbConfig::kAnaSevtFillSubevent:
			{
				AnaTmpl.InitializeCode();
				AnaTmpl.Substitute("$sevtNameLC", sevtNameLC);
				AnaTmpl.Substitute("$sevtNameUC", sevtNameUC);
				AnaTmpl.WriteCode(AnaStrm);
			}
			return(kTRUE);
		default:
			return(kFALSE);
	}
	return(kFALSE);
}
