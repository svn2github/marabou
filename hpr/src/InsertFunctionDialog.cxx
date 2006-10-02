#include "TCanvas.h"
#include "TEnv.h"
#include "TF1.h"
#include "TList.h"
#include "TObjString.h"
#include "TVirtualPad.h"
#include "TRootCanvas.h"
#include "TSystem.h"
#include "TGMrbValuesAndText.h"
#include "InsertFunctionDialog.h"
#include <iostream>
#include <Riostream.h>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(InsertFunctionDialog)

//___________________________________________________________________________

InsertFunctionDialog::InsertFunctionDialog() 
{
   static void *valp[25];
   Int_t ind = 0;
   RestoreDefaults();
   static TString excmd("InsertFunctionExecute()");
   TList *row_lab = new TList(); 
   ind = 0;
   row_lab->Clear();
   row_lab->Add(new TObjString("StringValue_Function Name"));
   valp[ind++] = &fName;

   row_lab->Add(new TObjString("StringValue_Title X"));
   valp[ind++] = &fXtitle;
   row_lab->Add(new TObjString("StringValue-Title Y"));
   valp[ind++] = &fYtitle;
   row_lab->Add(new TObjString("PlainIntVal_Npar"));
   valp[ind++] = &fNpar;
   row_lab->Add(new TObjString("DoubleValue-From"));
   valp[ind++] = &fFrom;
   row_lab->Add(new TObjString("DoubleValue-To"));
   valp[ind++] = &fTo;

   for (Int_t i =0; i < 10; i += 2) {
      row_lab->Add(new TObjString(Form("DoubleValue_Par[%d]",i)));
      valp[ind++] = &fPar[i];
      row_lab->Add(new TObjString(Form("DoubleValue+Par[%d]",i+1)));
      valp[ind++] = &fPar[i+1];
   }
   row_lab->Add(new TObjString("PlainIntVal_Pad Opacity "));
   valp[ind++] = &fPadOpacity;
   row_lab->Add(new TObjString("ColorSelect-Drawing color"));
   valp[ind++] = &fCol;
   row_lab->Add(new TObjString("CheckButton_Create new canvas"));
   valp[ind++] = &fNew_canvas;
   row_lab->Add(new TObjString("CommandButt_InsertFunctionExecute"));
   valp[ind++] = &excmd;

   static TString text;
   fTpointer = NULL; 
   const char * history = 0;
   fTpointer = &text;
   const char hist_file[] = {"text_formulas.txt"};
   history = hist_file;
   if (gSystem->AccessPathName(history)) {
      ofstream hfile(history);
      hfile << "[0]*exp([1]*x)*cos([2]*x)" << endl;
      hfile.close();
   } 
   if (gROOT->GetVersionInt() < 40000) history = NULL;
   TRootCanvas* rc = (TRootCanvas*)gPad->GetCanvas()->GetCanvasImp();
   Bool_t ok;
   Int_t itemwidth = 280;
   ok = GetStringExt("Function formula", fTpointer, itemwidth, rc,
                      history, NULL, row_lab, valp,
                      NULL, NULL, NULL, this, this->ClassName());
 };  
//_________________________________________________________________________
            
InsertFunctionDialog::~InsertFunctionDialog() 
{
   SaveDefaults();
};
//_________________________________________________________________________
            
