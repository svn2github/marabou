#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TList.h"
#include "TMath.h"
#include "TGMsgBox.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
#include <fstream>
#include "hprbase.h"

using std::cout;
using std::cerr;
using std::endl;

namespace Hpr 
{

void WriteHistasASCII(TH1 *hist, TGWindow *window)
{
// *INDENT-OFF*
  const char helpText[] =
"As default only the channel contents is written\n\
to the file.|n\
\"Binwidth/2\" and \"Errors\" is useful if file should be used\n\
as input to Ascii2Graph: X, Y, ErrX, ErrY";
// *INDENT-ON*

   if ( hist->GetDimension() == 3 ) {
      WarnBox(" WriteHistasASCII: 3 dim not yet supported ", window);
      return;
   }
   static Int_t channels   = 0;
   static Int_t bincenters = 1;
   static Int_t binw2      = 0;
   static Int_t errors     = 0;
   static Int_t first_binX  = 0;
   static Int_t last_binX   = 0;
   static Int_t first_binY  = 0;
   static Int_t last_binY   = 0;
   static Int_t suppress_zeros = 1;

   static TString fname;
   fname = hist->GetName();
 //  fname = fHname;
   fname += ".ascii";

   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
;
   row_lab->Add(new TObjString("StringValue_File name"));
   valp[ind++] = &fname;
   if (hist->GetDimension() == 1) {
      row_lab->Add(new TObjString("CheckButton_Channel Numbers"));
      valp[ind++] = &channels;
      row_lab->Add(new TObjString("CheckButton_Bin Centers"));
      valp[ind++] = &bincenters;
      row_lab->Add(new TObjString("CheckButton_Bwidth/Sqrt(12) Errors X"));
      valp[ind++] = &binw2;
   }
   row_lab->Add(new TObjString("CheckButton_Errors Content (Y)"));
   valp[ind++] = &errors;
   row_lab->Add(new TObjString("PlainIntVal_first_binX"));
   valp[ind++] = &first_binX;
   row_lab->Add(new TObjString("PlainIntVal_last_binX"));
   valp[ind++] = &last_binX;
   if (hist->GetDimension() == 2) {
      row_lab->Add(new TObjString("PlainIntVal_first_binY"));
      valp[ind++] = &first_binY;
      row_lab->Add(new TObjString("PlainIntVal_last_binY"));
      valp[ind++] = &last_binY;
   }
   row_lab->Add(new TObjString("CheckButton_Suppress empty channels"));
   valp[ind++] = &suppress_zeros;

   Int_t itemwidth=250;
   ok = GetStringExt("Write hist as ASCII-file", NULL, itemwidth, window,
                   NULL, NULL, row_lab, valp, NULL, NULL, &helpText[0]);
   if (!ok) {
      return;
   }


   if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
//      cout << fname << " exists" << endl;
      if ( !QuestionBox("File exists, overwrite?",window) ) 
         return;
   }
   ofstream outfile((const char *) fname);
   if (outfile.fail()) {
      cout << "Opening: " << fname << " failed" << endl;
      return;
   }
   Int_t nl = 0;
   Int_t nbx1 = 1;
   Int_t nbx2 = hist->GetNbinsX();
   if (first_binX > 0) nbx1 = first_binX;
   if (last_binX > 0)  nbx2 = last_binX;

