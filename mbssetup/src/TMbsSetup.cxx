//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/src/TMbsSetup.cxx
// Purpose:        MBS setup
// Description:    Implements class methods to define a MBS setup 
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMbsSetup.cxx,v 1.45 2006-11-03 07:27:41 Rudolf.Lutter Exp $       
// Date:           
//
// Class TMbsSetup refers to a resource file in user's working directory
// named ".mbssetup" (if not defined otherwise).
//
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"
#include "TDirectory.h"
#include "TObjString.h"

#include "TMrbTemplate.h"
#include "TMrbLogger.h"
#include "TMrbSystem.h"
#include "TMrbWildcard.h"

#include "TMbsSetup.h"

#include "SetColor.h"

extern TSystem * gSystem;
extern TMrbLogger * gMrbLog;

TMbsSetup * gMbsSetup = NULL;

ClassImp(TMbsSetup)

TMbsSetup::TMbsSetup(const Char_t * SetupFile) : TMrbEnv() {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup
// Purpose:        Define a MBS setup
// Arguments:      Char_t * SetupFile -- name of setup file
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString defaultSetupPath;
	TString setupFile;

	TString defaultSetupFile;
	Int_t nofReadouts, n;

	TString settingsFile;

	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssetup.log");
	
	if (gMbsSetup != NULL) {
		gMrbLog->Err() << "Setup already defined" << endl;
		gMrbLog->Flush(this->ClassName());
	} else {
		fLofReadouts.Delete();

		fLofSetupModes.SetName("Setup Modes");				// ... setup modes
		fLofSetupModes.AddNamedX(kMbsLofSetupModes);	

		fLofProcs.SetName("Processor Types");				// ... processor types
		fLofProcs.AddNamedX(kMbsLofProcs);	

		fLofControllers.SetName("Crate Controllers");		// ... crate controllers
		fLofControllers.AddNamedX(kMbsLofControllers);

		fLofTriggerModules.SetName("Trigger Module Types");	// ... trigger modules
		fLofTriggerModules.AddNamedX(kMbsLofTriggerModules);

		fLofTriggerModes.SetName("Trigger Modes");			// ... trigger modes
		fLofTriggerModes.AddNamedX(kMbsLofTriggerModes);

		fLofSetupTags.SetName("Setup Tags");				// ... setup tags
		fLofSetupTags.AddNamedX(kMbsSetupTags);

		defaultSetupPath = gEnv->GetValue("TMbsSetup.DefaultSetupPath", ".:$HOME:$(MARABOU)/templates/mbssetup");

		setupFile = SetupFile;
		this->Open(setupFile.Data());			// open resource file

		gSystem->ExpandPathName(defaultSetupPath);
		TMrbSystem ux;
		TString tmplFile = "mbssetup.tmpl";
		ux.Which(defaultSetupFile, defaultSetupPath.Data(), tmplFile.Data());
		if (defaultSetupFile.IsNull()) {
			gMrbLog->Err() << "No such file - " << tmplFile << " (path = " << defaultSetupPath << ")" << endl;
			gMrbLog->Flush(this->ClassName());
			gMrbLog->Err() << "No default setup given - can't do the job, sorry" << endl;
			gMrbLog->Flush(this->ClassName());
		} else {
			TString rcFile = "mbssetup.rc";
			ux.Which(settingsFile, defaultSetupPath.Data(), rcFile.Data());
			if (settingsFile.IsNull()) {
				gMrbLog->Err() << "No such file - " << rcFile << " (path = " << defaultSetupPath << ")" << endl;
				gMrbLog->Flush(this->ClassName());
				gMrbLog->Err() << "No settings file given - can't do the job, sorry" << endl;
				gMrbLog->Flush(this->ClassName());
			} else {
				gMbsSetup = this; 						// holds addr of current setup def
				gDirectory->Append(this);

				fSettings = new TEnv(settingsFile.Data());	// open settings file

				this->OpenDefaults(defaultSetupFile.Data()); // open defaults file

				this->SetPrefix("TMbsSetup");			// resource names start with "TMbsSetup"

				this->CopyDefaults("TemplatePath");		// get template path: where to get file templates from
				this->CopyDefaults("HomeDir");			// get user's home dir as seen from lynxos
				this->CopyDefaults("Path");				// get path: where to write setup files
				this->CopyDefaults("Mode");				// get setup mode: single, dual, or multi proc

				fEvtBuilder = new TMbsEvtBuilder(); 	// create an event builder

				fReadoutError = new TMbsReadoutProc(-1);	// alloc readout proc for errors

				fVerbose = gEnv->GetValue("TMbsSetup.VerboseMode", kFALSE);

				nofReadouts = this->GetNofReadouts();
				nofReadouts = 1;
				for (n = 0; n < nofReadouts; n++) fLofReadouts.Add(new TMbsReadoutProc(n));
			}
		}
	}
}

TMbsSetup::~TMbsSetup() {
//__________________________________________________________________[C++ DTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup
// Purpose:        Delete a MBS setup
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Class destructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fLofReadouts.Delete();		// delete objects stored in lists
	fLofProcs.Delete();
	fLofTriggerModules.Delete();
	fLofTriggerModes.Delete();
	fLofSetupTags.Delete();
	fLofControllers.Delete();
	gDirectory->RecursiveRemove(gMbsSetup);
	gMbsSetup = NULL;
}

Bool_t TMbsSetup::GetRcVal(UInt_t & RcValue, const Char_t * Resource, const Char_t * ContrlType, const Char_t * ProcType, const Char_t * Mode, const Char_t * MbsVersion, const Char_t * LynxVersion) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::GetRcVal
// Purpose:        Get value from settings file
// Arguments:      UInt_t & RcValue        -- resulting settings value
//                 Char_t * Resource       -- resource name
//                 Char_t * ContrlType     -- type of controller
//                 Char_t * ProcType       -- type of processor
//                 Char_t * Mode           -- mode: single/multi proc, multi branch
//                 Char_t * MbsVersion     -- MBS' version
//                 Char_t * LynxVersion    -- version of lynxOs
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Reads settings from file mbssetup.rc
//                 Format:
//                 <LynxVersion>.<MbsVersion>.<CpuType>.<ContrType>.<Resource>:	<Value>
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	RcValue = 0xaffec0c0;

	if (Resource == NULL || *Resource == '\0') {
		gMrbLog->Err() << "Resource name missing" << endl;
		gMrbLog->Flush(this->ClassName(), "GetRcVal");
		return(kFALSE);
	}

	if (LynxVersion == NULL || *LynxVersion == '\0') LynxVersion = "*";
	if (MbsVersion == NULL || *MbsVersion == '\0') MbsVersion = "*";
	if (ProcType == NULL || *ProcType == '\0') ProcType = "*";
	if (ContrlType == NULL || *ContrlType == '\0') ContrlType = "*";

	if (strcmp(ProcType, "PPC") == 0) ProcType = "RIO2";

	TList * lofSettings = (TList *) fSettings->GetTable();
	if (lofSettings->GetEntries() == 0) {
		gMrbLog->Err() << "No entries in settings table" << endl;
		gMrbLog->Flush(this->ClassName(), "GetRcVal");
		return(kFALSE);
	}

	TString res = Form("%s-%s-%s-%s-%s.%s", LynxVersion, MbsVersion, Mode, ProcType, ContrlType, Resource);
	TString valStr;
	TIterator * iter = lofSettings->MakeIterator();
	TEnvRec * r;
	while (r = (TEnvRec *) iter->Next()) {
		TMrbWildcard w(r->GetName());
		if (w.Match(res.Data())) {
			valStr = r->GetValue();	
			RcValue = strtoul(valStr.Data(), NULL, 0);
			return(kTRUE);
		}
	}

	gMrbLog->Err() << "No value given for resource - " << res << endl;
	gMrbLog->Flush(this->ClassName(), "GetRcVal");
	return(kFALSE);
}

void TMbsSetup::RemoveSetup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::RemoveSetup
// Purpose:        Remove entries from setup data base
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Remove resources "TMbsSetup.*".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->Remove("*", kFALSE);	// remove entries
}

