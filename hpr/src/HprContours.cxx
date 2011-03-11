#include "TMath.h"
#include "FhContour.h"
#include "FitHist.h"
#include "FitHist_Help.h"
#include "TMrbNamedArray.h"
#include "HistPresent.h"
#include "hprbase.h"
#include "SetColorModeDialog.h"
#include "support.h"
#include "TGMrbInputDialog.h"

extern HistPresent *gHpr;

//_______________________________________________________________________________________

void FitHist::SaveUserContours()
{
	TString hname = fHname;
   Bool_t ok;
	Int_t ip = hname.Index(";");
	if (ip > 0) hname.Resize(ip);
   hname =
       GetString("Save contours with name", hname.Data(), &ok, mycanvas);
   if (!ok) return;
   Int_t ncont = fSelHist->GetContour();
   if (ncont <= 0) {
      Hpr::WarnBox("No Contours defined");
      return;
   }
   TMrbNamedArrayI * colors = 0;
   colors = dynamic_cast<TMrbNamedArrayI*>(fSelHist->
              GetListOfFunctions()->FindObject("Pixel"));
   if (!colors) {
      Hpr::WarnBox("No User Colors defined");
      return;
   }
   FhContour * contour = new FhContour(hname.Data(), "User contours", ncont);
   Double_t * values = contour->GetLevelArray()->GetArray();

	fSelHist->GetContour(values);
	*(contour->GetColorArray()) = *colors;
   contour->Print();
   new Save2FileDialog(contour, NULL, GetMyCanvas());
	
	if ( (!CreateDefaultsDir(mycanvas,kFALSE )) ) return;
	
	TEnv * env = GetDefaults(fHname, kFALSE, kTRUE); // fresh values
	if (!env) return;
	//     save user contour
//	TMrbNamedArrayI * colors = (TMrbNamedArrayI *)fSelHist->
//	GetListOfFunctions()->FindObject("Pixel");
	if (colors && colors->GetSize() > 0) {
		Int_t ncol = colors->GetSize();
		Int_t ncont = fSelHist->GetContour();
		Double_t * uc = 0;
		if (ncont == ncol) {
			uc = new Double_t[ncont];
			fSelHist->GetContour(uc);
		}
		Int_t pixval;
		Double_t cval;
		env->SetValue("Contours", ncol);
		for (Int_t i = 0; i < ncol; i++) {
			if (colors) pixval = (*colors)[i];
			else        pixval = 0;
			if (uc)     cval = uc[i];
			else        cval = 0;
			TString s;
			s = "Cont"; s += i;
			env->SetValue(s.Data(), cval);
			s = "Pix"; s += i;
			env->SetValue(s.Data(), pixval);
		}
		if (uc) delete [] uc;
	}
}
//_______________________________________________________________________________________

void FitHist::UseSelectedContour()
{
   if (!gHpr) return;
   if(gHpr->fSelectContour->GetSize() <= 0) {
      Hpr::WarnBox("No contour selected");
      return;
   } else if (gHpr->fSelectContour->GetSize() > 1) {
      Hpr::WarnBox("More then one selected\n\
Take first");
   }
   TObjString * objs = (TObjString *)gHpr->fSelectContour->At(0);
   TString fname = objs->GetString();
   Int_t pp = fname.Index(" ");
   if (pp <= 0) {cout << "pp<=0 in " << fname << endl; return;};
   fname.Resize(pp);
   TString cname = objs->GetString();
   cname.Remove(0,pp+1);
//   cout << "fname " << fname << "cont name " << cname << endl;
//   if (fRootFile) fRootFile->Close();
   TFile * rf =new TFile(fname);
   FhContour * ucont = (FhContour*)rf->Get(cname);
   TH2 * h2 = (TH2*)fSelHist;
   TArrayD * xyvals = ucont->GetLevelArray();
   TArrayI * colors = ucont->GetColorArray();
   if (xyvals->GetSum() > 0) {
      h2->SetContour(xyvals->GetSize(), xyvals->GetArray());
      AdjustMaximum(h2, xyvals);
      fSetLevels = kTRUE;
   } else {
      h2->SetContour(xyvals->GetSize());
      fSetLevels = kFALSE;
   }
   if (colors->GetSum() > 0) {
      TMrbNamedArrayI * ca = new TMrbNamedArrayI("Pixel",fHname.Data());
       ca->Set(colors->GetSize(), colors->GetArray());
      TObject * nai = fSelHist->GetListOfFunctions()->FindObject("Pixel");
      if (nai)  fSelHist->GetListOfFunctions()->Remove(nai);
      h2->GetListOfFunctions()->Add(ca);
      SetUserPalette(1001, colors);
      fSetColors = kTRUE;
   } else {
      fSetColors = kFALSE;
   }

   SaveDefaults();
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
   rf->Close();
   gDirectory = gROOT;
}
//_______________________________________________________________________________________

