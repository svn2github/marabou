#ifndef CALIBRATIONDIALOG
#define CALIBRATIONDIALOG
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
#ifdef MARABOUVERS
#include "HistPresent.h"
#endif
//_____________________________________________________________________________________

class CalibrationDialog : public TObject {

static const Int_t MAXPEAKS = 10;
static const Int_t MAXNOMPEAKS = 100;

private:
#ifdef MARABOUVERS
   HistPresent *fHistPresent;
#endif
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   TString fFuncFromFile;
   TString fFormula;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   TH1     *fCalHist;
   TF1     *fCalFunc;
   Double_t fX[MAXPEAKS];
   Double_t fY[MAXPEAKS];
   Double_t fXE[MAXPEAKS];
   Double_t fYE[MAXPEAKS];
   Double_t fCont[MAXPEAKS];
   Int_t    fUse[MAXPEAKS];
   Int_t    fMaxPeaks;
   Int_t    fNbinsX;
   Double_t fXlow;
   Double_t fXup;
   Int_t    fNpeaks;
   Int_t    fSetFlag[MAXNOMPEAKS];

public:
   CalibrationDialog(TH1 * hist, Int_t maxPeaks = 3);
   virtual ~CalibrationDialog(){};
//   void RecursiveRemove(TObject * obj) {};
   void CalculateFunction();
   void SaveFunction();
   void GetFunction();
   void FillCalibratedHist();
   void UpdatePeakList();
   void SetNominalValues();
   void SetValues();
//   void ClearLocalPeakList();
//   void ClearHistPeakList();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();

ClassDef(CalibrationDialog,0)
};
#endif
