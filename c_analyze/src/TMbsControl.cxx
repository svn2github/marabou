#include "TMbsControl.h"
#include "TSystem.h"
#include "TEnv.h"

static const char sepline[] =
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
static const char setblack[]   = "\033[30m";
static const char setred[]     = "\033[31m";
static const char setgreen[]   = "\033[32m";
static const char setyellow[]  = "\033[33m";
static const char setblue[]    = "\033[34m";
static const char setmagenta[] = "\033[35m";
static const char setcyan[]    = "\033[36m";
//_________________________________________________________________________________________

void Swap(Int_t * data, Int_t ndata, Int_t ie) {
   if(ie ==1)return;
   Int_t temp, res;
   for(Int_t i=0; i < ndata; i++){
      temp = data[i];
      res  = (temp >> 24) & 0xff;
      res += (temp >> 8)  & 0xff00;
      res += (temp << 8)  & 0xff0000;
      res += (temp << 24) & 0xff000000;
      data[i] = res;
   }
}
//_____________________________________________________________________________________

ClassImp(TMbsNode)
ClassImp(TMbsControl)
ClassImp(MessageServer)

TMbsControl::TMbsControl (const Char_t * node,const Char_t * version, 
                          const Char_t * dir){
   fCurNode = node;
   
   if(fCurNode.Index(".") > 0){
      fCurDomain = node;
      fCurDomain.Remove(0,fCurNode.Index("."));
   }
   fMBSVersion = version;
   fDir = dir;
   fUserName = gSystem->Getenv("USER");
   fGuiNode  = gSystem->HostName();
   if(!(fGuiNode.Contains("."))) {
      FILE *fp = gSystem->OpenPipe("dnsdomainname", "r");
      char line[60];
      if (fgets(line,60,fp) != NULL) {
		line[strlen(line)-1]='\0';
     	 gSystem->ClosePipe(fp);
     	 fGuiNode += ".";
     	 fGuiNode += line;
		}
   }
   fDisplay = gSystem->Getenv("DISPLAY");
   if(fDisplay.Length() <=4)fDisplay.Prepend(fGuiNode.Data());
   fPrompterSocket = NULL;
   fPrompterNode = NULL;
   fTriggerNode = NULL;
   fCollectorNode = NULL;
   fNodes = new TOrdCollection();
   fNodeNames = new TOrdCollection();
   cout << sepline << endl;
   cout << setgreen << "ctor TMbsControl" << endl;
   cout << "MbsMaster  " << fCurNode << endl;
   cout << "UserName   " << fUserName << endl;
   cout << "MBSVersion " << fMBSVersion << endl;
   cout << "Directory  " << fDir << endl;
   cout << "GuiNode    " << fGuiNode << endl;
   cout << "Display    " << fDisplay << setblack<< endl;
   cout << sepline << endl;
}
//_________________________________________________________________________________________

Int_t TMbsControl::GetNofMbsProcs(){

   if(fNodeNames->GetSize() == 0) GetNodeNames();    
   if(fNodeNames->GetSize() < 1) return -1;    
   cout << setblue<< "c_mbs:  Get Process name m_*" << setblack<< endl;   
   char line[256];
   Bool_t not_mine = kFALSE;
   TObjString * obj;
   Int_t nprocs = 0;
   TIter next(fNodeNames);
   while((obj = (TObjString *)next())){
      TString cmd = "/usr/bin/rsh "; 
      cmd += obj->GetString();
      cmd += fCurDomain;
      cout << "c_mbs: on node: " <<  obj->GetString().Data() 
      << fCurDomain.Data() << endl;
      cmd += " ps ax | grep m_";
      FILE *fp = gSystem->OpenPipe(cmd.Data(), "r");
      while (fgets(line,256,fp) != 0) {
         line[strlen(line)-1]='\0';
         cout << line << endl;
         TString sl(line);
         if(!sl.Contains(fUserName))not_mine = kTRUE;
         nprocs ++;
      }
      gSystem->ClosePipe(fp); 
   }
   if(not_mine) nprocs += 1000;
   return nprocs;
}
//_________________________________________________________________________________________

