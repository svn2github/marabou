#ifndef FITONEDIMDIALOG
#define FITONEDIMDIALOG
#include "TObject.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
//_____________________________________________________________________________________


class FitOneDimDialog : public TObject {

private:
   TRootCanvas* fParentWindow; 
   TString fFuncName;
   Int_t   fFuncNumber;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   FhMarkerList * fMarkers;
   Double_t fFrom;
   Double_t fTo;
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
   Int_t fFitOptLikelihood;  
   Int_t fFitOptQuiet;  
   Int_t fFitOptVerbose;  
   Int_t fFitOptMinos;  
   Int_t fFitOptErrors1;  
   Int_t fFitOptIntegral;  
   Int_t fFitOptNoDraw;  
   Int_t fFitOptAddAll; 

public:
   FitOneDimDialog(TH1 * hist);
   virtual ~FitOneDimDialog(){};
   void FitOneDimExecute();
   void SetFitOptions(){};
   Int_t GetMaxBin(TH1 * h1, Int_t binl, Int_t binu);
   void GetGaussEstimate(TH1 * h, Double_t from, Double_t to,  
                         Double_t bg,TArrayD & par);
   void DetLinearBackground();
   Int_t GetMarkers();
   void ClearMarkers();
   void PrintMarkers();
   Int_t SetMarkers();
   void SetFittingOptions();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();

ClassDef(FitOneDimDialog,0)
};
#endif
