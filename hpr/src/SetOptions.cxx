#include "TROOT.h"
#include "TEnv.h"
#include "TColor.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TFrame.h"
#include "TF1.h"

#include "TGMrbInputDialog.h"
#include "TGMrbTableFrame.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "TMrbHelpBrowser.h"

#include <iostream>
#include <fstream>
#include <sstream>

//#include "HprAutoExecMacro.cxx"

//_______________________________________________________________________
// *INDENT-OFF* 
//const char AutoExecMacro_1[]=
//"
void HistPresent::auto_exec_1()
{
   //example of macro called when a pad is redrawn
   //one must create a TExec object in the following way
   // TExec ex(\"ex\",\".x exec1.C\");
   // ex.Draw();
   // this macro prints the bin number and the bin content when one clicks
   //on the histogram contour of any histogram in a pad
   
   int event = gPad->GetEvent();
//   cout << "event "<< event << endl;
   if (event != 1) return;
   int px = gPad->GetEventX();
   TObject *select = gPad->GetSelected();
   if (!select) return;
//   cout << "selected: " << select->GetName() << endl;
   if (select->InheritsFrom("TH1")) {
      TH1 *h = (TH1*)select;
      Float_t xx = gPad->AbsPixeltoX(px);
      Float_t x  = gPad->PadtoX(xx);
      Int_t binx = h->GetXaxis()->FindBin(x);
      cout << "hist: "  <<  h->GetName()<< " bin= " << binx 
           << " cont= " << h->GetBinContent(binx)<< endl;
   }
   if (!strncmp(select->GetName(), "stats",5)) {
      cout << "stat box selected" << endl;
      TList * l = gPad->GetListOfPrimitives();
      TIter next(l);
      TObject * o;
     while ( (o = next()) ) {
         if(o->InheritsFrom("TH1")){
            TH1* h = (TH1*)o;
       cout << "-------------------------------------------------------" << endl;
            cout << "Statistics for histogram: " << h->GetName() << endl;
//       cout << "-------------------------------------------------------" << endl;
//            h->Print();
       cout << "-------------------------------------------------------" << endl;
            cout << "Entries:   " << h->GetEntries() << endl;            
            cout << "Integral:  " << h->Integral()   << endl;            
            cout << "Mean:      " << h->GetMean()    << endl;            
            cout << "RMS:       " << h->GetRMS()     << endl;
            Int_t fbin = h->GetXaxis()->GetFirst()-1;         
            Stat_t uf = h->Integral(0,fbin);
            cout << "UnderFlow: " << uf << endl;            
            Int_t lbin = h->GetXaxis()->GetLast()+1;         
            Stat_t of = h->Integral(lbin,h->GetNbinsX()+1);
            cout << "OverFlow:  " << of << endl;            
//            cout << ":" << h->() << endl;            
            break;
         }
      }
   }
   if (!strncmp(select->GetName(), "TFrame",6)) {
//      cout << "TFrame selected" << endl;
      if(gPad == gPad->GetMother()){
//         cout << "not in divided" << endl;
         return;
      } 
      HistPresent * hp = (HistPresent*)gROOT->FindObject("mypres");
      if(!hp) return;
      TList * l = gPad->GetListOfPrimitives();
      TIter next(l);
      TObject * o;
      while ( (o = next()) ) {
         if(o->InheritsFrom("TH1")){
            TH1* h = (TH1*)o;
            TString hname(h->GetName());
            Int_t last_us = hname.Last('_');    // chop off us added by GetSelHistAt
            if(last_us >0){
               hname.Remove(last_us);
               h->SetName(hname.Data());
            }
            hp->ShowHist(h);
            return;
         }
      }
   }
   if (select->InheritsFrom("TF1")) {
      TF1 * f = (TF1*)select;
       f->Print();
   }
}

//";
//__________________________________________________________________________

//const char AutoExecMacro_2[]=
//"
void HistPresent::auto_exec_2()
{
   //example of macro called when a mouse event occurs in a pad.
   // Example:
   // Root > TFile f("hsimple.root");
   // Root > hpxpy.Draw();
   // Root > c1.AddExec("ex2",".x exec2.C");
   // When moving the mouse in the canvas, a second canvas shows the
   // projection along X of the bin corresponding to the Y position
   // of the mouse. The resulting histogram is fitted with a gaussian.
   // A "dynamic" line shows the current bin position in Y.
   // This more elaborated example can be used as a starting point
   // to develop more powerful interactive applications exploiting CINT
   // as a development engine.
   //

   int event = gPad->GetEvent();
//   cout << "event "<< event << endl;
   if (event != 1 && event != 51) return;
   TObject *select = gPad->GetSelected();
   if(!select) return;
//   cout << "selected " << select->GetName() << endl;
   
   HistPresent * hpr = (HistPresent*)gROOT->FindObject("mypres");
   if(!hpr) return;
   if (event == 1 && select->InheritsFrom("TH2")) {
//      cout << "TFrame selected" << endl;
      if(gPad == gPad->GetMother()){
//         cout << "not in divided" << endl;
         return;
      } 
      TList * l = gPad->GetListOfPrimitives();
      TIter next(l);
      TObject * o;
      while ( (o = next()) ){
         if (o->InheritsFrom("TH1")) {
            TH1* h = (TH1*)o;
            hpr->ShowHist(h);
            return;
         }
      }
   }
   if(!hpr->GetAutoProj_X() && !hpr->GetAutoProj_Y()) return;
  
   if (!select->InheritsFrom("TH2")) {gPad->SetUniqueID(0); return;}
   gPad->GetCanvas()->FeedbackMode(kTRUE);

   //erase old position and draw a line at current position
   int pyold = gPad->GetUniqueID();
//   int px = gPad->GetEventX();
   int py = gPad->GetEventY();
   float uxmin = gPad->GetUxmin();
   float uxmax = gPad->GetUxmax();
   int pxmin = gPad->XtoAbsPixel(uxmin);
   int pxmax = gPad->XtoAbsPixel(uxmax);
   if(pyold) gVirtualX->DrawLine(pxmin,pyold,pxmax,pyold);
   gVirtualX->DrawLine(pxmin,py,pxmax,py);
   gPad->SetUniqueID(py);
   Float_t upy = gPad->AbsPixeltoY(py);
   Float_t y = gPad->PadtoY(upy);

   //create or set the new canvas c2
   TVirtualPad *padsav = gPad;
   TCanvas *c2 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("c2");
   if(c2) delete c2->GetPrimitive("Projection");
   else   c2 = new TCanvas("c2");
   c2->cd();

   //draw slice corresponding to mouse position
   TH2 *h = (TH2*)select;
   Int_t biny = h->GetYaxis()->FindBin(y);
   TH1D *hp = h->ProjectionX("",biny,biny);
   char title[80];
   sprintf(title,"Projection of biny=%d",biny);
   hp->SetName("Projection");
   hp->SetTitle(title);
   hp->Fit("gaus","ql");
    hp->Draw();
   c2->Update();
   padsav->cd();
}
//";
// *INDENT-ON* 
//_______________________________________________________________________

