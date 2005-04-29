//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            CptmControlData
// Purpose:        A GUI to control a CPTM module
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: CptmControlData.cxx,v 1.1 2005-04-29 11:35:22 rudi Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TSystem.h"
#include "TGClient.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbEnv.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "CptmControlData.h"

#include "SetColor.h"

ClassImp(CptmControlData)

extern TMrbLogger * gMrbLog;

CptmControlData::CptmControlData() : TNamed("CptmControlData", "CptmControlData") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData
// Purpose:        Common data base
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbEnv env;

	fCAMACHost = "";
	fDefaultCrate = -1;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
// initialize colors
	gClient->GetColorByName("black", fColorBlack);
	gClient->GetColorByName("blue", fColorDarkBlue);
	gClient->GetColorByName("lightblue2", fColorBlue);
	gClient->GetColorByName("gray75", fColorGray);
	gClient->GetColorByName("lightgoldenrod1", fColorGold);
	gClient->GetColorByName("antiquewhite1", fColorWhite);
	gClient->GetColorByName("darkseagreen", fColorGreen);
	gClient->GetColorByName("red", fColorRed);
	gClient->GetColorByName("yellow", fColorYellow);

// initialize fonts
	fNormalFont = gEnv->GetValue("Gui.NormalFont", "-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	fBoldFont = gEnv->GetValue("Gui.BoldFont", "-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fBoldFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fBoldFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("CptmControlData");
		fBoldFont = fNormalFont;
	}		
	fSlantedFont = gEnv->GetValue("Gui.SlantedFont", "-adobe-helvetica-medium-o-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fSlantedFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fSlantedFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("CptmControlData");
		fSlantedFont = fNormalFont;
	}		

// global switches
	if (gEnv->GetValue("CptmControl.DebugMode", kFALSE))		fStatus |= CptmControlData::kDGFDebugMode;
	else													fStatus &= ~CptmControlData::kDGFDebugMode;
	if (gEnv->GetValue("CptmControl.VerboseMode", kFALSE))	fStatus |= CptmControlData::kDGFVerboseMode;
	else													fStatus &= ~CptmControlData::kDGFVerboseMode;
	if (gEnv->GetValue("CptmControl.ModNumGlobal", kTRUE))	fStatus |= CptmControlData::kDGFModNumGlobal;
	else													fStatus &= ~CptmControlData::kDGFModNumGlobal;
	if (gEnv->GetValue("CptmControl.OfflineMode", kFALSE))	fStatus |= CptmControlData::kDGFOfflineMode;
	else													fStatus &= ~CptmControlData::kDGFOfflineMode;

// paths and filenames
	TString errMsg;
	Bool_t panic = kFALSE;
	TString path;

	env.Find(fCAMACHost, "DGFControl:TMrbDGF:TMrbEsone", "HostName", "ppc-0");

	env.Find(path, "CptmControl", "CptmCodeFile", "");
	if (path.Length() == 0) env.Find(path, "TMrbCPTM", "CodeFile", "cptm.rbf");
	fCptmCodeFile = path;
	gSystem->ExpandPathName(fCptmCodeFile);
	this->CheckAccess(fCptmCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	env.Find(path, "CptmControl", "CptmSettingsPath", "");
	if (path.Length() == 0) env.Find(path, "TMrbCPTM", "SettingsPath", "../cptmSettings");
	fCptmSettingsPath = path;
	gSystem->ExpandPathName(fCptmSettingsPath);
	this->CheckAccess(fCptmSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

	if (panic) {
		gMrbLog->Err() << "Can't continue. Correct errors, then start over. Sorry" << endl;
		gMrbLog->Flush(this->ClassName());
		gSystem->Exit(1);
	}

}

const Char_t * CptmControlData::GetResource(TString & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      TString & Result   -- where to put resulting value
//                 Char_t * Prefix    -- prefix "CptmControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
// Results:        Char_t * Result    -- resource value
// Exceptions:     
// Description:    Reads an ascii resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "CptmControl.Module.1.Address"
//                          and for      "CptmControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr;
		
	Result = "";
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		Result = gEnv->GetValue(resStr.Data(), "");
	}
	if (Result.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		Result = gEnv->GetValue(resStr.Data(), "");
	}
	return(Result.Data());
}

Int_t CptmControlData::GetResource(Int_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      Int_t & Result     -- where to put resulting value
//                 Char_t * Prefix    -- prefix "CptmControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
//                 Int_t Base         -- numerical base
// Results:        Int_t Result       -- resource value
// Exceptions:     
// Description:    Reads an integer resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "CptmControl.Module.1.Address"
//                          and for      "CptmControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr, resVal;
		
	resVal = "";
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	resVal.ToInteger(Result, Base);
	return(Result);
}

Bool_t CptmControlData::GetResource(Bool_t & Result, const Char_t * Prefix, Int_t Serial, const Char_t * Name, const Char_t * Resource) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData::GetResource
// Purpose:        Get resource from environment
// Arguments:      Bool_t & Result    -- where to put resulting value
//                 Char_t * Prefix    -- prefix "CptmControl.XXX"
//                 Int_t Serial       -- serial number
//                 Char_t * Name      -- name
//                 Char_t * Resource  -- resource name
// Results:        Int_t Result       -- resource value
// Exceptions:     
// Description:    Reads an boolean resource value from ROOT's environment.
//                 Resource name is either
//                        <Prefix>.<Serial>.<Resource>
//                 or     <Prefix>.<Name>.<Resource>
//                 Example: To get the address of module #1 with name "xyz"
//                          it looks for "CptmControl.Module.1.Address"
//                          and for      "CptmControl.Module.Xyz.Address"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbString resStr, resVal;
		
	if (Serial >= 0) {
		resStr = Prefix;
		resStr += ".";
		resStr += Serial;
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.Length() == 0 && Name != NULL) {
		resStr = Name;
		resStr(0,1).ToUpper();
		resStr.Prepend(".");
		resStr.Prepend(Prefix);
		resStr += ".";
		resStr += Resource;
		resVal = gEnv->GetValue(resStr.Data(), "");
	}
	if (resVal.CompareTo("TRUE") == 0 || resVal.CompareTo("true") == 0) Result = kTRUE; else Result = kFALSE;
	return(Result);
}

Bool_t CptmControlData::CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CptmControlData::CheckAccess
// Purpose:        Check access to file or path
// Arguments:      Char_t * FileOrPath    -- file or path spec
//                 Int_t AccessMode       -- access mode: kDGFAccessXXX
//                 TString & ErrMsg       -- resulting error message
//                 Bool_t WarningOnly     -- kTRUE -> warning
// Results:        kTRUE/kFALSE  
// Exceptions:     
// Description:    Checks if file or path exists and is readable/writable
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TMrbSystem ux;
	ErrMsg = "";
	Bool_t ok = kTRUE;

	if (AccessMode & kDGFAccessRegular && !ux.IsRegular(FileOrPath)) {
		ErrMsg = "Not a regular file - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	} else if (AccessMode & kDGFAccessDirectory && !ux.IsDirectory(FileOrPath)) {
		ErrMsg = "Not a directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	if (AccessMode & kDGFAccessRead && !ux.HasReadPermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No such file or directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}
	if (AccessMode & kDGFAccessWrite && !ux.HasWritePermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No write access to file/directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	return(ok);
}

