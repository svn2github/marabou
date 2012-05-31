// #include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TEnv.h"
#include "TFrame.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TGMsgBox.h"
#include "TMath.h"
#include "TVirtualFitter.h"
#include "TVirtualPad.h"
#include "TGMrbTableFrame.h"
#include "Fit2DimDialog.h"
#include "Save2FileDialog.h"
#include "FhPeak.h"
#include "TF1Range.h"
#include "SetColor.h"
#include <iostream>
//#ifdef MARABOUVERS
#include "HistPresent.h"
//#endif

using std::cout;
using std::endl;

//Double_t Fit2DimDialog::Fit2DimGauss(Double_t *xv, Double_t *par)
Double_t Fit2DimGauss(Double_t *xv, Double_t *par)
{
/*
  2 dim gaus at angle phi, offset x0, y0
  par[0]   x0
  par[1]   y0
  par[2]   phi
  par[3]   gauss constant
  par[4]   gauss widthx
  par[5]   gauss widthy
  ...
*/

   static Double_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t fitval  = 0;
   Double_t x0  = par[0];
   Double_t y0  = par[1];
   Double_t phi = par[2] * TMath::Pi() / 180;
   Double_t c = par[3];
   Double_t sigmax = par[4];
   Double_t sigmay = par[5];
   Double_t gx, gy;
   Double_t x = xv[0];
   Double_t y = xv[1];
   Double_t sinphi = TMath::Sin(phi);
   Double_t cosphi = TMath::Cos(phi);
 
   if (sigmax == 0)
      sigmax = 1;               //  force widths /= 0
   if (sigmay == 0)
      sigmay = 1;               //  force widths /= 0
   x -= x0;
   y -= y0;
   gx = x * cosphi + y * sinphi;
   gy = -x * sinphi + y * cosphi;
   Double_t argx = gx / (sqrt2 * sigmax);
   Double_t argy = gy / (sqrt2 * sigmay);
   fitval = c * exp(-argx * argx) / (sqrt2pi * sigmax);
   fitval    *= exp(-argy * argy) / (sqrt2pi * sigmay);
   return fitval;
}
//____________________________________________________________________________________

Fit2DimDialog::Fit2DimDialog(TH2 * hist, Int_t type, Int_t interactive)
{
   fInteractive = interactive;
   fSelHist = hist;
//   if (!fSelHist) {
//      cout << "No hist selected" << endl;
//      return;
//   }

   if (fSelHist && fSelHist->GetDimension() != 2) {
      cout << "Can only be used with 2-dim hist" << endl;
      return;
   }
 //  lBinW   = fSelHist->GetBinWidth(1);
   if (fSelHist) fName = fSelHist->GetName();

   DisplayMenu(type);
}
//____________________________________________________________________________________
//____________________________________________________________________________________