void TMbsSetup::Reset() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::Reset
// Purpose:        Reset resources to default values
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Resets resources "TMbsSetup.*".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;
	Int_t nofRdo;

	this->EvtBuilder()->Reset();
	nofRdo = this->GetDefault("NofReadouts", 0);
	for (n = 0; n < nofRdo; n++) this->ReadoutProc(n)->Reset();
	for (n = nofRdo; n < GetNofReadouts(); n++) {
		this->ReadoutProc(n)->RemoveSetup();
		fLofReadouts.RemoveAt(n);
	}
	this->CopyDefaults("NofReadouts", kTRUE, kTRUE);
	this->CopyDefaults("HomeDir", kTRUE, kTRUE);
	this->CopyDefaults("Mode", kTRUE, kTRUE);
}

TMbsReadoutProc * TMbsSetup::ReadoutProc(Int_t ReadoutId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::ReadoutProc
// Purpose:        Return addr of readout process
// Arguments:      Int_t ReadoutId         -- proc id
// Results:        TMbsReadoutProc * Proc  -- process
// Exceptions:
// Description:    Returns addr of readout proc defined by its id.
//                 Legal ids start with 1.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nprocs;

	nprocs = fLofReadouts.GetEntries();

	if (nprocs < 1) {
		gMrbLog->Err() << "No readout allocated" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadoutProc");
		return(fReadoutError);
	} else if (ReadoutId < 0 || ReadoutId > nprocs) {
		gMrbLog->Err() << "Illegal proc id - " << ReadoutId << " (should be in [0," << nprocs << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadoutProc");
		return(fReadoutError);
	} else {
		return((TMbsReadoutProc *) fLofReadouts[ReadoutId]);
	}
}

Bool_t TMbsSetup::SetHomeDir(const Char_t * HomeDir) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetHomeDir
// Purpose:        Define user's home dir
// Arguments:      Char_t * HomeDir    -- home directory
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.HomeDir".
//                 HomeDir is user's home directory as seen from MBS.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long_t dmy;
	Long64_t dmy64;
	Long_t flags;

	if (gSystem->GetPathInfo(HomeDir, &dmy, &dmy64, &flags, &dmy) != 0 || (flags & 0x2) == 0) {
		gMrbLog->Err() << "No such directory - " << HomeDir << endl;
		gMrbLog->Flush(this->ClassName(), "SetHomeDir");
		return(kFALSE);
	}
	this->Set("HomeDir", HomeDir);
	return(kTRUE);
}

const Char_t * TMbsSetup::GetHomeDir() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::GetHomeDir
// Purpose:        Read user's home dir
// Arguments:      --
// Results:        Char_t * HomeDir       -- home directory
// Exceptions:
// Description:    Gets resource "TMbsSetup.HomeDir".
//                 HomeDir is user's home directory as seen from MBS.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->Get(fHomeDir, "HomeDir");
	return(fHomeDir.Data());
}

Bool_t TMbsSetup::SetPath(const Char_t * Path, Bool_t Create) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetPath
// Purpose:        Define path where to write setup files
// Arguments:      Char_t * Path       -- path name
//                 Bool_t Create       -- kTRUE if path is to be created
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets resource "TMbsSetup.Path".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Long_t dmy;
	Long64_t dmy64;
	Long_t flags;
	TString pathName;
	TString remoteHome;

	pathName = Path;
	if (!pathName.BeginsWith("/") && !pathName.BeginsWith("./")) {
		pathName = gMbsSetup->GetHomeDir();	// get home dir
		if (pathName.IsNull()) {
			remoteHome = this->RemoteHomeDir();
			if (remoteHome.IsNull()) {
				gMrbLog->Err() << "Can't set path \"" << Path << "\" -" << endl;
				gMrbLog->Flush(this->ClassName(), "SetPath");
				gMrbLog->Err() << "\"TMbsSetup.DefaultHomeDir\" has to be set properly first" << endl;
				gMrbLog->Flush(this->ClassName(), "SetPath");
				return(kFALSE);
			} else {
				this->SetHomeDir(remoteHome.Data());
				pathName = remoteHome;
			}
		}

		pathName += "/";
		pathName += Path; 			// append current path
	}

	if (gSystem->GetPathInfo(pathName.Data(), &dmy, &dmy64, &flags, &dmy) != 0 || (flags & 0x2) == 0) {
		if (Create) {
			if (gSystem->MakeDirectory(pathName.Data()) == 0) {
				gMrbLog->Out() << "Creating directory " << pathName << endl;
				gMrbLog->Flush(this->ClassName(), "SetPath", setblue);
			} else {
				gMrbLog->Err() << "Can't create directory - " << pathName << endl;
				gMrbLog->Flush(this->ClassName(), "SetPath");
				return(kFALSE);
			}
		} else {
			gMrbLog->Err() << "No such directory - " << pathName << endl;
			gMrbLog->Flush(this->ClassName(), "SetPath");
			return(kFALSE);
		}
	}
	this->Set("Path", Path);
	return(kTRUE);
}

const Char_t * TMbsSetup::GetPath() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::GetPath
// Purpose:        Read path where to write setup files
// Arguments:      --
// Results:        Char_t * Path       -- path name
// Exceptions:
// Description:    Gets resource "TMbsSetup.Path".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	this->Get(fPath, "Path");
	return(fPath.Data());
}

Bool_t TMbsSetup::SetNofReadouts(Int_t NofReadouts) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetNofReadouts
// Purpose:        Define number of readout procs
// Arguments:      Int_t NofReadouts    -- number of readout procs
// Results:        --
// Exceptions:
// Description:    Sets resource "TMbsSetup.NofReadouts".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n;
	Int_t nofRdo;
	Int_t maxReadouts;

	maxReadouts = this->Get("MaxReadouts", 0);

	if (NofReadouts > maxReadouts) {
		gMrbLog->Err() << "Too many readout procs - " << NofReadouts << " (max " << maxReadouts << " allowed)" << endl;
		gMrbLog->Flush(this->ClassName(), "SetNofReadouts");
		return(kFALSE);
	} else {
		nofRdo = this->GetNofReadouts();
		this->Set("NofReadouts", NofReadouts);
		for (n = nofRdo; n < NofReadouts; n++) fLofReadouts.Add(new TMbsReadoutProc(n));
		for (n = NofReadouts; n < nofRdo; n++) {
			ReadoutProc(n)->RemoveSetup();
			fLofReadouts.RemoveAt(n);
		}
		return(kTRUE);
	}
}

Int_t TMbsSetup::GetNofReadouts() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::GetNofReadouts
// Purpose:        Return number of readout procs
// Arguments:      --
// Results:        Int_t NofReadouts    -- number of readout procs
// Exceptions:
// Description:    Reads resource "TMbsSetup.NofReadouts".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(this->Get("NofReadouts", 0));
}

