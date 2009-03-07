#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "TROOT.h"
#include "TQObject.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TCutG.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TGObject.h"
#include "TObjectTable.h"
#include "TTimer.h"
#include "TProfile.h"
#include "TPad.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TPolyLine.h"
#include "TPaveLabel.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "Buttons.h"
#include "TButton.h"
#include "TDiamond.h"
#include "HTCanvas.h"
#include "TContextMenu.h"
#include "TArray.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TRint.h"
#include "TList.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TArrayC.h"
#include "TArrayI.h"
#include "TExec.h"

#include "TGWidget.h"
#include "TGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitHist_Help.h"
#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FhContour.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbNamedArray.h"
#include "TMrbString.h"
#include "TMrbArrayD.h"
#include "TGMrbSliders.h"
#include "TGMrbInputDialog.h"
#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "FitOneDimDialog.h"
#include "SetHistOptDialog.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"

#include "hprbase.h"

//extern HistPresent* hp;
extern TFile *fWorkfile;
extern const char *fWorkname;
extern Int_t nHists;
//Int_t nPeaks;
//Double_t gTailSide;             // in fit with tail determines side: 1 left(low), -1 high(right)
//Float_t gBinW;
//extern Float_t gBinW;

enum dowhat { expand, projectx, projecty, statonly, projectf,
       projectboth , profilex, profiley};

ClassImp(FitHist)
//_______________________________________________________________________________
// constructor
FitHist::FitHist(const Text_t * name, const Text_t * title, TH1 * hist,
               const Text_t * hname, Int_t win_topx, Int_t win_topy,
               Int_t win_widx, Int_t win_widy)
        :TNamed(name, title)
{
   if (!hist) {
      cout << "NULL pointer in: " << name << endl;
      return;
   };

   if (gDirectory) {
      FitHist *hold =
          (FitHist *) gDirectory->GetList()->FindObject(GetName());
      if (hold) {
//         Warning("Build","Replacing existing : %s",GetName());
         gDirectory->GetList()->Remove(hold);
         delete hold;
      }
//      AppendDirectory();
      gDirectory->Append(this);
   }
   hp = (HistPresent *) gROOT->FindObject("mypres");
   if (!hp)
      cout << "running without HistPresent" << endl;
   fHname = hname;
//   Int_t pp = fHname.Index(".");
//   if(pp) fHname.Remove(0,pp+1);
//   cout << "ctor: " << GetName() << " hname: " << fHname.Data()<< endl;
   fCutPanel = NULL;
   fDialog  = NULL;
   fSetRange = kFALSE;
   fRangeLowX = 0;
   fRangeUpX = 0;
   fRangeLowY = 0;
   fRangeUpY = 0;
   fOrigLowX = hist->GetXaxis()->GetXmin();
   fOrigUpX  = hist->GetXaxis()->GetXmax();
   fOrigLowY = hist->GetYaxis()->GetXmin();
   fOrigUpY =  hist->GetYaxis()->GetXmax();
   fBinX_1 = fBinX_2 = -1;
   fLogx = 0;
   fLogy = 0;
   fLogz = 0;
   fUserContourLevels = 0;
//   datebox = 0;
   fExpInd = 0;

   fSelHist = hist;
   fFit1DimD = 0;
   fCalHist = NULL;
   fCalFitHist = NULL;
   fCalFunc = NULL;
   fSelPad = NULL;
   cHist   = NULL;
   expHist = NULL;
   fDateText = NULL;
   projHist = NULL;
   fTofLabels = NULL;
   fSelInside = kTRUE;
   fDeleteCalFlag = kFALSE;

   fActiveWindows = new TList();
   fPeaks = new TList();
   fActiveFunctions = new TList();
   fActiveCuts = new TList();
   fCmdLine = new TList();
   gROOT->GetListOfCleanups()->Add(this);

   if (hp) {
      fAllFunctions = hp->GetFunctionList();
      fAllWindows = hp->GetWindowList();
      fAllCuts = hp->GetCutList();
   }
//   fMarkers = new FhMarkerList(0);
   peaks = new TObjArray(0);
   fDimension = hist->GetDimension();
//  look for cuts and windows

   CheckList(fActiveCuts);
   CheckList(fActiveWindows);
   CheckList(fAllWindows);


   TList *lof = hist->GetListOfFunctions();
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
//         cout << "$$$$$$$$$$$$   "  << p << "  $$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
//         p->Dump();
 //        cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
         if (p->InheritsFrom(TMrbWindow::Class())) {
            TMrbWindow *w = dynamic_cast<TMrbWindow*>(p);
         	if (fDimension == 2)
            	fActiveCuts->Add(w);

         	if (fDimension == 1) {
            	if (fAllWindows->FindObject(w->GetName()));
	//               cout << p->GetName() << " already existing" <<endl;
            	else
               	fAllWindows->Add(w);
            	if (!fActiveWindows->FindObject(w->GetName()))
               	fActiveWindows->Add(w);
         	}
      	}
      }
   }
//   cout << "all windows ----------------------------" <<endl;
//   fAllWindows->Print();
//   cout << "active windows ----------------------------" <<endl;
   //  fActiveWindows->Print();

   TEnv env(".hprrc");         // inspect ROOT's environment
   fFitMacroName =
       env.GetValue("HistPresent.FitMacroName", "fit_user_function.C");

   RestoreDefaults();
//   cout << "FitMacroName " << fFitMacroName.Data()<< endl;

   fTemplateMacro = "TwoGaus";
   fFill1Dim      = env.GetValue("Set1DimOptDialog.fFill1Dim",      0);
   fHistFillColor = env.GetValue("Set1DimOptDialog.fHistFillColor", 1);
   fHistLineColor = env.GetValue("Set1DimOptDialog.fHistLineColor", 1);
   fHistLineWidth = env.GetValue("Set1DimOptDialog.fHistLineWidth", 1);
   fHistFillStyle = env.GetValue("Set1DimOptDialog.fHistFillStyle", 3001);
   fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
   f2DimBackgroundColor = env.GetValue("Set2DimOptDialog.f2DimBackgroundColor", 0);
   fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
   fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
   fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
   fDrawAxisAtTop = env.GetValue("Set1DimOptDialog.fDrawAxisAtTop", 0);
   fShowContour   = env.GetValue("Set1DimOptDialog.fShowContour", 0);
//   fShowErrors    = env.GetValue("Set1DimOptDialog.fShowErrors", 0);
   fErrorMode     = env.GetValue("Set1DimOptDialog.fErrorMode", "E");
   gStyle->SetErrorX(env.GetValue("Set1DimOptDialog.fErrorX", 0.));
   fDrawOpt2Dim   = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
   fShowZScale    = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
   if ( fShowZScale != 0 )fDrawOpt2Dim += "Z";
   fOptStat       = env.GetValue("WhatToShowDialog.fOptStat", 11111);
   fShowDateBox   = env.GetValue("WhatToShowDialog.fShowDateBox", 1);
   fShowStatBox   = env.GetValue("WhatToShowDialog.fShowStatBox", 1);
   fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay", 1);
   fShowTitle  = env.GetValue("WhatToShowDialog.fShowTitle", 1);
   fShowFitBox = env.GetValue("WhatToShowDialog.fShowFitBox", 1);
   fTitleCenterX  = env.GetValue("SetHistOptDialog.fTitleCenterX", 0);
   fTitleCenterY  = env.GetValue("SetHistOptDialog.fTitleCenterY", 0);
   fTitleCenterZ  = env.GetValue("SetHistOptDialog.fTitleCenterZ", 0);

   fSerialPx = 0;
   fSerialPy = 0;
   fSerialPf = 0;
   fFuncNumb = 0;
   fCutNumber = 0;
   wdw_numb = 0;
   if (env.GetValue("HistPresent.FitOptMinos", 0) != 0) fCallMinos = kTRUE;
   else                                                 fCallMinos = kFALSE;
   if (env.GetValue("HistPresent.FitOptKeepPar", 0) != 0)fKeepParameters  = kTRUE;
   else                                                  fKeepParameters = kFALSE;
   fOldMode = -1;
   fColSuperimpose = 1;
   fMax = hist->GetMaximum();
   fMin = hist->GetMinimum();
   fXmin = hist->GetXaxis()->GetXmin();
   fXmax = hist->GetXaxis()->GetXmax();
   if (fDimension == 2) {
      fYmin = hist->GetYaxis()->GetXmin();
      fYmax = hist->GetYaxis()->GetXmax();
   }
   TObject * obj = fSelHist->GetListOfFunctions()->FindObject("palette");
   if (obj) {
      cout << " Removing obj TPaletteAxis" << endl;
      fSelHist->GetListOfFunctions()->Remove(obj);
   }
   DisplayHist(hist, win_topx, win_topy, win_widx, win_widy);
   fLogx = cHist->GetLogx();
   fLogy = cHist->GetLogy();
   fLogz = cHist->GetLogz();
};

//________________________________________________________________

void FitHist::RecursiveRemove(TObject * obj)
{
//   cout << "FitHist::RecursiveRemove: " << obj << endl;
   fSelHist->GetListOfFunctions()->Remove(obj);
   fActiveCuts->Remove(obj);
   fActiveWindows->Remove(obj);
   fActiveFunctions->Remove(obj);
   if (obj == fFit1DimD) fFit1DimD = NULL;
   if (obj == fDialog) fDialog = NULL;
}

//------------------------------------------------------
// destructor
FitHist::~FitHist()
{
//    cout << " ~FitHist(): " << this << endl;
//   cout<< "enter FitHist  dtor "<< GetName()<<endl;
//   if(fMyTimer)fMyTimer->Delete();
//   fMyTimer=NULL;
   if (!expHist && hp && GeneralAttDialog::fRememberZoom) SaveDefaults(kTRUE);
   gDirectory->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fDialog != NULL ) fDialog->CloseDialog();
   if ( fFit1DimD ) fFit1DimD->CloseDialog();
   WindowSizeDialog::fNwindows -= 1;
   if ( expHist ) {
//      cout << "expHist " << expHist->GetName() << endl;
//      dont delete possible windows
      expHist->GetListOfFunctions()->Clear("nodelete");
      gDirectory->GetList()->Remove(expHist);
      delete expHist;
      expHist = 0;
   }
   if (fTofLabels) { delete fTofLabels; fTofLabels=NULL;}
   if (fCalFunc) delete fCalFunc;
   if (fDateText) delete fDateText;
   if (!cHist || !cHist->TestBit(TObject::kNotDeleted) ||
       cHist->TestBit(0xf0000000)) {
      cout << "~FitHist: " << this << " Canvas : " << cHist << " is deleted" << endl;

      cHist = 0;
   }

   if (cHist) {
      cHist->SetFitHist(NULL);
      if (fCanvasIsAlive) {
//         cout << " deleting " << cHist->GetName() << endl;
         delete cHist;
         cHist = 0;
      }
   }
   if (fCutPanel && fCutPanel->TestBit(TObject::kNotDeleted))
      delete fCutPanel;
//   if (fMarkers)
//      ClearMarks();
//      delete fMarkers;
   if (fActiveCuts)
      delete fActiveCuts;
   if (peaks)
      delete peaks;
   if (fCmdLine)
      delete fCmdLine;
//   if (datebox)
 //     delete datebox;
   gROOT->Reset();
};
//________________________________________________________________

