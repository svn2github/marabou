//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            CptmControlData
// Purpose:        A GUI to control a CPTM module
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: CptmControlData.cxx,v 1.2 2008-08-18 08:19:51 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TSystem.h"
#include "TGClient.h"
#include "TObjString.h"

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

	fCAMACHost = "";
	fDefaultCrate = -1;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
// open ROOT's resource data base
	fRootrc = new TMrbResource("CptmControl", ".rootrc");

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

	fCAMACHost = fRootrc->Get(".HostName", "ppc-0");

	TString path = fRootrc->Get(".CptmCodeFile", "");
	if (path.Length() == 0) path = fRootrc->Get("TMrbCPTM.CodeFile", "cptm.rbf");
	fCptmCodeFile = path;
	gSystem->ExpandPathName(fCptmCodeFile);
	this->CheckAccess(fCptmCodeFile.Data(), kDGFAccessRead, errMsg, kTRUE);

	path = fRootrc->Get(".CptmSettingsPath", "");
	if (path.Length() == 0) path = fRootrc->Get("TMrbCPTM.SettingsPath", "../cptmSettings");
	fCptmSettingsPath = path;
	gSystem->ExpandPathName(fCptmSettingsPath);
	this->CheckAccess(fCptmSettingsPath.Data(), kDGFAccessDirectory | kDGFAccessWrite, errMsg, kTRUE);

	if (panic) {
		gMrbLog->Err() << "Can't continue. Correct errors, then start over. Sorry" << endl;
		gMrbLog->Flush(this->ClassName());
		gSystem->Exit(1);
	}

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

