#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "TROOT.h"
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

#include "TGMsgBox.h"
#include "TGWidget.h"
#include "TGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitHist_Help.h"
#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FhMarker.h"
#include "FhContour.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbNamedArray.h"
#include "TMrbArrayD.h"

//extern HistPresent* hp;
extern TFile *fWorkfile;
extern const char *fWorkname;
extern Int_t nHists;
Int_t nPeaks;
Double_t gTailSide;             // in fit with tail determines side: 1 left(low), -1 high(right)
Float_t gBinW;

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
   fSetRange = kFALSE;
   fRangeLowX = 0;
   fRangeUpX = 0;
   fRangeLowY = 0;
   fRangeUpY = 0;
   fLinBgConst = fLinBgSlope = 0;
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
      gStyle->SetPalette(hp->fNofColorLevels, hp->fPalette);
 //     cout << "hp->fNofColorLevels " <<hp->fNofColorLevels 
 //           << " hp->fPalette" << hp->fPalette << endl;
   }
   markers = new TObjArray(0);
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

   TEnv env(".rootrc");         // inspect ROOT's environment
   fFitMacroName =
       env.GetValue("HistPresent.FitMacroName", "fit_user_function.C");
   
   RestoreDefaults();   
//   cout << "FitMacroName " << fFitMacroName.Data()<< endl;

   fTemplateMacro = "TwoGaus";
   fLiveStat1dim = env.GetValue("HistPresent.LiveStat1dim", 0);
   fLiveStat2dim = env.GetValue("HistPresent.LiveStat2dim", 0);
   fLiveGauss = env.GetValue("HistPresent.LiveGauss", 0);
   fLiveBG    = env.GetValue("HistPresent.LiveBG", 0);
   fFirstUse = 1;
   fSerialPx = 0;
   fSerialPy = 0;
   fSerialPf = 0;
   func_numb = 0;
   fCutNumber = 0;
   wdw_numb = 0;
//   fCallMinos = kFALSE;
//   fKeepParameters = kFALSE;
   fOldMode = -1;
   fColSuperimpose = 6;
   fMax = hist->GetMaximum();
   fMin = hist->GetMinimum();
   fXmin = hist->GetXaxis()->GetXmin();
   fXmax = hist->GetXaxis()->GetXmax();
   if (fDimension == 2) {
      fYmin = hist->GetYaxis()->GetXmin();
      fYmax = hist->GetYaxis()->GetXmax();
   }
   DisplayHist(hist, win_topx, win_topy, win_widx, win_widy);
   fLogx = cHist->GetLogx();
   fLogy = cHist->GetLogy();
   fLogz = cHist->GetLogz();
};

//________________________________________________________________

void FitHist::RecursiveRemove(TObject * obj)
{
//   cout << "RecursiveRemove " << obj <<  endl;
   fActiveCuts->Remove(obj);
   fActiveWindows->Remove(obj);
   fActiveFunctions->Remove(obj);
}

