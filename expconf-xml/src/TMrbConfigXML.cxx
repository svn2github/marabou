//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfigXML.cxx
// Purpose:        MARaBOU configuration: XML code generator
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfigXML.cxx,v 1.1 2008-03-05 12:37:16 Rudolf.Lutter Exp $
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

Bool_t TMrbXMLCodeClient::ProvideLofItems(const Char_t * Element, const Char_t * Tag, const Char_t * ItemName, TString & LofItems) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient::ProvideLofItems
// Purpose:        Return a list of items
// Arguments:      Char_t * Element   -- element name
//                 Char_t * Tag       -- current tag
//                 Char_t * ItemName  -- name of item
//                 TString & LofItems -- list of item values
// Results:        kTRUE/kFALSE
// Description:    Return list of items
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * tagIdx = gMrbConfig->fLofTags.FindByName(Tag);
	if (tagIdx == NULL) {
		if (ItemName)	gMrbLog->Err()	<< "Request from element <" << Element << ">, item=" << ItemName << ":" << endl;
		else			gMrbLog->Err()	<< "Request from element <" << Element << ">:" << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideLofItems");
		gMrbLog->Err()	<< Form("%*sNo such tag - ", sizeof("TMrbXMLCodeClient::"), "") << Tag << endl;
		gMrbLog->Flush();
		return(kFALSE);
	}

	Int_t tagGroup = tagIdx->GetIndex() & 0xF000;
	switch (tagGroup) {
		case TMrbConfig::kRdoXmlRoot:		break;
		case TMrbConfig::kAnaXmlRoot:		return(gMrbConfig->ProvideLofItems_Analyze(Element, tagIdx, ItemName, LofItems));
		case TMrbConfig::kCfgXmlRoot:		break;
		case TMrbConfig::kRcXmlRoot:		break;
		case TMrbConfig::kUmaXmlRoot:		break;
		case TMrbConfig::kUevXmlRoot:		break;
		case TMrbConfig::kXhitXmlRoot:		break;
		case TMrbConfig::kModuleXmlRoot:	break;
	}
	return(kFALSE);
}

Bool_t TMrbXMLCodeClient::ProvideSubst(const Char_t * Element, const Char_t * Tag, const Char_t * ItemName, const Char_t * ItemData, TEnv * LofSubst) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient::ProvideSubst
// Purpose:        Return substitutions
// Arguments:      Char_t * Element   -- element name
//                 Char_t * Tag       -- current tag
//                 Char_t * ItemName  -- name of item
//                 Char_t * ItemData  -- item value
//                 TEnv * LofSubst    -- list of substitutions
// Results:        kTRUE/kFALSE
// Description:    Return substitutions
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * tx = gMrbConfig->fLofTags.FindByName(Tag);
	if (tx == NULL) {
		if (ItemName)	gMrbLog->Err()	<< "Request from element <" << Element << ">, " << ItemName << "=" << ItemData << ":" << endl;
		else			gMrbLog->Err()	<< "Request from element <" << Element << ">:" << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideSubst");
		gMrbLog->Err()	<< "No such tag - " << Tag << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideSubst");
		return(kFALSE);
	}

	Int_t tagGroup = tx->GetIndex() & 0xF000;
	switch (tagGroup) {
		case TMrbConfig::kRdoXmlRoot:		break;
		case TMrbConfig::kAnaXmlRoot:		return(gMrbConfig->ProvideSubst_Analyze(Element, tx, ItemName, ItemData, LofSubst));
		case TMrbConfig::kCfgXmlRoot:		break;
		case TMrbConfig::kRcXmlRoot:		break;
		case TMrbConfig::kUmaXmlRoot:		break;
		case TMrbConfig::kUevXmlRoot:		break;
		case TMrbConfig::kXhitXmlRoot:		break;
		case TMrbConfig::kModuleXmlRoot:	break;
	}
	return(kFALSE);
}

