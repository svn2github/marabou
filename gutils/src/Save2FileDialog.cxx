#include "TROOT.h"
#include "TCanvas.h"
#include "TCanvasImp.h"
#include "TEnv.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TRootCanvas.h"
#include "TSystem.h"
//#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "HTCanvas.h"

ClassImp(Save2FileDialog)

Save2FileDialog::Save2FileDialog(TObject * obj, const char *lname, TRootCanvas *win)
{
   static const Char_t helpText[] =
"Save object to file in current unix directory.\n\
It can be saved into a subdir in the rootfile.\n\
If it does not exist it will be created \n\
more than one level of subdirs is allowed";

   static void *valp[50];
   Int_t ind = 0;
//   Bool_t ok = kTRUE;
   fCommand = "ExecuteSave()";
   RestoreDefaults();
   SetBit(kMustCleanup);
   fKeepDialog = 0;
   fObject = obj;
   fObjName = obj->GetName();
   if (lname)
      fObjName = lname;
   else
      fObjName = obj->GetName();
   if (obj->InheritsFrom("TList")) {
      fList = (TList*)obj;
      fObjName = "";
   } else {
      fList = NULL;
   }
	cout << fObjName << endl;
	TRegexp vers(";[0-9]*");
	fObjName(vers) = "";

   TList *row_lab = new TList();
   row_lab->Add(new TObjString("StringValue_Name of Output root file"));
   row_lab->Add(new TObjString("StringValue_Name of dir in root file"));
   row_lab->Add(new TObjString("StringValue_Save object with name"));
   if (fList)
      row_lab->Add(new TObjString("CheckButton_Write functions as list"));
//   row_lab->Add(new TObjString("CheckButton_Keep Dialog"));
//   row_lab->Add(new TObjString("CommandButt_Execute Save"));
   Int_t itemwidth = 320;

   valp[ind++] = &fFileName;
   valp[ind++] = &fDir;
   valp[ind++] = &fObjName;
   if (fList)
      valp[ind++] = &fAsList;
//   valp[ind++] = &fKeepDialog;
   valp[ind++] = &fCommand;
   TRootCanvas* window = win;
   if ( window == NULL)
      win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   TString text = obj->ClassName();
   text.Prepend ("Save ");
   text.Append(" to rootfile");
   Int_t retval = -2;   //wait for answer
   fWidget = new TGMrbValuesAndText(text, NULL,
                   &retval, itemwidth, window,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, helpText);
   cout << "retval " <<retval << endl;
   if (retval >= 0) {
      ExecuteSave();
      SaveDefaults();
   } else {
      cout << "Cancelled " <<retval << endl;
   }
   delete this;
};
//_________________________________________________________________________

Save2FileDialog::~Save2FileDialog()
{
//   cout << "dtor Save2FileDialog" << endl;
};
//_________________________________________________________________________

void Save2FileDialog::ExecuteSave()
{
   TFile * outfile;
   if (!gSystem->AccessPathName(fFileName, kFileExists))
      outfile = new TFile(fFileName, "UPDATE", "Output file");
   else
      outfile = new TFile(fFileName, "RECREATE", "Output file");
   if (fDir.Length() > 0) {
      TObjArray *oa;
      oa = fDir.Tokenize("/");
      TString ndir;
      for (Int_t i = 0; i < oa->GetEntries(); i++) {
         ndir = ((TObjString*)oa->At(i))->String();
         if ( (!gDirectory->cd(ndir)) ) {
            cout << "Ignore above message, creating dir: " << ndir << endl;
            TDirectory * dir = gDirectory->mkdir(ndir, ndir);
            if (dir) {
               dir->cd();
            } else {
               cout << "Cant create: " << fDir << endl;
               return;
            }
         }
      }
   }
   if (fList) {
      if (fAsList)
         fList->Write(fObjName, 1);
      else
         fList->Write();
      outfile->Close();
   } else {
      TString sname(fObject->GetName());
      cout <<"fObject->GetName() "  << fObject->GetName() << " fObjName " << fObjName <<endl;
      TNamed *tn = NULL;
#ifdef MARABOUVERS
		HTCanvas * htc = (HTCanvas*)fObject;
      if ( fObject->InheritsFrom("HTCanvas") ) {
			htc->SetName(fObjName);
      } else {
         tn = (TNamed *)fObject;
         tn->SetName(fObjName);
      }
#endif
		cout << "Saving " << fObject->GetName() << " to: "
        << gDirectory->GetPath() <<endl;
//		fObject->Dump();
		fObject->Write();
//      fObject->Write(fObjName);
      outfile->Close();
#ifdef MARABOUVERS
      if ( htc )
         htc->SetName(sname);
#endif
      if ( tn )
         tn->SetName(sname);
   }
//   if (fKeepDialog <= 0) {
//      SaveDefaults();
//      fWidget->CloseWindowExt();
//      delete fWidget;
//      delete this;
//  }
};
//_________________________________________________________________________

void Save2FileDialog::SaveDefaults()
{
//   cout << "Save2FileDialog::SaveDefaults() " << endl;
   TEnv env(".rootrc");
	env.SetValue("Save2FileDialog.FileName", fFileName);
	env.SetValue("Save2FileDialog.Dir",      fDir);
	env.SetValue("Save2FileDialog.AsList",   fAsList);
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________

void Save2FileDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
	fFileName = env.GetValue("Save2FileDialog.FileName", "workfile.root");
	fDir = env.GetValue("Save2FileDialog.Dir", "");
   fAsList = env.GetValue("Save2FileDialog.AsList", 0);
}
//_________________________________________________________________________

void Save2FileDialog::CloseDown(Int_t /*wid*/)
{
   cout << "Save2FileDialog::CloseDown()" << endl;
   SaveDefaults();
   delete this;
}
//_______________________________________________________________________

void Save2FileDialog::CloseDialog()
{
 //  cout << "FitOneDimDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fWidget ) fWidget->CloseWindowExt();
   delete this;
}
