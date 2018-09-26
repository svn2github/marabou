//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           ExaAnalyze.cxx
// Purpose:        Implement user-specific class methods
// Description:    Example configuration
//
// Header files:   ExaAnalyze.h     -- class definitions
// Author:         MBS and ROOT Based Online Offline Utility
// Revision:         
// Date:           Thu Nov 28 09:12:18 2002
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

static char sccsid[]	=	"%W%";

#include "TEnv.h"

// class definitions
#include "ExaAnalyze.h"

// common index declarations for analysis and readout
#include "ExaCommonIndices.h"

#include "SetColor.h"

#include <pthread.h>  // pthread header file

// implement class defs
ClassImp(TUsrEvtReadout)
ClassImp(TUsrSevtData)


// global objects
#include "ExaAnalyzeGlobals.h"



extern pthread_mutex_t global_data_mutex;		// global pthread mutex to protect TMapped data

extern TMrbLogger * gMrbLog;					// MARaBOU's logging mechanism

extern TMrbTransport * gMrbTransport;			// base class for MBS transport
extern TMrbAnalyze * gMrbAnalyze;				// base class for user's analysis

extern TUsrEvtStart * gStartEvent;				// start + stop events
extern TUsrEvtStop * gStopEvent;
extern TUsrDeadTime * gDeadTime;				// dead-time data

extern TMrbLofUserVars * gMrbLofUserVars;		// list of user vars/wdws

static Bool_t verboseMode;						// verbosity flag;

static Bool_t dumpIt = kFALSE;					// kTRUE - dump records
static Int_t recNo = 0;

Bool_t TMrbAnalyze::Initialize(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::Initialize
// Purpose:        Instantiate user objects, initialize values
// Arguments:      TMrbIOSpec * IOSpec  -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Instantiates events, histgrams, windows etc. and
//                 initializes values.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	verboseMode = (Bool_t) gEnv->GetValue("TMrbAnalyze.VerboseMode", kFALSE);

// add user's environment defs from file .ExaConfig.rc
	Bool_t fileOk = this->AddResourcesFromFile(".ExaConfig.rc");
	if (verboseMode && fileOk) {
		gMrbLog->Out()	<< "Reading user's environment defs from file .ExaConfig.rc" << endl;
		gMrbLog->Flush(this->ClassName(), "Initialize", setblue);
	}

// initialize histogram arrays

// initialize user lists: modules, params & histos
 	gMrbAnalyze->InitializeLists(3, 10);
	gMrbAnalyze->AddModuleToList("adc1", "Silena 4418/V ADC 8 x 4096", kMrbModuleAdc1, 0, 6);
	gMrbAnalyze->AddModuleToList("adc2", "Silena 4418/V ADC 8 x 4096", kMrbModuleAdc2, 6, 4);

// instantiate user's event classes and book histograms
	if (gReadout == NULL) {		// event readout, trigger 1
		gReadout = new TUsrEvtReadout();
 		Bool_t err = gReadout->IsZombie();
 		if (!err) {
			err |= !gReadout->BookParams();
			err |= !gReadout->BookHistograms();
			err |= !gReadout->AllocHitBuffers();
 		}
 		if (err) {
 			gMrbLog->Err()	<< "Something went wrong while creating event readout" << endl;
 			gMrbLog->Flush(this->ClassName(), "Initialize");
 		}
	}

// instantiate windows
	if (w1 == NULL) w1 = new TMrbWindowI("w1", 0, 1023);
// instantiate windows
	if (w3 == NULL) w3 = new TMrbWindowI("w3", 0, 1023);
// instantiate windows
	if (w2 == NULL) w2 = new TMrbWindowI("w2", 0, 1023);



	return(kTRUE);
}

Bool_t TMrbAnalyze::HandleUserMessage(const Char_t * UserArgs){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::HandleUserMessage
// Purpose:        Execute a user message
// Arguments:      Char_t * UserArgs
// Results:        kTRUE/kFALSE
// Description:    Called by message handler.
//                 Format of argument string depends on user's definitions
//                 and has to be decoded according to his requirements.
// Exceptions:     
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts = kFALSE;


	return(sts);
};