Bool_t TMrbXMLCodeClient::ProvideConditionFlag(const Char_t * Element, const Char_t * Tag, const Char_t * FlagName, TString & FlagValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient::ProvideConditionFlag
// Purpose:        Return a condition
// Arguments:      Char_t * Element      -- element name
//                 Char_t * Tag          -- current tag
//                 Char_t * FlagName     -- name of condition
//                 TString & FlagValue   -- value
// Results:        kTRUE/kFALSE
// Description:    Return conditionthis->GetName()
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * tx = gMrbConfig->fLofTags.FindByName(Tag);
	if (tx == NULL) {
		if (FlagName)	gMrbLog->Err()	<< "Request from element <" << Element << ">, flag=" << FlagName << ":" << endl;
		else			gMrbLog->Err()	<< "Request from element <" << Element << ">:" << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideConditionFlag");
		gMrbLog->Err()	<< "No such tag - " << Tag << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideConditionFlag");
		return(kFALSE);
	}

	Int_t tagGroup = tx->GetIndex() & 0xF000;
	switch (tagGroup) {
		case TMrbConfig::kRdoXmlRoot:		break;
		case TMrbConfig::kAnaXmlRoot:		return(gMrbConfig->ProvideConditionFlag_Analyze(Element, tx, FlagName, FlagValue));
		case TMrbConfig::kCfgXmlRoot:		break;
		case TMrbConfig::kRcXmlRoot:		break;
		case TMrbConfig::kUmaXmlRoot:		break;
		case TMrbConfig::kUevXmlRoot:		break;
		case TMrbConfig::kXhitXmlRoot:		break;
		case TMrbConfig::kModuleXmlRoot:	break;
	}
	return(kFALSE);
}

Bool_t TMrbXMLCodeClient::ProvideCode(const Char_t * Element, const Char_t * Tag, TEnv * LofSubst,
																const Char_t * ItemName, const Char_t * Item,
																TString & Code) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient::ProvideCode
// Purpose:        Return a piece of code
// Arguments:      Char_t * Element        -- element name
//                 Char_t * Tag            -- current tag
//                 TEnv * LofSubst         -- list of substitutions
//                 char_t * ItemName       -- item name
//                 char_t * Item           -- item value
//                 TString & Code          -- where to place code
// Results:        kTRUE/kFALSE
// Description:    Return code
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * tx = gMrbConfig->fLofTags.FindByName(Tag);
	if (tx == NULL) {
		if (Item)	gMrbLog->Err()	<< "Request from element <" << Element << ">, item=" << Item << ":" << endl;
		else		gMrbLog->Err()	<< "Request from element <" << Element << ">:" << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideCode");
		gMrbLog->Err()	<< "No such tag - " << Tag << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ProvideCode");
		return(kFALSE);
	}

	Int_t tagGroup = tx->GetIndex() & 0xF000;
	switch (tagGroup) {
		case TMrbConfig::kRdoXmlRoot:		break;
		case TMrbConfig::kAnaXmlRoot:		return(gMrbConfig->ProvideCode_Analyze(Element, tx, LofSubst, ItemName, Item, Code));
		case TMrbConfig::kCfgXmlRoot:		break;
		case TMrbConfig::kRcXmlRoot:		break;
		case TMrbConfig::kUmaXmlRoot:		break;
		case TMrbConfig::kUevXmlRoot:		break;
		case TMrbConfig::kXhitXmlRoot:		break;
		case TMrbConfig::kModuleXmlRoot:	break;
	}
	return(kFALSE);
}

Bool_t TMrbXMLCodeClient::ExecuteTask(const Char_t * Element, const Char_t * Tag, const Char_t * Subtag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbXMLCodeClient::ExecuteTask
// Purpose:        Execute task
// Arguments:      Char_t * Element        -- element name
//                 Char_t * Tag            -- current tag
//                 char_t * Subtag         -- subtag
// Results:        kTRUE/kFALSE
// Description:    Executes external task
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * tx = gMrbConfig->fLofTags.FindByName(Tag);
	if (tx == NULL) {
		if (Subtag)	gMrbLog->Err()	<< "Request from element <" << Element << ">, subtag=" << Subtag << ":" << endl;
		else		gMrbLog->Err()	<< "Request from element <" << Element << ">:" << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ExecuteTask");
		gMrbLog->Err()	<< "No such tag - " << Tag << endl;
		gMrbLog->Flush(gMrbConfig->ClassName(), "ExecuteTask");
		return(kFALSE);
	}

	Int_t tagGroup = tx->GetIndex() & 0xF000;
	switch (tagGroup) {
		case TMrbConfig::kRdoXmlRoot:		break;
		case TMrbConfig::kAnaXmlRoot:		return(gMrbConfig->ExecuteTask_Analyze(Element, tx, Subtag));
		case TMrbConfig::kCfgXmlRoot:		break;
		case TMrbConfig::kRcXmlRoot:		break;
		case TMrbConfig::kUmaXmlRoot:		break;
		case TMrbConfig::kUevXmlRoot:		break;
		case TMrbConfig::kXhitXmlRoot:		break;
		case TMrbConfig::kModuleXmlRoot:	break;
	}
	return(kFALSE);
}