void FitHist::SaveDefaults(Bool_t recalculate)
{
   if (!hp) return;      // not called from  Histpresenter

   if (!GeneralAttDialog::fRememberLastSet &&  !GeneralAttDialog::fRememberZoom) return;
//   cout << "Enter SaveDefaults " << endl;

//   TString defname("default/Last");
//   TEnv env(".hprrc");         // inspect ROOT's environment
//   env.SetValue("HistPresent.FitMacroName", fFitMacroName);

   Bool_t checkonly = kFALSE;
   if ( (!CreateDefaultsDir(mycanvas, checkonly)) ) return;
//   defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());

//   defname += "_";
//   defname += fHname;
//	Int_t ip = defname.Index(";");
//	if (ip > 0) defname.Resize(ip);
//   defname += ".def";

   TEnv * env = GetDefaults(fHname, kFALSE);
	if (!env) return;

   if (fDeleteCalFlag && !gSystem->AccessPathName(env->GetRcName())) {
      TString cmd(env->GetRcName());
      cmd.Prepend("rm ");
      gSystem->Exec(cmd.Data());
      cout << "Removing: " << cmd.Data() << endl;
      delete env;
      return;
   }

   env->SetValue("FitMacroName", fFitMacroName);
   if (TCanvas * ca =
       (TCanvas *) gROOT->FindObject(GetCanvasName())) {
      env->SetValue("LogX", ca->GetLogx());
      env->SetValue("LogY", ca->GetLogy());
      env->SetValue("LogZ", ca->GetLogz());
   } else {
      cerr << "Canvas deleted, cant find lin/log state" << endl;
   }
   if (recalculate && fSelHist->TestBit(TObject::kNotDeleted)) {
      Int_t first = fSelHist->GetXaxis()->GetFirst();
      Int_t last  = fSelHist->GetXaxis()->GetLast();
//      cout  << "recalculate fBinlx,ux:  " << first << " " << last << endl;
//      if (first > 1 || last < fSelHist->GetXaxis()->GetNbins()) {
         env->SetValue("fBinlx", first);
//         cout  << "recalculate fBinlx,ux:  " << first << " " << last << endl;
         env->SetValue("fBinux", last);
//      }
      if (fDimension == 2) {
         first = fSelHist->GetYaxis()->GetFirst();
         last  = fSelHist->GetYaxis()->GetLast();
//         if (first > 1 || last < fSelHist->GetYaxis()->GetNbins()) {
            env->SetValue("fBinly", first);
            env->SetValue("fBinuy", last);
//         }
      }
   } else {
//      cout << "take current fBinlx,y " << fBinlx  << " "  << fBinly  << endl;
      env->SetValue("fBinlx", fBinlx);
      env->SetValue("fBinux", fBinux);
      if (fDimension == 2) {
         env->SetValue("fBinly", fBinly);
         env->SetValue("fBinuy", fBinuy);
      }
   }
   //         check if hist is still alive
   if (fSelHist->TestBit(TObject::kNotDeleted)) {
      if (strlen(fSelHist->GetXaxis()->GetTitle()) > 0)
         env->SetValue("fXtitle", fSelHist->GetXaxis()->GetTitle());
      if (strlen(fSelHist->GetYaxis()->GetTitle()) > 0)
         env->SetValue("fYtitle", fSelHist->GetYaxis()->GetTitle());
//     save user contour
      if(fSelHist->InheritsFrom("TH2")) {
         TMrbNamedArrayI * colors = (TMrbNamedArrayI *)fSelHist->
            GetListOfFunctions()->FindObject("Pixel");
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
   	if (fSetRange) {
      	env->SetValue("fRangeLowX", fSelHist->GetXaxis()->GetXmin());
      	env->SetValue("fRangeUpX", fSelHist->GetXaxis()->GetXmax());
      	if (fDimension == 2) {
         	env->SetValue("fRangeLowY", fSelHist->GetYaxis()->GetXmin());
         	env->SetValue("fRangeUpY", fSelHist->GetYaxis()->GetXmax());
      	}
   	}
   }

//   cout << "env->SaveLevel(kEnvLocal): " << fBinlx << " " << fBinux << endl;
   env->SaveLevel(kEnvLocal);
   delete env;
   return;
}
//________________________________________________________________

void FitHist::RestoreDefaults()
{
   TEnv * lastset = GetDefaults(fHname);
//   cout << "RestoreDefaults() " << lastset<< endl;
	if (!lastset) return;
   fFitMacroName = lastset->GetValue("FitMacroName",fFitMacroName.Data());
   if (hp && (GeneralAttDialog::fRememberLastSet || GeneralAttDialog::fRememberZoom)) {
      fLogy = lastset->GetValue("LogY", fLogy);
      fRangeLowX = fSelHist->GetXaxis()->GetXmin();
      fRangeUpX = fSelHist->GetXaxis()->GetXmax();
      if (lastset->Lookup("fRangeLowX")) {
         fRangeLowX = lastset->GetValue("fRangeLowX", fRangeLowX);
         fRangeUpX = lastset->GetValue("fRangeUpX", fRangeUpX);
         fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(), fRangeLowX, fRangeUpX);
         fSetRange = kTRUE;
//         cout << "fRangeLowX " << fRangeLowX << endl;
      }
      fBinlx = fSelHist->GetXaxis()->GetFirst();
      fBinux = fSelHist->GetXaxis()->GetLast();
      if (lastset->Lookup("fBinlx")) {
         fBinlx = lastset->GetValue("fBinlx", fBinlx);
         fBinux = lastset->GetValue("fBinux", fBinux);
         fSelHist->GetXaxis()->SetRange(fBinlx, fBinux);
      }

      if (lastset->Lookup("fXtitle"))
         fSelHist->GetXaxis()->SetTitle(lastset->GetValue("fXtitle", ""));
      if (lastset->Lookup("fYtitle"))
         fSelHist->GetYaxis()->SetTitle(lastset->GetValue("fYtitle", ""));

      if (is2dim(fSelHist)) {
         fLogz = lastset->GetValue("LogZ", fLogz);
         fRangeLowY = fSelHist->GetYaxis()->GetXmin();
         fRangeUpY = fSelHist->GetYaxis()->GetXmax();
         if (lastset->Lookup("fRangeLowY")) {
            fRangeLowY = lastset->GetValue("fRangeLowY", fRangeLowY);
            fRangeUpY = lastset->GetValue("fRangeUpY", fRangeUpY);
            fSelHist->GetYaxis()->Set(fSelHist->GetNbinsY(), fRangeLowY,
                                  fRangeUpY);
            fSetRange = kTRUE;
         }
         fBinly = fSelHist->GetYaxis()->GetFirst();
         fBinuy = fSelHist->GetYaxis()->GetLast();
         if (lastset->Lookup("fBinly")) {
            fBinly = lastset->GetValue("fBinly", fBinly);
            fBinuy = lastset->GetValue("fBinuy", fBinuy);
            fSelHist->GetYaxis()->SetRange(fBinly, fBinuy);
         }
         if (lastset->Lookup("Contours")) {
            fUserContourLevels = lastset->GetValue("Contours", 0);
            if (fUserContourLevels > 0) {
               TString temp;
         		Double_t * uc = new Double_t[fUserContourLevels];
          		TMrbNamedArrayI * colors = new TMrbNamedArrayI("Pixel", fHname.Data());
               colors->Set(fUserContourLevels);
               Double_t all_levels = 0;
         		for (Int_t i = 0; i < fUserContourLevels; i++) {
                  temp = "Cont";
                  temp += i;
                  uc[i] = lastset->GetValue(temp.Data(), (Double_t)0);
                  all_levels += uc[i];
                  temp = "Pix";
                  temp += i;
                  (*colors)[i] = lastset->GetValue(temp.Data(), (Int_t)0);
         		}
   				if (all_levels != 0) fSelHist->SetContour(fUserContourLevels, uc);

//   				Int_t allcolors = 0;
//   				for (Int_t i=0; i< fUserContourLevels; i++) allcolors += (*colors)[i];
					if (colors->GetSum() != 0) {
				 //  set new Palette
     			   	fSelHist->GetListOfFunctions()->Add(colors);
//                  colors->Dump();
      				SetUserPalette(1001, colors);
					}
         		if (uc) delete [] uc;
            }
         }
      }
   }
   if (lastset)
      delete lastset;
}
//______________________________________________________________________________________

void FitHist::handle_mouse()
{
   Double_t fChi2Limit = 5;
   static TF1 * gFitFunc;
   Double_t cont, sum, mean;
   static Double_t gconst, center, sigma, bwidth,  esigma;
   static Double_t startbinX_lowedge, startbinY_lowedge;
   static Double_t chi2 = 0;
   static Int_t startbinX, startbinY;
   static Double_t ratio1, ratio2;
   static Int_t px1old, py1old, px2old, py2old;

   static Bool_t is2dim = kFALSE;
   static Bool_t first_fit = kFALSE;
   static Bool_t skip_after_TCUTG = kFALSE;
   static TH1 * hist = 0;
   static Int_t npar = 0;
   static TLine * lowedge = 0;
   static TLine * upedge = 0;
   static TBox * box = 0;
   static Int_t nrows = 4;
   Int_t px, py;
   if (gROOT->GetEditorMode() != 0) return;
   Int_t event = gPad->GetEvent();
   if (event ==  kKeyPress) {
//    cout << "px: "  << (char)gPad->GetEventX() << endl;
      char ch = (char)gPad->GetEventX();
      if ( ch == 'C' ||ch == 'c') {
         TRootCanvas *rc =  (TRootCanvas*)cHist->GetCanvasImp();
//        if (cHist->GetAutoExec()) cHist->ToggleAutoExec();
//         gSystem->Sleep(200);
         rc->ShowEditor(kFALSE);
         rc->SendCloseMessage();
         return;
      }
   }
   TObject *select = gPad->GetSelected();
   if (!select) return;
   if (event == kButton1Up && skip_after_TCUTG) {
      skip_after_TCUTG = kFALSE;
      return;
   }
   if (event == kButton1Down) {
      if (gROOT->FindObject("CUTG")) {
         cout << "handle_mouse(): UpdateCut();" << endl;
         UpdateCut();
         skip_after_TCUTG = kTRUE;
         return;
      }
   }
   if (select->InheritsFrom("TPave")) return;
   px = gPad->GetEventX();
   py = gPad->GetEventY();
//  check if lin / log scale changed
   if (hp && GeneralAttDialog::fRememberZoom) {
   	if (gPad->GetLogx() !=  fLogx ||
      	 gPad->GetLogy() !=  fLogy ||
      	 gPad->GetLogz() !=  fLogz ) {
      	 fLogx = gPad->GetLogx();
      	 fLogy = gPad->GetLogy();
      	 fLogz = gPad->GetLogz();
      	 if      (fDimension == 1) cHist->SetLog(fLogy);
      	 else if (fDimension == 2) SetLogz(fLogz);
      	 SaveDefaults();
   	}
      if (event == kButton1Up) {
         if (select->IsA() == TAxis::Class()) {
            if (fDimension == 2 || !strncmp(select->GetName(), "xaxis", 5)) {
//               cout << "handle_mouse " << GetSelHist()->GetName() << endl;
               fBinlx = fSelHist->GetXaxis()->GetFirst();
               fBinux = fSelHist->GetXaxis()->GetLast();
               if (fDimension == 2) {
                  fBinly = fSelHist->GetYaxis()->GetFirst();
                  fBinuy = fSelHist->GetYaxis()->GetLast();
               }
               SaveDefaults();
               return;
            }
         }
      }
   }
   if (event == kButton1Down) {
      if (select->InheritsFrom("TF1")) {
         TF1 *f = (TF1*)select;
         cout  << endl << "------  Function " << f->GetName() << " --------" << endl;
         for (Int_t i = 0; i < f->GetNpar(); i++) {
            cout << f->GetParName(i) << "  " << f->GetParameter(i) << endl;
         }
         cout  << endl << "----------------------------" << endl;
         TH1 * h = Hpr::FindHistOfTF1(gPad, f->GetName(), 1);
         if (h) cout << "popped " << f << endl;
         return;
      }
   }
//   if ( (!fLiveStat1Dim && fDimension == 1) || (!fLiveStat2Dim && fDimension == 2) ||
//         fDimension == 3 )  return;
   if ( fDimension == 3 )  return;

   TEnv env(".hprrc");
   if (event == kButton1Down) {
      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(cHist, px, py))) {
         TList * lofp = gPad->GetListOfPrimitives();
         if (!lofp) return;
         TIter next(lofp);
         TObject * obj = 0;
         while ( (obj = next()) ) {
            if (obj->InheritsFrom("TH3")) return;
            if (obj->InheritsFrom("TH1")) hist = (TH1*)obj;
         }
         if (!hist) {
            cout << "No histogram found" << endl;
            return;
         }
         TString sopt = hist->GetOption();
//         cout << " sopt " << sopt << endl;
         if (sopt.Contains("SURF", TString::kIgnoreCase) ||
             sopt.Contains("LEGO", TString::kIgnoreCase)) {
             return;
         }

         if (hist->GetDimension() == 3) {
//            cout << "3d histogram not supported" << endl;
            return;
         } else if (hist->GetDimension() == 2){
            is2dim = kTRUE;
         } else {
            is2dim = kFALSE;
         }

         if (lowedge) {
            delete lowedge; lowedge = 0;
            delete upedge; upedge = 0;
         }

         if (box) {delete box; box = 0;};
         if (gFitFunc) { delete gFitFunc; gFitFunc = 0;}
         first_fit = kFALSE;
         Axis_t xx = gPad->AbsPixeltoX(px);
         startbinX = hist->GetXaxis()->FindBin(xx);
         startbinX_lowedge = hist->GetXaxis()->GetBinLowEdge(startbinX);

         if (is2dim) {
            Axis_t yy = gPad->AbsPixeltoY(py);
            startbinY = hist->GetYaxis()->FindBin(yy);
            startbinY_lowedge = hist->GetYaxis()->GetBinLowEdge(startbinY);
            cont = hist->GetBinContent(startbinX, startbinY);
            nrows = 4;
         } else {
            bwidth = hist->GetBinWidth(startbinX);
            esigma = hist->GetBinWidth(startbinX) * TMath::Sqrt(12.);
            cont = hist->GetBinContent(startbinX);
            gconst = cont * bwidth / TMath::Sqrt(2*TMath::Pi() * esigma);
            center = hist->GetBinCenter(startbinX);
				fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
				fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);

            if (fLiveGauss) {
               if (fLiveBG) {
                  gFitFunc = new TF1("fitfunc", "pol1+gaus(2)");
                  npar = 5;
                  if (nrows != 6) {delete fTofLabels; fTofLabels = 0;}
                  nrows = 6;
               } else {
                  gFitFunc = new TF1("fitfunc", "gaus");
 //                 gFitFunc ->SetParameters(gconst, center, esigma);
                 npar = 3;
                 if (nrows != 5) {delete fTofLabels; fTofLabels = 0;}
                 nrows = 5;
               }
            } else {
               if (nrows != 4) {delete fTofLabels; fTofLabels = 0;}
               nrows = 4;
            }
         }