//------------------------------------------------------ 
// destructor
FitHist::~FitHist()
{
//    cout << " ~FitHist(): " << endl;
//   cout<< "enter FitHist  dtor "<< GetName()<<endl;
//   if(fMyTimer)fMyTimer->Delete();
//   fMyTimer=NULL;
   if (!expHist && hp && hp->fRememberZoom) SaveDefaults(kTRUE);
   gDirectory->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   if (hp) hp->fNwindows -= 1;
   if (expHist) {
//      cout << "expHist " << expHist->GetName() << endl;
//      dont delete possible windows
      expHist->GetListOfFunctions()->Clear("nodelete");
      gDirectory->GetList()->Remove(expHist);
      delete expHist;
      expHist = 0;
   }
   if (fCalFitHist) delete fCalFitHist;
   if (fTofLabels) { delete fTofLabels; fTofLabels=NULL;}
   if (fCalHist) delete fCalHist;
   if (fCalFunc) delete fCalFunc;
   if (fDateText) delete fDateText;
   if (!cHist || !cHist->TestBit(TObject::kNotDeleted) ||
       cHist->TestBit(0xf0000000)) {
      cout << "~FitHist: " << this << " Canvas : " << cHist << " is deleted" << endl;

      cHist = 0;
   }

   if (cHist) {
      cHist->ClearFitHist();
      if (fCanvasIsAlive) {
//         cout << " deleting " << cHist->GetName() << endl;
         delete cHist;
         cHist = 0;
      }
   }
   if (fCutPanel && fCutPanel->TestBit(TObject::kNotDeleted))
      delete fCutPanel;
   if (markers)
      delete markers;
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

   if (!hp->fRememberLastSet &&  !hp->fRememberZoom) return;
//   cout << "Enter SaveDefaults " << endl;

   TString defname("defaults/Last");
   TEnv env(".rootrc");         // inspect ROOT's environment
   env.SetValue("HistPresent.FitMacroName", fFitMacroName);

   Bool_t checkonly = kTRUE;
   if ( (!CreateDefaultsDir(mycanvas, checkonly)) ) return;
   defname = env.GetValue("HistPresent.LastSettingsName", defname.Data());

   defname += "_";
   defname += fHname;
	Int_t ip = defname.Index(";");
	if (ip > 0) defname.Resize(ip);
   defname += ".def";

   if (fDeleteCalFlag && !gSystem->AccessPathName(defname.Data())) {
      defname.Prepend("rm ");
      gSystem->Exec(defname.Data());
      cout << "Removing: " << defname.Data() << endl;
      return;
   } 

//   cout << "SaveDefaults in: "<< defname.Data()  << endl;
   ofstream wstream;
   wstream.open(defname, ios::out);
   if (!wstream.good()) {
      cerr << "SaveDefaults: " << gSystem->GetError() << " - " 
           << defname << endl;
      return;
   }
   wstream << "FitMacroName: " << fFitMacroName << endl;

   if (TCanvas * ca =
       (TCanvas *) gROOT->FindObject(GetCanvasName())) {
      wstream << "LogX:  " << ca->GetLogx() << endl;
      wstream << "LogY:  " << ca->GetLogy() << endl;
      wstream << "LogZ:  " << ca->GetLogz() << endl;
   } else {
      cerr << "Canvas deleted, cant find lin/log state" << endl;
   }
   if (recalculate && fSelHist->TestBit(TObject::kNotDeleted)) {
      wstream << "fBinlx:  " << fSelHist->GetXaxis()-> GetFirst() << endl;
//      cout  << "recalculate fBinlx:  " << fSelHist->GetXaxis()-> GetFirst() << endl;
      wstream << "fBinux:  " << fSelHist->GetXaxis()-> GetLast() << endl;
      if (fDimension == 2) {
         wstream << "fBinly:  " << fSelHist->GetYaxis()->
             GetFirst() << endl;
         wstream << "fBinuy:  " << fSelHist->GetYaxis()->
             GetLast() << endl;
      }
   } else {
//      cout << "take current fBinlx " << fBinlx  << endl;
      wstream << "fBinlx:  " << fBinlx << endl;
      wstream << "fBinux:  " << fBinux << endl;
      if (fDimension == 2) {
         wstream << "fBinly:  " << fBinly << endl;
         wstream << "fBinuy:  " << fBinuy << endl;
      }
   }
   //         check if hist is still alive
   if (fSelHist->TestBit(TObject::kNotDeleted)) {
      if (strlen(fSelHist->GetXaxis()->GetTitle()) > 0) 
         wstream << "fXtitle: " << fSelHist->GetXaxis()->GetTitle() << endl;
      if (strlen(fSelHist->GetYaxis()->GetTitle()) > 0)
         wstream << "fYtitle: " << fSelHist->GetYaxis()->GetTitle() << endl;
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
            wstream << "Contours: " << ncol << endl;
            for (Int_t i = 0; i < ncol; i++) {
               if (colors) pixval = (*colors)[i];
               else        pixval = 0;
               if (uc)     cval = uc[i];
               else        cval = 0;
               wstream << "Cont" << i << ": " << cval<< endl;
               wstream << "Pix"  << i << ": " << pixval << endl;
            } 
            if (uc) delete [] uc;     
         } 
      }
   	if (fSetRange) {
      	wstream << "fRangeLowX:  " << fSelHist->GetXaxis()->GetXmin() << endl;
      	wstream << "fRangeUpX:   " << fSelHist->GetXaxis()->GetXmax() << endl;
      	if (fDimension == 2) {
         	wstream << "fRangeLowY:  " << fSelHist->GetYaxis()->GetXmin() << endl;
         	wstream << "fRangeUpY:   " << fSelHist->GetYaxis()->GetXmax() << endl;
      	}
   	}
      if (fCalFunc && fCalHist) {
         wstream << "CalFuncName:  " << fCalFunc->GetName()  << endl;
         wstream << "CalFuncForm:  " << fCalFunc->GetTitle()  << endl;
         wstream << "CalFuncNpar:  " << fCalFunc->GetNpar()  << endl;
         for (Int_t i =0; i < fCalFunc->GetNpar(); i++) {
           wstream << "CalFuncPar" << i << ":   " << fCalFunc->GetParameter(i)  << endl;
         }
         wstream << "CalHistNbin:  " << fCalHist->GetNbinsX()  << endl;
         wstream << "CalHistXmin:  " << fCalHist->GetXaxis()->GetXmin()  << endl;
         wstream << "CalHistXmax:  " << fCalHist->GetXaxis()->GetXmax()  << endl;
      }
   }
   return;
}
//________________________________________________________________