void HistPresent::RestoreOptions()
{
   TEnv env(".rootrc");
   fWintopx = 515;              // origin of window
   fWintopy = 5;
   fWinwidx_1dim = 750;
   fWinwidy_1dim = 550;
   fWinwidx_2dim = 750;
   fWinwidy_2dim = 750;
   fWinshifty = 30;
   fWinshiftx = 0;
   fWinwidx_hlist = 0; 
   fWinwidx_2dim =
       env.GetValue("HistPresent.WindowXWidth_2dim", fWinwidx_2dim);
   fWinwidy_2dim =
       env.GetValue("HistPresent.WindowYWidth_2dim", fWinwidy_2dim);
   fWinwidx_1dim =
       env.GetValue("HistPresent.WindowXWidth_1dim", fWinwidx_1dim);
   fWinwidy_1dim =
       env.GetValue("HistPresent.WindowYWidth_1dim", fWinwidy_1dim);
   if (fWinwidx_1dim <= 0)
      fWinwidx_1dim = 750;
   if (fWinwidy_1dim <= 0)
      fWinwidy_1dim = 550;
   if (fWinwidx_2dim <= 0)
      fWinwidx_2dim = 750;
   if (fWinwidy_2dim <= 0)
      fWinwidy_2dim = 750;
   fWinwidx_hlist = env.GetValue("HistPresent.WindowXWidth_List", fWinwidx_hlist);
   fWintopx = env.GetValue("HistPresent.WindowX", fWintopx);
   fWintopy = env.GetValue("HistPresent.WindowY", fWintopy);
   fWincurx = fWintopx;
   fWincury = fWintopy;
   fWinshiftx = env.GetValue("HistPresent.WindowXShift", fWinshiftx);
   fWinshifty = env.GetValue("HistPresent.WindowYShift", fWinshifty);
   *fDrawOpt2Dim =
       env.GetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim->Data());
   *f2DimColorPalette =
       env.GetValue("HistPresent.ColorPalette", f2DimColorPalette->Data());
   fShowErrors = env.GetValue("HistPresent.ShowErrors", fShowErrors);
   fDrawAxisAtTop = env.GetValue("HistPresent.DrawAxisAtTop", fDrawAxisAtTop);
   fFill1Dim = env.GetValue("HistPresent.Fill1Dim", fFill1Dim);
   fShowContour = env.GetValue("HistPresent.ShowContour", fShowContour);
   fOptStat = env.GetValue("HistPresent.OptStat", 1001111);
   fShowDateBox = env.GetValue("HistPresent.ShowDateBox", 1);
   fShowStatBox = env.GetValue("HistPresent.ShowStatBox", 1);
   fUseTimeOfDisplay = env.GetValue("HistPresent.UseTimeOfDisplay", 0);
   fShowTitle = env.GetValue("HistPresent.ShowTitle", 1);
   fEnableCalibration = env.GetValue("HistPresent.EnableCalibration", 0);
   fShowFitBox = env.GetValue("HistPresent.ShowFitBox", 1);
   fShowFittedCurves = env.GetValue("HistPresent.ShowFittedCurves", 1);
   fShowPSFile = env.GetValue("HistPresent.AutoShowPSFile", 1);
   fShowListsOnly= env.GetValue("HistPresent.ShowListsOnly", 0);
   fRememberTreeHists = env.GetValue("HistPresent.RememberTreeHists", 0);
   fRememberLastSet = env.GetValue("HistPresent.RememberLastSet", 1);
   fRememberZoom = env.GetValue("HistPresent.RememberZoom", 0);
   fUseAttributeMacro = env.GetValue("HistPresent.UseAttributeMacro", 1);
   fShowAllAsFirst = env.GetValue("HistPresent.ShowAllAsFirst", 1);
   fUseRegexp = env.GetValue("HistPresent.UseRegexp", 0);
   fProjectBothRatio =
       atof(env.GetValue("HistPresent.ProjectBothRatio", "0.6"));
   fLogScaleMin = atof(env.GetValue("HistPresent.LogScaleMin", "1"));
   fAutoUpdateDelay =
       atof(env.GetValue("HistPresent.AutoUpdateDelay", "2"));
   fPeakMwidth = env.GetValue("HistPresent.fPeakMwidth", 11);
   fPeakThreshold = env.GetValue("HistPresent.fPeakThreshold", 3.);
   fLiveStat1dim= env.GetValue("HistPresent.LiveStat1dim", 0);
   fLiveStat2dim= env.GetValue("HistPresent.LiveStat2dim", 0);
   fLiveGauss = env.GetValue("HistPresent.LiveGauss", 0);
   fLiveBG = env.GetValue("HistPresent.LiveBG", 0);

   fFitOptLikelihood = env.GetValue("HistPresent.FitOptLikelihood", 0);
   fFitOptQuiet = env.GetValue("HistPresent.FitOptQuiet", 0);
   fFitOptVerbose = env.GetValue("HistPresent.FitOptVerbose", 0);
   fFitOptMinos = env.GetValue("HistPresent.FitOptMinos", 0);
   fFitOptErrors1 = env.GetValue("HistPresent.FitOptErrors1", 0);
   fFitOptIntegral = env.GetValue("HistPresent.FitOptIntegral", 0);
   fFitOptNoDraw = env.GetValue("HistPresent.FitOptNoDraw", 0);
   fFitOptAddAll = env.GetValue("HistPresent.FitOptAddAll", 0);
   fFitOptKeepParameters = env.GetValue("HistPresent.FitOptKeepPara", 0);
   f2DimBackgroundColor =
       env.GetValue("HistPresent.2DimBackgroundColor", 0);
   f1DimFillColor = env.GetValue("HistPresent.1DimFillColor", 46);
   fStatFont = env.GetValue("HistPresent.StatBoxFont", 40);
   fTitleFont = env.GetValue("HistPresent.TitleBoxFont", 60);
   fFitOptUseLinBg = env.GetValue("HistPresent.FitOptUseLinBg", 0);
   fMaxListEntries= env.GetValue("HistPresent.MaxListEntries", 333);
   fDisplayCalibrated= env.GetValue("HistPresent.DisplayCalibrated", 1);
   *fHostToConnect =
       env.GetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   fSocketToConnect =
       env.GetValue("HistPresent.SocketToConnect", fSocketToConnect);
   fAutoExec_1 = env.GetValue("HistPresent.AutoExec_1", 1);
   fAutoExec_2 = env.GetValue("HistPresent.AutoExec_2", 1);
   fAutoProj_X = env.GetValue("HistPresent.AutoProj_X", 0);
   fAutoProj_Y = env.GetValue("HistPresent.AutoProj_Y", 0);
   *fHelpDir =
       env.GetValue("HistPresent.HelpDir",
                    "/usr/local/marabou_doc/html/hpr");

   if (!gSystem->AccessPathName(fHelpDir->Data()))
      fHelpBrowser = new TMrbHelpBrowser(fHelpDir->Data());
   else
      fHelpBrowser = NULL;

   fNofGreyLevels = 30;
   fGreyPalette = new Int_t[fNofGreyLevels];
   fGreyPaletteInv = new Int_t[fNofGreyLevels];
   static Int_t mono = 0;
   TColor *color;
   if (mono == 0) {
      mono = 1;
      Float_t frac = 1 / (Float_t) fNofGreyLevels;
      for (Int_t i = 0; i < fNofGreyLevels; i++) {
         Float_t x = (Float_t) i;
         color =
             new TColor(331 + i, 1 - x * frac, 1 - x * frac, 1 - x * frac,
                        "");
         fGreyPaletteInv[i] = 331 + i;
         color = new TColor(301 + i, x * frac, x * frac, x * frac, "");
         fGreyPalette[i] = 301 + i;
      }
   }
   if (f2DimColorPalette->Contains("MONO")) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPalette;
   } else if (f2DimColorPalette->Contains("MINV")) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPaletteInv;
   } else if (f2DimColorPalette->Contains("REGB")) {
      fNofColorLevels = 1;
      fPalette = NULL;
   } else {
      fNofColorLevels = 50;
      fPalette = NULL;
   }
   gStyle->SetPalette(fNofGreyLevels, fPalette);