Bool_t TMrbAnalyze::ReloadParams(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReloadParams
// Purpose:        Reload parameters from file
// Arguments:      TMrbIOSpec * IOSpec   -- i/o specifications
// Results:        kTRUE/kFALSE
// Description:    Reloads params (vars, wdws, functions etc.) from root file
// Exceptions:     
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


	return(this->ReloadVarsAndWdws(IOSpec));
};

Bool_t TMrbAnalyze::WriteRootTree(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::WriteRootTree
// Purpose:        Open a file to write raw data to tree
// Arguments:      TMrbIOSpec * IOSpec   -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens a ROOT file and creates trees.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString rootFile;

	if ((IOSpec->GetOutputMode() & TMrbIOSpec::kOutputOpen) == 0) return(kTRUE);

	if (fWriteRootTree) {
		gMrbLog->Err()	<< "ROOT file already open - " << fRootFileOut->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRootTree");
		return(kFALSE);
	}

	rootFile = IOSpec->GetOutputFile();

	if (verboseMode) {
		gMrbLog->Out()	<< "Opening ROOT file \"" << rootFile << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "WriteRootTree");
	}

	fRootFileOut = new TFile(rootFile.Data(), "RECREATE", "Example configuration");
	fRootFileOut->SetCompressionLevel(1);
	fWriteRootTree = kTRUE;

// create ROOT trees for each event
	gReadout->CreateTree();

// create trees for start/stop info
	gStartEvent->CreateTree();
	gStopEvent->CreateTree();

// create tree to hold dead-time data
	gDeadTime->CreateTree();

	return(kTRUE);
}

Bool_t TMrbAnalyze::OpenRootFile(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::OpenRootFile
// Purpose:        Open a file to replay tree data
// Arguments:      TMrbIOSpec * IOSpec    -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Opens a ROOT file for input.
//                 Replay mode only.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TString rootFile;

	if ((IOSpec->GetInputMode() & TMrbIOSpec::kInputRoot) == 0) return(kTRUE);

	if (fRootFileIn != NULL) {
		gMrbLog->Err()	<< "ROOT file already open - " << fRootFileIn->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "OpenRootFile");
		return(kFALSE);
	}

	rootFile = IOSpec->GetInputFile();

	if (rootFile.CompareTo("fake.root") == 0) {
		this->SetFakeMode();
// set fake mode for all trees
		gReadout->SetFakeMode();
		if (verboseMode) {
			gMrbLog->Out()	<< "Running in FAKE mode" << endl;
			gMrbLog->Flush(this->ClassName(), "OpenRootFile");
		}
	} else {
		gSystem->ExpandPathName(rootFile);
		if(gSystem->AccessPathName(rootFile, kFileExists)){
			gMrbLog->Err()	<< "No such file - " << rootFile << endl;
			gMrbLog->Flush(this->ClassName(), "OpenRootFile");
			return(kFALSE);
		} else {
			if (verboseMode) {
				gMrbLog->Out()	<< "Opening ROOT file \"" << rootFile << "\"" << endl;
				gMrbLog->Flush(this->ClassName(), "OpenRootFile");
			}
		}
		fRootFileIn = new TFile(rootFile.Data(), "READ");

// initialize ROOT trees
		gReadout->InitializeTree(fRootFileIn);

// initialize trees containing start/stop information
		gStartEvent->InitializeTree(fRootFileIn);
		gStopEvent->InitializeTree(fRootFileIn);

// initialize dead-time tree
		gDeadTime->InitializeTree(fRootFileIn);

// turn on replay mode
		fReplayMode = kTRUE;
		gReadout->SetReplayMode(kTRUE);
	}
	return(kTRUE);
}

Bool_t TMrbAnalyze::ReplayEvents(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::ReplayEvents
// Purpose:        Read data from ROOT file
// Arguments:      TMrbIOSpec * IOSpec  -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Replays event data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Bool_t sts;

	if (!this->IsFakeMode()) {
		if (fRootFileIn == NULL) {
			gMrbLog->Err()	<< "No ROOT file open" << endl;
			gMrbLog->Flush(this->ClassName(), "ReplayEvents");
			return(kFALSE);
		}

// output start/stop time stamps
		this->PrintStartStop();
	}

// replay tree data for each event with replay mode turned-on
	sts = kTRUE;
	if (gReadout->IsReplayMode()) {
		if (!gReadout->ReplayTreeData(IOSpec)) sts = kFALSE;
	}

	return(sts);
}

