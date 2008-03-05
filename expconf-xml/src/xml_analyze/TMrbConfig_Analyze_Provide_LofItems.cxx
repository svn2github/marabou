//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig_Analyze_Provide_LofItems.cxx
// Purpose:        MARaBOU configuration: XML code generator
//                 Called by method TMrbConfig::MakeAnalyzeCode()
//                 Provides a list of items to be processed by <foreach>...</foreach>
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfig_Analyze_Provide_LofItems.cxx,v 1.1 2008-03-05 12:37:36 Rudolf.Lutter Exp $
// Date:           
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <pwd.h>
#include <sys/types.h>

#include "TROOT.h"
#include "TKey.h"
#include "TFile.h"
#include "TEnv.h"

#include "TMrbSystem.h"
#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbLofUserVars.h"
#include "TMrbString.h"
#include "TMrbConfig.h"
#include "TMrbEvent.h"
#include "TMrbEvent_10_1.h"
#include "TMrbSubevent.h"
#include "TMrbModule.h"
#include "TMrbModuleChannel.h"
#include "TMrbCamacModule.h"
#include "TMrbVMEModule.h"
#include "TMrbCamacScaler.h"
#include "TMrbNamedArray.h"
#include "TMrbMesytec_Mux16.h"
#include "TMbsSetup.h"

#include "TMrbConfigCommon.h"

#include "SetColor.h"

extern TSystem * gSystem;

extern TMrbLofUserVars * gMrbLofUserVars; 		// a list of all user vars/windows
extern TMrbLogger * gMrbLog;					// message logger
extern TMrbXMLCodeGen * gMrbXMLCodeGen; 		// xml code generator

extern TMrbConfig * gMrbConfig;