//   CheckAutoExecFiles();
//
   if (fShowStatBox) {
      gStyle->SetOptStat(fOptStat);
      gStyle->SetOptFit(fShowFitBox);
   } else {
      gStyle->SetOptFit(0);
      gStyle->SetOptStat(0);
   }
   gStyle->SetOptTitle(fShowTitle);
//   gStyle->SetStatFont(fStatFont);
   gStyle->SetTitleFont(fTitleFont);
   gROOT->ForceStyle();
//   cout << "RestoreOptions,  fUseAttributeMacro " << fUseAttributeMacro << endl;
}

//_______________________________________________________________________
void SetIntValue(TEnv & env, const Char_t * name, Int_t value)
{
   env.SetValue(name, Form("%d", value));
}

//_______________________________________________________________________

void HistPresent::SaveOptions()
{
   TEnv env(".rootrc");
   SetIntValue(env, "HistPresent.WindowXWidth_2dim", fWinwidx_2dim);
   SetIntValue(env, "HistPresent.WindowYWidth_2dim", fWinwidy_2dim);
   SetIntValue(env, "HistPresent.WindowXWidth_1dim", fWinwidx_1dim);
   SetIntValue(env, "HistPresent.WindowYWidth_1dim", fWinwidy_1dim);
   SetIntValue(env, "HistPresent.WindowXShift", fWinshiftx);
   SetIntValue(env, "HistPresent.WindowYShift", fWinshifty);
   SetIntValue(env, "HistPresent.WindowX", fWintopx);
   SetIntValue(env, "HistPresent.WindowY", fWintopy);
   SetIntValue(env, "HistPresent.WindowXWidth_List", fWinwidx_hlist);
   SetIntValue(env, "HistPresent.ShowErrors", fShowErrors);
   SetIntValue(env, "HistPresent.DrawAxisAtTop", fDrawAxisAtTop);
   SetIntValue(env, "HistPresent.Fill1Dim", fFill1Dim);
   SetIntValue(env, "HistPresent.2DimBackgroundColor", f2DimBackgroundColor);
   SetIntValue(env, "HistPresent.1DimFillColor", f1DimFillColor);
   SetIntValue(env, "HistPresent.StatBoxFont", fStatFont);
   SetIntValue(env, "HistPresent.TitleBoxFont", fTitleFont);
   SetIntValue(env, "HistPresent.ShowContour", fShowContour);
   SetIntValue(env, "HistPresent.OptStat", fOptStat);
   SetIntValue(env, "HistPresent.ShowDateBox", fShowDateBox);
   SetIntValue(env, "HistPresent.ShowStatBox", fShowStatBox);
   SetIntValue(env, "HistPresent.UseTimeOfDisplay", fUseTimeOfDisplay);
   SetIntValue(env, "HistPresent.ShowTitle", fShowTitle);
   SetIntValue(env, "HistPresent.EnableCalibration", fEnableCalibration);
   SetIntValue(env, "HistPresent.ShowFitBox", fShowFitBox);
   SetIntValue(env, "HistPresent.ShowFittedCurves", fShowFittedCurves);
   SetIntValue(env, "HistPresent.AutoShowPSFile", fShowPSFile);
   SetIntValue(env, "HistPresent.ShowListsOnly", fShowListsOnly);
   SetIntValue(env, "HistPresent.RememberTreeHists", fRememberTreeHists);
   SetIntValue(env, "HistPresent.RememberLastSet", fRememberLastSet);
   SetIntValue(env, "HistPresent.RememberZoom", fRememberZoom);
   SetIntValue(env, "HistPresent.UseAttributeMacro", fUseAttributeMacro);
   SetIntValue(env, "HistPresent.ShowAllAsFirst",    fShowAllAsFirst);
   SetIntValue(env, "HistPresent.UseRegexp", fUseRegexp);
   SetIntValue(env, "HistPresent.FitOptLikelihood", fFitOptLikelihood);
   SetIntValue(env, "HistPresent.FitOptQuiet", fFitOptQuiet);
   SetIntValue(env, "HistPresent.FitOptVerbose", fFitOptVerbose);
   SetIntValue(env, "HistPresent.FitOptMinos", fFitOptMinos);
   SetIntValue(env, "HistPresent.FitOptErrors1", fFitOptErrors1);
   SetIntValue(env, "HistPresent.FitOptIntegral", fFitOptIntegral);
   SetIntValue(env, "HistPresent.FitOptNoDraw", fFitOptNoDraw);
   SetIntValue(env, "HistPresent.FitOptAddAll", fFitOptAddAll);
   SetIntValue(env, "HistPresent.FitOptKeepPara", fFitOptKeepParameters);
   SetIntValue(env, "HistPresent.FitOptUseLinBg", fFitOptUseLinBg);
   SetIntValue(env, "HistPresent.MaxListEntries", fMaxListEntries);
   SetIntValue(env, "HistPresent.DisplayCalibrated", fDisplayCalibrated);
   SetIntValue(env, "HistPresent.AutoExec_1", fAutoExec_1);
   SetIntValue(env, "HistPresent.AutoExec_2", fAutoExec_2);
   SetIntValue(env, "HistPresent.AutoProj_X", fAutoProj_X);
   SetIntValue(env, "HistPresent.AutoProj_Y", fAutoProj_Y);
//  char options
   env.SetValue("HistPresent.ColorPalette", f2DimColorPalette->Data());
   env.SetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim->Data());
   env.SetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   SetIntValue(env, "HistPresent.SocketToConnect", fSocketToConnect);
