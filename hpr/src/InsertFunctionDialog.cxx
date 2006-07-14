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

InsertFunctionDialog::InsertFunctionDialog(Int_t npar) 
{
   if (npar < 1) {
      cout << "Number of parameters must be >= 1" << endl;
      return;
   }
   if (npar > 10) {
      cout << "Number of parameters must be <= 10" << endl;
      return;
   }
   static void *valp[25];
   Int_t ind = 0;
   fNpar = npar;
   RestoreDefaults();
   static TString excmd("InsertFunctionExecute()");
   TList *row_lab = new TList(); 
   ind = 0;
   row_lab->Clear();
   row_lab->Add(new TObjString("StringValue_Function Name"));
   valp[ind++] = &fName;

   row_lab->Add(new TObjString("StringValue_X axis title"));
   valp[ind++] = &fXtitle;
   row_lab->Add(new TObjString("StringValue_Y axis title"));
   valp[ind++] = &fYtitle;
   row_lab->Add(new TObjString("DoubleValue_From"));
   valp[ind++] = &fFrom;
   row_lab->Add(new TObjString("DoubleValue+To"));
   valp[ind++] = &fTo;

   for (Int_t i =0; i < fNpar; i++) {
      row_lab->Add(new TObjString(Form("DoubleValue_Value of Parameter %d",i)));
      valp[ind++] = &fPar[i];
   }
   row_lab->Add(new TObjString("ColorSelect_Drawing color"));
   valp[ind++] = &fCol;
   row_lab->Add(new TObjString("PlainIntVal+Opacity of pad (0-100)"));
   valp[ind++] = &fPad_opacity;
   row_lab->Add(new TObjString("RadioButton_Use new(selected) pad"));
   valp[ind++] = &fNew_pad;
   row_lab->Add(new TObjString("RadioButton_Use same (selected) pad"));
   valp[ind++] = &fSame_pad;
   row_lab->Add(new TObjString("RadioButton_Create separate canvas"));
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
   Int_t itemwidth = 280;
   GetStringExt("Function formula", fTpointer, itemwidth, rc,
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
   cout << "InsertFunctionExecute() " << fNpar << endl;
   while (gROOT->GetListOfFunctions()->FindObject(fName)) {
      cout << "Function with name: " << fName << 
      " already exists, incement index" << endl;
      this->IncrementIndex(&fName);
   }
   if (fTpointer->Length() < 2) {
      cout << "Formaula too short: " << fTpointer->Data() << endl;
      return;
   }
   TF1 * f = new TF1(fName.Data(), fTpointer->Data(), fFrom, fTo);
   if (gROOT->GetListOfFunctions()->FindObject(fName) == 0) {
       cout << "Error in formula, watch printout" << endl;
   }
   f->SetParameters(fPar);
   f->SetLineColor(fCol);
   if (fPad_opacity > 0) {
      if (fPad_opacity > 100) fPad_opacity = 100;
      gPad->SetFillStyle(4000 + fPad_opacity);
   }
   if (fNew_canvas != 0) new TCanvas("cc", "cc", 600,400);
   if (fSame_pad   != 0) f->Draw("same");
   else                  f->Draw();
   gPad->Update();
};
//_________________________________________________________________________
            
void InsertFunctionDialog::SaveDefaults()
{
   cout << "HTCanvas::InsertFunction::SaveDefaults()" << endl;
   TEnv env(".rootrc");
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
   env.SetValue("InsertFunctionDialog.Pad_opacity", fPad_opacity);
   env.SetValue("InsertFunctionDialog.Same_pad"   , fSame_pad   );
   env.SetValue("InsertFunctionDialog.New_pad"    , fNew_pad	 );
   env.SetValue("InsertFunctionDialog.New_canvas" , fNew_canvas );
   env.SaveLevel(kEnvUser);
}
//_________________________________________________________________________
            
void InsertFunctionDialog::RestoreDefaults()
{
//   cout << "HTCanvas::InsertFunctionSetDefaults()" << endl;
   TEnv env(".rootrc");
   fPar[0]      = env.GetValue("InsertFunctionDialog.fPar0"		  , 1);
   fPar[1]      = env.GetValue("InsertFunctionDialog.fPar1"		  , 1);
   fPar[2]      = env.GetValue("InsertFunctionDialog.fPar2"		  , 1);
   fPar[3]      = env.GetValue("InsertFunctionDialog.fPar3"		  , 1);
   fPar[4]      = env.GetValue("InsertFunctionDialog.fPar4"		  , 1);
   fPar[5]      = env.GetValue("InsertFunctionDialog.fPar5"		  , 1);
   fPar[6]      = env.GetValue("InsertFunctionDialog.fPar6"		  , 1);
   fPar[7]      = env.GetValue("InsertFunctionDialog.fPar7"		  , 1);
   fPar[8]      = env.GetValue("InsertFunctionDialog.fPar8"		  , 1);
   fPar[9]      = env.GetValue("InsertFunctionDialog.fPar9"		  , 1);
   fName		    = env.GetValue("InsertFunctionDialog.Name"		  , "expcos");
   fXtitle 	    = env.GetValue("InsertFunctionDialog.Xtitle" 	  , "Xa");
   fYtitle 	    = env.GetValue("InsertFunctionDialog.Ytitle" 	  , "Ya"); 
   fFrom		    = env.GetValue("InsertFunctionDialog.From"		  , 0); 
   fTo			 = env.GetValue("InsertFunctionDialog.To"  		  , 10); 
   fCol 		    = env.GetValue("InsertFunctionDialog.Col" 		  , 4); 
   fPad_opacity = env.GetValue("InsertFunctionDialog.Pad_opacity", 30);
   fSame_pad	 = env.GetValue("InsertFunctionDialog.Same_pad"   , 0);
   fNew_pad	    = env.GetValue("InsertFunctionDialog.New_pad"	  , 1);
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
      cout << subs << endl;
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
 
