//__________________________________________________________________[C++ MAIN]
//////////////////////////////////////////////////////////////////////////////
// Name:           M_analyze
// Purpose:        Analyze MARaBOU data
// Arguments:      (see below)
// Results:        
// Exceptions:     
// Description:    Connects to server or file and inputs event data,
//                 then goes thru user's analyzing function.
//                 Data are expected to be in standard MBS format.
// Author:         R. Lutter, O.Schaile
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>,<a href=mailto:otto.schaile@physik.uni-muenchen.de>O. Schaile</a>
// Date:           Feb-1999 
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <unistd.h>
#include <iostream.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>

#include "TROOT.h"
#include "TRint.h"
#include "TOrdCollection.h"
#include "TCutG.h"
#include "TClass.h"
#include "TSystem.h"
#include "TUnixSystem.h"
#include "TString.h"
#include "TStringLong.h"
#include "TApplication.h"
#include "TMapFile.h"
#include "TError.h"
#include "TEnv.h"
#include "TRegexp.h"
#include "TSysEvtHandler.h"

#include "TDirectory.h"
#include "TSocket.h"
#include "TServerSocket.h"
#include "TMonitor.h"
#include "TMessage.h"

#include "TMrbTransport.h"
#include "TMrbAnalyze.h"
#include "TMrbStatistics.h"
#include <pthread.h>

#include "SetColor.h"

// global pthread mutex to protect TMapped data
extern pthread_mutex_t global_data_mutex;

/////////////////////////////////////////////

// Globals
TMrbAnalyze * u_analyze;
TMrbTransport * gMrbTransport;
TMrbIOSpec * ioSpec;

TServerSocket *ss = 0;

// if Offline (replay) dont use TMapFile
Bool_t kUseMap = kTRUE;
Int_t  gComSocket;
Bool_t verboseMode = kFALSE;
UInt_t input_mode = TMrbIOSpec::kInputRoot;

static unsigned int update_time = TMrbAnalyze::kUpdateTime;

TString our_pid_file = "/tmp/M_analyze.";

UInt_t Updates; 	// DUMMY to keep M_subs.cxx happy ...

// global mapped file pointer
TMapFile * M_prod = 0;

TMrbStatistics * gStat;
// global PTHREAD handles
pthread_t msg_thread, update_thread;

// pthread prototypes
void * msg_handler(void *);               // pthreaded message handler routine
void * update_handler(void *);            // pthreaded timed mapped file update routine
//__________________________________________________________________________________________

void PlayQuart(Int_t n){
   const char bell[] = "\007";
   Int_t a = 400;
   Int_t loadness = 100;
   Int_t duration = 200;
   ostrstream cmd1;
   ostrstream cmd2;
   Int_t quart = (Int_t) (a * pow(pow(2,1./12.), 4));
   cmd1 << "xset b " << loadness << " " << a     << " " << duration << ends;
   cmd2 << "xset b " << loadness << " " << quart << " " << duration << ends;
   for(Int_t i = 0; i< n; i++){
//      gSystem->Exec("xset b 100 400 200");
      gSystem->Exec(cmd1.str());
      cout << bell << endl;
      gSystem->Sleep(400);   
//      gSystem->Exec("xset b 100 504 300");
      gSystem->Exec(cmd2.str());
      cout << bell << endl;
      gSystem->Sleep(500);
   }
   gSystem->Exec(cmd1.str());
   cout << bell << endl;
   cmd1.rdbuf()->freeze(0);
   cmd2.rdbuf()->freeze(0);
}


Bool_t PutPid(TMrbAnalyze::EMrbRunStatus Status) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           PutPid
// Purpose:        Store process id and status on disk
// Arguments:      EMrbRunStatus Status    -- status flag
// Results:        
// Exceptions:     
// Description:    Creates a lock file on disk saving proc id and status
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

	pthread_mutex_lock(&global_data_mutex);
	ofstream wstream;
	if ( Status == TMrbAnalyze::M_STARTING ) {
		if 	( !gSystem->AccessPathName(our_pid_file.Data(), kFileExists) ) {
			cerr	<< setred
					<< "M_analyze: Warning - " << our_pid_file << " exists "
					<< endl;
			cerr	<< "           (last M_analyze probably didnt close cleanly)"
					<< setblack << endl;
			TString RmCmd = "rm  ";
			RmCmd += our_pid_file;
			gSystem->Exec((const char *)RmCmd);
		} 
	} 

