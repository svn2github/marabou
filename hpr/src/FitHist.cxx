#include <math.h>
#include <fstream.h>
#include <iomanip.h>
#include <iostream.h>
#include <strstream.h>

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

#include "TGMsgBox.h"
#include "TGWidget.h"
#include "TGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitHist_Help.h"
#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FhMarker.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"

//extern HistPresent* hp;
extern TFile *fWorkfile;
extern const char *fWorkname;
extern Int_t nHists;
Int_t nPeaks;
Double_t gTailSide;             // in fit with tail determines side: 1 left(low), -1 high(right)
Float_t gBinW;

enum dowhat { expand, projectx, projecty, statonly, projectf,
       projectboth };

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
   fBinX_1 = fBinX_2 = -1;
   fLogx = 0;
   fLogy = 0;
   fLogz = 0;
   datebox = 0;
   fSelHist = hist;
   fSelPad = NULL;
   cHist   = NULL;
   expHist = NULL;
   projHist = NULL;
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
//         p->Print();
         if (fDimension == 2 && is_a_cut(p))
            fActiveCuts->Add(p);

         if (fDimension == 1 && is_a_window(p)) {
            if (fAllWindows->FindObject(p->GetName()));
//               cout << p->GetName() << " already existing" <<endl;
            else
               fAllWindows->Add(p);
            if (!fActiveWindows->FindObject(p->GetName()))
               fActiveWindows->Add(p);
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
   fTemplateMacro = "TwoGaus";
   fFirstUse = 1;
   fSerialPx = 0;
   fSerialPy = 0;
   fSerialPf = 0;
   func_numb = 0;
   cut_numb = 0;
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
   gDirectory->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   if (hp) hp->fNwindows -= 1;
//   SaveDefaults();
   if (expHist) {
//      cout << "expHist " << expHist->GetName() << endl;
//      dont delete possible windows
      expHist->GetListOfFunctions()->Clear("nodelete");
      gDirectory->GetList()->Remove(expHist);
      delete expHist;
      expHist = 0;
   }
   if (!cHist || !cHist->TestBit(TObject::kNotDeleted) ||
       cHist->TestBit(0xf0000000)) {
      cout << "~FitHist: Canvas is deleted" << endl;
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
   if (datebox)
      delete datebox;
   gROOT->Reset();
};
//________________________________________________________________

void FitHist::SaveDefaults(Bool_t recalculate)
{

   if (hp && (hp->fRememberLastSet || hp->fRememberZoom)) {
      TString defname;
      Bool_t fok = kFALSE;
      TEnv env(".rootrc");         // inspect ROOT's environment
      env.SetValue("HistPresent.FitMacroName", fFitMacroName);
      env.Save();
      defname =
          env.GetValue("HistPresent.LastSettingsName", defname.Data());
      if (defname.Length() <= 0) {
         WarnBox("Setting defaults dir/name to defaults/Last");
         defname = "defaults/Last";
         env.SetValue("HistPresent.LastSettingsName", defname.Data());
         env.SaveLevel(kEnvUser);
         env.Save();
      }    
      fok = kTRUE;
      Int_t lslash = defname.Last('/');
      if (lslash > 0) {
         TString dirname = defname;
         dirname.Remove(lslash, 100);
         if (gSystem->AccessPathName(dirname.Data())) {
            fok = kFALSE;
            TString question = dirname;
            question += " does not exist, create it?";
            int buttons = kMBYes | kMBNo, retval = 0;
            EMsgBoxIcon icontype = kMBIconQuestion;
            new TGMsgBox(gClient->GetRoot(), mycanvas,
                         "Warning", (const char *) question,
                         icontype, buttons, &retval);
            if (retval == kMBYes) {
               if (gSystem->MakeDirectory((const char *) dirname) == 0) {
                  fok = kTRUE;
               } else {
                  dirname.Prepend("Error creating ");
                  dirname.Append(": ");
                  dirname.Append(gSystem->GetError());
                  WarnBox(dirname.Data());
               }
            }
         }
      }
      if (fok) {
         defname += "_";
         defname += fHname;
         defname += ".def";
         if (fDeleteCalFlag && !gSystem->AccessPathName(defname.Data())) {
            defname.Prepend("rm ");
            gSystem->Exec(defname.Data());
            cout << "Removing: " << defname.Data() << endl;
         } else {
            cout << "SaveDefaults in: "<< defname.Data()  << endl;
            ofstream wstream;
            wstream.open(defname, ios::out);
            if (wstream.good()) {
               if (TCanvas * ca =
                   (TCanvas *) gROOT->FindObject(GetCanvasName())) {
                  wstream << "LogX:  " << ca->GetLogx() << endl;
                  wstream << "LogY:  " << ca->GetLogy() << endl;
                  wstream << "LogZ:  " << ca->GetLogz() << endl;
               } else {
                  //            cerr << "~FitHist(): Canvas deleted, cant find lin/log state" 
                  //                 << endl;
               }
               if (recalculate) {
                  //         check if hist is still alive
                  if (fSelHist->TestBit(TObject::kNotDeleted) &&
                      !fSelHist->TestBit(0xf0000000)) {
                     wstream << "fBinlx:  " << fSelHist->GetXaxis()->
                         GetFirst() << endl;
                     wstream << "fBinux:  " << fSelHist->GetXaxis()->
                         GetLast() << endl;
                     if (is2dim(fSelHist)) {
                        wstream << "fBinly:  " << fSelHist->GetYaxis()->
                            GetFirst() << endl;
                        wstream << "fBinuy:  " << fSelHist->GetYaxis()->
                            GetLast() << endl;
                     }
                  } else {
                     cout << "fSelHist already deleted" << endl;
                  }
               } else {
                  cout << "take current fBinlx " << endl;
                  wstream << "fBinlx:  " << fBinlx << endl;
                  wstream << "fBinux:  " << fBinux << endl;
                  if (is2dim(fSelHist)) {
                     wstream << "fBinly:  " << fBinly << endl;
                     wstream << "fBinuy:  " << fBinuy << endl;
                  }
               }
               //         check if hist is still alive
               if (fSelHist->TestBit(TObject::kNotDeleted) &&
                   !fSelHist->TestBit(0xf0000000)
                   && fSelHist->InheritsFrom("TH1")) {
                  if (strlen(fSelHist->GetXaxis()->GetTitle()) > 0) {
                     cout << "FitHist dtor: save axis title" << endl;
                     wstream << "fXtitle:  " << fSelHist->GetXaxis()->
                         GetTitle() << endl;
                  }
                  if (strlen(fSelHist->GetYaxis()->GetTitle()) > 0)
                     wstream << "fYtitle:  " << fSelHist->GetYaxis()->
                         GetTitle() << endl;
               } else {
                  if (fXtitle.Length() > 0) {
                     wstream << "fXtitle:  " << fXtitle.Data() << endl;
                  }

                  if (fYtitle.Length() > 0) {
                     wstream << "fYtitle:  " << fYtitle.Data() << endl;
                  }
               }
               if (fSetRange) {
                  //            cout << "save fRangeLowX:  "  << fRangeLowX << endl;
                  //            cout << "save fRangeUpX:   "  << fRangeUpX << endl;
                  wstream << "fRangeLowX:  " << fRangeLowX << endl;
                  wstream << "fRangeUpX:  " << fRangeUpX << endl;
                  if (fDimension == 2) {
                     wstream << "fRangeLowY:  " << fRangeLowY << endl;
                     wstream << "fRangeUpY:  " << fRangeUpY << endl;
                  }
               }
            } else {
               cerr << "SaveDefaults: "
                   << gSystem->GetError() << " - " << defname << endl;
            }
         }
      }
   }
   return;
}
//________________________________________________________________

void FitHist::RestoreDefaults()
{
   TEnv *lastset = 0;
   if (hp && (hp->fRememberLastSet || hp->fRememberZoom) && (lastset = GetDefaults(fHname)) ) {
      fLogy = lastset->GetValue("LogY", fLogy);
      fRangeLowX = fSelHist->GetXaxis()->GetXmin();
      fRangeUpX = fSelHist->GetXaxis()->GetXmax();
      if (lastset->Lookup("fRangeLowX")) {
         fRangeLowX = lastset->GetValue("fRangeLowX", fRangeLowX);
         fRangeUpX = lastset->GetValue("fRangeUpX", fRangeUpX);
         fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(), fRangeLowX, fRangeUpX);
         fSetRange = kTRUE;
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
         if (lastset)
            delete lastset;
      }
   }
}
//______________________________________________________________________________________