//         if ( kTRUE ) {
         if ( !is2dim )
            fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
         if ( is2dim )
            fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
         if (fLiveStat1Dim && !is2dim || fLiveStat2Dim && is2dim) {
         	if (!fTofLabels) {
            	TOrdCollection rowlabels;
            	if (is2dim) {
               	rowlabels.Add(new TObjString("Low left Bin: Number (x,y), Low edge (x,y)"));
               	rowlabels.Add(new TObjString("Up right Bin: Number (x,y), Up edge (x,y)"));
            	} else {
               	rowlabels.Add(new TObjString("Low left Bin: Number, Low edge"));
               	rowlabels.Add(new TObjString("Up right Bin: Number, Up edge"));
            	}
            	rowlabels.Add(new TObjString("Current Bin: Content "));
               if (is2dim)
            	   rowlabels.Add(new TObjString("Integral, Mean Z"));
               else
            	   rowlabels.Add(new TObjString("Integral, Mean"));
            	TOrdCollection values;
            	if (is2dim) {
               	values.Add(new TObjString(Form("%d,  %d,  %lg,  %lg",
                        	 startbinX, startbinY, startbinX_lowedge , startbinY_lowedge)));
               	values.Add(new TObjString(Form("%d,  %d,  %lg,  %lg",
                        	 startbinX, startbinY,
                        	 startbinX_lowedge + hist->GetBinWidth(startbinX),
                        	 startbinY_lowedge + hist->GetYaxis()->GetBinWidth(startbinY))));
               	values.Add(new TObjString(Form("%lg", cont)));
               	values.Add(new TObjString(Form("%lg", cont)));

            	} else {
                  TString timestring;
                  if (hist->GetXaxis()->GetTimeDisplay()) {
                     ConvertTimeToString((time_t)startbinX_lowedge, hist->GetXaxis(), &timestring);
                     timestring.Prepend(Form("%d, ", startbinX));
                     values.Add(new TObjString(timestring.Data()));
                     ConvertTimeToString((time_t)(startbinX_lowedge + hist->GetBinWidth(startbinX)),
                                                  hist->GetXaxis(), &timestring);
                     timestring.Prepend(Form("%d, ", startbinX));
                     values.Add(new TObjString(timestring.Data()));

                  } else {
               	   values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge )));
               	   values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge
                     	  + hist->GetBinWidth(startbinX))));
                  }
               	values.Add(new TObjString(Form("%lg", cont)));
               	values.Add(new TObjString(Form("%lg", cont)));
               	 if (fLiveGauss) {
                  	 if (fLiveBG) {
                     	 rowlabels.Add(new TObjString("Background: const, slope"));
                     	 values.Add(new TObjString("no fit done yet"));
                  	 }
                  	 rowlabels.Add(new TObjString("Gaus: Cont, Mean, Sigma"));
                  	 values.Add(new TObjString("no fit done yet"));
               	}
            	}
            	TString title("Stat. in sel. region of: ");
            	title += fHname.Data();
            	Int_t xw = gPad->GetCanvas()->GetWindowTopX();
            	Int_t yw = gPad->GetCanvas()->GetWindowTopY() + gPad->GetCanvas()->GetWindowHeight() + 28;
            	fTofLabels = new TableOfLabels(0, &title, 1, nrows, &values,
                        	  0, &rowlabels, xw, yw);
            	fTofLabels->Connect("Destroyed()", "FitHist", this, "ClearTofl()");
	//            pressed = kTRUE;
         	} else {
            	fTofLabels->SetLabelText(0,0,Form("%d", startbinX));
            	fTofLabels->SetLabelText(0,1,Form("%d", startbinX));
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg", cont));
         	}
         }
         ratio1 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
         px1old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio1*(gPad->GetUxmax() - gPad->GetUxmin()));
         if (is2dim) {
            px2old = gPad->XtoAbsPixel(gPad->GetUxmax());
            Double_t yru = gPad->GetUymax() - gPad->GetUymin();
            ratio1 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru ;
            Double_t yu = gPad->GetUymin() + ratio1 * yru;
            py1old = gPad->YtoAbsPixel(yu);
            py2old = gPad->YtoAbsPixel(gPad->GetUymax());
         } else {
            px2old = px1old;
            py1old = gPad->YtoAbsPixel(gPad->GetUymin());
            py2old = gPad->YtoAbsPixel(gPad->GetUymax());
         }
         gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
         gVirtualX->SetLineColor(-1);
      }
   }  else if (event == kButton1Motion || event == kButton1Up) {
//      if(!hist || !fTofLabels) return;
      if(!hist) return;
      if (select->InheritsFrom("TH1")) {
         TString sopt = hist->GetOption();
         if (sopt.Contains("SURF", TString::kIgnoreCase) ||
             sopt.Contains("LEGO", TString::kIgnoreCase)) {
             return;
         }
      }

      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(cHist, px, py))){
         if (select->InheritsFrom("TH3")) return;
         Int_t px = gPad->GetEventX();
         Axis_t xx = gPad->AbsPixeltoX(px);
         Axis_t yy = gPad->AbsPixeltoY(py);
         Int_t binX = hist->GetXaxis()->FindBin(xx);
         Int_t binY;
         Int_t binXlow, binXup, binYlow, binYup;
         Axis_t XlowEdge, XupEdge, YlowEdge = 0, YupEdge = 0;
         binXlow = TMath::Min(startbinX, binX);
         binXup  = TMath::Max(startbinX, binX);
         XlowEdge = hist->GetXaxis()->GetBinLowEdge(binXlow);
         XupEdge  = hist->GetXaxis()->GetBinLowEdge(binXup) + hist->GetXaxis()->GetBinWidth(binXup);

         if (is2dim) {
            binY = hist->GetYaxis()->FindBin(yy);
            binYlow = TMath::Min(startbinY, binY);
            binYup  = TMath::Max(startbinY, binY);
            YlowEdge = hist->GetYaxis()->GetBinLowEdge(binYlow);
            YupEdge  = hist->GetYaxis()->GetBinLowEdge(binYup) + hist->GetYaxis()->GetBinWidth(binYup);
            if (fTofLabels) {
//            if (fLiveStat2Dim && fTofLabels) {
             	cont = hist->GetBinContent(binX, binY);
            	sum = hist->Integral(binXlow, binXup, binYlow, binYup);
            	Int_t totbins = (binYup - binYlow +1) * (binXup - binXlow +1);
            	if (totbins > 0) mean = sum / (Double_t)totbins;
            	else             mean = 0;

            	fTofLabels->SetLabelText(0,0,Form("%d,  %d,  %lg,  %lg", binXlow, binYlow, XlowEdge, XupEdge));
            	fTofLabels->SetLabelText(0,1,Form("%d,  %d,   %lg,  %lg", binXup,  binYup,  YlowEdge, YupEdge));
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg,  %lg", sum, mean));
            }
         } else {
              if (fTofLabels) {
//              if (fLiveStat1Dim && fTofLabels) {
              Double_t sumx = 0;
            	cont = hist->GetBinContent(binX);
            	sum = hist->Integral(binXlow, binXup);
//           	Int_t totbins = (binXup - binXlow +1);
               for (Int_t ibin = binXlow; ibin <= binXup; ibin++)
                  sumx = sumx + hist->GetBinContent(ibin) * hist->GetBinCenter(ibin);
            	if (sum > 0) mean = sumx / sum;
            	else             mean = 0;
               TString timestring;
               if (hist->GetXaxis()->GetTimeDisplay()) {
                  ConvertTimeToString((time_t)XlowEdge, hist->GetXaxis(), &timestring);
                  timestring.Prepend(Form("%d, ", binXlow));
            	   fTofLabels->SetLabelText(0, 0, timestring.Data());
                  ConvertTimeToString((time_t)XupEdge, hist->GetXaxis(), &timestring);
                  timestring.Prepend(Form("%d, ", binXup));
            	   fTofLabels->SetLabelText(0, 1, timestring.Data());
               } else {
            	   fTofLabels->SetLabelText(0,0,Form("%d,  %lg", binXlow, XlowEdge));
               	fTofLabels->SetLabelText(0,1,Form("%d,  %lg", binXup, XupEdge ));
               }
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg,  %lg", sum, mean));
            	if (fLiveGauss &&  (binXup - binXlow - npar) > 0) {
               	Int_t ndf = binXup - binXlow - npar;
               	if (first_fit) chi2 = gFitFunc->GetChisquare();
               	if (fLiveBG) {
                  	if (!first_fit                    ||
                     	 gFitFunc->GetParameter(2) < 0 ||
                     	 gFitFunc->GetParameter(4) < 0 ||
                     	 chi2 / ndf > fChi2Limit) {
	//                     	 cout << " old par at bin " << binX << ": ";
	//                     	 for (Int_t i = 0; i < gFitFunc->GetNpar(); i++)
	//                        	cout  << gFitFunc->GetParameter(i) << " ";
	//                     	 cout << chi2 << " " << ndf << endl;
                     		 gFitFunc->SetParameter(0, cont);
                      		 gFitFunc->SetParameter(1, 0);
                    			 gFitFunc->SetParameter(2, sum * bwidth);
                     		 gFitFunc->SetParameter(3, 0.5 * (XupEdge + XlowEdge));
                     		 gFitFunc->SetParameter(4, 0.25 * (XupEdge - XlowEdge) );
	 //                    	 cout << " new par: " << cont << " 0.0 "  << sum * bwidth << " "
	//                        	  << 0.5 * (XupEdge + XlowEdge)<< " "
	//                        	  << 0.5 * (XupEdge - XlowEdge)<< endl;
                  	}
               	} else {
                  	if (!first_fit                   ||
                     	gFitFunc->GetParameter(0) < 0 ||
                     	gFitFunc->GetParameter(3) < 0 ||
                     	chi2 / ndf > 5) {
	//                        cout << " old par at bin " << binX << ": ";
	//                     	for (Int_t i = 0; i < gFitFunc->GetNpar(); i++)
	//                        	cout  << gFitFunc->GetParameter(i) << " ";
	//                     	cout << chi2 << " " << ndf << endl;
                    			gFitFunc->SetParameter(0, sum * bwidth);
                     		gFitFunc->SetParameter(1, 0.5 * (XupEdge + XlowEdge));
                     		gFitFunc->SetParameter(2, 0.5 * (XupEdge - XlowEdge));
	//                        cout << " new par: " << sum * bwidth << " "
	//                        	  << 0.5 * (XupEdge + XlowEdge)<< " "
	//                        	  << 0.5 * (XupEdge - XlowEdge)<< endl;
                  	}
               	}
               	hist->Fit(gFitFunc, "RNLQ", "",XlowEdge ,XupEdge );
               	first_fit = kTRUE;
                  gFitFunc->SetRange(XlowEdge ,XupEdge);
               	gFitFunc->Draw("same");
               	gPad->Update();
               	gPad->GetCanvas()->GetFrame()->SetBit(TBox::kCannotMove);
               	if (fLiveBG) {
                  	sigma = gFitFunc->GetParameter(4);
                  	gconst = gFitFunc->GetParameter(2) *
                              	TMath::Sqrt(2*TMath::Pi() * sigma) / bwidth;
                  	center = gFitFunc->GetParameter(3);
                  	fTofLabels->SetLabelText(0,4,Form("%lg,  %lg",
                     	gFitFunc->GetParameter(0), gFitFunc->GetParameter(1)));
                  	fTofLabels ->SetLabelText(0,5, Form("%lg,  %lg,  %lg",
                              	 gconst, center, sigma));
               	} else {
                  	sigma = gFitFunc->GetParameter(2);
                  	gconst = gFitFunc->GetParameter(0) *
                              	TMath::Sqrt(2*TMath::Pi() * sigma) / bwidth;
                  	center = gFitFunc->GetParameter(1);
                  	fTofLabels ->SetLabelText(0, 4, Form("%lg,  %lg,  %lg",
                              	 gconst, center, sigma));
               	}
            	}
            }
         }
         if ( event == kButton1Motion) {
      	   gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
         	ratio2 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
      	   px2old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio2*(gPad->GetUxmax() - gPad->GetUxmin()));
            if (is2dim) {
               Double_t yru = gPad->GetUymax() - gPad->GetUymin();
         	   ratio2 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru;
      	      py2old = gPad->YtoAbsPixel(gPad->GetUymin() + ratio2 * yru );
            }
      	   gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
      	   gVirtualX->SetLineColor(-1);
         } else {
            ClearMarks();
            fMarkers = new FhMarkerList();
            fSelHist->GetListOfFunctions()->Add(fMarkers);
            if (is2dim) {
               box = new TBox(XlowEdge, YlowEdge, XupEdge, YupEdge);
               box->SetLineColor(-1);
               box->SetFillStyle(0);
               box->Draw();
               fMarkers->Add(new FhMarker(XlowEdge, YlowEdge, 28));
               fMarkers->Add(new FhMarker(XupEdge,  YupEdge, 28));
               PaintMarks();
            } else {
            	lowedge = new TLine(XlowEdge, hist->GetYaxis()->GetXmin(),
                                   XlowEdge, hist->GetYaxis()->GetXmax());
         	   upedge  = new TLine(XupEdge, hist->GetYaxis()->GetXmin(),
                                   XupEdge, hist->GetYaxis()->GetXmax());
         	   lowedge->SetLineColor(4); lowedge->Draw();
         	   upedge->SetLineColor(4); upedge->Draw();
               fMarkers->Add(new FhMarker(XlowEdge, yy, 28));
               fMarkers->Add(new FhMarker(XupEdge,  yy, 28));
               PaintMarks();
            }
         }
         if (fTofLabels)fTofLabels->RaiseWindow();
      }
   }
}
//_____________________________________________________________________________________

void FitHist::DrawTopAxis() {
//   cout << "  add axis (channels) on top" << endl;
   TGaxis *naxis;
   Axis_t x1, y1, x2, y2;
//      TString side("-");
   Float_t loff;
   Float_t lsize;
   Int_t ndiv;

   x1 = cHist->GetUxmin();
   x2 = cHist->GetUxmax();
   y1 = cHist->GetUymax();
   y2 = y1;
   loff = 0.0;
   ndiv = fSelHist->GetXaxis()->GetNdivisions();
   lsize = fSelHist->GetXaxis()->GetLabelSize();
   cHist->cd();
   Axis_t blow = (Axis_t)(fSelHist->GetXaxis()->GetFirst() - 1);
   Axis_t bup  = (Axis_t)(fSelHist->GetXaxis()->GetLast());
   naxis = new TGaxis(x1, y1, x2, y2,blow, bup, ndiv, "-W");
   naxis->SetGridLength(-0.8);
   naxis->SetLabelOffset(loff);
   naxis->SetLabelSize(lsize);
   naxis->Paint();
}
//_____________________________________________________________________________________
// Show histograms

void FitHist::DisplayHist(TH1 * hist, Int_t win_topx, Int_t win_topy,
                          Int_t win_widx, Int_t win_widy)
{
   if (hist != NULL) {
      fSelHist = hist;
   } else {
      cout << "Null pointer in DisplayHist" << endl;
      return;
   }
   fOrigHist = hist;

//  construct name of canvas and pad
   fCname = "C_";
   fCname += GetName();
   fEname = "E_";
   fEname += GetName();
   fCtitle = fSelHist->GetName();
   fCtitle += ": ";
   fCtitle += fSelHist->GetTitle();
//   FitHist * fh_this = this;
//   if (is3dim(hist)) fh_this = NULL;
   cHist = new HTCanvas(fCname.Data(), fCtitle.Data(),
                        win_topx, win_topy, win_widx, win_widy, hp, this);
   fCanvasIsAlive = kTRUE;
//   if (hp)
//      hp->GetCanvasList()->Add(cHist);
   cHist->ToggleEventStatus();

   cHist->SetEditable(kTRUE);

   hist->SetDirectory(gROOT);
//   if ( (is2dim(hist) && fLiveStat2Dim) || (!is2dim(hist) && fLiveStat1Dim) ){

      TString cmd("((FitHist*)gROOT->FindObject(\"");
      cmd += GetName();
      cmd += "\"))->handle_mouse()";
//      cout << "FitHist::DisplayHist cmd: " << cmd << endl;
      cHist->AddExec("handle_mouse", cmd.Data());
//   }
   gDirectory = gROOT;
   fSelPad = cHist;
   fSelPad->cd();

   if (is3dim(hist)) {
      fSelPad->cd();
      Draw3Dim();
   } else if (is2dim(hist)) {
      fSelPad->cd();
      Draw2Dim();
   } else {
      fSelPad->cd();
      if (fLogy)
         cHist->SetLogy();
      Draw1Dim();
   }
   cHist->Update();
   cHist->GetFrame()->SetBit(TBox::kCannotMove);
   gSystem->ProcessEvents();
//  set the drawing area taking intoa account toolbars and decoration
   cHist->SetWindowSize(win_widx + (win_widx - cHist->GetWw()),
                        win_widy + (win_widy - cHist->GetWh()));
//  look if there exists a calibrated version of this histogram
   gSystem->ProcessEvents();
};

//------------------------------------------------------
// Magnify

