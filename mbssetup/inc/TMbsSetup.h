#ifndef __TMbsSetup_h__
#define __TMbsSetup_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           mbssetup/inc/TMbsSetup.h
// Purpose:        Define a MBS setup
// Class:          TMbsSetup            -- base class
// Description:    Class definitions to generate a MBS setup.
// Author:         R. Lutter
// Revision:       $Id: TMbsSetup.h,v 1.3 2004-11-25 12:00:17 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TString.h"
#include "TArrayI.h"
#include "TObjArray.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbEnv.h"

#include "TMbsSetupCommon.h"
#include "TMbsEvtBuilder.h"
#include "TMbsReadoutProc.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMbsSetup
// Purpose:        Define setup and generate files
// Methods:
// Description:    Creates a MBS setup and generates setup files.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMbsSetup : public TMrbEnv {

	public:

		TMbsSetup(const Char_t * SetupFile = ".mbssetup");		// create config

		~TMbsSetup();	 				// explicit dtor

		Bool_t MakeSetupFiles();		// generate all files needed to run MBS

		inline TMbsReadoutProc * FirstReadout() { return((TMbsReadoutProc *) fLofReadouts.First()); };	// get readout
		inline TMbsReadoutProc * NextReadout(TMbsReadoutProc * Readout) {								// get next
					return((TMbsReadoutProc *) fLofReadouts.After((TObject *) Readout));
		};
		TMbsReadoutProc * ReadoutProc(Int_t ReadoutId);	// get readout proc by its number

		Bool_t SetMode(const Char_t * Mode);					// setup mode
		Bool_t SetMode(EMbsSetupMode Mode);
		TMrbNamedX * GetMode();

		Bool_t SetHomeDir(const Char_t * HomeDir);			// home dir (lynx)
		const Char_t * GetHomeDir();

		Bool_t SetPath(const Char_t * Path, Bool_t Create = kFALSE);		// where to put setup files
		const Char_t * GetPath();

		Bool_t SetNofReadouts(Int_t NofReadouts);					// number of readout procs
		Int_t GetNofReadouts();

		inline TMbsEvtBuilder * EvtBuilder() { return(fEvtBuilder); };

		Bool_t CheckSetup();					// check consistency

		void RemoveSetup(); 					// remove entries from setup data base
		void Reset();	 						// reset to default

		const Char_t * RemoteHomeDir();				// get home dir via remote login
		Bool_t SetHomeDirFromRemote();			// set home dir as seen by MBS

		const Char_t * EncodeArray(TArrayI & Data, Int_t NofEntries, Int_t Base = 10);	// convert array data to string
		const Char_t * EncodeArray(Int_t Data, Int_t Index, Int_t NofEntries, Int_t Base = 10);

		inline void Help() { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TMbsSetup.html&"); };

	protected:
		Bool_t ExpandFile(Int_t ProcID, TString & TemplatePath, TString & SetupFile, TString & DestPath);	// expand setup file
		Bool_t CreateNodeList(TString & NodeListFile);				// create node list
		Bool_t WriteRhostsFile(TString & RhostsFile);				// rewrite .rhosts

	public:										// public lists of key words:
		TMrbLofNamedX fLofSetupModes;			// ... setup modes
		TMrbLofNamedX fLofProcs;				// ... processors
		TMrbLofNamedX fLofControllers; 		// ... crate controllers
		TMrbLofNamedX fLofTriggerModules;		// ... trigger modules
		TMrbLofNamedX fLofTriggerModes; 		// ... trigger modes
		TMrbLofNamedX fLofSetupTags;	 		// ... setup tags

	protected:
		TString fTemplatePath; 					// where to find template files

		TMbsEvtBuilder * fEvtBuilder;			//! event builder (master)

		TObjArray fLofReadouts; 				// readout processors (slaves)
		TMbsReadoutProc * fReadoutError;		// ... error

		TString fHomeDir;						// temp storage: home dir
		TString fPath;							// ... path
		TString fRemoteHome;					// ... remote home dir
		TString fArrayString;					// temp storage method EncodeArray()

	ClassDef(TMbsSetup, 1)		// [MBS Setup] Base class to maintain setup data for GSI's MBS system
};

#endif