//  double
//   env.SetValue("HistPresent.ProjectBothRatio"   ,kEnvUser);
//   env.SetValue("HistPresent.LogScaleMin"        ,kEnvUser);
//   env.SetValue("HistPresent.AutoUpdateDelay"    ,kEnvUser);
   env.SetValue("HistPresent.ProjectBothRatio", fProjectBothRatio);
   env.SetValue("HistPresent.LogScaleMin", fLogScaleMin);
   env.SetValue("HistPresent.AutoUpdateDelay", fAutoUpdateDelay);
   env.SetValue("HistPresent.fPeakMwidth", fPeakMwidth);
   env.SetValue("HistPresent.fPeakThreshold", fPeakThreshold);
   env.SetValue("HistPresent.LiveStat1dim", fLiveStat1dim);
   env.SetValue("HistPresent.LiveStat2dim", fLiveStat2dim);
   env.SetValue("HistPresent.LiveGauss", fLiveGauss);
   env.SetValue("HistPresent.LiveBG", fLiveBG);

   env.SaveLevel(kEnvUser);
}

//_______________________________________________________________________

void HistPresent::Set1DimOptions(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 7;
   enum e_opt { e_contour, e_filled, e_errors, e_axisattop,
                e_livestat, e_livegauss,e_livebg };
   const char *opt[] = {
      "Show contour",
      "Fill histogram",
      "Show error bars",
      "Extra Xaxis (channels) at top",
      "Show live statbox when dragging mouse",
      "Do live Gauss fit",
      "Use linear background in live fit"
   };

   TArrayI flags(nopt);
   TOrdCollection *svalues = new TOrdCollection();
   for (Int_t i = 0; i < nopt; i++) {
      svalues->Add(new TObjString(opt[i]));
      flags[i] = 0;
      if (i == e_contour && fShowContour)
         flags[i] = 1;
      else if (i == e_filled && fFill1Dim)
         flags[i] = 1;
      else if (i == e_errors && fShowErrors)
         flags[i] = 1;
      else if (i == e_axisattop && fDrawAxisAtTop)
         flags[i] = 1;
      else if (i == e_livestat && fLiveStat1dim)
         flags[i] = 1;
      else if (i == e_livegauss && fLiveGauss)
         flags[i] = 1;
      else if (i == e_livebg && fLiveBG)
         flags[i] = 1;
   }
   Int_t retval;
   Int_t itemwidth = 240;
   new TGMrbTableFrame(win, &retval,
                      "1 dim options", itemwidth,
                       1, nopt,
                       svalues, 0, 0, &flags);
   if (retval < 0) {
//      cout << "canceled" << endl;
      return;
   }
   fShowContour = 0;
   fFill1Dim = 0;
   fShowErrors = 0;
   fDrawAxisAtTop = 0;
   fLiveStat1dim = 0;
   fLiveGauss = 0;
   fLiveBG = 0;
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0) {
         if (i == e_contour)
            fShowContour = 1;
         else if (i == e_errors)
            fShowErrors = 1;
         else if (i == e_filled)
            fFill1Dim = 1;
         else if (i == e_axisattop)
            fDrawAxisAtTop = 1;
         else if (i == e_livestat) 
           fLiveStat1dim = 1;
         else if (i == e_livegauss) 
           fLiveGauss = 1;
         else if (i == e_livebg) 
           fLiveBG = 1;
      }
   }
   SaveOptions();
   if (fh) {
      fh->SetSelectedPad();
      TString drawopt;
      TH1 *hi = fh->GetSelHist();
      if (fShowContour)
         drawopt = "";
      if (fShowErrors)
         drawopt += "e1";
      if (fFill1Dim) {
         hi->SetFillStyle(1001);
         hi->SetFillColor(44);
      } else
         hi->SetFillStyle(0);
      hi->SetDrawOption(drawopt.Data());
   }
}

