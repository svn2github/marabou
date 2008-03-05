//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig_Analyze_Provide_Subst.cxx
// Purpose:        MARaBOU configuration: XML code generator
//                 Called by method TMrbConfig::MakeAnalyzeCode()
//                 Provides a list of substitutions
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfig_Analyze_Provide_Subst.cxx,v 1.1 2008-03-05 12:37:36 Rudolf.Lutter Exp $
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

Bool_t TMrbConfig::ProvideSubst_Analyze(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideSubst_Analyze
// Purpose:        Return substitutions
// Arguments:      Char_t * Element          -- element name
//                 TMrbNamedX * Tag * Tag    -- current tag
//                 Char_t * ItemName         -- name of item
//                 Char_t * ItemData         -- item value
//                 TEnv * LofSubst           -- list of substitutions
// Results:        kTRUE/kFALSE
// Description:    Return substitutions for tag group 'analyze'
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0);

	if (verboseMode) {
		if (ItemName)	gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << " " << ItemName << "=" << ItemData << endl;
		else			gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ProvideSubst_Analyze");
	}

	switch (Tag->GetIndex()) {

		case TMrbConfig::kAnaXmlRoot:					return(this->ProvideSubst_AnaXmlRoot(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserGlobals:	 			return(this->ProvideSubst_AnaUserGlobals(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaHistoBookUserDefined:	 	return(this->ProvideSubst_AnaHistoBookUserDefined(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserInitializeBeforeHB:
		case TMrbConfig::kAnaUserInitializeAfterHB: 	return(this->ProvideSubst_AnaUserInitializeHB(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaClassImp:  				return(this->ProvideSubst_AnaClassImp(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaIncludeEvtSevtModGlobals: 	return(this->ProvideSubst_AnaEvtSevtModCommonCode(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaAddUserEnv: 				return(this->ProvideSubst_AnaAddUserEnv(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaHistoInitArrays: 			return(this->ProvideSubst_AnaHistoInitArrays(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaModuleInitList: 			return(this->ProvideSubst_AnaModuleInitList(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaModuleTimeOffset:  		return(this->ProvideSubst_AnaModuleTimeOffset(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventClassInstance: 		return(this->ProvideSubst_AnaEventClassInstance(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventUserClassInstance: 	return(this->ProvideSubst_AnaEventUserClassInstance(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaWdwClassInstance:  		return(this->ProvideSubst_AnaWdwClassInstance(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaWdwAdjustPointers:  		return(this->ProvideSubst_AnaWdwAdjustPointers(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaInitializeSevtMods:	 	return(this->ProvideSubst_AnaEvtSevtModCommonCode(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaHistoFillArrays:		 	return(this->ProvideSubst_AnaHistoFillArrays(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserDummyReloadParams: 	return(this->ProvideSubst_AnaDummyMethods(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserDummyFinishRun:		return(this->ProvideSubst_AnaDummyMethods(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserDummyMessages:		 	return(this->ProvideSubst_AnaDummyMethods(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaUserDummyInitialize:		return(this->ProvideSubst_AnaDummyMethods(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventCreateTree:			return(this->ProvideSubst_AnaEventTreeAndPointers(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventInitializeTree:		return(this->ProvideSubst_AnaEventTreeAndPointers(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventSetWriteTree:			return(this->ProvideSubst_AnaEventTreeAndPointers(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventSetFakeMode:			return(this->ProvideSubst_AnaEventReplayAndScaledown(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventSetReplayMode:		return(this->ProvideSubst_AnaEventReplayAndScaledown(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventReplayTree:			return(this->ProvideSubst_AnaEventReplayAndScaledown(Element, Tag, ItemName, ItemData, LofSubst));
		case TMrbConfig::kAnaEventSetScaleDown:			return(this->ProvideSubst_AnaEventReplayAndScaledown(Element, Tag, ItemName, ItemData, LofSubst));

		default:
			gMrbLog->Err()	<< "[" << Element << "] Tag=" << Tag->GetName() << ": Not yet implemented" << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideSubst_Analyze");
			return(kFALSE);
	}

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideSubst_XXX
// Purpose:        Return substitutions
// Arguments:      TMrbNamedX * Tag   -- tag
//                 Char_t * ItemName  -- item name
//                 Char_t * ItemData  -- data
//                 TEnv * LofSubst    -- list of substitutions
// Results:        kTRUE/kFALSE
// Description:    Return substitutions for tag group 'analyze'
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbConfig::ProvideSubst_AnaXmlRoot(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            ANA_XML_ROOT
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	this->Substitute(LofSubst, "xprefix", this->GetName());
	this->Substitute(LofSubst, "expTitle", this->GetTitle());
	this->Substitute(LofSubst, "author", this->GetAuthor());
	this->Substitute(LofSubst, "mailAddr", this->GetMailAddr());
	this->Substitute(LofSubst, "URL", this->GetURL());
	this->Substitute(LofSubst, "purpose", this->GetTitle());
	LofSubst->SetValue("@Request.OK", kTRUE);
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaUserGlobals(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            INCLUDE_USER_GLOBALS
//////////////////////////////////////////////////////////////////////////////

	this->Substitute(LofSubst, "iclFile", ItemData);
	LofSubst->SetValue("@Request.OK", kTRUE);
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaUserInitializeHB(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            INCLUDE_USER_INITIALIZE_XXX_HB
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (this->UserCodeToBeIncluded()) {
		TIterator * iclIter = fLofUserIncludes.MakeIterator();
		TMrbNamedX * icl;
		while (icl = (TMrbNamedX *) iclIter->Next()) {
			if ((icl->GetIndex() & TMrbConfig::kIclOptInitialize) == TMrbConfig::kIclOptInitialize) {
				TMrbLofNamedX * lofMethods = (TMrbLofNamedX *) icl->GetAssignedObject();
				TMrbNamedX * nx = (TMrbNamedX *) lofMethods->First();
				while (nx) {
					if ((nx->GetIndex() & TMrbConfig::kIclOptInitialize) == TMrbConfig::kIclOptInitialize) {
						TString method = nx->GetName();
						Int_t n = method.Index("::", 0);
						if (n >= 0) method = method(n + 2, method.Length() - n - 2);
						this->Substitute(LofSubst, "initUcode", method.Data());
						LofSubst->SetValue("@Request.OK", kTRUE);
						return(kTRUE);
					}
				}
			}
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaHistoBookUserDefined(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_BOOK_USER_DEFINED
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (fLofUserHistograms.First()) {
		TMrbNamedX * h = (TMrbNamedX *) fLofUserHistograms.FindObject(ItemData);
		if (h) {
			Bool_t hasArgStr = (h->GetAssignedObject())->InheritsFrom("TObjString");
			TMrbNamedX * cnd = (TMrbNamedX *) fLofHistoConditions.FindObject(h->GetName());
			if (hasArgStr) {
				if (cnd == NULL) {
					this->Substitute(LofSubst, "bookingMode", "UDS");
				} else {
					this->Substitute(LofSubst, "bookingMode", "UDSC");
				}
			} else {
				if (cnd == NULL) {
					this->Substitute(LofSubst, "bookingMode", (h->GetIndex() & TMrbConfig::kHistoTH1) ? "UD1" : "UD2");
				} else {
					this->Substitute(LofSubst, "bookingMode", (h->GetIndex() & TMrbConfig::kHistoTH1) ? "UD1C" : "UD2C");
					this->Substitute(LofSubst, "condition", cnd->GetTitle());
				}
			}
			this->Substitute(LofSubst, "hName", h->GetName());
			this->Substitute(LofSubst, "hTitle", h->GetTitle());
			if (hasArgStr) {
				TString argStr = ((TObjString *) h->GetAssignedObject())->GetString();
				this->Substitute(LofSubst, "hType", (fLofHistoTypes.FindByIndex(h->GetIndex()))->GetName());
				this->Substitute(LofSubst, "args", argStr);
			} else {
				TMrbNamedArrayD * a = (TMrbNamedArrayD *) h->GetAssignedObject();
				this->Substitute(LofSubst, "hType", a->GetName());
				Double_t * ap = a->GetArray();
				this->Substitute(LofSubst, "binSizeX", (Int_t) *ap++);
				this->Substitute(LofSubst, "lowerX", *ap++);
				this->Substitute(LofSubst, "upperX", *ap++);
				if (h->GetIndex() & TMrbConfig::kHistoTH2) {
					this->Substitute(LofSubst, "binSizeY", (Int_t) *ap++);
					this->Substitute(LofSubst, "lowerY", *ap++);
					this->Substitute(LofSubst, "upperY", *ap++);
				}
			}
		}
	}
	LofSubst->SetValue("@Request.OK", kTRUE);
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaClassImp(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            IMPLEMENT_CLASSES
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);

	TString className;

	TMrbEvent *	evt = (TMrbEvent *) fLofEvents.FindObject(ItemData);
	if (evt) {
		className = evt->GetName();
		className(0,1).ToUpper();
		className.Prepend("TUsrEvt");
		this->Substitute(LofSubst, "cname", className.Data());
		return(kTRUE);
	}
	TMrbSubevent * sevt = (TMrbSubevent *) fLofSubevents.FindObject(ItemData);
	if (sevt) {
		className = sevt->GetName();
		className(0,1).ToUpper();
		className.Prepend("TUsrSevt");
		this->Substitute(LofSubst, "cname", className.Data());
		return(kTRUE);
	}
	TMrbNamedX * ucl = (TMrbNamedX *) fLofUserClasses.FindByName(ItemData);
	if (ucl) {
		this->Substitute(LofSubst, "cname", ucl->GetName());
		return(kTRUE);
	}
	return(kFALSE);
}

Bool_t TMrbConfig::ProvideSubst_AnaAddUserEnv(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            ADD_USER_ENV
//////////////////////////////////////////////////////////////////////////////

	this->Substitute(LofSubst, "rcFile", ItemData);
	LofSubst->SetValue("@Request.OK", kTRUE);
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaHistoInitArrays(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_INIT_ARRAYS
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);

	TString itemData = ItemData;

	if (itemData.BeginsWith("N|") || itemData.BeginsWith("A|")) {
		TObjArray * ia = itemData.Tokenize("|");
		Int_t n = ia->GetEntries();
		TString arrayType = ((TObjString *) ia->At(0))->GetString();
		TString prefix = ((TObjString *) ia->At(1))->GetString();
		prefix = prefix.Strip(TString::kBoth);
		TString sevtName = ((TObjString *) ia->At(2))->GetString();
		TString paramName = (n == 4) ? ((TObjString *) ia->At(3))->GetString() : "???";
		delete ia;
		TMrbSubevent * sevt = (TMrbSubevent *) fLofSubevents.FindObject(sevtName.Data());
		if (sevt) {
			this->Substitute(LofSubst, "arrayType", arrayType.Data());
			this->Substitute(LofSubst, "sevtName", sevtName.Data());
			this->Substitute(LofSubst, "paramName", paramName.Data());
			this->Substitute(LofSubst, "hprefix", prefix.Data());
			this->Substitute(LofSubst, "nofParams", sevt->GetNofParams());
			TMrbModuleChannel * param = (TMrbModuleChannel *) sevt->GetLofParams()->FindObject(paramName.Data());
			this->Substitute(LofSubst, "indexRange", param ? (Int_t) param->GetIndexRange() : 0);
			LofSubst->SetValue("@Request.OK", kTRUE);
			return(kTRUE);
		}
	} else if (itemData.BeginsWith("U|")) {
		itemData = itemData(2, itemData.Length() - 2);
		TMrbNamedX * hArray = (TMrbNamedX *) fLofHistoArrays.FindObject(itemData.Data());
		if (hArray) {
			TObjArray * lofHistos = (TObjArray *) hArray->GetAssignedObject();
			this->Substitute(LofSubst, "arrayType", "U");
			this->Substitute(LofSubst, "hArrayName", hArray->GetName());
			this->Substitute(LofSubst, "nofHistos", lofHistos->GetEntriesFast());
			LofSubst->SetValue("@Request.OK", kTRUE);
			return(kTRUE);
		}
	}

	return(kFALSE);
}

Bool_t TMrbConfig::ProvideSubst_AnaModuleInitList(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            MODULE_INIT_LIST
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (ItemName == NULL) {
		Int_t nofParams = 0;
		TMrbModule * module;
		TIterator * modIter = fLofModules.MakeIterator();
		while (module = (TMrbModule *) modIter->Next()) nofParams += module->GetNofChannelsUsed();
		this->Substitute(LofSubst, "nofParams", nofParams);
		this->Substitute(LofSubst, "nofModules", this->GetNofModules());
		LofSubst->SetValue("@Request.OK", kTRUE);
		return(kTRUE);
	} else {
		static Int_t nofParams;
		if (LofSubst->GetValue("@Request.First", kFALSE)) nofParams = 0;
		LofSubst->SetValue("@Request.First", kFALSE);
		TMrbModule * module = (TMrbModule *)  fLofModules.FindObject(ItemData);
		if (module) {
			this->Substitute(LofSubst, "moduleName", module->GetName());
			this->Substitute(LofSubst, "moduleTitle", module->GetTitle());
			this->Substitute(LofSubst, "paramIndex", nofParams);
			this->Substitute(LofSubst, "nofParamsUsed", module->GetNofChannelsUsed());
			nofParams += module->GetNofChannelsUsed();
			LofSubst->SetValue("@Request.OK", kTRUE);
			return(kTRUE);
		}
	}
	return(kFALSE);
}

Bool_t TMrbConfig::ProvideSubst_AnaModuleTimeOffset(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            MODULE_TIME_OFFSET
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TMrbModule * module = (TMrbModule *) fLofModules.FindObject(ItemData);
	if (module) {
		this->Substitute(LofSubst, "moduleName", module->GetName());
		this->Substitute(LofSubst, "moduleTitle", module->GetTitle());
		this->Substitute(LofSubst, "timeOffset", module->GetTimeOffset());
		LofSubst->SetValue("@Request.OK", kFALSE);
		return(kTRUE);
	} else {
		return(kFALSE);
	}
}

Bool_t TMrbConfig::ProvideSubst_AnaEventClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            EVT_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TMrbEvent * evt = (TMrbEvent *) fLofEvents.FindObject(ItemData);
	if (evt) {
		this->Substitute(LofSubst, "evtName", evt->GetName());
		this->Substitute(LofSubst, "ptrName", evt->GetPointerName());
		this->Substitute(LofSubst, "trigNo", (Int_t) evt->GetTrigger());
		TIterator * sevtIter = evt->GetLofSubevents()->MakeIterator();
		TMrbSubevent * sevt;
		Bool_t hbFlag = kFALSE;
		while (sevt = (TMrbSubevent *) sevtIter->Next()) {
			if (sevt->NeedsHitBuffer()) {
				hbFlag = kTRUE;
				break;
			}
		}
		this->Substitute(LofSubst, "HBflag", hbFlag);
		LofSubst->SetValue("@Request.OK", kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaEventUserClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            EVT_USER_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TMrbNamedX * ucl = (TMrbNamedX *) fLofUserClasses.FindObject(ItemData);
	if (ucl) {
		this->Substitute(LofSubst, "className", ucl->GetName());
		this->Substitute(LofSubst, "classTitle", ucl->GetTitle());
		LofSubst->SetValue("@Request.OK", kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaWdwClassInstance(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            WDW_CLASS_INSTANCE
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (gMrbLofUserVars == NULL) return(kTRUE);
	TString itemData = ItemData;
	TString wdwName = itemData(2, itemData.Length() - 2);
	if (itemData.BeginsWith("I|")) {
		TMrbWindowI * wdw = (TMrbWindowI *) gMrbLofUserVars->Find(wdwName.Data());
		if (wdw) {
			this->Substitute(LofSubst, "wdwName", wdwName.Data());
			this->Substitute(LofSubst, "wdwLower", wdw->GetLowerLimit());
			this->Substitute(LofSubst, "wdwUpper", wdw->GetUpperLimit());
			this->Substitute(LofSubst, "wdwType", "1D");
			this->Substitute(LofSubst, "wdwClass", "TMrbWindowI");
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	} else if (itemData.BeginsWith("F|")) {
		TMrbWindowF * wdw = (TMrbWindowF *) gMrbLofUserVars->Find(wdwName.Data());
		if (wdw) {
			this->Substitute(LofSubst, "wdwName", wdwName.Data());
			this->Substitute(LofSubst, "wdwLower", wdw->GetLowerLimit());
			this->Substitute(LofSubst, "wdwUpper", wdw->GetUpperLimit());
			this->Substitute(LofSubst, "wdwType", "1D");
			this->Substitute(LofSubst, "wdwClass", "TMrbWindowF");
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	} else if (itemData.BeginsWith("C|")) {
		TMrbWindow2D * wdw = (TMrbWindow2D *) gMrbLofUserVars->Find(wdwName.Data());
		if (wdw) {
			this->Substitute(LofSubst, "wdwName", wdwName.Data());
			this->Substitute(LofSubst, "wdwType", "2D");
			this->Substitute(LofSubst, "wdwClass", "TMrbWindow2D");
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaWdwAdjustPointers(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            WDW_ADJUST_POINTERS
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (gMrbLofUserVars == NULL) return(kTRUE);
	TMrbWindow2D * wdw = (TMrbWindow2D *) gMrbLofUserVars->Find(ItemData);
	if (wdw) {
		this->Substitute(LofSubst, "wdwName", ItemData);
		this->Substitute(LofSubst, "wdwClass", "TMrbWindow2D");
		LofSubst->SetValue("@Request.OK", kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaHistoFillArrays(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_FILL_ARRAYS
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0);

	LofSubst->SetValue("@Request.OK", kFALSE);
	TMrbNamedX * hArray = (TMrbNamedX *) fLofHistoArrays.FindObject(ItemData);
	if (hArray) {
		ofstream list(hArray->GetTitle(), ios::out);
		if (!list.good()) {
			gMrbLog->Err() << "[" << Element << "] Tag=" << Tag->GetName() << ": " << gSystem->GetError() << " - " << hArray->GetTitle() << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaHistoFillArrays");
		} else {
			TObjArray * lofHistos = (TObjArray *) hArray->GetAssignedObject();
			TMrbNamedX * h;
			TIterator * hIter = lofHistos->MakeIterator();
			while (h = (TMrbNamedX *) hIter->Next()) {
				if (this->HistogramExists(h->GetName())) {
					this->Substitute(LofSubst, "hArrayName", hArray->GetName());
					this->Substitute(LofSubst, "hName", h->GetName());
					this->Substitute(LofSubst, "hTitle", h->GetTitle());
					list << h->GetName() << endl;
				} else {
					gMrbLog->Err() << "[" << Element << "] Tag=" << Tag->GetName() << ": No such histogram - " << h->GetName() << endl;
					gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaHistoFillArrays");
				}
			}
			if (verboseMode) {
				gMrbLog->Out()  << "[" << Element << "] Tag=" << Tag->GetName() << ": Creating histo list file - " << hArray->GetTitle()
								<< " (" << lofHistos->GetEntriesFast() << " entries)"
								<< endl;
				gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaHistoFillArrays");
			}
			list.close();
		}
		LofSubst->SetValue("@Request.OK", kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaDummyMethods(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tags:           DUMMY_INITIALIZE
//                 DUMMY_RELOAD_PARAMS
//                 DUMMY_FINISH_RUN
//                 DUMMY_HANDLE_MESSAGES
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0);

	LofSubst->SetValue("@Request.OK", kFALSE);
	if (this->UserCodeToBeIncluded()) {
		UInt_t iclOpt = 0xFFFFFFFF;
		switch (Tag->GetIndex()) {
			case kAnaUserDummyReloadParams: 	iclOpt = TMrbConfig::kIclOptReloadParams; break;
			case kAnaUserDummyFinishRun: 		iclOpt = TMrbConfig::kIclOptFinishRun; break;
			case kAnaUserDummyMessages:			iclOpt = TMrbConfig::kIclOptHandleMessages; break;
			case kAnaUserDummyInitialize:		iclOpt = TMrbConfig::kIclOptInitialize; break;
		}
		Bool_t userCode = kFALSE;
		TIterator * iclIter = fLofUserIncludes.MakeIterator();
		TMrbNamedX * icl;
		while (icl = (TMrbNamedX *) iclIter->Next()) {
			if ((icl->GetIndex() & iclOpt) == iclOpt) {
				userCode = kTRUE;
				if (verboseMode) {
					gMrbLog->Out() << "[" << Element << "] Tag=" << Tag->GetName() << ": User will provide own code" << endl;
					gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaDummyMethods");
				}
			} else if (verboseMode) {
				gMrbLog->Out()  << "[" << Element << "] Tag=" << Tag->GetName() << ": Using dummy method provided by system" << endl;
				gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaDummyMethods");
			}
		}
		if (!userCode) LofSubst->SetValue("@Request.OK", kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaEventTreeAndPointers(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tags:            EVT_DEFINE_POINTERS
//                  EVT_CREATE_TREE
//                  EVT_INITIALIZE_TREE
//                  EVT_SET_WRITE_TREE
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TString itemData = ItemData;
	TString evtName = itemData(2, itemData.Length() - 2);
	if (itemData.BeginsWith("E|")) {
		TMrbEvent * evt = (TMrbEvent *) fLofEvents.FindObject(evtName.Data());
		if (evt) {
			this->Substitute(LofSubst, "eventType", "E");
			this->Substitute(LofSubst, "evtName", evtName.Data());
			this->Substitute(LofSubst, "trigNo", evt->GetTrigger());
			this->Substitute(LofSubst, "className", evt->GetPointerName());
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	} else if (itemData.BeginsWith("U|")) {
		TMrbNamedX * ucl = (TMrbNamedX *) fLofUserClasses.FindObject(evtName.Data());
		if (ucl) {
			this->Substitute(LofSubst, "eventType", "U");
			this->Substitute(LofSubst, "className", ucl->GetName());
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaEventReplayAndScaledown(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tags:            EVT_SET_FAKE_MODE
//                  EVT_SET_REPLAY_MODE
//                  EVT_REPLAY_TREE
//                  EVT_SET_SCALEDOWN
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TString itemData = ItemData;
	TString evtName = itemData(2, itemData.Length() - 2);
	if (itemData.BeginsWith("E|")) {
		TMrbEvent * evt = (TMrbEvent *) fLofEvents.FindObject(evtName.Data());
		if (evt) {
			this->Substitute(LofSubst, "eventType", "E");
			this->Substitute(LofSubst, "evtName", evtName.Data());
			this->Substitute(LofSubst, "trigNo", evt->GetTrigger());
			this->Substitute(LofSubst, "className", evt->GetPointerName());
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	} else if (itemData.BeginsWith("U|")) {
		TMrbNamedX * ucl = (TMrbNamedX *) fLofUserClasses.FindObject(evtName.Data());
		if (ucl) {
			this->Substitute(LofSubst, "eventType", "U");
			this->Substitute(LofSubst, "className", ucl->GetName());
			LofSubst->SetValue("@Request.OK", kTRUE);
		}
	}
	return(kTRUE);
}

Bool_t TMrbConfig::ProvideSubst_AnaEvtSevtModCommonCode(const Char_t * Element, TMrbNamedX * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
// Tags:           INCLUDE_EVT_SEVT_MOD_GLOBALS
//                 INITIALIZE_EVT_SEVT_MODS
//////////////////////////////////////////////////////////////////////////////

	LofSubst->SetValue("@Request.OK", kFALSE);
	TString itemName = ItemName;
	if (itemName.CompareTo("event") == 0) {
		TMrbEvent * evt = (TMrbEvent *) fLofEvents.FindObject(ItemData);
		if (evt == NULL) {
			gMrbLog->Err()	<< "[" << Element << "] No such event - " << ItemData << " (tag=" << Tag->GetName() << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaEvtSevtModCommonCode");
			return(kFALSE);
		}
		LofSubst->SetValue("evtName.Value", evt->GetName());
		LofSubst->SetValue("codeFile.Value", Form("%s%s.xml", evt->GetCommonCodeFile(), fPck->GetX().Data()));
	} else if (itemName.CompareTo("subevent") == 0) {
		TMrbSubevent * sevt = (TMrbSubevent *) fLofSubevents.FindObject(ItemData);
		if (sevt == NULL) {
			gMrbLog->Err()	<< "[" << Element << "] No such subevent - " << ItemData << " (tag=" << Tag->GetName() << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaEvtSevtModCommonCode");
			return(kFALSE);
		}
		LofSubst->SetValue("sevtName.Value", sevt->GetName());
		LofSubst->SetValue("codeFile.Value", Form("%s%s.xml", sevt->GetCommonCodeFile(), fPck->GetX().Data()));
	} else if (itemName.CompareTo("module") == 0) {
		TMrbModule * module = (TMrbModule *) fLofModules.FindObject(ItemData);
		if (module == NULL) {
			gMrbLog->Err()	<< "[" << Element << "] No such module - " << ItemData << " (tag=" << Tag->GetName() << ")" << endl;
			gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaEvtSevtModCommonCode");
			return(kFALSE);
		}
		LofSubst->SetValue("moduleName.Value", module->GetName());
		LofSubst->SetValue("codeFile.Value", Form("%s%s.xml", module->GetCommonCodeFile(), fPck->GetX().Data()));
	} else {
		if (itemName.IsNull()) itemName = "<NULL>";
		gMrbLog->Err()	<< "[" << Element << "] Wrong item name - " << itemName << endl;
		gMrbLog->Flush(this->ClassName(), "ProvideSubst_AnaEvtSevtModCommonCode");
		return(kFALSE);
	}

	LofSubst->SetValue("@Request.OK", kTRUE);
	LofSubst->Print();
	getchar();
	return(kTRUE);
}