void FitHist::RestoreDefaults()
{
   TEnv * lastset = GetDefaults(fHname);
	if (!lastset) return;
   fFitMacroName = lastset->GetValue("FitMacroName",fFitMacroName.Data());
   if (hp && (hp->fRememberLastSet || hp->fRememberZoom)) {
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

   if (gROOT->GetEditorMode() != 0) return;
   Int_t event = gPad->GetEvent();
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
//   if (gROOT->GetEditorMode() != 0) return;
   
//  check if lin / log scale changed
   if (hp && hp->fRememberZoom) {
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
   Int_t px = gPad->GetEventX();
   Int_t py = gPad->GetEventY();
   if (event == kButton1Down) {
      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(cHist, px, py))) {
         TList * lofp = gPad->GetListOfPrimitives();
         if (!lofp) return;
         TIter next(lofp);
         TObject * obj = 0; 
         while ( (obj = next()) ) {
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
             cout << " sopt " << sopt << endl;
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
         if (hp) {
            fLiveStat1dim = hp->fLiveStat1dim;
            fLiveStat2dim = hp->fLiveStat2dim;
            fLiveGauss = hp->fLiveGauss;
            fLiveBG = hp->fLiveBG;
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
               values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge )));
               values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge
                       + hist->GetBinWidth(startbinX))));
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
      if(!hist || !fTofLabels) return;
      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(cHist, px, py))){
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
            cont = hist->GetBinContent(binX, binY);
            binYlow = TMath::Min(startbinY, binY);
            binYup  = TMath::Max(startbinY, binY);
            sum = hist->Integral(binXlow, binXup, binYlow, binYup);
            Int_t totbins = (binYup - binYlow +1) * (binXup - binXlow +1);
            if (totbins > 0) mean = sum / (Double_t)totbins;
            else             mean = 0;
            YlowEdge = hist->GetYaxis()->GetBinLowEdge(binYlow);
            YupEdge  = hist->GetYaxis()->GetBinLowEdge(binYup) + hist->GetYaxis()->GetBinWidth(binYup);
 
            fTofLabels->SetLabelText(0,0,Form("%d,  %d,  %lg,  %lg", binXlow, binYlow, XlowEdge, XupEdge));     
            fTofLabels->SetLabelText(0,1,Form("%d,  %d,   %lg,  %lg", binXup,  binYup,  YlowEdge, YupEdge));  
            fTofLabels->SetLabelText(0,2,Form("%lg", cont));   
            fTofLabels->SetLabelText(0,3,Form("%lg,  %lg", sum, mean));
         } else {
            cont = hist->GetBinContent(binX);
            sum = hist->Integral(binXlow, binXup);
            Int_t totbins = (binXup - binXlow +1);
            if (totbins > 0) mean = sum / (Double_t)totbins;
            else             mean = 0;
            fTofLabels->SetLabelText(0,0,Form("%d,  %lg", binXlow, XlowEdge));   
            fTofLabels->SetLabelText(0,1,Form("%d,  %lg", binXup, XupEdge ));
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
               hist->Fit(gFitFunc, "RnlQ", "",XlowEdge ,XupEdge );
               first_fit = kTRUE;
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
            if (is2dim) {
               box = new TBox(XlowEdge, YlowEdge, XupEdge, YupEdge);
               box->SetLineColor(-1);
               box->SetFillStyle(0);
               box->Draw();
               ClearMarks();
               markers->Add(new FhMarker(XlowEdge, YlowEdge, 28));
               markers->Add(new FhMarker(XupEdge,  YupEdge, 28));
               PaintMarks();
            } else {
            	lowedge = new TLine(XlowEdge, hist->GetYaxis()->GetXmin(), 
                                   XlowEdge, hist->GetYaxis()->GetXmax());  
         	   upedge  = new TLine(XupEdge, hist->GetYaxis()->GetXmin(), 
                                   XupEdge, hist->GetYaxis()->GetXmax());
         	   lowedge->SetLineColor(4); lowedge->Draw();  
         	   upedge->SetLineColor(4); upedge->Draw(); 
               ClearMarks();
               markers->Add(new FhMarker(XlowEdge, yy, 28));
               markers->Add(new FhMarker(XupEdge,  yy, 28));
               PaintMarks();
            }
         }
         fTofLabels->RaiseWindow();
      }
   }
}
//_____________________________________________________________________________________

