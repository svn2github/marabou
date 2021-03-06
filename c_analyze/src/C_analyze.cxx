//#include <stdlib.h>
#include <sys/socket.h>
#include "TString.h"
//#include "TRint.h"
#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>

#include <TGClient.h>
#include <TGResourcePool.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGaxis.h>
#include <TGButton.h>
//#include <TGTextEntry.h>
//#include <QTGTextView.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGFileDialog.h>
#include "TGScrollBar.h"
#include "TRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TUnixSystem.h"
#include "TInetAddress.h"
#include "TH1.h"
#include "TFile.h"
#include "TGWindow.h"
#include "TDatime.h"
#include <TEnv.h>
#include <TStyle.h>
#include <TSocket.h>
#include <TStopwatch.h>
#include <TMonitor.h>
#include <TMessage.h>
#include "TTime.h"
#include "TPaveText.h"
#include "C_analyze.h"
#include "C_analyze_Help.h"
#include "TGMrbInputDialog.h"
#include "TGMrbHelpWindow.h"
#include "TMrbNamedX.h"
#include "TMrbString.h"

#include "SetColor.h"
#include <iostream>
#include <fstream>
#include <iomanip>
//#include <sstream.h>

#include <dirent.h>

//Bool_t kUseMap = kTRUE;
// #include "/usr/local/include/uti/psinfo.h"

TObjArray * ppcArray;
Int_t nofPPCs;

static const char sepline[] =
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
//_____________________________________________________________________________________
//
Bool_t confirm(const char *quest, TGMainFrame * frame){
	Int_t retval = kMBYes, buttons = kMBYes | kMBNo;
	EMsgBoxIcon icontype = kMBIconQuestion;
	new TGMsgBox(gClient->GetRoot(),frame,
	 "Question",quest,icontype, buttons, &retval);
	return(retval == kMBYes);
}
//_____________________________________________________________________________________

class MyTimer;
FhMainFrame* mainWindow;
MyTimer* mt;

TGTextEntry       *ccc;
//_____________________________________________________________________________________
//_____________________________________________________________________________________

class MyTimer : public TTimer{

public:
	MyTimer(Long_t ms, Bool_t synch);
	Bool_t Notify();
	Bool_t ReadNotify(){return Notify();}
};
//_____________________________________________________________________________________
//
MyTimer::MyTimer(Long_t ms, Bool_t synch):TTimer(ms,synch){
//     cout << "init mytimer" << endl;
	gSystem->AddTimer(this);
}
//_____________________________________________________________________________________
//
Bool_t MyTimer::Notify() {
//   cout << " Notify reached" << endl;
	mainWindow->Runloop();
	Reset();
	return kTRUE;
};
//_____________________________________________________________________________________
//
void WarnBox( const char * message, TGWindow* win = 0){
	int retval=0;
	cout << setred << "C_analyze: " << message << setblack << endl;
	new TGMsgBox(gClient->GetRoot(), win,
	"Warning", message,
//      gClient->GetPicture("/home/rg/schaileo/myroot/xpm/warn.xpm"),
	kMBIconExclamation,
	kMBDismiss, &retval);
}
//_____________________________________________________________________________________

Int_t chkquota(const char * file, Int_t hard_hwm,
					Int_t warn_hwm, Int_t loglev){
//
//   check if user has write access and enough disk space
//   and quota are left.
//
//
//   return: -1 no write access, space  or quota above hard high water mark
//           -2 space or quota  above warning mark
//           -3
//           -4 could not get all file info, errors from sys calls
//           -5 no quota active
//
// get full pathname
	cout << "loglev " << loglev<< endl;
	TString dir(gSystem->DirName(file));
	if(!gSystem->IsAbsoluteFileName(dir.Data())){
		dir.Prepend("/");
		dir.Prepend(gSystem->WorkingDirectory());
	}
//   cout << dir << endl;
//  check if we have write access
	if(gSystem->AccessPathName(dir.Data(), kWritePermission)){
		cout << setred << "No write permission on: "
			  << dir.Data() << setblack << endl;
		return -1;
	} else {
		cout << setgreen << "Write permission ok on: "
			  << dir.Data() << setblack << endl;
	}
// make sure it is mounted when quota and mount command is executed
	TString lscmd = "ls ";
	lscmd += dir.Data();
	lscmd += " > /dev/null";
	gSystem->Exec(lscmd.Data());
//  quota
	TString cmd("quota > ");
	TString quotafile("/tmp/");
	quotafile += gSystem->Getenv("USER");
	quotafile += "_quota.lst";
	cmd += quotafile;
	gSystem->Exec(cmd);
//  mounts
//   cmd = "mount > ";
//   TString mountfile("/tmp/");
//   mountfile += gSystem->Getenv("USER");
//   mountfile += "_mounts.lst";
//   cmd += mountfile;
//   gSystem->Exec(cmd);
//
	ifstream wstream;
	TString wline;
//
	TString mountfile("/etc/mtab");
	TList mountpoints;
	TList filesystems;

	wstream.open(mountfile.Data(), ios::in);
	if (!wstream.good()) {
	cerr	<< "C_analyze: "
			<< gSystem->GetError() << " - " << mountfile.Data()
			<< endl;
		return -4;
	}
	for (;;) {
		wline.ReadLine(wstream, kFALSE);
		if (wstream.eof()) {
			wstream.close();
			break;
		}
		Int_t sep = wline.Index(" ");
		if(sep < 0)break;
		TString temp(wline(0,sep));
		mountpoints.Add(new TObjString(temp));
		wline.Remove(0,sep+1);
		sep = wline.Index(" ");
		if(sep < 0)break;
		temp = wline(0,sep);
		filesystems.Add(new TObjString(temp));
//      cout << wline.Data() << endl;
	}
//  find mount point for input directory, look for longest match
	Int_t mpind = -1;
	for(Int_t i=0; i < filesystems.GetSize(); i++){
		if(dir.BeginsWith(((TObjString*)filesystems.At(i))->GetString())){
			if(mpind < 0 || (mpind >=0 &&
			((TObjString*)filesystems.At(i))->GetString().Length() >
			((TObjString*)filesystems.At(mpind))->GetString().Length()))mpind = i;
		}
	}
	if(mpind < 0){
		cout << "Mount point not found for: " << dir << endl;
		return -4;
	}
	wstream.close();

//  check free disc space
//	Int_t error = 0;
	Long_t id=0, bsize=0, blocks=0, bfree=0;
	Double_t tot_size;
	Double_t free_size;
	const char * fs =
	((TObjString*)filesystems.At(mpind))->GetString().Data();
	if(!gSystem->GetFsInfo(fs, &id, &bsize, &blocks, &bfree)){
		tot_size = (Double_t)(blocks) * (Double_t)bsize /(1000*1024);
		free_size = (Double_t)(bfree) * (Double_t)bsize /(1000*1024);
		if(loglev > 0 || free_size < warn_hwm)
			cout << "Filesysteminfo for: " << fs << endl
//               << " retcode " << result
//               << " bsize " << bsize
			<< "total size: " << tot_size << " Mbytes "<< endl
			<< "free:       " << free_size<< " Mbytes "<< endl
			<< "warn limit: " << warn_hwm << " Mbytes "<< endl
			<< "hard limit: " << hard_hwm << " Mbytes "
			<< endl;
			if(free_size < hard_hwm){
				cout << setred << "Fatal: disk space below hard limit" << setblack << endl;
				return -1;
			} else if(free_size < warn_hwm){
				cout << setred << "Warning: disk space low " << setblack << endl;
//				error = -2;
			}
	} else {
		cout << "failed to get Filesysteminfo for: " << fs << endl;
//		error = -5;
	}
//  now look in quota file
	wstream.open(quotafile.Data(), ios::in);
	if (!wstream.good()) {
	cerr	<< "C_analyze: "
			<< gSystem->GetError() << " - " << quotafile.Data()
			<< endl;
		return -4;
	}
	Int_t used, soft_limit, hard_limit;
	Int_t f_used = -1, f_soft_limit = -1, grace = -1,
			f_hard_limit = -1, f_grace = -1;
//   char dummy[32];

	Bool_t ok = kFALSE;
	TRegexp ast("\\*");
	for (;;) {
		wline.ReadLine(wstream, kFALSE);
		if (wstream.eof()) {
			wstream.close();
			cout << "No quota active for: " << dir << endl;
			return -5;
		}
//      cout << wline << endl;
		if(ok){
//         cout << wline << endl;
//			Bool_t exceeded = kFALSE;

			if(wline.Contains("*")){
//				exceeded = kTRUE;
				while(wline.Index(ast) >= 0){
					wline(ast) = " ";
				}
			}
//  check quota
			TMrbString tline(wline);
			TMrbString value;
			TRegexp letter("[^0-9]");
			TObjArray lof;
			Int_t nitems = tline.Split(lof, " ", kTRUE);
	//      cout << "nitems " << nitems << endl;
			Int_t pos = 0;
			value = ((TObjString *)lof[pos++])->String();
			value.SplitOffInteger(value, used, 10);
			value = ((TObjString *)lof[pos++])->String();
			value.SplitOffInteger(value, soft_limit, 10);
			value = ((TObjString *)lof[pos++])->String();
			value.SplitOffInteger(value, hard_limit, 10);
	//     possible grace has a letter
			value = ((TObjString *)lof[pos++])->String();
	//     cout << value << endl;
			if(nitems > 6 && value.Index(letter)>= 0){
				value.Resize(value.Index(letter));
	//         cout << value << endl;
				 value.SplitOffInteger(value, grace, 10);
				value = ((TObjString *)lof[pos++])->String();
			}
			value.SplitOffInteger(value, f_used, 10);

			value = ((TObjString *)lof[pos++])->String();
			value.SplitOffInteger(value, f_soft_limit, 10);

			value = ((TObjString *)lof[pos++])->String();
			value.SplitOffInteger(value, f_hard_limit, 10);

			if(pos <= nitems && value.Index(letter)>= 0){
				value.Resize(value.Index(letter));
				cout << value << endl;
				value.SplitOffInteger(value, f_grace, 10);
			}

	//      line >> used >> soft_limit >> hard_limit;

			tot_size = (Double_t)(bsize) * (Double_t)hard_limit /(1000*1024);
			free_size = (Double_t)(hard_limit-used) * (Double_t)bsize /(1000*1024);

			if(loglev > 0 || free_size < warn_hwm){
				cout  << "Quota on: " << fs << endl
						<< "hard_limit: " << tot_size << endl
						<< "available : " << free_size << endl;
				if(grace > 0)
				cout << "grace left: " << setmagenta
					  << ((TObjString *)lof[3])->String() << setblack << endl;

				if(free_size < hard_hwm){
					cout << setred << "Fatal: quota below hard limit" << setblack << endl;
					return -1;
				} else if(free_size < warn_hwm){
					cout << setred << "Warning: disk quota low " << setblack << endl;
//					error = -2;
				}
			}
	//      if(exceeded)line >> dummy;
	  //    line >> f_used >> f_soft_limit >> f_hard_limit;
			if(loglev > 0 || f_used >= f_hard_limit){
				cout << "Files used: "<< f_used << endl
					  << "Hard_limit: " << f_hard_limit << endl;
			}
			if (f_used > 0) {
				if(f_used >= f_hard_limit){
					cout << setred << "Fatal: file limit exceeded" << setblack << endl;
					return -1;
				}
				if(grace > -1 && grace <= 1){
					cout << setred << "Fatal: no grace left" << grace << setblack << endl;
					return -1;
				}
				if(grace > -1 && grace <= 10){
					cout << setred << "Warning : grace low" << grace << setblack << endl;
					return -2;
				}
			}
		}
		if(wline.BeginsWith(((TObjString*)mountpoints.At(mpind))->GetString())){
			ok = kTRUE;
			cout << "Inspect: " <<
			((TObjString*)mountpoints.At(mpind))->GetString() << endl;
		}
		else  ok = kFALSE;
	}
}

//_____________________________________________________________________________________
//_____________________________________________________________________________________

FhMainFrame::FhMainFrame(const TGWindow *p, UInt_t w, UInt_t h,
								 Int_t attachid, Int_t attachsock)
				 : TGMainFrame(p, w, h), fYellowTextGC(TGButton::GetDefaultGC())
{

// if attachid > 0 try to attach to a running M_analyze with Id given on command line
// otherwise try to find a socket to communicate with M_analyze (9091-9095)
	fOurPidFile = new TString("");
	fAttachid = attachid;
	fAttachsock = attachsock;
	cout << "fAttachid " << fAttachid << endl;
//  on success fComSocket and  fOurPidFile will be defined

	fWidgets = new TList();
	fComSocket = NULL;
	fMbsControl = 0;
	fMbsDebug = kFALSE;
	fMessageServer = 0;
	fWasStarted = 0;
	fSetup = NULL;
	fC_Status = M_ABSENT;
	fM_Status = M_ABSENT;
	fM_OldStatus = M_ABSENT;
	fForcedStop = kFALSE;
	fAutoRestart   = kFALSE;
	fAutoRestartRT = kFALSE;

	fDefFile = new TString("C_analyze.def");
	if(!GetDefaults())cout << "something went wrong getting defaults" << endl;
	fM_Status = IsAnalyzeRunning(1);
	fC_Status = fM_Status;
	if(fM_Status != M_ABSENT){
		Int_t retval = kMBYes;
		new TGMsgBox(gClient->GetRoot(),this,
			"Question",
			"M_analyze already running, \n\
trying to attach?",
//         "Do you want to attach MBS",
			kMBIconQuestion, kMBYes | kMBNo, &retval);
		if(retval == kMBYes){
			fAttach = kTRUE;
		} else {
			TGMainFrame::CloseWindow();
			gApplication->Terminate(0);
		}

	} else fAttach = kFALSE;
	if (!gSystem->AccessPathName(fHelpFile->Data()))
	  fHelpBrowser = new TMrbHelpBrowser(fHelpFile->Data());

	fActualOutputFile = new TString("");
	fHistList = new TString("*");
	fStopwatch = new TStopwatch();
	fTotal_time_no_event = 0; fEvents_before = 0;
	fTotal_time_elapsed = 0;
	fTotal_livetime = 0;
	fWaitedForStop = 0;
	if(fSetup != NULL) delete fSetup;
	if (!gSystem->AccessPathName(".mbssetup")) {
		fSetup = new TMbsSetup(".mbssetup");
	} else {
		fSetup = new TMbsSetup();
	}

	gClient->GetColorByName("white", white);
	gClient->GetColorByName("blue", blue);
	gClient->GetColorByName("lightblue", lightblue);
	gClient->GetColorByName("antiquewhite1", antiquewhite1);
	gClient->GetColorByName("grey", grey);
	gClient->GetColorByName("green", green);
	gClient->GetColorByName("red",    red);
	gClient->GetColorByName("magenta", magenta);
	gClient->GetColorByName("cyan", cyan);
	gClient->GetColorByName("yellow", yellow);


	Pixel_t yellowp;
	fClient->GetColorByName("yellow", yellowp);
	fYellowTextGC.SetForeground(yellowp);

	fLO1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2);
	fLO2 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kLHintsExpandX|kRaisedFrame, 2, 2, 2, 2);
	fLO3 = new TGLayoutHints(kLHintsRight | kLHintsExpandX|kRaisedFrame, 2, 2, 2, 2);
	fLO4 = new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2);

//  ------------- menu bar at top
	fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
												  0, 0, 1, 1);
	fMenuBarItemLayout = new TGLayoutHints(kLHintsCenterY | kLHintsLeft|kRaisedFrame, 2, 2, 2, 2);
	fMenuBarHelpLayout = new TGLayoutHints(kLHintsCenterY | kLHintsRight);