Bool_t TMrbConfig::Substitute(TEnv * LofSubst, const Char_t * SubstName, const Char_t * SubstValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Substitute
// Purpose:        Set a value in list of Substitutions
// Arguments:      TEnv * LofSubst       -- list of substitutions
//                 Char_t * SubstName    -- name of substitution
//                 Char_t * SubstValue   -- value
// Results:        kTRUE/kFALSE
// Description:    Substitute char string
//////////////////////////////////////////////////////////////////////////////

	TString res = Form("%s.Value", SubstName);
	TString sval = LofSubst->GetValue(res.Data(), "undef");
	if (sval.CompareTo("undef") == 0) {
		gMrbLog->Err()	<< "No such substitution - " << SubstName << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
		return(kFALSE);
	}
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Substituting " << SubstName << " by >>" << SubstValue << "<<" << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
	}
	LofSubst->SetValue(res.Data(), SubstValue);
	return(kTRUE);
}

Bool_t TMrbConfig::Substitute(TEnv * LofSubst, const Char_t * SubstName, Int_t SubstValue, const Char_t * Format) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Substitute
// Purpose:        Set a value in list of Substitutions
// Arguments:      TEnv * LofSubst       -- list of substitutions
//                 Char_t * SubstName    -- name of substitution
//                 Int_t  SubstValue     -- value
// Results:        kTRUE/kFALSE
// Description:    Substitute integer value
//////////////////////////////////////////////////////////////////////////////

	TString res = Form("%s.Value", SubstName);
	TString sval = LofSubst->GetValue(res.Data(), "undef");
	if (sval.CompareTo("undef") == 0) {
		gMrbLog->Err()	<< "No such substitution - " << SubstName << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
		return(kFALSE);
	}
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Substituting " << SubstName << " by >>" << SubstValue << "<<" << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
	}
	LofSubst->SetValue(res.Data(), Form(Format, SubstValue));
	return(kTRUE);
}

Bool_t TMrbConfig::Substitute(TEnv * LofSubst, const Char_t * SubstName, Double_t SubstValue, const Char_t * Format) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Substitute
// Purpose:        Set a value in list of Substitutions
// Arguments:      TEnv * LofSubst       -- list of substitutions
//                 Char_t * SubstName    -- name of substitution
//                 Double_t  SubstValue  -- value
// Results:        kTRUE/kFALSE
// Description:    Substitute float value
//////////////////////////////////////////////////////////////////////////////

	TString res = Form("%s.Value", SubstName);
	TString sval = LofSubst->GetValue(res.Data(), "undef");
	if (sval.CompareTo("undef") == 0) {
		gMrbLog->Err()	<< "No such substitution - " << SubstName << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
		return(kFALSE);
	}
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Substituting " << SubstName << " by >>" << SubstValue << "<<" << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
	}
	LofSubst->SetValue(res.Data(), Form(Format, SubstValue));
	return(kTRUE);
}

Bool_t TMrbConfig::Substitute(TEnv * LofSubst, const Char_t * SubstName, Bool_t SubstValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::Substitute
// Purpose:        Set a value in list of Substitutions
// Arguments:      TEnv * LofSubst       -- list of substitutions
//                 Char_t * SubstName    -- name of substitution
//                 Bool_t  SubstValue    -- value
// Results:        kTRUE/kFALSE
// Description:    Substitute float value
//////////////////////////////////////////////////////////////////////////////

	TString res = Form("%s.Value", SubstName);
	TString sval = LofSubst->GetValue(res.Data(), "undef");
	if (sval.CompareTo("undef") == 0) {
		gMrbLog->Err()	<< "No such substitution - " << SubstName << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
		return(kFALSE);
	}
	if (this->IsVerbose()) {
		gMrbLog->Out()	<< "Substituting " << SubstName << " by >>" << SubstValue << "<<" << endl;
		gMrbLog->Flush(this->ClassName(), "Substitute");
	}
	LofSubst->SetValue(res.Data(), SubstValue);
	return(kTRUE);
}

Bool_t TMrbConfig::IsCalledBy(const Char_t * Element, const Char_t * ElemName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::IsCalledBy
// Purpose:        Check which element is calling
// Arguments:      Char_t * Element      -- calling element
//                 Char_t * ElemName     -- expected element name
// Results:        kTRUE/kFALSE
// Description:    Check if excpected element is calling or not
//////////////////////////////////////////////////////////////////////////////

	TString element = Element;
	return(element.CompareTo(ElemName) == 0);
}