void FitHist::Magnify()
{
   if (is2dim(fSelHist)) {
      Hpr::WarnBox("Magnify 2 dim not yet supported ");
      return;
   }
   Int_t newx = (Int_t) (2.1 * fSelHist->GetNbinsX());
   Int_t wx = cHist->GetWw();
   if (newx <= wx) {
      cout << "Screen resolution already good enough" << endl;
      return;
   } else {
      Float_t fac = (Float_t) newx / (Float_t) wx;
      TAxis *xaxis = fSelHist->GetXaxis();
      TAxis *yaxis = fSelHist->GetYaxis();

      xaxis->SetNdivisions(2080);
      yaxis->SetTickLength(0.03 / fac);
      cHist->SetLeftMargin(0.1 / fac);
      cHist->SetRightMargin(0.1 / fac);
      TPaveStats * st = (TPaveStats *)cHist->GetPrimitive("stats");
      if (st) {
        st->SetX1NDC(1 - 0.4 / fac);
        st->SetX2NDC(0.99);
      }
//      gStyle->SetStatW(0.19 / fac);
//      gStyle->SetTitleXSize(0.02 / fac);
//      gStyle->SetLabelOffset(0.001 / fac, "y");

   }
   Int_t newy = (Int_t) (0.95 * cHist->GetWh());
//   cHist->SetWindowSize(wx, cHist->GetWh());
//   cout << "newx, newy " << newx << " " << newy<< endl;
   cHist->SetCanvasSize(newx, newy);
   cHist->Modified(kTRUE);
   cHist->Update();
   cHist->GetFrame()->SetBit(TBox::kCannotMove);
}
//_______________________________________________________________________________________

void FitHist::Entire()
{
//   fSelHist->GetListOfFunctions()->Print();
//   fOrigHist->GetListOfFunctions()->Print();
   if (expHist) {
      cout << " Entire() expHist->Delete()" <<endl;
      expHist->GetListOfFunctions()->Clear("nodelete");
      TF1 *ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("backf");
      if (ff) delete ff;
           ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("tailf");
      if (ff) delete ff;
           ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("gausf");
      if (ff) delete ff;
      expHist->Delete();
      expHist = NULL;
   }
//   fSelHist->GetListOfFunctions()->Print();
//   fOrigHist->GetListOfFunctions()->Print();
   fSelHist = fOrigHist;
   fSelHist->SetMinimum(-1111);
   fSelHist->SetMaximum(-1111);
//   cout << fSelHist->GetXaxis()->GetTitle() << endl;
//   cout << fSelHist->GetYaxis()->GetTitle() << endl;

//   fSelHist->SetMaximum(fMax);
//   fSelHist->SetMinimum(fMin);
   fSelHist->GetXaxis()->SetRange(1, fSelHist->GetNbinsX());
   fBinlx = 1;
   fBinux =  fSelHist->GetNbinsX();
   if (is2dim(fSelHist)) {
      fBinly = 1;
      fBinuy =  fSelHist->GetNbinsY();
   }
   SaveDefaults(kTRUE);

   ClearMarks();
   fSelPad->cd();
   if (is2dim(fSelHist)) {
      fSelHist->GetYaxis()->SetRange(1, fSelHist->GetNbinsY());
      Draw2Dim();
   } else if (is3dim(fSelHist)){
      Draw3Dim();
   } else {
      Draw1Dim();
      TIter next(fSelHist->GetListOfFunctions());
      while (TObject *o = next()) {
         if (o->InheritsFrom("TF1")) {
           ((TF1*)o)->Draw("same");
         }
      }
   }
   cHist->Modified(kTRUE);
   cHist->Update();
   cHist->GetFrame()->SetBit(TBox::kCannotMove);
};
//_______________________________________________________________________________________

void FitHist::RebinOne()
{
   if (fSelHist == NULL) return;

   TString title(fSelHist->GetTitle());
   title += "_rebin";
   TString name(fSelHist->GetName());
   name += "_rebin";

   static Int_t ngroupX = 2;
   static Int_t ngroupY = 2;
   Bool_t ok;
   TH1 *newhist = NULL;
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("StringValue_Name of rebinned hist"));
   row_lab->Add(new TObjString("StringValue_Title of rebinned hist"));
   row_lab->Add(new TObjString("PlainIntVal_Rebin value X"));
   if (is2dim(fSelHist))
      row_lab->Add(new TObjString("PlainIntVal_Rebin value Y"));

   valp[ind++] = &name;
   valp[ind++] = &title;
   valp[ind++] = &ngroupX;
   if (is2dim(fSelHist))
      valp[ind++] = &ngroupY;

   Int_t itemwidth = 400;
   ok = GetStringExt("Rebin parameters", NULL, itemwidth, mycanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   title += ngroupX;
   name  += ngroupX;
   if (is2dim(fSelHist)) {
//      cout << "nyi" << endl;
      title += "_";
      name  += "_";
      title += ngroupY;
      name  += ngroupY;

      Int_t nbinsXnew = fSelHist->GetNbinsX() / ngroupX;
      Int_t nbinsYnew = fSelHist->GetNbinsY() / ngroupY;
      if (nbinsXnew * ngroupX != fSelHist->GetNbinsX()) {
         nbinsXnew += 1;
         TString warn("Warning: Number of bins X not multiple of ");
         warn += ngroupX;
         Hpr::WarnBox(warn.Data());
      }
      if (nbinsYnew * ngroupY != fSelHist->GetNbinsY()) {
         nbinsYnew += 1;
         TString warn("Warning: Number of bins Y not multiple of ");
         warn += ngroupY;
         Hpr::WarnBox(warn.Data());
      }
      TH2* h2 = (TH2*)fSelHist;
      newhist = h2->Rebin2D(ngroupX, ngroupY, name.Data());
   } else {
      newhist = (TH1*)fSelHist->Clone();
      newhist->GetListOfFunctions()->Clear();
      Int_t first = fSelHist->GetXaxis()->GetFirst();
      Int_t last = fSelHist->GetXaxis()->GetLast();
      newhist->Rebin(ngroupX);
      first /= ngroupX;
      last /= ngroupY;
      newhist->GetXaxis()->SetRange(first, last);
      newhist->SetName(name);
   }
   if (newhist) {
      newhist->SetTitle(title);
      if (hp) hp->ShowHist(newhist);
      else    newhist->Draw();
   }
}
//_______________________________________________________________________________________

void FitHist::RedefineAxis()
{
   if (expHist) {
      Hpr::WarnBox("RedefineAxis not implemented \
for expanded Histogram");
      return;
   }
   Int_t n;
   if (is2dim(fSelHist))
      n = 2;
   else
      n = 1;
   TArrayD xyvals(2 * n);
   //   Double_t *xyvals = new Double_t[2*n];
//  TString title("Set new axis limits");
   TOrdCollection *row_lab = new TOrdCollection();
   row_lab->Add(new TObjString("new xlow, xup"));
   xyvals[0] = fSelHist->GetXaxis()->GetXmin();
   if (!is2dim(fSelHist))
      xyvals[1] = fSelHist->GetXaxis()->GetXmax();
   else {
      xyvals[1] = fSelHist->GetYaxis()->GetXmin();
      xyvals[2] = fSelHist->GetXaxis()->GetXmax();
      xyvals[3] = fSelHist->GetYaxis()->GetXmax();
      row_lab->Add(new TObjString("new ylow, yup"));
   }
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Set new axis limits", itemwidth,
                       ncols, n, xyvals, precission, NULL, row_lab);
//   cout << ret << endl;
   if (ret >= 0) {
      if (!is2dim(fSelHist)) {
         fRangeLowX = xyvals[0];
         fRangeUpX = xyvals[1];
         fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(),
                                   fRangeLowX, fRangeUpX);
      } else {
         fRangeLowX = xyvals[0];
         fRangeUpX = xyvals[2];
         fRangeLowY = xyvals[1];
         fRangeUpY = xyvals[3];
         fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(),
                                   fRangeLowX, fRangeUpX);
         fSelHist->GetYaxis()->Set(fSelHist->GetNbinsY(),
                                   fRangeLowY, fRangeUpY);
      }
      fSetRange = kTRUE;
      cHist->Modified(kTRUE);
      cHist->Update();
   }
//   if (xyvals) delete [] xyvals;
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
};
//_______________________________________________________________________________________

void FitHist::AddAxis(Int_t where)
{
//   Double_t *xyvals = new Double_t[2];
   TString side;
   TArrayD xyvals(2);
//   TString title("Define axis limits");
   TOrdCollection *row_lab = new TOrdCollection();
   row_lab->Add(new TObjString("Lower, Upper"));
   if (where == 1) {            // xaxis
      xyvals[0] = cHist->GetFrame()->GetX1();
      xyvals[1] = cHist->GetFrame()->GetX2();
      if (cHist->GetLogx()) {
         xyvals[0] = TMath::Power(10, xyvals[0]);
         xyvals[1] = TMath::Power(10, xyvals[1]);
         side += "G";
      }
   } else {
      xyvals[0] = cHist->GetFrame()->GetY1();
      xyvals[1] = cHist->GetFrame()->GetY2();
      if (cHist->GetLogy()) {
         cout << "LogY " << endl;
         xyvals[0] = TMath::Power(10, xyvals[0]);
         xyvals[1] = TMath::Power(10, xyvals[1]);
         side += "G";
      }
   }
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, nrows = 1, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Define axis limits", itemwidth,
                       ncols, nrows, xyvals, precission, NULL, row_lab);
   if (ret >= 0) {
      TGaxis *naxis;
      Axis_t x1, y1, x2, y2;
//      TString side("-");
      Float_t loff = 0.05;
      Float_t lsize;
      Int_t ndiv;
      if (where == 1) {
         side += "-";
         x1 = cHist->GetFrame()->GetX1();
         x2 = cHist->GetFrame()->GetX2();
         y1 = cHist->GetFrame()->GetY2();
         y2 = y1;
         loff = 0.02;
         ndiv = fSelHist->GetXaxis()->GetNdivisions();
         lsize = fSelHist->GetXaxis()->GetLabelSize();
      } else {
         side += "+";
         x1 = cHist->GetFrame()->GetX2();
         x2 = x1;
         y1 = cHist->GetFrame()->GetY1();
         y2 = cHist->GetFrame()->GetY2();
         ndiv = fSelHist->GetYaxis()->GetNdivisions();
         lsize = fSelHist->GetYaxis()->GetLabelSize();
      }
      cHist->cd();
      naxis =
          new TGaxis(x1, y1, x2, y2, xyvals[0], xyvals[1], ndiv,
                     side.Data());
      naxis->SetLabelOffset(loff);
      naxis->SetLabelSize(lsize);
      naxis->Draw();
      cHist->Modified(kTRUE);
      cHist->Update();
   }
//   if (xyvals) delete [] xyvals;
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
};
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

//   if (fSetLevels) {
      fSelHist->GetContour(values);
//   } else {
//      contour->GetLevelArray()->Reset();
//   }
//   if (fSetColors) {
      *(contour->GetColorArray()) = *colors;
//   } else {
//      contour->GetLevelArray()->Reset();
//
//   }
   contour->Print();
   new Save2FileDialog(contour, NULL, GetMyCanvas());
//   if (OpenWorkFile(mycanvas)) {
//      contour->Write();
//      CloseWorkFile();
//   }

}
//_______________________________________________________________________________________