void Fit2DimDialog::DisplayMenu(Int_t type)
{
static const Char_t helptext_gaus[] =
"This widget allows fitting of a \n\
2 dim gaussian shaped peak optionally with a linear background.\n\
The linear background may be forced to zero (Force BG = 0)\n\
or to be constant(Force BG slope = 0)\n\
Marks (use middle mouse button to define them) are used\n\
to provide limits of the fit region and\n\
the position of peak to be fitted.";

   fParentWindow = NULL;

   fFuncNumber = 0;
   fUsedbg     = 0;
   fUseoldpars = 0;
   fSigmaX = 0;
   fSigmaY = 0;
   fConstant = 0;
   fOffX = 0;
   fOffY = 0;
   fPhi  = 0;
   fFixSigmaX = 0;
   fFixSigmaY = 0;
   fFixConstant = 0;
   fFixOffX = 0;
   fFixOffY = 0;
   fFixPhi = 0;
   fBgX0 = 0;
   fBgY0 = 0;
   fBgSlopeX = 0;
   fBgSlopeY = 0;
   RestoreDefaults();
   fSelPad = NULL;
   SetBit(kMustCleanup);
   fFuncName = Form("_%d", fFuncNumber);
//   TString t (fName);
//   t.Prepend("Fit_");
   fFuncName.Prepend("xygaus");
   fFuncNumber++;
   fUseoldpars = 0;
   fMarkers = NULL;
   fFitFunc = NULL;
   fReqNmarks= 0;
   fDialog = NULL;
   TAxis *xaxis = fSelHist->GetXaxis();
   TAxis *yaxis = fSelHist->GetYaxis();
   fFromX = xaxis->GetXmin();
   fToX   = xaxis->GetXmax() ;
   fFromY = yaxis->GetXmin();
   fToY   = yaxis->GetXmax() ;
   gROOT->GetListOfCleanups()->Add(this);

   TIter next(gROOT->GetListOfCanvases());
   TCanvas *c;
   while ( (c = (TCanvas*)next()) ) {
      if (c->GetListOfPrimitives()->FindObject(fName)) {
         fSelPad = c;
         fSelPad->cd();
//         cout << "fSelPad " << fSelPad << endl;
         break;
      }
   }
   if (fSelPad == NULL) {
     cout << "fSelPad = 0!!" <<  endl;
   } else {
      fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
   }
   if (fInteractive > 0 ) {
      TString title;
		TList *row_lab = new TList();
		static void *valp[50];
		Int_t ind = 0;
		static TString exgcmd("FitGausExecute()");
		static TString exdgcmd("DrawGausExecute()");
		static TString clmcmd("ClearMarkers()");
		static TString clfcmd("ClearFunctionList()");
		static TString wrfcmd("WriteoutFunction()");
		static TString setmcmd("SetMarkers()");
		static TString prtcmd("PrintMarkers()");
		static TString sfocmd("SetFittingOptions()");
		static TString fhrcmd("FillHistRandom()");
		static TString svcmd("SaveFunction()");
		static TString gfcmd("GetFunction()");
		static Int_t dummy = 0;
		TString * text = NULL;
		const char * history = NULL;
//		const char hist_file[] = {"func_formulae_hist.txt"};
	//   history = hist_file;
	//   if (gROOT->GetVersionInt() < 40000) history = NULL;

	//   fSelHist->Dump();
		const char * helptext = NULL;
      TString tagname;
//		} else if (type == 2) {
         fNpar = 6;
         title = "2 dim Gaussian";
			helptext = helptext_gaus;
	//      row_lab->Add(new TObjString("CommentOnly_Function: a + b*exp(c*(x-d))"));
			row_lab->Add(new TObjString("CommentOnly_Function: 2 dim Gauss)"));
			valp[ind++] = &dummy;
			row_lab->Add(new TObjString("DoubleV+CbF_OffX"));
			valp[ind++] = &fOffX;
//			row_lab->Add(new TObjString("CheckButton-Fix"));
			valp[ind++] = &fFixOffX;
			row_lab->Add(new TObjString("DoubleV+CbF+OffY"));
			valp[ind++] = &fOffY;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fFixOffY;
			row_lab->Add(new TObjString("DoubleV+CbF_Phi "));
			valp[ind++] = &fPhi;
//			row_lab->Add(new TObjString("CheckButton-Fix"));
			valp[ind++] = &fFixPhi;
			row_lab->Add(new TObjString("DoubleV+CbF+Cnst"));
			valp[ind++] = &fConstant;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fFixConstant;
			row_lab->Add(new TObjString("DoubleV+CbF_SigX"));
			valp[ind++] = &fSigmaX;
//			row_lab->Add(new TObjString("CheckButton-Fix"));
			valp[ind++] = &fFixSigmaX;
			row_lab->Add(new TObjString("DoubleV+CbF+SigY"));
			valp[ind++] = &fSigmaY;
//			row_lab->Add(new TObjString("CheckButton+Fix"));
			valp[ind++] = &fFixSigmaY;
		row_lab->Add(new TObjString("DoubleValue_FromX"));
		valp[ind++] = &fFromX;
		row_lab->Add(new TObjString("DoubleValue+ToX"));
		valp[ind++] = &fToX;
		row_lab->Add(new TObjString("DoubleValue_FromY"));
		valp[ind++] = &fFromY;
		row_lab->Add(new TObjString("DoubleValue+ToY"));
		valp[ind++] = &fToY;
		row_lab->Add(new TObjString("CheckButton_Add funcs to hist"));
		valp[ind++] = &fFitOptAddAll;
		row_lab->Add(new TObjString("CheckButton+Auto Clear marks"));
		valp[ind++] = &fAutoClearMarks;
		row_lab->Add(new TObjString("ColorSelect_LineCol"));
		valp[ind++] = &fColor;
		row_lab->Add(new TObjString("PlainShtVal+Wid"));
		valp[ind++] = &fWidth;
		row_lab->Add(new TObjString("LineSSelect+Sty"));
		valp[ind++] = &fStyle;
		row_lab->Add(new TObjString("StringValue_FuncName"));
		valp[ind++] = &fFuncName;
		row_lab->Add(new TObjString("CommandButt_Exe Fit"));
			valp[ind++] = &exgcmd;
			row_lab->Add(new TObjString("CommandButt+Draw only"));
			valp[ind++] = &exdgcmd;
		row_lab->Add(new TObjString("CommandButt_Opts/Params"));
		valp[ind++] = &sfocmd;
		row_lab->Add(new TObjString("CommandButt+Clear FuncList"));
		valp[ind++] = &clfcmd;
//		row_lab->Add(new TObjString("CommandButt+Writeout Func"));
//		valp[ind++] = &wrfcmd;

		row_lab->Add(new TObjString("CommandButt_Print Marks"));
		valp[ind++] = &prtcmd;
		row_lab->Add(new TObjString("CommandButt+Clear Marks"));
		valp[ind++] = &clmcmd;
/*
			row_lab->Add(new TObjString("CommandButt-Set N Marks"));
			valp[ind++] = &setmcmd;
			row_lab->Add(new TObjString("PlainIntVal-N"));
			valp[ind++] = &fReqNmarks;
*/
//	   	row_lab->Add(new TObjString("CommandButt_Save Func to File"));
//		   valp[ind++] = &svcmd;
//		   row_lab->Add(new TObjString("CommandButt+Get Func from File"));
//		   valp[ind++] = &gfcmd;
//			row_lab->Add(new TObjString("CommandButt_Fill random"));
//			valp[ind++] = &fhrcmd;
//			row_lab->Add(new TObjString("PlainIntVal+N events"));
//			valp[ind++] = &fNevents;
		Int_t itemwidth = 360;
		Int_t ok = 0;
		fDialog =
		new TGMrbValuesAndText (title.Data(), text, &ok, itemwidth,
								fParentWindow, history, NULL, row_lab, valp,
								NULL, NULL, helptext, this, this->ClassName());
   }
}
//__________________________________________________________________________