//  enter out Id into /var/tmp to tell the world we are running
	wstream.open(our_pid_file, ios::out);
	if (!wstream.good()) {
		cerr	<< setred
				<< "M_analyze: "
				<< gSystem->GetError() << " - " << our_pid_file
				<< setblack << endl;
		pthread_mutex_unlock(&global_data_mutex);
		return(kFALSE);
	} else {
//		if ( verboseMode ) {
			Int_t pid = gSystem->GetPid();
			cout	<< "M_analyze: Writing " 
               << pid << " " << Status << " " << gComSocket  
               << " into " << our_pid_file  << endl;
//		}
		wstream << gSystem->GetPid() 
              << " " << Status << " " << gComSocket << endl;
		wstream.close();
	}
	pthread_mutex_unlock(&global_data_mutex);
	return(kTRUE);
}
   

//extern void InitGui();
//VoidFuncPtr_t initfuncs[] = { InitGui, 0 };

//TROOT root("M_analyze","Marabous Analysis framework", initfuncs);
//TROOT root("M_analyze","Marabous Analysis froutpututput");


int main(int argc, char **argv) {
//______________________________________________________________[MAIN PROGRAM]
//////////////////////////////////////////////////////////////////////////////
// Name:           M_analyze
// Purpose:        Analyze MARaBOU data
//////////////////////////////////////////////////////////////////////////////

	TMrbIOSpec::EMrbOutputFormat oFormat = TMrbIOSpec::kFormatNone;

	void exit_from_analyze(int);

	if(argc <= 2) {
		cerr	<< setred
				<<	endl
				<<	"Usage: M_analyze data_source [input_type]" << endl
				<<	"                             [start_event] [stop_event]" << endl
				<<	"                             [run_number] [down_scale]" << endl
				<<	"                             [output_file] [file_size]" << endl
				<<	"                             [parm_file] [histo_file]" << endl
				<<	"                             [mmap_file] [mmap_size]" << endl
				<< endl
				<<	"       ARGUMENT     DEFAULT            MEANING" << endl
				<<	"       data_source    n/a              server addr or file name" << endl
				<<	"       input_type     \"FILE\"             input type:" << endl
				<<	"                                       FILE   read data from ROOT tree (ext = .root)" << endl
				<<	"                                              or from MBS native file (ext = .lmd)" << endl
				<<	"                                              or from MBS event data file (ext = .med)" << endl
				<<	"                                       SYNC   connect to server SYNChronously" << endl
				<<	"                                       ASYNC  connect to server ASYNChronously" << endl
				<<	"                                       LIST   read from file list" << endl
				<<	"       start_event    0                event number / time stamp to start with" << endl
				<<	"       stop_event     0                event number / time stamp to end with" << endl
				<<	"                                       (time stamps are given as :nnn or hh:mm:ss[:xxx])" << endl
				<<	"       run_number     0                run number (-1)" << endl
				<<	"       down_scale     1                analysis will be scaled down by this factor" << endl
				<<	"       output_file    \"none\"             where to write converted MBS raw data" << endl
				<<	"       file_size      0                approx. size of output file in MB" << endl
				<<	"       parm_file      \"none\"             name of parameter file (.root / .par) to be loaded at startup" << endl
				<<	"       histo_file     \"Histos_...\"       name / prefix if histogram file" << endl
				<<	"       mmap_file      \"/tmp/M_prod.map\"  name of mmap file" << endl
				<<	"       mmap_size      12               size of file in MBs" << endl
				<<	"       gComSocket     0                socket for communication with C_analyze" << endl
				<< setblack <<	endl << endl;
		exit(0);
	}

//   for(int i=0; i < argc; i++)
//      cout << "arg " << i << ": " << argv[i] << endl;

	verboseMode = (Bool_t) gEnv->GetValue("TMrbAnalyze.VerboseMode", kFALSE);

// install signal handler
	signal(SIGINT, exit_from_analyze);

// pass unix arguments
	Int_t argNo = 0;
// data source
	argNo++;
	TString data_source;
	data_source = argv[argNo];
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Data source: " << argv[argNo] << endl;

// input type: FILE, SYNC, ASYNC, LIST
	argNo++;
	TString input_type;
	TMrbIOSpec::EMrbInputMode input_mode;
	if(argc > argNo)	input_type = argv[argNo];
	else				input_type = "F";
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Input type:  " <<  input_type<< endl;

	TRegexp rxroot("\\.root$");
	TRegexp rxmed("\\.med$");
	TRegexp rxlmd("\\.lmd$");

	switch (input_type(0)) {
		case 'f':
		case 'F':	input_type = "FILE";
					input_mode = TMrbIOSpec::kInputFile;
					if (data_source.Index(rxroot, 0) > 0) {
						input_mode = TMrbIOSpec::kInputRoot;
					} else if (data_source.Index(rxmed, 0) > 0) {
						input_mode = TMrbIOSpec::kInputMED;
					} else if (data_source.Index(rxlmd, 0) > 0) {
						input_mode = TMrbIOSpec::kInputLMD;
					} else {
						cerr	<< setred
								<< "M_analyze: Illegal file extension - " << data_source
								<< " (sould be \".root\", \".med\", or \".lmd\")"
								<< setblack << endl;
						exit(1);
					}
					if (gSystem->AccessPathName(data_source.Data(), kFileExists)) {
						cerr	<< setred
								<< "M_analyze: No such file - " << data_source
								<< setblack << endl;
						exit(1);
					}
					break;
		case 'l':
		case 'L':	input_type = "LIST";
					input_mode = TMrbIOSpec::kInputList;
					if (data_source.Index(".list", 0) > 0) {
						input_mode = TMrbIOSpec::kInputList;
					} else {
						cerr	<< setred
								<< "M_analyze: Illegal file extension - " << data_source
								<< " (sould be .list)"
								<< setblack << endl;
						exit(1);
					}
					if (gSystem->AccessPathName(data_source.Data(), kFileExists)) {
						cerr	<< setred
								<< "M_analyze: No such file - " << data_source
								<< setblack << endl;
						exit(1);
					}
					break;
		case 's':
		case 'S':	input_type = "SYNC";
					input_mode = TMrbIOSpec::kInputSync;
					break;
		case 'a':
		case 'A':	input_type = "ASYNC";
					input_mode = TMrbIOSpec::kInputAsync;
					break;
		default:	cerr	<< setred
							<< "M_analyze: Illegal input type - " << input_type
							<< setblack << endl;
					exit(1);
	}

	if ((input_mode & TMrbIOSpec::kInputTCP) == TMrbIOSpec::kInputTCP) {
		TInetAddress * ia = new TInetAddress(gSystem->GetHostByName(data_source));
		TString hAddr = ia->GetHostName();
		if (hAddr.CompareTo("UnknownHost") == 0) {
			cerr	<< setred
					<< "M_analyze: No such host - " << data_source
					<< setblack << endl;
			exit(1);
		}
	}

// start_event, stop_event
	TString start_event, stop_event;
	argNo++;
	if(argc > argNo)	start_event = argv[argNo];
	else				start_event = "0";
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Start event: " << start_event << endl;
	argNo++;
	if(argc > argNo)	stop_event = argv[argNo];
	else				stop_event = "0";
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Stop event:  " << stop_event << endl;

// run number
	Int_t runnr;
	argNo++;
	if(argc > argNo)	runnr = atoi(argv[argNo]);
	else				runnr = 0;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Run number:  " << runnr << endl;

// down scale
	Int_t downscale;
	argNo++;
	if(argc > argNo)	downscale = atoi(argv[argNo]);
	else				downscale = 1;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Downscale:   " << downscale << endl;
  
// output file & size
	TString output_file;
	TMrbIOSpec::EMrbOutputMode output_mode;
	argNo++;
	if(argc > argNo)	output_file = argv[argNo];
	else				output_file = "none";
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Output file: " << output_file << endl;

	if (output_file.CompareTo("none") == 0)	output_mode = TMrbIOSpec::kOutputNone;
	else {
		output_mode = (TMrbIOSpec::EMrbOutputMode) (TMrbIOSpec::kOutputOpen | TMrbIOSpec::kOutputWriteTree | TMrbIOSpec::kOutputClose);
		oFormat = TMrbIOSpec::kFormatNone;
		if (output_file.Index(rxroot, 0) > 0) oFormat = TMrbIOSpec::kFormatRoot;
		else if (output_file.Index(rxmed, 0) > 0) oFormat = TMrbIOSpec::kFormatMED;
		else if (output_file.Index(rxlmd, 0) > 0) oFormat = TMrbIOSpec::kFormatLMD;
		else {
			cerr << setred << "M_analyze: Wrong output format - " << output_file << setblack << endl;
			exit(1);
		}
	}

	Int_t file_size;
	argNo++;
	if(argc > argNo)	file_size = atoi(argv[argNo]);
	else				file_size = 0;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] File size:   " << file_size << endl;

