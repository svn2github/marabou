#ifndef CALIBRATIONDIALOG
#define CALIBRATIONDIALOG
#include "TArrayD.h"
#include "TGraph.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
//#include "defineMarabou.h"
#ifdef MARABOUVERS
#include "HistPresent.h"
#endif
//_____________________________________________________________________________________

class CalibrationDialog : public TObject {

static const Int_t MAXPEAKS = 100;
static const Int_t MAXNOMPEAKS = 100;

private:
#ifdef MARABOUVERS
   HistPresent *fHistPresent;
#endif
   TRootCanvas        *fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TGMrbValuesAndText *fDialogSetNominal;
   TGMrbValuesAndText *fAutoSelectDialog;
   TString     fFuncName;
   TString     fFuncFromFile;
   TString     fFormula;
   TVirtualPad *fSelPad;
   TH1         *fSelHist;
   TH1         *fCalHist;
   TF1         *fCalFunc;
   TList    fPeakList;
   Double_t fX[MAXPEAKS];
   Double_t fY[MAXPEAKS];
   Double_t fXE[MAXPEAKS];
   Double_t fYE[MAXPEAKS];
   Double_t fCont[MAXPEAKS];
   Int_t    fUse[MAXPEAKS];
   Int_t    fAssigned[MAXPEAKS];
   Int_t    fMaxPeaks;
   Int_t    fNpeaks;
   Int_t    fSetFlag[MAXNOMPEAKS];
   Int_t    fGaugeNpeaks;
   TList    fGaugeName;
   TArrayD  fGaugeEnergy;
   TArrayD  fGaugeError;
   TArrayD  fGaugeIntensity;
   Int_t    fAutoAssigned;  
   
   Int_t    fInteractive;     // Flag for user interaction, 0: no Dialog
   Int_t    fVerbose;         // Verbose printout
   Int_t    fCalibratedNbinsX;// Params for calibrated histogram, Nbins
   Double_t fCalibratedXlow;  // Xlow
   Double_t fCalibratedXup;   // Xup
   Int_t    fEu152Gauge;      // Use Eu 152 gauge source
   Int_t    fCo60Gauge;       // Use Co60 gauge source
   Int_t    fY88Gauge;        // Use Y88gauge source
   Int_t    fCustomGauge;     // Use user provided gauge peaks
   TString  fCustomGaugeFile; // File name for user provided gauge peaks
   Int_t    fMatchNbins;      // Number of bins for peak match histogram
   Double_t fMatchMin;        // Low edge for peak match histogram
   Double_t fMatchMax;        // Upper edge for peak match histogram
   Double_t fGainMin;         // Min gain value  for scanning
   Double_t fGainMax;         // Max gain value  for scanning
   Double_t fGainStep;        // Step size gain for scanning
   Double_t fOffMin;          // Min offset value for scanning
   Double_t fOffMax;          // Max offset value for scanning
   Double_t fOffStep;         // Step size for offset for scanning
   Double_t fAccept;          // Acceptance limit: Abs (gauge - measured)

public:
   CalibrationDialog(TH1 * hist, Int_t interactive = 1);
   virtual ~CalibrationDialog(){};
//   void RecursiveRemove(TObject * obj) {};
   void CalculateFunction();
   void SaveFunction();
   void GetFunction();
   void ExecuteGetFunction();
   void FillCalibratedHist();
   void UpdatePeakList();
   void SetNominalValues();
   void SetBuiltinGaugeValues();
   void QueryReadGaugeFile();
   Int_t ReadGaugeFile();
   void SetValues();
   void AutoSelectDialog();
   void ExecuteAutoSelect();
   Int_t FindNumberOfPeaks();
//   void ClearHistPeakList();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();
   void DisableDialogs();
   void EnableDialogs();
   void SetInteractive( Int_t interactive) { fInteractive = interactive; };
   void SetVerbose(Int_t val) { fVerbose = val; };
   void SetCalibratedNbinsX( Int_t calibratednbinsx) { fCalibratedNbinsX = calibratednbinsx; };
   void SetCalibratedXlow( Double_t calibratedxlow) { fCalibratedXlow = calibratedxlow; };
   void SetCalibratedXup( Double_t calibratedxup) { fCalibratedXup = calibratedxup; };
   void SetCustomGauge( Int_t customgauge) { fCustomGauge = customgauge; };
   void SetCustomGaugeFile(const Char_t * customgaugefile) { fCustomGaugeFile = customgaugefile; };
   void SetFormula(const Char_t * formula) { fCustomGaugeFile = formula; };
   void SetMatchNbins( Int_t matchnbins) { fMatchNbins = matchnbins; };
   void SetMatchMin( Double_t matchmin) { fMatchMin = matchmin; };
   void SetMatchMax( Double_t matchmax) { fMatchMax = matchmax; };
   void SetGainMin( Double_t gainmin) { fGainMin = gainmin; };
   void SetGainMax( Double_t gainmax) { fGainMax = gainmax; };
   void SetGainStep( Double_t gainstep) { fGainStep = gainstep; };
   void SetOffMin( Double_t offmin) { fOffMin = offmin; };
   void SetOffMax( Double_t offmax) { fOffMax = offmax; };
   void SetOffStep( Double_t offstep) { fOffStep = offstep; };
   void SetAccept( Double_t val) { fAccept = val; };
   void SetEu152Gauge( Int_t eu152gauge) { fEu152Gauge = eu152gauge; };
   void SetCo60Gauge( Int_t co60gauge) { fCo60Gauge = co60gauge; };
   void SetY88Gauge( Int_t y88gauge) { fY88Gauge = y88gauge; };

   TF1   *GetCalFunction()     { return fCalFunc;};
   Int_t GetVerbose()          { return fVerbose; };
   Int_t GetInteractive()      { return fInteractive; };
   Int_t GetCalibratedNbinsX() { return fCalibratedNbinsX; };
   Double_t GetCalibratedXlow(){ return fCalibratedXlow; };
   Double_t GetCalibratedXup() { return fCalibratedXup; };
   Int_t GetCustomGauge()      { return fCustomGauge; };
   const Char_t *GetCustomGaugeFile(){ return fCustomGaugeFile.Data(); };
   const Char_t *GetFormula()    { return fFormula.Data(); };
   Int_t GetMatchNbins() { return fMatchNbins; };
   Double_t GetMatchMin(){ return fMatchMin; };
   Double_t GetMatchMax(){ return fMatchMax; };
   Double_t GetGainMin() { return fGainMin; };
   Double_t GetGainMax() { return fGainMax; };
   Double_t GetGainStep(){ return fGainStep; };
   Double_t GetOffMin()  { return fOffMin; };
   Double_t GetOffMax()  { return fOffMax; };
   Double_t GetOffStep() { return fOffStep; };
   Double_t GetAccept()  { return fAccept; };
   TList * GetPeakList() { return &fPeakList; };
   Int_t GetEu152Gauge() { return fEu152Gauge; };
   Int_t GetCo60Gauge()  { return fCo60Gauge; };
   Int_t GetY88Gauge()   { return fY88Gauge; };


ClassDef(CalibrationDialog,0)
};
#endif
