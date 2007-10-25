//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           expconf/src/TMrbSubevent.cxx
// Purpose:        MARaBOU configuration: subevent definition
// Description:    Implements class methods to define subevents
// Header files:   TMrbConfig.h  -- MARaBOU class definitions
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSubevent.cxx,v 1.35 2007-10-25 17:24:13 Marabou Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "TEnv.h"
#include "TRootBrowser.h"
#include "TGLabel.h"

#include "TMrbSystem.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbLogger.h"
#include "TMrbConfig.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbSubevent.h"
#include "TMrbSubevent_Data_S.h"
#include "TMrbSubevent_Data_I.h"

#include "SetColor.h"

const SMrbNamedXShort kMrbSevtReadoutOptions[] =
							{
								{0, NULL}
							};

const SMrbNamedXShort kMrbSevtAnalyzeOptions[] =
							{
								{TMrbConfig::kAnaOptParamsByName,	"BYNAME"		},
								{~TMrbConfig::kAnaOptParamsByName,	"BYINDEX"		},
								{TMrbConfig::kAnaOptHistograms, 	"HISTOGRAMS"	},
								{~TMrbConfig::kAnaOptHistograms,	"NOHISTOGRAMS"	},
								{TMrbConfig::kAnaOptSevtDefault,	"DEFAULT"		},
								{0, 								NULL			}
							};

const SMrbNamedXShort kMrbSevtConfigOptions[] =
							{
								{0, NULL}
							};

extern TMrbConfig * gMrbConfig;
extern TMrbLogger * gMrbLog;

ClassImp(TMrbSubevent)

TMrbSubevent::TMrbSubevent(const Char_t * SevtName, const Char_t * SevtTitle, Int_t Crate) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent
// Purpose:        Create a subevent
// Arguments:      Char_t * SevtName       -- subevent name
//                 Char_t * SevtTitle      -- ... and title
//                 Int_t Crate             -- crate number
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
	if (gMrbConfig == NULL) {
		gMrbLog->Err() << SevtName << ": No config defined" << endl;
		gMrbLog->Flush(this->ClassName());
		this->MakeZombie();
	} else if (gMrbConfig->NameNotLegal("subevent", SevtName)) {
		this->MakeZombie();
	} else {
		if (gMrbConfig->FindSubevent(SevtName) != NULL) {
			gMrbLog->Err() << "Subevent already defined - " << SevtName << endl;
			gMrbLog->Flush(this->ClassName());
			this->MakeZombie();
		} else {
			SetTitle(SevtTitle);
			SetName(SevtName);

			fNofParams = 0; 				// init list of params
			fLofParams.Delete();

			fNofModules = 0;
			
			fNofShorts = 0; 				// 16 bit words

			fNofEvents = 0; 				// no events assigned

			fLofEvents.SetName("List of events");
			fLofModules.SetName("List of modules");
			fLofParams.SetName("List of parameters");

			fHasIndexedParams = kFALSE; 	// default: single params only

			fPrependPrefix = kFALSE;				// w/o prefix
			fHistosToBeAllocated = kTRUE;			// allocate histograms
			fHistosToBeFilledIfTrueHit = kFALSE;	// fill histos in any case

			fReadoutOptions = TMrbConfig::kNoOptionSpecified;
			fAnalyzeOptions = TMrbConfig::kNoOptionSpecified;
			fConfigOptions = TMrbConfig::kNoOptionSpecified;

			fArrayMode = kFALSE;
			
			fXhit = "";
			fHitDataLength = 0;

			fSoftModule = NULL;

			fMbsBranch.Set(-1, "none");

			fSerial = gMrbConfig->AssignSevtSerial();		// assign a unique id to be used by MBS
			fCrate = Crate; 								// store crate number

			fCreateHistoArray = kFALSE;

			gMrbConfig->AddSubevent(this); 	// insert subevent in list
		}
	}
}

Bool_t TMrbSubevent::AddParam(TObject * Param) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::AddParam
// Purpose:        Add a new param to subevent
// Arguments:      TObject * Param        -- param addr
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Adds a param to list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * chn;
	TMrbModule * module;
	
	chn = (TMrbModuleChannel *) Param;
	module = chn->Parent();
	
	fLofParams.Add(chn);
	fNofParams++;
	fNofShorts += module->GetNofShortsPerChannel();
	return(kTRUE);
}

TObject * TMrbSubevent::FindModuleByID(TMrbConfig::EMrbModuleID ModuleID, TObject * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::FindModuleByID
// Purpose:        Find a module by its id
// Arguments:      EMrbModuleID ModuleID  -- module id
//                 TObject * After        -- search to be started after this module
// Results:        TObject * Module       -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 next module having specified id
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	if (After == NULL) {
		module = (TMrbModule *) fLofModules.First();
	} else {
		module = (TMrbModule *) fLofModules.After(After);
	}
	while (module) {
		if (module->GetModuleID()->GetIndex() == ModuleID) return(module);
		module = (TMrbModule *) fLofModules.After(module);
	}
	return(NULL);
}

TObject * TMrbSubevent::FindModuleByType(UInt_t ModuleType, TObject * After) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::FindModuleByType
// Purpose:        Find a module by its type
// Arguments:      UInt_t ModuleType     -- module type
//                 TObject * After       -- search to be started after this module
// Results:        TObject * Module      -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 next module having specified type
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	if (After == NULL) {
		module = (TMrbModule *) fLofModules.First();
	} else {
		module = (TMrbModule *) fLofModules.After(After);
	}
	while (module) {
		if ((UInt_t) module->GetType()->GetIndex() == ModuleType) return(module);
		module = (TMrbModule *) fLofModules.After(module);
	}
	return(NULL);
}

TObject * TMrbSubevent::FindModuleBySerial(Int_t ModuleSerial) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::FindModuleBySerial
// Purpose:        Find a module by its unique serial number
// Arguments:      Int_t ModuleSerial       -- serial number
// Results:        TObject * Module         -- module address
// Exceptions:
// Description:    Loops thru the list of modules to find
//                 specified serial number
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	module = (TMrbModule *) fLofModules.First();
	while (module) {
		if (module->GetSerial() == ModuleSerial) return(module);
		module = (TMrbModule *) fLofModules.After(module);
	}
	return(NULL);
}