Fit2DimDialog::~Fit2DimDialog()
{
// cout << "dtor Fit2DimDialog: " << this << endl;
   gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void Fit2DimDialog::RecursiveRemove(TObject * obj)
{
//   cout << "Fit2DimDialog::RecursiveRemove: this " << this << " obj "
//        << obj << " fSelHist " <<  fSelHist <<  endl;
   if (obj == fSelHist) {
 //      cout << "Fit2DimDialog::RecursiveRemove: this " << this << " obj "
 //       << obj << " fSelHist " <<  fSelHist <<  endl;
      if (fInteractive > 0) CloseDialog();
   }
}
//__________________________________________________________________________

void Fit2DimDialog::ClearFunctionList()
{
   TList temp;
   TList *lof = fSelHist->GetListOfFunctions();
   TIter next(lof);
   TObject *obj;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("TF2"))
         temp.Add(obj);
   }
   TIter next1(&temp);
   while ( (obj = next1()) ) {
      lof->Remove(obj);
//      delete obj;
   }
   temp.Delete();

   if (gPad) {
		TList temp1;
		TList *lof1 = gPad->GetListOfPrimitives();
		TIter next2(lof1);
		while ( (obj = next2()) ) {
			if (obj->InheritsFrom("TF2"))
				temp1.Add(obj);
		}
		TIter next3(&temp);
		while ( (obj = next3()) ) {
			lof1->Remove(obj);
	//      delete obj;
		}
      temp1.Delete();
      gPad->Modified();
      gPad->Update();
   }
}
//__________________________________________________________________________

