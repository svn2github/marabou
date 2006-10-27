#ifndef CALIBRATIONDIALOG
#define CALIBRATIONDIALOG
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "HistPresent.h"
//_____________________________________________________________________________________


class CalibrationDialog : public TObject {

static const Int_t MAXPEAKS = 10;
private:
   HistPresent *fHistPresent;
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   TString fFormula;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   TF1     *fCalFunc;
   Double_t fX[MAXPEAKS];
   Double_t fY[MAXPEAKS];
   Double_t fXE[MAXPEAKS];
   Double_t fYE[MAXPEAKS];
   Int_t    fUse[MAXPEAKS];
   Int_t    fMaxPeaks;
   Int_t    fNbinsX;
   Double_t fXlow;
   Double_t fXup;
   Int_t    fNpeaks;

public:
   CalibrationDialog(TH1 * hist, Int_t maxPeaks = 3);
   virtual ~CalibrationDialog(){};
//   void RecursiveRemove(TObject * obj) {};
   void CalculateFunction();
   void FillCalibratedHist();
   void UpdatePeakList();
   void ClearLocalPeakList();
   void ClearHistPeakList();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();

ClassDef(CalibrationDialog,0)
};
#endif