void FitHist::UseSelectedContour()
{
   if (!hp) return;
   if(hp->fSelectContour->GetSize() <= 0) {
      Hpr::WarnBox("No contour selected");
      return;
   } else if (hp->fSelectContour->GetSize() > 1) {
      Hpr::WarnBox("More then one selected\n\
Take first");
   }
   TObjString * objs = (TObjString *)hp->fSelectContour->At(0);
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
   cHist->Modified(kTRUE);
   cHist->Update();
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
   cHist->Modified(kTRUE);
   cHist->Update();
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
//      if (use_old && oldcol && i < old_ncont ) {
//         (*xyvals)[i] = fSelHist->GetContourLevel(i);
//         (*colors)[i] = oldcol->At(i);
//      } else {
//    assume colorindeces 51 - 100 ( rainbow colors)
         if (hp) {
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

//         Int_t colind = Int_t( (i + 1)* (50 / (Float_t)ncont)) + 50;
//         Int_t colind = TMath::Nint( i * ( nofLevels / (Float_t)(ncont))) + startIndex;
//         colind = TMath::Min(colind, startIndex + nofLevels - 1);
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
//   Int_t   allcolors = 0;
//   Double_t allconts = 0;
//   for (Int_t i=0; i< ncont; i++) {
//      allcolors += (*colors)[i];
//      allconts  += (*xyvals)[i];
 //  }
//   if (allconts > 0) {
   if (set_levels) {
      fUserContourLevels = ncont;
      h2->SetContour(ncont, xyvals->GetArray());
      AdjustMaximum(h2, xyvals);
   }
//	if (allcolors > 0) {
//	if (set_colors) {
 //  set new Palette
   if (fSetColors) {
      TMrbNamedArrayI * ca = new TMrbNamedArrayI("Pixel",fHname.Data());
      ca->Set(ncont, colors->GetArray());
      h2->GetListOfFunctions()->Add(ca);
      SetUserPalette(1001, colors);
   }
//	}
   SaveDefaults();
   cHist->Modified(kTRUE);
   cHist->Update();
}
//_______________________________________________________________________________________

void FitHist::PictToPSFile(Int_t plain_flag)
{
   if (fSelHist) {
      TString hname = fSelHist->GetName();
      hname += ".ps";
      Bool_t ok;
      hname =
          GetString("Write PS-file with name", hname.Data(), &ok,
                    mycanvas);
      if (!ok) {
         cout << " cancelled " << endl;
         return;
      }

      if (plain_flag > 0) {
         cHist->SetFillStyle(0);
         cHist->SetBorderMode(0);
         cHist->GetFrame()->SetFillStyle(0);
         cHist->GetFrame()->SetBorderMode(0);

         TPaveText *tpt = (TPaveText *) cHist->GetPrimitive("stats");
         if (tpt)
            tpt->SetFillStyle(0);
         tpt = (TPaveText *) cHist->GetPrimitive("title");
         if (tpt)
            tpt->SetFillStyle(0);
      }
      gStyle->SetPaperSize(19., 28.);
      cHist->SaveAs(hname.Data());
   }
};
//_______________________________________________________________________________________

void FitHist::PictToLP()
{
   if (fSelHist) {
      TString cmd = "lpr -P";
      cmd += gSystem->Getenv("PRINTER");
      TEnv env(".rootrc");      // inspect ROOT's environment
      const char *ccmd =
          env.GetValue("HistPresent.PrintCommand", cmd.Data());
      cmd = ccmd;
      Bool_t ok;
      cmd = GetString("Printer command: ", cmd.Data(), &ok, mycanvas);
      if (!ok) {
         //      cout << "canceled" << endl;
         return;
      }
      cHist->SetFillStyle(0);
      cHist->SetBorderMode(0);
      gStyle->SetPaperSize(19., 28.);
      cHist->SaveAs("temp_pict.ps");
      TString prcmd = cmd;
      prcmd += " ";
      prcmd += "temp_pict.ps";
      gSystem->Exec(prcmd.Data());
      if (cmd.Contains("lpr ")) {
         cmd(2, 1) = "q";
         gSystem->Exec(cmd.Data());
      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteFunctions()
{
   if (fSelHist) {
      ClearMarks();
      new Save2FileDialog(fSelHist->GetListOfFunctions(), NULL, GetMyCanvas());
//      if (OpenWorkFile()) {
//         fSelHist->GetListOfFunctions()->Write();
//         CloseWorkFile();
//      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteOutCanvas()
{
   if (fSelHist) {
      TString hname = cHist->GetName();
      hname += ".canvas";
      Bool_t ok;
      TObject *obj;
      hname =
          GetString("Save canvas with name", hname.Data(), &ok, mycanvas);
      if (!ok)
         return;
      if ((obj = gROOT->FindObject(hname.Data()))) {
         Hpr::WarnBox("Object with this name already exists");
         cout << setred << "Object with this name already exists: "
             << obj->ClassName() << setblack << endl;
         return;
      }
//      if (OpenWorkFile()) {
         if (cHist->GetAutoExec())
            cHist->ToggleAutoExec();
//         cHist->SetName(hname.Data());
         TCanvas *nc =
             new TCanvas(hname.Data(), cHist->GetTitle(), 50, 500, 720,
                         500);
//         TCanvas * nc = new TCanvas(hname.Data(), "xxx" ,720, 500);
         TIter next(cHist->GetListOfPrimitives());
         while ((obj = next())) {
            if (is_a_hist(obj)) {
               TH1 *hi = (TH1 *) obj;
               if (is2dim(hi)) {
                  hi->SetOption(fDrawOpt2Dim);
               } else {
                  TString drawopt;
                  if (fShowContour)
                     drawopt = "hist";
                     if ( fErrorMode != "none")
                        drawopt += "e1";
                  hi->SetOption(drawopt.Data());
                  if (fFill1Dim) {
                     if (fHistFillStyle == 0) fHistFillStyle = 1001;
                     hi->SetFillStyle(fHistFillStyle);
                     hi->SetFillColor(fHistFillColor);
                  } else
                     hi->SetFillStyle(0);
               }
            }
            gROOT->SetSelectedPad(nc);
            nc->GetListOfPrimitives()->Add(obj->Clone());
         }
         if (nc->GetAutoExec())
            nc->ToggleAutoExec();
//         nc->Write();
//         CloseWorkFile();
         new Save2FileDialog(nc, NULL, GetMyCanvas());
         delete nc;
//      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteOutHist()
{
   if (fSelHist) {
      new Save2FileDialog(fSelHist, NULL, GetMyCanvas());
   }
};

//_______________________________________________________________________________________

#include "time.h"

void AddAsString(Double_t x, TAxis * a, Int_t  prec, TOrdCollection *entries)
{
   struct tm * utctis;
   char CHLABEL[256];
   char *LABEL;
   LABEL  = &CHLABEL[0];
	TObjString * os;
   if (a == NULL || !(a->GetTimeDisplay())) {
     	TMrbString s(x, prec);		      // convert double to string
		os = new TObjString(s.Data());   // wrap TObject around it
   } else {
      time_t t = (time_t)x;
      TString tf(a->GetTimeFormat());
      Int_t itoff = tf.Index("%F");
      if (itoff > 0){
         TString tos = tf;
         tos.Remove(0,itoff + 2);
         tf.Resize(itoff);
      }
      utctis = gmtime(&t);
//      cout << "time " << t <<  " tf.Data() " <<  tf.Data() << endl;
      Int_t nchar = strftime(LABEL, 256, tf.Data(), utctis);
      if (nchar <= 0)os = new TObjString("xxx");
		else           os = new TObjString(LABEL);   // wrap TObject around it
   }
   entries->Add((TObject *) os);	   // add to collection of string
}
//_______________________________________________________________________________________

void FitHist::SetXaxisRange()
{
   SetAxisHist(cHist, fSelHist->GetXaxis());
}
//_______________________________________________________________________________________

void FitHist::SetYaxisRange()
{
   SetAxisHist(cHist, fSelHist->GetYaxis());
}
//_______________________________________________________________________________________

void FitHist::SetZaxisRange()
{
   SetAxisHist(cHist, fSelHist->GetZaxis());
}
//_______________________________________________________________________________________

void FitHist::Set2Marks()
{
   Int_t n = 2;
   static Float_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
//   Double_t *xyvals = new Double_t[n*2];
   TArrayD xyvals(2 * n);
   TOrdCollection *row_lab = new TOrdCollection();

   xyvals[0] = x1;
   xyvals[1] = x2;
   xyvals[2] = y1;
   xyvals[3] = y2;
   row_lab->Add(new TObjString("x,y value"));
   row_lab->Add(new TObjString("x,y value"));
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, nrows = n, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Marks", itemwidth,
                       ncols, nrows, xyvals, precission, NULL, row_lab);
   if (ret >= 0) {
      x1 = xyvals[0];
      x2 = xyvals[1];
      y1 = xyvals[2];
      y2 = xyvals[3];
      fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
      if (fMarkers == NULL) {
         fMarkers = new  FhMarkerList();
         fSelHist->GetListOfFunctions()->Add(fMarkers);
      }
      fMarkers->Add(new FhMarker(x1, y1, 28));
      fMarkers->Add(new FhMarker(x2, y2, 28));
   }
//   if (xyvals) delete [] xyvals;
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
   PaintMarks();
};
//_______________________________________________________________________________________

void FitHist::AddMark(TPad * pad, Int_t px, Int_t py)
{
   Float_t x, y;
   Int_t binx = 0, biny = 0;
   if (pad != (TVirtualPad *) cHist) {
      cout << " its not us " << endl;
      return;
   }
//   cout << " FitHist::AddMark " << endl;
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
 //     cout << "new  FhMarkerList()  " << endl;
      fMarkers = new  FhMarkerList();
      fSelHist->GetListOfFunctions()->Add(fMarkers);
   }
   x = pad->AbsPixeltoX(px);
   y = pad->AbsPixeltoY(py);
   if (pad->GetLogx()) x = TMath::Power(10, x);
   if (pad->GetLogy()) y = TMath::Power(10, y);
   binx = XBinNumber(fSelHist, x);
   if (is2dim(fOrigHist))
      biny = YBinNumber(fSelHist, y);
//   binx     = fXaxis.FindBin(x);
//   biny     = fYaxis.FindBin(y);
   FhMarker *m = new FhMarker(x, y, 28);
   m->SetMarkerColor(6);
//   m->Draw();
   m->SetMarkerColor(6);
//   m->Paint();
//   m->Print();
   fMarkers->Add(m);
	PaintMarks();

}
//_______________________________________________________________________________________

void FitHist::ClearMarks()
{
//
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL ) return;
   fSelHist->GetListOfFunctions()->Remove(fMarkers);
   TIter next(fMarkers);
   FhMarker * ti;
   while ( (ti = (FhMarker *) next()) ) {
      cHist->GetListOfPrimitives()->Remove(ti);
   }

   fMarkers->Delete();
//   fMarkers->Clear();
   cHist->Modified(kTRUE);
   cHist->Update();
};
//_______________________________________________________________________________________

Int_t FitHist::GetMarks(TH1 * hist)
{
   Int_t nval = 0;
   Axis_t xmin, xmax, ymin = 0, ymax = 0;
   if (hist) {
      TAxis *xaxis = hist->GetXaxis();
      Int_t first = xaxis->GetFirst();
      Int_t last = xaxis->GetLast();
//      xmin = xaxis->GetBinLowEdge(first);
//      xmax = xaxis->GetBinUpEdge(last);
      xmin = xaxis->GetBinLowEdge(first) + 0.5 * xaxis->GetBinWidth(first);
      xmax = xaxis->GetBinUpEdge(last)   - 0.5 * xaxis->GetBinWidth(last);
      if (is2dim(hist)) {
         TAxis *yaxis = hist->GetYaxis();
         first = yaxis->GetFirst();
         last = yaxis->GetLast();
//         ymin = yaxis->GetBinLowEdge(first);
 //        ymax = yaxis->GetBinUpEdge(last);
         ymin = yaxis->GetBinLowEdge(first) + 0.5 * yaxis->GetBinWidth(first);
         ymax = yaxis->GetBinUpEdge(last)   - 0.5 * yaxis->GetBinWidth(last);
      }
   } else {
      cout << "Null pointer to hist" << endl;
      return -1;
   }
   fMarkers = (FhMarkerList*)hist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
      fMarkers = new  FhMarkerList();
      hist->GetListOfFunctions()->Add(fMarkers);
   }

   nval = fMarkers->GetEntries();
   if (nval == 1) {
      if (fSelInside)
         fMarkers->Add(new FhMarker(xmin, ymin, 30));
      else
         fMarkers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   if (nval == 0) {
      fMarkers->Add(new FhMarker(xmin, ymin, 30));
      fMarkers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   fMarkers->Sort();
   return nval;
};
//_______________________________________________________________________________________

void FitHist::PrintMarks()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
      cout << "--- Current fMarkers --------------" << endl;
      FhMarker *ti;
      TIter next(fMarkers);
      while ( (ti = (FhMarker *) next()) ) {
         cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
      }
   } else {
      cout << "--- No marks set ---" << endl;
   }
};
//_______________________________________________________________________________________

void FitHist::PaintMarks()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
//      cout << "--- Current fMarkers --------------" << endl;
      FhMarker *ti;
      TIter next(fMarkers);
      cHist->cd();
      while ( (ti = (FhMarker *) next()) ) {
         ti->Draw();
      }
      cHist->Modified(kTRUE);
      cHist->Update();
   }
};

//____________________________________________________________________________________

Bool_t FitHist::IsThereAnyHist()
{
   TList *tl = gDirectory->GetList();
   TIter next(tl);
   while (TObject * obj = next()) {
      if (is_a_hist(obj))
         return kTRUE;
   }
   return kFALSE;
}

//____________________________________________________________________________________

TH1 *FitHist::GetOneHist()
{
   Int_t nhists = 0;
   TOrdCollection *entries = new TOrdCollection();
   TH1 * hist = NULL;
   TList *tl = gDirectory->GetList();
   TIter next(tl);
   while (TObject * obj = next()) {
      if (is_a_hist(obj)) {
         hist = (TH1 *) obj;
         if (hist != fSelHist) {
            entries->Add(new TObjString(hist->GetName()));
            nhists++;
         }
      }
   }
   if (nhists <= 0) {
      Hpr::WarnBox("No Histogram found");
      return 0;
   }

   TArrayI flags(nhists);
   flags.Reset();
   Int_t retval = 0;

   Int_t itemwidth = 240;
   new TGMrbTableFrame(mycanvas,
   						  &retval,
   						  "Select a histogram",
   						  itemwidth, 1, nhists,
   						  entries, 0, 0, &flags,
   						  nhists);
   if (retval < 0){
      return NULL;
   }
   TObjString * objs;
   TString s;
   for (Int_t i = 0; i < nhists; i++) {
//      Char_t sel = selected[i];
      if (flags[i] == 1) {
         objs = (TObjString *) entries->At(i);
         s = objs->String();
         hist = (TH1 *) gROOT->FindObject((const char *) s);
         break;
      }
   }
   delete entries;
   if (!hist) {
      Hpr::WarnBox("No Histogram found");
   }
   if (hist == fSelHist) {
      Hpr::WarnBox("Cant use same histogram");
      hist = NULL;
   }
   return hist;
}

//____________________________________________________________________________________

void FitHist::GetRange()
{
   TH1 *hist = GetOneHist();
   if (!hist)
      return;

   fSelHist->GetXaxis()->SetRange(hist->GetXaxis()->GetFirst(),
                                  hist->GetXaxis()->GetLast());
   if ( fSelHist->GetDimension() > 1 )
      fSelHist->GetYaxis()->SetRange(hist->GetYaxis()->GetFirst(),
                                     hist->GetYaxis()->GetLast());
   if ( fSelHist->GetDimension() > 2 )
      fSelHist->GetZaxis()->SetRange(hist->GetZaxis()->GetFirst(),
                                     hist->GetZaxis()->GetLast());
   cHist->Modified();
   cHist->Update();
}

//____________________________________________________________________________________

void FitHist::KolmogorovTest()
{
   TH1 *hist;
   if (hp->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
      if (hp->GetSelectedHist()->GetSize() > 1) {
         Hpr::WarnBox("More than 1 selection");
         return;
      }
      hist = hp->GetSelHistAt(0);
   } else
      hist = GetOneHist();      // look in memory
   if (hist) {
      fSelHist->KolmogorovTest(hist, "D");
   }
}
//____________________________________________________________________________________

void FitHist::Superimpose(Int_t mode)
{
   TH1 *hist;
   TPaveLabel *tname;
//  choose from histo list
//   Int_t nh = hp->GetSelectedHist()->GetSize();
   if (hp->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
      if (hp->GetSelectedHist()->GetSize() > 1) {
         Hpr::WarnBox("More than 1 selection,\n please choose only one");
         return;
      }
      hist = hp->GetSelHistAt(0);
   } else {
      hist = GetOneHist();      // look in memory
   }
   if ( !hist ) {
      Hpr::WarnBox("No hist selected");
      return;
   }
	if (hist->GetDimension() != fSelHist->GetDimension()) {
		Hpr::WarnBox("Dimensions of histograms differ");
		return;
	}
   if ( hist->GetDimension() == 3 ) {
      if ( hist->GetMarkerColor() <= 1 ) {
      	fColSuperimpose += 1;
       	if ( fColSuperimpose > 7 )
		      fColSuperimpose = 2;
         hist->SetMarkerColor(fColSuperimpose);
      }
   } else if ( hist->GetDimension() == 1 ) {
      if ( hist->GetLineColor() <= 1 ) {
      	fColSuperimpose += 1;
       	if (fColSuperimpose > 7)
		      fColSuperimpose = 2;
      }
   }

	TGaxis *naxis = 0;
	TH1 *hdisp = hist;
	if (mode == 1 && !is2dim(fSelHist)) {	// scale
		cout << "!!!!!!!!!!!!!!!!" << endl;
		Stat_t maxy = 0;
		if (fSelHist->GetXaxis()->GetNbins() != hist->GetXaxis()->GetNbins()) {
//  case expanded histogram
			Axis_t x, xmin, xmax;
			xmin = fSelHist->GetXaxis()->GetXmin();
			xmax = fSelHist->GetXaxis()->GetXmax();
			for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++) {
				x = hist->GetBinCenter(i);
				if (x >= xmin && x < xmax && hist->GetBinContent(i) > maxy)
					maxy = hist->GetBinContent(i);
			}
		} else {
//  case zoomed histogram
			for (Int_t i = fSelHist->GetXaxis()->GetFirst();
					i <= fSelHist->GetXaxis()->GetLast();
					i++) {
				if (hist->GetBinContent(i) > maxy)
					maxy = hist->GetBinContent(i);
			}

		}
		if (fSelHist->GetMaximum() > 0 && maxy > 0) {
			hdisp = (TH1 *) hist->Clone();
			TString name = hdisp->GetName();
			name += "_scaled";
			hdisp->SetName(name.Data());
			Float_t sf = fSelHist->GetMaximum() / maxy;
			cout << "Scale " << hdisp->GetName() << " by " << sf << endl;
			hdisp->Scale(sf);
			for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
				hdisp->SetBinError(i, sf * hist->GetBinError(i));
			}
			cout << "Superimpose: Scale errors linearly" << endl;
			naxis = new TGaxis(fSelHist->GetXaxis()->GetXmax(),
									fSelHist->GetYaxis()->GetXmin(),
									fSelHist->GetXaxis()->GetXmax(),
									fSelHist->GetMaximum(),
									hist->GetYaxis()->GetXmin(), maxy, 510,
									"+");
			naxis->SetLabelOffset(0.05);
		}
	}
	cHist->cd();
   TString drawopt = fSelHist->GetDrawOption();
   if ( hist->GetDimension() == 1 ) {
	   hdisp->SetLineColor(fColSuperimpose);
	   if (!drawopt.Contains("E", TString::kIgnoreCase))
		   drawopt += "hist";
   }
	drawopt += "SAME";
//   cout << "drawopt " << drawopt << endl;
	hdisp->DrawCopy(drawopt.Data());
	Float_t x1 = 0.2;
	Float_t x2 = 0.3;
	Float_t y1 = 1 - 0.05 * fColSuperimpose;
	Float_t y2 = y1 + 0.05;

	tname = new TPaveLabel(x1, y1, x2, y2, hdisp->GetName(), "NDC");
	tname->SetFillColor(0);
	tname->SetTextColor(fColSuperimpose);
	tname->Draw();

	if (naxis) {
		naxis->Draw();
		naxis->SetLineColor(fColSuperimpose);
		naxis->SetTextColor(fColSuperimpose);
	}
	cHist->Update();
}