Bool_t TMrbConfig::ProvideLofItems_Analyze(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideLofItems_Analyze
// Purpose:        Return a list of items
// Arguments:      Char_t * Element       -- element name
//                 TMrbNamedX * Tag       -- current tag
//                 Char_t * ItemName      -- name of item
//                 TString & LofItems     -- list of item values
// Results:        kTRUE/kFALSE
// Description:    Return a list of items to be processed by <foreach>...</foreach>
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";

	if (this->IsVerbose()) {
		if (ItemName)	gMrbLog->Out() << "[" << Element << "] Tag=" << Tag->GetName() << " Item=" << ItemName << endl;
		else			gMrbLog->Out() << "[" << Element << "] Tag=" << Tag->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ProvideLofItems_Analyze");
	}

	switch (Tag->GetIndex()) {

		case TMrbConfig::kAnaUserGlobals:				return(this->ProvideLofItems_AnaUserGlobals(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaHistoBookUserDefined:		return(this->ProvideLofItems_AnaHistoBookUserDefined(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaUserInitializeBeforeHB: 	return(kTRUE);
		case TMrbConfig::kAnaUserInitializeAfterHB: 	return(kTRUE);
		case TMrbConfig::kAnaClassImp:					return(this->ProvideLofItems_AnaClassImp(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaIncludeEvtSevtModGlobals:	return(this->ProvideLofItems_AnaEvtSevtModCommonCode(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaAddUserEnv:				return(this->ProvideLofItems_AnaAddUserEnv(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaHistoInitArrays:			return(this->ProvideLofItems_AnaHistoInitArrays(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaModuleInitList:			return(this->ProvideLofItems_AnaModuleInitList(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaModuleTimeOffset:			return(this->ProvideLofItems_AnaModuleTimeOffset(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventClassInstance:		return(this->ProvideLofItems_AnaEventClassInstance(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventUserClassInstance:	return(this->ProvideLofItems_AnaEventUserClassInstance(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaWdwClassInstance:			return(this->ProvideLofItems_AnaWdwClassInstance(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaWdwAdjustPointers:			return(this->ProvideLofItems_AnaWdwAdjustPointers(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaInitializeSevtMods: 		return(this->ProvideLofItems_AnaEvtSevtModCommonCode(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaHistoFillArrays:			return(this->ProvideLofItems_AnaHistoFillArrays(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventCreateTree: 			return(this->ProvideLofItems_AnaEventTreeAndPointers(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventInitializeTree: 		return(this->ProvideLofItems_AnaEventTreeAndPointers(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventSetWriteTree: 		return(this->ProvideLofItems_AnaEventTreeAndPointers(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventSetFakeMode: 			return(this->ProvideLofItems_AnaEventReplayAndScaledown(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventSetReplayMode:		return(this->ProvideLofItems_AnaEventReplayAndScaledown(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventReplayTree:			return(this->ProvideLofItems_AnaEventReplayAndScaledown(Element, Tag, ItemName, LofItems));
		case TMrbConfig::kAnaEventSetScaleDown: 		return(this->ProvideLofItems_AnaEventReplayAndScaledown(Element, Tag, ItemName, LofItems));

		default:
			gMrbLog->Err()	<< "[" << Element << "] Tag=" << Tag->GetName() << ": Not yet implemented" << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideLofItems_Analyze");
			return(kFALSE);
	}
	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideLofItems_XXX
// Purpose:        Return a list of items
// Arguments:      TMrbNamedX * Tag       -- tag
//                 Char_t * ItemName      -- item name
//                 TString & LofItems     -- list of item values
// Results:        kTRUE/kFALSE
// Description:    Return a list of items to be processed by <foreach>...</foreach>
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbConfig::ProvideLofItems_AnaUserGlobals(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            INCLUDE_USER_GLOBALS
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	if (gMrbConfig->UserCodeToBeIncluded()) {
		TIterator * iclIter = gMrbConfig->GetLofUserIncludes()->MakeIterator();
		TMrbNamedX * icl;
		while (icl = (TMrbNamedX *) iclIter->Next()) {
			if (icl->GetIndex() & TMrbConfig::kIclOptHeaderFile) LofItems += Form("%s:", icl->GetName());
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaHistoBookUserDefined(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_BOOK_USER_DEFINED
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	if (fLofUserHistograms.First()) {
		TMrbNamedX * h;
		TIterator * hIter = fLofUserHistograms.MakeIterator();
		while (h = (TMrbNamedX *) hIter->Next()) LofItems += Form("%s:", h->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaClassImp(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            IMPLEMENT_CLASSES
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";

	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) LofItems += Form("%s:", evt->GetName());

	TMrbSubevent * sevt;
	TIterator * sevtIter = fLofSubevents.MakeIterator();
	while (sevt = (TMrbSubevent *) sevtIter->Next()) LofItems += Form("%s:", sevt->GetName());

	TMrbNamedX * ucl;
	TIterator * iclIter = fLofUserClasses.MakeIterator();
	while (ucl = (TMrbNamedX *) iclIter->Next()) {
		if (ucl->GetIndex() & kIclOptUserClass) LofItems += Form("%s:", ucl->GetName());
	}

	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaEvtSevtModCommonCode(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tags:           INCLUDE_EVT_SEVT_MOD_GLOBALS
//                 INITIALIZE_EVT_SEVT_MODS
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	
	TString item = ItemName;

	TList onceOnly;
	onceOnly.Delete();

	if (item.CompareTo("event") == 0) {
		TMrbEvent * evt;
		TIterator * evtIter = fLofEvents.MakeIterator();
		while (evt = (TMrbEvent *) evtIter->Next()) {
			if (evt->GetCommonCodeFile() && onceOnly.FindObject(evt->GetCommonCodeFile()) == NULL) {
				LofItems += Form("%s:", evt->GetName());
				onceOnly.Add(new TNamed(evt->GetCommonCodeFile(), ""));
			}
		}
		return(kTRUE);
	} else if  (item.CompareTo("subevent") == 0) {
		TMrbSubevent * sevt;
		TIterator * sevtIter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) sevtIter->Next()) {
			if (sevt->GetCommonCodeFile() && onceOnly.FindObject(sevt->GetCommonCodeFile()) == NULL) {
				LofItems += Form("%s:", sevt->GetName());
				onceOnly.Add(new TNamed(sevt->GetCommonCodeFile(), ""));
			}
		}
		return(kTRUE);
	} else if  (item.CompareTo("module") == 0) {
		onceOnly.Delete();
		TMrbModule * module;
		TIterator * modIter = fLofModules.MakeIterator();
		while (module = (TMrbModule *) modIter->Next()) {
			if (module->GetCommonCodeFile() && onceOnly.FindObject(module->GetCommonCodeFile()) == NULL) {
				LofItems += Form("%s:", module->GetName());
				onceOnly.Add(new TNamed(module->GetCommonCodeFile(), ""));
			}
		}
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaAddUserEnv(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            ADD_USER_ENV
//////////////////////////////////////////////////////////////////////////////

	TString expName = this->GetName();
	expName(0,1).ToUpper();
	TString rcFile = ".";
	rcFile += expName;
	rcFile += "Config.rc";
	LofItems = rcFile;
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaHistoInitArrays(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_INIT_ARRAYS
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";

	if (gMrbConfig->GetAnalyzeOptions() & kAnaOptHistograms) {
		TMrbSubevent * sevt;
		TIterator * sevtIter = fLofSubevents.MakeIterator();
		while (sevt = (TMrbSubevent *) sevtIter->Next()) {
			TString prefix = sevt->PrefixToBePrepended() ? sevt->GetPrefix() : sevt->GetName();
			Bool_t prependPrefix = sevt->PrefixToBePrepended() || gMrbConfig->LongParamNamesToBeUsed();
			Bool_t stdHistos = kFALSE;
			TMrbEvent * evt;
			TIterator * evtIter = sevt->GetLofEvents()->MakeIterator();
			while (evt = (TMrbEvent *) evtIter->Next()) {
				Bool_t doIt = kFALSE;
				TString pfx;
				if (evt->HasPrivateHistograms()) {
					pfx = evt->GetPrefix();
					doIt = kTRUE;
				} else if (!stdHistos) {
					pfx = prependPrefix ? prefix : " ";
					stdHistos = kTRUE;
					doIt = kTRUE;
				} 
				if (doIt) {
					if (sevt->IsInArrayMode()) {
						LofItems += Form("A|%s|%s:", pfx.Data(), sevt->GetName());
					} else {
						TMrbModuleChannel * param;
						TIterator * parIter = sevt->GetLofParams()->MakeIterator();
						while (param = (TMrbModuleChannel *) parIter->Next()) {
							TMrbModule * module = (TMrbModule *) param->Parent();
							if (module->HistosToBeAllocated() && module->GetRange() > 0) {
								Int_t paramStatus = param->GetStatus();
								if (paramStatus == TMrbConfig::kChannelArray) {
									LofItems += Form("N|%s|%s|%s:", pfx.Data(), sevt->GetName(), param->GetName());
								}
							}
						}
					}
				}
			}
		}

		if (fLofHistoArrays.First()) {
			TMrbNamedX * hArray;
			TIterator * hIter = fLofHistoArrays.MakeIterator();
			while (hArray = (TMrbNamedX *) hIter->Next()) LofItems += Form("U|%s:", hArray->GetName());
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaModuleInitList(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            MODULE_INIT_LIST
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->GetNofChannelsUsed() > 0) LofItems += Form("%s:", module->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaModuleTimeOffset(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            MODULE_TIME_OFFSET
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbModule * module;
	TIterator * modIter = fLofModules.MakeIterator();
	while (module = (TMrbModule *) modIter->Next()) {
		if (module->GetTimeOffset() != 0) LofItems += Form("%s:", module->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaEventClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            EVT_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) LofItems += Form("%s:", evt->GetName());
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaEventUserClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            EVT_USER_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbNamedX * ucl;
	TIterator * uclIter = fLofUserClasses.MakeIterator();
	while (ucl = (TMrbNamedX *) uclIter->Next()) {
		if (ucl->GetIndex() == kIclOptUserDefinedEvent) LofItems += Form("%s:", ucl->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaWdwClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            WDW_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	if (gMrbLofUserVars) {
		TObject * obj = gMrbLofUserVars->First();
		while (obj) {
			UInt_t id = obj->GetUniqueID();
			UInt_t wdwType = id & kIsWindow;
			switch (wdwType) {
				case kWindowI:	LofItems += Form("I|%s:", ((TMrbWindowI *) obj)->GetName()); break;
				case kWindowF:	LofItems += Form("F|%s:", ((TMrbWindowF *) obj)->GetName()); break;
				case kWindow2D: LofItems += Form("C|%s:", ((TMrbWindow2D *) obj)->GetName()); break;
			}
			obj = gMrbLofUserVars->After(obj);
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaWdwAdjustPointers(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            WDW_ADJUST_POINTERS
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	if (gMrbLofUserVars) {
		TObject * obj = gMrbLofUserVars->First();
		while (obj) {
			UInt_t id = obj->GetUniqueID();
			UInt_t wdwType = id & kIsWindow;
			if (wdwType == kWindow2D) LofItems += Form("%s:", ((TMrbWindow2D *) obj)->GetName());
			obj = gMrbLofUserVars->After(obj);
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaHistoFillArrays(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_FILL_ARRAYS
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	this->CreateHistoArrays();
	TMrbNamedX * hArray;
	TIterator * hIter = fLofHistoArrays.MakeIterator();
	while (hArray = (TMrbNamedX *) hIter->Next()) LofItems += Form("%s:", hArray->GetName());
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaEventTreeAndPointers(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tags:            EVT_DEFINE_POINTERS
//                  EVT_CREATE_TREE
//                  EVT_INITIALIZE_TREE
//                  EVT_SET_WRITE_TREE
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) {
		if (!evt->IsReservedEvent()) LofItems += Form("E|%s:", evt->GetName());
	}
	TMrbNamedX * ucl;
	TIterator * uclIter = fLofUserClasses.MakeIterator();
	while (ucl = (TMrbNamedX *) uclIter->Next()) {
		if (ucl->GetIndex() == kIclOptUserDefinedEvent) LofItems += Form("U|%s:", ucl->GetName());
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideLofItems_AnaEventReplayAndScaledown(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
// Tags:            EVT_SET_FAKE_MODE
//                  EVT_SET_REPLAY_MODE
//                  EVT_REPLAY_TREE
//                  EVT_SET_SCALEDOWN
//////////////////////////////////////////////////////////////////////////////

	LofItems = "";
	TMrbEvent * evt;
	TIterator * evtIter = fLofEvents.MakeIterator();
	while (evt = (TMrbEvent *) evtIter->Next()) {
		TString evtName;
		switch (evt->GetTrigger()) {
			case TMrbConfig::kTriggerStartAcq:	evtName = "startEvent"; break;
			case TMrbConfig::kTriggerStopAcq:	evtName = "stopEvent"; break;
			default:							evtName = evt->GetName(); break;
		}
		LofItems += Form("E|%s:", evtName.Data());
	}
	TMrbNamedX * ucl;
	TIterator * uclIter = fLofUserClasses.MakeIterator();
	while (ucl = (TMrbNamedX *) uclIter->Next()) {
		if (ucl->GetIndex() == kIclOptUserDefinedEvent) LofItems += Form("U|%s:", ucl->GetName());
	}
	return(kTRUE);
}