Int_t TMbsControl::GetNodeNames(){
   TString dir; 
   TString cmd; 
   FILE *fp;
   char line[60];
   if (!fDir.BeginsWith("/")) {
      cmd = "/usr/bin/rsh "; cmd += fCurNode; cmd += " pwd";
      cout << cmd << endl;
      fp = gSystem->OpenPipe(cmd.Data(), "r");
      fgets(line,60,fp); 
      if(strlen(line)>2) line[strlen(line)-1]='\0';
      else return -1;
      dir = line; 
      dir += "/"; dir += fDir;
      gSystem->ClosePipe(fp);
   } else {
      dir = fDir;
   } 
   cout << setblue<< "c_mbs:  Get Node names from " << fDir << "/node_list.txt"<< setblack << endl;   

   cmd = "/usr/bin/rsh "; cmd += fCurNode;  cmd += " cat ";
   cmd += dir; cmd += "/node_list.txt";
   fp = gSystem->OpenPipe(cmd.Data(), "r");
   while (fgets(line,60,fp) != 0) {
      line[strlen(line)-1]='\0';
      cout << "Node: " << line;
      if(strstr(line, "Unknown")) cout << setred<< "!!!!!!!!!!!!!!"; 
      else fNodeNames->Add(new TObjString(line));
      cout << setblack<< endl;
   }
   gSystem->ClosePipe(fp); 
   return fNodeNames->GetSize();
}
//_________________________________________________________________________________________

Int_t TMbsControl::GetMbsNodes(){
   Int_t Endian, Status;
   if(!fPrompterSocket)if(!ConnectPrompter(10))return -1;
//   cout << setblue<< "c_mbs: Getting node list"<< setblack<< endl;
   Int_t nlines = SendToPrompter("$nodes");
   if(nlines < 2){
      cout<< setred << "failed, nlines = " << nlines << setblack<< endl;
      return -1;
   }

//   cout<< setgreen<< "ok, nlines = " << nlines<< setblack << endl;
   char * line = new char[64];
   Int_t nbytes;
   TMbsNode * mbsnode;
   for(Int_t i=0; i < nlines-1; i++){
      nbytes = fPrompterSocket->RecvRaw(line, 64);
      if(nbytes <= 0){cout<< setred<< " cant get node name " << setblack<< endl; return kFALSE;};
      cout << "c_mbs: Node: " << line << endl;
      mbsnode = new TMbsNode(line);
      fNodes->Add(mbsnode);
   }
   nbytes = fPrompterSocket->RecvRaw(line, 64);
   cout << "c_mbs: Dir:  " << line << endl;
   fFullDir = line;
//   cout << "c_mbs: Dir: " << line << endl;
   fPrompterSocket->RecvRaw(&Endian, 4);
   fPrompterSocket->RecvRaw(&Status, 4);
   Swap(&Status, 1, Endian);
   return nlines;
}
//_________________________________________________________________________________________

Bool_t TMbsControl:: IdentifyMbsNodes(){
  if(!fNodes){cout << setred<< "c_mbs: No nodes known" << setblack<< endl; return kFALSE;};
  TMbsNode * mbsnode;
  if (fNodes->GetSize() > 1) {
     TIter next(fNodes);
     while((mbsnode = (TMbsNode*)next())){
        mbsnode->GetStatus();
//        mbsnode->PrintHeader(); 
        if(mbsnode->iTrigger()){
           fTriggerNode = mbsnode;
           cout << "c_mbs: TriggerNode: " << mbsnode->GetName()<< endl;
        }         
        if(mbsnode->iPrompt()){
           fPrompterNode = mbsnode;         
           cout << "c_mbs: PrompterNode: " << mbsnode->GetName()<< endl;
        }
        if(mbsnode->iCollector()){
           fCollectorNode = mbsnode;         
           cout << "c_mbs: CollectorNode: " << mbsnode->GetName()<< endl;
        }
      }
   } else {
      mbsnode = (TMbsNode *) fNodes->First();
      fTriggerNode = fPrompterNode = fCollectorNode = mbsnode;
      cout << "c_mbs: TriggerNode: " << mbsnode->GetName()<< endl;
      cout << "c_mbs: PrompterNode: " << mbsnode->GetName()<< endl;
      cout << "c_mbs: CollectorNode: " << mbsnode->GetName()<< endl;
   }
   return kTRUE;
};
//_________________________________________________________________________________________