//   fHFr     = new TGCompositeFrame(this, 100, 20, kHorizontalFrame|kRaisedFrame);

	fMenuFile = new TGPopupMenu(fClient->GetRoot());
	fMenuFile->AddEntry("Exit", M_QUIT);
	fMenuParameters = new TGPopupMenu(fClient->GetRoot());
	fMenuParameters->AddEntry("No Event Warning Time", M_NOEVENT);
	fMenuParameters->AddEntry("Maximum output file size", M_MAXFILESIZE);
	fMenuParameters->AddEntry("Enable automatic restart after max file size", M_AUTORESTART);
	if (fAutoRestart) fMenuParameters->CheckEntry( M_AUTORESTART);
	else              fMenuParameters->UnCheckEntry( M_AUTORESTART);

	fMenuParameters->AddEntry("Maximum run time (seconds)", M_MAXRUNTIME);
	fMenuParameters->AddEntry("Enable automatic restart after Maximum run time", M_AUTORESTART_RUNTIME);
	if (fAutoRestart) fMenuParameters->CheckEntry( M_AUTORESTART_RUNTIME);
	else              fMenuParameters->UnCheckEntry( M_AUTORESTART_RUNTIME);
	fMenuParameters->AddEntry("Time constant (seconds) in Avg Rate", M_AVERAGE);
	fMenuParameters->AddEntry("Disk space warn limit (Mbyte)", M_WARNHWM);
	fMenuParameters->AddEntry("Disk space hard limit (Mbyte)", M_HARDHWM);
	fMenuParameters->AddEntry("Check disk quota",              M_CHKQUOTA);
	fMenuParameters->AddEntry("Play sound at end of run", M_PLAYSOUND);
	if (fPlaySound) fMenuParameters->CheckEntry(M_PLAYSOUND );
	else            fMenuParameters->UnCheckEntry(M_PLAYSOUND);
	fMenuParameters->AddEntry("Sound file name", M_SOUNDFILE);
	fMenuParameters->AddEntry("Sound player", M_SOUNDPLAYER);
	fMenuParameters->AddEntry("C_analyse Verbose level", M_VERBLEV);
	fMenuParameters->AddEntry("Run M_analyze in debugger (0, 1, 2)", M_MADEBUG);

	fMenuParameters->AddSeparator();

	fMenuMbs = new TGPopupMenu(fClient->GetRoot());
	fMenuMbs->AddEntry("Gate length [units 100 ns]", M_GATEL);
	fMenuMbs->AddEntry("Mbs buffer size", M_BUFSIZE);
	fMenuMbs->AddEntry("Mbs buffers / stream", M_BUFSTREAM);
	fMenuMbs->AddSeparator();
	fMenuMbs->AddEntry("Name of readout code", M_CODENAME);
	fMenuMbs->AddEntry("Compile readout function", M_COMPILE);


	fMenuMbs->AddSeparator();
	fMenuMbs->AddEntry("Print Mbs Status", M_MBSSTATUS);
	fMenuMbs->AddEntry("Setup Mbs files on Lynx", M_MBSSETUP);
	fMenuMbs->AddEntry("Reload m_read_meb", M_RELOAD);
	fMenuMbs->AddEntry("Clear Mbs", M_CLEAR);
	fMenuMbs->AddSeparator();
	fMenuMbs->AddEntry("Execute Setup Mbs at Configure", M_AUTOSETUP);
	if(fAutoSetup)fMenuMbs->CheckEntry(M_AUTOSETUP);
	fMenuMbs->AddEntry("Mbs Log Level", M_MBSLOGLEVEL);
	fMenuMbs->AddEntry("Use m_read_meb debug version", M_DEBUG);

	fMenuHist = new TGPopupMenu(fClient->GetRoot());
	fMenuHist->AddEntry("Histogram Autosave Intervall", M_HSAVEINTERVALL);
	fMenuHist->AddEntry("Set Histogram Fill downscale factor", M_DOWNSCALE);
	fMenuHist->AddEntry("Reset all histograms", M_RESETALL);
	fMenuHist->AddEntry("Reset selected histograms", M_RESETSEL);
	fMenuHist->AddEntry("Histograms to be cleared", M_RESETLIST);

	fMenuEvent = new TGPopupMenu(fClient->GetRoot());
	fMenuEvent->AddEntry("No Event Warning Time", M_NOEVENT);
	fMenuEvent->AddSeparator();
	fMenuEvent->AddLabel("Following parameters are only used in replay");
	fMenuEvent->AddSeparator();
	fMenuEvent->AddEntry("Activate Eventnumber Selection", M_SELECTNUMBER);
	if(fSelectNumber)fMenuEvent->CheckEntry(M_SELECTNUMBER);
	else             fMenuEvent->UnCheckEntry(M_SELECTNUMBER);
	fMenuEvent->AddEntry("First Event (number)", M_STARTEVENT);
	fMenuEvent->AddEntry("Last  Event (number)", M_STOPEVENT);
	fMenuEvent->AddEntry("Activate Time Selection", M_SELECTTIME);
	if(fSelectTime)fMenuEvent->CheckEntry(M_SELECTTIME);
	else             fMenuEvent->UnCheckEntry(M_SELECTTIME);
	fMenuEvent->AddEntry("From (Time)",          M_FROMTIME);
	fMenuEvent->AddEntry("To   (Time)",          M_TOTIME);

	fMenuHelp = new TGPopupMenu(fClient->GetRoot());
	fMenuHelp->AddEntry("Help on C_analyze (old style)", M_HELPC);
	fMenuHelp->AddEntry("HelpBrowser", M_HELPBR);
	fMenuHelp->AddEntry("HelpBrowser name of html file", M_HELPNAME);
//   fMenuHelp->AddEntry("Help on C_analyze", M_HELPM);

	// Menu button messages are handled by the main frame (i.e. "this")
	// ProcessMessage() method.
	fMenuFile->Associate(this);
	fMenuParameters->Associate(this);
	fMenuMbs->Associate(this);
	fMenuHist->Associate(this);
	fMenuEvent->Associate(this);
	fMenuHelp->Associate(this);

	fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);

	fMenuBar->AddPopup("File", fMenuFile, fMenuBarItemLayout);
	fMenuBar->AddPopup("Parameters", fMenuParameters, fMenuBarItemLayout);
	fMenuBar->AddPopup("Mbs Control", fMenuMbs, fMenuBarItemLayout);
	fMenuBar->AddPopup("Histograms", fMenuHist, fMenuBarItemLayout);
	fMenuBar->AddPopup("Event Select", fMenuEvent, fMenuBarItemLayout);
	fMenuBar->AddPopup("Help", fMenuHelp, fMenuBarHelpLayout);
	fMenuBar->ChangeBackground(lightblue);

	this->AddFrame(fMenuBar, fMenuBarLayout);
//   fHFr->AddFrame(fMenuBar, fMenuBarItemLayout);
//   this->AddFrame(fHFr,fLO2);                // first line

//---------------------------
//  first line: Run number, Status,  Start time
	fHFr     = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);