void Fit2DimDialog::FitGausExecute()
{
   GausExecute(0);
}
//__________________________________________________________________________

void Fit2DimDialog::DrawGausExecute()
{
   GausExecute(1);
}
//__________________________________________________________________________

void Fit2DimDialog::GausExecute(Int_t draw_only)
{
   Int_t retval = 0;
   if (draw_only == 0 && fSelHist == NULL) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning",
                "No histogram nore graph defined\n Use Draw only" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   GetMarkers();
   cout << fFromX << " "  << " " <<fToX << endl;
 //  Double_t *xv, *par;
//    fFitFunc = new TF2(fFuncName.Data(), (void*)this,  &Fit2DimGauss,

   fFitFunc = new TF2(fFuncName.Data(), &Fit2DimGauss,
                      fFromX, fToX, fFromY, fToY, fNpar);
   Bool_t bound = kFALSE;
   fFitFunc->SetParameter(0, fOffX);
   if (fFixOffX |= 0) {
      fFitFunc->FixParameter(0, fOffX);
		bound = kTRUE;
   }
   fFitFunc->SetParName(0, "OffX");
   
   fFitFunc->SetParameter(1, fOffY);
   if (fFixOffY |= 0) {
      fFitFunc->FixParameter(1, fOffY);
		bound = kTRUE;
   }
   fFitFunc->SetParName(1, "OffY");
   fFitFunc->SetParameter(2, fPhi);
   if (fFixPhi |= 0) {
		fFitFunc->FixParameter(2, fPhi);
		bound = kTRUE;
	}
   fFitFunc->SetParName(2, "Phi");
   
   fFitFunc->SetParameter(3, fConstant);
   if (fFixConstant |= 0) {
		fFitFunc->FixParameter(3, fConstant);
		bound = kTRUE;
	}
   fFitFunc->SetParName(3, "Const");
   
   fFitFunc->SetParameter(4, fSigmaX);
   if (fFixSigmaX |= 0) {
		fFitFunc->FixParameter(4, fSigmaX);
		bound = kTRUE;
	}
   fFitFunc->SetParName(4, "SigmaX");
   
   fFitFunc->SetParameter(5, fSigmaY);
   if (fFixSigmaY |= 0) {
		fFitFunc->FixParameter(5, fSigmaY);
		bound = kTRUE;
	}
   fFitFunc->SetParName(5, "SigmaY");

   fFitFunc->SetLineWidth(fWidth);
   fFitFunc->SetLineColor(fColor);
   fFitFunc->SetNpx(fSelHist->GetNbinsX());
   fFitFunc->SetNpy(fSelHist->GetNbinsY());
   
 //  Dump();
//   cout << " ????????????????????????????? " << endl;
//   cout << "Formula: |" << fFitFunc->GetTitle() << "|"<< endl;
   if (draw_only != 0 ) {
		TCanvas * ccc = new TCanvas("ccc","fitted", 600,600);
      ccc->Draw();
      fFitFunc->Draw("colz");
      fFitFunc->Print();
      cout << " ????????????????????????????? " << endl;
      Dump();
   } else {
      TString fitopt = "R";     // fit in range
      if (fFitOptLikelihood)fitopt += "L";
      if (fFitOptQuiet)     fitopt += "Q";
      if (fFitOptVerbose)   fitopt += "V";
      if (fFitOptMinos)     fitopt += "E";
      if (fFitOptErrors1)   fitopt += "W";
      if (fFitOptIntegral)  fitopt += "I";
      if (fFitOptNoDraw)    fitopt += "0";
      if (fFitOptAddAll)    fitopt += "+";
      if (bound)            fitopt += "B";   // some pars are bound
      fitopt += "0";
      fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
      PrintCorrelation();
      fFitFunc->Print();
      fHistFitResult =  (TH2*)fSelHist->Clone();
      TString tname = fSelHist->GetName();
      tname.Prepend ("Fit_");
      fHistFitResult->SetName(tname);
      fHistFitResult->Reset();
		for (Int_t i = 1; i <= fHistFitResult->GetNbinsX(); i++) {
		   for (Int_t j = 1; j <= fHistFitResult->GetNbinsY(); j++) {
            Float_t xc = fHistFitResult->GetXaxis()->GetBinCenter(i);
            Float_t yc = fHistFitResult->GetYaxis()->GetBinCenter(j);
//            if (i == 50)
//            cout << i << " " << j << " " << xc << " " << yc << " " <<   fFitFunc->Eval(xc,yc)<< endl;
            Double_t fval =  fFitFunc->Eval(xc,yc);
            if ( fval >= 1. )
               fHistFitResult->SetBinContent(i,j, fFitFunc->Eval(xc,yc));
			}
		}
//      HistPresent * hpr = (HistPresent*)gROOT->FindObject("mypres");
 //     if ( hpr ) {

//			hpr ->ShowHist(fHistFitResult);
//      } else {
         fHistFitResult->Draw(fSelHist->GetDrawOption());
//		}
//      fFitFunc->Draw("colz");

      //     add to ListOfFunctions if requested
		if (fFitOptAddAll) {
		   TList *lof = fSelHist->GetListOfFunctions();
		   if (lof->GetSize() > 1) {
				TObject *last = lof->Last();
				lof->Remove(last);
				lof->AddFirst(last);
		   }
		}
	 }
	 fOffX     = fFitFunc->GetParameter(0);
	 fOffY     = fFitFunc->GetParameter(1);
	 fPhi      = fFitFunc->GetParameter(2);
	 fConstant = fFitFunc->GetParameter(3);
	 fSigmaX   = fFitFunc->GetParameter(4);
	 fSigmaY   = fFitFunc->GetParameter(5);
	 if (fAutoClearMarks) ClearMarkers();
	 IncrementIndex(&fFuncName);
//      PrintCorrelation();
   gPad->cd();
   gPad->Modified(kTRUE);
   gPad->Update();
}
//____________________________________________________________________________________