Bool_t TMrbSubevent::Use(const Char_t * ModuleName, const Char_t * Assignment, Bool_t BookHistos) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Use
// Purpose:        Assign parameters
// Arguments:      Char_t * ModuleName  -- module which houses these channels
//                 Char_t * Assignment  -- param name(s) & channel number(s)
//                 Bool_t BookHistos    -- kTRUE if to book histograms for these channels
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines which subaddresses to be used by assigning meangingful names.
//                 Assignment may be a single param name or a set of names, separated by blanks.
//                 Subsequent names will be assigned subsequent channel numbers.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;
	TMrbModuleChannel * cp;
	TString paramName;
	TString channelName;
	TString indexName;
	TMrbModuleChannel * arrayHead = NULL;
	Int_t channelNo, chnNo;
	Int_t indexRange;
	Int_t nch = 0;
	Int_t nx1 = 0;
	Int_t nx2 = 0;
	Bool_t ok;
	Bool_t useLongNames;
	TString softModName;
	Bool_t softModFlag;
	Int_t nofWords;
		
	TMrbModuleChannel::EMrbHistoMode histoMode;
	Char_t histoFlag;

	TMrbConfig::EMrbChannelStatus chnStatus;

	TString pos;

	softModFlag = kFALSE;
	
	if (ModuleName != NULL) {
		if ((module = (TMrbModule *) gMrbConfig->FindModule(ModuleName)) == NULL) {
			gMrbLog->Err() << fName << ": No such module - " << ModuleName << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if (!module->CheckSubeventType(this)) {
			gMrbLog->Err()	<< "Module " << ModuleName << " can't be stored in a subevent of type ["
							<< fSevtType << "," << fSevtSubtype << "]" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if (!this->CheckModuleID(module)) {
			gMrbLog->Err()	<< "Module " << ModuleName << " has wrong ID "
							<< module->GetModuleID()->GetName()
							<< " (" << setbase(16) << module->GetModuleID()->GetIndex() << ")" << setbase(10)
							<< " - can't be stored in a subevent of type ["
							<< fSevtType << "," << fSevtSubtype << "]" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if ((this->GetLegalDataTypes() && module->GetDataType()->GetIndex()) == 0) {
			gMrbLog->Err()	<< fName << ": Module " << ModuleName << " has "
							<< module->GetDataType()->GetTitle() << " data - can't be stored" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if (this->HasFixedLengthFormat() && !module->HasRandomReadout()) {
			gMrbLog->Err() << fName << ": Module " << ModuleName << " requires a variable-length subevent format" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if (!this->AllowsMultipleModules() && fNofModules > 0) {
			gMrbLog->Err()	<< fName << ": Can't assign module "
							<< ModuleName << " - only 1 module per subevent allowed" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if (fCrate != -1 && module->GetCrate() != fCrate) {
			gMrbLog->Err()	<< fName << ": Can't mix modules from different crates - " << module->GetName() << " in "
							<< (module->IsCamac() ? 	((TMrbCamacModule *) module)->GetPosition() :
														((TMrbVMEModule *) module)->GetPosition())
							<< " (should be C" << fCrate << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		fCrate = module->GetCrate();			// subevent may house modules from one single crate only
	} else if (this->IsA() == TMrbSubevent_Data_S::Class() || this->IsA() == TMrbSubevent_Data_I::Class()) {
		if (fSoftModule == NULL) {
			softModName = this->GetName();
			softModName += "_SoftModule";
			if (this->IsA())	nofWords = ((TMrbSubevent_Data_S *) this)->GetNofWords();
			else				nofWords = ((TMrbSubevent_Data_I *) this)->GetNofWords(); 
			fSoftModule = new TMrbModule(softModName.Data(), "@SoftMod@", nofWords, 0);
			for (Int_t i = 0; i < nofWords; i++) fSoftModule->GetLofChannels()->Add(new TMrbModuleChannel(fSoftModule, nch));
		}
		softModFlag = kTRUE;
		module = fSoftModule;
		ModuleName = module->GetName();
	} else {
		gMrbLog->Err()	<< fName << ": Needs a module to be assigned" << endl;
		gMrbLog->Flush(this->ClassName(), "Use");
		return(kFALSE);
	}
	
	istringstream assign(Assignment);

	channelNo = 0;							// normally starts with channel 0
	for (;;) {
		ok = kTRUE;
		indexRange = 1;
		chnStatus = TMrbConfig::kChannelSingle;
		assign >> paramName;
		if (paramName.Length() <= 0) break;
		if (paramName.Index(",") >= 0) {
			gMrbLog->Err() << fName << ": Illegal param name - " << paramName << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}
		if ((nx1 = paramName.Index("[")) >= 0) {
			if ((nx2 = paramName.Index("]")) > nx1) {
				if (this->IsInArrayMode()) {
					gMrbLog->Err() << fName << ": Explicit arrays may not be used in ARRAY MODE - " << paramName << endl;
					gMrbLog->Flush(this->ClassName(), "Use");
					return(kFALSE);
				}
				chnStatus = TMrbConfig::kChannelArray;
				fHasIndexedParams = kTRUE;
				indexName = paramName(nx1 + 1, nx2 - nx1 - 1);
				indexName += " ";			// to keep istringstream happy: s.good() needs a blank at end of input stream ...
				istringstream idx(indexName.Data());
				idx >> indexRange;
				if (!idx.good() || indexRange <= 1) ok = kFALSE;
			} else ok = kFALSE;
		}
		if (!ok) {
			gMrbLog->Err() << fName << ": Illegal index range - " << paramName << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		if ((nch = paramName.Index("=")) >= 0) {
			channelName = paramName(nch + 1, 1000);
			channelName += " ";			// to keep istringstream happy: s.good() needs a blank at end of input stream ...
			istringstream chn(channelName.Data());
			chn >> chnNo;
			if (!chn.good() || chnNo < channelNo) {
				gMrbLog->Err() << fName << ": Illegal channel number - " << paramName << endl;
				gMrbLog->Flush(this->ClassName(), "Use");
				return(kFALSE);
			}
			channelNo = chnNo;
			paramName = paramName(0,nch);
		}

		histoMode = BookHistos ? TMrbModuleChannel::kMrbHasHistogramDefault : TMrbModuleChannel::kMrbHasHistogramFalse;

		if (chnStatus == TMrbConfig::kChannelArray) {
			histoFlag = paramName(nx2 + 1);
			if (histoFlag == '+') {
				if (this->HistosToBeAllocated()) {
					histoMode = TMrbModuleChannel::kMrbHasHistogramTrue;
				} else {
					gMrbLog->Err()	<< fName << ": Param " << paramName
										<< " - can't allocate histogram(s) (subevent "
										<< this->GetName() << " doesn't provide histogramming)" << endl;
					gMrbLog->Flush(this->ClassName(), "Use");
					return(kFALSE);
				}
			} else if (histoFlag == '-') {
				histoMode = TMrbModuleChannel::kMrbHasHistogramFalse;
			}
			paramName = paramName(0, nx1);
		} else {
			histoFlag = paramName(paramName.Length() - 1);
			if (histoFlag == '+') {
				if (this->HistosToBeAllocated()) {
					histoMode = TMrbModuleChannel::kMrbHasHistogramTrue;
				} else {
					gMrbLog->Err()	<< fName << ": Param " << paramName
										<< " - can't allocate histogram(s) (subevent "
										<< this->GetName() << " doesn't provide histogramming)" << endl;
					gMrbLog->Flush(this->ClassName(), "Use");
					return(kFALSE);
				}
			} else if (histoFlag == '-') {
				histoMode = TMrbModuleChannel::kMrbHasHistogramFalse;
			}
			if (histoMode != TMrbModuleChannel::kMrbHasHistogramDefault)
															paramName = paramName(0, paramName.Length() - 1);
		}

		if ((cp = (TMrbModuleChannel *) this->FindParam(paramName.Data())) != NULL) {
			gMrbLog->Err()	<< fName << ": Param name already used - " << paramName
							<< " = " << cp->GetAddr() << endl;
			gMrbLog->Flush(this->ClassName(), "Use");
			return(kFALSE);
		}

		useLongNames = this->HistosToBeAllocated() ? (gMrbConfig->FindParam(paramName.Data()) != NULL) : kFALSE;
		for (nch = 0; nch < indexRange; nch++, channelNo++) {
			if ((cp = (TMrbModuleChannel *) module->GetChannel(channelNo)) == NULL) {
				gMrbLog->Err()	<< ModuleName << ": Channel out of range - " << paramName;
				if (chnStatus == TMrbConfig::kChannelArray) gMrbLog->Err() << nch;
				gMrbLog->Err()	<< "=" << channelNo << endl;
				gMrbLog->Flush(this->ClassName(), "Use");
				return(kFALSE);
			}

			if (cp->IsUsed()) {
				gMrbLog->Err()	<< ModuleName << ": Channel already assigned to subevent "
								<< (cp->UsedBy() ? cp->UsedBy()->GetName() : "???")
								<< ", param " << cp->GetName() << "=" << channelNo << endl;
				gMrbLog->Flush(this->ClassName(), "Use");
				return(kFALSE);
			}

			if (chnStatus == TMrbConfig::kChannelArray) {
				if (nch == 0) {
					arrayHead = cp;
					cp->SetIndexRange(indexRange);
					cp->SetStatus(TMrbConfig::kChannelArray);
					cp->SetName(paramName.Data());
				} else {
					cp->MarkIndexed(arrayHead);
					cp->SetStatus(TMrbConfig::kChannelArrElem);
					TMrbString pn(paramName.Data(), nch);
					cp->SetName(pn.Data());
				}
			} else {					
				cp->SetIndexRange(1);
				cp->MarkIndexed(NULL);
				cp->SetName(paramName.Data());
				cp->SetStatus(TMrbConfig::kChannelSingle);
			}
			cp->MarkUsed(this);
			cp->SetHistoMode(histoMode);

			this->AddParam(cp);
			
			if (useLongNames) gMrbConfig->UseLongParamNames();

			if (!softModFlag && this->FindModule((const Char_t *) module->GetName()) == NULL) this->AddModule(module);
		}
	}
	return(kTRUE);
}

Bool_t TMrbSubevent::MakeAnalyzeCode(ofstream & AnaStrm, TMrbConfig::EMrbAnalyzeTag TagIndex, const Char_t * Extension) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::MakeAnalyzeCode
// Purpose:        Write a piece of code for data analysis
// Arguments:      ofstream & AnaStrm       -- file output stream
//                 EMrbAnalyzeTag TagIndex  -- index of tag word from template file
//                 Char_t * Extension       -- file extensions to be appended (.h or .cxx)
// Results:
// Exceptions:
// Description:    Writes code needed for data analysis.
//                 Searches for following files:
//                      (1)  if subevent has private code
//                      (1a)    Subevent_<Name>.<Extension>.code
//                      (1b)    <PrivateCodeFile>.<Extension>.code
//                      (1c)    Subevent_<Class>.<Extension>.code
//                      (2)  standard:
//                              Subevent.<Extension>.code
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;
	TMrbModuleChannel * param;
	TMrbModule * module, * lmodule;
	TString line;
	TString evtNameUC;
	TString evtNameLC;
	TString sevtNameLC;
	TString sevtNameUC;
	TString paramNameLC;
	TString paramNameUC;
	TString moduleNameLC;
	TString moduleNameUC;

	TString prefixLC;
	TString prefixUC;
	TString prefixDotted;
	TString pUC;
	Bool_t prependPrefix;
	Bool_t pFlag = kFALSE;

	Bool_t stdHistosOK;
	Bool_t doIt;
	TMrbModuleChannel::EMrbHistoMode histoMode, defaultHistoMode;

	TMrbNamedX * dataType;

	Int_t wordsPerParam, wordsSoFar, nofWords, nofBytes;
	Bool_t anaOptLeaves;

	TMrbConfig::EMrbAnalyzeTag tagIdx;
	TMrbNamedX * analyzeTag;

	TString templatePath;
	TString anaTemplateFile;

	TString tf, tf1, tf2, tf3, tf4;
	const Char_t * pcf;
	
	TMrbString iniTag;

	TMrbTemplate anaTmpl;

	TMrbConfig::EMrbChannelStatus paramStatus;

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	sevtNameLC = this->GetName();
	sevtNameUC = this->GetName();
	sevtNameUC(0,1).ToUpper();

	if (this->PrefixToBePrepended()) {
		prefixLC = fPrefix; 
		prefixDotted = fPrefix;
	} else {
		prefixLC = sevtNameLC;
		prefixDotted = prefixLC + ".";
	}
	prefixUC = prefixLC;
	prefixUC(0,1).ToUpper();
	prependPrefix = this->PrefixToBePrepended() || gMrbConfig->LongParamNamesToBeUsed();

	templatePath = gEnv->GetValue("TMrbConfig.TemplatePath", ".:config:$(MARABOU)/templates/config");
	gSystem->ExpandPathName(templatePath);

	TString fileSpec = "";
	TMrbSystem ux;
	TObjArray err;
	err.Delete();
	if (this->HasPrivateCode()) {
		tf = "Subevent_";
		tf += sevtNameUC;
		tf += Extension;
		tf += ".code";
		tf1 = tf;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
		if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
		if (fileSpec.IsNull()) {
			pcf = this->GetPrivateCodeFile();
			if (pcf != NULL) {
				tf = pcf;
				tf += Extension;
				tf += ".code";
				tf2 = tf;
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
			if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
			if (fileSpec.IsNull()) {
				tf = this->ClassName();
				tf += Extension;
				tf += ".code";
				tf.ReplaceAll("TMrb", "");
				tf3 = tf;
				ux.Which(fileSpec, templatePath.Data(), tf.Data());
				if (fileSpec.IsNull()) err.Add(new TObjString(tf.Data()));
			}
		}
	}
	if (fileSpec.IsNull()) {
		tf = "Subevent";
		tf += Extension;
		tf += ".code";
		tf4 = tf;
		ux.Which(fileSpec, templatePath.Data(), tf.Data());
	}
	if (fileSpec.IsNull()) {
		gMrbLog->Err()	<< "Template file not found -" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
		gMrbLog->Err()	<< "          Searching on path " << templatePath << endl;
		gMrbLog->Flush();
		if (this->HasPrivateCode()) {
			gMrbLog->Err()	<< "          for file " << tf1 << endl;
			gMrbLog->Flush();
			if (tf2) {
				gMrbLog->Err()	<< "          or       " << tf2 << endl;
				gMrbLog->Flush();
			}
			gMrbLog->Err()	<< "          or       " << tf3 << endl;
			gMrbLog->Flush();
			gMrbLog->Err()	<< "          or       " << tf4 << endl;
			gMrbLog->Flush();
		} else {
			gMrbLog->Err()	<< "          for file " << tf4 << endl;
			gMrbLog->Flush();
		}
		return(kFALSE);
	}

	if (verboseMode) {
		gMrbLog->Out()  << "[" << sevtNameLC << "] Using template file " << fileSpec << endl;
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
		} else if (gMrbConfig->ExecUserMacro(&AnaStrm, this, analyzeTag->GetName())) {
			continue;
		} else {
			tagIdx = (TMrbConfig::EMrbAnalyzeTag) analyzeTag->GetIndex();
			if (this->MakeSpecialAnalyzeCode(AnaStrm, tagIdx, anaTmpl)) continue;
			switch (tagIdx) {
				case TMrbConfig::kAnaSevtNameLC:
					AnaStrm << anaTmpl.Encode(line, sevtNameLC) << endl;
					break;
				case TMrbConfig::kAnaSevtNameUC:
					AnaStrm << anaTmpl.Encode(line, sevtNameUC) << endl;
					break;
				case TMrbConfig::kAnaSevtTitle:
					AnaStrm << anaTmpl.Encode(line, GetTitle()) << endl;
					break;
				case TMrbConfig::kAnaSevtSerial:
					{
						TString sid = "";
						sid += fSerial;
						AnaStrm << anaTmpl.Encode(line, sid) << endl;
					}
					break;
				case TMrbConfig::kAnaSevtNofParams:
					{
						TString nofParams = "";
						nofParams += fNofParams;
						AnaStrm << anaTmpl.Encode(line, nofParams) << endl;
					}
					break;
				case TMrbConfig::kAnaSevtXhitClass:
					AnaStrm << anaTmpl.Encode(line, (this->HasXhit() ? "_Xhit" : "")) << endl;
					break;
				case TMrbConfig::kAnaSevtFriends:
					evt = (TMrbEvent *) fLofEvents.First();
					while (evt) {
						evtNameLC = evt->GetName();
						evtNameUC = evtNameLC;
						evtNameUC(0,1).ToUpper();
						anaTmpl.InitializeCode();
						anaTmpl.Substitute("$className", evtNameUC);
						anaTmpl.WriteCode(AnaStrm);
						evt = (TMrbEvent *) fLofEvents.After(evt);
					}
					break;
				case TMrbConfig::kAnaSevtCtor:
					if (this->HasIndexedParams()) {
						anaTmpl.InitializeCode("%AB%");
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.WriteCode(AnaStrm);
						param = (TMrbModuleChannel *) fLofParams.First();
						while (param) {
							module = (TMrbModule *) param->Parent();
							paramStatus = param->GetStatus();
							if (paramStatus != TMrbConfig::kChannelArrElem) {
								paramNameLC = param->GetName();
								paramNameUC = paramNameLC;
								paramNameUC(0,1).ToUpper();
								if (paramStatus == TMrbConfig::kChannelArray) {
									anaTmpl.InitializeCode("%AL%");
									anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
									anaTmpl.Substitute("$paramNameLC", paramNameLC);
									anaTmpl.Substitute("$paramNameUC", paramNameUC);
									anaTmpl.Substitute("$channelNo", (Int_t) param->GetAddr());
									anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
									dataType = module->GetDataType();
									if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
										dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
									}
									anaTmpl.Substitute("$dataType", dataType->GetTitle());
									anaTmpl.Substitute("$arrayType", dataType->GetName());
									anaTmpl.WriteCode(AnaStrm);
								}
							}
							param = (TMrbModuleChannel *) fLofParams.After(param);
						}
						anaTmpl.InitializeCode("%AE%");
						anaTmpl.WriteCode(AnaStrm);
					} else {
						anaTmpl.InitializeCode("%NA%");
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.WriteCode(AnaStrm);
					}
					break;
				case TMrbConfig::kAnaSevtPrivateHistograms:
					evt = (TMrbEvent *) fLofEvents.First();
					stdHistosOK = kFALSE;
					anaTmpl.InitializeCode("%R%");
					anaTmpl.Substitute("$iniVal", this->HistosToBeFilledIfTrueHit() ? -1 : 0);
					anaTmpl.WriteCode(AnaStrm);
					while (evt) {
						if (evt->HasPrivateHistograms()) {
							evtNameLC = evt->GetName();
							evtNameUC = evtNameLC;
							evtNameUC(0,1).ToUpper();
							anaTmpl.InitializeCode("%P%");
							anaTmpl.Substitute("$evtNameLC", evtNameLC);
							anaTmpl.Substitute("$evtNameUC", evtNameUC);
							anaTmpl.WriteCode(AnaStrm);
						} else stdHistosOK = kTRUE;
						evt = (TMrbEvent *) fLofEvents.After(evt);
					}
					if (stdHistosOK) {
						anaTmpl.InitializeCode("%N%");
						anaTmpl.WriteCode(AnaStrm);
					}
					break;
				case TMrbConfig::kAnaSevtDefineAddr:
				case TMrbConfig::kAnaSevtPrivateData:
					if (this->IsInArrayMode()) {
						param = (TMrbModuleChannel *) fLofParams.First();
						module = (TMrbModule *) param->Parent();
						dataType = module->GetDataType();
						if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
							dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
						}
						anaTmpl.InitializeCode("%A%");
						anaTmpl.Substitute("$dataType", dataType->GetTitle());
						anaTmpl.Substitute("$nofParams",this->GetNofParams());
						anaTmpl.WriteCode(AnaStrm);
					} else {
						param = (TMrbModuleChannel *) fLofParams.First();
						while (param) {
							module = (TMrbModule *) param->Parent();
							paramStatus = param->GetStatus();
							if (paramStatus != TMrbConfig::kChannelArrElem) {
								paramNameLC = param->GetName();
								paramNameUC = paramNameLC;
								paramNameUC(0,1).ToUpper();
								if (paramStatus == TMrbConfig::kChannelSingle)	anaTmpl.InitializeCode("%S%");
								else											anaTmpl.InitializeCode("%X%");
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$paramNameLC", paramNameLC);
								anaTmpl.Substitute("$paramNameUC", paramNameUC);
								anaTmpl.Substitute("$channelNo", (Int_t) param->GetAddr());
								anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
								dataType = module->GetDataType();
								if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
									dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
								}
								anaTmpl.Substitute("$dataType", dataType->GetTitle());
								anaTmpl.WriteCode(AnaStrm);
							}
							param = (TMrbModuleChannel *) fLofParams.After(param);
						}
					}
					break;
				case TMrbConfig::kAnaSevtFillSubevent:
					anaTmpl.InitializeCode("%B%");
					anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
					anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
					anaTmpl.WriteCode(AnaStrm);
					if (!this->IsRaw()) {
						if (this->HasFixedLengthFormat())	anaTmpl.InitializeCode("%FX%");
						else								anaTmpl.InitializeCode("%VR%");
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						nofWords = this->GetNofShorts();
						if (nofWords & 1) nofWords++;		// must be aligned to 32 bit
						anaTmpl.Substitute("$nofWords", nofWords);
						anaTmpl.Substitute("$sevtSerial", this->GetSerial());
						anaTmpl.WriteCode(AnaStrm);
					}
					anaTmpl.InitializeCode("%C%");
					anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
					anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
					anaTmpl.WriteCode(AnaStrm);
					if (this->IsRaw()) {
						anaTmpl.InitializeCode("%UDB%");
						anaTmpl.WriteCode(AnaStrm);
					}
					if (this->IsInArrayMode()) {
						param = (TMrbModuleChannel *) fLofParams.First();
						module = (TMrbModule *) param->Parent();
						dataType = module->GetDataType();
						if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
							dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
						}
						anaTmpl.Substitute("$dataType", dataType->GetTitle());
						iniTag = this->IsRaw() ? "%UDA" : "%A";
						iniTag += param->Parent()->GetDataType()->GetName();
						iniTag += "%";
						anaTmpl.InitializeCode(iniTag.Data());
						paramNameLC = param->GetName();
						paramNameUC = paramNameLC;
						paramNameUC(0,1).ToUpper();
						anaTmpl.Substitute("$firstParam", paramNameUC);
						paramNameLC = ((TMrbModuleChannel *) fLofParams.Last())->GetName();
						paramNameUC = paramNameLC;
						paramNameUC(0,1).ToUpper();
						anaTmpl.Substitute("$lastParam", paramNameUC);
						anaTmpl.Substitute("$dataType", dataType->GetTitle());
						anaTmpl.Substitute("$nofParams", this->GetNofParams());
						anaTmpl.WriteCode(AnaStrm);
						if (((module->GetNofShortsPerChannel() * this->GetNofParams()) % 2) == 1) {
							iniTag = this->IsRaw() ? "%UD" : "%";
							iniTag += "AL%";
							anaTmpl.InitializeCode(iniTag.Data());
							anaTmpl.WriteCode(AnaStrm);
						}
					} else {
						wordsSoFar = 0;
						param = (TMrbModuleChannel *) fLofParams.First();
						lmodule = NULL;
						while (param) {
							module = (TMrbModule *) param->Parent();
							if (module != lmodule) {
								anaTmpl.InitializeCode("%MD%");
								anaTmpl.Substitute("$moduleName", module->GetName());
								anaTmpl.Substitute("$moduleTitle", module->GetTitle());
								anaTmpl.Substitute("$moduleSerial", module->GetSerial());
								anaTmpl.WriteCode(AnaStrm);
							}
							lmodule = module;
							iniTag = this->IsRaw() ? "%UD" : "%";
							paramStatus = param->GetStatus();
							wordsPerParam = param->Parent()->GetNofShortsPerChannel();
							if (param->Parent()->GetDataType()->GetIndex() == TMrbConfig::kDataUInt && (wordsSoFar % 2) == 1) {
								iniTag += "AL%";
								anaTmpl.InitializeCode(iniTag.Data());
								anaTmpl.WriteCode(AnaStrm);
								wordsSoFar++;
							}
							iniTag = this->IsRaw() ? "%UD" : "%";
							if (paramStatus != TMrbConfig::kChannelArrElem) {
								paramNameLC = param->GetName();
								paramNameUC = paramNameLC;
								paramNameUC(0,1).ToUpper();
								if (paramStatus == TMrbConfig::kChannelSingle)	iniTag += "S";
								else											iniTag += "X";
								iniTag += param->Parent()->GetDataType()->GetName();
								iniTag += "%";
								anaTmpl.InitializeCode(iniTag.Data());
								if (prependPrefix)	anaTmpl.Substitute("$prefix", prefixDotted);
								else				anaTmpl.Substitute("$prefix", "");
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$moduleSerial", module->GetSerial());
								anaTmpl.Substitute("$chnNo", param->GetAddr());
								anaTmpl.Substitute("$paramNameLC", paramNameLC);
								anaTmpl.Substitute("$paramNameUC", paramNameUC);
								anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
								dataType = module->GetDataType();
								if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
									dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
								}
								anaTmpl.Substitute("$dataType", dataType->GetTitle());
								anaTmpl.WriteCode(AnaStrm);
								wordsSoFar += param->GetIndexRange() * wordsPerParam;
							}
							param = (TMrbModuleChannel *) fLofParams.After(param);
						}
					}
					anaTmpl.InitializeCode("%E%");
					anaTmpl.WriteCode(AnaStrm);
					break;
				case TMrbConfig::kAnaSevtInitializeBranch:
					anaOptLeaves = (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptLeaves) != 0;
					if (anaOptLeaves & this->NeedsBranchMode()) {
						gMrbLog->Err()	<< "[" << this->GetName()
										<< "] can't run in LEAVES mode - switching to BRANCH mode" << endl;
						gMrbLog->Flush(this->ClassName(), "MakeAnalyzeCode");
						anaOptLeaves = kFALSE;
					}
					if (anaOptLeaves) {
						anaTmpl.InitializeCode("%LVB%");
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						anaTmpl.WriteCode(AnaStrm);
						if (this->IsInArrayMode()) {
							anaTmpl.InitializeCode("%LVA%");
							anaTmpl.Substitute("$nofParams", this->GetNofParams());
							anaTmpl.WriteCode(AnaStrm);
						} else {
							param = (TMrbModuleChannel *) fLofParams.First();
							while (param) {
								module = (TMrbModule *) param->Parent();
								paramStatus = param->GetStatus();
								if (paramStatus != TMrbConfig::kChannelArrElem) {
									paramNameLC = param->GetName();
									paramNameUC = paramNameLC;
									paramNameUC(0,1).ToUpper();
									if (paramStatus == TMrbConfig::kChannelSingle)		anaTmpl.InitializeCode("%LVS%");
									else												anaTmpl.InitializeCode("%LVX%");
									if (prependPrefix)	anaTmpl.Substitute("$prefix", prefixDotted);
									else				anaTmpl.Substitute("$prefix", "");
									anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
									anaTmpl.Substitute("$paramNameLC", paramNameLC);
									anaTmpl.Substitute("$paramNameUC", paramNameUC);
									anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
									dataType = module->GetDataType();
									if (module->ConvertToInt() && dataType->GetIndex() <= TMrbConfig::kDataUInt) {
										dataType = gMrbConfig->GetLofDataTypes()->FindByName("I");
									}
									anaTmpl.Substitute("$dataType", dataType->GetTitle());
									anaTmpl.WriteCode(AnaStrm);
								}
								param = (TMrbModuleChannel *) fLofParams.After(param);
							}
						}
						anaTmpl.InitializeCode("%LVE%");
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						anaTmpl.WriteCode(AnaStrm);
					} else {
						anaTmpl.InitializeCode("%BR%");
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						anaTmpl.WriteCode(AnaStrm);
					}
					break;
				case TMrbConfig::kAnaSevtBookParams:
					{
						anaTmpl.InitializeCode("%B%");
						anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
						anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
						anaTmpl.WriteCode(AnaStrm);
						if (this->IsA() != TMrbSubevent_Data_S::Class() && this->IsA() != TMrbSubevent_Data_I::Class()) {
							pFlag = prependPrefix;
							pUC = prefixUC;
							evt = (TMrbEvent *) fLofEvents.First();
							TList onceOnly;
							while (evt) {
								evtNameLC = evt->GetName();
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								param = (TMrbModuleChannel *) fLofParams.First();
								while (param) {
									if (onceOnly.FindObject(param->GetName()) == NULL) {
										onceOnly.Add(new TNamed(param->GetName(), ""));
										paramStatus = param->GetStatus();
										if (paramStatus != TMrbConfig::kChannelArrElem) {
											module = (TMrbModule *) param->Parent();
											moduleNameLC = module->GetName();
											moduleNameUC = moduleNameLC;
											moduleNameUC(0,1).ToUpper();
											paramNameLC = param->GetName();
											paramNameUC = paramNameLC;
											paramNameUC(0,1).ToUpper();
											if (this->IsInArrayMode())							anaTmpl.InitializeCode("%A%");
											else if (paramStatus == TMrbConfig::kChannelSingle) anaTmpl.InitializeCode("%S%");
											else												anaTmpl.InitializeCode("%X%");
											if (pFlag)	anaTmpl.Substitute("$prefix", pUC);
											else		anaTmpl.Substitute("$prefix", "");
											anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
											anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
											anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
											anaTmpl.Substitute("$paramNameLC", paramNameLC);
											anaTmpl.Substitute("$paramNameUC", paramNameUC);
											anaTmpl.Substitute("$paramIndex", param->GetAddr());
											anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
											anaTmpl.WriteCode(AnaStrm);
										}
									}
									param = (TMrbModuleChannel *) fLofParams.After(param);
								}
								evt = (TMrbEvent *) fLofEvents.After((TObject *) evt);
							}
							onceOnly.Delete();
						}
						anaTmpl.InitializeCode("%E%");
						anaTmpl.WriteCode(AnaStrm);
					}
					break;
				case TMrbConfig::kAnaSevtBookHistograms:
					{
						stdHistosOK = kFALSE;
						evt = (TMrbEvent *) fLofEvents.First();
						while (evt) {
							evtNameLC = evt->GetName();
							evtNameUC = evtNameLC;
							evtNameUC(0,1).ToUpper();
							doIt = kFALSE;
							if (evt->HasPrivateHistograms()) {
								anaTmpl.InitializeCode("%BP%");
								pFlag = kTRUE;
								pUC = evt->GetPrefix();
								pUC(0,1).ToUpper();
								doIt = kTRUE;
							} else if (!stdHistosOK) {
								anaTmpl.InitializeCode("%BN%");
								pFlag = prependPrefix;
								pUC = prefixUC;
								stdHistosOK = kTRUE;
								doIt = kTRUE;
							}

							if (doIt) {
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.WriteCode(AnaStrm);
								if (this->IsA() != TMrbSubevent_Data_S::Class() && this->IsA() != TMrbSubevent_Data_I::Class()) {
									if (this->GetAnalyzeOptions(evt) & TMrbConfig::kAnaOptHistograms)
											defaultHistoMode = TMrbModuleChannel::kMrbHasHistogramTrue;
									else	defaultHistoMode = TMrbModuleChannel::kMrbHasHistogramFalse;
									if (this->IsInArrayMode()) {
										param = (TMrbModuleChannel *) fLofParams.First();
										while (param) {
											histoMode = param->GetHistoMode();
											if (histoMode == TMrbModuleChannel::kMrbHasHistogramDefault) histoMode = defaultHistoMode;
											if (histoMode == TMrbModuleChannel::kMrbHasHistogramTrue) {
												paramStatus = param->GetStatus();
												if (paramStatus != TMrbConfig::kChannelArrElem) {
													module = (TMrbModule *) param->Parent();
													if (module->HistosToBeAllocated() && module->GetRange() > 0) {
														moduleNameLC = module->GetName();
														moduleNameUC = moduleNameLC;
														moduleNameUC(0,1).ToUpper();
														paramNameLC = param->GetName();
														paramNameUC = paramNameLC;
														paramNameUC(0,1).ToUpper();
														if (this->IsInArrayMode())							anaTmpl.InitializeCode("%A%");
														else if (paramStatus == TMrbConfig::kChannelSingle) anaTmpl.InitializeCode("%S%");
														else												anaTmpl.InitializeCode("%X%");
														if (pFlag)	anaTmpl.Substitute("$prefix", pUC);
														else		anaTmpl.Substitute("$prefix", "");
														anaTmpl.Substitute("$evtNameLC", evtNameLC);
														anaTmpl.Substitute("$evtNameUC", evtNameUC);
														anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
														anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
														anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
														anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
														anaTmpl.Substitute("$paramNameLC", paramNameLC);
														anaTmpl.Substitute("$paramNameUC", paramNameUC);
														anaTmpl.Substitute("$paramIndex", param->GetAddr());
														anaTmpl.Substitute("$hBinSize", (Int_t) module->GetBinRange());
														anaTmpl.Substitute("$hLowerLim", (Int_t) module->GetXmin());
														anaTmpl.Substitute("$hUpperLim", (Int_t) module->GetXmax());
														anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
														anaTmpl.WriteCode(AnaStrm);
													}
												}
											}
											param = (TMrbModuleChannel *) fLofParams.After(param);
										}
									} else {
										param = (TMrbModuleChannel *) fLofParams.First();
										while (param) {
											histoMode = param->GetHistoMode();
											if (histoMode == TMrbModuleChannel::kMrbHasHistogramDefault) histoMode = defaultHistoMode;
											if (histoMode == TMrbModuleChannel::kMrbHasHistogramTrue) {
												paramStatus = param->GetStatus();
												if (paramStatus != TMrbConfig::kChannelArrElem) {
													module = (TMrbModule *) param->Parent();
													if (module->HistosToBeAllocated() && module->GetRange() > 0) {
														moduleNameLC = module->GetName();
														moduleNameUC = moduleNameLC;
														moduleNameUC(0,1).ToUpper();	
														paramNameLC = param->GetName();
														paramNameUC = paramNameLC;
														paramNameUC(0,1).ToUpper();
														if (this->IsInArrayMode())							anaTmpl.InitializeCode("%A%");
														else if (paramStatus == TMrbConfig::kChannelSingle)	anaTmpl.InitializeCode("%S%");
														else												anaTmpl.InitializeCode("%X%");
														if (pFlag)	anaTmpl.Substitute("$prefix", pUC);
														else		anaTmpl.Substitute("$prefix", "");
														anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
														anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
														anaTmpl.Substitute("$moduleNameLC", moduleNameLC);
														anaTmpl.Substitute("$moduleNameUC", moduleNameUC);
														anaTmpl.Substitute("$paramNameLC", paramNameLC);
														anaTmpl.Substitute("$paramNameUC", paramNameUC);
														anaTmpl.Substitute("$paramIndex", param->GetAddr());
														anaTmpl.Substitute("$hBinSize", (Int_t) module->GetBinRange());
														anaTmpl.Substitute("$hLowerLim", (Int_t) module->GetXmin());
														anaTmpl.Substitute("$hUpperLim", (Int_t) module->GetXmax());
														anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
														anaTmpl.WriteCode(AnaStrm);
													}	
												}
											}
											param = (TMrbModuleChannel *) fLofParams.After(param);
										}
									}
								}
							}
							if (evt->HasPrivateHistograms()) {
								anaTmpl.InitializeCode("%E%");
								anaTmpl.WriteCode(AnaStrm);
							}
							evt = (TMrbEvent *) fLofEvents.After((TObject *) evt);
						}
						if (stdHistosOK) {
							anaTmpl.InitializeCode("%E%");
							anaTmpl.WriteCode(AnaStrm);
						}
					}
					break;
				case TMrbConfig::kAnaSevtFillHistograms:
					{
						stdHistosOK = kFALSE;
						evt = (TMrbEvent *) fLofEvents.First();
						while (evt) {
							doIt = kFALSE;
							if (evt->HasPrivateHistograms()) {
								anaTmpl.InitializeCode("%BP%");
								evtNameLC = evt->GetName();
								evtNameUC = evtNameLC;
								evtNameUC(0,1).ToUpper();
								pFlag = kTRUE;
								pUC = evt->GetPrefix();
								pUC(0,1).ToUpper();
								doIt = kTRUE;
							} else if (!stdHistosOK) {
								anaTmpl.InitializeCode("%BN%");
								pFlag = prependPrefix;
								pUC = prefixUC;
								stdHistosOK = kTRUE;
								doIt = kTRUE;
							}
							if (doIt) {
								anaTmpl.Substitute("$evtNameLC", evtNameLC);
								anaTmpl.Substitute("$evtNameUC", evtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.WriteCode(AnaStrm);
								if (this->IsInArrayMode()) {
									anaTmpl.InitializeCode(this->HistosToBeFilledIfTrueHit() ? "%PA" : "%A%");
									if (pFlag)	anaTmpl.Substitute("$prefix", pUC);
									else		anaTmpl.Substitute("$prefix", "");
									anaTmpl.Substitute("$evtNameLC", evtNameLC);
									anaTmpl.Substitute("$evtNameUC", evtNameUC);
									anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
									anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
									param = (TMrbModuleChannel *) fLofParams.First();
									paramNameLC = param->GetName();
									paramNameUC = paramNameLC;
									paramNameUC(0,1).ToUpper();
									anaTmpl.Substitute("$firstParam", paramNameUC);
									param = (TMrbModuleChannel *) fLofParams.Last();
									paramNameLC = param->GetName();
									paramNameLC = param->GetName();
									paramNameUC = paramNameLC;
									paramNameUC(0,1).ToUpper();
									anaTmpl.Substitute("$lastParam", paramNameUC);
									anaTmpl.Substitute("$nofParams", this->GetNofParams());
									anaTmpl.WriteCode(AnaStrm);
								} else {
									if (this->GetAnalyzeOptions(evt) & TMrbConfig::kAnaOptHistograms)
												defaultHistoMode = TMrbModuleChannel::kMrbHasHistogramTrue;
									else		defaultHistoMode = TMrbModuleChannel::kMrbHasHistogramFalse;
									param = (TMrbModuleChannel *) fLofParams.First();
									while (param) {
										histoMode = param->GetHistoMode();
										if (histoMode == TMrbModuleChannel::kMrbHasHistogramDefault) histoMode = defaultHistoMode;
										if (histoMode == TMrbModuleChannel::kMrbHasHistogramTrue) {
											paramStatus = param->GetStatus();
											if (paramStatus != TMrbConfig::kChannelArrElem) {
												module = (TMrbModule *) param->Parent();
												if (module->HistosToBeAllocated() && module->GetRange() > 0) {
													paramNameLC = param->GetName();
													paramNameUC = paramNameLC;
													paramNameUC(0,1).ToUpper();
													iniTag = this->HistosToBeFilledIfTrueHit() ? "%P" : "%";
													iniTag += (paramStatus == TMrbConfig::kChannelSingle) ? "S%" : "X%";
													anaTmpl.InitializeCode(iniTag);
													if (pFlag)	anaTmpl.Substitute("$prefix", pUC);
													else		anaTmpl.Substitute("$prefix", "");
													anaTmpl.Substitute("$evtNameLC", evtNameLC);
													anaTmpl.Substitute("$evtNameUC", evtNameUC);
													anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
													anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
													anaTmpl.Substitute("$paramNameLC", paramNameLC);
													anaTmpl.Substitute("$paramNameUC", paramNameUC);
													anaTmpl.Substitute("$indexRange", (Int_t) param->GetIndexRange());
													anaTmpl.WriteCode(AnaStrm);
												}
											}
										}
										param = (TMrbModuleChannel *) fLofParams.After(param);
									}
								}
							}
							if (evt->HasPrivateHistograms()) {
								anaTmpl.InitializeCode("%E%");
								anaTmpl.WriteCode(AnaStrm);
							}
							evt = (TMrbEvent *) fLofEvents.After((TObject *) evt);
						}	
						if (stdHistosOK) {
							anaTmpl.InitializeCode("%E%");
							anaTmpl.WriteCode(AnaStrm);
						}
					}
					break;
				case TMrbConfig::kAnaSevtResetData:
					if (this->IsInArrayMode()) {
						anaTmpl.InitializeCode("%A%");
						param = (TMrbModuleChannel *) fLofParams.First();
						paramNameLC = param->GetName();
						paramNameUC = paramNameLC;
						paramNameUC(0,1).ToUpper();
						anaTmpl.Substitute("$firstParam", paramNameUC);
						param = (TMrbModuleChannel *) fLofParams.Last();
						paramNameLC = param->GetName();
						paramNameUC = paramNameLC;
						paramNameUC(0,1).ToUpper();
						anaTmpl.Substitute("$lastParam", paramNameUC);
						anaTmpl.Substitute("$nofParams", this->GetNofParams());
						anaTmpl.WriteCode(AnaStrm);
					} else {
						param = (TMrbModuleChannel *) fLofParams.First();
						while (param) {
							module = (TMrbModule *) param->Parent();
							paramStatus = param->GetStatus();
							if (paramStatus != TMrbConfig::kChannelArrElem) {
								paramNameLC = param->GetName();
								paramNameUC = paramNameLC;
								paramNameUC(0,1).ToUpper();
								if (paramStatus == TMrbConfig::kChannelSingle)	iniTag = "%S%";
								else											iniTag = "%X%";
								anaTmpl.InitializeCode(iniTag.Data());
								anaTmpl.Substitute("$sevtNameUC", sevtNameUC);
								anaTmpl.Substitute("$sevtNameLC", sevtNameLC);
								anaTmpl.Substitute("$paramNameLC", paramNameLC);
								anaTmpl.Substitute("$paramNameUC", paramNameUC);
								nofBytes = param->GetIndexRange() * module->GetNofShortsPerChannel() * sizeof(Short_t);
								anaTmpl.Substitute("$indexRange", param->GetIndexRange());
								anaTmpl.Substitute("$nofBytes", nofBytes);
								anaTmpl.WriteCode(AnaStrm);
							}
							param = (TMrbModuleChannel *) fLofParams.After(param);
						}
					}
					break;
			}
		}
	}
	return(kTRUE);
}

Bool_t TMrbSubevent::MakeAnalyzeCode(ofstream & AnaStrm,	TMrbConfig::EMrbAnalyzeTag TagIndex,
															TMrbEvent * Event,
															TMrbTemplate & Template,
															const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::MakeAnalyzeCode
// Purpose:        Write a piece of code for data analysis
// Arguments:      ofstream & AnaStrm           -- file output stream
//                 EMrbAnalyzeTag TagIndex      -- index of tag word from template file
//                 TMrbEvent * Event            -- associated event
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code needed for data analysis.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbEvent * evt;
	TMrbModuleChannel * param;
	TMrbModule * module;
	TString evtNameLC;
	TString evtNameUC;
	TString sevtNameLC;
	TString sevtNameUC;
	TString paramNameLC;
	TString paramNameUC;

	TString prefixLC;
	TString prefixUC;
	TString prefixDotted;
	TString pUC;
	Bool_t prependPrefix;
	Bool_t pFlag = kFALSE;

	Char_t * iniS = NULL;
	Char_t * iniX = NULL;

	Bool_t stdHistosOK;
	Bool_t doIt;

	TMrbConfig::EMrbChannelStatus paramStatus;

	sevtNameLC = GetName();
	sevtNameUC = sevtNameLC;
	sevtNameUC(0,1).ToUpper();

	if (this->PrefixToBePrepended()) {
		prefixLC = fPrefix;
		prefixDotted = prefixLC;
	} else {
		prefixLC = sevtNameLC;
		prefixDotted = prefixLC + ".";
	}
	prefixUC = prefixLC;
	prefixUC(0,1).ToUpper();
	prependPrefix = this->PrefixToBePrepended() || gMrbConfig->LongParamNamesToBeUsed();

	if (this->MakeSpecialAnalyzeCode(AnaStrm, TagIndex, Template)) return(kTRUE);

	switch (TagIndex) {
		case TMrbConfig::kAnaEventSetBranchStatus:
			param = (TMrbModuleChannel *) fLofParams.First();
			while (param) {
				paramStatus = param->GetStatus();
				if (paramStatus != TMrbConfig::kChannelArrElem) {
					paramNameLC = param->GetName();
					paramNameUC = paramNameLC;
					paramNameUC(0,1).ToUpper();
					if (paramStatus == TMrbConfig::kChannelSingle)	Template.InitializeCode("%S%");
					else												Template.InitializeCode("%X%");
					if (prependPrefix)	Template.Substitute("$prefix", prefixDotted);
					else				Template.Substitute("$prefix", "");
					Template.Substitute("$treeName", Event->GetName());
					Template.Substitute("$sevtNameLC", sevtNameLC);
					Template.Substitute("$paramNameLC", paramNameLC);
					Template.Substitute("$paramNameUC", paramNameUC);
					Template.Substitute("$indexRange", (Int_t) param->GetIndexRange());
					Template.WriteCode(AnaStrm);
				}
				param = (TMrbModuleChannel *) fLofParams.After(param);
			}
			break;
		case TMrbConfig::kAnaHistoDefinePointers:
			if (this->GetAnalyzeOptions(Event) & TMrbConfig::kAnaOptHistograms) {
				stdHistosOK = kFALSE;
				evt = (TMrbEvent *) fLofEvents.First();
				while (evt) {
					evtNameLC = evt->GetName();
					evtNameUC = evtNameLC;
					evtNameUC(0,1).ToUpper();
					doIt = kFALSE;
					if (evt->HasPrivateHistograms()) {
						pFlag = kTRUE;
						pUC = evt->GetPrefix();
						pUC(0,1).ToUpper();
						iniS = "%SP%";
						iniX = "%XP%";
						if (this->IsInArrayMode()) {
							iniS = "%AP%";
							iniX = "%AP%";
						} else {
							iniS = "%SP%";
							iniX = "%XP%";
						}
						doIt = kTRUE;
					} else if (!stdHistosOK) {
						pFlag = prependPrefix;
						pUC = prefixUC;
						stdHistosOK = kTRUE;
						if (this->IsInArrayMode()) {
							iniS = "%AN%";
							iniX = "%AN%";
						} else {
							iniS = "%SN%";
							iniX = "%XN%";
						}
						doIt = kTRUE;
					} 
					if (doIt) {
						param = (TMrbModuleChannel *) fLofParams.First();
						while (param) {
							module = (TMrbModule *) param->Parent();
							if (module->HistosToBeAllocated() && module->GetRange() > 0) {
								paramStatus = param->GetStatus();
								if (paramStatus != TMrbConfig::kChannelArrElem) {
									paramNameLC = param->GetName();
									paramNameUC = paramNameLC;
									paramNameUC(0,1).ToUpper();
									if (paramStatus == TMrbConfig::kChannelSingle)	Template.InitializeCode(iniS);
									else											Template.InitializeCode(iniX);
									if (pFlag)	Template.Substitute("$prefix", pUC);
									else		Template.Substitute("$prefix", "");
									Template.Substitute("$evtNameLC", evtNameLC);
									Template.Substitute("$evtNameUC", evtNameUC);
									Template.Substitute("$sevtNameLC", sevtNameLC);
									Template.Substitute("$sevtNameUC", sevtNameUC);
									Template.Substitute("$paramNameLC", paramNameLC);
									Template.Substitute("$paramNameUC", paramNameUC);
									Template.Substitute("$indexRange", (Int_t) param->GetIndexRange());
									Template.WriteCode(AnaStrm);
								}
							}
							param = (TMrbModuleChannel *) fLofParams.After(param);
						}
					}
					if (this->IsInArrayMode()) {
						if (evt->HasPrivateHistograms()) {
							iniS = "%APX%";
							pUC = evt->GetPrefix();
							pUC(0,1).ToUpper();
						} else {
							iniS = "%ANX%";
							pUC = "";
						}
						Template.InitializeCode(iniS);
						Template.Substitute("$prefix", pUC);
						Template.Substitute("$evtNameLC", evtNameLC);
						Template.Substitute("$evtNameUC", evtNameUC);
						Template.Substitute("$sevtNameLC", sevtNameLC);
						Template.Substitute("$sevtNameUC", sevtNameUC);
						Template.Substitute("$paramNameLC", paramNameLC);
						Template.Substitute("$paramNameUC", paramNameUC);
						Template.Substitute("$nofParams", this->GetNofParams());
						Template.WriteCode(AnaStrm);
					}
					evt = (TMrbEvent *) fLofEvents.After((TObject *) evt);
				}
			}
			break;
		case TMrbConfig::kAnaHistoInitializeArrays:
			if (this->HistosToBeAllocated() && (this->GetAnalyzeOptions(Event) & TMrbConfig::kAnaOptHistograms)) {
				stdHistosOK = kFALSE;
				evt = (TMrbEvent *) fLofEvents.First();
				while (evt) {
					evtNameLC = evt->GetName();
					evtNameUC = evtNameLC;
					evtNameUC(0,1).ToUpper();
					doIt = kFALSE;
					if (evt->HasPrivateHistograms()) {
						pFlag = kTRUE;
						pUC = evt->GetPrefix();
						pUC(0,1).ToUpper();
						doIt = kTRUE;
					} else if (!stdHistosOK) {
						pFlag = prependPrefix;
						pUC = prefixUC;
						stdHistosOK = kTRUE;
						doIt = kTRUE;
					} 
					if (doIt) {
						if (this->IsInArrayMode()) {
							Template.InitializeCode("%A%");
							if (pFlag)	Template.Substitute("$prefix", pUC);
							else		Template.Substitute("$prefix", "");
							Template.Substitute("$evtNameLC", evtNameLC);
							Template.Substitute("$evtNameUC", evtNameUC);
							Template.Substitute("$sevtNameLC", sevtNameLC);
							Template.Substitute("$sevtNameUC", sevtNameUC);
							Template.Substitute("$paramNameLC", paramNameLC);
							Template.Substitute("$paramNameUC", paramNameUC);
							Template.Substitute("$nofParams", this->GetNofParams());
							Template.WriteCode(AnaStrm);
						} else {
							param = (TMrbModuleChannel *) fLofParams.First();
							while (param) {
								module = (TMrbModule *) param->Parent();
								if (module->HistosToBeAllocated() && module->GetRange() > 0) {
									paramStatus = param->GetStatus();
									if (paramStatus != TMrbConfig::kChannelArrElem) {
										paramNameLC = param->GetName();
										paramNameUC = paramNameLC;
										paramNameUC(0,1).ToUpper();
										if (paramStatus == TMrbConfig::kChannelArray) {
											Template.InitializeCode("%N%");
											if (pFlag)	Template.Substitute("$prefix", pUC);
											else		Template.Substitute("$prefix", "");
											Template.Substitute("$evtNameLC", evtNameLC);
											Template.Substitute("$evtNameUC", evtNameUC);
											Template.Substitute("$sevtNameLC", sevtNameLC);
											Template.Substitute("$sevtNameUC", sevtNameUC);
											Template.Substitute("$paramNameLC", paramNameLC);
											Template.Substitute("$paramNameUC", paramNameUC);
											Template.Substitute("$indexRange", (Int_t) param->GetIndexRange());
											Template.WriteCode(AnaStrm);
										}
									}
								}
								param = (TMrbModuleChannel *) fLofParams.After(param);
							}
						}
					}
					evt = (TMrbEvent *) fLofEvents.After((TObject *) evt);
				}
			}
			break;
	}
	return(kTRUE);
}

Bool_t TMrbSubevent::MakeConfigCode(ofstream & CfgStrm,	TMrbConfig::EMrbConfigTag TagIndex,
														TMrbTemplate & Template,
														const Char_t * Prefix) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::MakeConfigCode
// Purpose:        Write some lines to the config file
// Arguments:      ofstream & CfgStrm           -- file output stream
//                 EMrbConfigTag TagIndex       -- index of tag word from template file
//                 TMrbTemplate & Template      -- template
//                 Char_t * Prefix              -- prefix to select template code
// Results:
// Exceptions:
// Description:    Writes code into config file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * param;
	TMrbModule * module = NULL;
	TMrbModule * lmodule;

	Int_t chnNo;
	TString lofParams;

	switch (TagIndex) {
		case TMrbConfig::kCfgAssignParams:
			param = (TMrbModuleChannel *) fLofParams.First();
			lmodule = module;
			lofParams = "";
			chnNo = -1;
			while (param) {
				module = (TMrbModule *) param->Parent();
				if (lofParams.Length() > 65 || module != lmodule) {
					lofParams = lofParams.Strip(TString::kBoth);
					if (lofParams.Length() > 0) {
						Template.InitializeCode();
						Template.Substitute("$sevtNameLC", this->GetName());
						Template.Substitute("$moduleNameLC", lmodule->GetName());
						Template.Substitute("$lofParams", lofParams);
						Template.WriteCode(CfgStrm);
					}
					lmodule = module;
					lofParams = "";
					chnNo = -1;
				}
				if (module == lmodule) {
					lofParams += param->GetName();
					if (param->GetAddr() != chnNo + 1) {
						lofParams += "=";
						TMrbString xChn(param->GetAddr());
						lofParams += xChn;
					}
					chnNo = param->GetAddr();
					lofParams += " ";
				}
				param = (TMrbModuleChannel *) fLofParams.After(param);
			}
			lofParams = lofParams.Strip(TString::kBoth);
			if (lofParams.Length() > 0) {
				Template.InitializeCode();
				Template.Substitute("$sevtNameLC", this->GetName());
				Template.Substitute("$moduleNameLC", lmodule->GetName());
				Template.Substitute("$lofParams", lofParams);
				Template.WriteCode(CfgStrm);
			}
			break;
	}
	return(kTRUE);
}

void TMrbSubevent::Print(ostream & OutStrm, const Char_t * Prefix) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Print
// Purpose:        Output current subevent definitions
// Arguments:      ofstream & Out   -- output stream
//                 Char_t * Prefix  -- prefix to be output in front
// Results:        --
// Exceptions:
// Description:    Outputs current subevent definition to stdout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * param;
	TMrbEvent * evt;
	Int_t pOffs;
	
	OutStrm << Prefix << "|\n";
	OutStrm << Prefix << "+-> Subevent Definition:" << endl;
	OutStrm << Prefix << "       Name          : " << this->GetName() << endl;
	OutStrm << Prefix << "       Title         : " << this->GetTitle() << endl;
	OutStrm << Prefix << "       Type/Subtype  : [" << fSevtType << "," << fSevtSubtype << "]" << endl;
	OutStrm << Prefix << "       Description   : " << fSevtDescr << endl;
	OutStrm << Prefix << "       Trigger(s)    : ";
	evt = (TMrbEvent *) fLofEvents.First();
	while (evt) {
		OutStrm << evt->GetName() << "(" << evt->GetTrigger() << ") ";
		evt = (TMrbEvent *) fLofEvents.After(evt);
	}
	OutStrm << endl;
	OutStrm << Prefix << "       Parameters    : " << fNofParams << endl;
	if (fNofParams > 0) {
		OutStrm << Prefix << Form("%-23s%-18s%-15s%s", "", "Addr", "Name", "Module") << endl;
		param = (TMrbModuleChannel *) fLofParams.First();
		Int_t po = 0;
		while (param) {
			if (param->GetStatus() == TMrbConfig::kChannelArray) {
				param->Print(OutStrm, kTRUE, kFALSE, Prefix);
				pOffs = param->GetAddr() + param->GetIndexRange();
				po += pOffs;
				param = (po <= fLofParams.GetLast()) ? (TMrbModuleChannel *) fLofParams.At(po) : NULL;
			} else {
				param->Print(OutStrm, kFALSE, kFALSE, Prefix);
				param = (TMrbModuleChannel *) fLofParams.After(param);
			}
		}
	}
}