void TMrbAnalyze::SetScaleDown(Int_t ScaleDown) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbAnalyze::SetScaleDown
// Purpose:        Set a scale-down value common to all events
// Arguments:      Int_t ScaleDown    -- scale-down factor
// Results:        
// Exceptions:     
// Description:    Defines a global scale-down factor valid for all events.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fScaleDown = ScaleDown;

// set scale down individually for each event
	gReadout->SetScaleDown(ScaleDown);
}


Bool_t TUsrSevtData::FillSubevent(const UShort_t * SevtData, Int_t SevtWC, Int_t TimeStamp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::FillSubevent
// Purpose:        Fill local instance of subevent data with MBS data
// Arguments:      const UShort_t * SevtData   -- pointer to unpacked subevent data
//                 Int_t SevtWC                -- word count
//                 Int_t TimeStamp             -- time since last start
//                                                in units of 100 microsecs
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Assigns MBS data to corresponding data members
//                 of subevent data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t i, j;

	if (SevtWC == 0) return(kTRUE);

	if (SevtWC != 10) {	// test word count
		cerr	<< setred << this->ClassName() << "::FillSubevent(): data (Serial=1): Wrong ber of data words - "
				<< SevtWC << " (should be 10)"
				<< setblack << endl;
	}
	fTimeStamp = TimeStamp; 				// store time since last "start acquisition"
	this->SetTimeStamp(TimeStamp);			// passed as argument by parent event

// pass MBS data to subevent
// start of module adc1, Silena 4418/V ADC 8 x 4096, Serial=1
	etot = (Int_t) *SevtData++;	// data.etot
	de1 = (Int_t) *SevtData++;	// data.de1
	de2 = (Int_t) *SevtData++;	// data.de2
	de3 = (Int_t) *SevtData++;	// data.de3
	de4 = (Int_t) *SevtData++;	// data.de4
	veto = (Int_t) *SevtData++;	// data.veto
// start of module adc2, Silena 4418/V ADC 8 x 4096, Serial=2
	pos1r = (Int_t) *SevtData++;	// data.pos1r
	pos1l = (Int_t) *SevtData++;	// data.pos1l
	pos2r = (Int_t) *SevtData++;	// data.pos2r
	pos2l = (Int_t) *SevtData++;	// data.pos2l

	return(kTRUE);
}

Bool_t TUsrSevtData::BookParams() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name: 		  TUsrSevtData::BookParams
// Purpose:  	  Fill param list for subevent data
// Arguments:	  
// Results:  	  kTRUE/kFALSE
// Exceptions:	  
// Description:    Generates entries in the list of params.
// Keywords: 	  
//////////////////////////////////////////////////////////////////////////////

 	register Int_t i;
 	TMrbString pName;

 	if (gMrbAnalyze->FindParam("etot") == NULL) gMrbAnalyze->AddParamToList("etot", &etot, kMrbModuleAdc1, 0);
 	if (gMrbAnalyze->FindParam("de1") == NULL) gMrbAnalyze->AddParamToList("de1", &de1, kMrbModuleAdc1, 1);
 	if (gMrbAnalyze->FindParam("de2") == NULL) gMrbAnalyze->AddParamToList("de2", &de2, kMrbModuleAdc1, 2);
 	if (gMrbAnalyze->FindParam("de3") == NULL) gMrbAnalyze->AddParamToList("de3", &de3, kMrbModuleAdc1, 3);
 	if (gMrbAnalyze->FindParam("de4") == NULL) gMrbAnalyze->AddParamToList("de4", &de4, kMrbModuleAdc1, 4);
 	if (gMrbAnalyze->FindParam("veto") == NULL) gMrbAnalyze->AddParamToList("veto", &veto, kMrbModuleAdc1, 5);
 	if (gMrbAnalyze->FindParam("pos1r") == NULL) gMrbAnalyze->AddParamToList("pos1r", &pos1r, kMrbModuleAdc2, 0);
 	if (gMrbAnalyze->FindParam("pos1l") == NULL) gMrbAnalyze->AddParamToList("pos1l", &pos1l, kMrbModuleAdc2, 1);
 	if (gMrbAnalyze->FindParam("pos2r") == NULL) gMrbAnalyze->AddParamToList("pos2r", &pos2r, kMrbModuleAdc2, 2);
 	if (gMrbAnalyze->FindParam("pos2l") == NULL) gMrbAnalyze->AddParamToList("pos2l", &pos2l, kMrbModuleAdc2, 3);

	return(kTRUE);
}