// param file
	TString param_file;
	TMrbIOSpec::EMrbParamMode param_mode;
	argNo++;
	if(argc > argNo)	param_file = argv[argNo];
	else				param_file = "none";
	if (param_file.CompareTo("none") == 0)			param_mode = TMrbIOSpec::kParamNone;
	else if (param_file.Index(".root", 0) > 0)		param_mode = TMrbIOSpec::kParamReload;
	else if (param_file.Index(".par", 0) > 0)		param_mode = TMrbIOSpec::kParamReloadAscii;
	else {
		cerr	<< setred
				<< "M_analyze: Illegal file extension - " << param_file
				<< " (should be .root (standard ROOT) or .par (ASCII))"
				<< setblack << endl;
		exit(1);
	}
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Param file:  " << param_file << endl;

// histo file
	TString histo_file;
	TMrbIOSpec::EMrbHistoMode histo_mode;
	argNo++;
	if(argc > argNo)	histo_file = argv[argNo];
	else				histo_file = "none";
	if (histo_file.CompareTo("none") == 0)	histo_mode = TMrbIOSpec::kHistoNone;
	else									histo_mode = TMrbIOSpec::kHistoSave;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Histo file:  " << histo_file << endl;
  
// mmap file & size
	TString mmap_name;
	argNo++;
	if(argc > argNo)	mmap_name = argv[argNo];
	else				mmap_name = "/tmp/M_prod.map";
   if (mmap_name.CompareTo("none") == 0) kUseMap = kFALSE;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] TMap name:   " << mmap_name << endl;
	Int_t mmap_size;
	argNo++;
	if(argc > argNo)	mmap_size = kMB * atoi(argv[argNo]);
	else				mmap_size = kMB * 12;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] TMap size:   " << mmap_size << endl;

	gComSocket = 0;
	argNo++;
	if(argc > argNo)	gComSocket = atoi(argv[argNo]);
   our_pid_file += gComSocket;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Communication socket:   " <<  gComSocket<< endl;

   if (verboseMode) cout << "M_analyze: Reading of arguments done (" << argNo << " args)" << endl;

	PutPid(TMrbAnalyze::M_STARTING);

   if ( gComSocket > 0 ) ss = new TServerSocket(gComSocket, kTRUE);

   TString  * new_name = NULL;
   TFile * save_map = NULL;
   const char * hname = NULL;
   if(kUseMap){
		if( !(gSystem->AccessPathName((const Text_t *)mmap_name, kFileExists)) ) {
	//   if mapped file exists,  save histos to temp file for later restore
      	new_name = new TString(mmap_name);
      	new_name->Append("_save.root");
      	if (verboseMode) cout	<< setgreen
								<< "M_analyze: M_prod being opened as READ" << endl
         						<< "           Saving histograms to " << new_name->Data() << endl; 
	   	M_prod = TMapFile::Create((const Text_t *)mmap_name,"READ");
      	save_map = new TFile(new_name->Data(),"RECREATE");
      	TMapRec *mr = M_prod->GetFirst();
      	if(mr){
         	while (M_prod->OrgAddress(mr)) {
            	if(!mr) break;
            	hname=mr->GetName();
            	TH1 *hist=0;
            	hist    = (TH1 *) M_prod->Get(hname, hist);
            	if(hist){ hist->Write(); delete hist; hist = NULL;}
	//            cout << "Writing: " << hname << endl;
            	mr=mr->GetNext();         
         	}
      	}
    		M_prod->Close();
      	TString RmCmd = "rm "; 
      	RmCmd += (const Text_t *)mmap_name;
      	gSystem->Exec((const char *)RmCmd);
		} 
   		if (verboseMode) cout	<< setgreen
								<< "M_analyze: M_prod being opened as NEW"
								<< setblack << endl;
		TMapFile::SetMapAddress(0x46000000);		// alloc map file in hi mem
		M_prod = TMapFile::Create((const Text_t *)mmap_name,"RECREATE",
										mmap_size, "M memory mapped file");
   }