//_______________________________________________________________________
void HistPresent::Set2DimOptions(TGWindow * win, FitHist * fh)
{
   const Int_t nopt = 24;
   enum drawopt2 { e_scat, e_box, e_cont0, e_contz, e_cont1, e_cont2,
          e_cont3,
      e_col, e_colz, e_lego1, e_lego2, e_lego3,
      e_surf1, e_surf2, e_surf3, e_surf4, e_text,
      e_arr, e_livestat, e_fb, e_bb
   };
   const char *drawopt2[] =
       { "scat", "box", "cont0", "contz", "cont1", "cont2", "cont3",
      "col", "COLZ", "lego1", "lego2", "lego3",
      "surf1", "surf2", "surf3", "surf4",
      "text", "arr", "BB", "FB", "MONO", "MINV", "REGB"
   };
   const char *gDrawOpt2Text[] = {
      "Scatter pixels",
      "Boxes",
      "Contour, filled",
      "Contour, filled, with scale",
      "Contour, colored lines",
      "Contour, styled lines",
      "Contour, b/w lines",
      "Color levels",
      "Color levels, with scale",
      "Lego plot, colored shading",
      "Lego plot, colored level",
      "Lego plot, b/w",
      "Surface, colored,  mesh",
      "Surface, colored, no mesh",
      "Surface + Contour",
      "Surface, Gourand shading",
      "Numbers",
      "Arrows indicating gradient",
      "Show live statbox when dragging mouse",
      "Dont show back box",
      "Dont show front box",
      "Monochrome levels, highest=white",
      "Monochrome levels, highest=black",
      "Rainbow colors"
   };
   TArrayI flags(nopt);
   TOrdCollection *svalues = new TOrdCollection();
   for (Int_t i = 0; i < nopt; i++) {
      if (fDrawOpt2Dim->Contains(drawopt2[i]))
         flags[i] = 1;
      else
         flags[i] = 0;
      svalues->Add(new TObjString(gDrawOpt2Text[i]));
   }
   if (f2DimColorPalette->Contains("REGB"))
      flags[nopt - 1] = 1;
   else if (f2DimColorPalette->Contains("MINV"))
      flags[nopt - 2] = 1;
   else if (f2DimColorPalette->Contains("MONO"))
      flags[nopt - 3] = 1;
   flags[nopt - 6] = fLiveStat2dim;
   Int_t retval;
   Int_t itemwidth = 240;
   new TGMrbTableFrame(win, &retval,
   						  "How to show a 2dim hist",
   						  itemwidth, 1, nopt,
   						  svalues, 0, 0, &flags,
   						  nopt - 6);
   if (retval < 0) {
//      cout << "canceled" << endl;
      return;
   }
   for (Int_t i = 0; i < nopt - 6; i++) {
      if (flags[i] != 0)
         *fDrawOpt2Dim = drawopt2[i];
   }
   fLiveStat2dim = flags[nopt - 6];

   if (flags[nopt - 5] != 0)
      *fDrawOpt2Dim += "BB";
   if (flags[nopt - 4] != 0)
      *fDrawOpt2Dim += "FB";

   if (flags[nopt - 3] != 0) {
      *f2DimColorPalette = "MONO";
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPalette;
//      gStyle->SetPalette(fNofGreyLevels, fGreyPalette);
      cout << "Setting MONO " << endl;
   } else if (flags[nopt - 2] != 0) {
      *f2DimColorPalette = "MINV";
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPaletteInv;

//      gStyle->SetPalette(fNofGreyLevels, fGreyPaletteInv);
      cout << "Setting MINV " << endl;
   } else if (flags[nopt - 1] != 0) {
      *f2DimColorPalette = "REGB";
      fNofColorLevels = 1;
      fPalette = 0;

//      gStyle->SetPalette(1, 0); // violet - red 
   } else {
      *f2DimColorPalette = "DEFA";
      fNofColorLevels = 50;
      fPalette = 0;
 //     gStyle->SetPalette(50, 0);	// default palette
   }
      gStyle->SetPalette(fNofColorLevels, fPalette);

   if (fh) {
      fh->SetSelectedPad();
//      cout << " fDrawOpt2Dim: " << fDrawOpt2Dim->Data() << endl;
      fh->GetSelHist()->SetDrawOption(fDrawOpt2Dim->Data());
   }
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetDisplayOptions(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 12;
   enum e_opt { e_title, e_date, e_display, e_statbox, e_name, e_entries,
      e_mean, e_rms, e_underflow, e_overflow, e_integral,
      e_fit, e_monochrome
   };
   Int_t ib_name = 1, ib_entries = 10,
       ib_mean = 100, ib_rms = 1000, ib_underflow = 10000, ib_overflow =
       100000, ib_integral = 1000000;
   const char *opt[] = {
      "Show histogram title",
      "Show date box",
      "Use time of display",
      "Show statistics box",
      "Show name of histogram",

      "Show number of entries",
      "Show mean",
      "Show rms",
      "Show underflow",
      "Show overflow",

      "Show integral",
      "Show fit parameters"
   };
   TArrayI flags(nopt);
   TOrdCollection *svalues = new TOrdCollection();
   for (Int_t i = 0; i < nopt; i++) {
      svalues->Add(new TObjString(opt[i]));
      flags[i] = 0;
      if (i == e_title && fShowTitle)
         flags[i] = 1;
      else if (i == e_date && fShowDateBox)
         flags[i] = 1;
      else if (i == e_display && fUseTimeOfDisplay)
         flags[i] = 1;
      else if (i == e_statbox && fShowStatBox)
         flags[i] = 1;
      else if (i == e_name && (fOptStat / ib_name) % 10)
         flags[i] = 1;
      else if (i == e_entries && (fOptStat / ib_entries) % 10)
         flags[i] = 1;
      else if (i == e_mean && (fOptStat / ib_mean) % 10)
         flags[i] = 1;
      else if (i == e_rms && (fOptStat / ib_rms) % 10)
         flags[i] = 1;
      else if (i == e_underflow && (fOptStat / ib_underflow) % 10)
         flags[i] = 1;
      else if (i == e_overflow && (fOptStat / ib_overflow) % 10)
         flags[i] = 1;
      else if (i == e_integral && (fOptStat / ib_integral) % 10)
         flags[i] = 1;
      else if (i == e_fit && fShowFitBox)
         flags[i] = 1;
   }
   Int_t retval;
   Int_t itemwidth = 240;
   new TGMrbTableFrame(win, &retval,
   						  "What elements to show",
   						  itemwidth, 1, nopt,
   						  svalues, 0, 0, &flags);
   if (retval < 0) {
//      cout << "canceled" << endl;
      return;
   }
   fOptStat = 0;
   fShowTitle = 0;
   fShowDateBox = 0;
   fUseTimeOfDisplay = 0;
   fShowStatBox = 0;
   fShowFitBox = 0;
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0) {
         if (i == e_title)
            fShowTitle = 1;
         else if (i == e_date)
            fShowDateBox = 1;
         else if (i == e_display)
            fUseTimeOfDisplay = 1;
         else if (i == e_statbox)
            fShowStatBox = 1;
         else if (i == e_entries)
            fOptStat += ib_entries;
         else if (i == e_name)
            fOptStat += ib_name;
         else if (i == e_mean)
            fOptStat += ib_mean;
         else if (i == e_rms)
            fOptStat += ib_rms;
         else if (i == e_underflow)
            fOptStat += ib_underflow;
         else if (i == e_overflow)
            fOptStat += ib_overflow;
         else if (i == e_integral)
            fOptStat += ib_integral;
         else if (i == e_fit)
            fShowFitBox = 1;
      }
   }

   if (fShowStatBox) {
      gStyle->SetOptStat(fOptStat);
      gStyle->SetOptFit(fShowFitBox);
   } else {
      gStyle->SetOptFit(0);
      gStyle->SetOptStat(0);
   }
   gStyle->SetOptTitle(fShowTitle);
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetFittingOptions(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 11;
   enum e_opt { e_likeli,
      e_quiet,
      e_verbose,
      e_minos,
      e_errors,
      e_integral,
      e_nodraw,
      e_addall,
      e_keep,
      e_uselinbg,
      e_fitted
   };
   const char *opt[] = {
      "Use Loglikelihood method",
      "Quiet, minimal printout",
      "Verbose printout",
      "Use Minos to improve fit",
      "Set all errors to 1",
      "Use Integral of function in bin",
      "Dont draw result function",
      "Add all fitted functions to hist",
      "Keep parameters for next fit",
      "Use fixed linear background",
      "Display compents of fit function"
   };
   TArrayI flags(nopt);
   TOrdCollection *svalues = new TOrdCollection();
   for (Int_t i = 0; i < nopt; i++) {
      svalues->Add(new TObjString(opt[i]));
      flags[i] = 0;
      if (i == e_likeli && fFitOptLikelihood)
         flags[i] = 1;
      else if (i == e_quiet && fFitOptQuiet)
         flags[i] = 1;
      else if (i == e_verbose && fFitOptVerbose)
         flags[i] = 1;
      else if (i == e_minos && fFitOptMinos)
         flags[i] = 1;
      else if (i == e_errors && fFitOptErrors1)
         flags[i] = 1;
      else if (i == e_integral && fFitOptIntegral)
         flags[i] = 1;
      else if (i == e_nodraw && fFitOptNoDraw)
         flags[i] = 1;
      else if (i == e_addall && fFitOptAddAll)
         flags[i] = 1;
      else if (i == e_keep && fFitOptKeepParameters)
         flags[i] = 1;
      else if (i == e_uselinbg && fFitOptUseLinBg)
         flags[i] = 1;
      else if (i == e_fitted && fShowFittedCurves)
         flags[i] = 1;
   }
   Int_t retval;
   Int_t itemwidth = 240;
   new TGMrbTableFrame(win, &retval,
                       "Fitting Options",
                       itemwidth, 1, nopt,
                       svalues, 0, 0, &flags);
   if (retval < 0) {
//      cout << "canceled" << endl;
      return;
   }
   fFitOptLikelihood = 0;
   fFitOptQuiet = 0;
   fFitOptVerbose = 0;
   fFitOptMinos = 0;
   fFitOptErrors1 = 0;
   fFitOptIntegral = 0;
   fFitOptNoDraw = 0;
   fFitOptAddAll = 0;
   fFitOptKeepParameters = 0;
   fFitOptUseLinBg = 0;
   fShowFittedCurves = 0;
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0) {
         if (i == e_likeli)
            fFitOptLikelihood = 1;
         else if (i == e_quiet)
            fFitOptQuiet = 1;
         else if (i == e_verbose)
            fFitOptVerbose = 1;
         else if (i == e_minos)
            fFitOptMinos = 1;
         else if (i == e_errors)
            fFitOptErrors1 = 1;
         else if (i == e_integral)
            fFitOptIntegral = 1;
         else if (i == e_nodraw)
            fFitOptNoDraw = 1;
         else if (i == e_addall)
            fFitOptAddAll = 1;
         else if (i == e_keep)
            fFitOptKeepParameters = 1;
         else if (i == e_uselinbg)
            fFitOptUseLinBg = 1;
         else if (i == e_fitted)
            fShowFittedCurves = 1;
      }
   }
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetVariousOptions(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 14;
   enum e_opt { e_listsonly, e_psfile, e_enablecal, e_displaycal, 
      e_fitted, e_treehists, e_savelast,
      e_savezoom, e_useattr, e_allasfirst, e_useregexp, e_auto_1, e_auto_2,
      e_auto_x, e_auto_y
   };
   const char *opt[] = {
      "Show histlists only",
      "Show PS file after creation",
      "Enable calibration",
      "Auto Display calibrated hist",
      "Display compents of fit function",
      "Remember hist limits if showing trees",
      "Remember Expand settings (Marks)",
      "Remember Zoomings (by left mouse)",
      "Use Attribute Macro",
      "In Show Selected: Show All As First",
      "Use Regular expression syntax",
      "Auto exec macro 1-dim",
      "Auto exec macro 2-dim",
      "Auto exec project X",
      "Auto exec project Y"
   };
// *INDENT-OFF* 
   const char helptext[] = 
"\n\
With sockets, show lists only\n\
-----------------------------\n\
If very many histograms are used this option allows\n\
to display user defined lists of histograms only.\n\
This avoids fetching the statistics of all histograms\n\
when tables are assembled.\n\
\n\
Show PS file after creation\n\
---------------------------\n\
Automatically invoke ghostview when a PostScript file is\n\
generated.\n\
____________________________________________________________\n\
Enable calibration\n\
------------------\n\
The X - scale of of a 1-dim histogram may be recalibrated\n\
providing (more than 1) x-y-points. If this option is active\n\
the user is asked after a fit to a peak if the mean value \n\
should be added to the list of calibration points.\n\
_____________________________________________________________\n\
Auto Display calibrated hist\n\
----------------------------\n\
Display automatically a calibrated histogram if calibration\n\
data are storteed in defaulsts file\n\
_____________________________________________________________\n\
\n\
Draw a fitted curve into the histogram.\n\
_____________________________________________________________\n\
Display fitted curves\n\
---------------------\n\
Draw a fitted curve into the histogram.\n\
______________________________________________________________\n\
Remember hist limits if showing trees\n\
-------------------------------------\n\
When displaying trees (ntuples) limits for histograms \n\
(number of channels, lower, upper edge), these values\n\
may be remembered for each tree entry between sessions.\n\
______________________________________________________________\n\
Remember Expand settings (Marks) \n\
-----------------------------------\n\
Using marks expanded parts of hists may be shown. This\n\
option allows to pass these to later sessions.\n\
______________________________________________________________\n\
Remember Zoomings (by left mouse)\n\
---------------------------------\n\
Pressing the left mouse button in the scale of a histogram\n\
dragging to the required limit allows to zoom in the picture.\n\
This option allows to pass these limits to later sessions.\n\
______________________________________________________________\n\
Use Attribute Macro \n\
-------------------\n\
Each time a histogram is display a macro (FH_setdefaults.C)\n\
may be executed. This option can be switched on or off.\n\
______________________________________________________________\n\
Use Regular expression syntax\n\
------------------------------\n\
Normally wild card syntax (e.g. ls *.root to list all\n\
files ending with .root) is used in file/histo selection\n\
masks. One may switch to the more powerful Regular expression.\n\
For details consult a book on Unix.\n\
______________________________________________________________ \n\
Auto exec macro 1-dim:\n\
---------------------\n\
A macro (default name auto_exec_1.C) is executed when the left\n\
mouse button is pressed in a pad containing a 1-dim histogram. \n\
The default behaviour is as follows:\n\
   In a pad with several hists: display this hist enlarged \n\
   On a histogram contour:      print info for this channel\n\
   In the statistics box:       print full statistics\n\
   On a function:               print its parameters.\n\
_______________________________________________________________\n\
Auto exec macro 2-dim:\n\
---------------------\n\
A macro (default name auto_exec_2.C) is executed when the left\n\
mouse button is pressed in a pad containing a 2-dim histogram. \n\
The default behaviour is as follows:\n\
   In a pad with several hists: display this hist enlarged \n\
\n\
   When the mouse is moved:     \n\
   Open a new window and display a histogram containing the\n\
   projection on the x-axis taking only the channels with the\n\
   y-values pointed to by the mouse.\n\
   This feature can be switched by the options:\n\
\n\
Auto exec project X \n\
and\n\
Auto exec project Y \n\
\n\
";
// *INDENT-ON* 

   TArrayI flags(nopt);
   TOrdCollection *svalues = new TOrdCollection();
   for (Int_t i = 0; i < nopt; i++) {
      svalues->Add(new TObjString(opt[i]));
      flags[i] = 0;
      if (i == e_enablecal && fEnableCalibration)
         flags[i] = 1;
      else if (i == e_displaycal && fDisplayCalibrated)
         flags[i] = 1;
      else if (i == e_listsonly && fShowListsOnly)
         flags[i] = 1;
      else if (i == e_psfile && fShowPSFile)
         flags[i] = 1;
      else if (i == e_fitted && fShowFittedCurves)
         flags[i] = 1;
      else if (i == e_treehists && fRememberTreeHists)
         flags[i] = 1;
      else if (i == e_savelast && fRememberLastSet)
         flags[i] = 1;
      else if (i == e_savezoom && fRememberZoom)
         flags[i] = 1;
      else if (i == e_useattr && fUseAttributeMacro)
         flags[i] = 1;
      else if (i == e_allasfirst && fShowAllAsFirst)
         flags[i] = 1;
      else if (i == e_useregexp && fUseRegexp)
         flags[i] = 1;
      else if (i == e_auto_1 && fAutoExec_1)
         flags[i] = 1;
      else if (i == e_auto_2 && fAutoExec_2)
         flags[i] = 1;
      else if (i == e_auto_x && fAutoProj_X)
         flags[i] = 1;
      else if (i == e_auto_y && fAutoProj_Y)
         flags[i] = 1;
   }
   Int_t retval;
   Int_t itemwidth = 240;
   new TGMrbTableFrame(win, &retval,
   						  "Various Options",
   						  itemwidth, 1, nopt,
   						  svalues, 0, 0, &flags, 0,
                       helptext);
   if (retval < 0) {
//      cout << "canceled" << endl;
      return;
   }

   fEnableCalibration = 0;
   fDisplayCalibrated = 0;
   fShowFittedCurves = 0;
   fShowPSFile = 0;
   fShowListsOnly = 0;
   fRememberTreeHists = 0;
   fRememberLastSet = 0;
   fRememberZoom = 0;
   fUseAttributeMacro = 0;
   fShowAllAsFirst = 0;
   fUseRegexp = 0;
   fAutoExec_1 = 0;
   fAutoExec_2 = 0;
   fAutoProj_X = 0;
   fAutoProj_Y = 0;
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0) {
         if (i == e_enablecal)
            fEnableCalibration = 1;
         else if (i == e_displaycal)
             fDisplayCalibrated= 1;
         else if (i == e_psfile)
            fShowPSFile = 1;
         else if (i == e_listsonly)
            fShowListsOnly = 1;
         else if (i == e_fitted)
            fShowFittedCurves = 1;
         else if (i == e_treehists)
            fRememberTreeHists = 1;
         else if (i == e_savelast)
            fRememberLastSet = 1;
         else if (i == e_savezoom)
            fRememberZoom = 1;
         else if (i == e_useattr)
            fUseAttributeMacro = 1;
         else if (i == e_allasfirst)
            fShowAllAsFirst = 1;
         else if (i == e_useregexp)
            fUseRegexp = 1;
         else if (i == e_auto_1)
            fAutoExec_1 = 1;
         else if (i == e_auto_2)
            fAutoExec_2 = 1;
         else if (i == e_auto_x)
            fAutoProj_X = 1;
         else if (i == e_auto_y)
            fAutoProj_Y = 1;
      }
   }
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetWindowSizes(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 10;
//   Double_t *values = new Double_t[nopt];
   TArrayD values(nopt);
   TOrdCollection *row_lab = new TOrdCollection();
//   TList *col_lab = new TList();
//   col_lab->Add(new TObjString("X value"));
//   col_lab->Add(new TObjString("Y value"));

   Int_t vp = 0;
   row_lab->Add(new TObjString("Window_X_Width_1dim"));
   row_lab->Add(new TObjString("Window_Y_Width_1dim"));
   row_lab->Add(new TObjString("Window_X_Width_2dim"));
   row_lab->Add(new TObjString("Window_Y_Width_2dim"));
   row_lab->Add(new TObjString("Window_Shift_X"));
   row_lab->Add(new TObjString("Window_Shift_Y"));
   row_lab->Add(new TObjString("Window_Top_X"));
   row_lab->Add(new TObjString("Window_Top_Y"));
   row_lab->Add(new TObjString("Window_X_Width_List"));
   row_lab->Add(new TObjString("Project_Both_Ratio"));

   values[vp++] = fWinwidx_1dim;
   values[vp++] = fWinwidy_1dim;
   values[vp++] = fWinwidx_2dim;
   values[vp++] = fWinwidy_2dim;
   values[vp++] = fWinshiftx;
   values[vp++] = fWinshifty;
   values[vp++] = fWintopx;
   values[vp++] = fWintopy;
   values[vp++] = fWinwidx_hlist;
   values[vp++] = fProjectBothRatio;
   Int_t ret, itemwidth = 240, precission = 5;
   TGMrbTableOfDoubles(win, &ret, "Default window sizes and positions",
                       itemwidth, 1, nopt, values, precission, 0, row_lab);
   if (ret >= 0) {
      vp = 0;
      fWinwidx_1dim = (Int_t) values[vp++];
      fWinwidy_1dim = (Int_t) values[vp++];
      fWinwidx_2dim = (Int_t) values[vp++];
      fWinwidy_2dim = (Int_t) values[vp++];
      fWinshiftx = (Int_t) values[vp++];
      fWinshifty = (Int_t) values[vp++];
      fWintopx = (Int_t) values[vp++];
      fWintopy = (Int_t) values[vp++];
      fWinwidx_hlist = (Int_t) values[vp++];
      fProjectBothRatio = values[vp];
      fWincury = fWintopy;
      fWincurx = fWintopx;
   }
   SaveOptions();
}

