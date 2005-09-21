#ifndef HPR_CANALYZE
#define HPR_CANALYZE
#include "TMbsControl.h"
#include "TMbsSetup.h"
#include "TMrbHelpBrowser.h" 

namespace std {} using namespace std;

enum CAnalyzeCommandIdentifiers {
   M_CLEAR, M_START_STOP, M_PAUSE, M_CONFIG, M_RESET, M_SAVEMAP, M_DELETEMAP,
   M_INTERVAL_COMBO, M_DOWNSCALE, M_HELP,M_LOADPAR, C_TRIGGER,C_MASTER,C_READOUT,
   C_MBSVERSION, R_FAKE, R_FILE, R_FILELIST, R_NET, R_ACTIVE, R_AUTO, R_PASSIVE, 
   M_MBSSTATUS, M_BUFSIZE, M_BUFSTREAM, M_NOEVENT,
   M_HELPC, M_HELPM, M_HELPBR, M_HELPNAME,
   M_COMMENT, M_RESETALL, M_RESETSEL, M_STARTEVENT, M_STOPEVENT, M_FROMTIME
   , M_TOTIME, M_GATEL, M_MBSSETUP, M_CODENAME, M_COMPILE
   , M_MAXFILESIZE, M_MAXRUNTIME, M_MBSLOGLEVEL, M_SELECTTIME, M_SELECTNUMBER 
   ,M_RUNNR, M_INPUT, M_DIR, M_MAPF, M_MAPS, M_HISTF, M_ROOTF, M_PARF
   ,M_QUIT, M_AVERAGE, M_SAVESETUP, M_AUTOSETUP, M_WHICHHIST, M_RESETLIST
   ,M_PAUSE_RESET, M_WARNHWM, M_HARDHWM, M_VERBLEV, M_CHKQUOTA, M_RELOAD
   ,M_DEBUG,M_MADEBUG, M_AUTORESTART, M_AUTORESTART_RUNTIME, M_HSAVEINTERVALL};
enum M_status {M_ABSENT, M_STARTING, M_RUNNING, 
               M_PAUSING, M_STOPPING, M_STOPPED, M_DIRTY, M_DIED, 
               M_CONFIGURED, M_CONFIGURING};
const Int_t MAXTIMEOUT = 50;

static const Int_t NPPC = 13, MBSV = 2, TRIGS = 2;

static const Char_t * masters[] = {"ppc-0", "ppc-1", "ppc-2", "ppc-3", "ppc-4", "ppc-5", "ppc-6", "ppc-7", "ppc-8", "ppc-1", "ppc-3", "ppc-5", "ppc-7"};   
static const Char_t * slaves[]  = {"ppc-0", "ppc-0", "ppc-2", "ppc-2", "ppc-4", "ppc-4", "ppc-6", "ppc-6", "ppc-8", "ppc-1", "ppc-3", "ppc-5", "ppc-7"};
static const Char_t * triggers[]  = {"VME", "CAMAC"}; 

static const Int_t  MINSOCKET = 9090, MAXSOCKET = 9095;
 
//______________________________________________________________________

class FhMainFrame : public TGMainFrame {

private:
   TList *fWidgets;
   TGCompositeFrame   *fHFr, *fHFrHalf,*fLabelFr;
   TRootEmbeddedCanvas *fRateHist;
   TGTextButton        *fConfigButton, *fStartStopButton, *fClearButton, 
                      *fPauseButton, *fResetButton, *fQuitButton,
                      *fSaveMapButton,
                      *fParButton , *fMbsSetupButton, *fWhichHistButton;
   TGLabel            *fLabel, *fOutSize, *fRunTime, *fTotalEvents, *fRate,
                      *fStartTime, *fDeadTime, *fStatus, *fNev, *fTbSockNr;

   TGTextEntry        *fTeMap, *fTeMapSize, *fTeFile,
                      *fTeRootFile, *fTeHistFile,*fTeRunNr,
                      *fTeComment, *fTeParFile, 
                      *fTeDir;
   TGTextBuffer       *fTbMap, *fTbMapSize, *fTbFile,
                      *fTbRootFile, *fTbHistFile, *fTbRunNr, 
                      *fTbComment, *fTbParFile, 
                      *fTbFileList,
                      *fTbDir,*fTbConvTime;
   TGLayoutHints      *fLO1, *fLO2, *fLO3, *fLO4, *fLO5, *fLR1;

