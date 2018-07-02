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

namespace std {} using namespace std;

#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
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

#include <execinfo.h>

// global pthread mutex 
extern pthread_mutex_t global_data_mutex;

/////////////////////////////////////////////

// Globals
TMrbAnalyze * u_analyze;
extern TMrbTransport * gMrbTransport;
TMrbIOSpec * ioSpec;

TServerSocket *gServerSocket = 0;
Int_t  gComSocket;
Int_t hsave_intervall;
static Bool_t verboseMode = kFALSE;
static TString our_pid_file = "/tmp/M_analyze_";
TMrbStatistics * gStat;
// global PTHREAD handles
pthread_t msg_thread, update_thread;

// pthread prototypes
void * msg_handler(void *);               // pthreaded message handler routine
void * update_handler(void *);            // pthreaded timed update routine

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

	void exit_from_analyze(int);
	void handle_segviol(int);

	if(argc <= 1) {
		cerr	<< setred
				<<	endl
				<<	"Usage: M_analyze data_source [input_type]" << endl
				<<	"                             [start_event] [stop_event]" << endl
				<<	"                             [run_number] [down_scale]" << endl
				<<	"                             [output_file] [file_size]" << endl
				<<	"                             [parm_file] [histo_file]" << endl
				<<	"                             [mmap_file] [mmap_size]" << endl
				<<	"                             [hsave_intervall]" << endl
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
				<<	"       mmap_file      \"none\"        dummy argument for backward compatibility" << endl
				<<	"       mmap_size      0               dummy argument for backward compatibility" << endl
				<<	"       gComSocket     0                socket for communication with C_analyze" << endl
				<< "       hsave_intervall 0               frequency for autosaving histos [sec]" << endl
 				<< setblack <<	endl << endl;
		exit(0);
	}

//   for(int i=0; i < argc; i++)
//      cout << "arg " << i << ": " << argv[i] << endl;

	verboseMode = (Bool_t) gEnv->GetValue("TMrbAnalyze.VerboseMode", kFALSE);

// install signal handler
	signal(SIGINT, exit_from_analyze);
	signal(SIGSEGV, handle_segviol);

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
	else {
      if (data_source.EndsWith("list")) input_type = "L";
      else                              input_type = "F";
   }
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Input type:  " <<  input_type<< endl;

	TRegexp rxroot("\\.root$");
	TRegexp rxmed("\\.med$");
	TRegexp rxlmd("\\.lmd$");
	TRegexp rxlist("\\.list$");

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
					if (data_source.Index(rxlist, 0) > 0) {
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
		output_mode = (TMrbIOSpec::EMrbOutputMode) (TMrbIOSpec::kOutputOpen | TMrbIOSpec::kOutputClose);
		if (output_file.Index(rxroot, 0) > 0) output_mode = (TMrbIOSpec::EMrbOutputMode) (output_mode | TMrbIOSpec::kOutputWriteRootTree);
		else if (output_file.Index(rxmed, 0) > 0) output_mode = (TMrbIOSpec::EMrbOutputMode) (output_mode | TMrbIOSpec::kOutputWriteMEDFormat);
		else if (output_file.Index(rxlmd, 0) > 0) output_mode = (TMrbIOSpec::EMrbOutputMode) (output_mode | TMrbIOSpec::kOutputWriteLMDFormat);
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
	else if (param_file.Index(":", 0) > 0)			param_mode = TMrbIOSpec::kParamReloadMultiple;
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
	else {
 	   histo_file = "Histos_";
      histo_file += data_source;
      if (!histo_file.EndsWith("root")) histo_file += "root";
   }
	if (histo_file.CompareTo("none") == 0)	histo_mode = TMrbIOSpec::kHistoNone;
	else									histo_mode = TMrbIOSpec::kHistoSave;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Histo file:  " << histo_file << endl;

// mmap file & size
	TString mmap_name = "none";
	argNo++;

	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] TMap name:   " << mmap_name << endl;
	Int_t mmap_size = 0;
	argNo++;