void Fit2DimDialog::GetGaussEstimate(TH2 * h, Double_t fromX, Double_t toX,
                                              Double_t fromY, Double_t toY,
                                       Double_t bg, TArrayD &par)
{
   Int_t frombin = h->FindBin(fromX);
   Int_t tobin = h->FindBin(toX);
   Double_t sum = 0;
   Double_t sumx = 0;
   Double_t sumx2 = 0;
   for (Int_t i = frombin; i <= tobin; i++) {
      Double_t cont = TMath::Max(h->GetBinContent(i) - bg, 0.);
      Double_t x = h->GetBinCenter(i);
      sum += cont;
      sumx += x * cont;
      sumx2 += x * x * cont;
   }
   par[0] = sum;
   if (sum > 0.) {
      par[1] = sumx / sum;
      if ( (sumx2 /sum - par[1]*par[1]) > 0)
         par[2] = TMath::Sqrt(sumx2 / sum - par[1]*par[1]);
      else
         par[2] = 1;
   } else {
      par[1] = 0.5 * (toX + fromX);
      par[2] = 1;
   }
}
//____________________________________________________________________________________

Int_t Fit2DimDialog::GetMaxBin(TH2 * h1, Int_t binlX, Int_t binuX, 
                                         Int_t binlY, Int_t binuY)
{
   Double_t maxcont = 0.;
   Int_t bin = (binuX - binlX) / 2;
   if (bin <= 0) {
      bin = -bin;
      return bin;
   }
   for (Int_t i = binlX; i <= binuX; i++) {
      Double_t cont = (Double_t) h1->GetBinContent(i);
      if (cont > maxcont) {
         maxcont = cont;
         bin = i;
      }
   };
//  cout << "MaxBin " << bin  << endl;
   return bin;
};
//____________________________________________________________________________________
void Fit2DimDialog::PrintMarkers()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL || fMarkers->GetEntries() <= 0) {
      cout << " No markers set" << endl;
   } else {
      fMarkers->Print();
   }
}
//____________________________________________________________________________________