Bool_t  TMbsControl::StartMbs(){

   TString rshCmd;

   if(fNodeNames->GetSize() == 0) GetNodeNames();
   if(fNodeNames->GetSize() < 1) {
      cout << setred << "Cant get node names" << setblack<< endl;
      return kFALSE;
   } else {
      cout << setgreen << "Got " << fNodeNames->GetSize() << " node"; 
      if(fNodeNames->GetSize() > 1) cout << "s";
      cout << setblack<< endl;
   }
   TString prnode = ((TObjString*)fNodeNames->At(0))->GetString();
   TString cunode = fCurNode;

   if(cunode.Index(".") > 0){
      cunode.Remove(cunode.Index("."), 100);
   }
   if(prnode != cunode){
      cout << setred << "Prompter node names mismatch:" << endl;
      cout << "In node_list.txt:  " << prnode.Data()<< endl;
      cout << "In Control window: " << cunode.Data()<< setblack<< endl;
      return kFALSE;
   }
//   cout << " StartMbs(), prnode: " << prnode.Data() << endl;
   TString trnode;
   if(fNodeNames->GetSize() > 1)
      trnode = ((TObjString*)fNodeNames->At(1))->GetString();
   else trnode = prnode;
   cout << " StartMbs() trnode: " << trnode.Data() << endl;

	if(!strncmp(prnode.Data(),"ppc",3)){
		cout << "Later: Got " << fNodeNames->GetSize() << " node"<< endl; 
   
		if (gEnv->GetValue("TMbsSetup.ConfigVSB", kTRUE)) {
			rshCmd = "/usr/bin/rsh ";
			rshCmd = rshCmd  + trnode.Data() + " -l " + fUserName.Data() +
              " /bin/ces/vsbini "+ kReadoutVSBAddress;
      		cout << setmagenta<< rshCmd << setblack<< endl;
     	 	gSystem->Exec(rshCmd.Data());
      		if(fNodeNames->GetSize() > 1){
				rshCmd = "/usr/bin/rsh ";
				rshCmd = rshCmd  + prnode.Data() + " -l " + fUserName.Data() +
                 " /bin/ces/vsbini "+ kEvbVSBAddress;
				cout << setmagenta<< rshCmd << setblack<< endl;
				gSystem->Exec(rshCmd.Data());
     		}
		}
		if(fNodeNames->GetSize() == 1){
			rshCmd = "/usr/bin/rsh ";
			rshCmd = rshCmd  + trnode.Data() + " -l " + fUserName.Data() +
                " /bin/ces/vmeconfig -a /mbs/driv/trig-vme_2.5_RIO2/vmetab";
			cout << setmagenta<< rshCmd << setblack<< endl;
			gSystem->Exec(rshCmd.Data());
		} 
	}

   rshCmd = "/usr/bin/rsh ";
   rshCmd = rshCmd  + fCurNode.Data() + " -l " + fUserName.Data() + 
            " /mbs/" + fMBSVersion.Data() + "/script/mbsstartup.sc " + 
            fMBSVersion.Data()+" "+ fDir.Data() +" "+ fGuiNode.Data();
   cout << setmagenta<< rshCmd << setblack<< endl;
   gSystem->Exec(rshCmd.Data());
   gSystem->Sleep(2000);      // wait 2 seconds
   Int_t SecsToWait = gEnv->GetValue("TMrbAnalyze.WaitForPrompter", 10);
	cout << setblue<< "c_mbs: Connecting prompter, (max " << SecsToWait << " sec) " << endl;
   if(!ConnectPrompter(SecsToWait)) {cout << setred<< " failed !!!!!!!! " << setblack<< endl; return kFALSE;};
   cout << setgreen<< " succeeded "<< setblack << endl;
   if(GetMbsNodes() < 1) return kFALSE;
//   cout << "c_mbs: Status: " << Status << endl;
   TString cmd = "set xdisp ";
   cmd += fDisplay;
   SendToPrompter(cmd.Data());
//   Int_t status = SendToPrompter(cmd.Data());
//   cout << "Status: " << status << endl;
   return kTRUE;
}
//_________________________________________________________________________________________