//	if(argc > argNo)	mmap_size = TMrbAnalyze::kMB * atoi(argv[argNo]);
//	else				mmap_size = 0;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] TMap size:   " << mmap_size << endl;

	gComSocket = 0;
	argNo++;
	if(argc > argNo)	gComSocket = atoi(argv[argNo]);
        our_pid_file += gSystem->Getenv("USER");
	our_pid_file += ".";
	our_pid_file += gComSocket;
	if (verboseMode) cout << "M_analyze: [Arg" << argNo << "] Communication socket:   " <<  gComSocket<< endl;
   hsave_intervall = 0;
	argNo++;
	if(argc > argNo)	hsave_intervall = atoi(argv[argNo]);
	if (verboseMode) cout << "M_analyze: Histo save intervall[sec]:   " << hsave_intervall << endl;
   if (verboseMode) cout << "M_analyze: Reading of arguments done (" << argNo << " args)" << endl;

	PutPid(TMrbAnalyze::M_STARTING);

   if ( gComSocket > 0 ) gServerSocket = new TServerSocket(gComSocket, kTRUE);

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
//
   gStat = new TMrbStatistics("M_analyze");
//   gStat->Init();
   gStat->Fill();

//   gStat->Print();
	if ( verboseMode ) gROOT->ls();

//	input from MBS: tcp or lmd
   gMrbTransport = NULL;
	if ((input_mode & TMrbIOSpec::kInputMBS) != 0) {
		if ( verboseMode ) cout	<< "M_analyze: Connecting to MBS - data source is " << data_source
								<< " (input type \"" << input_type << "\")"
								<< endl;

//	instance connection to MBS, file or tcp
		gMrbTransport = new TMrbTransport("M_analyze", "Connection to MBS");
		gMrbTransport->Version();
		gMrbTransport->SetStopFlag(kFALSE);

	    cout << " PutPid(TMrbAnalyze::M_RUNNING)" << endl;
	    PutPid(TMrbAnalyze::M_RUNNING);
	    u_analyze->SetRunStatus(TMrbAnalyze::M_RUNNING);
		if (!gMrbTransport->Open(data_source, input_type)) exit(1);

		if (gMrbTransport->OpenLogFile("M_analyze.log") && verboseMode) {
			cout << "M_analyze: Appending log data to file M_analyze.log" << endl;
		}

//  create root file to store trees
		if (output_mode && TMrbIOSpec::kOutputOpen != 0) {

			u_analyze->SetFileSize(file_size);

			if (output_mode & TMrbIOSpec::kOutputWriteMEDFormat) {
				if (!gMrbTransport->OpenMEDFile(output_file)) exit(1);
			} else if (output_mode & TMrbIOSpec::kOutputWriteLMDFormat) {
				if (!gMrbTransport->OpenLMDFile(output_file)) exit(1);
			} else if (output_mode & TMrbIOSpec::kOutputWriteRootTree) {
				if (!u_analyze->WriteRootTree(ioSpec)) exit(1);
			} else {
				cerr << setred << "M_analyze: Wrong output format - " << output_file << setblack << endl;
				exit(1);
			}
		}

//	input from file list
	} else if (input_mode == TMrbIOSpec::kInputList) {
		Int_t nofEntries = u_analyze->OpenFileList(data_source, ioSpec);
		if (nofEntries == 0) exit (1);
		if ( verboseMode ) cout	<< "M_analyze: Replaying data according to file list " << data_source
								<< " (" << nofEntries << " entries)"
								<< endl;
	}

//	start the two pthreads HERE

//	gSystem->Sleep(1000);       // give main  some time

   if (gComSocket > 0) pthread_create(&msg_thread, NULL, &msg_handler, NULL);
   if (gComSocket > 0 ) pthread_create(&update_thread, NULL, &update_handler, NULL);

	gSystem->Sleep(1000);       // give the threads some time