   if ( hist->GetDimension() != 2 ) {
      for (Int_t i = nbx1; i <= nbx2; i++) {
         if (bincenters && suppress_zeros &&
             hist->GetBinContent(i)  == 0) continue;
         if (channels)
            outfile << i << "\t";
         if (bincenters)
            outfile << hist->GetBinCenter(i) << "\t";
         outfile << hist->GetBinContent(i);
         if (binw2)
            outfile << "\t" <<  hist->GetBinWidth(i) / TMath::Sqrt(12);
         if (errors)
            outfile << "\t" << hist->GetBinError(i);
         outfile << std::endl;
         nl++;
      }

   } else {
      Int_t nby1 = 1;
      Int_t nby2 = hist->GetNbinsY();
      if (first_binY > 0) nby1 = first_binY;
      if (last_binY > 0)  nby2 = last_binY;
      TAxis * xa = hist->GetXaxis();
      TAxis * ya = hist->GetYaxis();
      for (Int_t i = nbx1; i <= nbx2; i++) {
         for (Int_t k = nby1; k <= nby2; k++) {
            if (suppress_zeros && hist->GetCellContent(i, k)  == 0) continue;
            outfile << xa->GetBinCenter(i) << "\t";
            outfile << ya->GetBinCenter(k) << "\t";

            outfile << hist->GetCellContent(i,k);
            if (errors)
            outfile << "\t" << hist->GetCellError(i,k);
            outfile << std::endl;
            nl++;
         }
      }
   }
   cout << nl << " lines written to: " << (const char *) fname << endl;
   outfile.close();
};
//___________________________________________________________________________

void WarnBox(const char *message, TGWindow *window)
{
   Int_t retval = 0;
   new TGMsgBox(gClient->GetRoot(), window,
                "Warning", message, kMBIconExclamation, kMBDismiss,
                &retval);
}
//___________________________________________________________________________

Bool_t QuestionBox(const char *message, TGWindow *window)
{
      int buttons = kMBOk | kMBDismiss, retval = 0;
      new TGMsgBox(gClient->GetRoot(), window, "Question", message,
                   kMBIconQuestion, buttons, &retval);
       return ( retval != kMBDismiss );
}

//_______________________________________________________________________________________

TH1 * FindHistOfTF1(TVirtualPad * ca, const char * fname, Int_t push_pop)
{
   if (!ca) return NULL;
   TIter next(ca->GetListOfPrimitives());
   while (TObject * obj = next()) {
      if (obj->InheritsFrom("TH1")) {
         TList *lof = ((TH1*)obj)->GetListOfFunctions();
         TObject *o = lof->FindObject(fname);
         if (o) {
            if (push_pop != 0) {
               lof->Remove(o);
               if (push_pop > 0) 
                  lof->AddFirst(o);
               else
                  lof->Add(0);
            }
            return (TH1*)o;
         }
      }
   }
   return NULL;
};
//_______________________________________________________________________________________

TH1 * FindHistInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TH1")) {
			return (TH1*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

TGraph * FindGraphInPad(TVirtualPad * ca)
{
	if (!ca) return NULL;
	TIter next(ca->GetListOfPrimitives());
	while (TObject * obj = next()) {
		if (obj->InheritsFrom("TGraph")) {
			return (TGraph*)obj;
		}
	}
	return NULL;
};
//_______________________________________________________________________________________

Bool_t HistLimitsMatch(TH1* h1, TH1* h2)
{
//	cout << " HistLimitsMatch : " << h1->GetName() 
//		<< " " << h2->GetName()<< endl;
	if (h1->GetDimension() != h2->GetDimension() ) 
		return kFALSE;
	TAxis * a1 = h1->GetXaxis();
	TAxis * a2 = h2->GetXaxis();
	if ( a1->GetNbins() != a2->GetNbins() )
		return kFALSE;
	if ( a1->GetXmin() != a2->GetXmin() )
		return kFALSE;
	if ( a1->GetXmax() != a2->GetXmax() )
		return kFALSE;
	if (h1->GetDimension() > 1 && h2->GetDimension() > 1 ) {
		a1 = h1->GetYaxis();
		a2 = h2->GetYaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	if (h1->GetDimension() > 2 && h2->GetDimension() > 2 ) {
		a1 = h1->GetZaxis();
		a2 = h2->GetZaxis();
		if ( a1->GetNbins() != a2->GetNbins() )
			return kFALSE;
		if ( a1->GetXmin() != a2->GetXmin() )
			return kFALSE;
		if ( a1->GetXmax() != a2->GetXmax() )
			return kFALSE;
	}
	return kTRUE;
}

}   // end namespace Hpr