Bool_t  TMbsControl::StopMbs(){
   TString rshCmd;
   Bool_t ok = kTRUE;
   if(PrompterConnected()){
      ok = SendToPrompter("$exit");
      ok = DisConnectPrompter();
   }
   rshCmd = "/usr/bin/rsh ";
   rshCmd = rshCmd  + fCurNode.Data() + " -l " + fUserName.Data() + 
            " /mbs/" + fMBSVersion.Data() + "/script/mbsshutall.sc " + 
            fMBSVersion.Data()+" "+ fDir.Data();
   cout << setmagenta << rshCmd << setblack<< endl;
   gSystem->Exec(rshCmd.Data());
   cout << setgreen<< " c_mbs: --------------- Reset done  --------------------" << setblack<< endl;
   return ok;
}
//_________________________________________________________________________________________

Bool_t TMbsControl:: Startup(Bool_t debug){
   Int_t ok;
   if(debug){
      cout << setblue << "c_mbs: Starting DAQ processes in debug mode" << setblack << endl;
      ok = SendToPrompter("@startup_d");
   } else {
      cout << setblue << "c_mbs: Starting DAQ processes " << setblack << endl;
      ok = SendToPrompter("@startup");
   }
   if(ok == 0){
      cout << setgreen<< "c_mbs: startup ok " << setblack<< endl;
      IdentifyMbsNodes();
      return kTRUE;
   } else {
      cout << setred<< "c_mbs: startup failed, code: " 
           << setbase(16) << ok << setblack<< endl;
      return kFALSE;
   }
}
//_________________________________________________________________________________________

Bool_t TMbsControl:: Reload(Bool_t debug){
   Int_t ok;
   if(debug){
      cout << setblue << "c_mbs: Reloading m_read_meb in debug mode" << setblack << endl;
      ok = SendToPrompter("@reload_meb_d");
   } else {
      cout << setblue << "c_mbs: Reloading m_read_meb" << setblack << endl;
      ok = SendToPrompter("@reload_meb");
   }
   if(ok == 0){
      cout << setgreen<< "c_mbs: reload ok " << setblack<< endl;
      IdentifyMbsNodes();
      return kTRUE;
   } else {
      cout << setred<< "c_mbs: reload failed, code: " 
           << setbase(16) << ok << setblack<< endl;
      return kFALSE;
   }
}
//_________________________________________________________________________________________

Bool_t  TMbsControl::Shutdown(){
   if(SendToPrompter("@shutdown") == 0) return kTRUE;
   else                                 return kFALSE;
};
//_________________________________________________________________________________________

Bool_t  TMbsControl::InitMbs(){
   if(SendToPrompter("@init") == 0) return kTRUE;
   else                             return kFALSE;
};
//_________________________________________________________________________________________

Bool_t  TMbsControl::StartAcquisition(){
   Int_t stat;
   cout << setblue << "Enable event triggers" << setblack << endl;
   if(fTriggerNode){
      TString setnode = "set disp "; setnode += fTriggerNode->GetName();
      SendToPrompter(setnode.Data()); 
   } else {
      cout << setred<< "c_mbs: TriggerNode not set, probably no @Startup given" << setblack<< endl;
      return kFALSE;
   }  
   stat = SendToPrompter("sta ac");
   if(fPrompterNode){
      TString setnode = "set disp "; setnode += fPrompterNode->GetName();
      SendToPrompter(setnode.Data()); 
   }
   if(stat == 0) return kTRUE;
   else          return kFALSE;
};
//_________________________________________________________________________________________

Bool_t  TMbsControl::StopAcquisition() {
   Int_t stat;
   cout << setblue << "Disable event triggers" << setblack << endl;
   if(fTriggerNode){
      TString setnode = "set disp "; setnode += fTriggerNode->GetName();
      stat = SendToPrompter(setnode.Data()); 
   }
   stat = SendToPrompter("sto ac");
   if(fPrompterNode){
      TString setnode = "set disp "; setnode += fPrompterNode->GetName();
      SendToPrompter(setnode.Data()); 
   }
   if(stat == 0) return kTRUE;
   else          return kFALSE;
};
//_________________________________________________________________________________________

Bool_t TMbsControl:: ConnectPrompter(Int_t tmout){
   Int_t timeout = tmout;
   while(timeout > 0){
      fPrompterSocket = new TSocket(fCurNode.Data(), kCommandPort);
      if(fPrompterSocket->IsValid()) return kTRUE;

      timeout --;
      delete fPrompterSocket;
      fPrompterSocket = 0;
      gSystem->Sleep(1000);      // wait one second
   }
   cout << setred<< "c_mbs: Cant access prompter on: " << fCurNode.Data() 
        << " at port: " << kCommandPort << setblack<< endl;
   return kFALSE;
}
//_________________________________________________________________________________________

