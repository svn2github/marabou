// #include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"
#include "TEnv.h"
#include "TFrame.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TGMsgBox.h"
#include "TMath.h"
#include "TVirtualPad.h"
#include "TGMrbTableFrame.h"
#include "FitOneDimDialog.h"
#include "SetColor.h"
#include <iostream>
#ifdef MARABOUVERS
#include "FitHist.h"
#endif

using std::cout;
using std::endl;
//__________________________________________________________________________
//
//  globals needed for non member fitting functions
//Int_t    lNpeaks;
//Double_t lTailSide;   // if tail determines side: 1 left(low), -1 high(right)
//Float_t  lBinW;
static const Int_t kFix = 3;

//___________________________________________________________________________  
//___________________________________________________________________________

Double_t gaus_only(Double_t * x, Double_t * par)
{
/*
  gaus + linear background
  par[kFix +0]   gauss width
  par[kFix +1]   gauss0 constant
  par[kFix +2]   gauss0 mean
  par[kFix +3]   gauss1 constant
  ...
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];

   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t sigma   = par[kFix +0];
   if (sigma == 0)
      sigma = 1;               //  force widths /= 0
   for (Int_t i = 0; i < lNpeaks; i ++) {
      arg = (x[0] - par[kFix +2 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +1 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
//   cout << x[0] << " "<< par[kFix +0]<< " " << par[kFix +1]<< " " << par[kFix +2]<< " " << lBinW << " "<< fitval << endl;
   return lBinW * fitval;
}
//____________________________________________________________________________ 

Double_t gaus_only_vsig(Double_t * x, Double_t * par)
{
/*
  gaus + linear background
  par[kFix +0]   gauss0 constant
  par[kFix +1]   gauss0 mean
  par[kFix +2]   gauss width
  par[kFix +3]   gauss1 constant
  ...
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t sigma   = par[kFix +0];
   if (sigma == 0)
      sigma = 1;               //  force widths /= 0
   for (Int_t i = 0; i < lNpeaks; i ++) {
      sigma   = par[kFix +2 + 3 * i];
      if (sigma == 0) sigma = 1;               //  force widths /= 0
      arg = (x[0] - par[kFix +1 + 3 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +0 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}
//____________________________________________________________________________ 

Double_t gaus_cbg(Double_t * x, Double_t * par)
{
/*
  gaus + const background
  par[kFix +0]   back ground
  par[kFix +1]   gauss width
  par[kFix +2]   gauss0 constant
  par[kFix +3]   gauss0 mean
  par[kFix +4]   gauss1 constant
  ...
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);

   Double_t arg;
   Double_t fitval  = 0;
   Double_t sigma   = par[kFix +1];
   if (sigma == 0)
      sigma = 1;               //  force widths /= 0
   fitval = par[kFix +0];
   for (Int_t i = 0; i < lNpeaks; i ++) {
      arg = (x[0] - par[kFix +3 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +2 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}
//____________________________________________________________________________ 

Double_t gaus_cbg_vsig(Double_t * x, Double_t * par)
{
/*
  gaus + const background
  par[kFix +0]   back ground
  par[kFix +1]   gauss0 constant
  par[kFix +2]   gauss0 mean
  par[kFix +3]   gauss width
  par[kFix +4]   gauss1 constant
  ...
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
 
  static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval;
   Double_t sigma ;
   if (sigma == 0)
      sigma = 1;               //  force widths /= 0
   fitval = par[kFix +0];
   for (Int_t i = 0; i < lNpeaks; i ++) {
      sigma   = par[kFix +3 + 3 * i];
      if (sigma == 0) sigma = 1;               //  force widths /= 0
      arg = (x[0] - par[kFix +2 + 3 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +1 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}
//____________________________________________________________________________ 

Double_t gaus_lbg(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]   background constant
  par[kFix +1]   background slope
  par[kFix +2]   gauss width
  par[kFix +3]   gauss0 constant
  par[kFix +4]   gauss0 mean
  par[kFix +5]   gauss1 constant
  par[kFix +6]   gauss1 mean
  par[kFix +7]   gauss2 constant
  par[kFix +8]   gauss2 mean
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];

   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[kFix +0];
   Double_t bgslope = par[kFix +1];
   Double_t sigma   = par[kFix +2];
   if (sigma == 0) sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < lNpeaks; i ++) {
      arg = (x[0] - par[kFix +4 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}
//____________________________________________________________________________ 

Double_t gaus_lbg_vsig(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]   background constant
  par[kFix +1]   background slope
  par[kFix +2]   gauss0 constant
  par[kFix +3]   gauss0 mean
  par[kFix +4]   gauss0 width
  par[kFix +5]   gauss1 constant
  par[kFix +6]   gauss1 mean
  .....
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[kFix +0];
   Double_t bgslope = par[kFix +1];
   Double_t sigma;
   if (sigma == 0)
      sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < lNpeaks; i ++) {
      sigma = par[kFix +4 + 3 * i];
      if (sigma == 0) sigma = 1;               //  force widths /= 0
      arg = (x[0] - par[kFix +3 + 3 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[kFix +2 + 3 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}

//_____________________________________________________________________________ 

Double_t gaus_tail(Double_t * x, Double_t * par)
{
/*
  par[kFix +0]   tail fraction
  par[kFix +1]   tail width
  par[kFix +2]   background constant
  par[kFix +3]   background slope
  par[kFix +4]   gauss width
  par[kFix +5]   gauss0 constant
  par[kFix +6]   gauss0 mean
  par[kFix +7]   gauss1 constant
  par[kFix +8]   gauss1 mean
  par[kFix +9]   gauss2 constant
  par[kFix +10]  gauss2 mean
*/
   Double_t lBinW = par[0];
   Int_t lNpeaks = (Int_t)par[1];
   Double_t lTailSide = par[2];

   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t xij;
   Double_t arg;
   Double_t tail;
   Double_t g2b;
   Double_t err;
   Double_t norm;
   Double_t fitval = 0;
   Double_t tailfrac  = par[kFix +0];
   Double_t tailwidth = par[kFix +1];
   Double_t bgconst   = par[kFix +2];
   Double_t bgslope   = par[kFix +3];
   Double_t sigma     = par[kFix +4];

//  force widths /= 0
   if (tailwidth == 0)
      tailwidth = 1;
   if (sigma == 0)
      sigma = 1;

   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < lNpeaks; i++) {
      xij = (x[0] - par[kFix +6 + 2 * i]);
      arg = xij / (sqrt2 * sigma);
      xij *= lTailSide;
      tail = exp(xij / tailwidth) / tailwidth;
      g2b = 0.5 * sigma / tailwidth;
      err = 0.5 * tailfrac * par[kFix +5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
      norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
      fitval = fitval + norm * err * tail 
                      + par[kFix +5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return lBinW * fitval;
}

//____________________________________________________________________________________ 

Double_t tailf(Double_t * x, Double_t * par)
{
//  force widths /= 0
   Double_t lBinW = par[0];
//   Int_t lNpeaks = (Int_t)par[1];
   Double_t lTailSide = par[2];

   Double_t const0    = par[kFix +0];
   Double_t mean      = par[kFix +1];
   Double_t gauswidth = par[kFix +2];
   Double_t tailwidth = par[kFix +3];

   if (gauswidth == 0)
      gauswidth = 1;
   if (tailwidth == 0)
      tailwidth = 1;

   Double_t xij = (x[0] - mean) *lTailSide ;

   Double_t tail = exp(xij / tailwidth) / tailwidth;
   Double_t g2b = 0.5 * gauswidth / tailwidth;
   Double_t err = 0.5 * const0 * TMath::Erfc(xij / gauswidth + g2b);
   Double_t norm = exp(0.25 * pow(gauswidth, 2) / pow(tailwidth, 2));
   Double_t fitval = norm * err * tail;
   return lBinW * fitval;
}

//____________________________________________________________________________________ 

Double_t backf(Double_t * x, Double_t * par)
{
   Double_t lBinW = par[0];
//   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
   Double_t fitval = par[kFix +0] + x[0] * par[kFix +1];
   return lBinW * fitval;
}

//____________________________________________________________________________________ 

Double_t gausf(Double_t * x, Double_t * par)
{
   Double_t lBinW = par[0];
//   Int_t lNpeaks = (Int_t)par[1];
//   Double_t lTailSide = par[2];
   static Double_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   if (par[kFix +2] == 0)
      par[kFix +2] = 1;

   Double_t xij = x[0] - par[kFix +1];

   Double_t arggaus = xij / (sqrt2 * par[kFix +2]);
   Double_t fitval = par[kFix +0] * exp(-arggaus * arggaus) / (sqrt2pi * par[kFix +2]);
   return lBinW * fitval;
}
//____________________________________________________________________________________ 
//____________________________________________________________________________________ 

FitOneDimDialog::FitOneDimDialog(TH1 * hist, Int_t type)
{
   fSelHist = hist;
   fGraph = NULL;
   if (!fSelHist) {
      cout << "No hist selected" << endl;
      return;
   }

   if (fSelHist->GetDimension() != 1) {
      cout << "Can only be used with 1-dim hist" << endl;
      return;
   }
 //  lBinW   = fSelHist->GetBinWidth(1);
   fName = fSelHist->GetName();
   DisplayMenu(type);
}
//____________________________________________________________________________________ 

FitOneDimDialog::FitOneDimDialog(TGraph * graph, Int_t type)
{
   if (!graph) {
      cout << "No graph selected" << endl;
      return;
   }
   fGraph = graph;
   fSelHist = graph->GetHistogram();
   if (!fSelHist) {
      cout << "Graph must be drawn" << endl;
      return;
   }

   if (fSelHist->GetDimension() != 1) {
      cout << "Can only be used with 1-dim hist" << endl;
      return;
   }
//   lBinW   = 1;
   fName = fGraph->GetName();
   DisplayMenu(type);
}
//_________________________________________________________________________
//
// This widget allows fitting of any number of gaussian shaped peaks
// optionally with a low or high energy tail plus a linear background.
//
//_________________________________________________________________________
//____________________________________________________________________________________ 

void FitOneDimDialog::DisplayMenu(Int_t type)
{

static const Char_t helptext_gaus[] =
"This widget allows fitting of any number of \n\
gaussian shaped peaks optionally with a low or high\n\
energy tail plus a linear background.\n\
The linear background may be forced to zero (Force BG = 0)\n\
or to be constant(Force BG slope = 0)\n\
The gauss width may be forced to the same for all peaks\n\
In case with tail the parameter Ta_fract (tail\n\
fraction) is the contents of the tail part relative\n\
to the pure gaussian part. In case of more then one\n\
peak this fraction, the width of the tail and of the\n\
gaussian are the same for all peaks. Marks (use middle\n\
mouse button to define them) are used to provide \n\
lower and upper limits of the fit region and the positions\n\
of peaks to be fitted. So if 3 peaks should be fitted\n\
5 marks need to be defined \n\
The program estimates from this the start\n\
parameters of the fit which are presented and may be\n\
changed. Values may be fixed and bound. When using\n\
bounds please be aware of the warnings in the MINUIT\n\
manual on this. Fitted parameters may be kept as\n\
start for a new fit.\n\
A linear background predetermined from 2 regions in\n\
the spectrum outside the peaks can be incorporated\n\
in the following way:\n\
\n\
Define 4 marks to select the 2 regions. Then use the\n\
item 'Determine linear background' from the Fit\n\
popup menu to calculate the parameters const and\n\
slope of the linear background.In a subsequent fit \n\
the parameters Bg_Const and Bg_Slope will be fixed \n\
to the above calculated values.  If needed these \n\
values can still be changed manually. Unfixing them \n\
will let MINUIT vary their values.\n\
\n\
The quality of the start parameters of a fit determine\n\
the speed of convergence. Bad start parameters may lead\n\
MINUIT to find an unwanted minimum. Start parameters\n\
may be checked by drawing the fit function with these \n\
parameters without actually doing a fit.\n\
";

static const Char_t helptext_exp[] =
"Fit exponential with offset in x and y:\n\
f(x) = a + b * exp ((x -x0) * c)\n\
x0 is introduced for convenience to simplify\n\
determination of start parameters\n\
Function limits may be given by  modifying fFrom\n\
and fTo or by setting 2 marks\n\
\n\
";

static const Char_t helptext_pol[] =
"Fit polynomial f(x) = a0 + a1*x +a2*x*x ..)\n\
Degree must be >= 0, any degree is allowed\n\
but may be not meaningful\n\
Only a0, a1, a2, a3 may be given start values\n\
higher coefficients get start values = 0\n\
Function limits may be given by  modifying fFrom\n\
and fTo or by setting 2 marks\n\
";

static const Char_t helptext_form[] =
"Fit a user defined function:\n\
e.g.: [0]*sin([1]*(x+[2])\n\
Note: The number of parameters is defined\n\
implicitly by the formula. Maximimum 6 parameters are\n\
allowed, if more parameters are needed use a fit macro\n\
Function limits may be given by modifying fFrom\n\
and fTo explicitely or by setting 2 marks\n\
Examples:\n\
Error function with y offset [0], constant [1]\n\
x offset [2] and width[3]:\n\
[0] + [1]*TMath::Erf((x-[2]) / [3])\n\
\n\
Sum of sine functions producing a beat:\n\
[0] + [1]*sin([2]*x+ [3]) + [2] * sin([4]*x + [5])\n\
\n\
A BreitWigner with constant [0], mean [1] and gamma [2]\n\
[0] * TMath::BreitWigner(x,[1], [2])\n\
\n\
For a complete list of implemented functions consult\n\
the root doc at: http://root.cern.ch\n\
";

   fParentWindow = NULL; 

   fFuncNumber = 0;
   fLinBgConst = 0;
   fLinBgSlope = 0;
   fUsedbg     = 0;
   fUseoldpars = 0;
   fConstant   = 0;
   fMean       = 0;
   fMeanError  = 0;
   fAdded      = -1;
   RestoreDefaults();
   fSelPad = gPad;
   SetBit(kMustCleanup);
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
//  function name
//   fFuncName = fSelHist->GetName();
//   Int_t ip = fFuncName.Index(";");
//	if (ip > 0)fFuncName.Resize(ip);
   fFuncName = Form("_%d", fFuncNumber);
   fFuncNumber++;
//   fFuncName.Prepend("f");
//   const char hist_file[] = {"funcname_hist.txt"};
//   history = hist_file;
//   if (gROOT->GetVersionInt() < 40000) history = NULL;

   TList *row_lab = new TList(); 
   static void *valp[25];
   Int_t ind = 0;
   static TString exgcmd("FitGausExecute()");
   static TString expcmd("FitExpExecute()");
   static TString exdcmd("DrawExpExecute()");
   static TString expolcmd("FitPolExecute()");
   static TString exdpolcmd("DrawPolExecute()");
   static TString exformcmd("FitFormExecute()");
   static TString exdformcmd("DrawFormExecute()");
//   static TString accmd("AddToCalibration()");
   static TString lbgcmd("DetLinearBackground()");
   static TString clmcmd("ClearMarkers()");
   static TString setmcmd("SetMarkers()");
   static TString prtcmd("PrintMarkers()");
   static TString sfocmd("SetFittingOptions()");
   static Int_t dummy = 0;
   TString * text = NULL;
   const char * history = NULL;
   const char hist_file[] = {"func_formulae_hist.txt"};
//   history = hist_file;
//   if (gROOT->GetVersionInt() < 40000) history = NULL;

   fNpeaks = 1;
   fUseoldpars = 0;
   fLinBgSet = kFALSE;
   fMarkers = NULL; 
   TAxis *xaxis = fSelHist->GetXaxis();
   fFrom = xaxis->GetXmin();
   fTo   = xaxis->GetXmax() ;
//   fSelHist->Dump();
   const char * helptext = NULL;
   if (type == 1) {
      helptext = helptext_gaus;
      fFuncName.Prepend(fGausFuncName);
		GetMarkers();
		row_lab->Add(new TObjString("PlainIntVal_N Peaks"));
		valp[ind++] = &fNpeaks;
		row_lab->Add(new TObjString("CheckButton+Low Tail"));
		valp[ind++] = &fLowtail;
		row_lab->Add(new TObjString("CheckButton+High Tail"));
		valp[ind++] = &fHightail;
		row_lab->Add(new TObjString("CheckButton_Force Background=0"));
		valp[ind++] = &fBackg0;
		row_lab->Add(new TObjString("CheckButton+Force BG Slope=0"));
		valp[ind++] = &fSlope0;
		row_lab->Add(new TObjString("CheckButton_Common Gauss width"));
		valp[ind++] = &fOnesig;
		row_lab->Add(new TObjString("CheckButton+Use pre det lin bg"));
		valp[ind++] = &fUsedbg;
		row_lab->Add(new TObjString("CheckButton_Use pars of prev fit"));
		valp[ind++] = &fUseoldpars;
		row_lab->Add(new TObjString("CheckButton+Show components of fit"));
		valp[ind++] = &fShowcof;
   } else if (type == 2) {
      helptext = helptext_exp;
      fFuncName.Prepend(fExpFuncName);
      row_lab->Add(new TObjString("CommentOnly_Function: a + b*exp(c*(x-d))"));
		valp[ind++] = &dummy;
      row_lab->Add(new TObjString("DoubleValue_a"));
		valp[ind++] = &fExpA;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fExpFixA;
      row_lab->Add(new TObjString("DoubleValue-b"));
		valp[ind++] = &fExpB;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fExpFixB;
      row_lab->Add(new TObjString("DoubleValue_c"));
		valp[ind++] = &fExpC;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fExpFixC;
      row_lab->Add(new TObjString("DoubleValue+d"));
		valp[ind++] = &fExpD;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fExpFixD;

   } else if (type == 3) {
      helptext = helptext_pol;
      fFuncName.Prepend(fPolFuncName);
      row_lab->Add(new TObjString("CommentOnly_Pol: a0 + a1*x +.."));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("PlainIntVal+Degree"));
		valp[ind++] = &fPolN;
      row_lab->Add(new TObjString("DoubleValue_a0"));
		valp[ind++] = &fPolA;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fPolFixA;
      row_lab->Add(new TObjString("DoubleValue-a1"));
		valp[ind++] = &fPolB;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fPolFixB;
      row_lab->Add(new TObjString("DoubleValue_a2"));
		valp[ind++] = &fPolC;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fPolFixC;
      row_lab->Add(new TObjString("DoubleValue+a3"));
		valp[ind++] = &fPolD;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fPolFixD;
   } else if (type == 4) {
      helptext = helptext_form;
      fFuncName.Prepend(fFormFuncName);
      row_lab->Add(new TObjString("CommentOnly_User defined formula"));
		valp[ind++] = &dummy;
      row_lab->Add(new TObjString("DoubleValue_p0"));
		valp[ind++] = &fFormA;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixA;
      row_lab->Add(new TObjString("DoubleValue-p1"));
		valp[ind++] = &fFormB;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixB;
      row_lab->Add(new TObjString("DoubleValue_p2"));
		valp[ind++] = &fFormC;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixC;
      row_lab->Add(new TObjString("DoubleValue+p3"));
		valp[ind++] = &fFormD;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixD;
      row_lab->Add(new TObjString("DoubleValue_p4"));
		valp[ind++] = &fFormE;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixE;
      row_lab->Add(new TObjString("DoubleValue+p5"));
		valp[ind++] = &fFormF;
		row_lab->Add(new TObjString("CheckButton-Fix"));
		valp[ind++] = &fFormFixF;
   }
   row_lab->Add(new TObjString("DoubleValue_From"));
   valp[ind++] = &fFrom;
   row_lab->Add(new TObjString("DoubleValue+To"));
   valp[ind++] = &fTo;
   row_lab->Add(new TObjString("CheckButton_Add all functions to hist"));
   valp[ind++] = &fFitOptAddAll;
   row_lab->Add(new TObjString("CheckButton+Clear marks after fit"));
   valp[ind++] = &fAutoClearMarks;
   row_lab->Add(new TObjString("ColorSelect_LCol"));
   valp[ind++] = &fColor;
   row_lab->Add(new TObjString("PlainShtVal-LWid"));
   valp[ind++] = &fWidth;
   row_lab->Add(new TObjString("LineSSelect-LSty"));
   valp[ind++] = &fStyle;
   row_lab->Add(new TObjString("StringValue_FuncName"));
   valp[ind++] = &fFuncName;
   row_lab->Add(new TObjString("CommandButt_Execute Fitting"));
   if (type == 1) { 
      valp[ind++] = &exgcmd;
//      cout << exgcmd << endl;
      row_lab->Add(new TObjString("CommandButt+Determine Lin BG"));
      valp[ind++] = &lbgcmd;
   } else if (type == 2) {
      valp[ind++] = &expcmd;
      row_lab->Add(new TObjString("CommandButt+Draw only"));
      valp[ind++] = &exdcmd;
   } else if (type == 3) {
      valp[ind++] = &expolcmd;
      row_lab->Add(new TObjString("CommandButt+Draw only"));
      valp[ind++] = &exdpolcmd;
   } else if (type == 4) {
      valp[ind++] = &exformcmd;
      row_lab->Add(new TObjString("CommandButt+Draw only"));
      valp[ind++] = &exdformcmd;
      history = hist_file;
      text = &fFormula;
   }
/*
#ifdef MARABOUVERS
   if (fGraph == NULL) {
      row_lab->Add(new TObjString("CommandButt+Add To Calibration"));
      valp[ind++] = &accmd;
   }
#endif
*/
   row_lab->Add(new TObjString("CommandButt_Fitting Options"));
   valp[ind++] = &sfocmd;
   row_lab->Add(new TObjString("CommandButt+Print Markers"));
   valp[ind++] = &prtcmd;
   row_lab->Add(new TObjString("CommandButt_Clear Marks"));
   valp[ind++] = &clmcmd;
   if (type == 1) {
      row_lab->Add(new TObjString("CommandButt-Set N Marks"));
      valp[ind++] = &setmcmd;
      row_lab->Add(new TObjString("PlainIntVal-N"));
      valp[ind++] = &fNmarks;
   } else {
      row_lab->Add(new TObjString("CommandButt-Set 2 Marks"));
      fNmarks = 2;
      valp[ind++] = &setmcmd;
   }
   Int_t itemwidth = 320;
//   TRootCanvas* fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
//   cout << "fParentWindow " << fParentWindow << endl;
   Int_t ok = 0;
   fDialog =
   new TGMrbValuesAndText ("Function", text, &ok, itemwidth,
                      fParentWindow, history, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
//  Bool_t ok;
//  ok = GetStringExt("Function name", &fFuncName, itemwidth,
//                      fParentWindow, history, NULL, row_lab, valp,
//                      NULL, NULL, helptext, this, this->ClassName());
}
//__________________________________________________________________________

FitOneDimDialog::~FitOneDimDialog()
{
   gROOT->GetListOfCleanups()->Remove(this);
}
//__________________________________________________________________________

void FitOneDimDialog::RecursiveRemove(TObject * obj)
{
//   cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
//        << obj << " fSelHist " <<  fSelHist <<  endl;
   if ((fGraph && obj == fGraph) || obj == fSelHist) { 
 //      cout << "FitOneDimDialog::RecursiveRemove: this " << this << " obj "  
 //       << obj << " fSelHist " <<  fSelHist <<  endl;
      CloseDialog(); 
   }
}
//__________________________________________________________________________

void FitOneDimDialog::FitGausExecute()
{
   Int_t retval = 0;
   if (fGraph != NULL && fGraph->GetN() == 0) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning",
                "No histogram nore graph defined\n Use Draw only" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   static TArrayD * par = NULL;
   Int_t ind = 0;
   Double_t lTailSide = 0;
   if (fHightail == 1) {
      lTailSide = -1;
      if (fLowtail) {
         cout << "Tail side dubious, use low tail" << endl;
         lTailSide = 1;
      }
   } 
   if (fLowtail)
      lTailSide = 1;
   if (lTailSide != 0) {
/*
      if (fBackg0 != 0) {
         cout << "With tail ignore: Force Background = 0" << endl;
         fBackg0 = 0;
      }
      if (fSlope0 != 0) {
         cout << "With tail ignore: Force BG slope = 0" << endl;
         fSlope0 = 0;
      }
*/
      if (fOnesig == 0) {
         cout << "With tail ignore: Force common Gaus width" << endl;
         fOnesig = 1;
      }
   }
   if (fBackg0 != 0) {
      cout << "With Background == 0, Force BG slope = 0" << endl;
      fSlope0 = 1;
   }
   Double_t lBinW = fSelHist->GetBinWidth(1);
   Bool_t setpars = (fUseoldpars == 0);
//   if (setpars) cout << "ent setpars true" << endl;

   Int_t npars;
   if ( GetMarkers() <= 0 ) { 
      Int_t retval = 0;
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning", "No marks set,\n need at least 2" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   if (fOnesig == 0) 
      npars = fNpeaks * 3;
   else
      npars = 1 + fNpeaks * 2;
//   npars += kFix;
   if (fBackg0 == 0 || lTailSide != 0)  npars++;
   if ((fBackg0 == 0 && fSlope0 == 0) || lTailSide != 0)  npars++;
   if (lTailSide != 0) npars += 2;
   if (par == NULL) {
      par    = new TArrayD(3 * npars);
      setpars = kTRUE;
      if (setpars)  cout << "new setpars true: npars " << npars << endl;
   } else {
      if (par->GetSize() != 3 * npars) {
         par->Set(3 * npars);
         setpars = kTRUE;
         if (setpars)  cout << "size setpars true: npars "  << npars<< endl;;
      }
   }
//   cout << "npars: " << npars << endl;
//   cout << (*par)[0] << " " << (*par)[1] << " "  << (*par)[2] << endl;
   TArrayI * fbflags   = new TArrayI(2 * npars);
// get estimates
   TArrayD gpar(3);
   ind = 0;
   Int_t bin_from = fSelHist->FindBin(fFrom);
//   Int_t bin_to   = fSelHist->FindBin(fTo);
//   Int_t bin = 0; 
   TF1 * func;
   if (lTailSide) {
      func = new TF1(fFuncName, gaus_tail, fFrom, fTo, npars+kFix);
      cout << "lTailSide " << lTailSide<< endl;
   } else {
      if (fOnesig) {
         if (fBackg0 != 0) 
            func = new TF1(fFuncName, gaus_only, fFrom, fTo, npars+kFix);
         else if (fSlope0 != 0)
            func = new TF1(fFuncName, gaus_cbg,  fFrom, fTo, npars+kFix);
         else
            func = new TF1(fFuncName, gaus_lbg,  fFrom, fTo, npars+kFix); 
      } else {
         if (fBackg0 != 0) 
            func = new TF1(fFuncName, gaus_only_vsig, fFrom, fTo, npars+kFix);
         else if (fSlope0 != 0)
            func = new TF1(fFuncName, gaus_cbg_vsig,  fFrom, fTo, npars+kFix);
         else
            func = new TF1(fFuncName, gaus_lbg_vsig,  fFrom, fTo, npars+kFix); 
      }
   }
   func->SetParameter(0, lBinW);
   func->SetParameter(1, fNpeaks);
   func->SetParameter(2, lTailSide);
   func->SetParName(0, "BinW");
   func->SetParName(1, "Npeaks");
   func->SetParName(2, "TailSide");
// in case it was not incremented
   ind = npars;
// add bound and fixflags
   for (Int_t i = 0; i < 2 * npars; i++) {
      (*fbflags)[i] = 0;
      (*par)[ind++] = 0;
   }  
   ind = 0;
   TOrdCollection row_lab;
//   cout << par->GetSize() << endl;
   if (lTailSide != 0) {
      func->SetParName(ind+kFix,      "Ta_fract");
      row_lab.Add(new TObjString("Ta_fract"));
      if (setpars) (*par)[ind] = 1;
      ind++;
      func->SetParName(ind+kFix,      "Ta_width");
      row_lab.Add(new TObjString("Ta_width"));
      if (setpars) (*par)[ind] = 5;
      ind++;
      (*fbflags)[2] = fBackg0;
      (*fbflags)[3] = fSlope0;
   }
 
   Double_t bgest = fSelHist->GetBinContent(bin_from);
   if (fBackg0 == 0 || fUsedbg == 1|| lTailSide != 0) {
      func->SetParName(ind+kFix,      "Bg_Const");
      row_lab.Add(new TObjString("Bg_Const"));
      if (setpars) (*par)[ind] = bgest/fSelHist->GetBinWidth(bin_from);
      ind++;
   }
   if (fSlope0 == 0 || fUsedbg == 1 || lTailSide != 0) {
      func->SetParName(ind+kFix,      "Bg_Slope");
      row_lab.Add(new TObjString("Bg_Slope"));
      if (setpars) (*par)[ind] = 0;
      ind++;
   }
//   cout << "fNmarks " <<  fNmarks<< endl;
   TString lab; 
   Int_t ind_sigma = 0; 
   if (fOnesig == 1) {
//  use same sigma for all peaks
      func->SetParName(ind+kFix,      "Ga_Sigma_");
      row_lab.Add(new TObjString("Ga_Sigma_"));
      ind_sigma = ind;
      if (setpars) (*par)[ind] = -1;                        // Sigma
      ind++;
   }
   if (fNmarks == 2) {
// 2 marks only assume peak in between
//      bin = GetMaxBin(fSelHist, bin_from, bin_to);
      if (fGraph == NULL) 
         GetGaussEstimate(fSelHist, fFrom, fTo, bgest, gpar);
      else
         GetGaussEstimate(fGraph, fFrom, fTo, bgest, gpar);
      func->SetParName(ind+kFix,      "Ga_Const0");
      row_lab.Add(new TObjString("Ga_Const0"));
      if (setpars) (*par)[ind]     = gpar[0];
      ind++;
      if (lTailSide != 0) (*par)[ind -1] *= 0.5;
      func->SetParName(ind+kFix,      "Ga_Mean_0");
      row_lab.Add(new TObjString("Ga_Mean_0"));
      if (setpars) (*par)[ind]     = gpar[1];
      ind++;
      if (fOnesig == 1) {
         if (setpars) (*par)[ind_sigma] = gpar[2];
      } else {
         func->SetParName(ind+kFix,      "Ga_Sigma_");
         row_lab.Add(new TObjString("Ga_Sigma_"));
         if (setpars) (*par)[ind] = gpar[2];                // Sigma
         ind++;
      }

   } else {
//    look for peaks at intermediate marks
      for (Int_t i = 1; i < fNmarks - 1; i++) {
         Double_t mpos = ((FhMarker *) fMarkers->At(i))->GetX();
         Double_t mprev = ((FhMarker *) fMarkers->At(i-1))->GetX();
         Double_t mnext = ((FhMarker *) fMarkers->At(i+1))->GetX();
//         bin           = fSelHist->FindBin(mpos);
         Double_t d1 = (mpos-mprev);
         Double_t d2 = (mnext-mpos);
         d1 = 0.5 * TMath::Min(d1, d2);
         if (fGraph == NULL) 
            GetGaussEstimate(fSelHist, mpos-d1, mpos+d1, bgest, gpar);
         else
            GetGaussEstimate(fGraph, mpos-d1, mpos+d1, bgest, gpar);
         lab = "Ga_Const";
         lab += (i - 1);
         func->SetParName(ind+kFix,  lab.Data() );
         row_lab.Add(new TObjString(lab.Data()));
         if (setpars) (*par)[ind]     = gpar[0];
         ind++;
         lab = "Ga_Mean_";
         lab += (i - 1);
         func->SetParName(ind+kFix,  lab.Data() );
         row_lab.Add(new TObjString(lab.Data()));
         if (setpars) (*par)[ind]     = gpar[1];
         ind++;
         if (fOnesig == 1) {
            if (gpar[2] > (*par)[ind_sigma])         // use biggest sigma as est
               if (setpars) (*par)[ind_sigma] = gpar[2];
         } else {
            lab = "Ga_Sigma";
            lab += (i - 1);
            func->SetParName(ind+kFix,  lab.Data() );
            row_lab.Add(new TObjString(lab.Data()));
            if (setpars) (*par)[ind]     = gpar[2];
            ind++;
         }

      }
   }
   if (fUsedbg) {
//  use predermined linear background
      Int_t offset = 0;
      if (lTailSide != 0) offset = 2;
      (*par)[0 + offset] = fLinBgConst / lBinW;      
      (*par)[1 + offset] = fLinBgSlope / lBinW;  
      (*fbflags)[0 + offset] = 1;
      (*fbflags)[1 + offset] = 1;
   }  
   Int_t ncols = 3;
   TOrdCollection col_lab ;
   col_lab.Add(new TObjString("StartVal"));
   col_lab.Add(new TObjString("Low Lim "));
   col_lab.Add(new TObjString("Up Lim  "));
   col_lab.Add(new TObjString("fix it"));
   col_lab.Add(new TObjString("bound"));

   TString title("Start parameters");
   Int_t ret = 0; 
   Int_t itemwidth = 120;
   Int_t precission = 5;
//   row_lab.Print();

//   TGMrbTableOfDoubles(NULL, &ret, title.Data(), itemwidth,
   TGMrbTableOfDoubles(fParentWindow, &ret, title.Data(), itemwidth,
                       ncols, npars, *par, precission,
                       &col_lab, &row_lab, fbflags, 0,
                       NULL, "Do fit", "Draw only");
   if (ret < 0) return;
   Bool_t do_fit = (ret == 0);
   cout <<
       "-------------------------------------------------------------"
       << endl;
   if (ret == 0)
      cout << "Fitting Gauss ";
   else
      cout << "Drawing Gauss ";
   if (fSlope0 == 0) {
      cout << "+ linear BG ";
   } else if (fBackg0 == 0) { 
      cout << "+ const BG ";
   } 
   cout << "to Histogram:" << fSelHist->GetName()
        << " from " << fFrom << " fTo " << fTo << endl;
   cout <<
       "-------------------------------------------------------------"
       << endl;
//  Look for fix /bound flag
   Bool_t bound = kFALSE;
   func->FixParameter(0, func->GetParameter(0));
   func->FixParameter(1, func->GetParameter(1));
   func->FixParameter(2, func->GetParameter(2));
   
   for (Int_t i = 0; i < npars; i++) {
      if ((*fbflags)[i] == 1) {
         func->FixParameter(i+kFix, (*par)[i]);
      } 
// bound flag
      if ((*fbflags)[i + npars] == 1) {
         func->SetParLimits(i+kFix, (*par)[i + npars], (*par)[i + 2 * npars]);
         bound = kTRUE;
         if ((*par)[i + 2 * npars] == (*par)[i + npars]) {
            cout << "warning upper = lower bound" << endl;
         }
      }
      func->SetParameter(i+kFix, (*par)[i]);
   }
   func->SetLineWidth(fWidth);
   func->SetLineColor(fColor);

//   func->SetParameters(par->GetArray());

   func->Print();
//  now fit it 
   gPad->cd();
   if (do_fit) {
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

//      cout << "fitopt.Data() " << fitopt.Data() << endl;
      if (fGraph != NULL) {
         fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
   //     add to ListOfFunctions if requested
         if (fFitOptAddAll) {
            TList *lof = fGraph->GetListOfFunctions();
            if (lof->GetSize() > 1) {
               TObject *last = lof->Last();
               lof->Remove(last);
               lof->AddFirst(last);
            }
         }

      } else {
         fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
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
      func->SetFillStyle(0);
      if (fAutoClearMarks) ClearMarkers();
   } else {
//    no fit requested draw
      gPad->cd();
      func->Draw("same");
      cout << setblue << "No fit done, function drawn with start parameters"
          << setblack << endl;
   }

//  draw components of fit, skip simple gaus without bg

   for (Int_t i = 0; i < npars; i++) { 
     (*par)[i] = func->GetParameter(i+kFix);
//     cout << (*par)[i] << endl;
   }
   if (fShowcof != 0 && fGraph == NULL) {
//      func->GetParameters(par->GetArray());
      Double_t fdpar[4];
      if (lTailSide != 0) {
         fdpar[0] = (*par)[2] ;
         fdpar[1] = (*par)[3];
      } else {
         fdpar[0] = (*par)[0];
         fdpar[1] = (*par)[1];
      }
      if (fBackg0 == 0) {
         TF1 *back = new TF1("backf", backf, fFrom, fTo, 2+kFix);
         back->SetParameter(0, lBinW);
         back->SetParameter(0+kFix, fdpar[0]);
         if (fSlope0 == 0) back->SetParameter(1+kFix, fdpar[1]);
         else              back->SetParameter(1+kFix, 0);
         back->Save(fFrom, fTo, 0, 0, 0, 0);
         back->SetLineColor(kRed);
         back->SetLineStyle(3);
         back->Draw("same");
//         back->Print();
         fSelHist->GetListOfFunctions()->Add(back);
         back->SetParent(fSelHist);
      }
//
      Int_t offset = 0;
      if (lTailSide != 0) {
         offset = 4;
      } else {
         if (fSlope0 == 0)    offset++;
         if (fBackg0 == 0)    offset++;
      }
      Int_t mult = 3;
      if (fOnesig == 1) {
         offset++;
         mult = 2;
      }

      for (Int_t j = 0; j < fNpeaks; j++) {
         fdpar[0] = (*par)[0 + offset + mult * j];
         fdpar[1] = (*par)[1 + offset + mult * j];
         if (fOnesig == 1) 
            fdpar[2] = (*par)[offset -1];
         else
            fdpar[2] = (*par)[2 + offset + mult * j];
         TF1 *gaus = new TF1("gausf", gausf, fFrom, fTo, 3+kFix);
         gaus->SetParameter(0, lBinW);
         for (Int_t i = 0; i<3; i++)
            gaus->SetParameter(i+kFix, fdpar[i]);
         TF1 *g1 = new TF1();
         gaus->Copy(*g1);
         g1->SetLineColor(6);
         g1->SetLineStyle(4);
   //         g1->Draw("same");
         fSelHist->GetListOfFunctions()->Add(g1);
         g1->SetParent(fSelHist);
         g1->Save(fFrom, fTo, 0, 0, 0, 0);
         if (lTailSide != 0) {
            fdpar[0] = (*par)[0] * (*par)[0 + offset + mult * j];	// const
            fdpar[1] = (*par)[1 + offset + mult * j];	// position
            if (fOnesig == 1) 
               fdpar[2] = (*par)[offset -1];
            else
               fdpar[2] = (*par)[2 + offset + mult * j];
            fdpar[3] = (*par)[1]; // tail width
            TF1 *tail = new TF1("tailf", tailf, fFrom, fTo, 4+kFix);
            tail->SetParameter(0, lBinW);
            tail->SetParameter(2, lTailSide);
            for (Int_t i = 0; i<4; i++)
               tail->SetParameter(i+kFix, fdpar[i]);
            tail->Save(fFrom, fTo, 0, 0, 0, 0);
            tail->SetLineColor(7);
            tail->SetLineStyle(2);
   //            tail->Draw("same");
            fSelHist->GetListOfFunctions()->Add(tail);
            tail->SetParent(fSelHist);
   //            if(fOrigHist != fSelHist)fOrigHist->GetListOfFunctions()->Add(tail);
         }
      }
   }
   delete fbflags;
//   delete par;
//   if (fUseoldpars == 0 && do_fit) ClearMarks();
   TString drawopt = fSelHist->GetOption();
//   cout << "drawopt " <<drawopt << endl;

   TRegexp hist("hist");
   drawopt(hist) = "";
//   cout << "new drawopt " <<drawopt << endl;
   fSelHist->SetDrawOption(drawopt);
   fSelHist->SetOption(drawopt);
   gPad->Modified(kTRUE);
   gPad->Update();
   gPad->GetFrame()->SetBit(TBox::kCannotMove);
   SaveDefaults();
}

//____________________________________________________________________________________ 

void FitOneDimDialog::GetGaussEstimate(TH1 * h, Double_t from, Double_t to, 
                                       Double_t bg, TArrayD &par)
{
   Int_t frombin = h->FindBin(from);
   Int_t tobin = h->FindBin(to);
   Double_t sum = 0;
   Double_t sumx = 0;
   Double_t sumx2 = 0;
   for (Int_t i = frombin; i <= tobin; i++) {
      Double_t cont = h->GetBinContent(i) - bg;
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
      par[1] = 0.5 * (to - from); 
      par[2] = 1;
   }
}
//____________________________________________________________________________________ 

void FitOneDimDialog::GetGaussEstimate(TGraph * g, Double_t from, Double_t to, 
                                       Double_t bg, TArrayD & par)
{
   Double_t sum = 0;
   Double_t sumx = 0;
   Double_t sumx2 = 0;
   Double_t x, y;
   for (Int_t i = 0; i < g->GetN(); i++) {
      g->GetPoint(i, x, y);
      if (x >= from && x <= to) {
         Double_t cont = y - bg;
         sum += cont;
         sumx += x * cont;
         sumx2 += x * x * cont;
      }
   }
   par[0] = sum;
   if (sum > 0.) {
      par[1] = sumx / sum;
      if ( (sumx2 /sum - par[1]*par[1]) > 0)
         par[2] = TMath::Sqrt(sumx2 / sum - par[1]*par[1]);
      else 
         par[2] = 1;
   } else {
      par[1] = 0.5 * (to - from); 
      par[2] = 1;
   }
}
//____________________________________________________________________________________ 

Int_t FitOneDimDialog::GetMaxBin(TH1 * h1, Int_t binl, Int_t binu)
{
   Double_t maxcont = 0.;
   Int_t bin = (binu - binl) / 2;
   if (bin <= 0) {
      bin = -bin;
      return bin;
   }
   for (Int_t i = binl; i <= binu; i++) {
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
void FitOneDimDialog::PrintMarkers()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL || fMarkers->GetEntries() <= 0) {
      cout << " No markers set" << endl;
   } else {
      fMarkers->Print();
   }
}
//____________________________________________________________________________________ 

Int_t FitOneDimDialog::GetMarkers()
{
   fNmarks = 2;
//   if (fSelHist) {
//      fFrom = fSelHist->GetXaxis()->GetMinimum();
//      fTo   = fSelHist->GetXaxis()->GetMaximum();
//   }
// find number of peaks to fit 
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL) {
      if (fMarkers->GetEntries() <= 0) return fNmarks;
      fMarkers->Sort();
      fNmarks = fMarkers->GetEntries();
      fFrom = ((FhMarker *) fMarkers->At(0))->GetX();
      if (fNmarks > 1) {
         fTo   = ((FhMarker *) fMarkers->At(fNmarks - 1))->GetX();
      } else {
         Int_t retval = 0;
         new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Question", "Less then 2 marks set, fit anyway??",
                kMBIconQuestion, kMBYes | kMBNo, &retval);
         if (retval == kMBNo) return -1;
         fNmarks = 2;
      }
   } 
     
   if (fNmarks == 2) { 
      fNpeaks = 1;
   } else {
      fNpeaks = fNmarks - 2;
   }
//   lNpeaks = fNpeaks;
//   cout << fNmarks << " markers found " << endl;
   return fNmarks;
}
//____________________________________________________________________________________ 

void FitOneDimDialog::ClearMarkers() {
   if (fMarkers) fMarkers->Delete();
   fMarkers = NULL;
   if (fSelPad) {
      fSelPad->Modified();
      fSelPad->Update();
   }
};
//____________________________________________________________________________________ 

Int_t  FitOneDimDialog::SetMarkers() {
   cout << "Request " << fNmarks << " Markers" << endl;
   Int_t nmarks = 0;
   if (fNmarks <= 0) {
      Int_t retval = 0;
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning","fNmarks <= 0" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return 0;
   }
   ClearMarkers();
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
      fMarkers = new  FhMarkerList();
      fSelHist->GetListOfFunctions()->Add(fMarkers);
   }
   for (Int_t i = 0; i < fNmarks; i++) {
      TMarker * m1  = (TMarker*)gPad->WaitPrimitive("TMarker");
   	m1 = (TMarker *)gPad->GetListOfPrimitives()->Last();
      if (m1 == NULL) break;
      Double_t x = m1->GetX();
      Double_t y = m1->GetY();
      if (i == 0) fFrom = x;
      if (i == 1) fTo = x;

//      if (fSelPad->GetLogx()) x = TMath::Power(10, x);
//      if (fSelPad->GetLogy()) y = TMath::Power(10, y);
      FhMarker *m = new FhMarker(x, y, 28);
      m->SetMarkerColor(6);
      m->Paint();
      fMarkers->Add(m);
      delete m1;
      nmarks++;
   }
   if (fSelPad) fSelPad->Update();
   fDialog->ReloadValues();
   return nmarks;
};
//____________________________________________________________________________________ 

void FitOneDimDialog::DetLinearBackground()
{
   if (GetMarkers() != 4) {
      Int_t retval = 0;
      TGWindow* win = (TGWindow*)fParentWindow;
      new TGMsgBox(gClient->GetRoot(), win,
                "Warning", "Please set exactly 4 marks" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   Double_t xlow1 = ((FhMarker *) fMarkers->At(0))->GetX();
   Double_t xup1  = ((FhMarker *) fMarkers->At(1))->GetX();
   Double_t xlow2 = ((FhMarker *) fMarkers->At(2))->GetX();
   Double_t xup2  = ((FhMarker *) fMarkers->At(3))->GetX();
   Int_t binlow1  = fSelHist->FindBin(xlow1);
   Int_t binup1   = fSelHist->FindBin(xup1);
   Int_t binlow2  = fSelHist->FindBin(xlow2);
   Int_t binup2   = fSelHist->FindBin(xup2);

   Double_t sum1 = 0, sum2 = 0,sumx = 0;
   for (Int_t i = binlow1; i <= binup1; i++) {
      sum1 += fSelHist->GetBinContent(i);
      sumx += fSelHist->GetBinContent(i) * fSelHist->GetBinCenter(i);
   }
   Double_t meanx1 = sumx / sum1;
   Double_t meany1 = sum1 / (binup1 - binlow1 + 1);
   sumx = 0;
   for (Int_t i = binlow2; i <= binup2; i++) {
      sum2 += fSelHist->GetBinContent(i);
      sumx += fSelHist->GetBinContent(i) * fSelHist->GetBinCenter(i);
   }
   Double_t meanx2 = sumx / sum2;
   Double_t meany2 = sum2 /(binup2 - binlow2 + 1);
//   cout << "meanx1 " << meanx1 << " meany1 " << meany1 
//         << " meanx2 " << meanx2 << " meany2 " <<  meany2 << endl; 
   fLinBgSlope =  (meany2 - meany1) / (meanx2 - meanx1);
//   fLinBgSlope /= fSelHist->GetBinWidth(1);
   fLinBgConst = meany1 - fLinBgSlope * meanx1;
//   fLinBgConst /= fSelHist->GetBinWidth(1);

//   TF1 *lin_bg = new TF1("lin_bg", "pol1", xlow1, xup2);
//   lin_bg->SetParameters(fLinBgConst, fLinBgSlope);
//   cHist->cd();
//   lin_bg->Draw("same");
   cout << "Linear Background: [" << xlow1 << ", " << xup1 << "], ["
        << xlow2 << ", " << xup2 << "]" << endl 
        << "Const = " << fLinBgConst
        << " Slope = " << fLinBgSlope << endl;
//   ClearMarks();
   fLinBgSet = kTRUE;
   return ;
}
//_______________________________________________________________________

void FitOneDimDialog::SetFittingOptions()
{
   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   row_lab->Add(new TObjString("CheckButton_Use Loglikelihood method"));
   row_lab->Add(new TObjString("CheckButton_Quiet)); minimal printout"));
   row_lab->Add(new TObjString("CheckButton_Verbose printout"));
   row_lab->Add(new TObjString("CheckButton_Use Minos to improve fit"));
   row_lab->Add(new TObjString("CheckButton_Set all errors to 1"));
   row_lab->Add(new TObjString("CheckButton_Use Integral of function in bin"));
   row_lab->Add(new TObjString("CheckButton_Dont draw result function"));
   row_lab->Add(new TObjString("CheckButton_Add all fitted functions to hist"));

   valp[ind++] = &fFitOptLikelihood    ;
   valp[ind++] = &fFitOptQuiet         ;
   valp[ind++] = &fFitOptVerbose       ;
   valp[ind++] = &fFitOptMinos         ;
   valp[ind++] = &fFitOptErrors1       ;
   valp[ind++] = &fFitOptIntegral      ;
   valp[ind++] = &fFitOptNoDraw        ;
   valp[ind++] = &fFitOptAddAll        ;
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Fitting options", NULL, itemwidth, fParentWindow 
                     ,NULL, NULL, row_lab, valp);
   if (ok) SaveDefaults(); 
}
//_______________________________________________________________________

void FitOneDimDialog::ClearFunctionList()
{
   fSelHist->GetListOfFunctions()->Delete();
}
//________________________________________________________________________

void FitOneDimDialog::FitExpExecute()
{
   ExpExecute(0);    // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawExpExecute()
{
   ExpExecute(1);    // draw only
}
//________________________________________________________________________

void FitOneDimDialog::ExpExecute(Int_t draw_only)
{
   Int_t retval = 0;
   if (draw_only == 0 && (fGraph != NULL && fGraph->GetN()) == 0) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning",
                "No histogram nore graph defined\n Use Draw only" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   if ( GetMarkers() <= 0 ) { 
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning", "No marks set,\n need at least 2" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   TF1 *func = new TF1(fFuncName.Data(),"[0] + [1]*exp([2]*(x - [3]))",fFrom,fTo);
   func->SetParameter(0, fExpA);
   if (fExpFixA |= 0) func->FixParameter(0, fExpA);
   func->SetParName(0, "a (y_off)");
   func->SetParameter(1, fExpB);
   if (fExpFixB |= 0) func->FixParameter(1, fExpB);
   func->SetParName(1, "b (const)");
   func->SetParameter(2, fExpC);
   if (fExpFixC |= 0) func->FixParameter(2, fExpC);
   func->SetParName(2, "c (slope)");
   func->SetParameter(3, fExpD);
   if (fExpFixD |= 0) func->FixParameter(3, fExpD);
   func->SetParName(3, "d (x_off)");
   func->SetLineWidth(fWidth);
   func->SetLineColor(fColor);
   if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
      func->Draw("same");
      func->Print();
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
      Bool_t bound = (fExpFixA + fExpFixB + fExpFixC + fExpFixD) != 0;
      if (bound)            fitopt += "B";   // some pars are bound
      if (fGraph != NULL) {
         fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
   //     add to ListOfFunctions if requested
         if (fFitOptAddAll) {
            TList *lof = fGraph->GetListOfFunctions();
            if (lof->GetSize() > 1) {
               TObject *last = lof->Last();
               lof->Remove(last);
               lof->AddFirst(last);
            }
         }

      } else if (fSelHist != NULL) {
         fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
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
		fExpA = func->GetParameter(0);
		fExpB = func->GetParameter(1);
		fExpC = func->GetParameter(2);
		fExpD = func->GetParameter(3);
      if (fAutoClearMarks) ClearMarkers();
      IncrementIndex(&fFuncName);
   }
   gPad->Modified(kTRUE);
   gPad->Update();
}
//________________________________________________________________________

void FitOneDimDialog::FitPolExecute()
{
   PolExecute(0);    // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawPolExecute()
{
   PolExecute(1);    // draw only
}
//________________________________________________________________________

void FitOneDimDialog::PolExecute(Int_t draw_only)
{
   Int_t retval = 0;
   if (draw_only == 0 && (fGraph != NULL && fGraph->GetN()) == 0) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning",
                "No histogram nore graph defined\n Use Draw only" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   if (fPolN < 0) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning", "Degree of Polynom < 0" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   if ( GetMarkers() <= 0 ) { 
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning", "No marks set,\n need at least 2" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   TString fn;
   TString pn;
//   TArrayD par(fPolN+1);
   
   for (Int_t i = 0; i <= fPolN; i++) {
      if (i > 0) fn += "+";
      fn += "["; fn += i;fn += "]";
      for (Int_t k = 1; k <= i; k++) {
         fn += "*x";
      }
   }
   cout << "fn: " << fn << endl;
   TF1 *func = new TF1(fFuncName.Data(),fn.Data(), fFrom, fTo);
   for (Int_t i = 0; i <= fPolN; i++) {
      pn = "a"; pn += i;
      func->SetParName(i, pn);
      if (i == 0) {
         func->SetParameter(i, fPolA);
         if (fPolFixA |= 0) func->FixParameter(i, fPolA);
      } else if (i == 1) {
         func->SetParameter(i, fPolB);
         if (fPolFixB |= 0) func->FixParameter(i, fPolB);
      } else if (i == 2) {
         func->SetParameter(i, fPolC);
         if (fPolFixC |= 0) func->FixParameter(i, fPolC);
      } else if (i == 3) {
         func->SetParameter(i, fPolD);
         if (fPolFixD |= 0) func->FixParameter(i, fPolD);
      } else {
          func->SetParameter(i, 0.);
      }
   }
   func->SetLineWidth(fWidth);
   func->SetLineColor(fColor);
   if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
      func->Draw("same");
//      func->Print();
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
      Bool_t bound = (fPolFixA + fPolFixB + fPolFixC + fPolFixD) != 0;
      if (bound)            fitopt += "B";   // some pars are bound
      if (fGraph != NULL) {
         fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
   //     add to ListOfFunctions if requested
         if (fFitOptAddAll) {
            TList *lof = fGraph->GetListOfFunctions();
            if (lof->GetSize() > 1) {
               TObject *last = lof->Last();
               lof->Remove(last);
               lof->AddFirst(last);
            }
         }

      } else if (fSelHist != NULL){
         fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
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

		fPolA = func->GetParameter(0);
		if (fPolN > 0) fPolB = func->GetParameter(1);
		if (fPolN > 1) fPolC = func->GetParameter(2);
		if (fPolN > 2) fPolD = func->GetParameter(3);
      IncrementIndex(&fFuncName);
      if (fAutoClearMarks) ClearMarkers();
   }
   gPad->Modified(kTRUE);
   gPad->Update();
}
//________________________________________________________________________

void FitOneDimDialog::FitFormExecute()
{
   FormExecute(0);    // do fitting
}
//________________________________________________________________________

void FitOneDimDialog::DrawFormExecute()
{
   FormExecute(1);    // draw only
}
//________________________________________________________________________

void FitOneDimDialog::FormExecute(Int_t draw_only)
{
   Int_t retval = 0;
   if (draw_only == 0 && (fGraph != NULL && fGraph->GetN()) == 0) {
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning",
                "No histogram nore graph defined\n Use Draw only" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }

   if ( GetMarkers() <= 0 ) { 
      new TGMsgBox(gClient->GetRoot(), (TGWindow*)fParentWindow,
                "Warning", "No marks set,\n need at least 2" ,
                kMBIconExclamation, kMBDismiss, &retval);
      return;
   }
   TString fn;
   TString pn;
   TF1 *func = new TF1(fFuncName,fFormula, fFrom, fTo);
   if (func->GetNdim() <= 0) {
      cout << "Something wrong with formula" << endl;
      return;
   }
   Int_t npars = func->GetNpar();
   if (npars > 6) {
      cout << "Max 6 parameters allowed, correct function" << endl;
      return;
   }
   for (Int_t i = 0; i < npars; i++) {
      pn = "a"; pn += i;
      func->SetParName(i, pn);
      if (i == 0) {
         func->SetParameter(i, fFormA);
         if (fFormFixA |= 0) func->FixParameter(i, fFormA);
      } else if (i == 1) {
         func->SetParameter(i, fFormB);
         if (fFormFixB |= 0) func->FixParameter(i, fFormB);
      } else if (i == 2) {
         func->SetParameter(i, fFormC);
         if (fFormFixC |= 0) func->FixParameter(i, fFormC);
      } else if (i == 3) {
         func->SetParameter(i, fFormD);
         if (fFormFixD |= 0) func->FixParameter(i, fFormD);
      } else if (i == 4) {
         func->SetParameter(i, fFormE);
         if (fFormFixE |= 0) func->FixParameter(i, fFormE);
      } else if (i == 5) {
         func->SetParameter(i, fFormF);
         if (fFormFixF |= 0) func->FixParameter(i, fFormF);
      }
   }
   func->SetLineWidth(fWidth);
   func->SetLineColor(fColor);
   if (draw_only != 0 || (fGraph == NULL && fSelHist == NULL)) {
      func->SetNpx(1000);
      func->Draw("same");
//      func->Print();
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
      Bool_t bound = (fFormFixA + fFormFixB + fFormFixC + fFormFixD+ fFormFixE + fFormFixF) != 0;
      if (bound)            fitopt += "B";   // some pars are bound
      if ( fGraph != NULL) {
         fGraph->Fit(fFuncName.Data(), fitopt.Data(), "SAMES");	//  here fitting is done
   //     add to ListOfFunctions if requested
         if (fFitOptAddAll) {
            TList *lof = fGraph->GetListOfFunctions();
            if (lof->GetSize() > 1) {
               TObject *last = lof->Last();
               lof->Remove(last);
               lof->AddFirst(last);
            }
         }

      } else if (fSelHist != NULL){
         fSelHist->Fit(fFuncName.Data(), fitopt.Data());	//  here fitting is done
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
        

		fFormA = func->GetParameter(0);
		if (npars > 1) fFormB = func->GetParameter(1);
		if (npars > 2) fFormC = func->GetParameter(2);
		if (npars > 3) fFormD = func->GetParameter(3);
		if (npars > 4) fFormE = func->GetParameter(4);
		if (npars > 5) fFormF = func->GetParameter(5);
      IncrementIndex(&fFuncName);
      if (fAutoClearMarks) ClearMarkers();
   }
   gPad->Modified(kTRUE);
   gPad->Update();
}
//_______________________________________________________________________

void FitOneDimDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fFitOptLikelihood = env.GetValue("FitOneDimDialog.FitOptLikelihood", 0);
   fFitOptQuiet      = env.GetValue("FitOneDimDialog.FitOptQuiet", 0);
   fFitOptVerbose    = env.GetValue("FitOneDimDialog.FitOptVerbose", 0);
   fFitOptMinos      = env.GetValue("FitOneDimDialog.FitOptMinos", 0);
   fFitOptErrors1    = env.GetValue("FitOneDimDialog.FitOptErrors1", 0);
   fFitOptIntegral   = env.GetValue("FitOneDimDialog.FitOptIntegral", 0);
   fFitOptNoDraw     = env.GetValue("FitOneDimDialog.FitOptNoDraw", 0);
   fFitOptAddAll     = env.GetValue("FitOneDimDialog.FitOptAddAll", 0);
   fColor            = env.GetValue("FitOneDimDialog.Color", 4);
   fWidth            = env.GetValue("FitOneDimDialog.Width", 1);
   fStyle            = env.GetValue("FitOneDimDialog.Style", 1);
   fBackg0           = env.GetValue("FitOneDimDialog.Backg0", 0);
   fSlope0           = env.GetValue("FitOneDimDialog.Slope0", 0);
   fOnesig           = env.GetValue("FitOneDimDialog.Onesig", 0);
   fLowtail          = env.GetValue("FitOneDimDialog.Lowtail", 0);
   fHightail         = env.GetValue("FitOneDimDialog.Hightail",0);
   fShowcof          = env.GetValue("FitOneDimDialog.Showcof", 1);
   fExpA             = env.GetValue("FitOneDimDialog.fExpA", 0.);
   fExpB             = env.GetValue("FitOneDimDialog.fExpB", 1.);
   fExpC             = env.GetValue("FitOneDimDialog.fExpC", 1.);
   fExpD             = env.GetValue("FitOneDimDialog.fExpD", 0.);
   fExpFixA          = env.GetValue("FitOneDimDialog.fExpFixA", 0);
   fExpFixB          = env.GetValue("FitOneDimDialog.fExpFixB", 0);
   fExpFixC          = env.GetValue("FitOneDimDialog.fExpFixC", 0);
   fExpFixD          = env.GetValue("FitOneDimDialog.fExpFixD", 1);
   fPolN             = env.GetValue("FitOneDimDialog.fPolN", 1);
   fPolA             = env.GetValue("FitOneDimDialog.fPolA", 0.);
   fPolB             = env.GetValue("FitOneDimDialog.fPolB", 1.);
   fPolC             = env.GetValue("FitOneDimDialog.fPolC", 1.);
   fPolD             = env.GetValue("FitOneDimDialog.fPolD", 0.);
   fPolFixA          = env.GetValue("FitOneDimDialog.fPolFixA", 0);
   fPolFixB          = env.GetValue("FitOneDimDialog.fPolFixB", 0);
   fPolFixC          = env.GetValue("FitOneDimDialog.fPolFixC", 0);
   fPolFixD          = env.GetValue("FitOneDimDialog.fPolFixD", 0);
   fFormA             = env.GetValue("FitOneDimDialog.fFormA", 1.);
   fFormB             = env.GetValue("FitOneDimDialog.fFormB", 1.);
   fFormC             = env.GetValue("FitOneDimDialog.fFormC", 1.);
   fFormD             = env.GetValue("FitOneDimDialog.fFormD", 0.);
   fFormFixA          = env.GetValue("FitOneDimDialog.fFormFixA", 0);
   fFormFixB          = env.GetValue("FitOneDimDialog.fFormFixB", 0);
   fFormFixC          = env.GetValue("FitOneDimDialog.fFormFixC", 0);
   fFormFixD          = env.GetValue("FitOneDimDialog.fFormFixD", 0);
   fFormFixE          = env.GetValue("FitOneDimDialog.fFormFixE", 0);
   fFormFixF          = env.GetValue("FitOneDimDialog.fFormFixF", 0);
   fGausFuncName     = env.GetValue("FitOneDimDialog.fGausFuncName", "gaus_fun");
   fExpFuncName      = env.GetValue("FitOneDimDialog.fExpFuncName", "exp_fun");
   fPolFuncName      = env.GetValue("FitOneDimDialog.fPolFuncName", "pol_fun");
   fFormFuncName     = env.GetValue("FitOneDimDialog.fFormFuncName", "form_fun");
}
//_______________________________________________________________________

void FitOneDimDialog::SaveDefaults()
{
   TEnv env(".rootrc");
   env.SetValue("FitOneDimDialog.FitOptLikelihood", fFitOptLikelihood);
   env.SetValue("FitOneDimDialog.FitOptQuiet",      fFitOptQuiet);
   env.SetValue("FitOneDimDialog.FitOptVerbose",    fFitOptVerbose);
   env.SetValue("FitOneDimDialog.FitOptMinos",      fFitOptMinos);
   env.SetValue("FitOneDimDialog.FitOptErrors1",    fFitOptErrors1);
   env.SetValue("FitOneDimDialog.FitOptIntegral",   fFitOptIntegral);
   env.SetValue("FitOneDimDialog.FitOptNoDraw",     fFitOptNoDraw);
   env.SetValue("FitOneDimDialog.FitOptAddAll",     fFitOptAddAll);
   env.SetValue("FitOneDimDialog.Color",            fColor);
   env.SetValue("FitOneDimDialog.Width",            fWidth);
   env.SetValue("FitOneDimDialog.Style",            fStyle);
   env.SetValue("FitOneDimDialog.Backg0",           fBackg0);
   env.SetValue("FitOneDimDialog.Slope0",           fSlope0);
   env.SetValue("FitOneDimDialog.Onesig",           fOnesig);
   env.SetValue("FitOneDimDialog.Lowtail",          fLowtail);
   env.SetValue("FitOneDimDialog.Hightail",         fHightail);
   env.SetValue("FitOneDimDialog.Showcof",          fShowcof);
   env.SetValue("FitOneDimDialog.fExpA",   fExpA    );
   env.SetValue("FitOneDimDialog.fExpB",   fExpB    );
   env.SetValue("FitOneDimDialog.fExpC",   fExpC    );
   env.SetValue("FitOneDimDialog.fExpD",   fExpD    );
   env.SetValue("FitOneDimDialog.fExpFixA",fExpFixA );
   env.SetValue("FitOneDimDialog.fExpFixB",fExpFixB );
   env.SetValue("FitOneDimDialog.fExpFixC",fExpFixC );
   env.SetValue("FitOneDimDialog.fExpFixD",fExpFixD );
   env.SetValue("FitOneDimDialog.fPolN",   fPolN    );
   env.SetValue("FitOneDimDialog.fPolA",   fPolA    );
   env.SetValue("FitOneDimDialog.fPolB",   fPolB    );
   env.SetValue("FitOneDimDialog.fPolC",   fPolC    );
   env.SetValue("FitOneDimDialog.fPolD",   fPolD    );
   env.SetValue("FitOneDimDialog.fPolFixA",fPolFixA );
   env.SetValue("FitOneDimDialog.fPolFixB",fPolFixB );
   env.SetValue("FitOneDimDialog.fPolFixC",fPolFixC );
   env.SetValue("FitOneDimDialog.fPolFixD",fPolFixD );
   env.SetValue("FitOneDimDialog.fFormA",   fFormA    );
   env.SetValue("FitOneDimDialog.fFormB",   fFormB    );
   env.SetValue("FitOneDimDialog.fFormC",   fFormC    );
   env.SetValue("FitOneDimDialog.fFormD",   fFormD    );
   env.SetValue("FitOneDimDialog.fFormFixA",fFormFixA );
   env.SetValue("FitOneDimDialog.fFormFixB",fFormFixB );
   env.SetValue("FitOneDimDialog.fFormFixC",fFormFixC );
   env.SetValue("FitOneDimDialog.fFormFixD",fFormFixD );
   env.SetValue("FitOneDimDialog.fFormFixE",fFormFixE );
   env.SetValue("FitOneDimDialog.fFormFixF",fFormFixF );
   env.SetValue("FitOneDimDialog.fGausFuncName", fGausFuncName);
   env.SetValue("FitOneDimDialog.fExpFuncName",  fExpFuncName );
   env.SetValue("FitOneDimDialog.fPolFuncName",  fPolFuncName );
   env.SetValue("FitOneDimDialog.fFormFuncName", fFormFuncName);
   env.SaveLevel(kEnvLocal);
}
//_______________________________________________________________________

void FitOneDimDialog::CloseDialog()
{
//   cout << "FitOneDimDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   fDialog->CloseWindow();
}
//_______________________________________________________________________

void FitOneDimDialog::CloseDown()
{
//   cout << "FitOneDimDialog::CloseDown() " << endl;
   SaveDefaults();
   delete this;
}
//________________________________________________________________
void FitOneDimDialog::IncrementIndex(TString * arg)
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
