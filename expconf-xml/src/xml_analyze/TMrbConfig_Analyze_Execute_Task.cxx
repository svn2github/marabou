//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig_Analyze_Execute_Task.cxx
// Purpose:        MARaBOU configuration: XML code generator
//                 Called by method TMrbCOnfig::MakeAnalyzeCode()
//                 Provides a piece of code
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfig_Analyze_Execute_Task.cxx,v 1.1 2008-03-05 12:37:36 Rudolf.Lutter Exp $
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

Bool_t TMrbConfig::ExecuteTask_Analyze(const Char_t * Element,  TMrbNamedX * Tag, const Char_t * Subtag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideCode_Analyze
// Purpose:        Return piece of code
// Arguments:      Char_t * Element          -- element name
//                 TMrbNamedX * Tag * Tag    -- current tag
//                 Char_t * Subtag           -- subtag
// Results:        kTRUE/kFALSE
// Description:    Execute task for tag group 'analyze'
//////////////////////////////////////////////////////////////////////////////

	if (this->IsVerbose()) {
		if (Subtag)	gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << " Subtag=" << Subtag << endl;
		else		gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ExecuteTask_Analyze");
	}

	switch (Tag->GetIndex()) {

		case TMrbConfig::kAnaXmlRoot:					return(kTRUE);
		case TMrbConfig::kAnaUserGlobals:	 			return(kTRUE);
		case TMrbConfig::kAnaHistoBookUserDefined:	 	return(kTRUE);
		case TMrbConfig::kAnaUserInitializeBeforeHB:
		case TMrbConfig::kAnaUserInitializeAfterHB: 	return(kTRUE);
		case TMrbConfig::kAnaClassImp:  				return(kTRUE);
		case TMrbConfig::kAnaIncludeEvtSevtModGlobals:	return(kTRUE);
		case TMrbConfig::kAnaAddUserEnv: 				return(kTRUE);
		case TMrbConfig::kAnaHistoInitArrays:		 	return(kTRUE);
		case TMrbConfig::kAnaModuleInitList: 			return(kTRUE);
		case TMrbConfig::kAnaModuleTimeOffset:  		return(kTRUE);
		case TMrbConfig::kAnaEventClassInstance: 		return(kTRUE);
		case TMrbConfig::kAnaEventUserClassInstance: 	return(kTRUE);
		case TMrbConfig::kAnaWdwClassInstance:  		return(kTRUE);
		case TMrbConfig::kAnaInitializeSevtMods:	 	return(kTRUE);
		case TMrbConfig::kAnaHistoFillArrays:		 	return(this->ExecuteTask_AnaHistoFillArrays(Element, Tag, Subtag));

		default:
			gMrbLog->Err()	<< "[" << Element << "] Tag=" << Tag->GetName() << ": Not yet implemented" << endl;
			gMrbLog->Flush(this->ClassName(), "ExecuteTask_Analyze");
			return(kFALSE);
	}

	return(kTRUE);
}

//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ExecuteTask_XXX
// Purpose:        Execute task
// Arguments:      TMrbNamedX * Tag   -- tag
//                 Char_t * Subtag    -- subtag
// Description:    Executes external task
//////////////////////////////////////////////////////////////////////////////

Bool_t TMrbConfig::ExecuteTask_AnaHistoFillArrays(const Char_t * Element, TMrbNamedX * Tag, const Char_t * Subtag) {
//________________________________________________________________[C++ METHOD]
// Tag:            HISTO_FILL_ARRAYS
// Subtag:         --
//////////////////////////////////////////////////////////////////////////////

	Bool_t verboseMode = (this->IsVerbose() || (this->GetAnalyzeOptions() & kAnaOptVerbose) != 0);

	TIterator * sevtIter = fLofSubevents.MakeIterator();
	TMrbSubevent * sevt;
	while (sevt = (TMrbSubevent *) sevtIter->Next()) {
		if (sevt->HistosToBeAllocated() && sevt->IsInArrayMode()) {
			TString listName = sevt->GetName();
			listName += ".histlist";
			ofstream list(listName.Data(), ios::out);
			if (!list.good()) {
				gMrbLog->Err() << gSystem->GetError() << " - " << listName << endl;
				gMrbLog->Flush(this->ClassName(), "ExecuteTask_AnaHistoFillArrays");
			} else {
				TIterator * paramIter = sevt->GetLofParams()->MakeIterator();
				TMrbModuleChannel * param;
				while (param =  (TMrbModuleChannel *) paramIter->Next()) {
					TString paramName = param->GetName();
					paramName(0,1).ToUpper();
					paramName.Prepend("h");
					list << paramName << endl;
				}
				list.close();
				if (verboseMode) {
					gMrbLog->Out()  << "Creating histo list file - " << listName
									<< " (" << sevt->GetNofParams() << " entries)"
									<< endl;
					gMrbLog->Flush(this->ClassName(), "ExecuteTask_AnaHistoFillArrays");
				}
			}
		}
	}
	return(kTRUE);
}