void InsertFunctionDialog::InsertFunctionExecute()
{
   Bool_t same = kFALSE;
   Int_t flag = 0;
   if (fNew_canvas == 0) flag = GetFunctionPad();
   if ( flag < 0) {
      cout << "No suitable pad found, please create one" << endl;
      return;
   } else if (flag == 2) {
      cout << "Overlay function in selected pad" << endl;
      same = kTRUE;
   }
//   cout << "InsertFunctionExecute() " << fNpar << endl;
   while (gROOT->GetListOfFunctions()->FindObject(fName)) {
      cout << "Function with name: " << fName << 
      " already exists, will increment index" << endl;
      this->IncrementIndex(&fName);
   }
   if (fTpointer->Length() < 2) {
      cout << "Formula too short: " << fTpointer->Data() << endl;
      return;
   }
   TF1 * f = new TF1(fName.Data(), fTpointer->Data(), fFrom, fTo);
   if (gROOT->GetListOfFunctions()->FindObject(fName) == 0) {
       cout << "Error in formula, watch printout" << endl;
   }
   f->SetParameters(fPar);
   f->SetLineColor(fCol);
   if ( fPadOpacity > 0) {
      if ( fPadOpacity > 100) fPadOpacity = 100;
      gPad->SetFillStyle(4000 + fPadOpacity);
   }
   if (fNew_canvas != 0) {
      new TCanvas("cc", "cc", 600,400);
      f->Draw();
   } else { 
      if (same) f->Draw("same");
      else      f->Draw();
   }
   gPad->Update();
};
//_________________________________________________________________________
            
