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
   Double_t fExpD;
   Int_t fExpFixA;
   Int_t fExpFixB;
   Int_t fExpFixC;
   Int_t fExpFixD;
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
   Int_t fUseoldpars;
   Int_t fUsedbg;
   Int_t fNpeaks;
   Int_t fNmarks;
   Color_t fColor;
   Width_t fWidth;
   Style_t fStyle;
   Int_t   fBackg0;
   Int_t   fSlope0;
   Int_t   fOnesig;
   Int_t   fLowtail;
   Int_t   fHightail;
   Int_t   fShowcof;
   Int_t   fAutoClearMarks;
   Int_t fFitOptLikelihood;  
   Int_t fFitOptQuiet;  
   Int_t fFitOptVerbose;  
   Int_t fFitOptMinos;  
   Int_t fFitOptErrors1;  
   Int_t fFitOptIntegral;  
   Int_t fFitOptNoDraw;  
   Int_t fFitOptAddAll; 

public:
   FitOneDimDialog(TH1 * hist, Int_t type = 1);
   FitOneDimDialog(TGraph * graph, Int_t type = 1);
   void DisplayMenu(Int_t type = 1);
   virtual ~FitOneDimDialog();
   void RecursiveRemove(TObject * obj);
   void FitGausExecute();
   void FitExpExecute();
   void DrawExpExecute();
   void ExpExecute(Int_t draw_only = 0);
   void FitPolExecute();
   void DrawPolExecute();
   void PolExecute(Int_t draw_only = 0);
   void FitFormExecute();
   void DrawFormExecute();
   void FormExecute(Int_t draw_only = 0);
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
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();
   void IncrementIndex(TString * arg);

ClassDef(FitOneDimDialog,0)
};
#endif
