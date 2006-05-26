#include "TEnv.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TGMrbValuesAndText.h"
#include "HistPresent.h"
#include "HprNtuple.h"

ClassImp(HprNtuple)

HprNtuple::HprNtuple(HistPresent * hpr, TGWindow * win) 
{
   const Char_t helpText[] = "Read values from ASCII file and fill ntuple \n\
Values in variable list must ne separated by colons (:)";


   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   fHistPresent = hpr;
   fCommand = "Fill_The_Ntuple";
   fCommandHead = "Show_Head_of_File()";
   RestoreDefaults();
   TList *row_lab = new TList(); 
   row_lab->Add(new TObjString("FileRequest_Name of Inputfile"));
   row_lab->Add(new TObjString("StringValue_Name of Outputfile"));
   row_lab->Add(new TObjString("StringValue_Name of Ntuple"));
   row_lab->Add(new TObjString("StringValue_Title of Ntuple"));
   row_lab->Add(new TObjString("StringValue_List of Variables"));
   row_lab->Add(new TObjString("CommandButt_Show_Head_of_File"));
   row_lab->Add(new TObjString("CommandButt_Fill_The_Ntuple"));
   Int_t itemwidth = 320;

   valp[ind++] = &fInputFileName;
   valp[ind++] = &fOutputFileName;
   valp[ind++] = &fNtupleName;
   valp[ind++] = &fNtupleTitle;
   valp[ind++] = &fListOfVars;
   valp[ind++] = &fCommandHead;
   valp[ind++] = &fCommand;

   ok = GetStringExt("Define parameters", NULL, itemwidth, win,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0], this, this->ClassName());
};  
//_________________________________________________________________________
            
HprNtuple::~HprNtuple() 
{
   SaveDefaults();
};
//_________________________________________________________________________
            
void HprNtuple::Fill_The_Ntuple()
{
   if (gSystem->AccessPathName((const char *) fInputFileName, kFileExists)) {
      cout << "No such file: " << fInputFileName << endl;
      return;
   }
   TFile * outf = new TFile(fOutputFileName, "RECREATE");
   TNtupleD * nt = new TNtupleD(fNtupleName, fNtupleTitle, fListOfVars);
   Long64_t n = nt->ReadFile(fInputFileName);
   nt->Write();
   outf->Close();
   cout << "Rows read: " << n << endl;
   return;
};
//_________________________________________________________________________
            
void HprNtuple::SaveDefaults()
{
   cout << "HprNtuple::SaveDefaults() " << endl;
   TEnv env(".rootrc");
	env.SetValue("HistPresent.ToNtupleInputFileName"  , fInputFileName );
	env.SetValue("HistPresent.ToNtupleOutputFileName" , fOutputFileName);
	env.SetValue("HistPresent.ToNtupleNtupleName" 	  , fNtupleName	 );
	env.SetValue("HistPresent.ToNtupleNtupleTitle"	  , fNtupleTitle   );
	env.SetValue("HistPresent.ToNtupleListOfVars"	  , fListOfVars	 );
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void HprNtuple::RestoreDefaults()
{
   TEnv env(".rootrc");
	fInputFileName  = env.GetValue("HistPresent.ToNtupleInputFileName"  ,"nt.asc");
	fOutputFileName = env.GetValue("HistPresent.ToNtupleOutputFileName" ,"nt.root");
	fNtupleName 	 = env.GetValue("HistPresent.ToNtupleNtupleName" 	  ,"nt0");
	fNtupleTitle	 = env.GetValue("HistPresent.ToNtupleNtupleTitle"	  ,"My Ntuples");
	fListOfVars 	 = env.GetValue("HistPresent.ToNtupleListOfVars"	  ,"x:y:z");
}
//_________________________________________________________________________
            
void HprNtuple::Show_Head_of_File()
{
   TString cmd(fInputFileName.Data());
   cmd.Prepend("head ");
//   cmd.Append("\")");
   gSystem->Exec(cmd);
}
//_________________________________________________________________________
            
void HprNtuple::CloseDown()
{
   cout << "HprNtuple::CloseDown() " << endl;
   delete this;
}
 