Bool_t TUsrSevtData::BookHistograms() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::BookHistograms
// Purpose:        Define histograms for subevent data
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Instantiates histograms for subevent data.
//                 Each histogram will be booked only once.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register Int_t i;
	TMrbString hName, hTitle;
	TString paramName;

	if (hEtot == NULL) {
 			hEtot = new TH1F("hEtot", "data.etot", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hEtot, kMrbModuleAdc1, 0);
 	}
	if (hDe1 == NULL) {
 			hDe1 = new TH1F("hDe1", "data.de1", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hDe1, kMrbModuleAdc1, 1);
 	}
	if (hDe2 == NULL) {
 			hDe2 = new TH1F("hDe2", "data.de2", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hDe2, kMrbModuleAdc1, 2);
 	}
	if (hDe3 == NULL) {
 			hDe3 = new TH1F("hDe3", "data.de3", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hDe3, kMrbModuleAdc1, 3);
 	}
	if (hDe4 == NULL) {
 			hDe4 = new TH1F("hDe4", "data.de4", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hDe4, kMrbModuleAdc1, 4);
 	}
	if (hVeto == NULL) {
 			hVeto = new TH1F("hVeto", "data.veto", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hVeto, kMrbModuleAdc1, 5);
 	}
	if (hPos1r == NULL) {
 			hPos1r = new TH1F("hPos1r", "data.pos1r", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hPos1r, kMrbModuleAdc2, 0);
 	}
	if (hPos1l == NULL) {
 			hPos1l = new TH1F("hPos1l", "data.pos1l", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hPos1l, kMrbModuleAdc2, 1);
 	}
	if (hPos2r == NULL) {
 			hPos2r = new TH1F("hPos2r", "data.pos2r", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hPos2r, kMrbModuleAdc2, 2);
 	}
	if (hPos2l == NULL) {
 			hPos2l = new TH1F("hPos2l", "data.pos2l", 1024, 0.0, 4096.0);
 			gMrbAnalyze->AddHistoToList(hPos2l, kMrbModuleAdc2, 3);
 	}

	return(kTRUE);
}

Bool_t TUsrSevtData::FillHistograms() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::FillHistograms
// Purpose:        Accumulate histograms for subevent data
// Arguments:      
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Performs add-one to histograms.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register Int_t i;

// add-one to histograms
	hEtot->Fill((Axis_t) etot);
	hDe1->Fill((Axis_t) de1);
	hDe2->Fill((Axis_t) de2);
	hDe3->Fill((Axis_t) de3);
	hDe4->Fill((Axis_t) de4);
	hVeto->Fill((Axis_t) veto);
	hPos1r->Fill((Axis_t) pos1r);
	hPos1l->Fill((Axis_t) pos1l);
	hPos2r->Fill((Axis_t) pos2r);
	hPos2l->Fill((Axis_t) pos2l);

	gMrbAnalyze->SetUpdateFlag(); 		// activate update handler
	return(kTRUE);
}

Bool_t TUsrSevtData::AddBranch(TTree * Tree) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::AddBranch
// Purpose:        Create a branch for subevent data
// Arguments:      TTree * Tree      -- pointer to parent's tree struct
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Calls method TTree::Branch for this subevent.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t bufSize;
	Int_t splitMode;

	if (gMrbAnalyze->TreeToBeWritten()) {
		bufSize = gMrbAnalyze->GetBranchSize();
		splitMode = gMrbAnalyze->GetSplitLevel();
		if (verboseMode) {
			cout	<< this->ClassName() << "::AddBranch(): adding branch \"data\" to tree \""
					<< Tree->GetName() << "\" (bufsiz="
					<< bufSize
					<< ", split=" << splitMode << ")"
					<< endl;
		}
		fMyAddr = (TObject *) this; 		// we need a '**' ref to this object
		Tree->Branch( 	"data", this->ClassName(),
						&fMyAddr,
						bufSize,
						splitMode);
	}
	return(kTRUE);
}