void FitHist::handle_mouse()
{   
   if (hp && !(hp->fRememberZoom)) return;

   Int_t event = gPad->GetEvent();
//   cout << "Event " << event << endl;
   TObject *select = gPad->GetSelected();
   if (!select) return;
   if (gPad->GetLogx() !=  fLogx ||
       gPad->GetLogy() !=  fLogy ||
       gPad->GetLogz() !=  fLogz ) {
       fLogx = gPad->GetLogx();
       fLogy = gPad->GetLogy();
       fLogz = gPad->GetLogz();
       if      (fDimension == 1) mycanvas->SetLog(fLogy);
       else if (fDimension == 2) mycanvas->SetLog(fLogz);
       SaveDefaults();
   }
   if (event != kButton1Up) return;
   if (select->IsA() == TAxis::Class()) {
//      cout << "handle_mouse " << GetSelHist()->GetName() << endl;
      if (is2dim(GetSelHist()) || !strncmp(select->GetName(), "xaxis", 5)) {
         SaveDefaults();
      }
   }
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
   RestoreDefaults();   
//  construct name of canvas and pad
   fCname = "C_";
   fCname += GetName();
   fEname = "E_";
   fEname += GetName();
   fCtitle = fSelHist->GetName();
   fCtitle += ": ";
   fCtitle += fSelHist->GetTitle();
   cHist = new HTCanvas(fCname.Data(), fCtitle.Data(),
                        win_topx, win_topy, win_widx, win_widy, hp, this);
   fCanvasIsAlive = kTRUE;
   if (hp)
      hp->GetCanvasList()->Add(cHist);
   cHist->ToggleEventStatus();

   cHist->SetEditable(kTRUE);

   hist->SetDirectory(gROOT);
   //  fRootFile->Close();   
   TString cmd("((FitHist*)gROOT->FindObject(\""); 
   cmd += GetName();
   cmd += "\"))->handle_mouse()";
 //  cout << "cmd: " << cmd << endl;
   cHist->AddExec("handle_mouse", cmd.Data());
   gDirectory = gROOT;
   fSelPad = cHist;
   fSelPad->cd();
   if (hp && hp->GetUseTimeOfDisplay()) {
      TDatime dt;
      hist->SetUniqueID(dt.Get());
//     cout << whichtime.Data() << endl;
   }

//   ClearMarks();
//   Axis_t xl = 0, xu = 0, yl = 0, yu = 0, xrl=0, xru=0, yrl=0, yru=0;
//   Int_t  xbinl = 0, xbinu = 0, ybinl = 0, ybinu = 0;

   if (is2dim(hist)) {
      fSelPad->cd();
      if (fLogz) {
         cHist->SetLogz();
         if (hp && hp->fLogScaleMin > 0)
            hist->SetMinimum(hp->fLogScaleMin);
      }
      Draw2Dim();
      if (hp && hp->fAutoExec_2 ) {
   		TString cmd("((HistPresent*)gROOT->FindObject(\""); 
   		cmd += GetName();
   		cmd += "\"))->auto_exec_2()";
         cHist->AddExec("ex2", cmd.Data());
 //  cout << "cmd: " << cmd << endl;
 //        TString exs(".x ");
 //        exs += hp->fAutoExecName_2->Data();
//         cHist->AddExec("ex2", exs.Data());
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
//      if (hp && hp->fAutoExec_1 
//             && !gSystem->AccessPathName(hp->fAutoExecName_1->Data())) {
//         TString exs(".x ");
//         exs += hp->fAutoExecName_1->Data();
//         cHist->AddExec("ex1", exs.Data());
      }
   }
   cHist->Update();
};