Bool_t TMbsSetup::MakeSetupFiles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::MakeSetupFiles
// Purpose:        Install setup files
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Generates setup files from templates.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	gMrbLog->Out()	<< "Checking setup data base ..." << endl;
	gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");

	if (!this->CheckSetup()) {
		gMrbLog->Err() << "Sorry, can't proceed." << endl;
		gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
		return(kFALSE);
	}

	Int_t nofReadouts = this->GetNofReadouts();
	TMrbNamedX * setupMode = this->GetMode();
	EMbsSetupMode smode = (EMbsSetupMode) (setupMode ?  setupMode->GetIndex() : 0);

	TString templatePath;
	this->Get(templatePath, "TemplatePath");

	if (smode == kModeSingleProc)		templatePath += "/singleproc";
	else if (smode == kModeMultiProc)	templatePath += "/multiproc";
	else if (smode == kModeMultiBranch)	templatePath += "/multibranch";
	else {
		gMrbLog->Err() << "Wrong setup mode - " << setupMode->GetName() << "(" << setupMode->GetIndex() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
		return(kFALSE);
	}

	gSystem->ExpandPathName(templatePath);

	Int_t nofErrors = 0;

	TString installPath = this->GetPath();
	if (!installPath.BeginsWith("/") && !installPath.BeginsWith("./")) {
		installPath = this->GetHomeDir();
		installPath += "/";
		installPath += this->GetPath();
	}
	
	TString srcPath = templatePath;
	gMrbLog->Out()	<< "Installing file(s) " << srcPath << " -> " << installPath << " ..." << endl;
	gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
	void * dirPtr = gSystem->OpenDirectory(srcPath.Data());
	if (dirPtr == NULL) {
		gMrbLog->Err() << gSystem->GetError() << " - " << srcPath << endl;
		gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
		nofErrors++;
	} else {
		TObjArray lofFiles;
		lofFiles.Delete();
		TMrbSystem ux;
		for (;;) {
			const Char_t * dirEntry = gSystem->GetDirEntry(dirPtr);
			if (dirEntry == NULL) break;
			TString fileName = Form("%s/%s", srcPath.Data(), dirEntry);
			if (ux.IsRegular(fileName.Data())) lofFiles.Add(new TObjString(dirEntry));
		}
		if (lofFiles.GetEntries() == 0) {
			gMrbLog->Err() << "No files to install from directory " << srcPath << endl;
			gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
			nofErrors++;
		} else {
			TIterator * iter = lofFiles.MakeIterator();
			TObjString * os;
			while (os = (TObjString *) iter->Next()) {
				TString fileName = os->String();
				if (this->ExpandFile(0, srcPath, fileName, installPath, fileName)) {
					cout	<< setblue << "   ... " << fileName
							<< setblack << endl;
				} else {
					nofErrors++;
					cout	<< "   ... " << fileName
							<< setred << " (with errors)" << setblack << endl;
				}
			}
		}
	}

	if (smode == kModeMultiProc) {
		templatePath += "/vme";
		installPath += "/";
		for (Int_t nrdo = 0; nrdo < nofReadouts; nrdo++) {
			TString destPath = installPath + this->ReadoutProc(nrdo)->GetPath();
			TString srcPath = templatePath;
			gMrbLog->Out()	<< "Installing file(s) " << srcPath << " -> " << destPath << " ..." << endl;
			gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
			void * dirPtr = gSystem->OpenDirectory(srcPath.Data());
			if (dirPtr == NULL) {
				gMrbLog->Err() << gSystem->GetError() << " - " << srcPath << endl;
				gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
				nofErrors++;
			} else {
				TObjArray lofFiles;
				lofFiles.Delete();
				TMrbSystem ux;
				for (;;) {
					const Char_t * dirEntry = gSystem->GetDirEntry(dirPtr);
					if (dirEntry == NULL) break;
					TString fileName = Form("%s/%s", srcPath.Data(), dirEntry);
					if (ux.IsRegular(fileName.Data()))  lofFiles.Add(new TObjString(dirEntry));
				}
				if (lofFiles.GetEntries() == 0) {
					gMrbLog->Err() << "No files to install from directory " << srcPath << endl;
					gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
					nofErrors++;
				} else {
					TIterator * iter = lofFiles.MakeIterator();
					TObjString * os;
					while (os = (TObjString *) iter->Next()) {
						TString fileName = os->String();
						if (this->ExpandFile(nrdo, srcPath, fileName, destPath, fileName)) {
							cout	<< setblue << "   ... " << fileName
									<< setblack << endl;
						} else {
							nofErrors++;
							cout	<< "   ... " << fileName
									<< setred << " (with errors)" << setblack << endl;
						}
					}
				}
			}
		}
	} else if (smode == kModeMultiBranch) {
		templatePath += "/vme";
		installPath += "/";
		for (Int_t nrdo = 0; nrdo < nofReadouts; nrdo++) {
			TString destPath = installPath + this->ReadoutProc(nrdo)->GetPath();
			TString srcPath = templatePath;
			gMrbLog->Out()	<< "Installing file(s) " << srcPath << " -> " << destPath << " ..." << endl;
			gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
			void * dirPtr = gSystem->OpenDirectory(srcPath.Data());
			if (dirPtr == NULL) {
				gMrbLog->Err() << gSystem->GetError() << " - " << srcPath << endl;
				gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
				nofErrors++;
			} else {
				TObjArray lofFiles;
				lofFiles.Delete();
				TMrbSystem ux;
				for (;;) {
					const Char_t * dirEntry = gSystem->GetDirEntry(dirPtr);
					if (dirEntry == NULL) break;
					TString fileName = Form("%s/%s", srcPath.Data(), dirEntry);
					if (ux.IsRegular(fileName.Data()))  lofFiles.Add(new TObjString(dirEntry));
				}
				if (lofFiles.GetEntries() == 0) {
					gMrbLog->Err() << "No files to install from directory " << srcPath << endl;
					gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
					nofErrors++;
				} else {
					TIterator * iter = lofFiles.MakeIterator();
					TObjString * os;
					while (os = (TObjString *) iter->Next()) {
						TString srcFile = os->String();
						TString destFile = os->String();
						destFile.ReplaceAll("NNN", Form("%d", nrdo));
						if (this->ExpandFile(nrdo, srcPath, srcFile, destPath, destFile)) {
							cout	<< setblue << "   ... " << srcFile << " -> " << destFile
									<< setblack << endl;
						} else {
							nofErrors++;
							cout	<< "   ... " << srcFile << " -> " << destFile
									<< setred << " (with errors)" << setblack << endl;
						}
					}
				}
			}
		}
	}

	this->Get(templatePath, "TemplatePath");

	if (smode == kModeSingleProc)	templatePath += "/singleproc";
	else if (smode == kModeMultiProc)	templatePath += "/multiproc";
	else if (smode == kModeMultiBranch)	templatePath += "/multibranch";
	gSystem->ExpandPathName(templatePath);

	TString nodelistFile = this->GetPath();
	if (!nodelistFile.BeginsWith("/") && !nodelistFile.BeginsWith("./")) {
		nodelistFile = this->GetHomeDir();
		nodelistFile += "/";
		nodelistFile += this->GetPath();
	}
	nodelistFile += "/node_list.txt";
	cout	<< this->ClassName() << "::MakeSetupFiles(): Creating node list " << nodelistFile << " ..."
			<< endl;
	if (!this->CreateNodeList(nodelistFile)) nofErrors++;

	TString rhostsFile = this->GetHomeDir();
	rhostsFile += "/.rhosts";
	cout	<< this->ClassName() << "::MakeSetupFiles(): Writing host names to file " << rhostsFile << " ..."
			<< endl;
	if (!this->WriteRhostsFile(rhostsFile)) nofErrors++;

	if (nofErrors > 0) {
		gMrbLog->Err() << "Something went wrong, files may be incomplete ..." << endl;
		gMrbLog->Flush(this->ClassName(), "MakeSetupFiles");
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}