void FitHist::DrawTopAxis() {
//  add axis (channels) on top
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
   if (hp)
      hp->GetCanvasList()->Add(cHist);
   cHist->ToggleEventStatus();

   cHist->SetEditable(kTRUE);

   hist->SetDirectory(gROOT);
   if ( (is2dim(hist) && fLiveStat2dim) || (!is2dim(hist) && fLiveStat1dim) ){  
      TString cmd("((FitHist*)gROOT->FindObject(\""); 
      cmd += GetName();
      cmd += "\"))->handle_mouse()";
 //  cout << "cmd: " << cmd << endl;
      cHist->AddExec("handle_mouse", cmd.Data());
   }
   gDirectory = gROOT;
   fSelPad = cHist;
   fSelPad->cd();

   if (is3dim(hist)) {
      fSelPad->cd();
      Draw3Dim();
   } else if (is2dim(hist)) {
      fSelPad->cd();
      Draw2Dim();
      if (hp && hp->fAutoExec_2 ) {
   		TString cmd("((HistPresent*)gROOT->FindObject(\""); 
   		cmd += GetName();
   		cmd += "\"))->auto_exec_2()";
         cHist->AddExec("ex2", cmd.Data());
      }
   } else {
      fSelPad->cd();
      if (fLogy)
         cHist->SetLogy();
      Draw1Dim();
      if (hp && hp->fAutoExec_1 ) {
   		TString cmd("((HistPresent*)gROOT->FindObject(\""); 
   		cmd += GetName();
   		cmd += "\"))->auto_exec_1()";
         cHist->AddExec("ex1", cmd.Data());
      }
   }
   cHist->Update();
   cHist->GetFrame()->SetBit(TBox::kCannotMove);
//  look if there exists a calibrated version of this histogram
   TEnv *lastset = 0;
   if (hp && hp->fDisplayCalibrated && (lastset = GetDefaults(fHname)) ) {
      if (lastset->Lookup("CalFuncName")) {
         fCalFunc = new TF1(lastset->GetValue("CalFuncName", "xx"), lastset->GetValue("CalFuncForm", "xx"));
         Int_t npar = lastset->GetValue("CalFuncNpar", 2);
//          cout << "npar " << npar << endl;
         TString parname;
         for (Int_t i = 0; i < npar; i++) {
            parname = "CalFuncPar";
            parname += Form("%d", i);
            fCalFunc->SetParameter(i, (Axis_t)(lastset->GetValue(parname.Data(), 0.)));
//            cout << parname  << (Axis_t)(lastset->GetValue(parname.Data(), 0.)) << endl;
         }
         Int_t nbin_cal = lastset->GetValue("CalHistNbin", 0);
         Axis_t low_cal = lastset->GetValue("CalHistXmin", 0);
         Axis_t up_cal  = lastset->GetValue("CalHistXmax", 0);
         fCalHist = calhist(fSelHist,   fCalFunc, nbin_cal, 
                              low_cal, (up_cal - low_cal) / nbin_cal,(const char *)fHname);
         hp->ShowHist(fCalHist);
      }
      if (lastset) delete lastset;
   } 
};
  
//------------------------------------------------------ 
// Magnify

void FitHist::Magnify()
{
   if (is2dim(fSelHist)) {
      WarnBox("Magnify 2 dim not yet supported ");
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
      expHist->GetListOfFunctions()->Clear("nodelete");
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
   }
   cHist->Modified(kTRUE);
   cHist->Update();
   cHist->GetFrame()->SetBit(TBox::kCannotMove);
};
//_______________________________________________________________________________________

void FitHist::RebinOne()
{
   static Int_t ngroup = 2;
   if (fSelHist) {
      Bool_t ok;
      ngroup = GetInteger("Rebin value", ngroup, &ok, mycanvas);
      if (!ok || ngroup <= 0)
         return;
      Int_t first = fSelHist->GetXaxis()->GetFirst();
      Int_t last = fSelHist->GetXaxis()->GetLast();
      fSelHist->Rebin(ngroup);
      first /= ngroup;
      last /= ngroup;
      fSelHist->GetXaxis()->SetRange(first, last);
      TString title(fSelHist->GetTitle());
      title += "_rebinned_by_";
      title += ngroup;
      fSelHist->SetTitle(title);
      cHist->Modified(kTRUE);
      cHist->Update();
   }
}
//_______________________________________________________________________________________

