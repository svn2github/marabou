//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEControlData
// Purpose:        A GUI to control VME modules
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEControlData.cxx,v 1.1 2008-08-26 06:33:24 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-08-26 06:33:24 $
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include "TEnv.h"
#include "TSystem.h"
#include "TObjString.h"

#include "TMrbNamedX.h"
#include "TMrbResource.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"

#include "VMEControlData.h"

#include "SetColor.h"

ClassImp(VMEControlData)

extern TMrbLogger * gMrbLog;

VMEControlData::VMEControlData(const Char_t * RcFile) : TNamed("VMEControlData", "VMEControlData") {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData
// Purpose:        Common data base
// Arguments:      --
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
	
// open ROOT's resource data base
	fRootrc = new TMrbResource("VMEControl", ".rootrc");

// open VMEcontrol's resource data base
	fRcFile = RcFile;
	if (fRcFile.IsNull()) fRcFile = ".VMEControl.rc";
	fVctrlrc = new TMrbResource("VMEControl", fRcFile.Data());

	fFrameWidth = fRootrc->Get(".FrameWidth", 0);
	if (fFrameWidth <= 0) fFrameWidth = fVctrlrc->Get(".FrameWidth", kFrameWidth);
	fFrameHeight = fRootrc->Get(".FrameHeight", 0);
	if (fFrameHeight <= 0) fFrameHeight = fVctrlrc->Get(".FrameHeight", kFrameHeight);

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
		gMrbLog->Flush("VMEControlData");
		fBoldFont = fNormalFont;
	}		
	fSlantedFont = gEnv->GetValue("Gui.SlantedFont", "-adobe-helvetica-medium-o-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fSlantedFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fSlantedFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("VMEControlData");
		fSlantedFont = fNormalFont;
	}		
	fFixedFont = gEnv->GetValue("Gui.FixedFont", "-adobe-courier-bold-r-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fFixedFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fFixedFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("VMEControlData");
		fFixedFont = fNormalFont;
	}		
}

Bool_t VMEControlData::CheckAccess(const Char_t * FileOrPath, Int_t AccessMode, TString & ErrMsg, Bool_t WarningOnly) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::CheckAccess
// Purpose:        Check access to file or path
// Arguments:      Char_t * FileOrPath    -- file or path spec
//                 Int_t AccessMode       -- access mode: kVMEAccessXXX
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

	if (AccessMode & kVMEAccessRegular && !ux.IsRegular(FileOrPath)) {
		ErrMsg = "Not a regular file - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	} else if (AccessMode & kVMEAccessDirectory && !ux.IsDirectory(FileOrPath)) {
		ErrMsg = "Not a directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	if (AccessMode & kVMEAccessRead && !ux.HasReadPermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No such file or directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}
	if (AccessMode & kVMEAccessWrite && !ux.HasWritePermission(FileOrPath)) {
		if (ErrMsg.Length() > 0) ErrMsg += "\n";
		ErrMsg = "No write access to file/directory - ";
		ErrMsg += FileOrPath;
		if (WarningOnly) gMrbLog->Wrn() << ErrMsg << endl; else gMrbLog->Err() << ErrMsg << endl;
		gMrbLog->Flush(this->ClassName(), "CheckAccess");
		ok = kFALSE;
	}

	return(ok);
}