//____________________________________________________________________________________
//  find limits

void FitHist::GetLimits()
{
   Int_t inp;
   fBinX_1 = 0;
   fBinY_1 = 0;
   fBinX_2 = 0;
   fBinY_2 = 0;

   FhMarker *p;
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) return;
   TIter next(fMarkers);
//   fMarkers->Sort();
   while ( (p = (FhMarker *) next()) ) {
      inp = fSelHist->GetXaxis()->FindBin(p->GetX());
//      inp = XBinNumber(fSelHist, p->GetX());
      if (inp > 0) {
//         cout << "p->GetX() " << p->GetX()<<  " xbin " << inp << endl;
         if (fBinX_1 <= 0) {
            fBinX_1 = inp;
            fX_1 = p->GetX();
         };
         if (fBinX_1 > 0 && inp < fBinX_1) {
            fBinX_1 = inp;
            fX_1 = p->GetX();
         }
         if (inp > fBinX_2) {
            fBinX_2 = inp;
            fX_2 = p->GetX();
         }
      }
      if (is2dim(fSelHist)) inp = fSelHist->GetYaxis()->FindBin(p->GetY());
      else                  inp = 1;

//      inp = YBinNumber(fSelHist, p->GetY());
//      inp = (Int_t)fYbins->At(i);
      if (inp > 0) {
//         cout << "ybin " << inp << endl;
         if (fBinY_1 <= 0) {
            fBinY_1 = inp;
            fY_1 = p->GetY();
         }
         if (fBinY_1 > 0 && inp < fBinY_1) {
            fBinY_1 = inp;
            fY_1 = p->GetY();
         }
         if (inp > fBinY_2) {
            fBinY_2 = inp;
            fY_2 = p->GetY();
         }
      }
   }
   if (fBinX_1 > fBinX_2) {
      int bin = fBinX_1;
      fBinX_1 = fBinX_2;
      fBinX_2 = bin;
      Axis_t val = fX_1;
      fX_1 = fX_2;
      fX_2 = val;
   }
   if (fBinY_1 > fBinY_2) {
      int bin = fBinY_1;
      fBinY_1 = fBinY_2;
      fBinY_2 = bin;
      Axis_t val = fY_1;
      fY_1 = fY_2;
      fY_2 = val;
   }
}

//____________________________________________________________________________________
//

void FitHist::OutputStat()
{
//   enum dowhat {expand, projectx, projecty, statonly};

//   ExpandProject(statonly);
   cout <<
       "============================================================="
       << endl;

   if (!fSelPad) {
      cout << "Cant find pad, call Debugger" << endl;
      return;
   }
   if (GetMarks(fSelHist) <= 0 && !Nwindows()) {
      cout << "No marks, no windows" << endl;
      return;
   }
//  1-dim case
   if (!is2dim(fSelHist)) {
      Double_t x = 0, cont = 0;
      Double_t sum   = 0;
      Double_t sumx  = 0;
      Double_t sumx2 = 0;
      Double_t mean  = 0;
      Double_t sigma = 0;
      Float_t xlow, xup;
      if (Nwindows() == 0) {
         if (GetMarks(fSelHist) != 2) {
            Hpr::WarnBox("Please select 1 window or exactly 2 marks");
            return;
         }
         GetLimits();
         xlow = fX_1;
         xup = fX_2;
         cout << "Using marks " << endl;
      } else {
         if (Nwindows() > 1) {
            Hpr::WarnBox("Please select one window");
            return;
         }
         TMrbWindow *wdw = (TMrbWindow *) fActiveWindows->First();
         xlow = wdw->GetX1();
         xup = wdw->GetX2();
         cout << "Using window " << wdw->GetName() << endl;
      }
      for (Int_t i = 1; i <= fSelHist->GetNbinsX(); i++) {
         x = fSelHist->GetBinCenter(i);
         if (x >= xlow && x < xup) {
            cont = fSelHist->GetBinContent(i);
            sum += cont;
            sumx += x * cont;
            sumx2 += x * x * cont;
         }
      }
      if (sum > 0.) {
         mean = sumx / sum;
         sigma = sqrt(sumx2 / sum - mean * mean);
      }
      cout << "Statistics for Histogram:" << fSelHist->GetName()
          << " from " << xlow << " to " << xup << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
      cout << "Content    " << sum << endl;
      cout << "Mean       " << mean << endl;
      cout << "Sigma      " << sigma << endl;
      cout << "2.35*Sigma " << 2.35 * sigma << endl;	// 2*sqrt(2ln2)
      cout <<
          "-------------------------------------------------------------"
          << endl;

      TOrdCollection *row_lab = new TOrdCollection();
      row_lab->Add(new TObjString("Content"));
      row_lab->Add(new TObjString("Mean"));
      row_lab->Add(new TObjString("Sigma"));
      row_lab->Add(new TObjString("2.35*Sigma"));

      TOrdCollection *col_lab = new TOrdCollection();
      col_lab->Add(new TObjString((char *) fOrigHist->GetName()));
      TArrayD par(4);
//      double par[4];
      par[0] = sum;
      par[1] = mean;
      par[2] = sigma;
      par[3] = 2.35 * sigma;
      TString title("Statistics in window: ");
      title += Form("%f",xlow);
      title += " to ";
      title += Form("%f",xup);
      Int_t ret = 0, ncols = 1, nrows = 4, itemwidth = 240, precission = 5;
      TGMrbTableOfDoubles(mycanvas, &ret, title.Data(), itemwidth,
                          ncols, nrows, par, precission, col_lab, row_lab);
      if (col_lab)
         col_lab->Delete();
      if (row_lab)
         row_lab->Delete();
      if (col_lab)
         delete col_lab;
      if (row_lab)
         delete row_lab;
      return;
   } else {
      GetLimits();
      double sum = 0;
      UpdateCut();
//      CheckList(fActiveCuts);
      int nc = Ncuts();
      TAxis *xaxis = fSelHist->GetXaxis();
      TAxis *yaxis = fSelHist->GetYaxis();
      for (int i = 1; i <= fSelHist->GetNbinsX(); i++) {
         Axis_t xcent = xaxis->GetBinCenter(i);
//         if (xcent >= fX_1 && xcent < fX_2) {
            for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
               Axis_t ycent = yaxis->GetBinCenter(j);
//               if (ycent >= fY_1 && ycent < fY_2) {
                  if (nc <= 0 ||
                      (nc > 0 && InsideCut((float) xcent, (float) ycent)))
                     sum += fSelHist->GetCellContent(i, j);
//               }
            }
//         }
      }
      cout << "Statistics for 2 dim Histogram:" << fSelHist->
          GetName() << endl;

//      cout << " X from " << fX_1 << " to " << fX_2 << endl;
//      cout << " Y from " << fY_1 << " to " << fY_2 << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
      if (nc) {
         cout << "Cuts applied: " << endl;
         TIter next(fActiveCuts);
         while (TMrbWindow2D * cut = (TMrbWindow2D *) next())
            PrintOneCut(cut);
         cout <<
             "-------------------------------------------------------------"
             << endl;
      } else {
         cout << "No cuts applied!!!" << endl;
      }
      cout << "Content ";
      if (fSelInside) cout << "inside: ";
      else            cout << "outside: ";
      cout << sum << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
   }
}

//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::Expand()
{
//   enum dowhat {expand, projectx, projecty, statonly};
   ExpandProject(expand);
   SaveDefaults(kFALSE);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectBoth()
{
//   enum dowhat {expand, projectx, projecty, statonly,projectboth};
   ExpandProject(projectboth);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectX()
{
//   enum dowhat {expand, projectx, projecty, statonly};
   ExpandProject(projectx);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectY()
{
   ExpandProject(projecty);
}

//____________________________________________________________________________________

// Show projection of histograms along a function

void FitHist::ProjectF()
{
   ExpandProject(projectf);
}

//____________________________________________________________________________________

// Fast Fourier Transform

void FitHist::FastFT()
{
#if ROOTVERSION < 51100
   cout << "FFT not yet available" << endl;
#else
   const char helpText[] =
" This function allows to do discrete Fourier transforms of TH1 and TH2.\n\
 Available transform types and flags are described below.\n\
\n\
 To extract more information about the transform, use the function\n\
  TVirtualFFT::GetCurrentTransform() to get a pointer to the current\n\
  transform object.\n\
\n\
 Parameters:\n\
  1st - histogram for the output. If a null pointer is passed, a new histogram is created\n\
  and returned, otherwise, the provided histogram is used and should be big enough\n\
\n\
  Options: option parameters consists of 3 parts:\n\
    - option on what to return\n\
   \"RE\" - returns a histogram of the real part of the output\n\
   \"IM\" - returns a histogram of the imaginary part of the output\n\
   \"MAG\"- returns a histogram of the magnitude of the output\n\
   \"PH\" - returns a histogram of the phase of the output\n\
\n\
    - option of transform type\n\
   \"R2C\"  - real to complex transforms - default\n\
   \"R2HC\" - real to halfcomplex (special format of storing output data,\n\
          results the same as for R2C)\n\
   \"DHT\" - discrete Hartley transform\n\
         real to real transforms (sine and cosine):\n\
   \"R2R_0\", \"R2R_1\", \"R2R_2\", \"R2R_3\" - discrete cosine transforms of types I-IV\n\
   \"R2R_4\", \"R2R_5\", \"R2R_6\", \"R2R_7\" - discrete sine transforms of types I-IV\n\
    To specify the type of each dimension of a 2-dimensional real to real\n\
    transform, use options of form \"R2R_XX\", for example, \"R2R_02\" for a transform,\n\
    which is of type \"R2R_0\" in 1st dimension and  \"R2R_2\" in the 2nd.\n\
\n\
    - option of transform flag\n\
    \"ES\" (from \"estimate\") - no time in preparing the transform, but probably sub-optimal\n\
       performance\n\
    \"M\" (from \"measure\")   - some time spend in finding the optimal way to do the transform\n\
    \"P\" (from \"patient\")   - more time spend in finding the optimal way to do the transform\n\
    \"EX\" (from \"exhaustive\") - the most optimal way is found\n\
     This option should be chosen depending on how many transforms of the same size and\n\
     type are going to be done. Planning is only done once, for the first transform of this\n\
     size and type. Default is \"ES\".\n\
";

   static Int_t mag = 1, re = 0 , img = 0 , ph = 0,
                r2c = 1, dht = 0;
   static TString opt_r2r("R2R_0");

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("CheckButton_Magnitude"));
   row_lab->Add(new TObjString("CheckButton_Real Part"));
   row_lab->Add(new TObjString("CheckButton_Imaginary Part"));
   row_lab->Add(new TObjString("CheckButton_Phase"));
   row_lab->Add(new TObjString("RadioButton_TType: Real to Complex"));
   row_lab->Add(new TObjString("RadioButton_TType: Discrete Hartley"));
//   row_lab->Add(new TObjString("RadioButton_TType: Real to Real"));
//   row_lab->Add(new TObjString("StringValue_Option for Real to Real"));

   valp[ind++] = &mag;
   valp[ind++] = &re;
   valp[ind++] = &img;
   valp[ind++] = &ph;
   valp[ind++] = &r2c;
   valp[ind++] = &dht;
//   valp[ind++] = &r2r;
//   valp[ind++] = &opt_r2r;

   Int_t itemwidth = 320;
   ok = GetStringExt("FFT Parameters", NULL, itemwidth, mycanvas,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0]);

   TString option;
   TString name;
   TString title;

   if (r2c) option = "R2C";
   if (dht) option = "DHT";
//   if (r2r) option = opt_r2r;
   if (mag != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "MAG";
      title = fSelHist->GetTitle();
      title += "-Magnitude of FFT, ";
      title += option;
      option += " MAG";
      cout << "Option: " << option << endl;
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (hp) hp->ShowHist(hresult);
   }
   if (re != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "RE";
      title = fSelHist->GetTitle();
      title += "-Real part of FFT, ";
      title += option;
      option += " RE";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (hp) hp->ShowHist(hresult);
   }
   if (img != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "IM";
      title = fSelHist->GetTitle();
      title += "-Imaginary part of FFT, ";
      title += option;
      option += " IM";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (hp) hp->ShowHist(hresult);
   }
   if (ph != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "PH";
      title = fSelHist->GetTitle();
      title += "-Phase of FFT, ";
      title += option;
      option += " PH";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (hp) hp->ShowHist(hresult);
   }
#endif
}

//____________________________________________________________________________________

// Rotate 2dim histograms

void FitHist::Rotate(Int_t sense)
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be rotated");
      return;
   }
   Double_t tan_alpha = 0;
   Double_t alpha_deg = 0;
   TList * fl = fSelHist->GetListOfFunctions();
   if (fl->GetSize() > 0) {
      TIter next(fl);
      while (TObject * obj = next()){
         if(obj->IsA() == TF1::Class()) {
            TF1 * func = (TF1*)obj;
            if (func->GetNpar() == 2) {
               if (tan_alpha != 0)
                cout << setred << "More than 1 Pol1 defined, take last"
                     << setblack << endl;
              tan_alpha = func->GetParameter(1);
            }
         }
      }
   }
   if (tan_alpha == 0) {
      Bool_t ok;
      alpha_deg = (Double_t)GetFloat("Enter angle in degrees or 0 to define Pol1",
         0, &ok, mycanvas);
      if (!ok ||  alpha_deg == 0) {
         Hpr::WarnBox("Please fit a Pol1 to define rotation angle");
         return;
      }
      alpha_deg = TMath::Abs(alpha_deg);
      if (sense == 1) alpha_deg = - alpha_deg;
   } else {
      if (sense == 1) tan_alpha = - 1 / tan_alpha;
      alpha_deg = TMath::ATan(tan_alpha) * 180. / TMath::Pi();
   }
   TH2 * hrot = rotate_hist((TH2*)fSelHist, alpha_deg, fSerialRot);
   fSerialRot++;
   cHist->cd();
   if  (hp) hp->ShowHist(hrot);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Transpose 2dim histograms