Bool_t TUsrSevtData::InitializeBranch(TTree * Tree) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::InitializeBranch
// Purpose:        Initialize branch addresses for subevent data
// Arguments:      TTree * Tree      -- pointer to parent's tree struct
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Set branch address for subevent data.
//                 Subevent will be fetched from tree AS A WHOLE.
//                 >> Replay mode only <<
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fBranch = Tree->GetBranch("data");
	fMyAddr = this;
	fBranch->SetAddress(&fMyAddr);

	return(kTRUE);
}

void TUsrSevtData::Reset(Int_t InitValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtData::Reset
// Purpose:        Reset subevent data
// Arguments:      Int_t InitValue   -- initialization value
// Results:        --
// Exceptions:     
// Description:    Initializes subevent data with given value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	etot = InitValue;	// data.etot
	de1 = InitValue;	// data.de1
	de2 = InitValue;	// data.de2
	de3 = InitValue;	// data.de3
	de4 = InitValue;	// data.de4
	veto = InitValue;	// data.veto
	pos1r = InitValue;	// data.pos1r
	pos1l = InitValue;	// data.pos1l
	pos2r = InitValue;	// data.pos2r
	pos2l = InitValue;	// data.pos2l
}


Bool_t TUsrEvtReadout::BookParams() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::BookParams
// Purpose:        Setup param lists for event readout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Fills param tables for all subevents in event readout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	data.BookParams();


	return(kTRUE);
}

Bool_t TUsrEvtReadout::BookHistograms() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::BookHistograms
// Purpose:        Book histograms for event readout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Books histograms for all subevents in event readout.
//                 If this is a multi-trigger event derived from single-bit
//                 triggers booking is already done there.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	data.BookHistograms();

		
	return(kTRUE);
}

Bool_t TUsrEvtReadout::AllocHitBuffers() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::AllocHitBuffers
// Purpose:        Allocate hit buffer for event readout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Allocates hit buffers for subevents to store subevent data
//                 (class TUsrHitBuffer / TClonesArray) 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TUsrHitBuffer * eventTimeBuffer = NULL;

	Int_t nofEntriesE, highWaterE, nofEntriesS, highWaterS;
	UInt_t offsetE, offsetS;

 	nofEntriesE = gEnv->GetValue(gMrbAnalyze->GetResource("Event.Readout.HitBuffer.NofEntries"), 1000);
 	highWaterE = gEnv->GetValue(gMrbAnalyze->GetResource("Event.Readout.HitBuffer.HighWater"), 0);
 	offsetE = (UInt_t) gEnv->GetValue(gMrbAnalyze->GetResource("Event.Readout.HitBuffer.Offset"), (Int_t) 0);


	return(kTRUE);
}

Bool_t TUsrEvtReadout::CreateTree() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::CreateTree
// Purpose:        Create a tree for event readout
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Creates a tree for event readout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t autoSave;
	
	if (gMrbAnalyze->TreeToBeWritten()) {
		if (verboseMode) {
			cout	<< this->ClassName() << "::CreateTree(): Creating tree \"readout\""
					<< endl;
		}
		fTreeOut = new TTree("readout",
								"readout of camac data");
		autoSave = gEnv->GetValue("TUsrEvtReadout.AutoSave",
								10000000);
				
		fTreeOut->SetAutoSave(autoSave);

		data.AddBranch(fTreeOut);		// branch for subevent data
	}
	return(kTRUE);
}

Bool_t TUsrEvtReadout::InitializeTree(TFile * RootFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::InitializeTree
// Purpose:        Initialize tree for event readout
// Arguments:      TFile * RootFile  -- file containing tree data
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Initializes tree and branches for event readout.
//                 Replay mode only.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	register TObject * obj;

	obj = RootFile->Get("readout");
	if (obj == NULL) {
		gMrbLog->Err()	<< "Tree \"readout\" not found in ROOT file " << RootFile->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	if (obj->IsA() != TTree::Class()) {
		gMrbLog->Err()	<< "Not a TTree object - " << obj->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "InitializeTree");
		return(kFALSE);
	}
	fTreeIn = (TTree *) obj;
	fReplayMode = kTRUE;

// initialize branches and assign addresses
	data.InitializeBranch(fTreeIn);

	return(kTRUE);
}