Bool_t TMbsControl:: DisConnectPrompter(){
   if(fPrompterSocket){
      cout << setblue<< "c_mbs: Close connection to prompter on: " << fCurNode.Data() 
           << " at port: " << kCommandPort << setblack<< endl;
      fPrompterSocket->Close();
      delete fPrompterSocket;
      fPrompterSocket = 0;
      return kTRUE;
   } else {
      cout << setred << "c_mbs: No connection open to prompter on: " << fCurNode.Data() 
           << " at port: " << kCommandPort  << setblack<< endl;
      return kFALSE;
   }
}
//_________________________________________________________________________________________

Int_t TMbsControl:: SendToPrompter(const char * cmd){
   if(fPrompterSocket){
      Int_t Endian, Status;
      TString command(fGuiNode.Data());
      command=command + ":" + cmd + "\0";
      cout << "c_mbs: Send to prompter: "<< setmagenta << command.Data()<< setblack << endl;
      Int_t last = command.Length();
      command.Resize(256);
      command[last] = '\0';
      fPrompterSocket->SendRaw(command.Data(), 256);
      fPrompterSocket->RecvRaw(&Endian, 4);
      fPrompterSocket->RecvRaw(&Status, 4);
      Swap(&Status, 1, Endian); 
//      cout << "Status: " << Status << endl;
      return Status;
   } else return -1;
}
//_________________________________________________________________________________________

Int_t TMbsControl:: GetFromPrompter(char * rchar, Int_t nbytes){
   if(fPrompterSocket){
      return fPrompterSocket->RecvRaw(rchar, nbytes);
   } else return -1;
}
//_________________________________________________________________________________________

/*void TMbsControl::Swap(Int_t * data, Int_t ndata, Int_t ie) {
   if(ie ==1)return;
   Int_t temp, res;
   for(Int_t i=0; i < ndata; i++){
      temp = data[i];
      res  = (temp >> 24) & 0xff;
      res += (temp >> 8)  & 0xff00;
      res += (temp << 8)  & 0xff0000;
      res += (temp << 24) & 0xff000000;
      data[i] = res;
   }
}*/
//_________________________________________________________________________________________
//_________________________________________________________________________________________

TMbsNode::TMbsNode (const Char_t * node) : TNamed(node, "MbsNode") {

   for(Int_t i=0; i < kHeaderLength; i++)      fHeader[i] = 0;
   for(Int_t i=0; i < kRunTableLength; i++)    fRunTable[i] = 0;
   for(Int_t i=0; i < kServerTableLength; i++) fServerTable[i] = 0;
   for(Int_t i=0; i < kTypesLength; i++)       fTypes[i] = 0;
   for(Int_t i=0; i < kMapLength; i++)         fMap[i] = 0;
   for(Int_t i=0; i < kActiveLength; i++)      fActive[i] = 0;
   if      (!strncmp(node,"cvc",3))fProcType = TMbsNode::cvc;
   else if (!strncmp(node,"ppc",3))fProcType = TMbsNode::ppc;
   else                            fProcType = TMbsNode::unknown;
   fProcessNames = new TOrdCollection();
//   cout << "ctor TMbsNode " << node << endl;
}
//_________________________________________________________________________________________