//------------------------------------------------------ 
// Magnify

void FitHist::Magnify()
{
   if (is2dim(fSelHist)) {
      WarnBox("Magnify 2 dim not yet supported ");
      return;
   }
   Int_t newx = (Int_t) 2.1 * fSelHist->GetNbinsX();
   Int_t wx = cHist->GetWw();
   if (newx <= wx)
      return;
   else {
      Float_t fac = (Float_t) newx / (Float_t) wx;
      TAxis *xaxis = fSelHist->GetXaxis();
      TAxis *yaxis = fSelHist->GetYaxis();
      xaxis->SetNdivisions(2080);
      yaxis->SetTickLength(0.03 / fac);
      cHist->SetLeftMargin(0.1 / fac);
      cHist->SetRightMargin(0.1 / fac);
      gStyle->SetStatW(0.19 / fac);
      gStyle->SetTitleXSize(0.02 / fac);
      gStyle->SetLabelOffset(0.001 / fac, "y");
   }
   Int_t newy = (Int_t) 0.95 * cHist->GetWh();
   cHist->SetCanvasSize(newx, newy);
   cHist->Modified(kTRUE);
   cHist->Update();

}

//------------------------------------------------------ 

// Display entire

void FitHist::Entire()
{
   if (expHist) {
      expHist->GetListOfFunctions()->Clear();
      expHist->Delete();
      expHist = NULL;
   }
   fSelHist = fOrigHist;
   fSelHist->SetMinimum(-1111);
   fSelHist->SetMaximum(-1111);
//   fSelHist->SetMaximum(fMax);  
//   fSelHist->SetMinimum(fMin);
   fSelHist->GetXaxis()->SetRange(1, fSelHist->GetNbinsX());

   ClearMarks();
   fSelPad->cd();
   if (is2dim(fSelHist)) {
      fSelHist->GetYaxis()->SetRange(1, fSelHist->GetNbinsY());
      Draw2Dim();
   } else {
      Draw1Dim();
   }
   cHist->Modified(kTRUE);
   cHist->Update();
   SaveDefaults();
};