Bool_t TUsrEvtReadout::ReplayTreeData(TMrbIOSpec * IOSpec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::ReplayTreeData
// Purpose:        Replay tree data for event readout
// Arguments:      TMrbIOSpec * IOSpec  -- i/o specifications
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Reads in tree data for event readout.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Int_t nofEntries, nofEvents, nofBytes, nbytes;
	Int_t startEvent, stopEvent;
	TString startTime, stopTime;
	register Int_t i;
	TMrbAnalyze::EMrbRunStatus rsts;
	Int_t timeStamp;

	if (!this->IsFakeMode() && fTreeIn == NULL) return(kFALSE);

	nofBytes = 0;
	nofEvents = 0;

	if (fReplayMode) {
		if (!this->IsFakeMode()) nofEntries = (Int_t) fTreeIn->GetEntries();
		startEvent = IOSpec->GetStartEvent();
		stopEvent = IOSpec->GetStopEvent();

		if (IOSpec->IsTimeStampMode()) {
			IOSpec->ConvertToTimeStamp(startTime, startEvent);
			IOSpec->ConvertToTimeStamp(stopTime, stopEvent);

			if (startEvent > stopEvent) {
				gMrbLog->Err()	<< "Timestamps out of order - start at "
								<< startTime << " ... stop at " << stopTime << endl;
				gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				return(kFALSE);
			}

			if (verboseMode) {
				if (this->IsFakeMode()) {
					gMrbLog->Out()	<< "Replaying tree in FAKE mode" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				} else {
					gMrbLog->Out()	<< "Replaying tree " << fTreeIn->GetName() << " -" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
					gMrbLog->Out()	<< "Events with time stamps " << startTime << " ... " << stopTime << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				}
			}

			for (i = 0; i < nofEntries; i++) {
				if (!gMrbAnalyze->TestRunStatus()) return(kFALSE);
				pthread_mutex_lock(&global_data_mutex);
				if (!this->IsFakeMode()) nbytes = fTreeIn->GetEvent(i);
				timeStamp = data.fTimeStamp;
				if (timeStamp >= startEvent && timeStamp <= stopEvent) {
					nofEvents++;
					fNofEvents++;
					nofBytes += nbytes;
					gMrbAnalyze->IncrEventCount();
					this->Analyze();
				}
				pthread_mutex_unlock(&global_data_mutex);
				if (timeStamp > stopEvent) break;
			}

			if (verboseMode) {
				if (this->IsFakeMode()) {
					gMrbLog->Out()	<< nofEvents
									<< " entries (timestamps " << startTime << " ... " << stopTime
									<< ") faked" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
				} else {
					gMrbLog->Out()	<< nofEvents
									<< " entries (timestamps " << startTime << " ... " << stopTime
									<< ") read from tree \"" << fTreeIn->GetName() << "\" (" << nofBytes << " bytes)"
									<< endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
				}
			}
		
		} else {
			
			if (startEvent > stopEvent) {
				gMrbLog->Err()	<< "Event numbers out of order - start at "
								<< startEvent << " ... stop at " << stopEvent << endl;
				gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				return(kFALSE);
			}

			if (startEvent > nofEntries) {
				gMrbLog->Err()	<< "StartEvent (" << startEvent
								<< ") beyond end of data (" << stopEvent << ")" << endl;
				gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				return(kFALSE);
			}

			if (stopEvent == 0 || stopEvent >= nofEntries) stopEvent = nofEntries - 1;

			if (verboseMode) {
				if (this->IsFakeMode()) {
					gMrbLog->Out()	<< "Replaying tree in FAKE mode" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData");
				} else {
					gMrbLog->Out()	<< "Replaying tree " << fTreeIn->GetName() << " -" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
					gMrbLog->Out()	<< "Events " << startEvent << " ... " << stopEvent << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
				}
			}

			for (i = startEvent; i <= stopEvent; i++) {
				if (!gMrbAnalyze->TestRunStatus()) return(kFALSE);
				pthread_mutex_lock(&global_data_mutex);
				if (!this->IsFakeMode()) nofBytes += fTreeIn->GetEvent(i);
				nofEvents++;
				fNofEvents++;
				gMrbAnalyze->IncrEventCount();
				this->Analyze();
				pthread_mutex_unlock(&global_data_mutex);
			}

			if (verboseMode) {
				if (this->IsFakeMode()) {
					gMrbLog->Out()	<< nofEvents << " entries (" << startEvent << " ... " << stopEvent
									<< ") faked" << endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
				} else {
					gMrbLog->Out()	<< nofEvents << " entries (" << startEvent << " ... " << stopEvent
									<< ") read from tree \"" << fTreeIn->GetName() << "\" (" << nofBytes << " bytes)"
									<< endl;
					gMrbLog->Flush(this->ClassName(), "ReplayTreeData", setblue);
				}
			}
		}
	}
	return(kTRUE);
}

