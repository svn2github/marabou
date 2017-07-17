#include "TROOT.h"
#include "TCanvas.h"
#include "TCanvasImp.h"
#include "TEnv.h"
#include "TFile.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TRootCanvas.h"
#include "TSystem.h"
#include "TH1.h"
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
more than one level of subdirs is allowed\n\
If the object is a TList it may be saved as \n\
a TList optionally with a name or as single objects\n\
in which case there names are the original ones.\n\
";

   static void *valp[50];
   Int_t ind = 0;
//   Bool_t ok = kTRUE;
	fCanceled = 0;
   fCommand = "ExecuteSave()";
   RestoreDefaults();
   SetBit(kMustCleanup);
   fKeepDialog = 0;
   fHistInRange = 0;
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
//	cout << "lname " << lname << "fObjName " << fObjName << endl;
	TRegexp vers(";[0-9]*");
	fObjName(vers) = "";

   TList *row_lab = new TList();
   row_lab->Add(new TObjString("StringValue_Output root file"));
   valp[ind++] = &fFileName;
   row_lab->Add(new TObjString("StringValue_Dir in root file\
&ROOT files may have directories.n\
If this name is not blank\n\
object will be saved in this dir.\n\
If not existing it will be created"));
   valp[ind++] = &fDir;
   row_lab->Add(new TObjString("StringValue_Save with name\
&Default is to save with same name\n\
a new name may be given here"));
	fBidObjName = ind;
   valp[ind++] = &fObjName;
//   if ( obj->InheritsFrom("TH1") ){ 
//      row_lab->Add(new TObjString("CheckButton_Save hist with current ranges only"));
//		valp[ind++] = &fHistInRange;
//	}
   if (fList) {
      row_lab->Add(new TObjString("CheckButton_Write functions as list"));
      fBidAsList = ind;
      valp[ind++] = &fAsList;
	}
   Int_t itemwidth = 320;

//   valp[ind++] = &fCommand;
   TRootCanvas* window = win;
   if ( window == NULL)
      win = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   TString text = obj->ClassName();
   text.Prepend ("Save ");
   text.Append(" to rootfile");
   Int_t retval = -2;   //wait for answer
   fDialog = new TGMrbValuesAndText(text, NULL,
                   &retval, itemwidth, window,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, helpText);
 //                  NULL, NULL, helpText, this, this->ClassName());
   cout << "retval " <<retval << endl;
   if (retval >= 0) {
      ExecuteSave();
      SaveDefaults();
   } else {
      cout << "Cancelled " <<retval << endl;
      fCanceled = 1;
   }