Bool_t TMrbSubevent::Set(const Char_t * ParamName, const Char_t * RegName, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Set
// Purpose:        Set register values
// Arguments:      Char_t * ParamName       -- parameter name
//                 Char_t * RegName         -- register name
//                 Int_t Value              -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified param
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel *param;

	if ((param = (TMrbModuleChannel *) FindParam(ParamName)) == NULL) {
		gMrbLog->Err() << fName << ": No such param name - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	return(param->Set(RegName, Value));
}

Bool_t TMrbSubevent::Set(const Char_t * ParamName, Int_t RegIndex, Int_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Set
// Purpose:        Set register values
// Arguments:      Char_t * ParamName       -- parameter name
//                 Int_t RegIndex           -- register index
//                 Int_t Value              -- value to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets register values of specified param
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel *param;

	if ((param = (TMrbModuleChannel *) FindParam(ParamName)) == NULL) {
		gMrbLog->Err() << fName << ": No such param name - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "Set");
		return(kFALSE);
	}

	return(param->Set(RegIndex, Value));
}

Int_t TMrbSubevent::Get(const Char_t * ParamName, const Char_t * RegName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Get
// Purpose:        Get register values
// Arguments:      Char_t * ParamName       -- parameter name
//                 Char_t * RegName         -- register name
// Results:        Int_t Value              -- value to be set
// Exceptions:
// Description:    Gets register values of specified param
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel *param;

	if ((param = (TMrbModuleChannel *) FindParam(ParamName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	}

	return(param->Get(RegName));
}

Int_t TMrbSubevent::Get(const Char_t * ParamName, Int_t RegIndex) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::Get
// Purpose:        Get register values
// Arguments:      Char_t * ParamName       -- parameter name
//                 Int_t RegIndex           -- register index
// Results:        Int_t Value              -- value to be set
// Exceptions:
// Description:    Gets register values of specified param
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel *param;

	if ((param = (TMrbModuleChannel *) FindParam(ParamName)) == NULL) {
		gMrbLog->Err() << this->GetName() << ": No such param - " << ParamName << endl;
		gMrbLog->Flush(this->ClassName(), "Get");
		return(-1);
	}

	return(param->Get(RegIndex));
}

Bool_t TMrbSubevent::SetReadoutOptions(Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::SetReadoutOptions
// Purpose:        Define some options to be used in MakeReadoutCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets readout options for this subevent individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX readoutOptions;

	opt = readoutOptions.CheckPatternShort(this->ClassName(), "SetReadoutOptions", Options, kMrbSevtReadoutOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fReadoutOptions = opt;
	return(kTRUE);
}

Bool_t TMrbSubevent::SetAnalyzeOptions(Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::SetAnalyzeOptions
// Purpose:        Define some options to be used in MakeAnalyzeCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets analyze options for this subevent individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX analyzeOptions;

	opt = analyzeOptions.CheckPatternShort(this->ClassName(), "SetAnalyzeOptions", Options, kMrbSevtAnalyzeOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fAnalyzeOptions = opt;
	return(kTRUE);
}

Bool_t TMrbSubevent::SetConfigOptions(Option_t * Options) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::SetConfigOptions
// Purpose:        Define some options to be used in MakeConfigCode()
// Arguments:      Char_t * Options   -- options to be set
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets config options for this subevent individually
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t opt;
	TMrbLofNamedX configOptions;

	opt = configOptions.CheckPatternShort(this->ClassName(), "SetConfigOptions", Options, kMrbSevtConfigOptions);
	if (opt == TMrbLofNamedX::kIllIndexBit) return(kFALSE);
	fConfigOptions = opt;
	return(kTRUE);
}

UInt_t TMrbSubevent::GetReadoutOptions(TMrbEvent * Parent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::GetReadoutOptions
// Purpose:        Return current options for method MakeReadoutCode()
// Arguments:      TMrbEvent * Parent -- parent event
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns readout options:
//                     local options if fReadoutOptions is set
//                     options of parent event otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fReadoutOptions == TMrbConfig::kNoOptionSpecified && Parent) return(Parent->GetReadoutOptions());
	return(fReadoutOptions);
}

UInt_t TMrbSubevent::GetAnalyzeOptions(TMrbEvent * Parent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::GetAnalyzeOptions
// Purpose:        Return current options for method MakeAnalyzeCode()
// Arguments:      TMrbEvent * Parent -- parent event
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns analyze options:
//                     local options if fAnalyzeOptions is set
//                     options of parent event otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fAnalyzeOptions == TMrbConfig::kNoOptionSpecified)
						return(Parent ? Parent->GetAnalyzeOptions() : gMrbConfig->GetAnalyzeOptions());
	else				return(fAnalyzeOptions);
}

UInt_t TMrbSubevent::GetConfigOptions(TMrbEvent * Parent) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::GetConfigOptions
// Purpose:        Return current options for method MakeConfigCode()
// Arguments:      TMrbEvent * Parent -- parent event
// Results:        UInt_t Options     -- readout options
// Exceptions:
// Description:    Returns config options:
//                     local options if fConfigOptions is set
//                     options of parent event otherwise
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fConfigOptions == TMrbConfig::kNoOptionSpecified && Parent) return(Parent->GetConfigOptions());
	return(fConfigOptions);
}

const Char_t * TMrbSubevent::GetLofModulesAsString(TString & LofModules) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::GetLofModulesAsString
// Purpose:        Return a colon-separated string with module names
// Arguments:      TString & LofModules      -- where to store module names
// Results:        Char_t * LofModules       -- string containing module names
// Exceptions:
// Description:    Returns a colon-separated string with module names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModule * module;

	LofModules = "";
	if (fNofModules > 0) {
		module = (TMrbModule *) fLofModules.First();
		while (module) {
			if (LofModules.Length() > 0) LofModules += ":";
			LofModules += module->GetName();
			module = (TMrbModule *) fLofModules.After(module);
		}
	}
	return(LofModules.Data());
}