Bool_t TUsrEvtReadout::FillEvent(const s_vehe * EventData, const MBSDataIO * BaseAddr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::FillEvent
// Purpose:        Fill local event instance with MBS data
// Arguments:      const s_vehe * EventData   -- pointer to MBS event structure
//                 const MBSDataIO * BaseAddr -- pointer to MBS data base
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Decodes the event header,
//                 then loops over subevents
//                 and calls sevt-specific fill methods
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	UInt_t SevtType;
	const UShort_t * SevtData;
	const ULong_t * ClockData;
	const ULong_t * DeadTimeData;
	Int_t timeStamp;
	Int_t dtTotalEvents, dtAccuEvents, dtScalerContents;

	fEventNumber = (ULong_t) EventData->l_count;
	fNofEvents++;
	gMrbAnalyze->IncrEventCount();

	fClockRes = 0;									// reset time stamp
	fClockSecs = 0;
	fClockNsecs = 0;

	fSubeventPattern = 0;							// reset sevt pattern
	
	this->Reset();									// clear data section

	for (;;) {
		SevtType = NextSubeventHeader(BaseAddr);			// get next subevent
		if (SevtType == MBS_STYPE_EOE) return(kTRUE);		// end of event
		if (SevtType == MBS_STYPE_ABORT) return(kFALSE);	// error


		switch (BaseAddr->sevt_id) {				// dispatch according to subevent id
			case kMrbSevtData:				// subevent data: camac data, id=$sevtSerial
				if ((SevtData = NextSubeventData(BaseAddr, 10, kFALSE)) == NULL) return(kFALSE);
				if (!data.FillSubevent(SevtData, BaseAddr->sevt_wc, this->GetTimeStamp())) return(kFALSE);
				fSubeventPattern |= kMrbSevtBitData;
				continue;

			case kMrbSevtTimeStamp:					// subevent type [9000,1] (ROOT Id=999): time stamp
				if ((SevtData = NextSubeventData(BaseAddr, 100, kFALSE)) == NULL) return(kFALSE);
				ClockData = (ULong_t *) SevtData;
				fClockRes = *ClockData++;
				fClockSecs = *ClockData++;
				fClockNsecs = *ClockData++;
				timeStamp = this->CalcTimeDiff((TUsrEvent *) gStartEvent);	// calc time since last start
				this->SetTimeStamp(timeStamp);		// mark event with time stamp
				continue;

			case kMrbSevtDeadTime:					// subevent type [9000,2] (ROOT Id=998): dead time data
				if ((SevtData = NextSubeventData(BaseAddr, 100, kFALSE)) == NULL) return(kFALSE);
				DeadTimeData = (ULong_t *) SevtData;
				fClockRes = *DeadTimeData++;
				fClockSecs = *DeadTimeData++;
				fClockNsecs = *DeadTimeData++;
				timeStamp = this->CalcTimeDiff((TUsrEvent *) gStartEvent);	// calc time since last start
				this->SetTimeStamp(timeStamp);		// mark event with time stamp
				dtTotalEvents = *DeadTimeData++;
				dtAccuEvents = *DeadTimeData++;
				dtScalerContents = *DeadTimeData++;
				gDeadTime->Set(timeStamp, dtTotalEvents, dtAccuEvents, dtScalerContents);
				if (gMrbAnalyze->TreeToBeWritten()) gDeadTime->GetTreeOut()->Fill();
				continue;
				
			case kMrbSevtDummy: 					// dummy subevent created by MBS internally [111,111] (ROOT Id=888), wc = 0
				if ((SevtData = NextSubeventData(BaseAddr, 100, kFALSE)) == NULL) return(kFALSE);
				if (BaseAddr->sevt_wc != 0) {
					gMrbLog->Err()	<< "Dummy MBS subevent [111,111], serial=888 - wc = "
									<< BaseAddr->sevt_wc << " (should be 0)" << endl;
					gMrbLog->Flush(this->ClassName(), "FillEvent");
				}
				continue;

			default: 					// unknown subevent id
				gMrbLog->Err()	<< "Unknown subevent id - " << BaseAddr->sevt_id << endl;
				gMrbLog->Flush(this->ClassName(), "FillEvent");
				continue;
		}
		gMrbLog->Err()	<< "Illegal subevent - type=0x"
						<< setbase(16) << BaseAddr->sevt_otype
						<< setbase(10) << " ["
						<< (BaseAddr->sevt_otype >> 16) << "," << (BaseAddr->sevt_otype & 0xffff)
						<< "], serial=" << BaseAddr->sevt_id << endl;
		gMrbLog->Flush(this->ClassName(), "FillEvent");
	}
	return(kTRUE);
}