// pass unix args to i/o spec
	ioSpec = new TMrbIOSpec();
	if (!ioSpec->SetStartStop(start_event, stop_event)) exit(1);
	ioSpec->SetInputFile(data_source.Data(), input_mode);
	ioSpec->SetOutputFile(output_file.Data(), output_mode);
	ioSpec->SetParamFile(param_file.Data(), param_mode);
	ioSpec->SetHistoFile(histo_file.Data(), histo_mode);

//  create user's analyze class
	u_analyze = new TMrbAnalyze(ioSpec);
	u_analyze->SetScaleDown(downscale);
	u_analyze->SetRunId(runnr);
	u_analyze->SetRunStatus(TMrbAnalyze::M_STARTING);

	u_analyze->AddIOSpec(ioSpec);
//	PutPid(TMrbAnalyze::M_STARTING);
//	cout << "SizeOfMappedObjects before Update "
//        <<  u_analyze->GetSizeOfMappedObjects(M_prod) << endl;
//---
   if(kUseMap){
   	if(save_map){
	//     get histos back from temp file
      	cout << setgreen << "Restoring histograms from previous run" << setblack << endl;
      	TMapRec *mr = M_prod->GetFirst();
      	if(mr){
         	while (M_prod->OrgAddress(mr)) {
            	if(!mr) break;
            	hname=mr->GetName();
            	TH1 * hist=0;
            	TH1 * histint=0;
            	hist    = (TH1 *) save_map->Get(hname);
            	if(hist){ 
               	histint = (TH1*)gROOT->FindObject(hname);
               	if(histint){
                  	if(   hist->GetDimension() != histint->GetDimension() 
                     	|| hist->GetNbinsX()    + hist->GetNbinsY()    + hist->GetNbinsZ() != 
                        	histint->GetNbinsX() + histint->GetNbinsY() + histint->GetNbinsZ()
                     	|| hist->Sizeof()       != histint->Sizeof()){
                     	cout << "Histogram: " << hname << " changed size, wont restore" << endl;
                  	} else { 
                  	  hist->SetDirectory(gROOT);                    
                  	  histint->Add(hist,1);
                  	  histint->Print();                       
                  	}
               	}
            	}
            	mr=mr->GetNext();         
         	}
      	}
      	save_map->Close();
   	}
	//--- 
		M_prod->Update();								// update all objects in shared memory

//     add pointers to hist to directory in memory

      TMapRec * mr = M_prod->GetFirst();
      while (mr){
         TH1 * hm = (TH1 *)mr->GetObject();
         if(hm && hm->InheritsFrom(TH1::Class()))gROOT->Append(hm);
//         hm->Print();
         mr = mr->GetNext();
      }
//
	// check if map file size greater than size of mapped objects
		Int_t mobj_size = u_analyze->GetSizeOfMappedObjects(M_prod);
		mobj_size += kMB;			// quick hack: seems to be calculated too small by TMapfile!
		if (mobj_size > mmap_size) {
			Float_t xmb = ((Float_t) mobj_size) / kMB + .5;
			cerr	<< setred
					<< "M_analyze: Size of mapped objects ("
					<< setiosflags(ios::fixed) << setprecision(1) << xmb << " MB) > size of map file ("
					<< (mmap_size / kMB) << " MB)"
					<< endl;
			cerr	<< "M_analyze: Sorry, can't run ... Start over with MMapFile Size = "
					<< (Int_t) (xmb + .99) << ""
					<< setblack << endl;
			exit_from_analyze(1);
			PutPid(TMrbAnalyze::M_ABSENT);
			sleep(2);
			if(!gSystem->AccessPathName(our_pid_file.Data(), kFileExists)){
				TString RmCmd = "rm  ";
				RmCmd += our_pid_file;
				gSystem->Exec((const char *)RmCmd);
				if ( verboseMode ) cout << "M_analyze: removing " << our_pid_file << endl;
			} 
			cerr << setred << "M_analyze: terminated" << setblack << endl;
			exit(1);
		}
		u_analyze->SetMapFile(M_prod, mmap_size);
   }