const Char_t * TMrbSubevent::GetLofParamsAsString(TString & LofParams) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::GetLofParamsAsString
// Purpose:        Return a colon-separated string with module names
// Arguments:      TString & LofParams      -- where to store module names
// Results:        Char_t * LofParams       -- string containing module names
// Exceptions:
// Description:    Returns a colon-separated string with module names.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbModuleChannel * param;

	LofParams = "";
	if (fNofParams > 0) {
		param = (TMrbModuleChannel *) fLofParams.First();
		while (param) {
			if (LofParams.Length() > 0) LofParams += ":";
			LofParams += param->GetName();
			param = (TMrbModuleChannel *) fLofParams.After(param);
		}
	}
	return(LofParams.Data());
}

Bool_t TMrbSubevent::UseXhit(const Char_t * HitName, Int_t DataLength) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::UseXhit
// Purpose:        Define a special hit to be used instead of TUsrHit
// Arguments:      Char_t * HitName         -- name to be used
//                 Int_t DataLength         -- length if additional hit data in words
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines a new object inherited from normal TUsrHit.
//                 Adds some additional data (i.e. trace data).
//                 Code will be generated automatically depending on argument DataLength.
//                 If DataLength is missing (or = -1) no code generation will take place,
//                 it is then up to the user to provide his own class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (gMrbConfig->IsVerbose() || (gMrbConfig->GetAnalyzeOptions() & TMrbConfig::kAnaOptVerbose) != 0);

	TMrbNamedX * xHit = NULL;
	Bool_t rewriteCode = kFALSE;
	Bool_t needUserClass = kFALSE;

	fXhit = HitName;
	fXhit(0,1).ToUpper();
	fXhit.Prepend("TUsr");
	fHitDataLength = DataLength;
	if (this->NeedsHitBuffer()) {
		if (fHitDataLength != -1 && fHitDataLength <= 0) {
			gMrbLog->Err()	<< "[" << this->GetName() << "] Illegal hit data length - " << fHitDataLength << " (should be > 0)" << endl;
			gMrbLog->Flush(this->ClassName(), "UseXhit");
			xHit = NULL;
		} else {
			TObjArray * lofHits = gMrbConfig->GetLofXhits();
			xHit = (TMrbNamedX *) lofHits->FindObject(fXhit.Data());
			if (xHit) {
				if (xHit->GetIndex() == fHitDataLength) {
					TObjArray * lofSevts = (TObjArray *) xHit->GetAssignedObject();
					if (lofSevts->FindObject(this) == NULL) lofSevts->Add(this);
					rewriteCode = kFALSE;
				} else {
					if (fHitDataLength == -1) {
						fHitDataLength = xHit->GetIndex();
					} else if (fHitDataLength > xHit->GetIndex()) {
						xHit->SetIndex(fHitDataLength);
						if (verboseMode) {
							gMrbLog->Out()  << "[" << this->GetName() << "] Extending hit data length to " << fHitDataLength << endl;
							gMrbLog->Flush(this->ClassName(), "UseXhit");
						}
					}
					TObjArray * lofSevts = (TObjArray *) xHit->GetAssignedObject();
					lofSevts->Add(this);
					rewriteCode = kTRUE;
				}
			} else {
				if (fHitDataLength == -1) {
					gMrbLog->Err()	<< "[" << this->GetName() << "] Hit data length missing" << endl;
					gMrbLog->Flush(this->ClassName(), "UseXhit");
					xHit = NULL;
				} else {
					TObjArray * lofSevts = new TObjArray();
					lofSevts->Add(this);
					xHit = new TMrbNamedX(fHitDataLength, fXhit.Data(), "", lofSevts);
					lofHits->Add(xHit);
					if (verboseMode) {
						gMrbLog->Out()  << "[" << this->GetName() << "] Using special hit " << fXhit
										<< " (user defined)" << endl;
					}
					gMrbLog->Flush(this->ClassName(), "UseXhit");
					needUserClass = kTRUE;
					rewriteCode = kTRUE;
				}
			}
		}
	} else {
		gMrbLog->Err()	<< "[" << this->GetName() << "] Subevent is not configured to use a hit buffer - extended hit \""
						<< fXhit << "\" ignored" << endl;
		gMrbLog->Flush(this->ClassName(), "UseXhit");
		xHit = NULL;
	}
	if (xHit) {
		if (!rewriteCode) {
			TString xHitFile = fXhit;
			xHitFile += ".cxx";
			rewriteCode = (gSystem->AccessPathName(xHitFile.Data()) != 0);
		}
		if (!rewriteCode) {
			TString xHitFile = fXhit;
			xHitFile += ".h";
			rewriteCode = (gSystem->AccessPathName(xHitFile.Data()) != 0);
		}
		if (rewriteCode) {
			gMrbConfig->CreateXhit(xHit);
			if (needUserClass) {
				TString xHitFile = fXhit;
				xHitFile += ".cxx";
				gMrbConfig->IncludeUserClass(xHitFile.Data(), kFALSE);
			}
		}
	} else {
		fXhit = "";
		fHitDataLength = 0;
	}
	return(xHit != NULL);
}


Bool_t TMrbSubevent::SetMbsBranch(Int_t MbsBranchNo, const Char_t * MbsBranchName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSubevent::SetMbsBranch
// Purpose:        Assign subevent to a mbs branch
// Arguments:      Int_t MbsBranchNo         -- mbs branch number
//                 Char_t * BranchName       -- name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Assigns module to a mbs branch.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if(this->GetMbsBranchNo() != -1) {
		gMrbLog->Err()	<< "Subevent " << this->GetName() << " already assigned to mbs branch "
						<< fMbsBranch.GetName() << "(" << fMbsBranch.GetIndex() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMbsBranch");
		return(kFALSE);
	}
	if (!gMrbConfig->SetMbsBranch(fMbsBranch, MbsBranchNo, MbsBranchName)) {
		gMrbLog->Err()	<< "Subevent " << this->GetName() << "- can't set mbs branch" << endl;
		gMrbLog->Flush(this->ClassName(), "SetMbsBranch");
		return(kFALSE);
	}
	return(kTRUE);
}

void TMrbSubevent::Browse(TBrowser * Browser) {}