//  Run number
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("RUN:"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//
	fTeRunNr   = new TGTextEntry(fHFr, fTbRunNr = new TGTextBuffer(100), M_RUNNR);
	fTbRunNr->AddText(0,fRunNr->Data());
	fTeRunNr->Resize(60, fTeRunNr->GetDefaultHeight());
	fHFr->AddFrame(fTeRunNr,fLO2);
	fTeRunNr->Associate(this);
	fTeRunNr->SetToolTipText(Form("$R -> %s", fRunNr->Data()));
//  Status label
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Status:"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fStatus = new TGLabel(fLabelFr, new TGString("Absent"));
	if(fM_Status == M_RUNNING)fStatus->SetText(new TGString("Running"));
	if(fM_Status == M_RUNNING)fStatus->ChangeBackground(green);
	else                      fStatus->ChangeBackground(white);
	fLabelFr->AddFrame(fStatus,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//  Start time
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Start:"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fStartTime = new TGLabel(fLabelFr, new TGString("0"));
	SetTime();
	fStartTime->ChangeBackground(white);
	fLabelFr->AddFrame(fStartTime,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//  Socket number
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Socket:"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fTbSockNr = new TGLabel(fLabelFr, new TGString("0"));
 //  SetTime();
	fTbSockNr->ChangeBackground(white);
	fLabelFr->AddFrame(fTbSockNr,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	this->AddFrame(fHFr,fLO1);                // first line

	fHFr     = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);
//  total events
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Events:"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fNev = new TGLabel(fLabelFr, new TGString("0"));
	fNev->SetTextJustify(kTextRight);
	fNev->ChangeBackground(white);
	fLabelFr->AddFrame(fNev,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//  runtime
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("RunTime"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fRunTime = new TGLabel(fLabelFr, new TGString("0"));
	fRunTime->SetTextJustify(kTextRight);
	fRunTime->ChangeBackground(white);
	fLabelFr->AddFrame(fRunTime,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//  rate
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("AvgRate"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fRate = new TGLabel(fLabelFr, new TGString("0"));
	fRate->ChangeBackground(white);
	fRate->SetTextJustify(kTextRight);
	fLabelFr->AddFrame(fRate,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
//  deadtime
	fLabelFr = new TGCompositeFrame(fHFr, 60, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Dead_T"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	fLabelFr = new TGCompositeFrame(fHFr, 150, 20, kHorizontalFrame);
	fDeadTime = new TGLabel(fLabelFr, new TGString("0"));
	fDeadTime->ChangeBackground(white);
	fDeadTime->SetTextJustify(kTextRight);
	fLabelFr->AddFrame(fDeadTime,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);

	this->AddFrame(fHFr,fLO1);                // second line

//  Label Mbs vers
	fHFr     = new TGCompositeFrame(this, 100, 10, kHorizontalFrame);
	fLabelFr = new TGCompositeFrame(fHFr, 150, 10, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString(
	"Definition of Input Source, Online: TcpIp, Offline: Fake, File or File List"));
	fLabel->ChangeBackground(antiquewhite1);
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
	this->AddFrame(fHFr,fLO1);                // third line
//  Input: source label: file tcpip
	fHFr     = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);

	fHFrHalf = new TGCompositeFrame(fHFr, 100, 20, kHorizontalFrame);
//   fLabelFr = new TGCompositeFrame(fHFrHalf, 150, 20, kHorizontalFrame);
//   fLabel  = new TGLabel(fLabelFr, new TGString("Input:"));
//   fLabelFr->AddFrame(fLabel,fLO2);
//   fHFrHalf->AddFrame(fLabelFr,fLO2);
	fHFrHalf->AddFrame(fRNet = new TGRadioButton(fHFrHalf, "TcpIp", R_NET), fLO2);
	fRNet->Associate(this);
	if(*fInputSource == "TcpIp")  fRNet->SetState(kButtonDown);

	fHFrHalf->AddFrame(fRFake = new TGRadioButton(fHFrHalf, "Fake", R_FAKE), fLO2);
	fRFake->Associate(this);
	if(*fInputSource == "Fake")  fRFake->SetState(kButtonDown);

	fHFrHalf->AddFrame(fRFileList = new TGRadioButton(fHFrHalf, "FileList", R_FILELIST),fLO2);
	fRFileList->Associate(this);
	if(*fInputSource == "FileList") fRFileList->SetState(kButtonDown);

	fHFrHalf->AddFrame(fRFile = new TGRadioButton(fHFrHalf, "File", R_FILE),fLO2);
	fRFile->Associate(this);
	if(*fInputSource == "File") fRFile->SetState(kButtonDown);

	fHFr->AddFrame(fHFrHalf,fLO2);
//

	fTeInputFile   = new TGTextEntry(fHFr, fTbInputFile = new TGTextBuffer(100), M_INPUT);
	fTbInputFile->Clear();
	if (*fInputSource == "TcpIp") {
		fTbInputFile->AddText(0, "not used");
		fTeInputFile->SetToolTipText("no input file possible");
	} else if (*fInputSource == "File" || *fInputSource == "FileList") {
		fTbInputFile->AddText(0, fInputFile->Data());
		fTeInputFile->SetToolTipText(Form("%s - will be expanded to %s", fInputFile->Data(), this->ExpandName(fActualInputFile, fInputFile->Data())));
	} else if (*fInputSource == "Fake") {
		fTbInputFile->AddText(0, "fake.root");
		fTeInputFile->SetToolTipText("fake.root");
	}
	fTeInputFile->Resize(fTeInputFile->GetDefaultWidth(), fTeInputFile->GetDefaultHeight());
	fHFr->AddFrame(fTeInputFile,fLO3);
	this->AddFrame(fHFr,fLO1);                // third line
	fTeInputFile->Associate(this);

//  Label Mbs setup
	fHFr     = new TGCompositeFrame(this, 100, 10, kHorizontalFrame);
	fLabelFr = new TGCompositeFrame(fHFr, 150, 10, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString(
	"Setup of Mbs System on Lynx Processors"));
	fLabel->ChangeBackground(antiquewhite1);
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
	this->AddFrame(fHFr,fLO1);                // third line

	fHFr     = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);

// user may define his own ppc names

	TString ppcNames = gEnv->GetValue("TMbsSetup.EventBuilder.Name", "");
	if (ppcNames.IsNull()) ppcNames = gEnv->GetValue("TMrbAnalyze.PPCNames", "");
	if (ppcNames.IsNull()) {
	  ppcNames = "";
	  Bool_t shortNames = gEnv->GetValue("TMrbAnalyze.UseShortPPCNames", kFALSE);
	  if (shortNames) {
		  for (Int_t n = 0; n < NOF_PPCS; n++) {
			  if (n != 0) ppcNames += ":";
			  ppcNames += Form("ppc-%d", n);
		  }
	  } else {
		  for (Int_t n = 1; n <= NOF_PPCS; n++) {
			  if (n > 1) ppcNames += ":";
			  ppcNames += Form("gar-ex-ppc%02d", n);
		  }
	  }
	}
	ppcArray = ppcNames.Tokenize(":");
	nofPPCs = ppcArray->GetEntriesFast();

//  Connect to
	fLabelFr = new TGCompositeFrame(fHFr, 50, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Connect to"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO4);

	fCbConnect = new TGComboBox(fHFr, C_MASTER);
	fHFr->AddFrame(fCbConnect,fLO4);
	for(Int_t k=0; k<nofPPCs; k++){
		fCbConnect->AddEntry(((TObjString *) ppcArray->At(k))->GetString(), k+1);
	}
	for (Int_t k = 0; k < nofPPCs; k++) {
	  if (!strcmp(fConnect->Data(), ((TObjString *) ppcArray->At(k))->GetString())) {
			fCbConnect->Select(k+1);
		}
	}
	fCbConnect->Resize(130, 20);
	fCbConnect->Associate(this);

//  Directory
	fLabelFr = new TGCompositeFrame(fHFr, 20, 20, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("Directory"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO4);

	fTeDir = new TGTextEntry(fHFr, fTbDir = new TGTextBuffer(50), M_DIR);
	fTbDir->AddText(0,fDir->Data());
	fTeDir->Resize(60, fTeDir->GetDefaultHeight());
	fHFr->AddFrame(fTeDir,fLO2);
	gClient->NeedRedraw(fTeDir);
	this->AddFrame(fHFr,fLO1);                // another line
	fTeDir->Associate(this);

// File for Histgrams
	fHFr     = new TGCompositeFrame(this, 100, 10, kHorizontalFrame);
	fLabelFr = new TGCompositeFrame(fHFr, 150, 10, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString("File for Histgrams"));
	fLabel->ChangeBackground(antiquewhite1);
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
	this->AddFrame(fHFr,fLO1);                // another  line

	fHFr     = new TGCompositeFrame(this, 100, 10, kHorizontalFrame);
	fHFrHalf = new TGCompositeFrame(fHFr, 100, 20, kHorizontalFrame);
	fLabelFr = new TGCompositeFrame(fHFrHalf, 150, 20, kHorizontalFrame);

	fSaveMapButton = new TGTextButton(fHFrHalf, "Save", M_SAVEHISTS, fYellowTextGC() );
	fSaveMapButton->ChangeBackground(blue);
	fSaveMapButton->Associate(this);
	fHFrHalf->AddFrame(fSaveMapButton, fLO1);
	fSaveMapButton->SetState(kButtonDisabled);

	fLabel  = new TGLabel(fLabelFr, new TGString("File for saved histos"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFrHalf->AddFrame(fLabelFr,fLO2);
	fHFr->AddFrame(fHFrHalf,fLO2);

	fTeHistFile   = new TGTextEntry(fHFr,fTbHistFile = new TGTextBuffer(100), M_HISTF);
	fTbHistFile->AddText(0,(const char *)*fHistFile);
	fTeHistFile->Resize(300, fTeHistFile->GetDefaultHeight());
	fHFr->AddFrame(fTeHistFile,fLO2);
	this->AddFrame(fHFr,fLO1);                //  output file line
	fTeHistFile->Associate(this);
	fTeHistFile->SetToolTipText(Form("%s - will be expanded to %s", fHistFile->Data(), this->ExpandName(fActualHistFile, fHistFile->Data())));

//  Output file
//  Label Output file
	fHFr     = new TGCompositeFrame(this, 100, 10, kHorizontalFrame);
	fLabelFr = new TGCompositeFrame(fHFr, 150, 10, kHorizontalFrame);
	fLabel      = new TGLabel(fLabelFr, new TGString(
	"Output file for events (in root format)"));
	fLabel->ChangeBackground(antiquewhite1);
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFr->AddFrame(fLabelFr,fLO2);
	this->AddFrame(fHFr,fLO1);                // third line
	fHFr = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);
	fHFrHalf = new TGCompositeFrame(fHFr, 100, 20, kHorizontalFrame);

	fLabelFr = new TGCompositeFrame(fHFrHalf, 150, 20, kHorizontalFrame);
	fLabel  = new TGLabel(fLabelFr, new TGString("Cur. Size"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFrHalf->AddFrame(fLabelFr,fLO2);
	fLabelFr = new TGCompositeFrame(fHFrHalf, 150, 20, kHorizontalFrame);

	fOutSize = new TGLabel(fLabelFr, new TGString("0"));
	fOutSize->ChangeBackground(white);
	fOutSize->SetTextJustify(kTextRight);
	fLabelFr->AddFrame(fOutSize,fLO2);
	fHFrHalf->AddFrame(fLabelFr,fLO2);

	fHFrHalf->AddFrame(fRActive =  new TGRadioButton(fHFrHalf, "en/disable", R_ACTIVE), fLO2);
	fHFr->AddFrame(fHFrHalf,fLO2);
	if(fWriteOutput) {
		fRActive->ChangeBackground(green);
		fRActive->SetState(kButtonDown);
		fRActive->SetToolTipText("Output file enabled");
	} else {
		fRActive->SetState(kButtonUp);
		fRActive->ChangeBackground(red);
		fRActive->SetToolTipText("Output file not enabled");
	}
	fRActive->Associate(this);

	fTeOutputFile   = new TGTextEntry(fHFr, fTbOutputFile = new TGTextBuffer(100), M_ROOTF);
	fTbOutputFile->AddText(0,(const char *)*fOutputFile);
	fTeOutputFile->Resize(300, fTeOutputFile->GetDefaultHeight());
	fHFr->AddFrame(fTeOutputFile,fLO2);
	this->AddFrame(fHFr,fLO1);                //  output file line
	fTeOutputFile->Associate(this);
	fTeOutputFile->SetToolTipText(Form("%s - will be expanded to %s", fOutputFile->Data(), this->ExpandName(fActualOutputFile, fOutputFile->Data())));
//  reload parameter file
	fHFr  = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
	fHFrHalf = new TGCompositeFrame(fHFr, 100, 20, kHorizontalFrame);
	fParButton = new TGTextButton( fHFrHalf,
					"Reload", M_LOADPAR, fYellowTextGC());
	fParButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fParButton->ChangeBackground(blue);
	fParButton->Associate(this);
	fHFrHalf->AddFrame(fParButton, fLO1);

	fLabelFr = new TGCompositeFrame(fHFrHalf, 150, 20, kHorizontalFrame);
	fLabel  = new TGLabel(fLabelFr, new TGString("Parameter file"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFrHalf->AddFrame(fLabelFr,fLO2);
	fHFr->AddFrame(fHFrHalf, fLO2);

	fTeParFile = new TGTextEntry(fHFr, fTbParFile = new TGTextBuffer(100), M_PARF);
	fTbParFile->AddText(0,(const char *)*fParFile);
	fTeParFile->Resize(300, fTeParFile->GetDefaultHeight());
	fHFr->AddFrame(fTeParFile, fLO2);
	this->AddFrame(fHFr,fLO1);
	fTeParFile->Associate(this);
	fTeParFile->SetToolTipText(Form("%s - will be expanded to %s", fParFile->Data(), this->ExpandName(fActualParFile, fParFile->Data())));
//  comment

	fHFr = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);
	fHFrHalf = new TGCompositeFrame(fHFr, 100, 20, kHorizontalFrame);
	fWhichHistButton = new TGTextButton( fHFrHalf,
					"Switch to DeadTime", M_WHICHHIST, fYellowTextGC());
	fWhichHistButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fWhichHistButton->ChangeBackground(blue);
	fWhichHistButton->Associate(this);
	fHFrHalf->AddFrame(fWhichHistButton, fLO1);

	fLabelFr = new TGCompositeFrame(fHFrHalf, 200, 20, kHorizontalFrame);
	fLabel  = new TGLabel(fLabelFr, new TGString("Comment"));
	fLabelFr->AddFrame(fLabel,fLO2);
	fHFrHalf->AddFrame(fLabelFr,fLO4);
	fHFr->AddFrame(fHFrHalf, fLO2);

	fTeComment   = new TGTextEntry(fHFr, fTbComment = new TGTextBuffer(100));
	fTbComment->AddText(0,(const char *)*fComment);
	fTeComment->Resize(450, fTeComment->GetDefaultHeight());
	fTeComment->SetToolTipText("short description");
	fHFr->AddFrame(fTeComment,fLO3);
	this->AddFrame(fHFr,fLO1);                //  output file line
//  rate histos
	fLO5      = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
									kLHintsExpandY, 5, 5, 5, 5);
	fHFr    = new TGCompositeFrame(this, 100, 20, kHorizontalFrame);
	fRateHist = new TRootEmbeddedCanvas("RateHist", fHFr, 100, 200);
	fHFr->AddFrame(fRateHist, fLO5);
	this->AddFrame(fHFr,fLO5);
	fRateHist->GetCanvas()->SetBorderMode(0);

//  action buttons

	fHFr  = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);

	fClearButton = new TGTextButton( fHFr, "Clear MBS", M_CLEAR, fYellowTextGC());
	fClearButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
//   if(!(*fInputSource) == "TcpIp")fStartStopButton->SetText(new TGHotString("NoOp"));
	fClearButton->ChangeBackground(blue);
	fClearButton->Associate(this);
	if(*fInputSource != "TcpIp")fClearButton->SetState(kButtonDisabled);
	fHFr->AddFrame(fClearButton, fLO1);

//   fMbsSetupButton = new TGTextButton( fHFr, "Mbs Setup", M_MBSSETUP, fYellowTextGC());
//   fMbsSetupButton->ChangeBackground(blue);
//   fMbsSetupButton->Associate(this);
//   fHFr->AddFrame(fMbsSetupButton, fLO1);

	fConfigButton = new TGTextButton( fHFr, "Configure", M_CONFIG, fYellowTextGC());
	fConfigButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fConfigButton->ChangeBackground(blue);
	fConfigButton->Associate(this);
	if(*fInputSource != "TcpIp")fConfigButton->SetState(kButtonDisabled);
	fHFr->AddFrame(fConfigButton, fLO1);


	if(fAttach)fStartStopButton =
		 new TGTextButton( fHFr, "Stop", M_START_STOP, fYellowTextGC());
	else fStartStopButton =
		 new TGTextButton( fHFr, "Start", M_START_STOP, fYellowTextGC());
	fStartStopButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fStartStopButton->ChangeBackground(blue);
	fStartStopButton->Associate(this);
	fHFr->AddFrame(fStartStopButton, fLO1);

	if(fM_Status == M_PAUSING) fPauseButton =
		 new TGTextButton( fHFr, "Resume", M_PAUSE, fYellowTextGC());
	else  fPauseButton =
		 new TGTextButton( fHFr, "Pause", M_PAUSE, fYellowTextGC());
	fPauseButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fPauseButton->ChangeBackground(blue);
	if( fM_Status != M_RUNNING && fM_Status != M_PAUSING)
		fPauseButton->SetState(kButtonDisabled);
	fPauseButton->Associate(this);
	fHFr->AddFrame(fPauseButton, fLO1);

	fResetButton = new TGTextButton( fHFr, "ResetHist", M_PAUSE_RESET, fYellowTextGC());
	fResetButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fResetButton->ChangeBackground(blue);
	fResetButton->Associate(this);
	fHFr->AddFrame(fResetButton, fLO1);

	fSaveSetupButton = new TGTextButton( fHFr, "Save Setup", M_SAVESETUP, fYellowTextGC());
	fSaveSetupButton->SetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
	fSaveSetupButton->ChangeBackground(blue);
	fSaveSetupButton->Associate(this);
	fHFr->AddFrame(fSaveSetupButton, fLO1);

	this->AddFrame(fHFr,fLO1);

	if(fM_Status == M_RUNNING || fM_Status == M_PAUSING)

//      fConfigButton->SetText(new TGHotString("Attach"));
		fConfigButton->SetState(kButtonDisabled);

	else {
		SetButtonsToDefaults();
	}
//   if(!fUseMap){
//      fPauseButton->SetState(kButtonDisabled);

//   }
	if(fAttach && *fInputSource == "TcpIp"){
		if(Configure()){
			cout << setgreen  << "Attach successful?" << setblack << endl;
		} else
			cout << setred  << "Attach failed"<< setblack << endl;
	}

	SetWindowName("Control Window for Mbs and M_analyze");
	MapSubwindows();
//   Resize(GetDefaultSize());
	Resize(700,700);

	MapWindow();
//   fTeComment->SetFocus();
/*
	ccc = fTeComment;

	Window_t wdum;
	int      ax, ay;

	gVirtualX->TranslateCoordinates(ccc->GetId(), ccc->GetParent()->GetId(),
			((TGFrame *) ccc)->GetWidth(),
			((TGFrame *) ccc)->GetHeight(),
								  ax, ay, wdum);
	cout << " ax, ay; " << ax << " " << ay << endl;
	gVirtualX->UpdateWindow(1);
	gVirtualX->Warp(ax, ay);
	gVirtualX->UpdateWindow(1);
*/
}
//_____________________________________________________________________________________
//
FhMainFrame::~FhMainFrame()
{
	fWidgets->Delete();
	delete fWidgets;
}
//_____________________________________________________________________________________
//
Int_t FhMainFrame::MessageToM_analyze(const char * mess) {

  if (fComSocket == 0){
	  cout << setred << "No connection to M_analyze open" << setblack << endl;
	  return -1;
  }
  Int_t nobs, retval;
  static TMessage *mess0;
// Open connection to server
//so   TSocket *sock = new TSocket("localhost", fComSocket);
	TSocket *sock = fComSocket;

// Wait till we get the start message
//  nobs= sock->Recv(mess0);
//   }
//  char str[32];
//  nobs = sock->Recv(str, 32);
  cout <<"MessageToM_analyze, send: " << mess << endl;
  sock->Send(mess);          // send message
  retval=0;

//  cout << "Send" << endl;

  while((nobs = sock->Recv(mess0))) // receive next message
  {
	 char *str0=new char[nobs];
	 mess0->ReadString(str0, nobs);
	 delete mess0;
	 if(!strncmp(str0, "ACK", 3))  // on ACK just exit
	 {
		if (fVerbLevel > 0) cout << str0 << endl;
		delete [] str0;
		break;
	}
	 if(!strncmp(str0,"ERROR",5))  // on error, print and exit
	 {
		if (fVerbLevel > 0) cout << str0 << endl;
		delete [] str0;
		break;
	 }
	 else retval=atoi(str0);
	 delete [] str0;
  }
  return retval;
}
//_____________________________________________________________________________________
//
// Stop message

void FhMainFrame::StopMessage(){
	ofstream wstream;
	const char logfile[] = "C_analyze.log";
	if(gSystem->AccessPathName(logfile)){
		wstream.open(logfile, ios::out);
		wstream << "RunNr Start    Stop      Events  Comment" << endl;
		wstream.close();
	}
	wstream.open(logfile, ios:: app);
	if (!wstream.good()) {
		cerr	<< "C_analyze: "
				<< gSystem->GetError() << " - " << logfile
				<< endl;
			return;
	}
	wstream << fRunNr->Data()         << "    " ;
	wstream << fStartTime->GetText()->GetString()  << " " ;
	SetTime();                                // update to current
	wstream << fStartTime->GetText()->GetString()  << "   " ;
	wstream << fNev->GetText()->GetString()  << "   " ;
	wstream << fTbComment->GetString();
	wstream << endl;
	wstream.close();
//
	fStartStopButton->SetText(new TGHotString("Start"));
	fPauseButton->SetText(new TGHotString("Pause"));
	*fOldRunNr = *fRunNr;
	if(fInputSource->Contains("TcpIp")){
//      istrstream s(fTbRunNr->GetString());
		Int_t irun = atoi(fTbRunNr->GetString());
//      s >> irun;
		irun ++;
		*fRunNr = Form("%03d", irun);
		fTbRunNr->Clear();
		fTbRunNr->AddText(0,*fRunNr);
		gClient->NeedRedraw(fTeRunNr);
	}
}

//_____________________________________________________________________________________

void FhMainFrame::SetButtonsToDefaults()
{
//   cout << "SetButtonsToDefaults, fM_Status " << fM_Status<< endl;
	fPauseButton->SetState(kButtonDisabled);
	fStartStopButton->SetText(new TGHotString("Start"));
	fPauseButton->SetText(new TGHotString("Pause"));

	if(*fInputSource == "TcpIp"){
		if(fM_Status != M_CONFIGURED){
			fStartStopButton->SetState(kButtonDisabled);
			fConfigButton->SetState(kButtonUp);
		} else {
			fStartStopButton->SetState(kButtonUp);
			fConfigButton->SetState(kButtonDisabled);
		}
		fClearButton->SetState(kButtonUp);
 //     fMbsSetupButton->SetState(kButtonUp);
	} else {
		fStartStopButton->SetState(kButtonUp);
		fConfigButton->SetState(kButtonDisabled);
		fClearButton->SetState(kButtonDisabled);
//      fMbsSetupButton->SetState(kButtonDisabled);
	}
}
//_____________________________________________________________________________________
//
void FhMainFrame::SetRadioButtons(Bool_t set)
{
	if(set){
		fRFile->SetState(kButtonUp);
		fRFileList->SetState(kButtonUp);
		fRNet->SetState(kButtonUp);
		if      (*fInputSource == "File")    fRFile->SetState(kButtonDown);
		else if(*fInputSource == "Fake")     fRFake->SetState(kButtonDown);
		else if(*fInputSource == "FileList") fRFileList->SetState(kButtonDown);
		else                                 fRNet->SetState(kButtonDown);
	} else {
		fRFile->SetState(kButtonDisabled);
		fRFake->SetState(kButtonDisabled);
		fRFileList->SetState(kButtonDisabled);
		fRNet->SetState(kButtonDisabled);
	}
}
//_____________________________________________________________________________________
//
void FhMainFrame::CloseWindow()
{
//   cout << "TGMainFrame::CloseWindow()" << endl;
	// Close dialog in response to window manager close.
	int buttons = kMBYes | kMBNo, retval=0;
	EMsgBoxIcon icontype = kMBIconQuestion;
	if(IsAnalyzeRunning(1) != M_ABSENT && IsAnalyzeRunning(1) != M_DIRTY){
		new TGMsgBox(gClient->GetRoot(),this,
		"Question",
		"You might loose control of Mbs, do still want to quit?",
		icontype,
		buttons, &retval);
	} else retval= kMBYes;
	if(retval == kMBYes){
		shutdown(6006, 2);
		if(fMbsControl)fMbsControl->DisConnectPrompter();
		PutDefaults();
//      CloseWindow();
		TGMainFrame::CloseWindow();
		gApplication->Terminate(0);
	}

//   TGMainFrame::CloseWindow();
//   gApplication->Terminate(0);
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::AskforRemove(const char * fname){
	int buttons = kMBYes | kMBNo, retval=0;
	TString warn = fname;
	warn += " already exists, delete it?";
	new TGMsgBox(gClient->GetRoot(),this,
	"Question", (const char *)warn,
	 kMBIconQuestion,
//   gClient->GetPicture("/home/rg/schaileo/myroot/xpm/warn1.xpm"),
	buttons, &retval);
	if(retval == kMBYes){
		TString RmCmd = "rm ";
		RmCmd += fname;
		gSystem->Exec((const char *)RmCmd);
		return kTRUE;
	} else return kFALSE;
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::CheckHostsUp(){
	Bool_t mok = CheckOneHostUp(((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString());
	return mok;
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::CheckOneHostUp(const char * nname)
{
	if(strlen(nname) <= 1)return kTRUE;
	TString pingcmd = "ping -c 2 -q ";
	pingcmd +=  nname;
	pingcmd += " 2>/dev/null >/dev/null";
	Int_t stat = gSystem->Exec(pingcmd);
	if ( stat != 0 ) {
		cout << setred;
		if(stat == 512)cout << "Unknown host: ";
		if(stat == 256)cout << "Cannot reach: ";
		cout  << nname << setblack << endl;
		WarnBox("Cant reach Lynx processors", this);
		return kFALSE;
	} else return kTRUE;
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::CheckParams()
{
	Bool_t ok = kTRUE;
//  does M_analyze exist in current dir
	if(gSystem->AccessPathName("M_analyze")){
		WarnBox("No M_analyze found in current working directory", this);
		return kFALSE;
	}
	fSockNr = GetComSocket(fAttachid, fAttachsock);
	if ( fSockNr <= 0 ) {
		cout << setred
			  << "Could not get free socket, check for already running M_analyze"
			  << setblack << endl;
		return kFALSE;
	}
	if (fAttachid > 0) {
		cout << "CheckParams: new TSocket(\"localhost\", " << fSockNr << ")" << endl;
		if (!gSystem->AccessPathName("localhost")) {
			cout << setred<< "Cant connect to \"localhost\" (remove file \"localhost\" first!)" << endl;
			return kFALSE;
		}
		fComSocket = new TSocket("localhost", fSockNr);
// Wait till we get the start message
		TMessage * mess;
		Int_t nobs= fComSocket->Recv(mess);
		if (mess->What() == kMESS_STRING) {
			char str[64];
			mess->ReadString(str, 64);
			cout << "CheckParams():" << nobs<< " received: " << str << endl;
			delete mess;
		}
		if (fComSocket->IsValid()) return kTRUE;
		else                       return kFALSE;
	}
	cout << "CheckParams: fSockNr " << fSockNr << endl;
	TString sn;
	sn += fSockNr;
	fTbSockNr->SetText(new TGString(sn.Data()));
 //   ok = CheckHostsUp();
	if(fRNet->GetState() == kButtonDown){
		if(!CheckHostsUp()) ok = kFALSE;
	} else {
		if(*fInputSource == "FileList"){
			TString temp = fTbInputFile->GetString();
			if(!temp.Contains(".list")){
				WarnBox("FileList must end with: .list", this);
				return kFALSE;
			}
		} else if (*fInputSource == "Fake") {
			gSystem->Exec("touch fake.root");
		}
	}
	TString fin;
	TString fout;
	*fRunNr = fTbRunNr->GetString();
	if(fRActive->GetState() == kButtonDown){
		fout = fTbOutputFile->GetString();
	  this->ExpandName(&fout, fout.Data());
	  fTeOutputFile->SetToolTipText(fout.Data());

		Int_t error = chkquota(fout, fHardHWM, fWarnHWM, fVerbLevel);
		if(error == -1)ok = kFALSE;
		if(error == -2)WarnBox("Filespace/quota low, watch out", this);

		fin = fTbInputFile->GetString();
	  this->ExpandName(&fin, fin.Data());
	  fTeInputFile->SetToolTipText(fin.Data());
		if(!strcmp(fin.Data(),fout.Data())){
		 this->ResetToolTips();
		 WarnBox("Input file == Output file", this);
			ok = kFALSE;
		} else {
			if(!gSystem->AccessPathName(fout.Data())){
				if(!AskforRemove(fout.Data())) {
				ok = kFALSE;
				this->ResetToolTips();
			}
			}
		}
	}
	if(fRFile->GetState() == kButtonDown){
		fin = fTbInputFile->GetString();
	  this->ExpandName(&fin, fin.Data());
	  fTeInputFile->SetToolTipText(fin.Data());
		if(gSystem->AccessPathName(fin.Data())){
			fin += " does not exist";
		 this->ResetToolTips();
			WarnBox((const char *)fin, this);
			ok = kFALSE;
		}
	}
	if(fSelectTime){
		if(!fFromTime->Contains(":") || !fToTime->Contains(":")){
			WarnBox("Selection by time active,\n\
values must contain at least one colon (:)", this);
			ok = kFALSE;
		}
	}
	if(fSelectNumber){
		if(fStartEvent > fStopEvent){
			WarnBox("Selection by event number active,\n\
last event < first event", this);
			ok = kFALSE;
		}
	}
	return ok;
}
//_____________________________________________________________________________________
Bool_t FhMainFrame::MbsStatus(){
	if(!fMbsControl) fMbsControl = new TMbsControl(((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString(),
												  fProcType->Data(),
												  fUseSSH,
																  fMbsVersion->Data(),
																  fTbDir->GetString());
	cout << sepline << endl;
	TDatime date;
	cout << setgreen<< "Mbs Status at: " << date.AsString()<< setblack << endl;
	cout << sepline << endl;
	Int_t nb=fMbsControl->GetNofMbsProcs();
	cout << "c_ana: Number of Mbs processes running: " << nb << endl;
	cout << sepline << endl;
	if(nb <= 0 || nb >= 1000 ) return kFALSE;
	TMbsNode * pn = fMbsControl->PrompterNode();
	if(pn){
		cout << "c_ana: Waiting for status from: " << pn->GetName() << endl;
		pn->GetStatus();
		cout << setgreen<< "c_ana: Status on Prompter Node: "<< pn->GetName() << setblack<< endl;
		cout << sepline << endl;
		cout << setw(12)<< setiosflags(ios::left) << "User: "  << pn->fUser << endl;
//      cout << setw(12)<< setiosflags(ios::left) << "fDaqDate: "  << pn->fDaqDate << endl;
//      cout << setw(12)<< setiosflags(ios::left) << "fRun: "  << pn->fRun << endl;
		cout << setw(12)<< setiosflags(ios::left) << "fRemote: "  << pn->fRemote << endl;
//      cout << setw(12)<< setiosflags(ios::left) << "fNode: "  << pn->fNode << endl;
		cout << setw(12)<< setiosflags(ios::left) << "fDisp: "  << pn->fDisp << endl;
		cout << setw(12)<< setiosflags(ios::left) << "fReadout: "  << pn->fReadout << endl;
		cout << setw(12)<< setiosflags(ios::left) << "fRemDir: "  << pn->fRemDir << endl;
//      cout << setw(12)<< setiosflags(ios::left) << "fStr: "  << pn->fStr << endl;
//      cout << setw(12)<< setiosflags(ios::left) << ": "  << pn-> << endl;
		cout << sepline << endl;
		cout << setw(12)<< setiosflags(ios::left)  << "Trigger: ";
		if( pn->iRun() == 0) cout << setred        << "disabled " ;
		else                 cout << setgreen      << "enabled " ;
		cout << setblack << endl;
		cout << setw(12)<< setiosflags(ios::left)  << "Events: "  << pn->laNevents() << endl;
		cout << setw(12)<< setiosflags(ios::left)  << "Buffers: " << pn->laNbuffers()<< endl;
		cout << setw(12)<< setiosflags(ios::left)  << "Streams: " << pn->laNstreams()<< endl;
		cout << setw(12)<< setiosflags(ios::left)  << "Kilobytes: " << pn->laNKbyte()<< endl;
		cout << sepline << endl;
//      pn->PrintHeader();
//      cout << sepline << endl;
	} else  cout << setred<< "c_ana: Prompter node not yet known"<< setblack << endl;
	cout << setgreen<<
	"~~~~~~~~~~~~~~Mbs Status done ~~~~~~~~~~~~~~~~" << setblack << endl;
	return kTRUE;
}
//_____________________________________________________________________________________

Bool_t FhMainFrame::Configure()
{
	fM_Status = IsAnalyzeRunning(1);
	if(fM_Status == M_ABSENT || fAttach){
		if(!CheckHostsUp())return kFALSE;
		gSystem->ProcessEvents();
		if(!fAttach && fAutoSetup && !MbsSetup()){
			fM_Status = M_DIRTY;
			return kFALSE;
		}
		if(!fAttach)fStatus->SetText(new TGString("Configuring"));
		else       fStatus->SetText(new TGString("Attaching"));
		fStatus->ChangeBackground(cyan);
		gClient->NeedRedraw(fStatus);
		gSystem->ProcessEvents();
		fC_Status = M_CONFIGURING;

		TString mproc =  ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
		if (mproc.IsNull()) {
			WarnBox("No master proc given", this);
		  return(kTRUE);
		}

		if(!fMbsControl) fMbsControl = new TMbsControl(mproc.Data(),
													 fProcType->Data(),
													 fUseSSH,
																	  fMbsVersion->Data(),
																	  fTbDir->GetString());
		cout << setblue<< "c_ana: Configure, check for running Mbs processes on Lynx"<< setblack << endl;
		Int_t nb=fMbsControl->GetNofMbsProcs();
//     look if Mbs processes already running
		if(nb >= 5 && nb < 100){
			TString buf("There are ");
			buf +=  nb;
			buf +=  " Mbs processes running \n Do you want to attach MBS";

			cout << setgreen<< "c_ana: Configure, nprocs = " << nb << setblack<< endl;
			Int_t retval = kMBYes;
			new TGMsgBox(gClient->GetRoot(),this,
			"Question",
			buf.Data(),
//         "Do you want to attach MBS",
			kMBIconQuestion, kMBYes | kMBNo, &retval);
			if(retval == kMBYes){
				if(fMbsControl->GetMbsNodes() < 2) return kFALSE;
				if(!fMbsControl->IdentifyMbsNodes()) return kFALSE;
				if(fAttach){
					fStartStopButton->SetText(new TGHotString("Stop"));
					fM_Status = IsAnalyzeRunning(0);
					if(fM_Status == M_RUNNING){
					  fStatus->SetText(new TGString("Running"));
					  fStatus->ChangeBackground(green);
					}
					if(fM_Status == M_PAUSING){
					  fStatus->SetText(new TGString("Pausing"));
					  fStatus->ChangeBackground(magenta);
					}
				} else {
					fM_Status = M_CONFIGURED;
					fStartStopButton->SetText(new TGHotString("Start"));
					cout << setgreen<< "c_ana: Ok ready to Start" << nb
						  << setblack<< endl;
				}
				fStartStopButton->SetState(kButtonUp);
				fConfigButton->SetState(kButtonDisabled);
				SetRadioButtons(kFALSE);
				nb = 0;
			  return kTRUE;
			}
		}
//
		if(nb > 0){
			cout << setred << "c_ana: There are still MBS processes running" << setblack<< endl;
		  if(nb > 1000) {
			  cout << setred << "c_ana: We are not owner of them" << setblack<< endl;
			  WarnBox("There are still MBS processes running, \n\
We are not owner of them",this);
		  } else WarnBox("There are still MBS processes running, \n\
Try Clear MBS",this);
			 fC_Status = M_ABSENT;
			 fM_Status = M_DIRTY;
			 return kFALSE;
		} else if(nb < 0){
//         cout << setred << "c_ana: Could not reach Lynx processors" << setblack<< endl;
			 WarnBox("Could not reach Lynx processors Please check hostnames",this);
			 fC_Status = M_ABSENT;
			 fM_Status = M_DIRTY;
			 delete fMbsControl; fMbsControl = 0;
			 return kFALSE;
		} else {
			TMrbNamedX * nx = fSetup->ReadoutProc(0)->TriggerModule()->GetTriggerMode();
			if(fMbsControl->StartMbs(nx->GetIndex())){
//    start Message server
			mproc =  ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
			if (mproc.IsNull()) {
				WarnBox("No master proc given", this);
			  return(kTRUE);
			}
				fMessageServer = new MessageServer(mproc.Data(),
										fMessageIntervall, kTRUE);
				fMessageServer->SetLogLevel(fMbsLogLevel);
			if(fMbsControl->Startup(fMbsDebug)) {
					if(fMbsControl->PrompterNode()){
//                  cout << setgreen << "c_ana: PrompterNode "
//                       << fMbsControl->PrompterNode()->GetName()<< setblack << endl;
						TString prnode(fMbsControl->PrompterNode()->GetName());
//                  prnode.Prepend("^");
//                 TRegexp rprnode(prnode);
						*fConnect = ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
						if(fConnect->Index(prnode, 0, TString::kIgnoreCase) != 0){
							cout << sepline << endl;
							cout << setred << "c_ana: Mbs Master node   " <<  fConnect->Data() << endl;
							cout << "c_ana: Mbs Prompter node " << fMbsControl->PrompterNode()->GetName() << endl;
							cout << "c_ana: should be the same" << setblack << endl;
							WarnBox("Inconsistent Lynx nodes, watch terminal output",this);
						} else cout << setgreen << "c_ana: Mbs Master == Prompter ok"<< setblack << endl;
					} else cout<< setred  << "c_ana: PrompterNode not known !!!!! "<< setblack << endl;
					fC_Status = M_CONFIGURED;
					fM_Status = M_CONFIGURED;
					SetRadioButtons(kFALSE);
					fStartStopButton->SetText(new TGHotString("Start"));
					fConfigButton->SetState(kButtonDisabled);
					fStartStopButton->SetState(kButtonUp);
					cout << setgreen<< "c_ana: Ok ready to Start" << nb << setblack<< endl;
					return kTRUE;
			  }
			} else {
//            cout << setred << "c_ana: Something went wrong when starting" << setblack<< endl;
				WarnBox("Something went wrong when starting MBS,         \n\
Please watch terminal output", this);
				fC_Status = M_ABSENT;
				fM_Status = M_DIRTY;
				return kFALSE;
			}
		}
	} else {
//      WarnBox("Only allowed when ABSENT", this);
		return kFALSE;
	}
	return kFALSE;
}
//_____________________________________________________________________________________

Bool_t FhMainFrame::MbsSetup()
{
	fM_Status = IsAnalyzeRunning(1);
	if(fM_Status != M_ABSENT && fM_Status != M_STOPPED && fM_Status != M_DIRTY){
		if(!confirm("Do you really want to run MbsSetup in this state?",
						 this))return kFALSE;
	}
	if(!CheckHostsUp()) return kFALSE;
	cout << "MbsSetup,  HostsUp ok, mod fStatus" << endl;
	fStatus->SetText(new TGString("Setting up"));
	fStatus->ChangeBackground(cyan);
	gClient->NeedRedraw(fStatus);
	gSystem->ProcessEvents();
	if(fSetup == NULL) {
		if (!gSystem->AccessPathName(".mbssetup")) {
			fSetup = new TMbsSetup(".mbssetup");
		} else {
			fSetup = new TMbsSetup();
		}
	 }

	TString mproc = ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
	TString sproc = mproc;

	fSetup->EvtBuilder()->SetProcName(mproc.Data()); // Char_t *, name des master ppcs

	TString remoteHome = fSetup->RemoteHomeDir();
	if(remoteHome.IsNull()) {
		WarnBox("No .rhosts on Lynx home directory", this);
		return kFALSE;
	} else {
			cout << "remoteHome " << remoteHome.Data() << endl;
	}

	fSetup->SetNofReadouts(1);				      // momentan nur 1 readout proc
	fSetup->SetPath(fTbDir->GetString());                 // Char_t *, relativer pfad zum homeDir am ppc

	fSetup->ReadoutProc(0)->SetProcName(sproc.Data());		// dasselbe fuer den readout
	if(!strcmp(mproc, sproc)){
		fSetup->SetMode(kModeSingleProc);
	} else {
		fSetup->SetMode(kModeMultiProc);
	}

	fSetup->ReadoutProc(0)->TriggerModule()->SetType(kTriggerModuleVME);
	fSetup->ReadoutProc(0)->TriggerModule()->SetConversionTime(fGateLength);

	Bool_t ok = fSetup->MakeSetupFiles();		// erfindet alle files

	if(fSetup) fSetup->Save();
	return ok;
}

//_____________________________________________________________________________________

Bool_t FhMainFrame::MbsCompile()
{
	if(!confirm("Do you really want recompile?", this))return kFALSE;
	if(fSetup == NULL) {
		if (!gSystem->AccessPathName(".mbssetup")) {
			fSetup = new TMbsSetup(".mbssetup");
		} else {
			fSetup = new TMbsSetup();
		}
	 }
	 fStatus->SetText(new TGString("Compiling"));
	 fStatus->ChangeBackground(cyan);
	 gClient->NeedRedraw(fStatus);
	 gSystem->ProcessEvents();
	 if(fCodeName->Length() > 0){
		 TMbsReadoutProc * rp = fSetup->ReadoutProc(0);
		 if(rp){
			rp->SetCodeName(fCodeName->Data());
			 rp->CopyMakefile();
			 rp->CompileReadout(fMbsVersion->Data());
		 } else {WarnBox("No ReadoutProc defined", this); return kFALSE;}
	 } else {WarnBox("No Makefile found to compile readout function", this); return kFALSE;}

	fSetup->Save();
	fStatus->SetText(new TGString("Absent"));
	fStatus->ChangeBackground(green);
	gClient->NeedRedraw(fStatus);
	gSystem->ProcessEvents();
	return kTRUE;
}
//_____________________________________________________________________________________

Bool_t FhMainFrame::ClearMbs()
{
	fM_Status = IsAnalyzeRunning(1);
	if(fM_Status != M_ABSENT && fM_Status != M_STOPPED && fM_Status != M_DIRTY){
		if(!confirm("Really Clear MBS in this state?",this))return kFALSE;
	}
	if(!CheckHostsUp())return kFALSE;
	if(!fMbsControl) fMbsControl = new TMbsControl( ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString(),
																 fProcType->Data(), fUseSSH, fMbsVersion->Data(),
																  fTbDir->GetString());
	fM_Status = M_ABSENT;
	if (fMessageServer) {
		fMessageServer->Disconnect();
		delete fMessageServer;
		fMessageServer = NULL;
	}
	fStartStopButton->SetState(kButtonDisabled);
	fPauseButton->SetState(kButtonDisabled);
	Int_t nb=fMbsControl->GetNofMbsProcs();
	if(nb < 0){
//      cout << setred << "c_ana: Could not reach Lynx processors, Please check hostnames etc" << setblack<< endl;
		 WarnBox("Could not reach Lynx processors\n\
Please check hostnames etc",this);
		 fM_Status = M_DIRTY;
		 delete fMbsControl; fMbsControl = 0;
		 return kFALSE;
	} else if(nb > 1000){
//      cout << setred << "c_ana: We are not owner of all MBS processes" << setblack<< endl;
		WarnBox("We are not owner of all MBS processes \n\
Please watch terminal output for owner,\n\
Lynx processors might need reboot", this);
		 fM_Status = M_DIRTY;
		 return kFALSE;
	} else if (nb > 0) {
		 fMbsControl->StopMbs();
		 cout << setblue
		 << "c_ana: Checking if there are still MBS processes alive"<< setblack << endl;
		 if(fMbsControl->GetNofMbsProcs() > 0){
//      cout << setred << "c_ana: There are still MBS processes running" << setblack<< endl;
			 WarnBox("There are still MBS processes running,\n\
You can try Clear MBS again,\n\
but Lynx processors might need reboot", this);
			 fM_Status = M_DIRTY;
			 return kFALSE;
		} else {
			cout << setgreen << endl<< "c_ana: Ok, all MBS processes disappeared"<< setblack << endl << endl;
		}
	} else cout << setgreen << "c_ana: No MBS processes were running"<< setblack  << endl;

	fM_Status = M_ABSENT;
	fConfigButton->SetState(kButtonUp);
	SetRadioButtons(kTRUE);
	return kTRUE;
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::StartDAQ()
{
	if ( CheckParams() ) {

		if (fAttachid > 0) return kTRUE;

		fStartStopButton->SetState(kButtonDisabled);

		fStatus->SetText(new TGString("Starting"));
		fStatus->ChangeBackground(cyan);
		gClient->NeedRedraw(fStatus);
		gSystem->ProcessEvents();
		fC_Status = M_STARTING;
		TString startCmd(" ");

	  if(*fInputSource == "File" || *fInputSource == "FileList"){
		 this->ExpandName(fActualInputFile, fTbInputFile->GetString());
		 fTeInputFile->SetToolTipText(fActualInputFile->Data());
			startCmd += fActualInputFile->Data();
			if(*fInputSource == "File") startCmd += " F ";
			else                        startCmd += " L ";
		} else if (*fInputSource == "Fake") {
			startCmd += "fake.root F ";
		} else {
			startCmd += ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
			startCmd +=  " S ";
		}
//     Start event, Stop Event | Start time , Stop time
		if ( *fInputSource == "Fake") {
			Int_t stopev = fStopEvent;
			if (stopev <= 0){
				cout << "Setting # of events to 50 in Fake mode" << endl;
				stopev = 50;
			}
			startCmd += "0 ";
			startCmd += stopev;
			startCmd += " ";
		} else {
			if     (fSelectTime) {
				startCmd += fFromTime->Data();
				startCmd += " ";
				startCmd += fToTime->Data();
				startCmd += " ";
			} else if (fSelectNumber) {
				startCmd += fStartEvent;
				startCmd += " ";
				startCmd += fStopEvent;
				startCmd += " ";
			} else  {
				startCmd += "0 0 ";
			}
		}
//     Run Number, downscale factor, output file, max file size
		startCmd += fTbRunNr->GetString();
		startCmd += " ";
		startCmd += fDownscale;
		startCmd += " ";
		if(fRActive->GetState() == kButtonDown){
		 this->ExpandName(fActualOutputFile, fTbOutputFile->GetString());
		 fTeOutputFile->SetToolTipText(fActualOutputFile->Data());
			startCmd += fActualOutputFile->Data();
			startCmd += " ";
			startCmd += fMaxFileSize;
			startCmd += " ";
		} else {
			startCmd += "none 0 ";
		}
//     parameter file, histogram file
	  this->ExpandName(fActualParFile, fTbParFile->GetString());
	  fTeParFile->SetToolTipText(fActualParFile->Data());
		if (fActualParFile->Length() > 0) {
			 startCmd += fActualParFile->Data();
			 startCmd += " ";
		} else {
			 startCmd += "none ";
		}
//      if(fRAuto->GetState() == kButtonDown){
	  this->ExpandName(fActualHistFile, fTbHistFile->GetString());
	  fTeHistFile->SetToolTipText(fActualHistFile->Data());
		startCmd += fActualHistFile->Data();
//		was memory mapped file
		startCmd += " none 0 ";
//     socket for communication
		startCmd += " ";
		startCmd += fSockNr;
		startCmd += " ";
		startCmd += fHsaveIntervall;

//      cout << "Startdaq, fSockNr " << fSockNr << endl;
//      cout << "startCmd: " << startCmd << endl;
		if (fDebugMode > 0){
			TString gdbCmd("run ");
			gdbCmd += startCmd.Data();
			ofstream gdbrc;
			gdbrc.open(".gdbinit", ios::out);
			if (fDebugMode > 1)gdbrc << "b main" << endl;
			gdbrc << gdbCmd.Data() << endl;
			gdbrc << "bt" << endl;
			gdbrc.close();
			startCmd = "gdb ./M_analyze";
			if (fDebugMode == 1) startCmd += " < /dev/null &";
		} else {
			startCmd.Prepend("./M_analyze");
			startCmd += "&";
		}
		if (fDebugMode > 0) {
			cout << setred << "c_ana: Starting M_analyze in debugger" << setblack << endl;
			cout << setmagenta << "See .gdbinit for Start Command" << setblack<< endl;
		} else {
			cout << setblue << "c_ana: Starting M_analyze"<< setblack << endl;
			cout << "Executing: "<< setmagenta <<  startCmd << setblack<< endl;
		}
//      cout << setcyan << endl;
//      cerr << setmagenta << endl;
//
//     here M_analyze is launched
		fM_OldStatus = -1;
		TDatime st;
		Long_t start_time= (Long_t)st.Convert();
		gSystem->Exec(startCmd.Data());

		fTotal_livetime = 0;
		SetTime();
		Int_t sleeptime = 500;
		for(Int_t i=0; i <= MAXTIMEOUT; i++){
			cout << setblue << "c_ana: Waiting for M_analyze to start"<< setblack << endl;
//         gSystem->Sleep(1000);
			fM_Status = IsAnalyzeRunning(1);
			if (fM_Status == M_DIED) {
				cout << setred << "c_ana: M_analyze seems to be dead" << endl;
				cout << "c_ana: Debug and recompile, then try again"
					  << setblack << endl;
				fM_Status = M_DIRTY;
				fStartStopButton->SetState(kButtonUp);
				return kFALSE;

			} else if(fM_Status == M_RUNNING){
				Bool_t ok = kTRUE;
				fForcedStop = kFALSE;
				if(*fInputSource == "TcpIp")ok = fMbsControl->StartAcquisition();
				if(ok) {
				  fStopwatch->Reset();
				  fStopwatch->Start();
				  fTotal_time_no_event = 0; fEvents_before = 0;
				  fStartStopButton->SetText(new TGHotString("Stop"));
				  fPauseButton->SetText(new TGHotString("Pause"));
				  fStartStopButton->SetState(kButtonUp);
				  fPauseButton->SetState(kButtonUp);
				  fSaveMapButton->SetState(kButtonUp);
				  fWasStarted = 1;
				  if (fSockNr > 0) {
retrysocket:
					  cout << "new TSocket(\"localhost\", " << fSockNr << ")" << endl;
					  if (!gSystem->AccessPathName("localhost")) {
							cout << setred<< "Cant connect to \"localhost\" (remove file \"localhost\" first!)" << endl;
							return kFALSE;
					  }
					  fComSocket = NULL;
					  fComSocket = new TSocket("localhost", fSockNr);
				// Wait till we get the start message
					  if (fComSocket && fComSocket->IsValid())
						  cout << "wait for: " << fSockNr << endl;
					  else {
						  cout << setred << "Invalid: " << fSockNr << " M_analyze already running??? " << setblack<< endl;
						  fComSocket->Close();
						  gSystem->Sleep(1000);
						  goto retrysocket;
					  }
					 // Wait till we get the start message
					  char str[32];
					  Int_t nobs= fComSocket->Recv(str,32);
					  cout << "StartDaq():" << nobs<< " received: "
					  << " mess "<< str << endl;
//					  gSystem->Sleep(500);   <<<<< sleeping
				  }

				  return kTRUE;
				} else {
//               cout << setred << "c_ana: MBS readout didnt start correctly" << setblack<< endl;
					WarnBox("MBS readout didnt start correctly",this);
					fC_Status = M_ABSENT;
					fM_Status = M_DIRTY;
					return kFALSE;
				}
			}
// may be we missed start - stop completely
// check if marabou.log is younger then start_time and
// contains end of run signal TUsrEvtXstop
			if ( i > 4 ) {
				Long_t  id, size, flags, modtime;
				gSystem->GetPathInfo("marabou.log", &id, &size, &flags, &modtime);
				if ( modtime - start_time > 0 && modtime - start_time < 10 ) {
					TString cmd ("tail -20 marabou.log");
					TString line;
					FILE *fp = gSystem->OpenPipe(cmd, "r");
					while ( ( line.Gets(fp) ) ) {
						if ( line.Contains("TUsrEvtXstop") ) {
							cout << setcyan << "Seems M_analyze already stopped" <<
									setblack << endl;
							fC_Status = M_ABSENT;
							fM_Status = M_ABSENT;
							gSystem->ClosePipe(fp);
							return kTRUE;
						}
					}
					gSystem->ClosePipe(fp);
				}
			}
			gSystem->Sleep(sleeptime);
			sleeptime += 500;
		}
		fC_Status = M_ABSENT;
//      cout << setred << "c_ana: M_analyze didnt start correctly" << setblack<< endl;
		WarnBox("M_analyze didnt start correctly",this);
		return kFALSE;
	} else {
//      cout << setred << "c_ana: Something is wrong with files, please check input parameters" << setblack<< endl;
		WarnBox("Something is wrong with files,\n\
Please check input parameters",this);
		fC_Status = M_ABSENT;
		return kFALSE;
	}
}
//_____________________________________________________________________________________
//
Bool_t FhMainFrame::StopDAQ(){

	fStartStopButton->SetState(kButtonDisabled);
	fStatus->SetText(new TGString("Stopping"));
	fStatus->ChangeBackground(cyan);
	gClient->NeedRedraw(fStatus);

	this->ResetToolTips();

	gSystem->ProcessEvents();
	fC_Status = M_STOPPING;
	if(*fInputSource == "TcpIp"){
		if(!fMbsControl->StopAcquisition()){
			WarnBox("MBS stop didnt work correctly",this);
			return kFALSE;
		}
	} else {
		MessageToM_analyze("M_client terminate");
	}
	for(Int_t i=0; i <= MAXTIMEOUT; i++){
		cout << setblue << "c_ana: Waiting for M_analyze to stop" << setblack<< endl;
		gSystem->Sleep(1000);
		fM_Status = IsAnalyzeRunning(0);
		if(fM_Status == M_ABSENT){
			if(*fInputSource == "TcpIp"){
			  cout << setblue << "c_ana: Reinitialize Mbs" << setblack<< endl;
			  if(fMbsControl->InitMbs()){
		if (gEnv->GetValue("TMrbAnalyze.ReloadReadoutOnStop", kFALSE)) {
			if(!fMbsControl->Reload(fMbsDebug)) WarnBox("Something went wrong with reload", this);
		}
					fM_Status = M_CONFIGURED;
					fPauseButton->SetState(kButtonDisabled);
					fStartStopButton->SetState(kButtonUp);
					fStartStopButton->SetText(new TGHotString("Start"));
					fC_Status = M_ABSENT;
					StopMessage();
					cout << setgreen << "c_ana: Mbs ok" << setblack<< endl;
				  return kTRUE;
				} else {
//               cout << setred << "c_ana: Reinitialize Mbs failed" << setblack<< endl;
					WarnBox("Reinitialize Mbs failed",this);
					fM_Status = M_DIRTY;
					fC_Status = M_ABSENT;
					return kFALSE;
				}
			} else {
//            StopMessage();
				return kTRUE;
			}
		}
	}
	WarnBox("M_analyze didnt stop correctly,\n\
proceed on your own risk",this);
	if(*fInputSource == "TcpIp"){
		fM_Status = M_CONFIGURED;
		fC_Status = M_CONFIGURED;
	} else {
		fM_Status = M_ABSENT;
		fC_Status = M_ABSENT;
	}
	return kFALSE;
}
//_____________________________________________________________________________________
Bool_t RemoveBlanks(TGTextEntry * te){
	TString ts(te->GetText());
	if(ts.Contains(" ")){
		TRegexp bl(" ");
		while(ts.Contains(" ")){
			ts(bl) = "";
		}
		te->SetText(ts.Data());
		return kTRUE;
	} else return kFALSE;
}

//_____________________________________________________________________________________

Bool_t FhMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t /*parm2*/)
{
	// Process messages sent to this dialog.
//   int buttons= kMBOk | kMBDismiss, retval=0;
//   EMsgBoxIcon icontype = kMBIconQuestion;
//   cout << "ProcessMessage: " << msg << " " << parm1 << " " << parm2 << endl;
	TString dmy;
	Bool_t ok;
	switch(GET_MSG(msg)) {
		case kC_TEXTENTRY:
			switch (GET_SUBMSG(msg)) {
				case kTE_TEXTCHANGED:
					switch (parm1) {
						case M_INPUT:
					  if(RemoveBlanks(fTeInputFile)) cout << setred << "Removed blanks from: " << fTeInputFile->GetText() << setblack << endl;
					  *fInputFile = fTeInputFile->GetText();
					  fTeInputFile->SetToolTipText(Form("%s - will be expanded to %s", fInputFile->Data(), this->ExpandName(&dmy, fInputFile->Data())));
					  break;
						case M_ROOTF:
					  if(RemoveBlanks(fTeOutputFile)) cout << setred << "Removed blanks from: " << fTeOutputFile->GetText() << setblack << endl;
					  *fOutputFile = fTeOutputFile->GetText();
					  fTeOutputFile->SetToolTipText(Form("%s - will be expanded to %s", fOutputFile->Data(), this->ExpandName(&dmy, fOutputFile->Data())));
					  break;
						case M_HISTF:
					  if(RemoveBlanks(fTeHistFile)) cout << setred << "Removed blanks from: " << fTeHistFile->GetText() << setblack << endl;
					  *fHistFile = fTeHistFile->GetText();
					  fTeHistFile->SetToolTipText(Form("%s - will be expanded to %s", fHistFile->Data(), this->ExpandName(&dmy, fHistFile->Data())));
					  break;
						case M_RUNNR:
					  if(RemoveBlanks(fTeRunNr)) cout << setred << "Removed blanks from: " << fTeRunNr->GetText() << setblack << endl;
					  *fRunNr = fTeRunNr->GetText();
					  fTeRunNr->SetToolTipText(Form("$R -> %s", fRunNr->Data()));
					  this->ResetToolTips();
							break;
						case M_PARF:
					  if(RemoveBlanks(fTeParFile)) cout << setred << "Removed blanks from: " << fTeParFile->GetText() << setblack << endl;
					  *fParFile = fTeParFile->GetText();
					  fTeParFile->SetToolTipText(Form("%s - will be expanded to %s", fParFile->Data(), this->ExpandName(&dmy, fParFile->Data())));
					  break;
						case M_DIR:
							if(RemoveBlanks(fTeDir)) cout << setred << "Removed blanks from: " << fTeDir->GetText() << setblack << endl;
							break;
					}
					break;
				case kTE_ENTER:
					switch (parm1) {
						case M_RUNNR:
					  cout << "ENTER: " << fTeRunNr->GetText() << endl;
					  *fRunNr = fTeRunNr->GetText();
					  fTeRunNr->SetToolTipText(Form("$R -> %s", fRunNr->Data()));
					  this->ResetToolTips();
					  break;
						case  M_INPUT:
					  cout << "ENTER: " << fTeInputFile->GetText() << endl;
					  *fInputFile = fTeInputFile->GetText();
					  fTeInputFile->SetToolTipText(Form("%s - will be expanded to %s", fInputFile->Data(), this->ExpandName(&dmy, fInputFile->Data())));
					  break;
					}
					break;
			}
			break;
		case kC_COMMAND:
			switch(GET_SUBMSG(msg)) {
			  case kCM_MENU:
					switch(parm1) {
						case M_QUIT:
					  this->CloseWindow();
						break;
						case M_HELPC:
							new TGMrbHelpWindow(this, "Help on C_analyze...",
														Help_text, 550, 600);
						break;
						case M_HELPBR:
							if (fHelpBrowser) fHelpBrowser->DrawText("C_analyze.html");
						break;
						case M_HELPNAME:
							{
							*fHelpFile = GetString("HelpFile (html) name",fHelpFile->Data(), &ok, this);
							}
						break;
						case M_HSAVEINTERVALL:
							fHsaveIntervall = GetInteger("Histo Autosave Intervall", fHsaveIntervall, &ok, this);
							break;
						case M_DOWNSCALE:
							 if(fM_Status == M_RUNNING){
								  WarnBox("Not allowed during RUNNING", this);
							 } else {
								 TString DownscaleCmd = "M_client downscale ";
								  const char help_DOWNSCALE[] =
"To save time it can be useful to fill\n\
selected histograms only for a (random)\n\
subset of events. This feature must be \n\
implemented in the Analysis code.\n\
The value is available from \n\
gMrbAnalyze->GetScaleDown()";
								 Int_t bs = GetInteger("Hist Fill Downscale factor",
								 fDownscale, &ok, this, (const char *)0,
											(Bool_t*)0, help_DOWNSCALE);
								 if(!ok) break;
								 if(bs <= 0) {WarnBox("Illegal number", this); break;}
								 else        fDownscale = bs;
								 DownscaleCmd += fDownscale;
								 cout << DownscaleCmd << endl;
								 if(fM_Status == M_PAUSING)
									 MessageToM_analyze(DownscaleCmd.Data());
							 }
							 break;
						case M_RESETALL:
//                      {
							 {
							 if(fM_Status == M_PAUSING) MessageToM_analyze("M_client zero *");
							 else   WarnBox("Only allowed when PAUSING", this);
							 }
							 break;
						case M_RESETSEL:
//                      {
							 {
							 TString ResetCmd = "M_client zero ";
							 ResetCmd += GetString("Selection",fResetList->Data(), &ok, this);
							 cout << ResetCmd << endl;
							 if(fM_Status == M_PAUSING) MessageToM_analyze(ResetCmd.Data());
							 else   WarnBox("Only allowed when PAUSING", this);
							 }
							 break;
						case M_RESETLIST:
//                      {
							 {
							 TString temp;
							 temp = GetString("Select Hists",fResetList->Data(), &ok, this);
							 if(ok)*fResetList  = temp;
							 }
							 break;
						case M_NOEVENT:
							 {
							 const char help_NOEVENT[] =
"This selects the maximum time (seconds) with \n\
no event arriving until a warning is issued";
							 Int_t bs = GetInteger("Max time No event allowed",
							 (Int_t)fMax_time_no_event, &ok, this, (const char *)0,
											(Bool_t*)0, help_NOEVENT);
							 if(!ok) break;
							 if(bs <= 0) WarnBox("Illegal number", this);
							 else        fMax_time_no_event = bs;
							 }
							 break;
						case M_BUFSIZE:
							 {
							 Int_t bs = GetInteger("Buffer size", fBufSize, &ok, this);
							 if(!ok) break;
							 if(bs <= 0) WarnBox("Illegal number", this);
							 else        fBufSize = bs;
							 }
							 break;
						case M_BUFSTREAM:
							 {
							 Int_t bs = GetInteger("Buffers/ stream", fBuffers, &ok, this);
							 if(!ok) break;
							 if(bs <= 0) WarnBox("Illegal number", this);
							 else        fBuffers = bs;
							 }
							 break;
						case M_GATEL:
							 {
							 const char help_GATEL[] =
"Readout starts only after this time to allow\n\
for conversion times of hardware.\n\
Note: Unit is 100 ns for historical reasons";
							 Int_t bs = GetInteger("Gate length [100 ns]",
											fGateLength , &ok, this, (const char *)0,
											(Bool_t*)0, help_GATEL);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else        fGateLength = bs;
							 }
							 break;
						case M_CODENAME:
							*fCodeName=GetString("Name of readout code",
																	fCodeName->Data());
							break;
					  case M_COMPILE:
							MbsCompile();
							break;

						case M_MBSSTATUS:
							if(*fInputSource == "TcpIp")ok = MbsStatus();
							else WarnBox("Only possible with TcpIp as Input",this);
							break;

						case M_MBSSETUP:
							if(*fInputSource == "TcpIp"){
								if(!MbsSetup()){
									WarnBox("Error from MbsSetup", this);
									fStatus->SetText(new TGString("Setup failed"));
									fStatus->ChangeBackground(red);
								} else {
									fStatus->SetText(new TGString("Setup done"));
									fStatus->ChangeBackground(green);
								}
								gClient->NeedRedraw(fStatus);
								gSystem->ProcessEvents();
							}
							else WarnBox("Only possible with TcpIp as Input",this);
							break;
						case M_RELOAD:
							if(*fInputSource == "TcpIp"){
								fM_Status = IsAnalyzeRunning(0);
								if(fM_Status != M_CONFIGURED || fMbsControl == NULL){
									WarnBox("Mbs is not configured, cant reload", this);
								} else {
									if(!fMbsControl->Reload(fMbsDebug))
									WarnBox("Something went wrong with reload", this);
								}
							}
							else WarnBox("Only possible with TcpIp as Input",this);
							break;

						case M_CLEAR:
							if(*fInputSource == "TcpIp")ok = ClearMbs();
							else WarnBox("Only possible with TcpIp as Input",this);
							break;

						case M_STARTEVENT:
							 {
							 Int_t bs = GetInteger("First event to process", fStartEvent, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else        fStartEvent = bs;
							 }
							 break;
						case M_AVERAGE:
							 {
							 Int_t bs = GetInteger(
							 "Time constant (seconds) in Avg Rate", fAverage, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else        fAverage = bs;
							 }
							 break;
						case M_MBSLOGLEVEL:
							 {
							 Int_t bs = GetInteger("Mbs Log Level (0,1,2)",
											 fMbsLogLevel, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else {     fMbsLogLevel = bs;
											if(fMessageServer)
											fMessageServer->SetLogLevel(fMbsLogLevel);
							 }
							 }
							 break;
						case M_MAXFILESIZE:
							 {
							 Int_t bs = GetInteger("Maximum output file size(Mbyte)",
											 fMaxFileSize, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else       fMaxFileSize = bs;
							 }
							 break;
						case M_MAXRUNTIME:
							 {
							 Int_t bs = GetInteger("Maximum Runtime (Seconds)",
											 fMaxRunTime, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else       fMaxRunTime = bs;
							 }
							 break;
				  case M_AUTORESTART:
							 if (fAutoRestart) {
								 fMenuParameters->UnCheckEntry(M_AUTORESTART);
								 fAutoRestart = kFALSE;
							 } else {
								 fMenuParameters->CheckEntry(M_AUTORESTART);
								 fAutoRestart = kTRUE;
							 }
							 break;
				  case M_AUTORESTART_RUNTIME:
							 if (fAutoRestartRT) {
								 fMenuParameters->UnCheckEntry(M_AUTORESTART_RUNTIME);
								 fAutoRestartRT = kFALSE;
							 } else {
								 fMenuParameters->CheckEntry(M_AUTORESTART_RUNTIME);
								 fAutoRestartRT = kTRUE;
							 }
							 break;
						case M_WARNHWM:
							 {
							 Int_t bs = GetInteger("Disk space warn limit(Mbyte)",
											 fWarnHWM, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else       fWarnHWM = bs;
							 }
							 break;
						case M_HARDHWM:
							 {
							 Int_t bs = GetInteger("Disk space warn limit(Mbyte)",
											 fHardHWM, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else       fHardHWM = bs;
							 }
							 break;
						 case M_CHKQUOTA:
							 {
								chkquota(fTbOutputFile->GetString(), fHardHWM, fWarnHWM, fVerbLevel);
							 }
							 break;
				  case M_PLAYSOUND:
							 {
							 TEnv env(".rootrc");
							 if (fPlaySound) {
								 fMenuParameters->UnCheckEntry(M_PLAYSOUND);
								 fPlaySound = 0;
								 env.SetValue("M_analyze.PlaySound", 0);
							 } else {
								 fMenuParameters->CheckEntry(M_PLAYSOUND);
								 fPlaySound = 1;
								 env.SetValue("M_analyze.PlaySound", 1);
								 *fSoundFile = env.GetValue("M_analyze.EndOfRunSound", "$MARABOU/sounds/Ende_der_Seereise.wav");
								 TString expanded_name(fSoundFile->Data());
								 if (expanded_name.BeginsWith("$")) {
									 Int_t indslash = expanded_name.Index("/");
									 TString var = expanded_name(1,indslash-1);
									 expanded_name.Remove(0,indslash);
									 expanded_name.Prepend(gSystem->Getenv(var.Data()));
								 }
								 if (!gSystem->AccessPathName(expanded_name.Data(), kFileExists)) {
									 if (!gSystem->AccessPathName(fSoundPlayer->Data(), kFileExists)) {
										 TString scmd(*fSoundPlayer);
										 scmd += " ";
										 scmd += expanded_name.Data();
										 gSystem->Exec(scmd);
									 } else {
										 cout << "SoundPlayer " << *fSoundPlayer << " not found" <<endl;
									 }
								 } else {
									 cout << "SoundFile " << expanded_name << " not found" <<endl;
								 }
							 env.SaveLevel(kEnvLocal);
							 }
							 }
							 break;
						case M_SOUNDFILE:
							{
							TEnv env(".rootrc");
							*fSoundFile = env.GetValue("M_analyze.EndOfRunSound", "$MARABOU/sounds/Ende_der_Seereise.wav");
							*fSoundFile = GetString("Sound file name",fSoundFile->Data(), &ok, this);
							 env.SetValue("M_analyze.EndOfRunSound", fSoundFile->Data());
							 env.SaveLevel(kEnvLocal);
							}
							break;
						case M_SOUNDPLAYER:
							{
							TEnv env(".rootrc");
							*fSoundPlayer = env.GetValue("M_analyze.SoundPlayer", "/usr/bin/play");
							*fSoundPlayer = GetString("Sound player name",fSoundPlayer->Data(), &ok, this);
							 env.SetValue("M_analyze.SoundPlayer", fSoundPlayer->Data());
							 env.SaveLevel(kEnvLocal);
							}
							break;
						 case M_VERBLEV:
							 {
							 Int_t bs = GetInteger("C_analyze verbose level",
											 fVerbLevel, &ok, this);
							 if(!ok) break;
							 fVerbLevel = bs;
							 }
							 break;
						 case M_MADEBUG:
							 {
							  const char help_MADEBUG[] =
"Run M_analyze in debugger (gdb)\n\
0:  no debugger\n\
1:  just run with gdb, no interaction with gdb possible, \n\
	 produce backtrace incase of error, \n\
	 communication with C_analyze as normal.\n\
\n\
2:  put breakpoint at main(), i.e. at start of program,\n\
	 here more breakpoints may be set, \n\
	 normal dialog with gdb possible.\n\
	 However no communication with C_analyze possible.";

							Int_t bs = GetInteger("M_analyze debug mode",
											 fDebugMode, &ok, this,
											 NULL, (Bool_t*)NULL, help_MADEBUG);
							 if(!ok) break;
							 fDebugMode = bs;
							 }
							 break;
					  case M_STOPEVENT:
							 {
							 Int_t bs = GetInteger("Last event to process",fStopEvent, &ok, this);
							 if(!ok) break;
							 if(bs < 0) WarnBox("Illegal number", this);
							 else        fStopEvent = bs;
							 }
							 break;
						case M_FROMTIME:
							 *fFromTime = GetString("Start time", fFromTime->Data(), &ok, this);
							 break;
						case M_TOTIME:
							 *fToTime   = GetString("Stop time", fToTime->Data(), &ok, this);
							break;
						case M_SELECTNUMBER:
							if(fSelectNumber){
								fSelectNumber = kFALSE;
								fMenuEvent->UnCheckEntry(M_SELECTNUMBER);
							} else {
								fSelectNumber = kTRUE;
								fMenuEvent->CheckEntry(M_SELECTNUMBER);
								if(fSelectTime){
									cout << "Warning: Unset time selection" << endl;
									fSelectTime = kFALSE;
									fMenuEvent->UnCheckEntry(M_SELECTTIME);
								}
							}
							break;
						case M_SELECTTIME:
							if(fSelectTime){
								fSelectTime = kFALSE;
								fMenuEvent->UnCheckEntry(M_SELECTTIME);
							} else {
								fSelectTime = kTRUE;
								fMenuEvent->CheckEntry(M_SELECTTIME);
								if(fSelectNumber){
									cout << "Warning: Unset event number selection" << endl;
									fSelectNumber = kFALSE;
									fMenuEvent->UnCheckEntry(M_SELECTNUMBER);
								}
							}
							break;
						case M_AUTOSETUP:
							if(fAutoSetup){
								fAutoSetup = kFALSE;
								fMenuMbs->UnCheckEntry(M_AUTOSETUP);
							} else {
								fAutoSetup = kTRUE;
								fMenuMbs->CheckEntry(M_AUTOSETUP);
							}
							break;
						case M_DEBUG:
							if(fMbsDebug){
								fMbsDebug = kFALSE;
								fMenuMbs->UnCheckEntry(M_DEBUG);
							} else {
								fMbsDebug = kTRUE;
								fMenuMbs->CheckEntry(M_DEBUG);
							}
							break;

					}
			  case kCM_BUTTON:
					switch(parm1) {

						case M_CLEAR:
							if(*fInputSource == "TcpIp")ok = ClearMbs();
							else WarnBox("Only possible with TcpIp as Input",this);
							break;

						case M_CONFIG:
							if(*fInputSource == "TcpIp")ok = Configure();
							else WarnBox("Only possible with TcpIp as Input",this);
							break;
						case M_START_STOP:
							fM_Status = IsAnalyzeRunning(1);

							if(fM_Status == M_PAUSING){
								 WarnBox("Only allowed when running, please Resume first",this);
							} else  if ( fM_Status == M_RUNNING ){
								StopDAQ();
							} else if ( *fInputSource == "File"
										||*fInputSource == "FileList"
										||*fInputSource == "Fake"
									  || fM_Status == M_CONFIGURED ){

								this->StartDAQ();

							} else {
								cout << setred << "Operation not allowed" << setblack<< endl;
							}
//                     break;
//                  case M_STOP:
							break;

						case M_SAVESETUP:
							cout << "Saving setup" << endl;
							PutDefaults();
					 break;

						case M_PAUSE:
							{
							fM_Status = IsAnalyzeRunning(1);
							if(fM_Status == M_PAUSING || fM_Status == M_RUNNING){
								TString PauseCmd = "M_client ";
								if(fM_Status == M_PAUSING){
									fM_Status=M_RUNNING;
									fC_Status=M_RUNNING;
									PauseCmd  += "resume";
									MessageToM_analyze(PauseCmd.Data());
									fPauseButton->SetText(new TGHotString("Pause"));
								} else {
									if(fC_Status == M_PAUSING){
										WarnBox("Obviously no event arrived since pause\n\
force Resume", this);

										fM_Status=M_RUNNING;
										fC_Status=M_RUNNING;
										PauseCmd += "resume";
										fPauseButton->SetText(new TGHotString("Pause"));
									}  else {
										fM_Status=M_PAUSING;
										fC_Status=M_PAUSING;
										PauseCmd += "pause";
										fPauseButton->SetText(new TGHotString("Resume"));
									}
									MessageToM_analyze(PauseCmd.Data());
								}
								break;
							} else {
								WarnBox("M_analyze is not running", this);
							}
							}
							break;
						case M_PAUSE_RESET:
							{
							fM_Status = IsAnalyzeRunning(0);
							if (fM_Status != M_RUNNING &&  fM_Status != M_PAUSING) {
								 WarnBox("Not possible in this state", this);
								 break;
							} else {
								Bool_t was_running = kFALSE;
								if(fM_Status == M_RUNNING){
									was_running = kTRUE;
									MessageToM_analyze("M_client pause");
								}
								TString ResetCmd = "M_client zero ";
								ResetCmd += fResetList->Data();
								cout << ResetCmd << endl;
								MessageToM_analyze(ResetCmd.Data());
								if(was_running){
									MessageToM_analyze("M_client resume");
								}
							}
							}
							break;

						case M_SAVEHISTS:
							 fM_Status = IsAnalyzeRunning(0);
							 if (fM_Status == M_STOPPED || fM_Status == M_ABSENT
								 || fM_Status == M_PAUSING || fM_Status == M_CONFIGURED) {
								 MessageToM_analyze("M_client savehists");
							 } else {
//                         WarnBox("Wait until absent, paused or configured", this);
								 Bool_t was_running = kFALSE;
								 if(fM_Status == M_RUNNING){
									 was_running = kTRUE;
									 MessageToM_analyze("M_client pause");
								 } else {
									 MessageToM_analyze("M_client savehists");
								 }
								 if(was_running){
									 MessageToM_analyze("M_client resume");
								 }
							 }
							 break;
						case M_LOADPAR:
							 fM_Status = IsAnalyzeRunning(0);
							 if(fM_Status == M_PAUSING){
								 TString LoadCmd = "M_client reload ";
								 LoadCmd = LoadCmd
												+ fTbParFile->GetString();
								 cout << setmagenta << LoadCmd << setblack<< endl;
								 MessageToM_analyze(LoadCmd.Data());

							 } else WarnBox("Only allowed when Pausing", this);
							 break;
						case M_HELP:
							new TGMrbHelpWindow(this, "Help on C_analyze...",
														Help_text, 550, 600);
							 break;
						case M_WHICHHIST:
							 {
							 if(fShowRate){
								 fShowRate = kFALSE;
								 fWhichHistButton->SetText(new TGHotString("Switch_to_Rate_Hist"));
							 } else {
								 fShowRate = kTRUE;
								 fWhichHistButton->SetText(new TGHotString("Switch_to_Dead_Time"));
							 }
							 }
							 break;
					  }
					break;

				case kCM_RADIOBUTTON:
					switch (parm1) {
						case R_FAKE:
								*fInputSource = "Fake";
								fRFile->SetState(kButtonUp);
								fRNet->SetState(kButtonUp);
								fRFileList->SetState(kButtonUp);
						fTbInputFile->Clear();
						fTbInputFile->AddText(0, "fake.root");
						fTeInputFile->SetToolTipText("fake.root");
						gClient->NeedRedraw(fTeInputFile);
						fStartStopButton->SetState(kButtonUp);
								fClearButton->SetState(kButtonDisabled);
								fConfigButton->SetState(kButtonDisabled);
						case R_FILELIST:
								*fInputSource = "FileList";
								fRFake->SetState(kButtonUp);
								fRNet->SetState(kButtonUp);
								fRFile->SetState(kButtonUp);
						fTbInputFile->Clear();
						fTbInputFile->AddText(0, fInputFile->Data());
						fTeInputFile->SetToolTipText(this->ExpandName(fActualInputFile, fInputFile->Data()));
						gClient->NeedRedraw(fTeInputFile);
								fStartStopButton->SetState(kButtonUp);
								fClearButton->SetState(kButtonDisabled);
								fConfigButton->SetState(kButtonDisabled);
							break;
						case R_FILE:
								*fInputSource = "File";
								fRFake->SetState(kButtonUp);
								fRNet->SetState(kButtonUp);
								fRFileList->SetState(kButtonUp);
						fTbInputFile->Clear();
						fTbInputFile->AddText(0, fInputFile->Data());
						fTeInputFile->SetToolTipText(this->ExpandName(fActualInputFile, fInputFile->Data()));
						gClient->NeedRedraw(fTeInputFile);
								fStartStopButton->SetState(kButtonUp);
								fClearButton->SetState(kButtonDisabled);
								fConfigButton->SetState(kButtonDisabled);
							break;
						case R_NET:
								fRFake->SetState(kButtonUp);
								fRFile->SetState(kButtonUp);
								fRFileList->SetState(kButtonUp);
								*fInputSource = "TcpIp";
						fTbInputFile->Clear();
						fTbInputFile->AddText(0, "not used");
						fTeInputFile->SetToolTipText("no input file possible");
						gClient->NeedRedraw(fTeInputFile);
								fStartStopButton->SetState(kButtonDisabled);
								fClearButton->SetState(kButtonUp);
								fConfigButton->SetState(kButtonUp);
							break;
						case R_ACTIVE:
							if(fWriteOutput){
								fRActive->SetState(kButtonUp);
								fRActive->ChangeBackground(red);
								fWriteOutput = kFALSE;
								fRActive->SetToolTipText("Output file not enabled");
							} else {
								fRActive->SetState(kButtonDown);
								fRActive->ChangeBackground(green);
								fWriteOutput = kTRUE;
								fRActive->SetToolTipText("Output file enabled");
								fTeOutputFile->SetFocus();
								TString temp = fTeOutputFile->GetText();
								fTeOutputFile->SetText(temp.Data());

							}
							gClient->NeedRedraw(fRActive);
							break;
					}
				case kCM_CHECKBUTTON:
					break;
				case kCM_COMBOBOX:
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return kTRUE;
}
//________________________________________________________________________________

Bool_t FhMainFrame::PutDefaults(){

//   Bool_t ok = kTRUE;
	ofstream wstream;
	TString outfile;
	TString bckfile;
	outfile = *fDefFile;
	outfile.Strip(TString::kBoth);
	if(!gSystem->AccessPathName(outfile.Data())){
		bckfile = outfile.Data();
		bckfile += ".bck";
		cout << "Moving: " << outfile.Data()  << " to "
			  <<  bckfile.Data() << endl;
		TString MvCmd = "mv ";
		MvCmd += outfile;
		MvCmd += " ";
		MvCmd += bckfile;
		gSystem->Exec((const char *)MvCmd);
	}
	wstream.open(outfile, ios::out);
	if (!wstream.good()) {
		cerr	<< "C_analyze: "
				<< gSystem->GetError() << " - " << outfile
				<< endl;
			return(kFALSE);
	}
	TString wout = "FALSE";
	wstream << "INPUTFILE:  "  <<  fTbInputFile->GetString()    << endl;
	wstream << "INPUTSOURCE:"  <<  *fInputSource           << endl;
	if(fAutoSave) wout = "TRUE";
	else          wout = "FALSE";
	wstream << "AUTOSAVE:   "  <<  wout                  << endl;
	wstream << "RUNNR:      "  <<  fTbRunNr->GetString() << endl;
	wstream << "HISTFILE: "    <<  fTbHistFile->GetString()  << endl;
	wstream << "HELPFILE: "    <<  fHelpFile->Data()  << endl;
	wstream << "OUTPUTFILE: "  <<  fTbOutputFile->GetString()  << endl;
	wstream << "PARFILE: "     <<  fTbParFile->GetString()  << endl;
	wstream << "COMMENT: "     <<  fTbComment->GetString()  << endl;
	Int_t ppcSelected = fCbConnect->GetSelected();
	if (ppcSelected < 0 || ppcSelected > ppcArray->GetEntriesFast()) ppcSelected = 1;
	wstream << "CONNECT: "     <<  ((TObjString *) ppcArray->At(ppcSelected-1))->GetString() << endl;
	wstream << "DIR: "         <<  fTbDir->GetString()      << endl;
	wstream << "TRIGGER: VME"  << endl;
	wstream << "DOWNSCALE:  "  <<  fDownscale           << endl;
	wstream << "GATELENGTH: "  <<  fGateLength          << endl;
	wstream << "BUFSIZE: "     <<  fBufSize             << endl;
	wstream << "BUFFERS: "     <<  fBuffers             << endl;
	wstream << "MAXNOEVENT: "  <<  fMax_time_no_event   << endl;
	wstream << "MAXFILESIZE: " <<  fMaxFileSize         << endl;
	wstream << "AUTORESTART: " <<  fAutoRestart         << endl;
	wstream << "MAXRUNTIME: "  <<  fMaxRunTime          << endl;
	wstream << "AUTORESTART_RT: " <<  fAutoRestartRT    << endl;
	wstream << "WARNHWM: "     <<  fWarnHWM             << endl;
	wstream << "HARDHWM: "     <<  fHardHWM             << endl;
	wstream << "VERBLEV: "     <<  fVerbLevel           << endl;
	wstream << "DEBUGMODE: "   <<  fDebugMode           << endl;
	wstream << "STARTEVENT: "  <<  fStartEvent          << endl;
	wstream << "STOPEVENT: "   <<  fStopEvent           << endl;
	wstream << "FROMTIME: "    <<  fFromTime->Data()    << endl;
	wstream << "TOTIME: "      <<  fToTime->Data()      << endl;
	wstream << "RESETLIST: "   <<  fResetList->Data()   << endl;
	wstream << "MBSLOGLEVEL: " <<  fMbsLogLevel         << endl;
	wstream << "HSAVEINTERVALL: " <<  fHsaveIntervall   << endl;
	wstream << "AVERAGE:  "    <<  fAverage             << endl;
	if(fWriteOutput) wout = "TRUE";
	else             wout = "FALSE";
	wstream << "WRITEOUTPUT:"  <<  wout        << endl;
	if(fSelectTime) wout = "TRUE";
	else             wout = "FALSE";
	wstream << "SELECTTIME:"  <<  wout         << endl;
	if(fSelectNumber) wout = "TRUE";
	else             wout = "FALSE";
	wstream << "SELECTNUMBER:"  <<  wout       << endl;
	if(fAutoSetup) wout = "TRUE";
	else             wout = "FALSE";
	wstream << "AUTOSETUP:"  <<  wout       << endl;

	if( *fInputSource == "TcpIp" ) {
	  if(fSetup == NULL) {
		 if (!gSystem->AccessPathName(".mbssetup")) {
			 fSetup = new TMbsSetup(".mbssetup");
		 } else {
			 fSetup = new TMbsSetup();
		 }
		}
		TString mproc = ((TObjString *) ppcArray->At(fCbConnect->GetSelected()-1))->GetString();
		if (!mproc.IsNull()) {
		  fSetup->EvtBuilder()->SetProcName(mproc.Data());
		  fSetup->ReadoutProc(0)->SetProcName(mproc.Data());
	  }
	  fSetup->Save();
	}

  return kTRUE;
}
 //________________________________________________________________________________

Bool_t FhMainFrame::GetDefaults(){

	Bool_t ok = kTRUE;
	ifstream wstream;
	TString wline;
	TString infile;
	TString parName;
	TString parValue;
	infile = *fDefFile;
	infile.Strip(TString::kBoth);

	fInputFile   = new TString("run$R.root");
	fOutputFile	= new TString("run$R.root");
	fHistFile    = new TString("hists$R.root");
	fParFile     = new TString("none");

	fActualInputFile = new TString();
	fActualOutputFile = new TString();
	fActualHistFile = new TString();
	fActualParFile = new TString();

	fInputSource = new TString("File");
	fHelpFile    = new TString(gSystem->Getenv("MARABOU"));
	*fHelpFile   += "/doc/c_analyze";
//   fHelpFile    = new TString("/home/rg/schaileo/marabou/c_analyze/doc/C_analyze.html");
	TRegexp slash("/");
	TString subdir(gSystem->WorkingDirectory());
	Int_t islash;
	while (1) {
		islash = subdir.Index(slash);
		if(islash < 0) break;
		subdir = subdir.Remove(0,islash+1);
	}
	fRunNr       = new TString("001");
	fOldRunNr    = new TString("001");
	fComment     = new TString("");

	Bool_t shortNames = gEnv->GetValue("TMrbAnalyze.UseShortPPCNames", kFALSE);
	fConnect       = shortNames ? new TString("ppc-0") : new TString("gar-ex-ppc02");

	fMbsVersion   = new TString(gEnv->GetValue("TMbsSetup.MbsVersion", ""));
	if (fMbsVersion->Contains(".")) {
		fMbsVersion->ReplaceAll(".", "");
		fMbsVersion->Prepend("v");
	} else if (!fMbsVersion->BeginsWith("v")) {
		cerr	<< setred << "Malformed Mbs version - " << fMbsVersion << setblack << endl;
	  ok = kFALSE;
	}
	if (fMbsVersion->IsNull()) {
	  cerr	<< setred << "C_analyze: MBS version not defined" << setblack << endl;
	  ok = kFALSE;
	}
	fProcType = new TString(gEnv->GetValue("TMbsSetup.ProcType", ""));
	if (fProcType->IsNull()) {
	  cerr	<< setred << "C_analyze: PPC proc type not defined" << setblack << endl;
	  ok = kFALSE;
	}
	fUseSSH = gEnv->GetValue("TMbsSetup.UseSSH", kFALSE);
	
	fDir          = new TString("ppc");
	fCodeName        = new TString("");
	fFromTime        = new TString(":000");
	fToTime          = new TString(":000");
	fResetList          = new TString("*");
	fPlaySound       = gEnv->GetValue("M_analyze.PlaySound", 1);
	fSoundFile       = new TString(gEnv->GetValue("M_analyze.EndOfRunSound", "$MARABOU/sounds/Ende_der_Seereise.wav"));
	fSoundPlayer       = new TString(gEnv->GetValue("M_analyze.SoundPlayer", "/usr/bin/play"));
	fSelectTime      = kFALSE;
	fSelectNumber    = kFALSE;
	fAutoSetup    = kTRUE;
	fMbsLogLevel = 1;
	fMessageIntervall = 1000;       // 1 second
	fAverage          = 60;         // 60 seconds
	void* dirp=gSystem->OpenDirectory(".");
	TRegexp endwithmk("Readout\\.mk$");
	TRegexp endwithc("Readout\\.c$");
	TRegexp endwithh("Readout\\.h$");
	const char * fname;
	while ( (fname = gSystem->GetDirEntry(dirp)) && (fCodeName->Length() == 0) ) {
		TString sname(fname);
		if (sname.Index(endwithc) >= 0) {
		*fCodeName = sname;
		endwithc = "\\.c$";
		(*fCodeName)(endwithc) = "";
		} else if (sname.Index(endwithh) >= 0) {
		*fCodeName = sname;
		endwithh = "\\.h$";
		(*fCodeName)(endwithh) = "";
		} else if (sname.Index(endwithmk) >= 0) {
 		*fCodeName = sname;
		endwithmk = "\\.mk$";
		(*fCodeName)(endwithmk) = "";
	  }
	}
	fGateLength        = 500;
	fBufSize           = 0x4000;
	fBuffers           = 8;
	fMax_time_no_event = 20;
	fDownscale         = 1;
	fStartEvent        = 0;
	fStopEvent         = 0;
	fMaxRunTime        = 0;
	fMaxFileSize       = 1800;     // 1.8 Gbyte
	fHsaveIntervall    = 300;      // 5 Min
	fWriteOutput       = kFALSE;
	fAutoSave          = kFALSE;
	fAutoRestart       = 0;
	fAutoRestartRT     = 0;
	fShowRate          = kTRUE;
	fWarnHWM 	       = 200;
	fHardHWM 	       = 100;
	fVerbLevel         = 0;
	fDebugMode         = 0;
	if (!gSystem->AccessPathName(infile.Data())) {
		wstream.open(infile, ios::in);
		if (wstream.good()) {
			ok = kTRUE;
			while (1) {
		  		wline.ReadLine(wstream, kFALSE);
		 		if (wstream.eof()) {
					wstream.close();
					break;
				}
				wline.Strip();
				if (wline.Length() <= 0) continue;
				if (wline[0] == '#') continue;

				Int_t n = wline.Index(":");
				if (n < 0) {
					cerr	<< "C_analyze: Illegal format" << endl
						<< ">>" << wline << "<<"
						<< endl;
					ok = kFALSE;
				}
				parName= wline(0, n);
				parName = parName.Strip(TString::kBoth);
	//         cout << wline << endl;
				wline.Remove(0,n+1);
				parValue = wline;
	//         cout << parValue << endl;
				parValue = parValue.Strip(TString::kBoth);
	//         cout << parValue << endl;
				if(parName == "INPUTFILE")  *fInputFile   = parValue;
				if(parName == "INPUTSOURCE")*fInputSource = parValue;
				if(parName == "RUNNR")      *fRunNr       = parValue;
				if(parName == "OUTPUTFILE") *fOutputFile    = parValue;
				if(parName == "HISTFILE")  *fHistFile    = parValue;
				if(parName == "HELPFILE")  *fHelpFile    = parValue;
				if(parName == "PARFILE")    *fParFile		= parValue;
				if(parName == "COMMENT")    *fComment     = parValue;
				if(parName == "CONNECT")     *fConnect      = parValue;
				if(parName == "MBSVERS")    *fMbsVersion  = parValue;
				if(parName == "DIR")        *fDir         = parValue;
				if(parName == "RESETLIST")  *fResetList   = parValue;
				if(parName == "DOWNSCALE")  fDownscale    = atoi(parValue.Data());
				if(parName == "GATELENGTH") fGateLength   = atoi(parValue.Data());
				if(parName == "BUFSIZE")    fBufSize      = atoi(parValue.Data());
				if(parName == "BUFFERS")    fBuffers      = atoi(parValue.Data());
				if(parName == "MAXNOEVENT") fMax_time_no_event = atoi(parValue.Data());
				if(parName == "STARTEVENT" ) fStartEvent      = atoi(parValue.Data());
				if(parName == "AUTORESTART") fAutoRestart  = atoi(parValue.Data());
				if(parName == "AUTORESTART_RT") fAutoRestartRT  = atoi(parValue.Data());
				if(parName == "MAXFILESIZE")fMaxFileSize   = atoi(parValue.Data());
				if(parName == "MAXRUNTIME") fMaxRunTime    = atoi(parValue.Data());
				if(parName == "STOPEVENT" ) fStopEvent     = atoi(parValue.Data());
				if(parName == "WARNHWM" )   fWarnHWM       = atoi(parValue.Data());
				if(parName == "HARDHWM")    fHardHWM       = atoi(parValue.Data());
				if(parName == "VERBLEV")    fVerbLevel     = atoi(parValue.Data());
				if(parName == "DEBUGMODE")  fDebugMode     = atoi(parValue.Data());
				if(parName == "FROMTIME")   *fFromTime     = parValue;
				if(parName == "TOTIME"  )   *fToTime       = parValue;
				if(parName == "MBSLOGLEVEL"  ) fMbsLogLevel   = atoi(parValue.Data());
				if(parName == "HSAVEINTERVALL") fHsaveIntervall   = atoi(parValue.Data());
				if(parName == "AVERAGE"  )   fAverage   = atoi(parValue.Data());
				if(parName == "WRITEOUTPUT" && parValue.Index("TRUE") >= 0)
					fWriteOutput=kTRUE;
				if(parName == "AUTOSAVE" && parValue.Index("TRUE") >= 0)
					 fAutoSave=kTRUE;
				if(parName == "SELECTTIME" && parValue.Index("TRUE") >= 0)
					 fSelectTime=kTRUE;
				if(parName == "SELECTNUMBER" && parValue.Index("TRUE") >= 0)
					 fSelectNumber=kTRUE;
				if(parName == "AUTOSETUP" && parValue.Index("FALSE") >= 0)
					 fAutoSetup=kFALSE;
			}
		} else {
			cerr	<< setred << "C_analyze: "
				<< gSystem->GetError() << " - " << infile
				<< setblack << endl;
			ok = kFALSE;
		}
	} else {
		cout << setred << "File for defaults not found" << setblack << endl;
		ok = kFALSE;
	}
	if (!ok) cout << setred <<  "Couldn't read C_analyze.def" << setblack << endl;
   
	if (fInputFile->IsNull()) {
		*fInputFile = gEnv->GetValue("M_analyze.InputFilePath", "");
		if (fInputFile->IsNull()) {
			*fInputFile = "run$R.root";
		} else {
			*fInputFile += "/run$R.root";
		}
   }
   if (fOutputFile->IsNull()) {
		*fOutputFile = gEnv->GetValue("M_analyze.OutputFilePath", "");
		if (fOutputFile->IsNull()) {
			*fOutputFile = "run$R.root";
		} else {
			*fOutputFile += "/run$R.root";
		}
   }
   if (fHistFile->IsNull()) {
		*fHistFile = gEnv->GetValue("M_analyze.HistoFilePath", "");
		if (fHistFile->IsNull()) {
			*fHistFile = "hists$R.root";
		} else {
			*fHistFile += "/hists$R.root";
		}
	}
 
  if (!gSystem->AccessPathName(".mbssetup")) {
		cout << setblue << "Looking up setup data in .mbssetup" << setblack << endl;
	  if(fSetup == NULL) {
		 if (!gSystem->AccessPathName(".mbssetup")) {
			 fSetup = new TMbsSetup(".mbssetup");
		 } else {
			 fSetup = new TMbsSetup();
		 }
		}
		TMrbNamedX master, slave, trigger;
		fSetup->Get(master,   "EvtBuilder.Name",             fConnect->Data());
		fSetup->Get(slave,    "Readout0.Name",               fConnect->Data());
		fSetup->Get(trigger,  "Readout0.TriggerModule.Type", "VME");
		cout << "EvtBuilder      : "  << fConnect->Data() << endl;
		cout << "Readout0/Master : "  << fConnect->Data() << endl;
		cout << "Readout1/Slave  : "  << endl;
		cout << "TriggerModule   : " << trigger.GetName() << "(" << trigger.GetIndex() << ")" << endl;
	} else {
		cout << "No .mbssetup in cwd" << endl;
	}
	return kTRUE;
}
//________________________________________________________________________________

Int_t FhMainFrame::GetComSocket(Int_t attachid, Int_t attachsock)
{
//   Bool_t ok = kFALSE;
//   fComSocket = 0;
	Int_t socknr = 0;
	TString pidfile("/tmp/M_analyze_");
	pidfile += gSystem->Getenv("USER");
	pidfile += ".";
	Int_t baselength = pidfile.Length();
	Int_t sock1, sock2;

//  loop on files in /tmp
//  if online (input TcpIp) insist on socket MINSOCKET (9090)
//  otherwise look for 9091-9095

	if( *fInputSource == "TcpIp" ) {
		sock1 = MINSOCKET;
		sock2 = sock1;
	} else if (attachsock > 0) {
		sock1 = attachsock;
		sock2 = sock1;
	} else {
		sock1 = MINSOCKET+1;
		sock2 = MAXSOCKET;
	}

	for (Int_t sock = sock1; sock <= sock2; sock++) {
//      TString cmd("/usr/sbin/lsof -i :");
		TString cmd("/lsof -i :");
		if ( !gSystem->AccessPathName("/usr/sbin/lsof") ) {
			cmd.Prepend("/usr/sbin");
		} else {
			cmd.Prepend("/usr/bin");
		}
		cmd += sock;
		cmd += " | grep LISTEN";
//      cout << cmd << endl;
		FILE *fp = gSystem->OpenPipe(cmd, "r");
		TString result;
		if ( result.Gets(fp) ) {
			cout << "Port: " << sock << " in use by: " << result << endl;
			if  ( attachid > 0 && sock == attachsock ) {
				socknr = sock;
				 gSystem->ClosePipe(fp);
				break;
			}
		} else {
			socknr = sock;
			pidfile.Resize(baselength);
			pidfile += socknr;
			*fOurPidFile = pidfile;
			gSystem->ClosePipe(fp);
			break;
		}
		gSystem->ClosePipe(fp);
	}
	if ( socknr != 0 ) {
		if ( !gSystem->AccessPathName(pidfile.Data()) ) {
//  case pidfile exists
			ifstream wstream;
	//      TString wline;
			Int_t pid, status;
			wstream.open(pidfile.Data(), ios::in);
			wstream >> pid >> status;
			wstream.close();
			TString procs = "/proc/";
			procs += pid;
			if ( !gSystem->AccessPathName(procs.Data()) ){
				if ( attachid > 0 && attachid == pid ) {
					*fOurPidFile = pidfile;
					return socknr;
				}

			} else {
				int buttons = kMBYes | kMBNo, retval=0;
				EMsgBoxIcon icontype = kMBIconQuestion;
				TString emess("M_analyze with Id ");
				emess += pid;
				emess += "seems not running, clean lock file?";
				new TGMsgBox(gClient->GetRoot(),this,"Question",
				emess.Data(),
				icontype,
//            gClient->GetPicture("/home/rg/schaileo/myroot/xpm/warn1.xpm"),
				buttons, &retval);
				if ( retval == kMBYes ) {
					TString RmCmd = "rm ";
					RmCmd += pidfile;
					gSystem->Exec((const char *)RmCmd);
				} else {
					socknr = 0;
				}
			}
		}
//
//      if ( socknr == 0 && gSystem->AccessPathName(pidfile.Data()) ) {
//         cout << socknr << " " << pidfile.Data() << endl;
//         *fOurPidFile = pidfile;
 //        ok = kTRUE;
//      }
	}
	if ( attachid > 0 )
		cout <<  setred << "Cant find running M_analyze with pid: " << attachid
			  << setblack << endl;

// try to find a socket to communicate with M_analyze (9091-9095)
	if ( socknr == 0 )
		cout << setred << "Cant get free Socket, more than 5 M_analyze running?"
			 << setblack << endl;
//    cout << "GetComSocket return: " << socknr << endl;
	 return socknr;
}
//________________________________________________________________________________

Int_t FhMainFrame::IsAnalyzeRunning(Int_t ps_check){
//  look if M_analyze is already running, if M_analyze.pid does not
//  exist leave status unchanged

	if(fC_Status == M_PAUSING ) return fC_Status;

//   TString our_pid_file = "M_analyze.pid";
	Int_t status = fM_Status;
//    cout << "pidFile: " << fOurPidFile->Data() << " " << gSystem->AccessPathName(fOurPidFile->Data()) << endl;
	if(!gSystem->AccessPathName(fOurPidFile->Data())){
		ifstream wstream;
//      TString wline;
		Int_t pid;
		wstream.open(fOurPidFile->Data(), ios::in);
		wstream >> pid >> status;
		wstream.close();
//      cout << "pid, status " << pid << " " << status << endl;
		if(ps_check == 1){
			TString procs = "/proc/";
			procs += pid;
//         cout << procs.Data() << endl;
			if(!gSystem->AccessPathName(procs.Data())){
				 cout << "found " << procs.Data() << endl;

			} else {
				 cout << "Didnt find " << procs.Data()
				 << " remove " << fOurPidFile->Data() << endl;
					TString RmCmd = "rm ";
					RmCmd += fOurPidFile->Data();
					gSystem->Exec((const char *)RmCmd);
//            }
				if (status == M_STARTING || status == M_RUNNING) {
//               cout << setred   << "M_analyze seems to have died"
//                    << setblack << endl;
					status = M_DIED;
				} else {
					status = M_ABSENT;
				}
			}
		}
	}
	 else if (fM_Status != M_STARTING && fM_Status != M_DIRTY
			 && fM_Status != M_CONFIGURED) status=M_ABSENT;
	return status;
}
//________________________________________________________________________________

void FhMainFrame::Runloop(){
//   static Int_t fM_OldStatus=M_ABSENT;
	static Bool_t saving_hists=kFALSE;
	Long_t id, flags, modtime;
	Long64_t size;

	static TH1 * hrate     = 0;
	static TH1 * hdeadt    = 0;
	static TH1 * htemp     = 0;
	static TH1 * hnew      = 0;
	TCanvas *c1 = fRateHist->GetCanvas();
	TString stime;
	TMessage * message;
	Int_t nobs;
	fM_Status = IsAnalyzeRunning(0);

	if (fM_Status != M_RUNNING) this->SetTime();

//   cout << "fM_Status " << fM_Status<< endl;
	if(fM_Status == M_RUNNING && (!fForcedStop && fWriteOutput && fActualOutputFile->Length() > 1)){
		Int_t sts = gSystem->GetPathInfo(fActualOutputFile->Data(), &id, &size, &flags, &modtime);
		if (sts == 0) {
			fOutSize->SetText(new TGString(Form("%lld", size)));
			gClient->NeedRedraw(fOutSize);
			if (fMaxFileSize > 0 && size/1000000 > fMaxFileSize) {
				cout << setred << fActualOutputFile->Data() << ": size of output file = " << size/1000000
					  << "MB exceeds MaxFileSize = " << fMaxFileSize << " MB" << endl;
				cout << "force StopDAQ" << setblack << endl;
				fForcedStop = kTRUE;
				this->StopDAQ();
				return;
			}
		}
	}
	if(fM_Status == M_RUNNING && (!fForcedStop && fMaxRunTime > 0)){
		if (fTotal_livetime >= fMaxRunTime) {
			cout << endl << setblue << "*******************************************************" << endl;
			cout << "Runtime (sec) " << fMaxRunTime
				  << " reached MaxRunTime: " << fMaxRunTime << endl;
			cout << "force StopDAQ" << endl;
			cout << "*******************************************************" << setblack<< endl;
			fForcedStop = kTRUE;
			this->StopDAQ();
			return;
		}
	}
//   fM_Status = IsAnalyzeRunning(0);
	if(fForcedStop && (fAutoRestart || fAutoRestartRT)){
		if( (*fInputSource == "TcpIp" &&  fM_Status == M_CONFIGURED) || fM_Status == M_ABSENT) {
			fForcedStop = kFALSE;
			this->StartDAQ();
			fWaitedForStop = 0;
			return;
		} else {
			fWaitedForStop++;
			if (fWaitedForStop > 30) {
				cout << setred << "Waited 30 seconds for stopping, give up" << setblack << endl;
				fForcedStop = kFALSE;
				fWaitedForStop = 0;
			}
			gSystem->Sleep(1000);
		}
	}
//   cout << "fM_Status,fWasStarted " <<  fM_Status << " " << fWasStarted << endl;

//   if(fM_Status == M_RUNNING || fM_Status == M_PAUSING || fWasStarted){
	if ( fM_Status == M_RUNNING || fM_Status == M_PAUSING ){
		if ( fWasStarted && fM_Status != M_RUNNING && fM_Status != M_PAUSING ) {
			gSystem->Sleep(1000);
		}
		if ( fComSocket ) {
//so        TSocket * sock = new TSocket("localhost", fComSocket);
		  TSocket * sock = fComSocket;
		  if ( !sock || !sock->IsValid() ) {
				cout << setred << "No connection to M_analyze" << setblack << endl;
		  } else {
//      	  Int_t nobs =  sock->Recv(message);
//      	  if ( message ) delete message;
			  TString cmd("M_client gethist ");
			  TString which;
			  if (fShowRate) which = "RateHist";
			  else           which = "DeadTime";
			  cmd += which.Data();
//            cout << "Send: " << cmd.Data() << " to " << sock << endl;
			  sock->Send(cmd.Data());
			  nobs = sock->Recv(message);
//      	  cout << "M_client gethist RateHist, nobs: " << nobs << endl;
			  if ( nobs <= 0 || message->What() == kMESS_STRING ){
//         	  cout << setred << "C_analyze: cant get "<< which.Data() << setblack << endl;
				  if ( nobs > 0 ) {
					  char *str0 = new char[nobs];
					  message->ReadString(str0, nobs);
					  if (fVerbLevel > 0) cout << "Got string: " << str0 << endl;
					  delete [] str0;
				  }
			  } else if ( message->What() == kMESS_OBJECT ) {
//               if (hrate) delete hrate;
					hnew = (TH1*) message->ReadObject(message->GetClass());
			  } else {
				  cout << "Unknown message type" << endl;
			  }
			  if ( message ) delete message;
//so      	  sock->Close();
//so      	  delete sock;
		  }
	  }
	  if ( hnew ) {
		  htemp  = hrate;
		  if (fShowRate) { hrate  = hnew; hdeadt = NULL; }
		  else           { hdeadt = hnew; hrate  = NULL; }
	  } else {
		  htemp = 0;
	  }
//     if ( hrate ) {
//        hrate->Print();
//         show dead time history
	  if(!fShowRate && hdeadt){
			hdeadt->SetFillColor(kMagenta);
			hdeadt->SetLabelSize(0.1,"X");
			hdeadt->SetLabelSize(0.1,"Y");
			hdeadt->SetNdivisions(505,"Y");
			TPaveText *title = (TPaveText*)c1->GetPrimitive("title");
			Float_t titH = 0.15;
			Float_t titW = 0.4;
			Float_t titX1= 0.05;
			Float_t titY1= 0.9;
			if(title){
				titX1 = title->GetX1NDC();
				titW =  title->GetX2NDC() - titX1;
				titY1 = title->GetY1NDC();
				titH  = title->GetY2NDC() - titY1;
			}
			gROOT->ForceStyle();
			gStyle->SetTitleX(titX1);
			gStyle->SetTitleY(titY1 + titH);
			gStyle->SetTitleH(titH);
			gStyle->SetTitleW(titW);
			gStyle->SetOptStat(0);
			hdeadt->Draw();
			gPad->SetTicks(0,2);
			c1->Modified();
			c1->Update();
		}
		if(hrate){
			Int_t total = (Int_t)hrate->GetEntries();
			fNev->SetText(new TGString(Form("%d", total)));
			gClient->NeedRedraw(fNev);

			c1->cd();
			if(fShowRate){
				hrate->SetFillColor(kRed);
				hrate->SetLabelSize(0.1,"X");
				hrate->SetLabelSize(0.1,"Y");
				hrate->SetNdivisions(505,"Y");
//               hrate->Draw();
//               c1->Modified();
				TPaveText *title = (TPaveText*)c1->GetPrimitive("title");
				Float_t titH = 0.15;
				Float_t titW = 0.35;
				Float_t titX1= 0.02;
				Float_t titY1= 0.85;
				if(title){
					titX1 = title->GetX1NDC();
					titW =  title->GetX2NDC() - titX1;
					titY1 = title->GetY1NDC();
					titH  = title->GetY2NDC() - titY1;
				}
				gROOT->ForceStyle();
				gStyle->SetTitleX(titX1);
				gStyle->SetTitleY(titY1 + titH);
				gStyle->SetTitleH(titH);
				gStyle->SetTitleW(titW);
//               cout << " titX1 "<< titX1 << " titW " << titW
//                    << " titY1 "<< titY1 << " titH " << titH
//               << endl;
				gStyle->SetOptStat(0);
				hrate->Draw();
				gPad->SetTicks(0,2);
				c1->Modified();
				c1->Update();
			}
			if (htemp) {delete htemp; htemp = 0;}
			hnew = 0;

//
			Float_t etime = fStopwatch->RealTime();
			if(total == 0) etime = 0;      //wait for first events to arrive
			if(fM_Status == M_RUNNING){
//                  Float_t etime = fStopwatch->RealTime();
				fTotal_livetime += etime;
				Int_t isec = (Int_t)fTotal_livetime;
				fRunTime->SetText(new TGString(Form("%d",isec)));
				gClient->NeedRedraw(fRunTime);
				if(fTotal_livetime > 0){
					Float_t avg_rate = 0;
					Int_t avg_bins = hrate->GetNbinsX();
					if(fAverage <= 0 || fAverage >= avg_bins){
						avg_rate =  (Float_t)total/fTotal_livetime;
					} else {
						Float_t sum = 0;
						Float_t binsum = 0;
						for(Int_t i = avg_bins - fAverage+1; i <= avg_bins; i++){
							sum += hrate->GetBinContent(i);
							if(sum>0) binsum++; // skip trailing 0's
						}
						if(binsum >0)avg_rate = sum / binsum;
					}
					fRate->SetText(new TGString(Form("%d",(Int_t)avg_rate)));
					gClient->NeedRedraw(fRate);
//                same for deadtime if available
					if(hdeadt){
						avg_rate = 0;
						avg_bins = hdeadt->GetNbinsX();
						Int_t startbin = avg_bins - fAverage + 1;
						if(startbin < 0)startbin = 1;
						Float_t binsum = 0;
						Float_t sum = 0;

						for(Int_t i = startbin; i <= avg_bins; i++){
							sum += hdeadt->GetBinContent(i);
							if(sum>0) binsum++; // skip trailing 0's
						}
						if(binsum >0)avg_rate = sum /binsum;
						fDeadTime->SetText(new TGString(Form("%d",(Int_t)avg_rate)));
						gClient->NeedRedraw(fDeadTime);
					}
				}
				fTotal_time_elapsed += etime;
				fTotal_time_no_event +=  etime;
				fStopwatch->Reset();
				fStopwatch->Start();
//                  fStopwatch->Continue();
//                  cout << "#### time" << etime << endl;

				if(fTotal_time_no_event > fMax_time_no_event){
					fTotal_time_no_event = 0;
					if(total <= fEvents_before){
//                        cout << total << " " << fEvents_before << endl;
						cout << setred << bell <<
						"No events last " << fTotal_time_elapsed << " seconds"<< endl;
						if(!CheckHostsUp() && *fInputSource == "TcpIp")
							cout << "Seems a Lynx processor died"<< endl;
						cout << setblack;
					} else {
//                        fTotal_time_no_event = 0;
						fTotal_time_elapsed  = 0;
					}
				fEvents_before = total;
				}
			}
		}
//      }
		if(fWasStarted && fM_Status != M_RUNNING && fM_Status != M_PAUSING){
			cout << "fWasStarted = 0; " << endl;
			fWasStarted = 0;
		}
	}
	if(fM_OldStatus != fM_Status){
 //     cout << "Status : " << fM_Status << " fM_OldStatus: " << fM_OldStatus << endl;
		if ((fM_Status == M_ABSENT || fM_Status == M_CONFIGURED)&&
			(fM_OldStatus  == M_RUNNING || fM_OldStatus  == M_STOPPING ||
			 fM_OldStatus  == M_STOPPED || fM_OldStatus  == M_PAUSING ||
			 fM_OldStatus  == -1 )){
			if(fWriteOutput && *fInputSource == "TcpIp"){
				TString chmodCmd = "chmod -w ";
				chmodCmd += *fActualOutputFile;
				cout << setgreen << "For your info: " << chmodCmd.Data()
					  << setblack << endl;
				gSystem->Exec(chmodCmd.Data());
			}
			if(fAutoSave && !saving_hists){
//            saving_hists = kTRUE;
//            SaveMap(kFALSE);
 //           saving_hists = kFALSE;
				 cout << setblue << "Skip saving hists in C_analyze"
						<< setblack << endl;
			}
			if(*fInputSource != "TcpIp")StopMessage();
			SetButtonsToDefaults();
//         StopMessage();
		}
		if(fM_Status == M_ABSENT){
			fStatus->SetText(new TGString("Absent"));
			fStatus->ChangeBackground(green);
		}
		if(fM_Status == M_DIRTY){
			fStatus->SetText(new TGString("Dirty"));
			fStatus->ChangeBackground(red);
		}
		if(fM_Status == M_CONFIGURED){
			fStatus->SetText(new TGString("Configured"));
			fStatus->ChangeBackground(yellow);
		}
		if(fM_Status == M_STARTING){
			fStatus->SetText(new TGString("Starting"));
			fStatus->ChangeBackground(cyan);
		}
		if(fM_Status == M_CONFIGURING){
			fStatus->SetText(new TGString("Configuring"));
			fStatus->ChangeBackground(cyan);
		}
		if(fM_Status == M_RUNNING){
			fStatus->SetText(new TGString("Running"));
			fStatus->ChangeBackground(green);
		}
		if(fM_Status == M_PAUSING){
			fStatus->SetText(new TGString("Pausing"));
			fStatus->ChangeBackground(magenta);
//         fPauseButton->SetText(new TGHotString("Resume"));
		}
		if(fM_Status == M_STOPPING){
			fStatus->SetText(new TGString("Stopping"));
			fStatus->ChangeBackground(cyan);
//         TString TermCmd = "M_client terminate";
//         MessageToM_analyze(TermCmd);
		}
		if(fM_Status == M_STOPPED){
			fStatus->SetText(new TGString("Stopped"));
			fStatus->ChangeBackground(red);
		} else {

		}
		gClient->NeedRedraw(fStatus);
		fM_OldStatus=fM_Status;
	}
	fM_OldStatus=fM_Status;
};

//_____________________________________________________________________________________
// Set time string

void FhMainFrame::SetTime(){
 	time_t now = time(NULL);
	Char_t result[200];
	strftime(result, 200, "%T", localtime(&now));
	fStartTime->SetText(new TGString(result));
	gClient->NeedRedraw(fStartTime);
}

//________________________________________________________________________________

const Char_t * FhMainFrame::ExpandName(TString * Result, const Char_t * Format) {

 	time_t now = time(NULL);	// replace date & time place holders
	TString fmt = Format;

	TRegexp run = "$R";			// replace $R by run number
	fmt(run) = fRunNr->Data();
	TRegexp start = "$S";		// replace $S by "dd-mm-HHMM"
	fmt(start) = "%d-%m-%H%M";
	Char_t result[200];
	strftime(result, 200, fmt.Data(), localtime(&now));
	*Result = result;
	*Result = Result->Strip(TString::kBoth);
	return(Result->Data());
}

//________________________________________________________________________________

void FhMainFrame::ResetToolTips() {
	TString dmy;
	fTeInputFile->SetToolTipText(Form("%s - will be expanded to %s", fInputFile->Data(), this->ExpandName(&dmy, fInputFile->Data())));
	fTeOutputFile->SetToolTipText(Form("%s - will be expanded to %s", fOutputFile->Data(), this->ExpandName(&dmy, fOutputFile->Data())));
	fTeHistFile->SetToolTipText(Form("%s - will be expanded to %s", fHistFile->Data(), this->ExpandName(&dmy, fHistFile->Data())));
	fTeParFile->SetToolTipText(Form("%s - will be expanded to %s", fParFile->Data(), this->ExpandName(&dmy, fParFile->Data())));
}

//_____________________________________________________________________________________

int main(int argc, char **argv)
{
	TApplication theApp("App", &argc, argv);
	Int_t attachid = 0;
	Int_t attachsock = 0;
	if (argc > 2) {
		attachid = atoi(argv[1]);
		attachsock = atoi(argv[2]);
	}
	mainWindow = new FhMainFrame(gClient->GetRoot(), 400, 220, attachid, attachsock);
	cout << "Root Vers." <<  gROOT->GetVersion() << endl;
	Int_t delay = 1000;

	mt = new MyTimer(delay,kTRUE);
	theApp.Run();

	return 0;
}
