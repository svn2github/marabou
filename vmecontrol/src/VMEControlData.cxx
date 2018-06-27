//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// ame:            VMEControlData
// Purpose:        A GUI to control VME modules
// Description:    Common Database
// Modules:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: VMEControlData.cxx,v 1.14 2011-07-26 08:41:49 Marabou Exp $
// Date:           $Date: 2011-07-26 08:41:49 $
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

#include "TMrbC2Lynx.h"
#include "VMEControlData.h"

#include "SetColor.h"

ClassImp(VMEControlData)

extern TMrbC2Lynx * gMrbC2Lynx;
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
	fRootrc = new TMrbResource("VMEControl:TMrbConfig:TMrbC2Lynx", ".rootrc");

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

	fStatus = 0;
	
	TString errMsg;
	fRootrc->Get(fRcFile, ".RcFile", ".VMEControl.rc");
	gSystem->ExpandPathName(fRcFile);
	Bool_t ok = this->CheckAccess(fRcFile.Data(), kVMEAccessRead, errMsg, kFALSE);
	if (ok) {
		fVctrlrc = new TMrbResource("VMEControl:TMrbConfig:TMrbC2Lynx", fRcFile.Data());
		if (fFrameWidth <= 0) fFrameWidth = fVctrlrc->Get(".FrameWidth", kFrameWidth);
		if (fFrameHeight <= 0) fFrameHeight = fVctrlrc->Get(".FrameHeight", kFrameHeight);
		
		fRootrc->Get(fSettingsPath, ".SettingsPath", ".vmeSettings");
		gSystem->ExpandPathName(fSettingsPath);
		this->CheckAccess(fSettingsPath.Data(), kVMEAccessDirectory | kVMEAccessWrite, errMsg, kTRUE);

		fStatus = 0;
		if (fVctrlrc->Get(".VerboseMode", kFALSE)) fStatus |= kVMEVerboseMode;
		if (fVctrlrc->Get(".DebugMode", kFALSE)) fStatus |= kVMEDebugMode;
		if (fVctrlrc->Get(".DebugStopMode", kFALSE)) fStatus |= kVMEDebugStopMode;
		if (fVctrlrc->Get(".OfflineMode", kFALSE)) fStatus |= kVMEOfflineMode;

		fLofPanels.Clear();
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

Bool_t VMEControlData::SetupModuleList(TMrbLofNamedX & LofModules, const Char_t * ClassName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::SetupModuleList()
// Purpose:        Create module list
// Arguments:      TMrbLofNamedX & LofModules  -- list of modules
//                 Char_t * ClassName          -- class name
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Decodes env file and fills module list
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	LofModules.Clear(); 	// clear list - don't delete objects!!

	TString lm;
	fVctrlrc->Get(lm, ".LofModules", "");
	if (lm.IsNull()) {
		gMrbLog->Err()	<< "No (VME) modules defined in file " << fRcFile << endl;
		gMrbLog->Flush(this->ClassName(), "SetupModuleList");
		return(kFALSE);
	}
	
	Int_t errCnt = 0;
	Int_t from = 0;
	TString moduleName;
	while (lm.Tokenize(moduleName, from, ":")) {
		TString mnuc = moduleName;
		mnuc(0,1).ToUpper();
		TString interface;
		fVctrlrc->Get(interface, ".Module", mnuc.Data(), "Interface", "");
		if (interface.CompareTo("VME") == 0) {
			TString className;
			fVctrlrc->Get(className, ".Module", mnuc.Data(), "ClassName", "");
			if (className.IsNull()) {
				gMrbLog->Err()	<< "[" << moduleName << "] Class name missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			UInt_t vmeAddr = fVctrlrc->Get(".Module", mnuc.Data(), "HexAddr", 0);
			if (vmeAddr == 0) {
				gMrbLog->Err()	<< "[" << moduleName << "] VME addr missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			Int_t segSize = fVctrlrc->Get(".Module", mnuc.Data(), "SegSize", 0);
			if (segSize == 0) {
				gMrbLog->Err()	<< "[" << moduleName << "] Segment size missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			UInt_t vmeMapping = fVctrlrc->Get(".Module", mnuc.Data(), "Mapping", 0);
			if (vmeMapping == 0) {
				gMrbLog->Err()	<< "[" << moduleName << "] VME mapping mode missing" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				errCnt++;
				continue;
			}
			Int_t nofChannels = fVctrlrc->Get(".Module", mnuc.Data(), "NofChannelsUsed", 0);
			if (nofChannels == 0) {
				gMrbLog->Wrn()	<< "[" << moduleName << "] No channels assigned" << endl;
				gMrbLog->Flush(this->ClassName(), "SetupModuleList");
			}
			if (ClassName == NULL || className.CompareTo(ClassName) == 0) {
				if (className.CompareTo("TMrbSis_3302") == 0) {
					TC2LSis3302 * module = NULL;
					if (gMrbC2Lynx && (module = (TC2LSis3302 *) gMrbC2Lynx->FindModule(moduleName.Data()))) {
						if (module->GetAddress() != vmeAddr || module->GetNofChannels() != nofChannels) {
							gMrbLog->Wrn()	<< "[" << moduleName << "] Module already defined - addr="
											<< setbase(16) << module->GetAddress() << ", chns="
											<< setbase(10) << module->GetNofChannels() << endl;
							gMrbLog->Flush(this->ClassName(), "SetupModuleList");
							errCnt++;
						}
					}
					if (module == NULL) {
						module = new TC2LSis3302(moduleName.Data(), vmeAddr, segSize, nofChannels, vmeMapping, this->IsOffline());
					}
					if (module->IsZombie()) {
						errCnt++;
					} else {
						LofModules.AddLast(module);
					}
				} else if (className.CompareTo("TMrbCaen_V785") == 0) {
					gMrbLog->Wrn()	<< "[" << moduleName << "] Class " << className << " - not yet implemented" << endl;
					gMrbLog->Flush(this->ClassName(), "SetupModuleList");
				} else if (className.CompareTo("TMrbVulomTB") == 0) {
					TC2LVulomTB * module = NULL;
					if (gMrbC2Lynx && (module = (TC2LVulomTB *) gMrbC2Lynx->FindModule(moduleName.Data()))) {
						if (module->GetAddress() != vmeAddr || module->GetNofChannels() != nofChannels) {
							gMrbLog->Wrn()	<< "[" << moduleName << "] Module already defined - addr="
											<< setbase(16) << module->GetAddress() << ", chns="
											<< setbase(10) << module->GetNofChannels() << endl;
							gMrbLog->Flush(this->ClassName(), "SetupModuleList");
							errCnt++;
						}
					}
					if (module == NULL) {
						module = new TC2LVulomTB(moduleName.Data(), vmeAddr, segSize, nofChannels, vmeMapping, this->IsOffline());
					}
					if (module->IsZombie()) {
						errCnt++;
					} else {
						LofModules.AddLast(module);
					}
				} else {
					gMrbLog->Err()	<< "[" << moduleName << "] Unknown class name - " << className << endl;
					gMrbLog->Flush(this->ClassName(), "SetupModuleList");
					errCnt++;
				}
			}
		}
	}
	if (errCnt > 0) {
		gMrbLog->Err()	<< "Some errors occurred while processing file " << fRcFile << endl;
		gMrbLog->Flush(this->ClassName(), "SetupModuleList");
		return(kFALSE);
	}
	return(LofModules.GetEntries() > 0);
}

Int_t VMEControlData::MsgBox(TGWindow * Caller, const Char_t * Method, const Char_t * Title, const Char_t * Msg, EMsgBoxIcon Icon, Int_t Buttons) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::MsgBox()
// Purpose:        Show a message box
// Arguments:      TGWindow * Caller           -- calling class
//                 Char_t * Method             -- class method
//                 Char_t * Title              -- title
//                 Char_t * Msg                -- message
//                 EMsgBoxIcon Icon            -- icon to be displayed
//                 Int_t Buttons               -- button(s) to be shown
// Results:        Int_t Retval                -- return value as passed from TGMsgBox
// Exceptions:
// Description:    Shows a message box and outputs message to gMrbLog, too.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t retVal;
	TString title = Caller->ClassName();
	if (Method && *Method != '\0') title += Form("::%s()", Method);
	if (Title && *Title != '\0') title += Form(": %s", Title);
	if (Icon == kMBIconStop || Icon == kMBIconExclamation) {
		gMrbLog->Err()	<< Msg << endl;
		gMrbLog->Flush(Caller->ClassName(), Method);
	}
	new TGMsgBox(gClient->GetRoot(), Caller, title.Data(), Msg, Icon, Buttons, &retVal);
	return (retVal);
}

void VMEControlData::GetLofChannels(TMrbLofNamedX & LofChannels, Int_t NofChannels, const Char_t * Format, Bool_t PatternMode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::GetLofChannels()
// Purpose:        Create module list
// Arguments:      Int_t NofChannels            -- number of channels
//                 Char_t * Format              -- how to format channel names
//                 Bool_t PatternMode           -- store patterns instead of numbers if kTRUE
// Results:        TMrbLofNamedX & LofChannels  -- list of channels / channel patterns
// Exceptions:
// Description:    Generates a table containing channel numbers or patterns
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	LofChannels.Delete();
	Int_t bit = 1;
	if (PatternMode) {
		for (Int_t i = 0; i < NofChannels; i++) {
			LofChannels.AddNamedX(bit, Form(Format, i));
			bit <<= 1;
		}
		LofChannels.SetPatternMode();
	} else {
		for (Int_t i = 0; i < NofChannels; i++) LofChannels.AddNamedX(i, Form(Format, i));
	}
}

Bool_t VMEControlData::SetSis3302Verbose(TC2LSis3302 * Module) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           VMEControlData::SetSis3302Verbose()
// Purpose:        Turn on verbose and debug mode
// Arguments:      TC2LSis3302 * Module   -- current sis module
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets verbose/debug flag, also on server side
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t status = 0;
	
	if (this->IsVerbose()) status |= kSis3302StatusVerboseMode;
	if (this->IsDebug()) status |= kSis3302StatusDebugMode;
	if (this->IsDebugStop()) status |= kSis3302StatusDebugStopMode;
	if (!this->IsOffline() && Module != NULL) Module->SetVerboseMode(status, kSis3302StatusAnyVerboseMode);
	return(kTRUE);
}