Int_t Fit2DimDialog::GetMarkers()
{
   fNmarks = 0;
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL) {
      fNmarks  = fMarkers->GetEntries(); 
      if (fNmarks != 2) {
         cout << " Exactly 0 or 2 Markers needed: " << fNmarks << endl;
         ClearMarkers();
         return fNmarks;
      }
      fFromX = ((FhMarker *) fMarkers->At(0))->GetX();
      fToX   = ((FhMarker *) fMarkers->At(1))->GetX();
      if (fToX < fFromX) {
         Double_t temp = fFromX;
         fFromX = fToX;
         fToX   = temp;
      }
      fFromY = ((FhMarker *) fMarkers->At(0))->GetY();
      fToY   = ((FhMarker *) fMarkers->At(1))->GetY();
      if (fToY < fFromY) {
         Double_t temp = fFromY;
         fFromY = fToY;
         fToY   = temp;
      }
   }
   TAxis * xa = fSelHist->GetXaxis();
   TAxis * ya = fSelHist->GetYaxis();
   Double_t xl =  xa->GetBinLowEdge(xa->GetFirst());
   Double_t xu =  xa->GetBinLowEdge(xa->GetLast()) + xa->GetBinWidth(1);
   if ( fFromX < xl )
     fFromX = xl;
   if ( fToX > xu )
     fToX = xu;
   Double_t yl =  ya->GetBinLowEdge(ya->GetFirst());
   Double_t yu =  ya->GetBinLowEdge(ya->GetLast()) + ya->GetBinWidth(1);
   if ( fFromY < yl )
     fFromY = yl;
   if ( fToY > yu )
     fToY = yu;

   return fNmarks;
}
//____________________________________________________________________________________

void Fit2DimDialog::ClearMarkers() {
   if (fMarkers) fMarkers->Delete();
//   fMarkers = NULL;
   if (fSelPad) {
      fSelPad->Modified();
      fSelPad->Update();
   }
//   cout << "Clear Markers" << endl;
};
//____________________________________________________________________________________

Int_t  Fit2DimDialog::SetMarkers() {
   cout << "Request " << fReqNmarks << " Markers" << endl;
   Int_t nmarks = 0;
   if (fReqNmarks <= 0) {
      Int_t retval = 0;
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning","fNmarks <= 0" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return 0;
   }
   if ( fSelPad->GetAutoExec() )
      fSelPad->ToggleAutoExec();
   ClearMarkers();
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
      fMarkers = new  FhMarkerList();
      fSelHist->GetListOfFunctions()->Add(fMarkers);
   }
   for (Int_t i = 0; i < fReqNmarks; i++) {
      TMarker * m1  = (TMarker*)gPad->WaitPrimitive("TMarker");
   	m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
      if (m1 == NULL) break;
      Double_t x = m1->GetX();
      Double_t y = m1->GetY();
      if (i == 0) fFromX = x;
      if (i == 1) fToX = x;

//      if (fSelPad->GetLogx()) x = TMath::Power(10, x);
//      if (fSelPad->GetLogy()) y = TMath::Power(10, y);
      FhMarker *m = new FhMarker(x, y, 28);
      m->SetMarkerColor(6);
      m->Paint();
      fMarkers->Add(m);
      delete m1;
      nmarks++;
   }
   PrintMarkers();
   if (fSelPad) fSelPad->Update();
   if (fDialog) fDialog->ReloadValues();
   return nmarks;
};
//_______________________________________________________________________

