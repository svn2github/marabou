#ifndef TMBSCONTROL
#define TMBSCONTROL
#include "TTimer.h"
#include <TSocket.h>
#include <TObjString.h>
#include <TOrdCollection.h>

#include <iostream>
#include <iomanip>

const Int_t kHeaderLength      = 54;  
const Int_t kRunTableLength    = 20;
const Int_t kServerTableLength = 17;
const Int_t kTypesLength       = 20;
const Int_t kMapLength         = 20;
const Int_t kActiveLength      = 20;
const Int_t kTextLength        = 64;

const Int_t kStatusPort  = 6008;
const Int_t kCommandPort = 6006;
const Int_t kMessagePort = 6007;

const Char_t kReadoutVSBAddress[] = "0xf0000000";
const Char_t kEvbVSBAddress[] =     "0xf5000000";

enum eMbsStates {absent, dirty, loaded, configured, running, paused, moving};
enum eProcType {unknown, ppc, cvc};

class TMbsNode : public TNamed {

protected:

   TOrdCollection * fProcessNames;
   Int_t fHeader[kHeaderLength];
   Int_t fRunTable[kRunTableLength];
   Int_t fServerTable[kServerTableLength];
   Int_t fTypes[kTypesLength];
   Int_t fMap[kMapLength];
   Int_t fActive[kActiveLength];

   Int_t fNbytesRead;
   Int_t fEndian;
   Int_t fProcType;

public:
enum eProcType {unknown, ppc, cvc};
   TMbsNode(){};
   TMbsNode(const Char_t * node);
   ~TMbsNode(){};

   Int_t  GetStatus();
   Int_t  GetProcType(){return fProcType;};
   void   PrintHeader();
   Int_t  PrintProcesses();
   Int_t  GetNbytesRead(){ return fNbytesRead;};
//   void   Swap(Int_t * data, Int_t ndata, Int_t ie);

   Char_t fUser[kTextLength];   
   Char_t fDaqDate[kTextLength];
   Char_t fRun[kTextLength];    
   Char_t fExper[kTextLength];  
   Char_t fNode[kTextLength];   
   Char_t fRemote[kTextLength]; 
   Char_t fDisp[kTextLength];   
   Char_t fStr[kTextLength];    
   Char_t fSetup[kTextLength];  
   Char_t fMlSetup[kTextLength];
   Char_t fReadout[kTextLength];
   Char_t fRemDir[kTextLength]; 
   Char_t fDevice[kTextLength]; 
   Char_t fTapeL[kTextLength];  
   Char_t fFile[kTextLength];   
   Char_t fTrCli[kTextLength]; 
    
   Int_t iE              (){return  fHeader[0];};
   Int_t iSize           (){return  fHeader[2];};
   Int_t iSwap           (){return  fHeader[3];};
   Int_t iProcs          (){return  fHeader[4];};
   Int_t iLength         (){return  fHeader[5];};
   Int_t iTrigs          (){return  fHeader[6];};
   Int_t iDaqInit        (){return  fHeader[7];};
   Int_t iRun            (){return  fHeader[9];};
   Int_t iProcsA         (){return  fHeader[10];};
   Int_t iSetup          (){return  fHeader[11];};
   Int_t iSetMl          (){return  fHeader[12];};
   Int_t iCamTab         (){return  fHeader[13];};
   Int_t laNevents       (){return  fHeader[15];};
   Int_t laNbuffers      (){return  fHeader[16];};
   Int_t laNstreams      (){return  fHeader[17];};
   Int_t laNKbyte        (){return  fHeader[18];};
   Int_t laNEvtServEv    (){return  fHeader[19];};
   Int_t laNEvtServKb    (){return  fHeader[20];};
   Int_t laNStrServBuf   (){return  fHeader[21];};
   Int_t laNStrServKb    (){return  fHeader[22];};
   Int_t laNTapeKb       (){return  fHeader[23];};
   Int_t laNFileKb       (){return  fHeader[24];};
   Int_t laRevents       (){return  fHeader[25];};
   Int_t laRbuffers      (){return  fHeader[26];};
   Int_t laRstreams      (){return  fHeader[27];};
   Int_t laRKbyte        (){return  fHeader[28];};
   Int_t laRFileKb       (){return  fHeader[29];};
   Int_t laREvtServEv    (){return  fHeader[30];};
   Int_t laREvtServKb    (){return  fHeader[31];};
   Int_t laRStrServBuf   (){return  fHeader[32];};
   Int_t laRStrServKb    (){return  fHeader[33];};
   Int_t iTapePos        (){return  fHeader[37];};
   Int_t iTapeSize       (){return  fHeader[38];};
   Int_t iFileMax        (){return  fHeader[40];};
   Int_t iFileCur        (){return  fHeader[41];};
   Int_t iFileSize       (){return  fHeader[42];};
   Int_t iFilePos        (){return  fHeader[43];};
   Int_t iRecSize        (){return  fHeader[45];};
   Int_t iOpenFile       (){return  fHeader[47];};
   Int_t iTrigger        (){return  fHeader[50];};

//  running table
   Int_t iCollector    (){return fRunTable[1];};
   Int_t iTransport    (){return fRunTable[2];};
   Int_t iEvServ       (){return fRunTable[3];};
   Int_t iStrServ      (){return fRunTable[10];};
   Int_t iPrompt       (){return fRunTable[12];};