Int_t TMbsNode::GetStatus(){

   Int_t start = 1;
   Int_t nrec = 0;
   fNbytesRead = 0;
   Int_t iDummy;
   for(Int_t i=0; i < kHeaderLength; i++)      fHeader[i] = 0;
   for(Int_t i=0; i < kRunTableLength; i++)    fRunTable[i] = 0;
   for(Int_t i=0; i < kServerTableLength; i++) fServerTable[i] = 0;
   for(Int_t i=0; i < kTypesLength; i++)       fTypes[i] = 0;
   for(Int_t i=0; i < kMapLength; i++)         fMap[i] = 0;
   for(Int_t i=0; i < kActiveLength; i++)      fActive[i] = 0;

   TSocket *sock = new TSocket(GetName(), kStatusPort);
   if(!sock->IsValid()){
      cout << setred<< "cant access node: " <<  GetName()
           << " at port: " << kStatusPort << setblack<< endl;
      return -1;
   }
   sock->SendRaw(&start, 4);
   nrec = sock->RecvRaw(fHeader, kHeaderLength*4);
   if(nrec <=0)    return kFALSE;
   else{
      fEndian = fHeader[0];
      if(fEndian != 1)Swap(fHeader, kHeaderLength, fEndian);
      fNbytesRead  += nrec;
   }
// read trigger counts
   for(int i = 0; i < iTrigs() ; i++) sock->RecvRaw(&iDummy, 4);
// read invalid trigger counts
   for(int i = 0; i < iTrigs() ; i++) sock->RecvRaw(&iDummy, 4);
// read valid trigger counts
   for(int i = 0; i < iTrigs() ; i++) sock->RecvRaw(&iDummy, 4);

   nrec = sock->RecvRaw(fRunTable, kRunTableLength*4);
   if(nrec <=0)    return kFALSE;
   else{
      if(fEndian != 1)Swap(fRunTable, kRunTableLength, fEndian);
      fNbytesRead  += nrec;
   }
// read pid table
  for(int i = 0; i < iProcs(); i++) sock->RecvRaw(&iDummy, 4);

   nrec = sock->RecvRaw(fTypes, iProcs()*4);
   if(nrec <=0)    return kFALSE;
   else{
      if(fEndian != 1)Swap(fTypes, iProcs(), fEndian);
      fNbytesRead  += nrec;
   }
// read prio table
  for(int i = 0; i < iProcs() ; i++) sock->RecvRaw(&iDummy, 4);

   nrec = sock->RecvRaw(fActive, iProcs()*4);
   if(nrec <=0)    return kFALSE;
   else{
      if(fEndian != 1)Swap(fActive, iProcs(), fEndian);
//      cout << "fActive[0],[1] " << fActive[0] << " " << fActive[1] << endl;
      fNbytesRead  += nrec;
   }

   nrec = sock->RecvRaw(fServerTable, kServerTableLength*4);
   if(nrec <=0)    return kFALSE;
   else{
      if(fEndian != 1)Swap(fServerTable, kServerTableLength, fEndian);
      fNbytesRead  += nrec;
   }


   nrec = sock->RecvRaw(fUser, kTextLength);   fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fDaqDate, kTextLength);fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fRun, kTextLength);    fNbytesRead  += nrec;  
   nrec = sock->RecvRaw(fExper, kTextLength);  fNbytesRead  += nrec; 
   nrec = sock->RecvRaw(fNode, kTextLength);   fNbytesRead  += nrec; 
   nrec = sock->RecvRaw(fRemote, kTextLength); fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fDisp, kTextLength);   fNbytesRead  += nrec;  
   nrec = sock->RecvRaw(fStr, kTextLength);    fNbytesRead  += nrec;  
   nrec = sock->RecvRaw(fSetup, kTextLength);  fNbytesRead  += nrec; 
   nrec = sock->RecvRaw(fMlSetup, kTextLength);fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fReadout, kTextLength);fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fRemDir, kTextLength); fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fDevice, kTextLength); fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fTapeL, kTextLength);  fNbytesRead  += nrec; 
   nrec = sock->RecvRaw(fFile, kTextLength);   fNbytesRead  += nrec;
   nrec = sock->RecvRaw(fTrCli, kTextLength);  fNbytesRead  += nrec;   

   fProcessNames->Delete();            // clear list, delete old names
   for(Int_t i = 0; i < iProcsA(); i++){
      Char_t * pname = new Char_t[kTextLength];
      nrec = sock->RecvRaw(pname, kTextLength);  fNbytesRead  += nrec;
      fProcessNames->Add(new TObjString(pname));
//      cout << pname << endl;
      delete pname;
   }   
   sock->Close();
   delete sock;
   return laNevents();
}
//_________________________________________________________________________________________

