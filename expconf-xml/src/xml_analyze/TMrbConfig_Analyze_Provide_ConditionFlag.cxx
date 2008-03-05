//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig_Analyze_Provide_ConditionFlag.cxx
// Purpose:        MARaBOU configuration: XML code generator
//                 Called by method TMrbCOnfig::MakeAnalyzeCode()
//                 Provides a boolean value for <if>...</if>
// Description:    Implements class methods to define a MARaBOU configuration
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbConfig_Analyze_Provide_ConditionFlag.cxx,v 1.1 2008-03-05 12:37:36 Rudolf.Lutter Exp $
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

Bool_t TMrbConfig::ProvideConditionFlag_Analyze(const Char_t * Element, TMrbNamedX * Tag, const Char_t * FlagName, TString & FlagValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbConfig::ProvideConditionFlag_Analyze
// Purpose:        Return a condition
// Arguments:      Char_t * Element      -- element name
//                 Char_t * Tag          -- current tag
//                 Char_t * FlagName     -- name of condition
//                 TString & FlagValue   -- value
// Results:        kTRUE/kFALSE
// Description:    Return a condition for tag group 'analyze'
//////////////////////////////////////////////////////////////////////////////

	FlagValue = "";
	if (this->IsVerbose()) {
		if (FlagName)	gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << " Flag=" << FlagName << endl;
		else			gMrbLog->Out()	<< "[" << Element << "] Tag=" << Tag->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ProvideConditionFlag_Analyze");
	}
	return(kTRUE);
}
