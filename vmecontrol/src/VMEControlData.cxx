//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEControlData
// Purpose:        A GUI to control VME modules
// Description:    Common Database
// Modules:        
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEControlData.cxx,v 1.2 2008-08-28 07:16:48 Rudolf.Lutter Exp $       
// Date:           $Date: 2008-08-28 07:16:48 $
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
#include "TC2LSis3302.h"

#include "SetColor.h"

ClassImp(VMEControlData)

extern TMrbLogger * gMrbLog;

VMEControlData::VMEControlData() {
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

// GUI dims
	fFrameWidth = fRootrc->Get(".FrameWidth", 0);
	fFrameHeight = fRootrc->Get(".FrameHeight", 0);

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
	fFixedFont = gEnv->GetValue("Gui.FixedFont", "-adobe-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
	if (gClient->GetFontByName(fFixedFont.Data()) == 0) {
		gMrbLog->Err()	<< "No such font - " << fFixedFont << " (normal font used instead)" << endl;
		gMrbLog->Flush("VMEControlData");
		fFixedFont = fNormalFont;
	}		
// open VMEcontrol's resource data base
	TString errMsg;
	fRcFile = fRootrc->Get(".RcFile", ".VMEControl.rc");
	gSystem->ExpandPathName(fRcFile);
	Bool_t ok = this->CheckAccess(fRcFile.Data(), kVMEAccessRead, errMsg, kFALSE);
	if (ok) {
		fVctrlrc = new TMrbResource("VMEControl", fRcFile.Data());
		fLofModules.SetName("List of VME modules");
		fLofModules.Delete();
		if (fFrameWidth <= 0) fFrameWidth = fVctrlrc->Get(".FrameWidth", kFrameWidth);
		if (fFrameHeight <= 0) fFrameHeight = fVctrlrc->Get(".FrameHeight", kFrameHeight);
		if (this->SetupModuleList() <= 0) this->MakeZombie();
	} else {
		this->MakeZombie();
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

Int_t VMEControlData::SetupModuleList() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::SetupModuleList()
// Purpose:        Create module list
// Arguments:      --
// Results:        Int_t NofModules   -- number of VME modules
// Exceptions:     
// Description:    Decodes env file and fills module list 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fLofModules.Delete();
	fNofModules = 0;

	TString lofModules = fVctrlrc->Get(".LofModules", "");
	if (lofModules.IsNull()) {
		gMrbLog->Err()	<< "No (VME) modules defined in file " << fRcFile << endl;
		gMrbLog->Flush(this->ClassName(), "SetupModuleList");
		return(-1);
	}

	Int_t errCnt = 0;
	Int_t from = 0;
	TString moduleName;
	while (lofModules.Tokenize(moduleName, from, ":")) {
		TString interface = fVctrlrc->Get(".Module", moduleName.Data(), "Interface", "");
		if (interface.CompareTo("VME") == 0) {
			TString className = fVctrlrc->Get(".Module", moduleName.Data(), "ClassName", "");
			if (className.IsNull()) {
				gMrbLog->Err()	<< "[" << moduleName << "] Class name missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			UInt_t vmeAddr = fVctrlrc->Get(".Module", moduleName.Data(), "HexAddr", 0);
			if (vmeAddr == 0) {
				gMrbLog->Err()	<< "[" << moduleName << "] VME addr missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			Int_t nofChannels = fVctrlrc->Get(".Module", moduleName.Data(), "NofChannelsUsed", 0);
			if (nofChannels == 0) {
				gMrbLog->Wrn()	<< "[" << moduleName << "] No channels assigned" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
			}
			if (className.CompareTo("TMrbSis_3302") == 0) {
				TC2LSis3302 * module = new TC2LSis3302(moduleName.Data(), vmeAddr, nofChannels);
				if (module->IsZombie()) {
					errCnt++;
				} else {
					fNofModules++;
					fLofModules.AddNamedX(module);
				}
			} else if (className.CompareTo("TMrbCaen_V785") == 0) {
				gMrbLog->Wrn()	<< "[" << moduleName << "] Class " << className << " - not yet implemented" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
			} else {
				gMrbLog->Err()	<< "[" << moduleName << "] Unknown class name - " << className << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
			}
		}
	}
	if (fNofModules == 0) {
		gMrbLog->Err()	<< "No VME modules defined in file " << fRcFile << endl;
		gMrbLog->Flush(this->ClassName(), "SetupModuleList");
		return(-1);
	}

	return(fNofModules);
}