void Fit2DimDialog::SetFittingOptions()
{
   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   row_lab->Add(new TObjString("CheckButton_Use Loglikelihood method"));
   row_lab->Add(new TObjString("RadioButton_Quiet, minimal printout"));
   row_lab->Add(new TObjString("RadioButton_Print one line / peak"));
   row_lab->Add(new TObjString("RadioButton_Verbose printout"));
   row_lab->Add(new TObjString("CheckButton_Use Minos to improve fit"));
   row_lab->Add(new TObjString("CheckButton_Set all errors to 1"));
   row_lab->Add(new TObjString("CheckButton_Use Integral of function in bin"));
   row_lab->Add(new TObjString("CheckButton_Dont draw result function"));
   row_lab->Add(new TObjString("CheckButton_Add all fitted functions to hist"));
   row_lab->Add(new TObjString("CheckButton_Print covariance matrix"));

   valp[ind++] = &fFitOptLikelihood    ;
   valp[ind++] = &fFitOptQuiet         ;
   valp[ind++] = &fFitOptOneLine       ;
   valp[ind++] = &fFitOptVerbose       ;
   valp[ind++] = &fFitOptMinos         ;
   valp[ind++] = &fFitOptErrors1       ;
   valp[ind++] = &fFitOptIntegral      ;
   valp[ind++] = &fFitOptNoDraw        ;
   valp[ind++] = &fFitOptAddAll        ;
   valp[ind++] = &fFitPrintCovariance  ;
   Bool_t ok;
   Int_t itemwidth = 240;
   ok = GetStringExt("Fitting options", NULL, itemwidth, fParentWindow
                     ,NULL, NULL, row_lab, valp);
   if (ok) SaveDefaults();
}
//________________________________________________________________________
void Fit2DimDialog::PrintCorrelation()
{
  if (fFitPrintCovariance) {
		TVirtualFitter *fitter = TVirtualFitter::GetFitter();
		if (fitter->GetCovarianceMatrix() == 0) {
		   cout << "Invalid fit result, cant get covariance matrix" << endl;
			return;
		}
      Int_t ntot = fitter->GetNumberTotalParameters();
		cout << "-- Correlationmatrix (Cov(i,j) / (err i * err j)) -----" << endl;
      printf("%9s", "          ");
		for (Int_t i=0; i < ntot; i++) {
         if (fitter->IsFixed(i) == 0)printf("%11s", fitter->GetParName(i));
      }
	   cout << endl;
      Int_t indi = 0;
		for (Int_t i =0; i < ntot; i++) {
         if (fitter->IsFixed(i) == 0) {
            printf("%10s", fitter->GetParName(i));
				Int_t indj = 0;
				for (Int_t j=0; j < ntot; j++) {
               if (fitter->IsFixed(j) == 0) {
					   Double_t ei =  fitter->GetParError(i);
					   Double_t ej =  fitter->GetParError(j);
					   Double_t cor = fitter->GetCovarianceMatrixElement(indi, indj)/(ei*ej);
					   printf("%10.3g ", cor);
                  indj++;
               }
				}
            indi++;
			cout << endl;
         }
		}
      for (Int_t i=0; i < ntot; i++) {
         if (fitter->IsFixed(i) == 0) cout << "-----------";
      }
	   cout << endl;
	}
}
//________________________________________________________________________

TH1 *Fit2DimDialog::FindHistInPad()
{
   TList * lop = gPad->GetListOfPrimitives();
   TIter next(lop);
   TH1 *hist = NULL;
   while (TObject *obj = next()) {
      if(obj->InheritsFrom("TH1")) {
         hist = (TH1*)obj;
         break;
      }
   }
   return hist;
}
//________________________________________________________________________

void Fit2DimDialog::FillHistRandom()
{
}
//_______________________________________________________________________