//	cout << " PutPid(TMrbAnalyze::M_RUNNING)" << endl;
//	PutPid(TMrbAnalyze::M_RUNNING);
//	u_analyze->SetRunStatus(TMrbAnalyze::M_RUNNING);
   cout << "M_analyze start at: " << flush;
   gSystem->Exec("date");
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

		UInt_t rsts = (UInt_t) gMrbTransport->ReadEvents(0);
		if ( rsts == MBS_ETYPE_ABORT ) {
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
			if ((output_mode & TMrbIOSpec::kOutputWriteMEDFormat) == TMrbIOSpec::kOutputWriteMEDFormat) gMrbTransport->CloseMEDFile();
			if ((output_mode & TMrbIOSpec::kOutputWriteLMDFormat) == TMrbIOSpec::kOutputWriteLMDFormat) gMrbTransport->CloseLMDFile();
		}
//      cout << " u_analyze->SaveHistograms(*, ioSpec);" << endl;
   	ioSpec->SetHistoFile(histo_file.Data(), histo_mode);

		u_analyze->FinishRun(ioSpec, kTRUE);	// finish run, do some work BEFORE saving histos
		u_analyze->SaveHistograms("*", ioSpec);	// save histos if needed
		u_analyze->FinishRun(ioSpec, kFALSE);	// finish run AFTER saving histos

//	read events from root files in a loop
	} else {
      cout << "u_analyze->ProcessFileList()"<< endl;
		u_analyze->ProcessFileList();
		if ( verboseMode ) cout << "M_analyze: End of replay" << endl;
	}

//	cout << "STOPPING" << endl;
//	PutPid(TMrbAnalyze::M_STOPPING);
//	u_analyze->SetRunStatus(TMrbAnalyze::M_STOPPING);


	if ( verboseMode ) cout	<< "M_analyze: Waiting for update_thread to terminate" << endl;
   if (gComSocket > 0 ) {
		void *r;
		pthread_join(update_thread, &r);
	}
   if ( verboseMode ) cout	<< "M_analyze: update_thread terminated" << endl;
	u_analyze->CloseRootTree();		// close user's output file(s)
	if (gComSocket > 0) {
		if ( verboseMode ) cout	<< "M_analyze: Waiting for msg_thread to terminate"
						<< endl;
		void *r;
	   pthread_join(msg_thread, &r);
    	cout	<< "M_analyze: msg_thread terminated" << endl;
   }
	if(!gSystem->AccessPathName(our_pid_file.Data(), kFileExists)){
		TString RmCmd = "rm  ";
		RmCmd += our_pid_file;
		gSystem->Exec((const char *)RmCmd);
		if ( verboseMode ) cout << "M_analyze: removing " << our_pid_file << endl;
	}
	cout << "M_analyze terminated at: " << flush; gSystem->Exec("date");

	cout << "Events Processed: " << u_analyze->GetEventsProcessed()<< endl;

// close possible network connection
	if (gServerSocket != NULL) {
		cout << "Closing socket: " << gComSocket << endl;
		gServerSocket->Close();
		delete gServerSocket;
	}
   TEnv env(".rootrc");
//   env.Print()
   if (env.GetValue("M_analyze.PlaySound", 0) > 0) {
		TString soundfile = env.GetValue("M_analyze.EndOfRunSound", "");
		TString expanded_name(soundfile.Data());
		if (expanded_name.BeginsWith("$")) {
			Int_t indslash = expanded_name.Index("/");
			TString var = expanded_name(1,indslash-1);
			expanded_name.Remove(0,indslash);
			expanded_name.Prepend(gSystem->Getenv(var.Data()));
		}
		TString soundplayer = env.GetValue("M_analyze.SoundPlayer", "/usr/bin/play");
		if (!gSystem->AccessPathName(expanded_name.Data(), kFileExists)) {
			if (!gSystem->AccessPathName(soundplayer, kFileExists)) {
				TString cmd(soundplayer);
				cmd += " ";
				cmd += expanded_name;
				gSystem->Exec(cmd);
			} else {
				cout << "SoundPlayer " << soundplayer << " not found" <<endl;
			}
		} else {
			cout << "SoundFile " << expanded_name << " not found" <<endl;
		}
   }
//    for(Int_t i=0; i < 5; i++){
//       usleep(300000);
// 	   cout << "" << endl;
//   }
	cout << "Exit from M_analyze, no exit(0)" << endl;
//	exit(0);
}