//___________________________________________________________________________________________

void HistPresent::SetNumericalOptions(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 5;
//   Double_t *values = new Double_t[nopt];
   TArrayD values(nopt);
   TOrdCollection *row_lab = new TOrdCollection();

   Int_t vp = 0;
   row_lab->Add(new TObjString("LogScale_Minimum"));
   row_lab->Add(new TObjString("AutoUpdateDelay"));
   row_lab->Add(new TObjString("Max_Entries_in_HistList"));
   row_lab->Add(new TObjString("Width of response func in fpeak"));
   row_lab->Add(new TObjString("Threshold in fpeak"));

   values[vp++] = fLogScaleMin;
   values[vp++] = fAutoUpdateDelay;
   values[vp++] = fMaxListEntries;
   values[vp++] = fPeakMwidth;
   values[vp++] = fPeakThreshold;

;
//   Int_t ret=TGMrbTableOfDoubles(win, title, 1, nopt, values,
//                            0,row_lab);
   Int_t ret, itemwidth = 240, precission = 5;
   TGMrbTableOfDoubles(win, &ret, "Various numerical options", itemwidth,
                       1, nopt, values, precission, 0, row_lab);
   if (ret >= 0) {
      vp = 0;
      fLogScaleMin     = values[vp++];
      fAutoUpdateDelay = values[vp++];
      fMaxListEntries  = (Int_t)values[vp++];
      fPeakMwidth      = (Int_t)values[vp++];
      fPeakThreshold   = values[vp++];
   }
   SaveOptions();
}