//   
   gStat = new TMrbStatistics("M_analyze");
//   gStat->Init();
   gStat->Fill();

//   gStat->Print();
	if ( verboseMode ) gROOT->ls();

//	input from MBS: tcp or lmd
	if ((input_mode & TMrbIOSpec::kInputMBS) == TMrbIOSpec::kInputMBS) {
		if ( verboseMode ) cout	<< "M_analyze: Connecting to MBS - data source is " << data_source
								<< " (input type \"" << input_type << "\")"
								<< endl;

//	instance connection to MBS, file or tcp
		gMrbTransport = new TMrbTransport("M_analyze", "Connection to MBS");
		gMrbTransport->Version();

		if (!gMrbTransport->Open(data_source, input_type)) exit(1);
		if (gMrbTransport->OpenLogFile("M_analyze.log") && verboseMode) {
			cout << "M_analyze: Appending log data to file M_analyze.log" << endl;
		}

//  create root file to store trees
		if (output_mode && TMrbIOSpec::kOutputOpen != 0) {

			u_analyze->SetFileSize(file_size);

			if ((input_mode & TMrbIOSpec::kInputMBS) != 0) {
				if (oFormat == TMrbIOSpec::kFormatMED) {
					if (!gMrbTransport->OpenMEDFile(output_file)) exit(1);
				} else if (oFormat == TMrbIOSpec::kFormatLMD) {
					if (!gMrbTransport->OpenLMDFile(output_file)) exit(1);
				} else if (oFormat == TMrbIOSpec::kFormatRoot) {
					if (u_analyze->WriteRootTree(ioSpec)) exit(1);
				}
			} else if (oFormat == TMrbIOSpec::kFormatRoot) {
				if (u_analyze->WriteRootTree(ioSpec)) exit(1);
			} else {
				cerr << setred << "M_analyze: Wrong output format - " << output_file << setblack << endl;
				exit(1);
			}
		}

//	input from file list
	} else if (input_mode == TMrbIOSpec::kInputList) {
		Int_t nofEntries = u_analyze->OpenFileList(data_source, ioSpec);
		if (nofEntries == 0) exit (1);
		if ( verboseMode ) cout	<< "M_analyze: Replaying ROOT data according to file list " << data_source
								<< " (" << nofEntries << " entries)"
								<< endl;
	}

//	start the two pthreads HERE

   if (gComSocket > 0) pthread_create(&msg_thread, NULL, &msg_handler, NULL);
   pthread_create(&update_thread, NULL, &update_handler, NULL);

	gSystem->Sleep(1000);       // give the threads some time 

	cout << " PutPid(TMrbAnalyze::M_RUNNING)" << endl;
	PutPid(TMrbAnalyze::M_RUNNING);
	u_analyze->SetRunStatus(TMrbAnalyze::M_RUNNING);