//------------------------------------------------------ 

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

//------------------------------------------------------ 

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

//------------------------------------------------------ 

void FitHist::AddAxis(Int_t where)
{
//   Double_t *xyvals = new Double_t[2];
   TArrayD xyvals(2);
//   TString title("Define axis limits");
   TOrdCollection *row_lab = new TOrdCollection();
   row_lab->Add(new TObjString("Lower, Upper"));
   if (where == 1) {            // xaxis
      xyvals[0] = cHist->GetFrame()->GetX1();
      xyvals[1] = cHist->GetFrame()->GetX2();
   } else {
      xyvals[0] = cHist->GetFrame()->GetY1();
      xyvals[1] = cHist->GetFrame()->GetY2();
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
      TString side("-");
      Float_t loff = 0.05;
      Float_t lsize;
      Int_t ndiv;
      if (where == 1) {
         x1 = cHist->GetFrame()->GetX1();
         x2 = cHist->GetFrame()->GetX2();
         y1 = cHist->GetFrame()->GetY2();
         y2 = y1;
         loff = 0.02;
         ndiv = fSelHist->GetXaxis()->GetNdivisions();
         lsize = fSelHist->GetXaxis()->GetLabelSize();
      } else {
         side = "+";
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

//------------------------------------------------------ 

//void  FitHist::WriteHistasASCII(const char * fname, Bool_t contonly){
void FitHist::WriteHistasASCII()
{

// *INDENT-OFF* 
  const char helpText[] =
"As default only the channel contents is written
to the file. To write also the channel number the
option \"Output also Channel Number\" should be
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

//------------------------------------------------------ 

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

//------------------------------------------------------ 

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

//------------------------------------------------------ 

void FitHist::WriteFunctions()
{
   if (fSelHist) {
      if (OpenWorkFile()) {
         fSelHist->GetListOfFunctions()->Write();
         CloseWorkFile();
      }
   }
};

//------------------------------------------------------ 

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

//------------------------------------------------------  
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

//------------------------------------------------------  
void FitHist::WriteOutHist()
{
   if (fSelHist) {
      TString hname = fSelHist->GetName();
      Bool_t ok;
      hname =
          GetString("Save hist with name", hname.Data(), &ok, mycanvas);
      if (!ok)
         return;
      fSelHist->SetName(hname.Data());
      if (OpenWorkFile()) {
         fSelHist->Write();
         CloseWorkFile();
      }
   }
};

//------------------------------------------------------ 

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

//------------------------------------------------------ 
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
//   cout << " x " << x << " y " << y << "binx "<< binx  << " biny " << biny << endl;
}

//------------------------------------------------------ 

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

//------------------------------------------------------ 

Int_t FitHist::GetMarks(TH1 * hist)
{
//   
   Int_t nval = 0;
   Axis_t xmin, xmax, ymin = 0, ymax = 0;
   if (hist) {
      TAxis *xaxis = hist->GetXaxis();
      Int_t first = xaxis->GetFirst();
      Int_t last = xaxis->GetLast();
      xmin = xaxis->GetBinLowEdge(first);
      xmax = xaxis->GetBinUpEdge(last);
//      nxbins = hist->GetNbinsX();
//      xbinwidth=(xmax-xmin)/nxbins;
      if (is2dim(hist)) {
         TAxis *yaxis = hist->GetYaxis();
         first = yaxis->GetFirst();
         last = yaxis->GetLast();
         ymin = yaxis->GetBinLowEdge(first);
         ymax = yaxis->GetBinUpEdge(last);
//         nybins = hist->GetNbinsY();
//         ybinwidth=(ymax-ymin)/nybins;
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
//      if(fSelInside)markers->Add(new TPInputs(1,1,xmin,ymin));
//      else          markers->Add(new TPInputs(nxbins,nybins,xmax,ymax));
      nval = 2;
   }
   if (nval == 0) {
      markers->Add(new FhMarker(xmin, ymin, 30));
      markers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
//   } 
   markers->Sort();
   return nval;
};

//------------------------------------------------------ 

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

//------------------------------------------------------ 

void FitHist::PaintMarks()
{
   int nval = markers->GetEntries();
   if (nval > 0) {
      cout << "--- Current markers --------------" << endl;
      FhMarker *ti;
      TIter next(markers);
      cHist->cd();
      while ( (ti = (FhMarker *) next()) ) {
         cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
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
      inp = XBinNumber(fSelHist, p->GetX());
      if (inp > 0) {
//         cout <<  "xbin " << inp << endl;
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
      inp = YBinNumber(fSelHist, p->GetY());
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
      if (!Nwindows()) {
         GetLimits();
         xlow = fX_1;
         xup = fX_2;
         cout << "Using marks " << endl;
      } else {
         if (Nwindows() > 1) {
            WarnBox("Please selected one window");
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
      cout << "Content " << sum << endl;
      cout << "Mean    " << mean << endl;
      cout << "Sigma   " << sigma << endl;
      cout << "Fwhm    " << 2.35 * sigma << endl;	// 2*sqrt(2ln2)
      cout <<
          "-------------------------------------------------------------"
          << endl;

      TOrdCollection *row_lab = new TOrdCollection();
      row_lab->Add(new TObjString("Content"));
      row_lab->Add(new TObjString("Mean"));
      row_lab->Add(new TObjString("Sigma"));
      row_lab->Add(new TObjString("Fwhm"));

      TOrdCollection *col_lab = new TOrdCollection();
      col_lab->Add(new TObjString((char *) fOrigHist->GetName()));
      TArrayD par(4);
//      double par[4];
      par[0] = sum;
      par[1] = mean;
      par[2] = sigma;
      par[3] = 2.35 * sigma;
      ostrstream buf;
      buf << xlow << " to " << xup << '\0';
      TString title("Statistics in window: ");
      title += buf.str();
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
         if (xcent >= fX_1 && xcent < fX_2) {
            for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
               Axis_t ycent = yaxis->GetBinCenter(j);
               if (ycent >= fY_1 && ycent < fY_2) {
                  if (nc <= 0 ||
                      (nc > 0 && InsideCut((float) xcent, (float) ycent)))
                     sum += fSelHist->GetCellContent(i, j);
               }
            }
         }
      }
      cout << "Statistics for 2 dim Histogram:" << fSelHist->
          GetName() << endl;
      cout << " X from " << fX_1 << " to " << fX_2 << endl;
      cout << " Y from " << fY_1 << " to " << fY_2 << endl;
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
      }
      cout << "Content " << sum << endl;
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

// Show expanded part of histograms

void FitHist::ExpandProject(Int_t what)
{
//   enum dowhat {expand, projectx, projecty, statonly};

   if (!fSelPad) {
      cout << "Cant find pad, call Debugger" << endl;
   }

   if (GetMarks(fSelHist) <= 0) {
      cout << "No selection" << endl;
   }
   GetLimits();
//  1-dim case
   if (!is2dim(fSelHist)) {
      Double_t x = 0, cont = 0;
      Double_t sum = 0;
      Double_t sumx = 0;
      Double_t sumx2 = 0;
      Double_t mean;
      Double_t sigma;
//      cout << "fBinX_1,2 " << fBinX_1 << " " << fBinX_2 << endl;         
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
         expHist = new TH1D(fEname.Data(), fOrigHist->GetTitle(),
                            NbinX, fExplx, fExpux);
         TDatime dt;
         expHist->SetUniqueID(dt.Get());
         expHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         expHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
//         expHist->Sumw2();
      }
//      Int_t nperbinX = 1;
//      for (Int_t i=fBinlx; i <= fBinux ; i++ ){
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
//            Stat_t oldcont = expHist->GetBinContent(irebin);
//            oldcont+=cont;
//            expHist->SetBinContent(irebin,cont);
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
         TList *lof = fSelHist->GetListOfFunctions();
         if (lof->GetSize() != 1) {
            cout << "Exactly one function required" << endl;
            return;
         } else {
            func = (TF1 *) lof->First();
            if (!func) {
               cout << "No function found" << endl;
               return;
            }
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
      Int_t nperbinX = 1, nperbinY = 1;
      if (what == projectx || what == projectboth) {
         ostrstream pname;
//         TString pname = "Px_";
//         pname += fOrigHist->GetName();
         pname << "Px" << fSerialPx << "_" << fOrigHist->GetName() << '\0';
         projHistX = new TH1F(pname.str(), fOrigHist->GetTitle(),
                              NbinX, fExplx, fExpux);
         fSerialPx++;
         pname.rdbuf()->freeze(0);
//         fSelHist=projHist;
      }
      if (what == projecty || what == projectf || what == projectboth) {
         ostrstream pname;
//         TString pname = "Py_";
         Axis_t low = fExply;
         Axis_t up = fExpuy;
         if (what == projectf) {
            up = 0.5 * (fExpuy - fExply);
            low = -up;
            pname << "Pf" << fSerialPf;
            fSerialPf++;
         } else {
            pname << "Py" << fSerialPy;
            fSerialPy++;
         }
         pname << "_" << fOrigHist->GetName() << '\0';
         projHistY = new TH1F(pname.str(), fOrigHist->GetTitle(),
                              NbinY, low, up);
         TDatime dt;
         projHistY->SetUniqueID(dt.Get());
         pname.rdbuf()->freeze(0);
//         fSelHist=projHist;
      }
      if (what == expand) {
         Int_t MaxBin2dim = 1000;
         nperbinX = (NbinX - 1) / MaxBin2dim + 1;
         Int_t NbinXrebin = (NbinX - 1) / nperbinX + 1;
         nperbinY = (NbinY - 1) / MaxBin2dim + 1;
         Int_t NbinYrebin = (NbinY - 1) / nperbinY + 1;
         if (expHist)
            expHist->GetListOfFunctions()->Clear();
         expHist = new TH2F(fEname.Data(), fOrigHist->GetTitle(),
                            NbinXrebin, fExplx, fExpux, NbinYrebin, fExply,
                            fExpuy);
         expHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         expHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
         fSelHist = expHist;
         TDatime dt;
         expHist->SetUniqueID(dt.Get());
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
      fSelPad->cd();
      if (what == projectboth) {
         Double_t bothratio = hp->fProjectBothRatio;

         if (bothratio > 1 || bothratio <= 0)
            bothratio = 0.8;
         cHist->SetTopMargin(1 - bothratio);
         cHist->SetRightMargin(1 - bothratio);
         Double_t xmin = xa->GetXmin();
         Double_t xmax = xa->GetXmax();
         Double_t dx = (xmax - xmin) / (Double_t) projHistX->GetNbinsX();
         Double_t ymin = ya->GetXmin();
         Double_t ymax = ya->GetXmax();
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

         xmin = xa->GetXmin();
         xmax = xa->GetXmax();
         ymin = ya->GetXmin();
         ymax = ya->GetXmax();
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
   else
      Draw1Dim();
   ClearMarks();
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

void FitHist::Draw1Dim()
{
   TString drawopt;
   if (hp->GetShowStatBox()) {
      gStyle->SetOptStat(hp->GetOptStat());
//       gStyle->SetStatFont(hp->fStatFont); 
//       cout << "hp->GetOptStat() " << hp->GetOptStat() << endl;     
   } else {
      gStyle->SetOptStat(0);
   }
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
   fSelHist->Draw(drawopt.Data());
   gBinW = fSelHist->GetBinWidth(1);
   Float_t tz = 0.035;
   if (hp->GetShowDateBox())
      DrawDateBox(fSelHist, tz);
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
      cHist->Update();
   }
}

//____________________________________________________________________________

void FitHist::Draw2Dim()
{
   cHist->cd();
   if (hp->GetShowStatBox()) {
      gStyle->SetOptStat(hp->GetOptStat());
      gStyle->SetStatFont(hp->fStatFont);
//       cout << "hp->GetOptStat() " << hp->GetOptStat() << endl;     
   } else {
      gStyle->SetOptStat(0);
   }
   gStyle->SetOptTitle(hp->GetShowTitle());
   if (hp->GetShowTitle())
      gStyle->SetTitleFont(hp->fTitleFont);
//   cout << "DrawOpt2Dim: " << hp->fDrawOpt2Dim->Data() << endl;
//   fSelHist->Draw();
//   SetSelectedPad();
   fSelHist->Draw(hp->fDrawOpt2Dim->Data());
   Float_t tz = 0.035;
//   Float_t tz=gStyle->GetTitleH();
   if (hp->GetShowDateBox())
      DrawDateBox(fSelHist, tz);
   DrawCut();
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {
//      TF1 *p;
      TObject *p;
      TIter next(lof);
//      while( p= (TF1*)next() ){
      while ( (p = (TObject *) next()) ) {
//         p->Print();
         p->Draw("same");
         if (p->InheritsFrom(TF1::Class())) {
            TF1 *f = (TF1 *) p;
            f->SetParent(fSelHist);
         }
         if (p->InheritsFrom(TMrbWindow2D::Class())) {
            TMrbWindow2D *w = (TMrbWindow2D *) p;
            w->SetParent(fSelHist);
            fActiveCuts->Add(w);
         }
      }
   }
   if (hp->fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   UpdateCanvas();
   if (hp->f2DimBackgroundColor == 0) {
      cHist->GetFrame()->SetFillStyle(0);
   } else {
      cHist->GetFrame()->SetFillStyle(1001);
      cHist->GetFrame()->SetFillColor(hp->f2DimBackgroundColor);
//      cout <<  cHist->GetName() << endl;
//      cout << "GetFillColor() " <<  cHist->GetFrame()->GetFillColor() << endl;
   }
   cHist->Update();
}

//__________________________________________________________________________________

void FitHist::DrawDateBox(TH1 * fSelHist, Float_t tz)
{
   UInt_t uid = fSelHist->GetUniqueID();
   if (uid) {
      datebox =
          new TPaveText(0.7 - 6 * tz, 0.995 - 2 * tz, 0.70, 0.995 - tz,
                        "NDC");
      UInt_t year = uid >> 26;
      year += 1995;
      UInt_t month = (uid << 6) >> 28;
      UInt_t day = (uid << 10) >> 27;
      UInt_t hour = (uid << 15) >> 27;
      UInt_t min = (uid << 20) >> 26;
      ostrstream buf;
      buf << setfill('0');
      buf << setw(2) << day << "." << setw(2) << month << "."
          << year << " "
          << setw(2) << hour << ":" << setw(2) << min << '\0';
      datebox->AddText(buf.str());
      datebox->Draw();
   }
}
//______________________________________________________________________________________
  
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
         fSelHist->SetMinimum(fSelHist->GetBinContent(fSelHist->GetMinimumBin()));
   }
   cHist->SetLogz(state);
   SaveDefaults();
}