   Int_t iStrsrvScale    (){return fServerTable[6];};
   Int_t iStrsrvSync     (){return fServerTable[7];};
   Int_t iStrsrvNosync   (){return fServerTable[8];};
   Int_t iStrsrvKeep     (){return fServerTable[9];};
   Int_t iStrsrvNokeep   (){return fServerTable[10];};
   Int_t iStrsrvScaled   (){return fServerTable[11];};
   Int_t iEvtsrvScale    (){return fServerTable[12];};
   Int_t iEvtsrvEvents   (){return fServerTable[13];};
   Int_t iEvtsrvMaxcli   (){return fServerTable[14];};
   Int_t iEvtsrvAll      (){return fServerTable[15];};
   Int_t iEsosrvMaxcli   (){return fServerTable[16];};

ClassDef(TMbsNode,1)
};

//____________________________________________________________________

class TMbsControl : public TObject {

protected:
   TOrdCollection * fNodes;
   TOrdCollection * fNodeNames;
   TMbsNode * fPrompterNode;
   TMbsNode * fTriggerNode;
   TMbsNode * fCollectorNode;
   TString fCurNode;
   TString fCurDomain;
   TString fMBSVersion;
   TString fFullDir;
   TString fDir;
   TString fUserName;
   TString fGuiNode;
   TString fDisplay;
   TSocket * fPrompterSocket;

public:
   TMbsControl(){};
   TMbsControl(const Char_t * node,const Char_t * version, const Char_t * dir);
   ~TMbsControl(){};
   TMbsNode * PrompterNode(){return fPrompterNode;};
   TMbsNode * TriggerNode() {return fTriggerNode;};
   Bool_t PrompterConnected() {return fPrompterSocket != 0;};
   Int_t  GetNodeNames();
   Int_t  GetMbsNodes();
   Int_t  GetNofMbsProcs();
   Bool_t IdentifyMbsNodes();
   Bool_t StartMbs();
   Bool_t StopMbs();
   Bool_t ConnectPrompter(Int_t timeout = 10);
   Bool_t DisConnectPrompter();
   Int_t  SendToPrompter(const char * cmd);
   Int_t  GetFromPrompter(char * rchar, Int_t nbytes = 64);
//   Bool_t StartMessageServer(){};
   Bool_t Startup(Bool_t debug = kFALSE);
   Bool_t Reload(Bool_t debug = kFALSE);
   Bool_t  Shutdown();
   Bool_t  InitMbs();
   Bool_t  StartAcquisition();
   Bool_t  StopAcquisition();
//   void   Swap(Int_t * data, Int_t ndata, Int_t ie);
//____________________________________________________________________

ClassDef(TMbsControl,1)
};
//_____________________________________________________________________________________

class MessageServer : public TTimer{

protected:
   TSocket * fMessageSocket;
   Int_t fLogLevel;
public:
   MessageServer(const Char_t * msgnode, Long_t ms, Bool_t synch);
   ~MessageServer(){if(fMessageSocket) delete fMessageSocket;};
   void   SetLogLevel(Int_t ll){fLogLevel = ll;};
   Int_t  GetLogLevel(){return fLogLevel;};
   Bool_t Notify();
   Bool_t ReadNotify(){return Notify();}
//   void   Swap(Int_t * data, Int_t ndata, Int_t ie);
ClassDef(MessageServer,0)
};
#endif