//	read a given number of events from MBS (tcp or lmd file)
	if ((input_mode & TMrbIOSpec::kInputMBS) == TMrbIOSpec::kInputMBS ) {
		if ( ioSpec->IsTimeStampMode() ) {
			cerr	<< setred
					<< "M_analyze: Can't run in time stamp mode when taking data from MBS"
					<< setblack << endl;
			exit(1);
		}

//	reload params
		if ( param_mode != TMrbIOSpec::kParamNone ) u_analyze->ReloadParams(ioSpec);

		if ( (UInt_t)gMrbTransport->ReadEvents(0) == MBS_ETYPE_ABORT ) {
			cerr	<< setred
					<< "M_analyze: Unexpected ABORT returned by ReadEvents() - Click on STOP to terminate"
					<< setblack << endl;
			while ( u_analyze->GetRunStatus() != TMrbAnalyze::M_STOPPING ) {
				sleep(1);
				cerr << "" << ends;
			}
		}
		if ( verboseMode ) cout << "M_analyze: Exiting ReadEvents"<< endl;
//	output some statistics at end of session
		gMrbTransport->ShowStat();
		if( gMrbTransport != NULL ) {		// close connection to MBS
			if ( verboseMode ) {
				cout	<< "M_analyze: Closing connection to "
						<< data_source
						<< endl;
			}
			gMrbTransport->Close();
			gMrbTransport->FreeBuffers();
			if (oFormat == TMrbIOSpec::kFormatMED) gMrbTransport->CloseMEDFile();
			if (oFormat == TMrbIOSpec::kFormatLMD) gMrbTransport->CloseLMDFile();
		}
//      cout << " u_analyze->SaveHistograms(*, ioSpec);" << endl;
   	ioSpec->SetHistoFile(histo_file.Data(), histo_mode);

	u_analyze->SaveHistograms("*", ioSpec);	// save histos if needed

//	read events from root files in a loop
	} else {
      cout << "u_analyze->ProcessFileList()"<< endl;
		u_analyze->ProcessFileList();
		if ( verboseMode ) cout << "M_analyze: End of replay" << endl;
	}

	cout << "STOPPING" << endl;
	PutPid(TMrbAnalyze::M_STOPPING);
	u_analyze->SetRunStatus(TMrbAnalyze::M_STOPPING);

	
	if ( verboseMode ) cout	<< "M_analyze: Waiting for update_thread to terminate"
						<< endl;
   pthread_join(update_thread, NULL);
   if ( verboseMode ) cout	<< "M_analyze: update_thread terminated" << endl;
	u_analyze->CloseRootTree();		// close user's output file(s)
//	if(kUseMap){
//		if ( verboseMode ) cout << "M_analyze: Will close Mapfile" << endl;   
//		M_prod->Close();

//		if ( verboseMode ) cout << "M_analyze: Mapfile closed, sleep again 2 sec" << endl;
//   }
	if (gComSocket > 0) {
      pthread_cancel(msg_thread);
	   pthread_join(msg_thread, NULL);
   }
	if(!gSystem->AccessPathName(our_pid_file.Data(), kFileExists)){
		TString RmCmd = "rm  ";
		RmCmd += our_pid_file;
		gSystem->Exec((const char *)RmCmd);
		if ( verboseMode ) cout << "M_analyze: removing " << our_pid_file << endl;
	} 
	cout << "M_analyze terminated, Events Processed: " << 
         u_analyze->GetEventsProcessed()<< endl;

   PlayQuart(3);
//    for(Int_t i=0; i < 5; i++){
//       usleep(300000);
// 	   cout << "" << endl;
//   } 
}

void * update_handler(void * dummy) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           update_handler
// Purpose:        Pthread to handle update of mmapped file
// Arguments:      
// Results:        
// Exceptions:     
// Description:    Updates all objects in mmapped file
// Keywords:       
/////////////////////////////////////////////////////////////////////////////