void InsertFunctionDialog::SaveDefaults()
{
   cout << "HTCanvas::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
   env.SetValue("InsertFunctionDialog.fNpar"		  , fNpar);
   env.SetValue("InsertFunctionDialog.fPar0" 	  , fPar[0]);
   env.SetValue("InsertFunctionDialog.fPar1" 	  , fPar[1]);
   env.SetValue("InsertFunctionDialog.fPar2" 	  , fPar[2]);
   env.SetValue("InsertFunctionDialog.fPar3" 	  , fPar[3]);
   env.SetValue("InsertFunctionDialog.fPar4" 	  , fPar[4]);
   env.SetValue("InsertFunctionDialog.fPar5" 	  , fPar[5]);
   env.SetValue("InsertFunctionDialog.fPar6" 	  , fPar[6]);
   env.SetValue("InsertFunctionDialog.fPar7" 	  , fPar[7]);
   env.SetValue("InsertFunctionDialog.fPar8" 	  , fPar[8]);
   env.SetValue("InsertFunctionDialog.fPar9" 	  , fPar[9]);
   env.SetValue("InsertFunctionDialog.Name"  	  , fName		 );
   env.SetValue("InsertFunctionDialog.Xtitle"	  , fXtitle 	 );
   env.SetValue("InsertFunctionDialog.Ytitle"	  , fYtitle 	 ); 
   env.SetValue("InsertFunctionDialog.From"  	  , fFrom		 ); 
   env.SetValue("InsertFunctionDialog.To" 		  , fTo  		 ); 
   env.SetValue("InsertFunctionDialog.Col"		  , fCol 		 ); 
   env.SetValue("InsertFunctionDialog.PadOpacity", fPadOpacity );
   env.SetValue("InsertFunctionDialog.New_canvas" , fNew_canvas );
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void InsertFunctionDialog::RestoreDefaults()
{
//   cout << "HTCanvas::InsertFunctionSetDefaults()" << endl;
   TEnv env(".rootrc");
   fNpar        = env.GetValue("InsertFunctionDialog.fNpar"		  ,           3);
   fPar[0]      = env.GetValue("InsertFunctionDialog.fPar0"		  , (Double_t)1);
   fPar[1]      = env.GetValue("InsertFunctionDialog.fPar1"		  , (Double_t)0);
   fPar[2]      = env.GetValue("InsertFunctionDialog.fPar2"		  , (Double_t)0);
   fPar[3]      = env.GetValue("InsertFunctionDialog.fPar3"		  , (Double_t)0);
   fPar[4]      = env.GetValue("InsertFunctionDialog.fPar4"		  , (Double_t)0);
   fPar[5]      = env.GetValue("InsertFunctionDialog.fPar5"		  , (Double_t)0);
   fPar[6]      = env.GetValue("InsertFunctionDialog.fPar6"		  , (Double_t)0);
   fPar[7]      = env.GetValue("InsertFunctionDialog.fPar7"		  , (Double_t)0);
   fPar[8]      = env.GetValue("InsertFunctionDialog.fPar8"		  , (Double_t)0);
   fPar[9]      = env.GetValue("InsertFunctionDialog.fPar9"		  , (Double_t)0);
   fName		    = env.GetValue("InsertFunctionDialog.Name"		  , "expcos");
   fXtitle 	    = env.GetValue("InsertFunctionDialog.Xtitle" 	  , "Xa");
   fYtitle 	    = env.GetValue("InsertFunctionDialog.Ytitle" 	  , "Ya"); 
   fFrom		    = env.GetValue("InsertFunctionDialog.From"		  , 0); 
   fTo			 = env.GetValue("InsertFunctionDialog.To"  		  , 10); 
   fCol 		    = env.GetValue("InsertFunctionDialog.Col" 		  , 4); 
   fPadOpacity  = env.GetValue("InsertFunctionDialog.PadOpacity", 30);
	fNew_canvas  = env.GetValue("InsertFunctionDialog.New_canvas" , 0);
}
//_________________________________________________________________________
            
void InsertFunctionDialog::CloseDown()
{
   cout << "InsertFunctionDialog::CloseDown()" << endl;
   delete this;
}
//________________________________________________________________
void InsertFunctionDialog::IncrementIndex(TString * arg)
{
// find number at end of string and increment,
// if no number found add "_0";
   Int_t len = arg->Length();
   if (len < 0) return;
   Int_t ind = len - 1;
   Int_t first_digit = ind;
   TString subs;
   while (ind > 0) {
      subs = (*arg)(ind, len - ind);
//      cout << subs << endl;
      if (!subs.IsDigit()) break;
      first_digit = ind;
      ind--;
   }
   if (first_digit == ind) {
     *arg += "_0";
   } else { 
      subs = (*arg)(first_digit, len - first_digit);
      Int_t num = atol(subs.Data());
      arg->Resize(first_digit);
      *arg += (num + 1);
   }
}
//______________________________________________________________________________

Int_t InsertFunctionDialog::GetFunctionPad(TPad *ipad)
{
   TPad * pad = ipad;
   if (pad == NULL)pad = (TPad*)gPad;
   TObject *obj;
//  if selected pad/canvas contains a function use it
   TIter n1(pad->GetListOfPrimitives());
   while ( (obj = n1()) ) {
      if ( obj->InheritsFrom("TF1") ) {
         pad->cd(0);
         gROOT->SetSelectedPad(pad);
         return 2;
      }
   }
   TString cn(pad->ClassName());
   if (cn.Contains("Canvas")) {
      TIter next(pad->GetListOfPrimitives());
      TPad* p1  = NULL;
      while ( (obj = next()) ) {
         if (obj->InheritsFrom("TPad")) {
            p1 = (TPad*)obj;
            if (p1->GetListOfPrimitives()->GetSize() == 0) {
               p1->cd(0);
               gROOT->SetSelectedPad(p1);
               return 1;
            } else {
               TIter n2(p1->GetListOfPrimitives());
               TObject *ff;
               while ( (ff = n2()) ) {
                  if (ff->InheritsFrom("TF1")) {
                     p1->cd(0);
                     gROOT->SetSelectedPad(p1);
                     return 2;
                  }
               }
            }
         }
      }
   } else {
   	if (pad->GetListOfPrimitives()->GetSize() == 0) {
      	pad->cd(0);
      	gROOT->SetSelectedPad(pad);
      	return 1;
   	} else {
         TIter n2(pad->GetListOfPrimitives());
         TObject *ff;
         while ( (ff = n2()) ) {
            if ( ff->InheritsFrom("TF1") ) {
               pad->cd(0);
               gROOT->SetSelectedPad(pad);
               return 2;
            }
         }
   	}
   }
   return -1;   // no suitable pad found
}
 