void FitHist::RedefineAxis()
{
   if (expHist) {
      WarnBox("RedefineAxis not implemented \
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
   Int_t ret, ncols = 2, itemwidth = 120, precission = 5;
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
//   Int_t ret=TGMrbTableOfDoubles(mycanvas, title, 2, 1, xyvals,
//                            NULL,row_lab);
   Int_t ret, ncols = 2, nrows = 1, itemwidth = 120, precission = 5;
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
      WarnBox("No Contours defined");
      return;
   }
   TMrbNamedArrayI * colors = 0;
   colors = dynamic_cast<TMrbNamedArrayI*>(fSelHist->
              GetListOfFunctions()->FindObject("Pixel"));
   if (!colors) {
      WarnBox("No User Colors defined");
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
   if (OpenWorkFile(mycanvas)) {
      contour->Write();
      CloseWorkFile();
   }
}
//_______________________________________________________________________________________

void FitHist::UseSelectedContour()
{
   if (!hp) return;
   if(hp->fSelectContour->GetSize() <= 0) {
      WarnBox("No contour selected");
      return;
   } else if (hp->fSelectContour->GetSize() > 1) {
      WarnBox("More then one selected\n\
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
   if (hp) gStyle->SetPalette(hp->fNofColorLevels, hp->fPalette);
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
   for (Int_t i=0; i < ncont; i++) {
      (*colors)[i] = 0;
      Int_t ival = (Int_t)(i * fSelHist->GetMaximum() / ncont);
      (*xyvals)[i] = (Double_t) ival;
      if (use_old && oldcol && i < old_ncont ) {
         (*xyvals)[i] = fSelHist->GetContourLevel(i);
         (*colors)[i] = oldcol->At(i);
      } else {
//    assume colorindeces 51 - 100 ( rainbow colors)
         Int_t colind = Int_t( (i + 1)* (50 / (Float_t)ncont)) + 50;
//         cout << "colind " << colind<< endl;
         TColor * col = GetColorByInd(colind);
         if (col) (*colors)[i] = col->GetPixel();
      }
   }

   Int_t ret = ucont.Edit((TGWindow*)mycanvas);
   if (ret <  0) return;

   TH2 * h2 = (TH2*)fSelHist;
   fSetLevels = set_levels;
   fSetColors = set_colors;
//   Int_t   allcolors = 0;;
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

void FitHist::WriteHistasASCII()
{

// *INDENT-OFF* 
  const char helpText[] =
"As default only the channel contents is written\n\
to the file. To write also the channel number the\n\
option \"Output also Channel Number\" should be\n\
activated.";
// *INDENT-ON* 

   if (is2dim(fSelHist)) {
      WarnBox(" WriteHistasASCII: 2 dim not supported ");
      return;
   }
   Bool_t chan_and_cont = kFALSE;
   Bool_t chan_and_cont_and_bc = kFALSE;
   TString fname = fSelHist->GetName();
   fname += ".ascii";
   Bool_t ok;
   fname =
       GetString("Write ASCII-file with name", fname.Data(), &ok, mycanvas,
                 "Output also Channel Numbers", &chan_and_cont, helpText,
                 "Output also Bin Centers", &chan_and_cont_and_bc);
   if (!ok) {
      cout << " Cancelled " << endl;
      return;
   }
   if (!gSystem->AccessPathName((const char *) fname, kFileExists)) {
//      cout << fname << " exists" << endl;
      int buttons = kMBOk | kMBDismiss, retval = 0;
      EMsgBoxIcon icontype = kMBIconQuestion;
      new TGMsgBox(gClient->GetRoot(), mycanvas,
                   "Question", "File exists, overwrite?",
                   icontype, buttons, &retval);
      if (retval == kMBDismiss)
         return;
   }
   ofstream outfile;
   outfile.open((const char *) fname);
//   ofstream outfile((const char *)fname);
   if (outfile) {
      Int_t nbins = fSelHist->GetNbinsX();
      for (Int_t i = 1; i <= nbins; i++) {
         if (chan_and_cont)
            outfile << i << "\t";
         if (chan_and_cont_and_bc)
            outfile << fSelHist->GetBinCenter(i) << "\t";
         outfile << fSelHist->GetBinContent(i) << endl;
      }
      outfile.close();
      cout << fSelHist->GetNbinsX() << " bins written to: "
          << (const char *) fname << endl;
   } else {
      cout << " Cannot open: " << fname << endl;
   }
};
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
      if (OpenWorkFile()) {
         fSelHist->GetListOfFunctions()->Write();
         CloseWorkFile();
      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteFunctionList()
{
   if (fSelHist) {
      TString name = fSelHist->GetName();
      Int_t us = name.Index("_");
      if (us >= 0)
         name.Remove(0, us + 1);
      name += "_funcs";
      Bool_t ok;
      name =
          GetString("Save Function List with name", name.Data(), &ok,
                    mycanvas);
      if (!ok)
         return;
      if (OpenWorkFile()) {
         fSelHist->GetListOfFunctions()->Write(name.Data(), 1);
         CloseWorkFile();
      }
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
         WarnBox("Object with this name already exists");
         cout << setred << "Object with this name already exists: "
             << obj->ClassName() << setblack << endl;
         return;
      }
      if (OpenWorkFile()) {
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
                  hi->SetOption(hp->fDrawOpt2Dim->Data());
               } else {
                  TString drawopt;
                  if (hp->fShowContour)
                     drawopt = "hist";
                  if (hp->fShowErrors)
                     drawopt += "e1";
                  hi->SetOption(drawopt.Data());
                  if (hp->fFill1Dim) {
                     hi->SetFillStyle(1001);
                     hi->SetFillColor(hp->f1DimFillColor);
                  } else
                     hi->SetFillStyle(0);
               }
            }
            gROOT->SetSelectedPad(nc);
            nc->GetListOfPrimitives()->Add(obj->Clone());
         }
         if (nc->GetAutoExec())
            nc->ToggleAutoExec();
         nc->Write();
         CloseWorkFile();
         delete nc;
      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteOutHist()
{
   if (fSelHist) {
      TString hname = fSelHist->GetName();
      Int_t pp = hname.Index(";");
      if (pp > 0) hname.Resize(pp);
      Bool_t ok;
      hname =
          GetString("Save hist with name", hname.Data(), &ok, mycanvas);
      if (!ok)
         return;
      fSelHist->SetName(hname.Data());
      if (OpenWorkFile(mycanvas)) {
         fSelHist->Write();
         CloseWorkFile();
      }
   }
};
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
//   TString title("Marks");
//   Int_t ret=TGMrbTableOfDoubles(mycanvas,title,2, n, xyvals,NULL,row_lab);
//      cout << ret << endl;
   Int_t ret, ncols = 2, nrows = n, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Marks", itemwidth,
                       ncols, nrows, xyvals, precission, NULL, row_lab);
   if (ret >= 0) {
      x1 = xyvals[0];
      x2 = xyvals[1];
      y1 = xyvals[2];
      y2 = xyvals[3];
      markers->Add(new FhMarker(x1, y1, 28));
      markers->Add(new FhMarker(x2, y2, 28));
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
   x = pad->AbsPixeltoX(px);
   y = pad->AbsPixeltoY(py);
   binx = XBinNumber(fSelHist, x);
   if (is2dim(fOrigHist))
      biny = YBinNumber(fSelHist, y);
//   binx     = fXaxis.FindBin(x);
//   biny     = fYaxis.FindBin(y);
   FhMarker *m = new FhMarker(x, y, 28);
   m->SetMarkerColor(6);
   m->Draw();
   m->SetMarkerColor(6);
   m->Paint();
//   m->Print();
   markers->Add(m);
}
//_______________________________________________________________________________________

void FitHist::ClearMarks()
{
//  
   FhMarker *ti;
   TIter next(markers);
   while ( (ti = (FhMarker *) next()) ) {
//         cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
//         FhMarker *m = new FhMarker(ti->GetValX(), ti->GetValY(), 28);
      cHist->GetListOfPrimitives()->Remove(ti);
   }

   markers->Clear();
   fSelPad->Modified(kTRUE);
   fSelPad->Update();
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
   nval = markers->GetEntries();
   if (nval == 1) {
      if (fSelInside)
         markers->Add(new FhMarker(xmin, ymin, 30));
      else
         markers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   if (nval == 0) {
      markers->Add(new FhMarker(xmin, ymin, 30));
      markers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   markers->Sort();
   return nval;
};
//_______________________________________________________________________________________

void FitHist::PrintMarks()
{
   int nval = markers->GetEntries();
   if (nval > 0) {
      cout << "--- Current markers --------------" << endl;
      FhMarker *ti;
      TIter next(markers);
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
   int nval = markers->GetEntries();
   if (nval > 0) {
//      cout << "--- Current markers --------------" << endl;
      FhMarker *ti;
      TIter next(markers);
      cHist->cd();
      while ( (ti = (FhMarker *) next()) ) {
//         cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
//         FhMarker *m = new FhMarker(ti->GetValX(), ti->GetValY(), 28);
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
      WarnBox("No Histogram found");
      return 0;
   }

   TArrayI flags(nhists);
   flags.Reset();
   Int_t retval;

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
      WarnBox("No Histogram found");
   }
   if (hist == fSelHist) {
      WarnBox("Cant use same histogram");
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

   Axis_t xold, yold;
   xold = hist->GetXaxis()->GetXmin();
   if (is2dim(hist))
      yold = hist->GetYaxis()->GetXmin();
   else {
      if (is2dim(fSelHist))
         yold = fSelHist->GetYaxis()->GetXmin();
      else
         yold = 0;
   }
   markers->Add(new FhMarker(xold, yold, 2));
   xold = hist->GetXaxis()->GetXmax();
   if (is2dim(hist))
      yold = hist->GetYaxis()->GetXmax();
   else {
      if (is2dim(fSelHist))
         yold = fSelHist->GetYaxis()->GetXmax();
      else
         yold = 0;
   }
   markers->Add(new FhMarker(xold, yold, 2));
   Expand();
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
         WarnBox("More than 1 selection, take first");
      }
      hist = hp->GetSelHistAt(0);
   } else
      hist = GetOneHist();      // look in memory
//   if(hist){
//      hist->SetLineColor(fColSuperimpose);
//      cHist->cd();
//      hist->DrawCopy("same");
//      cHist->Update();
//      return;
//   }
   if (hist) {
      TGaxis *naxis = 0;
      TH1 *hdisp = hist;
      if (mode == 1 && !is2dim(fSelHist)) {	// scale
         cout << "!!!!!!!!!!!!!!!!" << endl;
         Stat_t maxy = 0;
         Axis_t x, xmin, xmax;
         xmin = fSelHist->GetXaxis()->GetXmin();
         xmax = fSelHist->GetXaxis()->GetXmax();
         for (Int_t i = 1; i <= hist->GetNbinsX(); i++) {
            x = hist->GetBinCenter(i);
            if (x >= xmin && x < xmax && hist->GetBinContent(i) > maxy)
               maxy = hist->GetBinContent(i);
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
      hdisp->SetLineColor(fColSuperimpose);
      cHist->cd();
      TString drawopt = fSelHist->GetDrawOption();
      drawopt += "same";
      cout << "drawopt " << drawopt << endl;
//      fSelHist->DrawCopy(drawopt.Data());
//      TH1* hnew =  (TH1*)hdisp->Clone();
      hdisp->DrawCopy(drawopt.Data());

//      hdisp->GetXaxis()->SetName("xaxis");
      Float_t x1 = 0.2;
      Float_t x2 = 0.3;
      Float_t y1 = 1 - 0.05 * fColSuperimpose;
      Float_t y2 = y1 + 0.05;

      tname = new TPaveLabel(x1, y1, x2, y2, hdisp->GetName(), "NDC");
//      tname->SetNDC(kTRUE);
      tname->SetTextColor(fColSuperimpose);
      tname->Draw();

      if (naxis) {
         naxis->Draw();
         naxis->SetLineColor(fColSuperimpose);
         naxis->SetTextColor(fColSuperimpose);
      }
      fColSuperimpose += 1;
      if (fColSuperimpose > 7)
         fColSuperimpose = 2;
      cHist->Update();
      //     return;

   } else
      WarnBox("No hist selected");
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
   TIter next(markers);
//   markers->Sort();
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
            WarnBox("Please select 1 window or exactly 2 marks");
            return;
         }  
         GetLimits();
         xlow = fX_1;
         xup = fX_2;
         cout << "Using marks " << endl;
      } else {
         if (Nwindows() > 1) {
            WarnBox("Please select one window");
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
//      Int_t ret = TGMrbTableOfDoubles(mycanvas,title, 1, 4, par,col_lab,row_lab);
      Int_t ret, ncols = 1, nrows = 4, itemwidth = 240, precission = 5;
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

// Transpsoe 2dim histograms

void FitHist::Transpose()
{
   if (!is2dim(fSelHist)) {
      WarnBox("Only 2dim hist can be transposed");
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
      WarnBox("Only 2dim hist can be profiled");
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
 //     cout << "max_error " << max_error << endl;
      for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
         if (h_prof->GetBinContent(ix) != 0 &&
             h_prof->GetBinError(ix) == 0)
               h_prof->SetBinError(ix, max_error);
      }
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
      WarnBox("Only 2dim hist can be profiled");
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
//      cout << "max_error " << max_error << endl;
      for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
         if (h_prof->GetBinContent(ix) != 0 &&
             h_prof->GetBinError(ix) == 0)
               h_prof->SetBinError(ix, max_error);
      }
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
            expHist->Fill(x, cont);
            if (i >= fBinlx && i <= fBinux)
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
                  WarnBox("More than 1 function defined, take first");
                else func =(TF1 *)obj;
            }
         }
         if (!func) {
            WarnBox("No function found");
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
         Double_t bothratio = hp->fProjectBothRatio;

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
      TF1 *p;
      TIter next(lof);
      while ( (p = (TF1 *) next()) ) {
//            p->Print();
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

void FitHist::WarnBox(const char *message)
{
   int retval = 0;
   new TGMsgBox(gClient->GetRoot(), mycanvas,
                "Warning", message, kMBIconExclamation, kMBDismiss,
                &retval);
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
   gStyle->SetOptTitle(hp->GetShowTitle());
   if (hp->GetShowTitle())
      gStyle->SetTitleFont(hp->fTitleFont);
   if (hp->fShowContour)
      drawopt = "hist";
   if (hp->fShowErrors)
      drawopt += "e1";
   if (hp->fFill1Dim) {
      fSelHist->SetFillStyle(1001);
      fSelHist->SetFillColor(hp->f1DimFillColor);
   } else
      fSelHist->SetFillStyle(0);
   fSelHist->SetOption(drawopt.Data());
   fSelHist->Draw();
   gBinW = fSelHist->GetBinWidth(1);
//   Float_t tz = 0.035;
//   if (hp->GetShowDateBox())
//      DrawDateBox(fSelHist, tz);
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
   if (hp->fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   DrawDate(); 
   fSelHist->SetStats(0);
   if (hp->GetShowStatBox()) fSelHist->SetStats(1);
//  add extra axis (channels) at top
   if (hp->fDrawAxisAtTop) {
      TPaveStats * st = (TPaveStats *)cHist->GetPrimitive("stats");
      if (st) {
         st->SetY1NDC(st->GetY1NDC()-0.15);           
         st->SetY2NDC(st->GetY2NDC()-0.15);
      }           
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
   if (hp && hp->fShowDateBox ) {
      TDatime dt;
      UInt_t da = fSelHist->GetUniqueID();
      if (da != 0 && !hp->fUseTimeOfDisplay) dt.Set(da);
      fDateText = new TText(gStyle->GetDateX(),gStyle->GetDateY(),dt.AsSQLString());
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
   gStyle->SetOptTitle(hp->GetShowTitle());
   if (hp->GetShowTitle())
      gStyle->SetTitleFont(hp->fTitleFont);
//   cout << "DrawOpt2Dim: " << hp->fDrawOpt2Dim->Data() << endl;
   fSelHist->Draw();
   fSelHist->SetStats(0);
   if (hp && hp->GetShowStatBox()) fSelHist->SetStats(1);
   fSelHist->SetOption(hp->fDrawOpt2Dim->Data());
   DrawCut();
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {

      lof->ls();
//      TF1 *p;
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
                SetUserPalette(1001, nai);
             }  
         }
      }
   }
   if (hp->fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   UpdateCanvas();
   if (hp->f2DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         cHist->GetFrame()->SetFillStyle(0);
      } else {
      	cHist->GetFrame()->SetFillStyle(1001);
      	cHist->GetFrame()->SetFillColor(10);
   	}
   } else {
      cHist->GetFrame()->SetFillStyle(1001);
      cHist->GetFrame()->SetFillColor(hp->f2DimBackgroundColor);
   }
   DrawDate();
   
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
   } else WarnBox("No function selected");
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
   if (!hp) return;
   SetSelectedPad();
   TString drawopt;
   if (hp->fShowContour)
      drawopt = "";
   if (hp->fShowErrors)
      drawopt += "e1";
   if (hp->fFill1Dim) {
      fSelHist->SetFillStyle(1001);
      fSelHist->SetFillColor(hp->f1DimFillColor);
   } else
      fSelHist->SetFillStyle(0);
//   cout << "UpdateDrawOptions() " << drawopt.Data() << endl;
   fSelHist->SetOption(drawopt.Data());
   fSelHist->SetDrawOption(drawopt.Data());
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