//   delete this;
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
		TObject *obj;
		TIter next((TList*)fList);
		while ( (obj = next()) ) {
			cout << "Writing: " << obj->GetName() << endl;
		}
      if (fAsList) {
			if (fObjName.Length() == 0) {
				fObjName = "func_list";
			}
			cout << endl 
			<< "Warning: List will not yet be displayed by HistPresent"
			<< endl << "Stored with name " << fObjName 
			<< endl;
         fList->Write(fObjName, 1);
      } else {
         fList->Write();
      }
      outfile->Close();
      return;
   }
   TNamed * objorig = 0;
   if (fObject->InheritsFrom("TNamed"))
		objorig = (TNamed*)fObject;
	/*
	if ( fObject->InheritsFrom("TH1") ) {
		TH1* ohist = (TH1*)fObject;
		cout << "Save2FileDialog: TH1 " << ohist->GetName() << endl;
		TAxis *xa = ohist->GetXaxis();
		TAxis *ya = NULL;
		if (ohist->GetDimension() > 1)
			ya = ohist->GetYaxis();
		TAxis *za = NULL;
		if (ohist->GetDimension() > 2)
			za = ohist->GetZaxis();
			
		Bool_t fulld = xa->GetFirst() == 1 && xa->GetLast() == xa->GetNbins();
		if (fulld && ya)
			fulld = ya->GetFirst() == 1 && ya->GetLast() == ya->GetNbins();
		if (fulld && za)
			fulld = za->GetFirst() == 1 && za->GetLast() == za->GetNbins();
		if ( !fulld ) {	
			TH1* nhist = (TH1*)ohist->Clone();
			nhist->Reset();
			Int_t nx1 =  xa->GetFirst();
			Int_t nx2 =  xa->GetLast();
			if (ohist->GetDimension() == 1) {
				nhist->SetBins(nx2 - nx1 + 1,
						xa->GetBinLowEdge(nx1),
						xa->GetBinLowEdge(nx2)+ xa->GetBinWidth(nx2));
				Int_t n=1;
				for (Int_t i = nx1; i <= nx2; i++ ) {
					nhist->SetBinContent(n, ohist->GetBinContent(i));
					n++;
				}
			} else {
				Int_t ny1 =  ya->GetFirst();
				Int_t ny2 =  ya->GetLast();
				if ( ohist->GetDimension() == 2) {
					nhist->SetBins(nx2 - nx1 + 1,
						xa->GetBinLowEdge(nx1),
						xa->GetBinLowEdge(nx2)+ xa->GetBinWidth(nx2),
						ny2 - ny1 + 1,
						ya->GetBinLowEdge(ny1),
						ya->GetBinLowEdge(ny2)+ ya->GetBinWidth(ny2));
					Int_t nx=1;
					for (Int_t i = nx1; i <= nx2; i++ ) {
						Int_t ny=1;
						for (Int_t j = ny1; j <= ny2; j++ ) {
							nhist->SetBinContent(nx, ny, ohist->GetBinContent(i,j));
							ny++;
						}
						nx++;
					}
				} else {
					Int_t nz1 =  za->GetFirst();
					Int_t nz2 =  za->GetLast();
					nhist->SetBins(nx2 - nx1 + 1,
						xa->GetBinLowEdge(nx1),
						xa->GetBinLowEdge(nx2)+ xa->GetBinWidth(nx2),
						ny2 - ny1 + 1,
						ya->GetBinLowEdge(ny1),
						ya->GetBinLowEdge(ny2)+ ya->GetBinWidth(ny2),
						nz2 - nz1 + 1,
						za->GetBinLowEdge(nz1),
						za->GetBinLowEdge(nz2)+ za->GetBinWidth(nz2));
					Int_t nx=1;
					for (Int_t i = nx1; i <= nx2; i++ ) {
						Int_t ny = 1;
						for (Int_t j = ny1; j <= ny2; j++ ) {
							Int_t nz = 1;
							for (Int_t k = nz1; k <= nz2; k++ ) {
								nhist->SetBinContent(nx, ny, nz, ohist->GetBinContent(i,j,k));
								nz++;
							}
							ny++;
						}
						nx++;
					}
				}
			}
			fObject = nhist;
		}
	}
	*/
	TString sname;
	TNamed * tn = NULL;
	if ( fObject->InheritsFrom("TNamed") ) {
		sname = fObject->GetName();
		if ( sname != fObjName) {
			tn = (TNamed *)fObject;
			tn->SetName(fObjName);
//			cout << "tn->SetName(fObjName) " << fObjName<< endl;
		}
	}
	fObject->Write();
	cout << "Write object with name: " << fObject->GetName() << endl;
	outfile->Close();
	if ( tn && fObject == objorig) {
//		cout << "objorig->SetName(smame) " << sname << endl;
		objorig->SetName(sname);
	}
};
//_________________________________________________________________________

void Save2FileDialog::SaveDefaults()
{
//   cout << "Save2FileDialog::SaveDefaults() " << endl;
   TEnv env(".hprrc");
	env.SetValue("Save2FileDialog.FileName", fFileName);
	env.SetValue("Save2FileDialog.fObjName", fObjName);
	env.SetValue("Save2FileDialog.Dir",      fDir);
	env.SetValue("Save2FileDialog.AsList",   fAsList);
   env.SaveLevel(kEnvLocal);
}
//_________________________________________________________________________

void Save2FileDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
	fFileName = env.GetValue("Save2FileDialog.FileName", "workfile.root");
	fDir = env.GetValue("Save2FileDialog.Dir", "");
   fAsList = env.GetValue("Save2FileDialog.AsList", 0);
}
//______________________________________________________________________

void Save2FileDialog::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
{
	cout << "Save2FileDialog::CRButtonPressed " << bid << endl;
	if (bid  ==  fBidAsList ) {
		if (fAsList == 0) 
			fDialog->DisableButton(fBidObjName);
		else 
			fDialog->EnableButton(fBidObjName);
	}
}
//_________________________________________________________________________

void Save2FileDialog::CloseDown(Int_t /*wid*/)
{
   cout << "Save2FileDialog::CloseDown()" << endl;
 //  SaveDefaults();
 //  delete this;
}
//_______________________________________________________________________

void Save2FileDialog::CloseDialog()
{
 //  cout << "FitOneDimDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fDialog ) fDialog->CloseWindowExt();
   delete this;
}