void Fit2DimDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fFitOptLikelihood = env.GetValue("Fit2DimDialog.FitOptLikelihood", 0);
   fFitOptQuiet      = env.GetValue("Fit2DimDialog.FitOptQuiet", 0);
   fFitOptOneLine    = env.GetValue("Fit2DimDialog.FitOptOneLine", 1);
   fFitOptVerbose    = env.GetValue("Fit2DimDialog.FitOptVerbose", 0);
   fFitOptMinos      = env.GetValue("Fit2DimDialog.FitOptMinos", 0);
   fFitOptErrors1    = env.GetValue("Fit2DimDialog.FitOptErrors1", 0);
   fFitOptIntegral   = env.GetValue("Fit2DimDialog.FitOptIntegral", 0);
   fFitOptNoDraw     = env.GetValue("Fit2DimDialog.FitOptNoDraw", 0);
   fFitOptAddAll     = env.GetValue("Fit2DimDialog.FitOptAddAll", 0);
   fFitPrintCovariance= env.GetValue("Fit2DimDialog.fFitPrintCovariance", 0);
   fAutoClearMarks   = env.GetValue("Fit2DimDialog.fAutoClearMarks", 0);
   fColor            = env.GetValue("Fit2DimDialog.Color", 4);
   fWidth            = env.GetValue("Fit2DimDialog.Width", 1);
   fStyle            = env.GetValue("Fit2DimDialog.Style", 1);
   fShowcof          = env.GetValue("Fit2DimDialog.Showcof", 1);
   fSigmaX           = env.GetValue("Fit2DimDialog.fSigmaX",  1.);
   fSigmaY           = env.GetValue("Fit2DimDialog.fSigmaY",  1.);
   fConstant         = env.GetValue("Fit2DimDialog.fConstant",100.);
   fOffX             = env.GetValue("Fit2DimDialog.fOffX",    0.);
   fOffY             = env.GetValue("Fit2DimDialog.fOffY",    0.);
   fPhi              = env.GetValue("Fit2DimDialog.fPhi",     0.);
}
//_______________________________________________________________________

void Fit2DimDialog::SaveDefaults()
{
   TEnv env(".hprrc");
   env.SetValue("Fit2DimDialog.FitOptLikelihood", fFitOptLikelihood);
   env.SetValue("Fit2DimDialog.FitOptQuiet",      fFitOptQuiet);
   env.SetValue("Fit2DimDialog.FitOptOneLine",    fFitOptOneLine);
   env.SetValue("Fit2DimDialog.FitOptVerbose",    fFitOptVerbose);
   env.SetValue("Fit2DimDialog.FitOptMinos",      fFitOptMinos);
   env.SetValue("Fit2DimDialog.FitOptErrors1",    fFitOptErrors1);
   env.SetValue("Fit2DimDialog.FitOptIntegral",   fFitOptIntegral);
   env.SetValue("Fit2DimDialog.FitOptNoDraw",     fFitOptNoDraw);
   env.SetValue("Fit2DimDialog.FitOptAddAll",     fFitOptAddAll);
   env.SetValue("Fit2DimDialog.fFitPrintCovariance", fFitPrintCovariance);
   env.SetValue("Fit2DimDialog.fAutoClearMarks",  fAutoClearMarks);
   env.SetValue("Fit2DimDialog.Color",            fColor);
   env.SetValue("Fit2DimDialog.Width",            fWidth);
   env.SetValue("Fit2DimDialog.Style",            fStyle);
   env.SetValue("Fit2DimDialog.fSigmaX",          fSigmaX);
   env.SetValue("Fit2DimDialog.fSigmaY",          fSigmaY);
   env.SetValue("Fit2DimDialog.fConstant",        fConstant);
   env.SetValue("Fit2DimDialog.fOffX",            fOffX );
   env.SetValue("Fit2DimDialog.fOffY",            fOffY);
   env.SetValue("Fit2DimDialog.fPhi",             fPhi);
   env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void Fit2DimDialog::CloseDialog()
{
 //  cout << "Fit2DimDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if (fDialog) fDialog->CloseWindowExt();
   delete this;
}
//_______________________________________________________________________

void Fit2DimDialog::CloseDown(Int_t wid)
{
//   cout << "Fit2DimDialog::CloseDown() " << endl;
   SaveDefaults();
   delete this;
}
//________________________________________________________________
void Fit2DimDialog::IncrementIndex(TString * arg)
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