//	update time in micro seconds

	Int_t  measured_update_time;
	struct tms buf1, buf2;
	clock_t t1, t2;
	static ULong_t total_sleep, seconds;
	seconds=0;

	while(1) {
		usleep(990000);         // sleep 1 second
		pthread_mutex_lock(&global_data_mutex);
		seconds ++;
		TH1F * rh = u_analyze->UpdateRateHistory();
		if ( M_prod ) M_prod->Update(rh); 
		TH1F * dth = u_analyze->UpdateDTimeHistory();
		if ( M_prod ) M_prod->Update(dth); 
		pthread_mutex_unlock(&global_data_mutex);
		total_sleep ++;
		if(total_sleep >= update_time){
			total_sleep = 0;
//	protect update with same mutex as fills
			pthread_mutex_lock(&global_data_mutex);
			if(u_analyze->IsToBeUpdated() > 0)
			{

// following could be done only once but maybe later will have dynamic changes in the
// contents of the TMapped file

// time the update
				t1=times(&buf1);
				if ( M_prod ) M_prod->Update();  // updates all objects in shared memory
				t2=times(&buf2);
// convert to seconds
				measured_update_time= (t2-t1)/CLOCKS_PER_SEC;
// guarantee that update takes no more than 10% of available time
				if((9*measured_update_time) > TMrbAnalyze::kUpdateTime){
					update_time= 9*measured_update_time;
					if ( verboseMode ) {
						cout	<< "M_analyze: changing update_time to "
								<< update_time
								<< endl;
         			}
				} 
			}
			pthread_mutex_unlock(&global_data_mutex);
		}
		if(u_analyze->GetRunStatus() == TMrbAnalyze::M_STOPPING){
			pthread_mutex_lock(&global_data_mutex);
			if ( M_prod ) M_prod->Update();  // updates all objects in shared memory
			pthread_mutex_unlock(&global_data_mutex);

			if ( verboseMode ) cout	<< "M_analyze: Terminating update thread"
									<< endl;
			pthread_exit(0);
		}
	}
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           exit_from_analyze
// Purpose:        Trap signal ^C
// Arguments:      Int_t n     -- signal number
// Results:        
// Exceptions:     
// Description:    Traps ^C and sets termination flag
// Keywords:       
///////////////////////////////////////////////////////////////////////////*/

void exit_from_analyze(int n) {

	signal(SIGINT, SIG_IGN);
	if ( verboseMode ) cout << "M_analyze: ^C seen" << endl;
	u_analyze->SetRunStatus(TMrbAnalyze::M_STOPPING);
	u_analyze->SetUpdateFlag();
	u_analyze->CloseRootTree();		// close user's file(s)
	if ( verboseMode ) cout << "M_analyze: Will close Mapfile" << endl;   
	if ( M_prod ) M_prod->Close();
	PutPid(TMrbAnalyze::M_STOPPING);
}

//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           msg_handler
// Purpose:        Pthread to handle messages
// Arguments:      
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
// History:        11 Jul/97 Cyril Broude
//                           Server process which allows clients to connect
//                           and send requests for operations to be performed
//                  2 Sep/97 CB
//                           Supports its own memory mapped file called M_Base
//                           and a remote memory mapped file called M_prod.
//                  9 Oct/97 CB
//                           can forward messages to M_analyze via socket
//                           e.g. zero, delete spectra
//                 16 Feb/99 OS
//                           simplify
//                           
/////////////////////////////////////////////////////////////////////////////