void FitHist::ClearUserContours()
{
   TH2 * h2 = (TH2*)fSelHist;
   h2->SetContour(20);
   SetColorModeDialog::SetColorMode();

   TObject * nai;
   while ( (nai = fSelHist->GetListOfFunctions()->FindObject("Pixel")) ){
      cout << " Removing user contour" << endl;
      fSelHist->GetListOfFunctions()->Remove(nai);
   }
   while ( (nai = fSelHist->GetListOfFunctions()->FindObject("palette")) ){
      cout << " Removing palette" << endl;
      fSelHist->GetListOfFunctions()->Remove(nai);
   }
   fUserContourLevels = 0;
   SaveDefaults();
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
}
//_______________________________________________________________________________________

void FitHist::SetUserContours()
{
   Int_t old_ncont = fSelHist->GetContour();
   Int_t ncont;
   Bool_t use_old;

   if(old_ncont > 0) {
      ncont = old_ncont;
      use_old = kTRUE;
   } else {
      use_old = kFALSE;
      ncont = 3;
   }
   Bool_t ok, set_levels = kTRUE, set_colors = kTRUE;

   ncont = GetInteger("Number of contours", ncont, &ok, mycanvas,
                      "Set Levels", &set_levels, 0,
                      "Set Colors", &set_colors);
   if (!ok || ncont <= 0) return;

   FhContour ucont("temp", "uc", ncont);
   TArrayD * xyvals = ucont.GetLevelArray();
   TArrayI * colors = ucont.GetColorArray();

   TMrbNamedArrayI * oldcol = 0;
   if (use_old) {
     oldcol = dynamic_cast<TMrbNamedArrayI*>(fSelHist->
              GetListOfFunctions()->FindObject("Pixel"));
     if (oldcol)fSelHist->GetListOfFunctions()->Remove(oldcol);
   }
	Int_t startIndex = 51;
	Int_t nofLevels = 50;
	Int_t colind;
	Float_t dcol = (Float_t)nofLevels / (Float_t)(ncont -1);
	Float_t colf = dcol + (Float_t)startIndex;
   for (Int_t i=0; i < ncont; i++) {
      (*colors)[i] = 0;
      Int_t ival = (Int_t)(i * fSelHist->GetMaximum() / ncont);
      (*xyvals)[i] = (Double_t) ival;
//    assume colorindeces 51 - 100 ( rainbow colors)
         if (gHpr) {
           startIndex = SetColorModeDialog::GetStartColorIndex();
           nofLevels  = SetColorModeDialog::GetNofColorLevels();
         }
         if (i == 0) {
            colind =  startIndex;
         } else if (i == ncont-1) {
            colind = startIndex + nofLevels - 1;
         } else {
            colind = TMath::Nint(colf);
            colf += dcol;
         }

         if (gDebug > 1) cout << "colind " << colind << endl;
         TColor * col = GetColorByInd(colind);
         if (col) (*colors)[i] = col->GetPixel();
//      }
   }

   Int_t ret = ucont.Edit((TGWindow*)mycanvas);
   if (ret <  0) return;

   TH2 * h2 = (TH2*)fSelHist;
   fSetLevels = set_levels;
   fSetColors = set_colors;
   if (set_levels) {
      fUserContourLevels = ncont;
      h2->SetContour(ncont, xyvals->GetArray());
      AdjustMaximum(h2, xyvals);
   }
   if (fSetColors) {
      TMrbNamedArrayI * ca = new TMrbNamedArrayI("Pixel",fHname.Data());
      ca->Set(ncont, colors->GetArray());
      h2->GetListOfFunctions()->Add(ca);
      SetUserPalette(1001, colors);
   }
//	}
   SaveDefaults();
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
}