void FitHist::Transpose()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be transposed");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_transp";
   TString htitle(h2->GetTitle());
   htitle += "_transp";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TH2F *h2_transp = new TH2F(hname, htitle,
                              ya->GetNbins(), ya->GetXmin(), ya->GetXmax(),
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   for (Int_t ix = 0; ix <= xa->GetNbins(); ix++) {
      for (Int_t iy = 0; iy <= ya->GetNbins(); iy++) {
         h2_transp->SetCellContent(iy, ix, h2->GetCellContent(ix, iy));
         h2_transp->SetCellError  (iy, ix, h2->GetCellError  (ix, iy));
      }
   }
   h2_transp->SetEntries(h2->GetEntries());
   h2_transp->GetXaxis()->SetTitle(h2->GetYaxis()->GetTitle());
   h2_transp->GetYaxis()->SetTitle(h2->GetXaxis()->GetTitle());
//   fSelHist = h2_transp;
   cHist->cd();
   if  (hp) hp->ShowHist(h2_transp);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileX()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be profiled");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_profX";
   hname += fSerialPx;
   fSerialPx++;
   TString htitle(h2->GetTitle());
   htitle += "_profX";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TMrbArrayD yslice(ya->GetNbins());
   yslice.SetBinWidth(ya->GetBinWidth(1));
   yslice.SetLowEdge(ya->GetXmin());
//   Int_t non_zero = 0;
   TH1D *h_prof = new TH1D(hname, htitle,
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   UpdateCut();
   Int_t nc = Ncuts();
   Double_t max_error = 0;
   for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
      yslice.Reset();
      for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
         if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
                              ya->GetBinCenter(iy)))
            continue;
         yslice[iy-1] = h2->GetCellContent(ix, iy);
      }
//      cout << yslice.GetMean() << endl;
      if (yslice.GetSum() != 0) {
         h_prof->SetBinContent(ix, yslice.GetMean());
         Double_t error = yslice.GetRMS() / TMath::Sqrt(yslice.GetSum());
         h_prof->SetBinError(ix, error);
         max_error = TMath::Max(error, max_error);
      }
   }
 //     cout << "max_error " << max_error << endl;
   for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
      if (h_prof->GetBinContent(ix) != 0 &&
          h_prof->GetBinError(ix) == 0)
            h_prof->SetBinError(ix, max_error);
   }
   h_prof->SetEntries(h2->GetEntries());
   cHist->cd();
   if  (hp) hp->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileY()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be profiled");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_profY";
   hname += fSerialPx;
   fSerialPx++;
   TString htitle(h2->GetTitle());
   htitle += "_profY";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TMrbArrayD xslice(xa->GetNbins());
   xslice.SetBinWidth(xa->GetBinWidth(1));
   xslice.SetLowEdge(xa->GetXmin());
//   Int_t non_zero = 0;
   TH1D *h_prof = new TH1D(hname, htitle,
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   UpdateCut();
   Int_t nc = Ncuts();
   Double_t max_error = 0;
   for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
      xslice.Reset();
      for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
         if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
                              ya->GetBinCenter(iy)))
            continue;
         xslice[ix-1] = h2->GetCellContent(ix, iy);
      }
//      cout << xslice.GetMean() << endl;
      if (xslice.GetSum() != 0) {
         h_prof->SetBinContent(iy, xslice.GetMean());
         Double_t error = xslice.GetRMS() / TMath::Sqrt(xslice.GetSum());
         h_prof->SetBinError(iy, error);
         max_error = TMath::Max(error, max_error);
      }
   }
//      cout << "max_error " << max_error << endl;
   for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
      if (h_prof->GetBinContent(ix) != 0 &&
          h_prof->GetBinError(ix) == 0)
            h_prof->SetBinError(ix, max_error);
   }
   h_prof->SetEntries(h2->GetEntries());
   cHist->cd();
   if  (hp) hp->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::ExpandProject(Int_t what)
{
//   enum dowhat {expand, projectx, projecty, statonly, profilex, profiley};

//   cout << "enter ExpandProject(Int_t what)" << endl;
   if (!fSelPad) {
      cout << "Cant find pad, call Debugger" << endl;
   }

   if (GetMarks(fSelHist) <= 0) {
      cout << "No selection" << endl;
   }
   TString expname;
   GetLimits();
//   cout << "fBinX_1,2 " << fBinX_1 << " " << fBinX_2 << endl;
//   cout << "fBinY_1,2 " << fBinY_1 << " " << fBinY_2 << endl;
//  1-dim case
   if (!is2dim(fSelHist)) {
      Double_t x = 0, cont = 0;
      Double_t sum = 0;
      Double_t sumx = 0;
      Double_t sumx2 = 0;
      Double_t mean;
      Double_t sigma;
//     get values in expanded hist
      TAxis *xaxis_new = fSelHist->GetXaxis();
      Axis_t bw = xaxis_new->GetBinWidth(fBinX_1);
      fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
      fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + bw;

      fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * bw);
      fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * bw);
      Int_t NbinX = fBinux - fBinlx + 1;
//      cout << "binlx,ux " << binlx << " " << binux << endl;

      if (what == expand) {
         if (expHist)
            expHist->GetListOfFunctions()->Clear();
         expname = fEname;
         expname += fExpInd;
         fExpInd++;
         expHist = new TH1D(expname, fOrigHist->GetTitle(),
                            NbinX, fExplx, fExpux);
         expHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         expHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
      }
      for (Int_t i = 0; i <= fOrigHist->GetNbinsX() + 1; i++) {
         cont = fOrigHist->GetBinContent(i);
         x = fOrigHist->GetBinCenter(i);
         if (i >= fBinlx && i <= fBinux) {
            sum += cont;
            sumx += x * cont;
            sumx2 += x * x * cont;
         }
         if (what == expand) {
            Int_t newbin = i - fBinlx + 1;
            if (fOrigHist->GetSumw2N()) {
               expHist->Fill(x, cont);
            } else {
               expHist->SetBinContent(newbin, cont);
            }
            if (i >= fBinlx && i <= fBinux && fOrigHist->GetSumw2N())
               expHist->SetBinError(newbin, fOrigHist->GetBinError(i));
         }
      }
      if (sum > 0.) {
         mean = sumx / sum;
         sigma = sqrt(sumx2 / sum - mean * mean);
      }
      if (expHist)
         expHist->SetEntries(Int_t(sum));

      if (what == statonly) {
         return;
      }
   } else {

//  2-dim case
      Double_t sum = 0;
//      Double_t x = 0;
      TF1 * func = NULL;
      if (what == projectf) {
         TIter next(fSelHist->GetListOfFunctions());
         while (TObject * obj =(TObject *)next()) {
            if (is_a_function(obj)) {
               if (func != NULL)
                  Hpr::WarnBox("More than 1 function defined, take first");
                else func =(TF1 *)obj;
            }
         }
         if (!func) {
            Hpr::WarnBox("No function found");
            return;
         }
      }
      TAxis *xaxis = fOrigHist->GetXaxis();
      TAxis *yaxis = fOrigHist->GetYaxis();
//     get values in expanded hist
      TAxis *xaxis_new = fSelHist->GetXaxis();
      fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
      Axis_t xbw = xaxis_new->GetBinWidth(fBinX_1);
      fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + xbw;

      fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * xbw);
      fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * xbw);
      Int_t NbinX = fBinux - fBinlx + 1;

      TAxis *yaxis_new = fSelHist->GetYaxis();
      fExply = yaxis_new->GetBinLowEdge(fBinY_1);
      Axis_t ybw = yaxis_new->GetBinWidth(fBinY_1);
      fExpuy = yaxis_new->GetBinLowEdge(fBinY_2) + ybw;
      fBinly = YBinNumber(fOrigHist, fExply + 0.5 * ybw);
      fBinuy = YBinNumber(fOrigHist, fExpuy - 0.5 * ybw);
      Int_t NbinY = fBinuy - fBinly + 1;
//      cout << "fExplx, ux, ly, uy "
//       <<fExplx << " "  << fExpux<< " "  << fExply<< " "  << fExpuy<< endl;
//      cout << "fBinlx, ux, ly, uy "
//       <<fBinlx << " "  << fBinux<< " "  << fBinly<< " "  << fBinuy<< endl;
      Int_t nperbinX = 1, nperbinY = 1;
      TString pname(fOrigHist->GetName());
      if (what == projectx ||what == projectboth) {
         pname += "_ProjX";
         pname += fSerialPx;

         projHistX = new TH1F(pname, fOrigHist->GetTitle(),
                              NbinX, fExplx, fExpux);
         fSerialPx++;
//         fSelHist=projHist;
      }
      if (what == projecty || what == projectf || what == projectboth) {
         Axis_t low = fExply;
         Axis_t up = fExpuy;
         if (what == projectf) {
            up = 0.5 * (fExpuy - fExply);
            low = -up;
            pname += "_Pfunc";
            pname += fSerialPf;
            fSerialPf++;
         } else {
            pname += "_ProjY";
            pname += fSerialPf;
            fSerialPy++;
         }
         projHistY = new TH1F(pname, fOrigHist->GetTitle(),
                              NbinY, low, up);
      }
      if (what == expand) {
         Int_t MaxBin2dim = 1000;
         nperbinX = (NbinX - 1) / MaxBin2dim + 1;
         Int_t NbinXrebin = (NbinX - 1) / nperbinX + 1;
         nperbinY = (NbinY - 1) / MaxBin2dim + 1;
         Int_t NbinYrebin = (NbinY - 1) / nperbinY + 1;
         if (expHist)
            expHist->GetListOfFunctions()->Clear();
         expname = fEname;
         expname += fExpInd;
         fExpInd++;
         expHist = new TH2F(expname, fOrigHist->GetTitle(),
                            NbinXrebin, fExplx, fExpux, NbinYrebin, fExply,
                            fExpuy);
         expHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         expHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
         fSelHist = expHist;
      }
      TAxis *xa = fSelHist->GetXaxis();
      TAxis *ya = fSelHist->GetYaxis();