void * msg_handler(void * dummy) {
   cout << "Enter msg_handler, gComSocket = " << gComSocket<< endl;
//   TServerSocket *ss = new TServerSocket(gComSocket, kTRUE);
   if (!(ss->IsValid())) {
      cout << "Invalid: " << gComSocket << endl;
      return dummy;
   }

   TMonitor *mon = new TMonitor();
   mon->Add(ss);
   mon->Print();

   TSocket *s0 = 0, *s1 = 0;
   TMessage *mess =0;
   Int_t maxwait = 1000;
//     TSocket  *sock = ss->Accept();
//     sock->Send("go 0");

   while (1){
      TSocket *sock;
      sock = mon->Select(maxwait);
      if (sock == (TSocket*)-1) {
         if (u_analyze->GetRunStatus() == TMrbAnalyze::M_STOPPING) break;
         else continue;   
      }
      if (sock->IsA() == TServerSocket::Class()) {

         if (!s0) {
            s0 = ((TServerSocket *)sock)->Accept();
            s0->Send("go 0");
            mon->Add(s0);
            cout << "Added socket s0" << endl;
         } else if (!s1) {
            s1 = ((TServerSocket *)sock)->Accept();
            s1->Send("go 1");
            mon->Add(s1);
            cout << "Added socket s1" << endl;
         } else
            cout << "only accept two client connections" << endl;

//         if (s0 && s1) {
 //           mon->Remove(ss);
//            ss->Close();
//            ss = NULL;
//         }
         continue;
      }

//      cout << "msg_handler(): socket: " << sock << endl;

      sock->Recv(mess);
      if (mess == 0) {
//  client exited without message
         cout << "M_analyze::msg_handler(): mess == 0" << endl;
         mon->Remove(sock);
         if (sock == s0) s0 = 0;
         if (sock == s1) s1 = 0;
         continue;
      }
//      if ( verboseMode ) cout << "M_analyze::msg_handler(): Message type " << mess->What() << endl;
      if(mess->What() == kMESS_STRING){
         char str[300];
         mess->ReadString(str, 250);
			if ( verboseMode ) cout << 
           "M_analyze::msg_handler(): Read from client: " << str << endl;
 //        no_of_parameters= M.parse(str, parms);
         TString  smess = str; smess = smess.Strip(TString::kBoth);
         if(smess(0,9) != "M_client "){               
            cerr << setred
				<< "M_analyze::msg_handler(): Illegal client - " << smess(0,9)
				<< setblack << endl;
//            break;
         } else smess.Remove(0,9);
         TString arg; TString cmd; 
         Int_t nc = smess.Index(" ");
         if(nc <= 0) cmd = smess;
         else {
            cmd = smess(0,nc);
            smess.Remove(0,nc+1);
            nc = smess.Index(" ");
            if(nc <= 0) arg = smess;
            else arg = smess(0,nc);
         }
         Bool_t send_ack = kTRUE;

         if     (cmd == "terminate") u_analyze->SetRunStatus(TMrbAnalyze::M_STOPPING);         
         else if(cmd == "pause")     u_analyze->SetRunStatus(TMrbAnalyze::M_PAUSING);
         else if(cmd == "resume")    u_analyze->SetRunStatus(TMrbAnalyze::M_RUNNING);

         else if(cmd == "downscale") {
            istrstream inbuf(arg.Data());
            Int_t downscale; inbuf >> downscale;
            if(downscale <= 0) downscale = 1;
                                     u_analyze->SetScaleDown(downscale);

         } else if(cmd == "reload") {
             u_analyze->ReloadParams(arg.Data());

         } else if(cmd == "savehists") {
             u_analyze->SaveHistograms("*", ioSpec);	// save histos

         } else if ( cmd == "gethist" ) {
			   pthread_mutex_lock(&global_data_mutex);
            TH1 * hist = (TH1 *)gROOT->GetList()->FindObject(arg.Data());
            if ( hist ) {
               TMessage * message = new  TMessage(kMESS_OBJECT);
               message->WriteObject(hist);     // write object in message buffer
//               hist->Print();
			      pthread_mutex_unlock(&global_data_mutex);
               sock->Send(*message);          // send message
               delete message;
            } else {
               TMessage * message = new  TMessage(kMESS_STRING);
               message->WriteString("Histo not found");
               cout << setred << "Histo not found |" 
               <<arg.Data() << "|" << setblack << endl;
			      pthread_mutex_unlock(&global_data_mutex);
               sock->Send(*message);          // send message
               delete message;
            }
            send_ack = kFALSE;
         } else if ( cmd == "getstat" ) {
            if ( gStat ) {
               TMessage * message = new  TMessage(kMESS_OBJECT);
			      pthread_mutex_lock(&global_data_mutex);
		         gStat->Update();
               message->WriteObject(gStat);     // write object in message buffer
			      pthread_mutex_unlock(&global_data_mutex);
               sock->Send(*message);          // send message
               delete message;
            } else {
               TMessage * message = new  TMessage(kMESS_STRING);
               message->WriteString("Stat not found");
               cout << setred << "Stat not found " << endl;
               sock->Send(*message);          // send message
               delete message;
            }
            send_ack = kFALSE;
        } else if ( cmd == "zero" ) {
           int count;
           TMessage mess0(kMESS_STRING);
           count = u_analyze->ClearHistograms(arg.Data());
		     if (count > 0) u_analyze->SetUpdateFlag();
           mess0.Reset();          // re-use TMessage object
           ostrstream buf;
           buf << "Number of histos cleared: " << count << ends;
           mess0.WriteString(buf.str());
           buf.rdbuf()->freeze(0);
           sock->Send(mess0);

         } else if ( cmd == "user" ){
           Int_t result = u_analyze->HandleMessage(smess.Data());
           if ( verboseMode ) cout << "UserMessage called, result: " 
                            << result << endl;                
         } else if ( cmd == "exit" ){
            send_ack = kFALSE;
         	cout << "M_analyze::msg_handler(): M_client exit" << endl;
         	mon->Remove(sock);
         	if (sock == s0) s0 = 0;
         	if (sock == s1) s1 = 0;
         	continue;
         } else {
            cerr << setred
				<< "M_analyze::msg_handler(): Invalid function request - " << str
				<< setblack << endl; 
            sock->Send("FAILED");
            send_ack = kFALSE;
        }
//        cout << "mon->Remove(sock) " << endl;
//        mon->Remove(sock);
        if(send_ack)sock->Send("ACK");
        if (u_analyze->GetRunStatus() == TMrbAnalyze::M_STOPPING) {
           break;
        }
         
      } else {
          cerr << setred
					<< "M_analyze::msg_handler(): Unexpected message"
					<< setblack << endl;
         sock->Send("FAILED");
      }
      if (mess) { delete mess; mess = 0;};
   }

   if (s0) s0->Close();
   if (s1) s1->Close();
   if (ss) ss->Close();

   if (mess) delete mess;
   cout << "exit msg_handler" << endl;
   return dummy;
}