Bool_t TUsrEvtReadout::Analyze() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrEvtReadout::Analyze
// Purpose:        Analyze data for event readout
//                 (Trigger 1)
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    User's analysis function for trigger 1
//                 Any sorting, filtering, and analyzing goes here.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

 	Bool_t treeToBeFilled = kTRUE;			// may be changed by user's analysis

	data.FillHistograms();	// fill histograms for subevent data

//++++++++ User's analyzing code goes here +++++++++++++


	if (gMrbAnalyze->TreeToBeWritten() && treeToBeFilled) fTreeOut->Fill();

	return(kTRUE);
}

void TUsrEvtReadout::Reset(Int_t InitValue) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TUsrSevtReadout::Reset
// Purpose:        Reset event data
// Arguments:      Int_t InitValue  -- initialization value
// Results:        --
// Exceptions:     
// Description:    Resets all subevents assigned to this event.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	data.Reset(InitValue);	// reset param data in subevent data
}

Bool_t TMrbTransport::ProcessEvent(s_vehe * EventData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbTransport::ProcessEvent
// Purpose:        Process event data
// Arguments:      s_vehe * EventData  -- pointer to event data (including header)
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    This is the event filling function.
//                 User will be called by method Analyze().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	const MBSDataIO * BaseAddr; 		// base addr of MBS data
	UInt_t sts;

	if (!gMrbAnalyze->TestRunStatus()) return(kFALSE);		// run is stopped (maybe after pausing)

	BaseAddr = GetMbsBase();			// fetch base addr

	switch (EventData->i_trigger) {		// dispatch according to trigger number
		case kMrbTriggerReadout:		// event readout, trigger 1
			pthread_mutex_lock(&global_data_mutex);
			gReadout->FillEvent(EventData, BaseAddr);
			sts = gReadout->Analyze();
			pthread_mutex_unlock(&global_data_mutex);
			return(sts);


		case kMrbTriggerStartAcquisition:					// start acquisition (trigger 14)
			gStartEvent->ExtractTimeStamp(EventData, BaseAddr);
			gStartEvent->Print();
			gMrbAnalyze->SetRunStatus(TMrbAnalyze::M_STARTING);
			return(kTRUE);

		case kMrbTriggerStopAcquisition:					// stop acquisition (trigger 15)
			gStopEvent->ExtractTimeStamp(EventData, BaseAddr);
			gStopEvent->Print();
			gMrbAnalyze->SetRunStatus(TMrbAnalyze::M_PAUSING);
			return(kFALSE);

		default:
			gMrbLog->Err()	<< "Illegal trigger number - #" << EventData->i_trigger << endl;
			gMrbLog->Flush(this->ClassName(), "ProcessEvent");
			return(kFALSE);
	}
	return(kTRUE);
}					