Bool_t TMbsSetup::WriteRhostsFile(TString & RhostsFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::WriteRhostsFile
// Purpose:        Rewrite .rhosts in home dir
// Arguments:      TString RhostsFile   -- path to .rhosts
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates a new .rhosts file containing all Lynx and Marabou
//                 hosts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////


	ofstream rhosts;

	Bool_t isOK = kTRUE;

	rhosts.open(RhostsFile, ios::out);
	if (!rhosts.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << RhostsFile << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
		return(kFALSE);
	}

	TString userName = gSystem->Getenv("USER");

	TMrbString hString = gEnv->GetValue("TMbsSetup.DefaultHost", "");
	if (hString.IsNull()) {
		hString = gSystem->Getenv("HOSTNAME");
		if (hString.IsNull()) {
			gMrbLog->Err()	<< "Can't determine hostname - set .rootrc:TMbsSetup.DefaultHost properly" << endl;
			gMrbLog->Err()	<< "Can't determine hostname - $HOSTNAME is empty (may be it's not exported)" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
			isOK = kFALSE;
		}
	}
	for (Int_t i = 0; i < kNofPPCs; i++) {
		hString += ":";
		hString += "ppc-";
		hString += i;
	}

	Int_t nofHosts = 0;
	Bool_t once = kFALSE;
	TString domain = "";
	TObjArray hNames;
	Int_t nh = hString.Split(hNames, ":", kTRUE);
	for (Int_t i = 0; i < nh; i++) {
		TString hName = ((TObjString *) hNames[i])->GetString();
		hName = hName.Strip(TString::kBoth);
		if (hName.IsNull()) continue;
		TInetAddress * ia = new TInetAddress(gSystem->GetHostByName(hName));
		TString hAddr = ia->GetHostName();
		if (hAddr.CompareTo("UnknownHost") == 0) {
			gMrbLog->Err() << "Can't resolve host name - " << hName << " (ignored)" << endl;
			gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
			continue;
		}
		
		if (domain.IsNull()) {
			Char_t hstr[1024];
			TString hcmd = "host ";
			hcmd += hName;
			FILE * hPipe = gSystem->OpenPipe(hcmd, "r");
			while (fgets(hstr, 1024, hPipe) != NULL) {
				hAddr = hstr;
				if (hAddr.Contains("has address")) {
					hAddr.Resize(hAddr.Index(" ", 0));
					break;
				}
			}
			gSystem->ClosePipe(hPipe);
		}

		if (hAddr.Index(".", 0) == -1) {
			if (domain.IsNull()) domain = gEnv->GetValue("TMbsSetup.DefaultDomain", "");
			if (domain.IsNull()) {
				if (!once) {
					gMrbLog->Err()	<< "Can't get default domain - set TMbsSetup.DefaultDomain in .rootrc properly" << endl;
					gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
				}
				once = kTRUE;
				if (this->IsVerbose()) {
					gMrbLog->Err()	<< "Got short host name \"" << hName
									<< "\" while expecting full host addr - only SHORT name written to .rhosts file" << endl;
					gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
				}
				rhosts << hName << " " << userName << endl;
				nofHosts++;
			} else {
				if (this->IsVerbose()) {
					gMrbLog->Wrn()	<< "Got short host name \"" << hName
									<< "\" while expecting full host addr - appending default domain \"." << domain << "\"" << endl;
					gMrbLog->Flush(this->ClassName(), "WriteRhostsFile");
				}
				hAddr = hName;
				hAddr += ".";
				hAddr += domain;
				rhosts << hName << " " << userName << endl;
				rhosts << hAddr << " " << userName << endl;
				nofHosts++;
			}
		} else {
			hAddr.ReplaceAll("\t", " ");
			Int_t blank = hAddr.Index(" ", 0);
			if (blank != -1) hAddr.Resize(blank);
			domain = hAddr(hAddr.Index(".", 0) + 1, 1000);
			rhosts << hName << " " << userName << endl;
			rhosts << hAddr << " " << userName << endl;
			nofHosts++;
		}
	}
	rhosts.close();

	if (isOK) {
		cout	<< setblue
				<< this->ClassName() << "::WriteRhostsFile(): " << nofHosts
				<< " Lynx/MARaBOU host(s) written"
				<< setblack << endl;
	}
	return(isOK);
}

Bool_t TMbsSetup::CreateNodeList(TString & NodeListFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::CreateNodeList
// Purpose:        Rewrite node list for MBS
// Arguments:      TString NodeListFile   -- path to node_list.txt
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Creates file node_list.txt
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t i;
	ofstream nodeList;
	Int_t nofReadouts;

	TMrbNamedX * setupMode;
	EMbsSetupMode smode;

	nofReadouts = this->GetNofReadouts();

	nodeList.open(NodeListFile, ios::out);
	if (!nodeList.good()) {
		gMrbLog->Err() << gSystem->GetError() << " - " << NodeListFile << endl;
		gMrbLog->Flush(this->ClassName(), "CreateNodeList");
		return(kFALSE);
	}

	setupMode = this->GetMode();
	smode = (EMbsSetupMode) (setupMode ? setupMode->GetIndex() : 0);

	nodeList << this->EvtBuilder()->GetProcName() << endl;
	if (smode == kModeMultiProc) {
		for (i = 0; i < nofReadouts; i++) nodeList << this->ReadoutProc(i)->GetProcName() << endl;
	}
	nodeList.close();
	return(kTRUE);
}