//___________________________________________________________________________________________

void HistPresent::SetFontsAndColors(TGWindow * win, FitHist * fh)
{
   Int_t nopt = 4;
//   Double_t *values = new Double_t[nopt];
   TArrayD values(nopt);
   TOrdCollection *row_lab = new TOrdCollection();

   Int_t vp = 0;
   row_lab->Add(new TObjString("1_Dim_FillColor"));
   row_lab->Add(new TObjString("2_Dim_BackgroundColor"));
   row_lab->Add(new TObjString("StatBox font"));
   row_lab->Add(new TObjString("TitleBox font"));

   values[vp++] = f1DimFillColor;
   values[vp++] = f2DimBackgroundColor;
   values[vp++] = fStatFont;
   values[vp++] = fTitleFont;
   Int_t ret, itemwidth = 240, precission = 5;
   TGMrbTableOfDoubles(win, &ret, "Fonts and Colors", itemwidth, 1, nopt,
                       values, precission, 0, row_lab);
   if (ret >= 0) {
      vp = 0;
      f1DimFillColor = (Int_t) values[vp++];
      if (values[vp] != f2DimBackgroundColor) {
         f2DimBackgroundColor = (Int_t) values[vp];
         if (fh && fh->Its2dim()) {
            HTCanvas *ch = fh->GetCanvas();
            if (f2DimBackgroundColor == 0) {
               ch->GetFrame()->SetFillStyle(0);
            } else {
               ch->GetFrame()->SetFillStyle(1001);
               ch->GetFrame()->SetFillColor(f2DimBackgroundColor);
            }
            fh->UpdateCanvas();
         }
      }
      vp++;
      fStatFont = (Int_t) values[vp++];
      fTitleFont = (Int_t) values[vp++];
//     force high precission
      fStatFont = fStatFont / 10 * 10 + 2;
      fTitleFont = fTitleFont / 10 * 10 + 2;
   }
//   if(values) delete [] values;
//   cout << "fStatFont " << fStatFont << endl;
   gStyle->SetStatFont(fStatFont);
   gStyle->SetTitleFont(fTitleFont);
   SaveOptions();
}