//     is there a cut active
      Axis_t xcent;
      Axis_t ycent;
      UpdateCut();
      int nc = Ncuts();
      for (Int_t i = 0; i <= fOrigHist->GetNbinsX() + 1; i++) {
         for (Int_t j = 0; j <= fOrigHist->GetNbinsY() + 1; j++) {
            xcent = xaxis->GetBinCenter(i);
            ycent = yaxis->GetBinCenter(j);
            if (nc && !InsideCut((float) xcent, (float) ycent))
               continue;
            Stat_t cont = fOrigHist->GetCellContent(i, j);
            if (what == projectx || what == projectboth) {
               if (j >= fBinly && j <= fBinuy)
                  projHistX->Fill(xcent, cont);
            }
            if (what == projecty || what == projectboth) {
               if (i >= fBinlx && i <= fBinux)
                  projHistY->Fill(ycent, cont);
            }
            if (what == projectf) {
               if (i >= fBinlx && i <= fBinux) {
                  ycent -= func->Eval(xcent);
                  projHistY->Fill(ycent, cont);
               }
            }
            if (what == expand) {
               Int_t irebin = Int_t((i - fBinlx) / nperbinX + 1);
               if (irebin < 0)
                  irebin = 0;
               if (irebin > fSelHist->GetNbinsX() + 1)
                  irebin = fSelHist->GetNbinsX() + 1;
               Int_t jrebin = Int_t((j - fBinly) / nperbinY + 1);
               if (jrebin < 0)
                  jrebin = 0;
               if (jrebin > fSelHist->GetNbinsY() + 1)
                  jrebin = fSelHist->GetNbinsY() + 1;
               Stat_t oldcont = fSelHist->GetCellContent(irebin, jrebin);
//                 cout << i << " " << j << " "<< irebin << " "<< jrebin << endl;
//                 cout << xcent << " " << ycent << " " << oldcont << " " << cont<< endl;
               oldcont += cont;
               Axis_t xcent = xa->GetBinCenter(irebin);
               Axis_t ycent = ya->GetBinCenter(jrebin);

               fSelHist->SetCellContent(irebin, jrebin, 0);
               TH2 *ph2 = (TH2 *) fSelHist;
               ph2->Fill(xcent, ycent, oldcont);
            }
            if (i >= fBinlx && i <= fBinux && j >= fBinly && j <= fBinuy)
               sum += cont;
         }
      }
      if (what == statonly) {
         return;
      }
      ClearMarks();
      fSelPad->cd();
      if (what == projectboth) {
         Double_t bothratio = WindowSizeDialog::fProjectBothRatio;

         if (bothratio > 1 || bothratio <= 0)
            bothratio = 0.8;
         cHist->SetTopMargin(1 - bothratio);
         cHist->SetRightMargin(1 - bothratio);
         Double_t xmin = xa->GetBinLowEdge(xa->GetFirst());
         Double_t xmax = xa->GetBinUpEdge(xa->GetLast());
         Double_t dx = (xmax - xmin) / (Double_t) projHistX->GetNbinsX();
         Double_t ymin = ya->GetBinLowEdge(ya->GetFirst());
         Double_t ymax = ya->GetBinUpEdge(ya->GetLast());
         Double_t y0 = ymax + 0.005 * (ymax - ymin);
         Double_t y1 = ymin + 0.9 * (ymax - ymin) / bothratio;
//         cout << "ymax " << ymax << " ymin " << ymin<< " y1 " << y1<< endl;
         Double_t maxcont = -10000000;
         Int_t nbins = projHistX->GetNbinsX();
         for (Int_t i = 1; i <= nbins; i++) {
            if (projHistX->GetBinContent(i) > maxcont)
               maxcont = projHistX->GetBinContent(i);
         }
         if (maxcont == 0)
            return;
         Double_t yfac = (y1 - y0) / maxcont;
         Int_t np = 0;
         Double_t x = xmin;
         Double_t y = y0;
         TPolyLine *lpx = new TPolyLine(nbins * 2 + 3);
         for (Int_t i = 1; i <= nbins; i++) {
            lpx->SetPoint(np, x, y);
            np++;
            y = y0 + yfac * projHistX->GetBinContent(i);
            lpx->SetPoint(np, x, y);
            np++;
            x += dx;
         }
         lpx->SetPoint(np, x, y);
         np++;
         lpx->SetPoint(np, x, y0);
         np++;
         lpx->SetPoint(np, xmin, y0);
         cout << "np " << np << endl;
         lpx->Draw("F");
         lpx->SetFillStyle(1001);
         lpx->SetFillColor(38);
         lpx->Draw();
         TGaxis *naxis =
             new TGaxis(xmin + dx * nbins, y0, xmin + dx * nbins, y1,
                        0, maxcont, 404, "-");
         naxis->SetLabelOffset(0.01);
         naxis->SetLabelSize(0.02);
         naxis->Draw();

//         xmin = xa->GetXmin();
//         xmax = xa->GetXmax();
//         ymin = ya->GetXmin();
//         ymax = ya->GetXmax();
         Double_t dy = (ymax - ymin) / (Double_t) projHistY->GetNbinsX();
         Double_t x0 = xmax + 0.005 * (xmax - xmin);
         Double_t x1 = xmin + 0.9 * (xmax - xmin) / bothratio;
         maxcont = -10000000;
         for (Int_t i = 1; i <= projHistY->GetNbinsX(); i++) {
            if (projHistY->GetBinContent(i) > maxcont)
               maxcont = projHistY->GetBinContent(i);
         }
         if (maxcont == 0)
            return;
         Double_t xfac = (x1 - x0) / maxcont;
         np = 0;
         x = x0;
         y = ymin;
         TPolyLine *lpy = new TPolyLine(projHistY->GetNbinsX() * 2 + 3);
         nbins = projHistY->GetNbinsX();
         for (Int_t i = 1; i <= nbins; i++) {
            lpy->SetPoint(np, x, y);
            np++;
            x = x0 + xfac * projHistY->GetBinContent(i);
            lpy->SetPoint(np, x, y);
            np++;
            y += dy;
         }
         lpy->SetPoint(np, x, y);
         np++;
         lpy->SetPoint(np, x0, y);
         np++;
         lpy->SetPoint(np, x0, ymin);
         cout << "np " << np << endl;
         lpy->Draw("F");
         lpy->SetFillStyle(1001);
         lpy->SetFillColor(45);
         lpy->Draw();
         naxis = new TGaxis(x0, ymin + nbins * dy, x1, ymin + nbins * dy,
                            0, maxcont, 404, "-");
         naxis->SetLabelOffset(0.01);
         naxis->SetLabelSize(0.02);
         naxis->Draw();
         cHist->Update();
         return;
      } else if (what == projectx) {
         hp->ShowHist(projHistX);
         return;
      } else if (what == projecty || what == projectf) {
         hp->ShowHist(projHistY);
         return;
      } else {
         fSelHist->SetEntries(sum);
      }
   }
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {
      TIter next(lof);
		TObject * p;
      while ( (p = next()) ) {
//            p->Print();
//  dont add TPaletteAxis
         if (strcmp(p->GetName(),"palette"))
            expHist->GetListOfFunctions()->Add(p);
      }
   }
   fSelHist = expHist;
   cHist->cd();
   if (is2dim(fSelHist))
      Draw2Dim();
   else if (is3dim(fSelHist))
      Draw3Dim();
   else
      Draw1Dim();

//   ClearMarks();
   cHist->Update();
}

//____________________________________________________________________________

void FitHist::ExecDefMacro()
{
   if (!gSystem->AccessPathName(attrname, kFileExists)) {
      gROOT->LoadMacro(attrname);
      TString cmd = attrname;
      Int_t p = cmd.Index(".");
      cmd.Remove(p, 2);
      cmd =
          cmd + "(\"" + fCname.Data() + "\",\"" + fSelHist->GetName() +
          "\")";
//      cout << cmd << endl;
      hp->SetCurrentHist(fSelHist);
      gROOT->ProcessLine((const char *) cmd);
   }
}

//____________________________________________________________________________
//
//void FitHist::DrawHist() {
 //   if (is2dim(fSelHist)) Draw2Dim();
//    else                  Draw1Dim();

//    delete this;
//};
//____________________________________________________________________________

void FitHist::Draw3Dim()
{
//   TString drawopt("iso");
   TString drawopt;
   fSelHist->Draw(drawopt);
}
//____________________________________________________________________________

void FitHist::Draw1Dim()
{
   TString drawopt;
   gROOT->ForceStyle();
   gStyle->SetOptTitle(fShowTitle);
   if ( fErrorMode != "none")
       drawopt = fErrorMode;
   if (fShowContour)
      drawopt += "hist";
//   if (fShowErrors)
//      drawopt += "e1";
   if (fFill1Dim && fSelHist->GetNbinsX() < 50000) {
      fSelHist->SetFillStyle(fHistFillStyle);
      fSelHist->SetFillColor(fHistFillColor);
   } else
      fSelHist->SetFillStyle(0);
   fSelHist->SetOption(drawopt.Data());
//   fSelHist->DrawCopy();
   fSelHist->Draw();
   if (fTitleCenterX)
      fSelHist->GetXaxis()->CenterTitle(kTRUE);
   if (fTitleCenterY)
      fSelHist->GetYaxis()->CenterTitle(kTRUE);
   cHist->GetFrame()->SetFillStyle(0);
   TList *lof = fOrigHist->GetListOfFunctions();

   Double_t ymax  = fSelHist->GetMaximum();
   Double_t ymove = 0.05 * ymax;
   Double_t y0    = 6 * ymove;
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
         if (p->InheritsFrom(TMrbWindow::Class())) {
            TMrbWindow *w = (TMrbWindow *) p;
            w->SetParent(fSelHist);
            if (w->GetY1() == 0) {
               w->SetY1(y0);
               w->SetY2(y0);
               y0 += ymove;
               if (y0 >= ymax) y0 = 3 * ymove;
            }
            w->Draw();
         }
      }
//      DrawWindows();
   }
   if (GeneralAttDialog::fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   DrawDate();
   fSelHist->SetStats(0);
   if (fShowStatBox) fSelHist->SetStats(1);
//  add extra axis (channels) at top
   if (fDrawAxisAtTop) {
      TPaveStats * st = (TPaveStats *)cHist->GetPrimitive("stats");
      if (st) {
         st->SetY1NDC(st->GetY1NDC()-0.15);
         st->SetY2NDC(st->GetY2NDC()-0.15);
      }
//      DrawTopAxis();

      TString cmd("((FitHist*)gROOT->FindObject(\"");
      cmd += GetName();
      cmd += "\"))->DrawTopAxis()";

      TExec * exec = new TExec("exec", cmd.Data());
      exec->Draw();

   }
   UpdateDrawOptions();
   cHist->Update();
}
//____________________________________________________________________________

void FitHist::DrawDate()
{
   if (fShowDateBox ) {
      TDatime dt;
      TString dtext("Drawn at: ");
      UInt_t da = fSelHist->GetUniqueID();
      if (da != 0 && !fUseTimeOfDisplay) {
         dt.Set(da);
         dtext = "Created at: ";
      }
      dtext += dt.AsSQLString();
      fDateText = new TText(gStyle->GetDateX(),gStyle->GetDateY(),dtext);
      fDateText->SetTextSize( gStyle->GetAttDate()->GetTextSize());
      fDateText->SetTextFont( gStyle->GetAttDate()->GetTextFont());
      fDateText->SetTextColor(gStyle->GetAttDate()->GetTextColor());
      fDateText->SetTextAlign(gStyle->GetAttDate()->GetTextAlign());
      fDateText->SetTextAngle(gStyle->GetAttDate()->GetTextAngle());
      fDateText->SetNDC();
      fDateText->Draw();
   }
}
//____________________________________________________________________________

void FitHist::Draw2Dim()
{
   cHist->cd();
//   SetLogz(cHist->GetLogz());
   SetLogz(fLogz);
//   gStyle->SetOptTitle(hp->GetShowTitle());
//   if (->GetShowTitle())
//      gStyle->SetTitleFont(hp->fTitleFont);
   cout << "FitHist::DrawOpt2Dim: " <<fDrawOpt2Dim << endl;
   fSelHist->DrawCopy(fDrawOpt2Dim);
   fSelHist->SetOption(fDrawOpt2Dim);
   fSelHist->SetDrawOption(fDrawOpt2Dim);
   fSelHist->SetStats(0);
   if (fTitleCenterX)
      fSelHist->GetXaxis()->CenterTitle(kTRUE);
   if (fTitleCenterY)
      fSelHist->GetYaxis()->CenterTitle(kTRUE);
   if (fTitleCenterY)
      fSelHist->GetYaxis()->CenterTitle(kTRUE);
   if (fShowStatBox) fSelHist->SetStats(1);
   if (gROOT->GetForceStyle()) {
      fSelHist->SetLineColor(gStyle->GetHistLineColor());
   }
   DrawCut();
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
//         p->Print();
         if (p->InheritsFrom(TF1::Class())) {
            TF1 *f =  dynamic_cast<TF1*>(p);
             f->Draw("same");
            f->SetParent(fSelHist);
         } else if (p->InheritsFrom(TMrbWindow2D::Class())) {
            TMrbWindow2D *w = dynamic_cast<TMrbWindow2D*>(p);
            w->Draw("same");
            w->SetParent(fSelHist);
            fActiveCuts->Add(w);
         } else if  (p->InheritsFrom(TMrbNamedArrayI::Class())) {
             TMrbNamedArrayI * nai = dynamic_cast<TMrbNamedArrayI*>(p);
             TString name(nai->GetName());
             if (name.BeginsWith("Pixel")) {
                cout << "SetUserPalette" << endl;
                SetUserPalette(1001, nai);
             }
         }
      }
   }
   if (GeneralAttDialog::fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   UpdateCanvas();
   if (f2DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         cHist->GetFrame()->SetFillStyle(0);
      } else {
      	cHist->GetFrame()->SetFillStyle(1001);
      	cHist->GetFrame()->SetFillColor(10);
   	}
   } else {
      cHist->GetFrame()->SetFillStyle(1001);
      cHist->GetFrame()->SetFillColor(f2DimBackgroundColor);
   }
   DrawDate();
   if (fLiveStat2Dim) {
       if (!cHist->GetAutoExec())
          cHist->ToggleAutoExec();
   } else  {
       if (cHist->GetAutoExec())
          cHist->ToggleAutoExec();
   }
//   cout << "FitHist::fDrawOpt2Dim: " <<fDrawOpt2Dim << endl;
 //  cout << "FitHist::Draw2Dim() AutoExec: ";
//   if (cHist->GetAutoExec()) cout << " on ";
//   else                      cout << "off ";
//   cout << endl;
   cHist->Update();
}

//__________________________________________________________________________________

void FitHist::AddFunctionsToHist()
{
   Int_t nval = CheckList(fAllFunctions);
   if (nval>0) {
      TF1 *fun;
      TIter next(fAllFunctions);
      while ( (fun = (TF1*)next()) ) {
         TF1 * funcp= new TF1(*fun);
         fSelHist->GetListOfFunctions()->Add(funcp);
         cHist->cd();
//         funcp->Draw("same");
         fun->Draw("same");
         cHist->Modified(kTRUE);
         cHist->Update();
      }
   } else Hpr::WarnBox("No function selected");
}
//______________________________________________________________________________________

void FitHist::SetLogz(Int_t state)
{
   fLogz = state;
   if (hp && hp->fLogScaleMin > 0){
      if(state > 0)
         fSelHist->SetMinimum(hp->fLogScaleMin);
      else
         fSelHist->SetMinimum(hp->fLinScaleMin);
   }
   cHist->SetLogz(state);
//   SaveDefaults();
}
//______________________________________________________________________________________

void FitHist::UpdateDrawOptions()
{
//   if (!hp) return;
   SetSelectedPad();
   TString drawopt;
   if (fDimension == 1) {
   	if (fShowContour) {
      	drawopt = "";
         if (!HasFunctions(fSelHist)) {
            if (fSelHist->GetSumw2N()) {
      	      drawopt = "hist";
            }
         }
      }
      if ( fErrorMode != "none") {
         drawopt += fErrorMode;
         if (fErrorMode == "E1" && gStyle->GetMarkerSize() == 0) {
            fSelHist->SetMarkerSize(0.01);
         }
      }
      if (fShowContour)
         drawopt += "hist";
   	if (fFill1Dim && fSelHist->GetNbinsX() < 50000) {
      	fSelHist->SetFillStyle(fHistFillStyle);
      	fSelHist->SetFillColor(fHistFillColor);
   	} else
      	fSelHist->SetFillStyle(0);
//      cout << "UpdateDrawOptions() " << drawopt.Data() << endl;
      fSelHist->SetOption(drawopt.Data());
      fSelHist->SetDrawOption(drawopt.Data());
   } else if (fDimension  == 2) {
      fSelHist->SetOption(fDrawOpt2Dim);
      fSelHist->SetDrawOption(fDrawOpt2Dim);
   }
   if (gROOT->GetForceStyle()) {
      SetHistOptDialog::SetDefaults();
   }
   gPad->Modified();
   gPad->Update();
}
//______________________________________________________________________________________

void FitHist::ColorMarked()
{
   if (fSelHist->GetDimension() != 1) {
      cout << "Currently only 1-dim supported" << endl;
      return;
   }
   GetLimits();
   Int_t color = getcol();
   if (color <= 0) return;
   TGraph * pl = PaintArea(fSelHist, fBinX_1, fBinX_2, color);
   if (pl) {
      cHist->cd();
      pl->Draw("LF");
      fSelHist->GetListOfFunctions()->Add(pl, "LF");
   }
   cHist->Modified();
   cHist->Update();
}
//____________________________________________________________________________________

void FitHist::Fit1DimDialog(Int_t type)
{
   if (fFit1DimD == NULL)
      fFit1DimD = new FitOneDimDialog(fSelHist, type);
}
//__________________________________________________________________

void FitHist::FindPeaks()
{
   new FindPeakDialog(fSelHist);
};

//__________________________________________________________________

void FitHist::Calibrate()
{
      new CalibrationDialog(fSelHist, 1);
};