   TGComboBox         *fCbTrigger, *fCbMaster, *fCbReadout, *fCbMbsVersion;
   
   TGRadioButton      *fRFake, *fRFile, *fRFileList, *fRNet, *fRActive,*fRAuto, *fRPassive;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuParameters, *fMenuHist, *fMenuEvent, *fMenuHelp
                      , *fMenuMbs, *fMenuSave;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

   TString *fInputFile, *fOutputFile, *fMap,
           *fHistList, *fRootFile, *fHistFile, *fDefFile, *fInputSource, 
           *fRunNr, *fOldRunNr, *fComment, *fPar, 
           *fMbsVersion, *fDir, *fMaster, *fReadout,
           *fTrigger, *fCodeName, *fFromTime, *fToTime,
           *fResetList, *fOurPidFile, *fHelpFile;

   TString *fSaveMap;
   Bool_t  fWriteOutput, fAutoSave, fSelectTime, fSelectNumber, fAutoSetup,
           fShowRate, fAttach, fUseMap, fMbsDebug, fForcedStop;
   Int_t   fM_Status;
   Int_t   fC_Status;
   Int_t   fWasStarted;
   Int_t   fEvents_before;
   Int_t   fBufSize, fBuffers, fGateLength, fDownscale, 
           fStartEvent, fStopEvent, fMapSize, fMaxFileSize, fMaxRunTime,
           fVerbLevel, fHardHWM, fWarnHWM, fAutoRestart, fAutoRestartRT,
			  fWaitedForStop, fHsaveIntervall;
   Float_t fMax_time_no_event;
   Float_t fTotal_time_no_event;
   Float_t fTotal_time_elapsed;
   Float_t fTotal_livetime;
   Int_t   fAverage;
   Int_t   fMbsLogLevel;
   Int_t   fDebugMode;
   Int_t   fMessageIntervall;

   Int_t   fSockNr; 
   TSocket  * fComSocket; 
   Int_t  fAttachid;
   Int_t  fAttachsock;
   MessageServer * fMessageServer;
   TMrbHelpBrowser * fHelpBrowser;
   TStopwatch * fStopwatch;
   TMapFile * fMfile;
   ULong_t white, red, green, blue, yellow, magenta, cyan, antiquewhite1, grey;

   SetWindowAttributes_t wattgreen;
   SetWindowAttributes_t wattred;
   SetWindowAttributes_t wattblue;
   SetWindowAttributes_t wattyellow;
   SetWindowAttributes_t wattmagenta;
   SetWindowAttributes_t wattcyan;
   TMbsControl * fMbsControl;
	TMbsSetup * fSetup;

   GContext_t * fPYellowTextGC;

   
public:
   FhMainFrame(const TGWindow *p, UInt_t w, UInt_t h, 
               Int_t attachid = 0, Int_t attachsock = 0);
   virtual ~FhMainFrame();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void Runloop();
   void SetTime();
   void SaveMap(Bool_t askforname = kTRUE);
   void StopMessage();
   void SetButtonsToDefaults();
   Bool_t AskforRemove(const char *);
   Bool_t CheckParams();
   Bool_t CheckHostsUp();
   Bool_t CheckOneHostUp(const char *);
   Bool_t ClearMbs();
   Bool_t MbsCompile();
   Bool_t MbsStatus();
   Bool_t MbsSetup();
   Bool_t Configure();
   Bool_t StartDAQ();
   Bool_t StopDAQ();
   Bool_t GetDefaults();
   Bool_t PutDefaults();
   Int_t GetComSocket(Int_t attachid = 0, Int_t attachsock = 0);
   void   SetRadioButtons(Bool_t);
   Int_t IsAnalyzeRunning(Int_t);
   Int_t MessageToM_analyze(const char *);
   TGTextEntry * GetDirText(){return fTeDir;}; 
};
#endif

