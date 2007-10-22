#ifndef FITONEDIMDIALOG
#define FITONEDIMDIALOG
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class FitOneDimDialog : public TObject {

private:
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   Int_t   fFuncNumber;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   TGraph  *fGraph;
   Color_t fColor;
   Width_t fWidth;
   Style_t fStyle;
   TString fName;
   TString fGausFuncName;
   TString fExpFuncName;
   TString fPolFuncName;
   TString fFormFuncName;
   TString fFormula;
   FhMarkerList * fMarkers;
   Double_t fFrom;
   Double_t fTo;
   Double_t fMean;
   Double_t fMeanError;
   Double_t fConstant;
   Double_t fExpA;
   Double_t fExpB;
   Double_t fExpC;
//   Double_t fExpD;
   Int_t fExpFixA;
   Int_t fExpFixB;
   Int_t fExpFixC;
//   Int_t fExpFixD;
   Int_t fPolN;
   Double_t fPolA;
   Double_t fPolB;
   Double_t fPolC;
   Double_t fPolD;
   Int_t fPolFixA;
   Int_t fPolFixB;
   Int_t fPolFixC;
   Int_t fPolFixD;
   Double_t fFormA;
   Double_t fFormB;
   Double_t fFormC;
   Double_t fFormD;
   Double_t fFormE;
   Double_t fFormF;
   Int_t fFormFixA;
   Int_t fFormFixB;
   Int_t fFormFixC;
   Int_t fFormFixD;
   Int_t fFormFixE;
   Int_t fFormFixF;
  
   Int_t    fAdded;
   Double_t fLinBgConst;
   Double_t fLinBgSlope;
   Bool_t   fLinBgSet;
   Int_t fUseoldpars;        // use parameters of previous fit
   Int_t fUsedbg;            // use prederminated linear background
   Int_t fNpeaks;            // number peaks used 
   Int_t fReqNmarks;
   Int_t fNmarks;
   Int_t fNevents;
   Int_t fFitOptAddAll;      // Add all fitted functions to histogram
   Bool_t fFitPeakListDone;
   Int_t fAutoClearMarks;    // Clear marks after fit
// The following parameters should be set with "FitPeakList"
   Int_t    fInteractive;    // Run interactivly, ie. really present dialog widgets 
   Double_t fPeakSep;        // if peaks are closer (unit of sigma) they are fitted together
   Double_t fFitWindow;      // Fit window: peak pos += fFitWindow
   Int_t fBackg0;            // Force background = 0
   Int_t fSlope0;            // Force background slope = 0 (use constant background)
   Int_t fOnesig;            // Use same sigma for all peaks ( if fittted together)
   Int_t fLowtail;           // Use low tail 
   Int_t fHightail;          // Use high tail 
   Int_t fShowcof;           // Display components of fit
   Int_t fConfirmStartValues;// Present a widget with start values before fitting
   Int_t fPrintStartValues;  // Print start values
   Int_t fFitOptLikelihood;  // Use Likelihood method (default chi2)
   Int_t fFitOptQuiet;       // Suppress printout during fitting
   Int_t fFitOptVerbose;     // Verbose printout during fitting 
   Int_t fFitOptMinos;       // Use minos to improve fit after migrad
   Int_t fFitOptErrors1;     // Set all errors to 1`
   Int_t fFitOptIntegral;    // Use integral of bin
   Int_t fFitOptNoDraw;      // dont draw fit result
   Int_t fFitPrintCovariance;// Print Correlation matrix (normalized covariance)

public:
   FitOneDimDialog(TH1 * hist, Int_t type = 1, Int_t interactive = 1);
   FitOneDimDialog(TGraph * graph, Int_t type = 1, Int_t interactive = 1);
   void DisplayMenu(Int_t type = 1);
   virtual ~FitOneDimDialog();
   void RecursiveRemove(TObject * obj);
   Bool_t FitPeakList();
   Bool_t FitGausExecute();
   void FitExpExecute();
   void DrawExpExecute();
   void CalcStartParExp();
   void ExpExecute(Int_t draw_only = 0);
   void FitPolExecute();
   void DrawPolExecute();
   void PolExecute(Int_t draw_only = 0);
   void PrintCorrelation(); 
   void FitFormExecute();
   void DrawFormExecute();
   void FormExecute(Int_t draw_only = 0);
   void FillHistRandom();
   TH1  *FindHistInPad();
//   void AddToCalibration();
   void SetFitOptions(){};
   Int_t GetMaxBin(TH1 * h1, Int_t binl, Int_t binu);
   void GetGaussEstimate(TH1 *h, Double_t from, Double_t to,  
                         Double_t bg,TArrayD & par);
   void GetGaussEstimate(TGraph *g, Double_t from, Double_t to,  
                         Double_t bg,TArrayD & par);
   void DetLinearBackground();
   Int_t GetMarkers();
   void ClearMarkers();
   void ClearFunctionList();
   void PrintMarkers();
   Int_t SetMarkers();
   void SetFittingOptions();
   Bool_t FitPeakListDone() { return fFitPeakListDone; };
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();
   void IncrementIndex(TString * arg);
//
   void SetPeakSep( Double_t peaksep) { fPeakSep = peaksep; };
   void SetFitWindow( Double_t fitwindow) { fFitWindow = fitwindow; };
   void SetUseoldpars( Int_t useoldpars) { fUseoldpars = useoldpars; };
   void SetUsedbg( Int_t usedbg) { fUsedbg = usedbg; };
   void SetNpeaks( Int_t npeaks) { fNpeaks = npeaks; };
   void SetReqNmarks( Int_t reqnmarks) { fReqNmarks = reqnmarks; };
   void SetNmarks( Int_t nmarks) { fNmarks = nmarks; };
   void SetNevents( Int_t nevents) { fNevents = nevents; };
   void SetBackg0( Int_t backg0) { fBackg0 = backg0; };
   void SetSlope0( Int_t slope0) { fSlope0 = slope0; };
   void SetOnesig( Int_t onesig) { fOnesig = onesig; };
   void SetLowtail( Int_t lowtail) { fLowtail = lowtail; };
   void SetHightail( Int_t hightail) { fHightail = hightail; };
   void SetShowcof( Int_t showcof) { fShowcof = showcof; };
   void SetAutoClearMarks( Int_t autoclearmarks) { fAutoClearMarks = autoclearmarks; };
   void SetConfirmStartValues( Int_t confirmstartvalues) { fConfirmStartValues = confirmstartvalues; };
   void SetFitOptLikelihood( Int_t fitoptlikelihood) { fFitOptLikelihood = fitoptlikelihood; };
   void SetFitOptQuiet( Int_t fitoptquiet) { fFitOptQuiet = fitoptquiet; };
   void SetFitOptVerbose( Int_t fitoptverbose) { fFitOptVerbose = fitoptverbose; };
   void SetFitOptMinos( Int_t fitoptminos) { fFitOptMinos = fitoptminos; };
   void SetFitOptErrors1( Int_t fitopterrors1) { fFitOptErrors1 = fitopterrors1; };
   void SetFitOptIntegral( Int_t fitoptintegral) { fFitOptIntegral = fitoptintegral; };
   void SetFitOptNoDraw( Int_t fitoptnodraw) { fFitOptNoDraw = fitoptnodraw; };
   void SetFitOptAddAll( Int_t fitoptaddall) { fFitOptAddAll = fitoptaddall; };
   void SetFitPrintCovariance( Int_t fitprintcovariance) { fFitPrintCovariance = fitprintcovariance; };
// 
   Double_t GetPeakSep() { return fPeakSep; };
   Double_t GetFitWindow() { return fFitWindow; };
   Int_t GetUseoldpars() { return fUseoldpars; };
   Int_t GetUsedbg() { return fUsedbg; };
   Int_t GetNpeaks() { return fNpeaks; };
   Int_t GetReqNmarks() { return fReqNmarks; };
   Int_t GetNmarks() { return fNmarks; };
   Int_t GetNevents() { return fNevents; };
   Int_t GetBackg0() { return fBackg0; };
   Int_t GetSlope0() { return fSlope0; };
   Int_t GetOnesig() { return fOnesig; };
   Int_t GetLowtail() { return fLowtail; };
   Int_t GetHightail() { return fHightail; };
   Int_t GetShowcof() { return fShowcof; };
   Int_t GetAutoClearMarks() { return fAutoClearMarks; };
   Int_t GetConfirmStartValues() { return fConfirmStartValues; };
   Int_t GetFitOptLikelihood() { return fFitOptLikelihood; };
   Int_t GetFitOptQuiet() { return fFitOptQuiet; };
   Int_t GetFitOptVerbose() { return fFitOptVerbose; };
   Int_t GetFitOptMinos() { return fFitOptMinos; };
   Int_t GetFitOptErrors1() { return fFitOptErrors1; };
   Int_t GetFitOptIntegral() { return fFitOptIntegral; };
   Int_t GetFitOptNoDraw() { return fFitOptNoDraw; };
   Int_t GetFitOptAddAll() { return fFitOptAddAll; };
   Int_t GetFitPrintCovariance() { return fFitPrintCovariance; };
   void CRButtonPressed(){};

ClassDef(FitOneDimDialog,0)
};
#endif