void TMbsNode::PrintHeader(){
      cout << "HEADER"  << endl;
      cout << "==================================================="  << endl;
      cout << "iE            "   <<fEndian             << endl;
      cout << "iSize         "   <<iSize()           << endl;
      cout << "iSwap         "   <<iSwap()           << endl;
      cout << "iProcs        "   <<iProcs()          << endl;
      cout << "iLength       "   <<iLength()         << endl;
      cout << "iTrigs        "   <<iTrigs()          << endl;
      cout << "iDaqInit      "   <<iDaqInit()        << endl;
      cout << "iRun          "   <<iRun()            << endl;
      cout << "iProcsA       "   <<iProcsA()         << endl;
      cout << "iSetup        "   <<iSetup()          << endl;
      cout << "iSetMl        "   <<iSetMl()          << endl;
      cout << "iCamTab       "   <<iCamTab()         << endl;
      cout << "laNevents     "   <<laNevents()       << endl;
      cout << "laNbuffers    "   <<laNbuffers()      << endl;
      cout << "laNstreams    "   <<laNstreams()      << endl;
      cout << "laNKbyte      "   <<laNKbyte()        << endl;
      cout << "laNEvtServEv  "   <<laNEvtServEv()    << endl;
      cout << "laNEvtServKb  "   <<laNEvtServKb()    << endl;
      cout << "laNStrServBuf "   <<laNStrServBuf()   << endl;
      cout << "laNStrServKb  "   <<laNStrServKb()    << endl;
      cout << "laNTapeKb     "   <<laNTapeKb()       << endl;
      cout << "laNFileKb     "   <<laNFileKb()       << endl;
      cout << "laRevents     "   <<laRevents()       << endl;
      cout << "laRbuffers    "   <<laRbuffers()      << endl;
      cout << "laRstreams    "   <<laRstreams()      << endl;
      cout << "laRKbyte      "   <<laRKbyte()        << endl;
      cout << "laRFileKb     "   <<laRFileKb()       << endl;
      cout << "laREvtServEv  "   <<laREvtServEv()    << endl;
      cout << "laREvtServKb  "   <<laREvtServKb()    << endl;
      cout << "laRStrServBuf "   <<laRStrServBuf()   << endl;
      cout << "laRStrServKb  "   <<laRStrServKb()    << endl;
      cout << "iTapePos      "   <<iTapePos()        << endl;
      cout << "iTapeSize     "   <<iTapeSize ()      << endl;
      cout << "iFileMax      "   <<iFileMax()        << endl;
      cout << "iFileCur      "   <<iFileCur()        << endl;
      cout << "iFileSize     "   <<iFileSize()       << endl;
      cout << "iFilePos      "   <<iFilePos()        << endl;
      cout << "iRecSize      "   <<iRecSize()        << endl;
      cout << "iOpenFile     "   <<iOpenFile()       << endl;
      cout << "iTrigger      "   <<iTrigger()        << endl;
      cout << "RUN TABLE"  << endl;
      cout << "==================================================="  << endl;
      cout << "iCollector    "   << fRunTable[1]<< endl;
      cout << "iTransport    "   << fRunTable[2]<< endl;
      cout << "iEvServ       "   << fRunTable[3]<< endl;
      cout << "iStrServ      "   << fRunTable[10]<< endl;
      cout << "iPrompt       "   << fRunTable[12]<< endl;
      cout << "SERVER TABLE"  << endl;
      cout << "==================================================="  << endl;
      cout << "iStrsrvScale    "   << fServerTable[6]<< endl;
      cout << "iStrsrvSync     "   << fServerTable[7]<< endl;
      cout << "iStrsrvNosync   "   << fServerTable[8]<< endl;
      cout << "iStrsrvKeep     "   << fServerTable[9]<< endl;
      cout << "iStrsrvNokeep   "   << fServerTable[10]<< endl;
      cout << "iStrsrvScaled   "   << fServerTable[11]<< endl;
      cout << "iEvtsrvScale    "   << fServerTable[12]<< endl;
      cout << "iEvtsrvEvents   "   << fServerTable[13]<< endl;
      cout << "iEvtsrvMaxcli   "   << fServerTable[14]<< endl;
      cout << "iEvtsrvAll      "   << fServerTable[15]<< endl;
      cout << "iEsosrvMaxcli   "   << fServerTable[16]<< endl;

}
//_________________________________________________________________________________________