Bool_t TMbsSetup::ExpandFile(Int_t ProcID, TString & TemplatePath, TString & SrcFile, TString & DestPath, TString & DestFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::ExpandFile
// Purpose:        Expand setup file
// Arguments:      Int_t ProcID                 -- readout proc id
//                 TString & TemplatePath       -- where templates reside
//                 TString & SrcFile            -- template source
//                 TString & DestPath           -- destination
//                 TString & DestFile           -- where to write expanded code
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Opens a template file and expands it.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofReadouts = this->GetNofReadouts();
	
	TString mbsPath = this->GetPath();
	if (!mbsPath.BeginsWith("/") && !mbsPath.BeginsWith("./")) {
		mbsPath = this->GetHomeDir();
		mbsPath += "/";
		mbsPath += this->GetPath();
	}

	TMrbNamedX * setupMode = this->GetMode();
	EMbsSetupMode sModeIdx = (EMbsSetupMode) (setupMode ? setupMode->GetIndex() : 0);
	TString sMode = "*";
	switch (sModeIdx) {
		case kModeSingleProc:	sMode = "SP"; break;
		case kModeMultiProc:	sMode = "MP"; break;
		case kModeMultiBranch:	sMode = "MB"; break;
	}
		
	TString templateFile = TemplatePath;
	templateFile += "/";
	templateFile += SrcFile;

	TMrbTemplate stpTmpl;
	if (!stpTmpl.Open(templateFile, &fLofSetupTags)) return(kFALSE);

	TString setupFile = DestPath;
	setupFile += "/";
	setupFile += DestFile;

	ofstream stp(setupFile.Data(), ios::out);
	if (!stp.good()) {	
		gMrbLog->Err() << gSystem->GetError() << " - " << setupFile << endl;
		gMrbLog->Flush(this->ClassName(), "ExpandFile");
		return(kFALSE);
	}

	Bool_t isOK = kTRUE;

	TString mbsVersion;
	this->Get(mbsVersion, "MbsVersion");
	if (mbsVersion.IsNull()) {
		gMrbLog->Err() << "MBS version not given - set TMbsSetup.MbsVersion properly" << endl;
		gMrbLog->Flush(this->ClassName(), "ExpandFile");
		return(kFALSE);
	}

	TString lynxVersion;
	this->Get(lynxVersion, "LynxVersion");
	if (lynxVersion.IsNull()) {
		gMrbLog->Err() << "Lynx version not given - set TMbsSetup.LynxVersion properly" << endl;
		gMrbLog->Flush(this->ClassName(), "ExpandFile");
		return(kFALSE);
	}

	TMrbNamedX * pType = this->ReadoutProc(ProcID)->GetType();
	TMrbNamedX * cType = this->ReadoutProc(ProcID)->GetController();
	TMrbNamedX * mType = this->ReadoutProc(ProcID)->TriggerModule()->GetType();

	for (;;) {
		TString line;
		TMrbNamedX * setupTag = stpTmpl.Next(line);
		if (stpTmpl.IsEof()) break;
		if (stpTmpl.IsError()) { isOK = kFALSE; continue; }
		if (stpTmpl.Status() & TMrbTemplate::kNoTag) {
			if (line.Index("//-") != 0) stp << line << endl;
		} else {
			Int_t tagIdx;
			switch (tagIdx = setupTag->GetIndex()) {
				case kSetAuthor:
					{
						stp << stpTmpl.Encode(line, gSystem->Getenv("USER")) << endl;
					}
					break;
				case kSetDate:
					{
						TDatime dt;
						stp << stpTmpl.Encode(line, dt.AsString()) << endl;
					}
					break;
				case kSetHostName:
					stpTmpl.InitializeCode();
					stpTmpl.Substitute("$hostName", this->EvtBuilder()->GetProcName());
					stpTmpl.WriteCode(stp);
					break;

				case kSetRdoNames:
					for (Int_t i = 0; i < nofReadouts; i++) {
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoNo", (Int_t) i);
						stpTmpl.Substitute("$rdoName", this->ReadoutProc(i)->GetProcName());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetHostFlag:
					{
						TArrayI arrayData(16);
						for (Int_t i = 0; i < nofReadouts; i++) arrayData[i] = 1;
						for (Int_t i = nofReadouts; i < kNofRdoProcs; i++) arrayData[i] = 0;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoFlagArr", this->EncodeArray(arrayData, kNofRdoProcs));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetSbsSetupPath:
					for (Int_t i = 0; i < nofReadouts; i++) {
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoNo", (Int_t) i);
						stpTmpl.Substitute("$rdoPath", this->ReadoutProc(i)->GetPath());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRdPipeBaseAddr:
					{
							TArrayI arrayData(16);
						for (Int_t i = nofReadouts; i < kNofRdoProcs; i++) arrayData[i] = 0;
						for (Int_t i = 0; i < nofReadouts; i++) {
							UInt_t pipeBase = this->ReadoutProc(i)->GetPipeBase();
							if (pipeBase == 0) {
								this->GetRcVal(pipeBase, "RdPipeBaseAddr", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
							}
							if (pipeBase == 0) {
								gMrbLog->Err()	<< "Base addr for readout pipe (#"
												<< (i + 1) << ") is 0 (see resource \"TMbsSetup.Readout"
												<< (i + 1) << ".RdPipeBaseAddr\")" << endl;
								gMrbLog->Flush(this->ClassName(), "ExpandFile");
								isOK = kFALSE;
							} else {
								arrayData[i] = pipeBase;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoPipeArr", this->EncodeArray(arrayData, kNofRdoProcs, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetBuffers:
					{
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$bufSize", this->EvtBuilder()->GetBufferSize(), 16);
						stpTmpl.Substitute("$nofBufs", (Int_t) this->EvtBuilder()->GetNofBuffers());
						stpTmpl.Substitute("$nofStreams", (Int_t) this->EvtBuilder()->GetNofStreams());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kStartReadout:
				case kStopReadout:
				case kReloadReadout:
					for (Int_t i = 0; i < nofReadouts; i++) {
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoProc", this->ReadoutProc(i)->GetProcName());
						stpTmpl.Substitute("$rdoPath", this->ReadoutProc(i)->GetPath());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kStartEvtBuilder:
				case kStopEvtBuilder:
					stpTmpl.InitializeCode();
					stpTmpl.Substitute("$evbProc", this->EvtBuilder()->GetProcName());
					stpTmpl.Substitute("$evbPath", mbsPath);
					stpTmpl.WriteCode(stp);
					break;

				case kStartEsone:
					if (cType->GetIndex() != kControllerUndefined) {
						stpTmpl.InitializeCode();
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetDispRdo:
					stpTmpl.InitializeCode();
					stpTmpl.Substitute("$rdoProc", this->ReadoutProc(ProcID)->GetProcName());
					stpTmpl.WriteCode(stp);
					break;

				case kSetDispEvb:
					stpTmpl.InitializeCode();
					stpTmpl.Substitute("$evbProc", this->EvtBuilder()->GetProcName());
					stpTmpl.WriteCode(stp);
					break;

				case kSetFlushTime:
					stpTmpl.InitializeCode();
					stpTmpl.Substitute("$flushTime", (Int_t) this->EvtBuilder()->GetFlushTime());
					stpTmpl.WriteCode(stp);
					break;

				case kSetMasterType:
					{
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoIntType", (Int_t) pType->GetIndex());
						stpTmpl.Substitute("$rdoAscType", pType->GetName());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemMemoryBase:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteMemoryBase"), 0);
						if (memBase == 0) {
							this->GetRcVal(memBase, "RemoteMemoryBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
						}
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteMemoryLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "RemoteMemoryLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memBase;
								memBase += memLength;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemMemoryBase", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemMemoryOffset:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memOffs = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteMemoryOffset"), 0);
						if (memOffs == 0) {
							this->GetRcVal(memOffs, "RemoteMemoryOffset", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memOffs;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemMemoryOffset", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemMemoryLength:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteMemoryLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "RemoteMemoryLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memLength;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemMemoryLength", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemCamacBase:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacBase"), 0);
						if (memBase == 0) {
							this->GetRcVal(memBase, "RemoteCamacBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
						} 
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "RemoteCamacLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memBase;
								memBase += memLength;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemCamacBase", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemCamacLength:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "RemoteCamacLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memLength;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemCamacLength", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemCamacOffset:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memOffs = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacOffset"), 0);
						if (memOffs == 0) {
							this->GetRcVal(memOffs, "RemoteCamacOffset", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
						} 
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							if (crate == 0) {
								arrayData[crate] = 0;
							} else {
								arrayData[crate] = memOffs;
							}
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemCamacOffset", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocEsoneBase:
					{
						TString res;
						UInt_t memBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalEsoneBase"), 0);
						if (memBase == 0) {
							this->GetRcVal(memBase, "LocalEsoneBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocEsoneBase", memBase, 16);
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetRemEsoneBase:
					{
						TString res;
						UInt_t memBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacBase"), 0);
						if (memBase == 0) {
							this->GetRcVal(memBase, "RemoteCamacBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "RemoteCamacLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "RemoteCamacLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						memBase -= memLength;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoRemEsoneBase", memBase, 16);
						stpTmpl.Substitute("$rdoRemCamacLength", memLength, 16);
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocMemoryBase:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalMemoryBase"), 0);
						if (memBase == 0) {
							this->GetRcVal(memBase, "LocalMemoryBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = memBase;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocMemoryBase", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocMemoryLength:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t memLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalMemoryLength"), 0);
						if (memLength == 0) {
							this->GetRcVal(memLength, "LocalMemoryLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = memLength;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocMemoryLength", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocPipeBase:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t pipeBase = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalPipeBase"), 0);
						if (pipeBase == 0) {
							this->GetRcVal(pipeBase, "LocalPipeBase", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = pipeBase;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocPipeBase", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocPipeOffset:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t pipeOffs = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalPipeOffset"), 0);
						if (pipeOffs == 0) {
							this->GetRcVal(pipeOffs, "LocalPipeOffset", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = pipeOffs;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocPipeOffset", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocPipeSegLength:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t pipeLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalPipeSegmentLength"), 0);
						if (pipeLength == 0) {
							this->GetRcVal(pipeLength, "LocalPipeSegmentLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = pipeLength;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocPipeSegLength", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetLocPipeLength:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						TString res;
						UInt_t pipeLength = this->Get(this->Resource(res, "Readout", ProcID + 1, "LocalPipeLength"), 0);
						if (pipeLength == 0) {
							this->GetRcVal(pipeLength, "LocalPipeLength", cType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data()); 
						} 
						arrayData[0] = pipeLength;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoLocPipeLength", this->EncodeArray(arrayData, kNofCrates, 16));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetSevtSize:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t i = 1; i < kNofTriggers; i++) {
							for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
							Bool_t found = kFALSE;
							Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
							Int_t sevtSize = 0;
							for (Int_t j = 0; j < n; j++) {
								Int_t crate = lofCrates[j];
								sevtSize = this->ReadoutProc(ProcID)->GetSevtSize(i);
								if (sevtSize == 0) {
									if (i == 1 || i == 14 || i == 15) sevtSize = this->ReadoutProc(ProcID)->GetSevtSize();
								}
								if (sevtSize > arrayData[crate]) {
									found = kTRUE;
									arrayData[crate] = sevtSize;
								}
							}
							arrayData[0] = sevtSize;
							if (found) {
								stpTmpl.InitializeCode();
								stpTmpl.Substitute("$trigNo", (Int_t) i);
								stpTmpl.Substitute("$sevtBufArr", this->EncodeArray(arrayData, kNofCrates));
								stpTmpl.WriteCode(stp);
							}
						}
					}
					break;

				case kSetRdoFlag:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						for (Int_t i = 0; i < n; i++) {
							Int_t crate = lofCrates[i];
							arrayData[crate] = 1;
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$rdoFlagArr", this->EncodeArray(arrayData, kNofCrates));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetControllerID:
					{
						TArrayI lofCrates(kNofCrates);
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						Int_t rdoCrate = this->ReadoutProc(ProcID)->GetCrate();
						TMrbNamedX * k = this->ReadoutProc(ProcID)->GetType();
						arrayData[rdoCrate] = k->GetIndex();
						Int_t n = this->ReadoutProc(ProcID)->GetCratesToBeRead(lofCrates);
						k = this->ReadoutProc(ProcID)->GetController();
						for (Int_t j = 0; j < n; j++) {
							Int_t crate = lofCrates[j];
							if (crate != rdoCrate) arrayData[crate] = k->GetIndex();
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$contrIdArr", this->EncodeArray(arrayData, kNofCrates));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetTriggerModule:
					{
						TMrbNamedX * k = this->ReadoutProc(ProcID)->TriggerModule()->GetType();
						stpTmpl.InitializeCode();
						switch (k->GetIndex()) {
							case kTriggerModuleCAMAC:
								stpTmpl.Substitute("$trigModuleComment", "TriggerModule: CAMAC / Polling");
								stpTmpl.Substitute("$trigMode", (Int_t) kTriggerModePolling);
								stpTmpl.Substitute("$trigType", (Int_t) kTriggerModuleCAMAC);
								stpTmpl.Substitute("$trigBase", "0xd0380000");
								break;
							case kTriggerModuleVME:
								k = this->ReadoutProc(ProcID)->TriggerModule()->GetTriggerMode();
								if (k->GetIndex() == kTriggerModeLocalInterrupt) {
									stpTmpl.Substitute("$trigModuleComment", "TriggerModule: VME / Local interrupt");
									stpTmpl.Substitute("$trigMode", (Int_t) kTriggerModeLocalInterrupt);
									stpTmpl.Substitute("$trigType", (Int_t) kTriggerModuleVME);
									stpTmpl.Substitute("$trigBase", "0x0");
								} else if (k->GetIndex() == kTriggerModeVsbInterrupt) {
									stpTmpl.Substitute("$trigModuleComment", "TriggerModule: VME / VSB interrupt (not implemented!!)");
									stpTmpl.Substitute("$trigMode", (Int_t) kTriggerModeVsbInterrupt);
									stpTmpl.Substitute("$trigType", 0);
									stpTmpl.Substitute("$trigBase", "0x0");
									gMrbLog->Wrn() << "Trigger mode \"VSB Interrupt\" not implemented" << endl;
									gMrbLog->Flush(this->ClassName(), "ExpandFile");
								} else {
									stpTmpl.Substitute("$trigModuleComment", "TriggerModule: VME / Polling");
									stpTmpl.Substitute("$trigMode", (Int_t) kTriggerModePolling);
									stpTmpl.Substitute("$trigType", (Int_t) kTriggerModuleVME);
									UInt_t trigBase;
									this->GetRcVal(trigBase, "TriggerModuleBase", mType->GetName(), pType->GetName(), sMode.Data(), mbsVersion.Data(), lynxVersion.Data());
									stpTmpl.Substitute("$trigBase", trigBase, 16);
								}
								break;
						}
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetTrigModStation:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						Int_t crate = this->ReadoutProc(ProcID)->GetCrate();
						arrayData[crate] = 1;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$trigStatArr", this->EncodeArray(arrayData, kNofCrates));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetTrigModFastClear:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						Int_t crate = this->ReadoutProc(ProcID)->GetCrate();
						Int_t fct = this->ReadoutProc(ProcID)->TriggerModule()->GetFastClearTime();
						arrayData[crate] = (fct == 0) ? kMinFCT : fct;
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$trigFctArr", this->EncodeArray(arrayData, kNofCrates));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetTrigModConvTime:
					{
						TArrayI arrayData(16);
						for (Int_t crate = 0; crate < kNofCrates; crate++) arrayData[crate] = 0;
						Int_t crate = this->ReadoutProc(ProcID)->GetCrate();
						arrayData[crate] = this->ReadoutProc(ProcID)->TriggerModule()->GetConversionTime();
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$trigCvtArr", this->EncodeArray(arrayData, kNofCrates));
						stpTmpl.WriteCode(stp);
					}
					break;

				case kLoadRdoSetup:
				case kStartRdoTask:
					{
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$mbsPath", mbsPath);
						stpTmpl.Substitute("$rdoPath", this->ReadoutProc(ProcID)->GetPath());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetIrqMode:
					{
						TMrbNamedX * k = this->ReadoutProc(ProcID)->TriggerModule()->GetTriggerMode();
						TString mode;
						if (k->GetIndex() == kTriggerModeVsbInterrupt) mode = "enable"; else mode = "disable";
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$irqMode", mode);
						stpTmpl.WriteCode(stp);
					}
					break;

				case kTestHostName:
				case kPrintBanner:
					if (sModeIdx == kModeMultiProc) {
						stpTmpl.InitializeCode("%B%");
						stpTmpl.Substitute("$evbName", this->EvtBuilder()->GetProcName());
						stpTmpl.WriteCode(stp);
						for (Int_t i = 0; i < nofReadouts; i++) {
							stpTmpl.InitializeCode("%L%");
							stpTmpl.Substitute("$rdoName", this->ReadoutProc(i)->GetProcName());
							stpTmpl.WriteCode(stp);
						}
						stpTmpl.InitializeCode("%E%");
						stpTmpl.WriteCode(stp);
					} else {
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$procName", this->EvtBuilder()->GetProcName());
						stpTmpl.WriteCode(stp);
					}
					break;

				case kSetVsbAddr:
					if (sModeIdx == kModeMultiProc) {
						if (!gEnv->GetValue("TMbsSetup.ConfigVSB", kTRUE)) {
							gMrbLog->Err() << "TMbsSetup.ConfigVSB has to be set in a MULTIPROC environment" << endl;
							gMrbLog->Flush(this->ClassName(), "ExpandFile");
							return(kFALSE);
						}
						stpTmpl.InitializeCode("%B%");
						stpTmpl.Substitute("$evbName", this->EvtBuilder()->GetProcName());
						stpTmpl.Substitute("$vsbAddr",this->EvtBuilder()->GetVSBAddr(), 16);
						stpTmpl.WriteCode(stp);
						for (Int_t i = 0; i < nofReadouts; i++) {
							stpTmpl.InitializeCode("%L%");
							stpTmpl.Substitute("$rdoName", this->ReadoutProc(i)->GetProcName());
							stpTmpl.Substitute("$vsbAddr", this->ReadoutProc(i)->GetVSBAddr(), 16);
							stpTmpl.WriteCode(stp);
						}
					} else if (gEnv->GetValue("TMbsSetup.ConfigVSB", kTRUE)) {
							stpTmpl.InitializeCode();
							stpTmpl.Substitute("$procName", this->EvtBuilder()->GetProcName());
							stpTmpl.Substitute("$vsbAddr", this->EvtBuilder()->GetVSBAddr(), 16);
							stpTmpl.WriteCode(stp);
					}
					break;

				case kMbsLogin:
					{
						TString mbsv;
						if (mbsVersion(0) != 'v') {
							mbsv = Form("v%c%c", mbsVersion(0), mbsVersion(2));
						} else {
							mbsv = mbsVersion;
						}
						stpTmpl.InitializeCode();
						stpTmpl.Substitute("$mbsVersion", mbsv.Data());
						stpTmpl.WriteCode(stp);
					}
					break;
			}
		}
	}
	stp.close();
	return(isOK);
}

const Char_t * TMbsSetup::EncodeArray(TArrayI & Data, Int_t NofEntries, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::EncodeArray
// Purpose:        Fill array data in a string
// Arguments:      TArray & Data         -- data to be encoded
//                 Int_t NofEntries      -- number of data entries
//                 Int_t Base            -- base
// Results:        Char_t * ArrayString  -- string representing array data
// Exceptions:
// Description:    Produces a string "(d1, d2, ..., dN)".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbString s;
	for (Int_t i = 0; i < NofEntries; i++) {
		if (i == 0) s = "("; else s += ",";
		switch (Base) {
			default:	Base = 10;
			case 8:
			case 16: 	s.AppendInteger(Data[i], 0, Base, kFALSE, kTRUE); break;
		}
	}
	s += ")";
	fArrayString = s;
	return(fArrayString.Data());
}

const Char_t * TMbsSetup::EncodeArray(Int_t Data, Int_t Index, Int_t NofEntries, Int_t Base) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::EncodeArray
// Purpose:        Fill array data in a string
// Arguments:      Int_t Data            -- data to be encoded
//                 Int_t Index           -- array index
//                 Int_t NofEntries      -- number of data points
//                 Int_t Base            -- base
// Results:        Char_t * ArrayString  -- string representing array data
// Exceptions:
// Description:    Produces a string "(0, ..., d, ..., 0)".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t data;
	TMrbString s;

	for (Int_t i = 0; i < NofEntries; i++) {
		data = (i == Index) ? Data : 0;
		if (i == 0) s = "("; else s += ",";
		switch (Base) {
			default:	Base = 10;
			case 8:
			case 16: 	s.AppendInteger(Data, 0, Base, kFALSE, kTRUE); break;
		}
	}
	s += ")";
	fArrayString = s;
	return(fArrayString.Data());
}

Bool_t TMbsSetup::CheckSetup() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::CheckSetup
// Purpose:        Consistency check
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Checks resource data base for its consistency.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * resType;
	Int_t nofErrors;

	Int_t sevtSize;

	Int_t n;
	Int_t nofReadouts;

	TString evtBuilder;
	TString readoutProc;

	TMrbNamedX * setupMode;
	EMbsSetupMode smode;

	TString fileList;
	Long_t dmy;
	Long64_t dmy64;
	Long_t flags;

	Int_t cvt;
	Int_t fct;

	nofErrors = 0;

	TString remoteHome = this->RemoteHomeDir();	// get home dir via remote login
	if (remoteHome.IsNull()) {
		gMrbLog->Err() << "Can't get remote home dir via rsh - MBS host not responding" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		nofErrors++;
	}

// TMbsSetup.TemplatePath:
	TString templatePath;
	this->Get(templatePath, "TemplatePath");
	if (templatePath.Length() == 0) {
		gMrbLog->Err() << fResourceName << " is not set" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		nofErrors++;
	}

// TMbsSetup.HomeDir:
	TString resString = this->GetHomeDir();
	if (resString.Length() == 0) {
		if (remoteHome.IsNull()) {
			gMrbLog->Err() << fResourceName << " is not set" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckSetup");
			nofErrors++;
		} else if (!this->SetHomeDir(remoteHome.Data())) nofErrors++;
	} else {
		if (!remoteHome.IsNull() && resString.CompareTo(remoteHome.Data()) != 0) {
			gMrbLog->Err() << "HomeDir is different from what is returned by remote login" << endl;
			gMrbLog->Flush(this->ClassName(), "CheckSetup");
			gMrbLog->Err() << "Current value = " << resString << " <> remote = " << remoteHome << endl;
			gMrbLog->Flush(this->ClassName(), "CheckSetup");
		}
		if (!this->SetHomeDir(resString.Data())) nofErrors++;
	}

// TMbsSetup.Path:
	resString = this->GetPath();
	if (resString.Length() == 0) {
		gMrbLog->Err() << fResourceName << " is not set" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		nofErrors++;
	} else if (!this->SetPath(resString.Data())) nofErrors++;

// TMbsSetup.EvtBuilder.Name:
// TMbsSetup.EvtBuilder.Address:
// TMbsSetup.EvtBuilder.Type:
	resString = this->EvtBuilder()->GetProcName();
	if (resString.Length() == 0) {
		gMrbLog->Err() << fResourceName << " is not set" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		nofErrors++;
	} else if (!this->EvtBuilder()->SetProcName(resString.Data())) nofErrors++;

// TMbsSetup.EvtBuilder.Crate:

// TMbsSetup.EvtBuilder.NofStreams:
// TMbsSetup.EvtBuilder.NofBuffers:
// TMbsSetup.EvtBuilder.BufferSize:
// TMbsSetup.EvtBuilder.VSBAddr:

// TMbsSetup.NofReadouts:
	if ((nofReadouts = this->GetNofReadouts()) == 0) {
		gMrbLog->Err() << fResourceName << " is not set ..." << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		nofErrors++;
	} else {

		this->SetNofReadouts(nofReadouts);

		for (n = 0; n < nofReadouts; n++) {

//			TMbsSetup.ReadoutNNN.Name:
//			TMbsSetup.ReadoutNNN.Address:
//			TMbsSetup.ReadoutNNN.Type:
			resString = this->ReadoutProc(n)->GetProcName();
			if (resString.Length() == 0) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->SetProcName(resString.Data())) nofErrors++;

// TMbsSetup.ReadoutNNN.Crate:

// TMbsSetup.ReadoutNNN.TriggerModule.Type:
			resType = this->ReadoutProc(n)->TriggerModule()->GetType();
			if (resType == NULL) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->TriggerModule()->SetType((EMbsTriggerModuleType) resType->GetIndex())) nofErrors++;

// TMbsSetup.ReadoutNNN.TriggerModule.Mode:
			resType = this->ReadoutProc(n)->TriggerModule()->GetTriggerMode();
			if (resType == NULL) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->TriggerModule()->SetTriggerMode((EMbsTriggerMode) resType->GetIndex())) nofErrors++;

// TMbsSetup.ReadoutNNN.TriggerModule.ConvTime:
			cvt = this->ReadoutProc(n)->TriggerModule()->GetConversionTime();
			if (cvt == 0) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->TriggerModule()->SetConversionTime(cvt)) nofErrors++;

// TMbsSetup.ReadoutNNN.TriggerModule.FastClearTime:
			fct = this->ReadoutProc(n)->TriggerModule()->GetFastClearTime();
			if (fct == 0) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (fct >= cvt) {
				gMrbLog->Err()	<< "FastClearTime = " << fct
								<< " has to be smaller than ConversionTime = " << cvt << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->TriggerModule()->SetFastClearTime(fct)) nofErrors++;

// TMbsSetup.ReadoutNNN.Controller:
			TArrayI lofCrates(kNofCrates);
			Int_t ncrates = this->ReadoutProc(n)->GetCratesToBeRead(lofCrates);
			Bool_t useCamac = kFALSE;
			for (Int_t i = 0; i < ncrates; i++) {
				if (lofCrates[i] > 0) {
					useCamac = kTRUE;
					break;
				}
			}
			if (useCamac) {
				resType = this->ReadoutProc(n)->GetController();
				if (resType == NULL) {
					gMrbLog->Err() << fResourceName << " is not set" << endl;
					gMrbLog->Flush(this->ClassName(), "CheckSetup");
					nofErrors++;
				} else if (!this->ReadoutProc(n)->SetController((EMbsControllerType) resType->GetIndex())) nofErrors++;
			} else if (!this->ReadoutProc(n)->SetController(kControllerNoCamac)) nofErrors++;

// TMbsSetup.ReadoutNNN.VSBAddr:
// TMbsSetup.ReadoutNNN.PipeBase:

// TMbsSetup.ReadoutNNN.SevtSize:
			sevtSize = this->ReadoutProc(n)->GetSevtSize();
			if (sevtSize == 0) {
				gMrbLog->Err() << fResourceName << " is not set" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			} else if (!this->ReadoutProc(n)->SetSevtSize(0, sevtSize)) nofErrors++;
		}
	}

	if (nofErrors == 0) {
		evtBuilder = this->EvtBuilder()->GetProcName();
		readoutProc = this->ReadoutProc(0)->GetProcName();
		if (nofReadouts == 1 && evtBuilder.CompareTo(readoutProc.Data()) == 0) {
			this->SetMode(kModeSingleProc);
		} else {
			this->SetMode(kModeMultiProc);
		}
		setupMode = this->GetMode();
		if (setupMode != NULL) {
			cout	<< this->ClassName() << "::CheckSetup(): Setup mode is "
					<< setupMode->GetName() << "(" << setupMode->GetIndex() << ")"
					<< endl;
		}
		smode = (EMbsSetupMode) setupMode->GetIndex();

// TMbsSetup.ReadoutNNN.Path:
		if (smode == kModeMultiProc) {
			for (n = 0; n < nofReadouts; n++) {
				resString = this->ReadoutProc(n)->GetPath();
				if (resString.Length() == 0) {
					gMrbLog->Err() << fResourceName << " is not set" << endl;
					gMrbLog->Flush(this->ClassName(), "CheckSetup");
					nofErrors++;
				} else if (!this->ReadoutProc(n)->SetPath(resString.Data(), kTRUE)) nofErrors++;
			}
		}

		this->Get(templatePath, "TemplatePath");

		if (smode == kModeSingleProc) {
			if (this->ReadoutProc(0)->TriggerModule()->GetTriggerMode()->GetIndex() == kTriggerModePolling) {
				gMrbLog->Wrn() << "You are using POLLING on triggers in a single-ppc environment - INTERRUPT mode is normally used" << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
			}
			templatePath += "/singleproc";
		} else if (smode == kModeMultiBranch) {
			templatePath += "/multibranch";
		} else {
			templatePath += "/multiproc";
		}
		gSystem->ExpandPathName(templatePath);
		if (gSystem->GetPathInfo(templatePath.Data(), &dmy, &dmy64, &flags, &dmy) != 0 || (flags & 0x2) == 0) {
			gMrbLog->Err() << "No such directory - " << templatePath << endl;
			gMrbLog->Flush(this->ClassName(), "CheckSetup");
			nofErrors++;
		}

		if (setupMode->GetIndex() == kModeMultiProc) {
			templatePath += "/vme";
			if (gSystem->GetPathInfo(templatePath.Data(), &dmy, &dmy64, &flags, &dmy) != 0 || (flags & 0x2) == 0) {
				gMrbLog->Err() << "No such directory - " << templatePath << endl;
				gMrbLog->Flush(this->ClassName(), "CheckSetup");
				nofErrors++;
			}
		}
	}

	if (nofErrors > 0) {
		gMrbLog->Err() << nofErrors << " error(s) detected" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckSetup");
		return(kFALSE);
	} else {
		gMrbLog->Out() << "Check done - no errors." << endl;
		gMrbLog->Flush(this->ClassName(), "", setblue);
		return(kTRUE);
	}
}

const Char_t * TMbsSetup::RemoteHomeDir() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsEvtBuilder::RemoteHomeDir
// Purpose:        Get user's home directory as seen from MBS
// Arguments:      --
// Results:        Char_t * HomeDir    -- home directory
// Exceptions:
// Description:    Tries a "rsh <proc> pwd".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString hostName;
	TString rshCmd;
	TString defaultHomeDir;
	Char_t pstr[1024];
	FILE * pwd;

	this->Get(hostName, "EvtBuilder.Address");
	if (hostName.IsNull()) {
		gMrbLog->Err() << "No event builder defined" << endl;
		gMrbLog->Flush(this->ClassName(), "RemoteHomeDir");
		fRemoteHome.Resize(0);
	} else {
		defaultHomeDir = gEnv->GetValue("TMbsSetup.DefaultHomeDir", "");		
		if (defaultHomeDir.IsNull()) {
			rshCmd = "rsh ";					// get home dir from remote login
			rshCmd += hostName;
			rshCmd += " pwd 2>&1";
			pwd = gSystem->OpenPipe(rshCmd, "r");
			fgets(pstr, 1024, pwd);
			gSystem->ClosePipe(pwd);

			fRemoteHome = pstr;
			fRemoteHome = fRemoteHome.Strip(TString::kTrailing, '\n');
			fRemoteHome = fRemoteHome.Strip(TString::kBoth);
			if (fRemoteHome(0) != '/') {
				gMrbLog->Err() << "Error during \"" << rshCmd << " - " << fRemoteHome << endl;
				gMrbLog->Flush(this->ClassName(), "RemoteHomeDir");
				gMrbLog->Err() << "No default given (check \"TMbsSetup.DefaultHomeDir\" in .rootrc)" << endl;
				gMrbLog->Flush(this->ClassName(), "RemoteHomeDir");
				fRemoteHome.Resize(0);
			}
		} else {
			fRemoteHome = defaultHomeDir;
		}
	}
	Int_t x = fRemoteHome.Index(":", 0);
	if (x >= 0) {
		fRemoteMbsHome = fRemoteHome(x + 1, 1000);
		fRemoteHome.Resize(x);
	} else {
		fRemoteMbsHome = fRemoteHome;
	}
	return(fRemoteHome.Data());
}

Bool_t TMbsSetup::SetHomeDirFromRemote() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetHomeDirFromRemote
// Purpose:        Set user's home directory as seen from MBS
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.HomeDir".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString homeDir;

	homeDir = this->RemoteHomeDir();
	if (homeDir.IsNull()) {
		gMrbLog->Err() << "No remote host - define event builder first" << endl;
		gMrbLog->Flush(this->ClassName(), "SetHomeDirFromRemote");
		return(kFALSE);
	} else {
		if (this->SetHomeDir(homeDir.Data())) {
			gMrbLog->Out() << "HomeDir set to \"" << homeDir << "\"" << endl;
			gMrbLog->Flush(this->ClassName(), "SetHomeDirFromRemote", setblue);
			return(kTRUE);
		} else {
			return(kFALSE);
		}
	}
}

Bool_t TMbsSetup::SetMode(const Char_t * Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetMode
// Purpose:        Define setup mode
// Arguments:      Char_t * Mode    -- setup mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * setupMode = gMbsSetup->fLofSetupModes.FindByName(Mode, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	if (setupMode == NULL) {
		gMrbLog->Err() << "Wrong setup mode - " << Mode << endl;
		gMrbLog->Flush(this->ClassName(), "SetMode");
		cerr	<< this->ClassName() << "::SetMode(): Legal modes are - "
				<< endl;
		gMbsSetup->fLofSetupModes.Print(cerr, "   > ");
		return(kFALSE);
	}

	TString mode = Form("%s(%d)", setupMode->GetName(), setupMode->GetIndex());
	gMbsSetup->Set("Mode", mode.Data());
	return(kTRUE);
}

Bool_t TMbsSetup::SetMode(EMbsSetupMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::SetMode
// Purpose:        Define setup mode
// Arguments:      EMbsSetupMode Mode    -- setup mode
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines resource "TMbsSetup.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * setupMode = gMbsSetup->fLofSetupModes.FindByIndex(Mode);
	if (setupMode == NULL) {
		gMrbLog->Err() << "Wrong setup mode - " << Mode << endl;
		gMrbLog->Flush(this->ClassName(), "SetMode");
		cerr	<< this->ClassName() << "::SetMode(): Legal modes are - "
				<< endl;
		gMbsSetup->fLofSetupModes.Print(cerr, "   > ");
		return(kFALSE);
	}

	TString mode = Form("%s(%d)", setupMode->GetName(), setupMode->GetIndex());
	gMbsSetup->Set("Mode", mode.Data());
	return(kTRUE);
}

TMrbNamedX * TMbsSetup::GetMode() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup::GetMode
// Purpose:        Read setup mode
// Arguments:      --
// Results:        TMrbNamedX * Mode
// Exceptions:
// Description:    Reads resource "TMbsSetup.Mode".
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * setupMode;
	TString resValue;
	Int_t n;

	this->Get(resValue, "Mode");
	if ((n = resValue.Index("(")) >= 0) resValue = resValue(0, n);
	setupMode = this->fLofSetupModes.FindByName(resValue, TMrbLofNamedX::kFindExact | TMrbLofNamedX::kFindIgnoreCase);
	return(setupMode);
}