void * update_handler(void * dummy) {
//________________________________________________________________[C FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           update_handler
// Purpose:        Pthread to handle update of histo files
// Arguments:
// Results:
// Exceptions:
// Description:    Updates all objects in mmapped file
// Keywords:
/////////////////////////////////////////////////////////////////////////////

//	update time in micro seconds

	static Int_t sleep_before_update = 0;

	while(1) {
		struct timespec t = { 1, 0 };
		nanosleep(&t, NULL);         // sleep 1 second
		pthread_mutex_lock(&global_data_mutex);
		u_analyze->UpdateRateHistory();
		u_analyze->UpdateDTimeHistory();
		pthread_mutex_unlock(&global_data_mutex);
		sleep_before_update ++;
		if (hsave_intervall > 0 && sleep_before_update >= hsave_intervall) {
		   sleep_before_update = 0;
//			pthread_mutex_lock(&global_data_mutex);
			u_analyze->SaveHistograms("*", ioSpec);
//			pthread_mutex_unlock(&global_data_mutex);
		}
		if(u_analyze->GetRunStatus() == TMrbAnalyze::M_STOPPING){
			if ( verboseMode ) cout	<< "M_analyze: Terminating update thread" << endl;
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

void handle_segviol(int n) {

	signal(SIGSEGV, SIG_IGN);
	cout << setred  << "M_analyze: sigviol" << setblack << endl;
//	gSystem->StackTrace();

	 void *array[100];
	 size_t size;
	 char **strings;
	 size_t i;

	 size = backtrace (array, 100);
	 strings = backtrace_symbols (array, size);

	 printf ("Obtained %zd stack frames.\n", size);

	 for (i = 0; i < size; i++)
		 printf ("%s\n", strings[i]);

	 free (strings);

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
//                 21 Feb/11 OS
//                           remove Mapfile stuff
//
/////////////////////////////////////////////////////////////////////////////

void * msg_handler(void * dummy) {
   const Int_t kMaxSock = 6;
   cout << setyellow<< "Enter msg_handler, gComSocket = " << gComSocket
	<< " RunStatus: "
	<<u_analyze->GetRunStatus()<<setblack<< endl;
	if (!(gServerSocket->IsValid())) {
      cout << "Invalid: " << gComSocket << endl;
      return dummy;
   }
	TString  smess;
   TString arg;
   TString cmd;
	TString buf;
   char *str = new char[300];
   TMonitor *mon = new TMonitor();
	mon->Add(gServerSocket);
   mon->Print();

   TSocket * s[kMaxSock] = {0};
   TMessage *mess =0;
   Int_t maxwait = 1000;
	TSocket *sock;

   while (1) {
      sock = mon->Select(maxwait);
      if (sock == (TSocket*)-1) {
		   if (u_analyze->GetRunStatus() == TMrbAnalyze::M_STOPPING) break; else continue;
      }
      if (sock->IsA() == TServerSocket::Class()) {
         Bool_t ok = kFALSE;
         for (Int_t i = 0; i < kMaxSock - 1; i++) {
            if (!s[i]) {
               s[i] = ((TServerSocket *)sock)->Accept();
               s[i]->Send("accept");
               mon->Add(s[i]);
               cout << "Added socket s[" << i << "]" << endl;
               ok = kTRUE;
               break;
            }
         }
//         cout << " GetListOfSockets() at start" << endl;
//        gROOT->GetListOfSockets()->ls();
         if (!ok) {
            cout << "only accept " << kMaxSock - 1 << " client connections" << endl;
            s[kMaxSock - 1] = ((TServerSocket *)sock)->Accept();
            s[kMaxSock - 1]->Send("reject");
            s[kMaxSock - 1]->Close();
            s[kMaxSock - 1] = NULL;
         }
         continue;
      }

      sock->Recv(mess);
      if (mess == 0) {
//  client exited without message
         cout << "M_analyze::msg_handler(): mess == 0" << endl;
         mon->Remove(sock);
         for (Int_t i = 0; i < kMaxSock - 1; i++) {
            if (sock == s[i]) s[i] = NULL;
         }
         continue;
      }
//      if ( verboseMode ) cout << "M_analyze::msg_handler(): Message type " << mess->What() << endl;
      if(mess->What() == kMESS_STRING){
//         char str[300];
         mess->ReadString(str, 250);
//	if ( verboseMode ) cout << "M_analyze::msg_handler(): Read from client: " << str << endl;
//        no_of_parameters= M.parse(str, parms);
         smess = str; 
         smess = smess.Strip(TString::kBoth);
         if(smess(0,9) != "M_client "){
            cerr << setred
				<< "M_analyze::msg_handler(): Illegal client - " << smess(0,9)
				<< setblack << endl;
//            break;
         } else smess.Remove(0,9);
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
//         if ( verboseMode ) cout << "M_analyze::msg_handler(): Message " << cmd << endl;

         if     (cmd == "terminate") {
			   u_analyze->SetRunStatus(TMrbAnalyze::M_STOPPING);
			   if (gMrbTransport) {
               cerr	<< "M_analyze::msg_handler(): Sending STOP to TMrbTransport" << endl;
			      gMrbTransport->SetStopFlag(kTRUE);
			   }
				cout << "msg_handler: terminate received" << endl;
			   break;
         } else if(cmd == "pause") {
			u_analyze->SetRunStatus(TMrbAnalyze::M_PAUSING);
         } else if(cmd == "resume") {
			u_analyze->SetRunStatus(TMrbAnalyze::M_RUNNING);

         } else if(cmd == "downscale") {
            istringstream inbuf(arg.Data());
            Int_t downscale; inbuf >> downscale;
            if(downscale <= 0) downscale = 1;
                                     u_analyze->SetScaleDown(downscale);

         } else if(cmd == "reload") {
             u_analyze->ReloadParams(arg.Data());
//
         } else if(cmd == "savehists") {
             u_analyze->SaveHistograms("*", ioSpec);	// save histos

         } else if ( cmd == "gethist" ) {
			   pthread_mutex_lock(&global_data_mutex);
				TH1 * hist = (TH1 *)gROOT->GetList()->FindObject(arg.Data());
				if (!hist || hist->GetEntries() <= 0 )
					hist = NULL;
            if ( (u_analyze->GetRunStatus() == TMrbAnalyze::M_RUNNING
                      || u_analyze->GetRunStatus() == TMrbAnalyze::M_PAUSING)
                && hist != NULL ) {
						TMessage * message = new  TMessage(kMESS_OBJECT);
						message->WriteObject(hist);     // write object in message buffer
						sock->Send(*message);          // send message
						delete message;
//					}
					pthread_mutex_unlock(&global_data_mutex);
				} else {
               TMessage * message = new  TMessage(kMESS_STRING);
               message->WriteString("Histo not found");
//                if (verboseMode)
//                  cout << setred << "Histo not found |"
//                  <<arg.Data() << "|" << setblack << endl;
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
           buf = Form("Number of histos cleared: %d", count);
           mess0.WriteString(buf.Data());
           sock->Send(mess0);

         } else if ( cmd == "user" ){
           Int_t result = u_analyze->HandleMessage(smess.Data());
           if ( verboseMode ) cout << "UserMessage called, result: "
                            << result << endl;
         } else if ( cmd == "exit" ){
            send_ack = kFALSE;
         	cout << "M_analyze::msg_handler(): M_client exit" << endl;
         	mon->Remove(sock);
         	for (Int_t i = 0; i < kMaxSock - 1; i++) {
            	if (sock == s[i]) s[i] = NULL;
         	}
         	continue;
         } else {
            cerr << setred
				<< "M_analyze::msg_handler(): Invalid function request - " << str
				<< setblack << endl;
            sock->Send("FAILED");
            send_ack = kFALSE;
        }
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
	if (mon) {
		if (sock) {
			mon->Remove(sock);
			gROOT->GetListOfSockets()->Remove(sock);
			delete sock;
		}
		delete mon;
	}
   if (mess) delete mess;
   delete [] str;
//   cout << " GetListOfSockets() at exit" << endl;
//  gROOT->GetListOfSockets()->ls();
   cout << "exit msg_handler " << endl<<flush;
	pthread_exit(0);
	//return dummy;
}