Int_t TMbsNode::PrintProcesses(){
   TIter next(fProcessNames);
   Int_t iproc =0;
   TObjString * obj;
   while((obj = (TObjString *)next())){ 
     cout << setw(35) << obj->GetString();    
     if(fActive[iproc] != 1) cout << " not ";
     else                    cout << "     ";
     cout << " running" << endl;
     iproc++;
   }
   return iproc;
}
//_________________________________________________________________________________________

/*void TMbsNode::Swap(Int_t * data, Int_t ndata, Int_t ie) {
   if(ie ==1)return;
   Int_t temp, res;
   for(Int_t i=0; i < ndata; i++){
      temp = data[i];
      res  = (temp >> 24) & 0xff;
      res += (temp >> 8)  & 0xff00;
      res += (temp << 8)  & 0xff0000;
      res += (temp << 24) & 0xff000000;
      data[i] = res;
   }
}*/
//_____________________________________________________________________________________
//
MessageServer::MessageServer(const char *msgnode, Long_t ms, Bool_t synch)
              :TTimer(ms,synch){
   fMessageSocket = new TSocket(msgnode, kMessagePort);
   if(fMessageSocket->IsValid()){
      fMessageSocket->SetOption(kNoBlock, 1);
      gSystem->AddTimer(this);
      cout << "MessageServer ctor: Init Message timer,  rep rate: " << ms << " msec"<< endl;
      fLogLevel = 1;
   } else {
      cout << "MessageServer ctor: Socket not valid" << endl;
      fLogLevel = -1;
      if(fMessageSocket){delete fMessageSocket; fMessageSocket=0;}
   }

}
//_________________________________________________________________________________________

Bool_t MessageServer::DisConnect(){
   if(fMessageSocket){
      cout << setblue<< "c_mbs: Close connection to : "  <<fMessageSocket  << setblack<< endl;
      fMessageSocket->Close();
      delete fMessageSocket;
      fMessageSocket = 0;
      return kTRUE;
   } else {
      cout << setred << "c_mbs: No connection open to: " << fMessageSocket << setblack<< endl;
      return kFALSE;
   }
}
//_________________________________________________________________________________________

Bool_t MessageServer::Notify() {

   if(fLogLevel <= 0)return kTRUE;
   static char mess[256];
   Int_t nb = 0;
   Int_t endian, len, type;

//   cout << " Notify reached, fLogLevel: " << fLogLevel<< endl;
//   Reset();
   if(fMessageSocket){
      while(1){
         nb = fMessageSocket->RecvRaw(&endian, 4); 
         if(nb <= 0) break;
         Swap( &endian, 1, endian);
         nb = fMessageSocket->RecvRaw(&type, 4);
         Swap( &type, 1, type);
         nb = fMessageSocket->RecvRaw(&len, 4);
         Swap( &len, 1, len);
//         cout << "endian "<< endian<< " type " << type << " len " << len<<endl;
         if(nb <= 0) break;
         if(len > 0){
            nb = fMessageSocket->RecvRaw(&mess[0], 256); 
//            if(nb>0)cout << "Log(len): "<< len << ": " << &mess[0] << endl;
            if(nb>0){
               if(strstr(mess, "SIGBUS") || strstr(mess, "recover")
               || strstr(mess, "removed")){
                  cout << setred << &mess[0]<< setblack << endl;
                  if(strstr(mess, "SIGBUS")){
                     cout <<  setred << "c_mbs: !!!! Check hardware " << endl;
                     cout << "c_mbs: !!!! Could be wrong Trigger module (CAMAC/VME)" 
                       << setblack << endl;
                  }
               } else if (fLogLevel > 1 || strstr(mess, "read_meb")){
                  if(strstr(mess, "read_meb"))cout << setgreen;
                  cout << "Mbs:  " << &mess[0]<< setblack << endl;
               }
            }
         } else break;
      }
   }
//   SetTime(1000);
   Reset();
//   cout << " back from runloop" << endl;
   return kTRUE;
};
//_________________________________________________________________________________________

/*void MessageServer::Swap(Int_t * data, Int_t ndata, Int_t ie) {
   if(ie ==1)return;
   Int_t temp, res;
   for(Int_t i=0; i < ndata; i++){
      temp = data[i];
      res  = (temp >> 24) & 0xff;
      res += (temp >> 8)  & 0xff00;
      res += (temp << 8)  & 0xff0000;
      res += (temp << 24) & 0xff000000;
      data[i] = res;
   }
}*/
