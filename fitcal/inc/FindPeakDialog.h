#ifndef FINDPEAKDIALOG
#define FINDPEAKDIALOG
#include "TH1.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TRootCanvas.h"
#include "TString.h"
#include "FhMarker.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


class FindPeakDialog : public TObject {

private:
   Int_t fInteractive;
   TRootCanvas* fParentWindow; 
   TGMrbValuesAndText *fDialog;
   TString fFuncName;
   TVirtualPad *fSelPad;
   TH1     *fSelHist;
   TString fName;
   FhMarkerList * fMarkers;
   Double_t fFrom;               // Used range, lower value
   Double_t fTo;                 // Used range, upper value
   Double_t fThreshold;          // Minimum peaks size used, unit: fraction compared to highest peak in range
                                 // Note: If height of peaks vary strongly use several ranges
                                 //       use dont clear list (see below)
   Double_t fSigma;              // Assumed peakwidth
   Double_t fTwoPeakSeparation;  // mimimum separation of two peaks, unit: sigma
   Int_t    fClearList;          // clear list of found peaks before new search
   Int_t    fMarkow;					// use Markow algorithm
   Int_t    fRemoveBG;           // remove background before deconvolution
   Int_t    fShowMarkers;        // draw polymarker at found peak
   Int_t    fFindPeakDone;       // number of peaksearches executed

public:
   FindPeakDialog(TH1 * hist, Int_t interactive =1);
   virtual ~FindPeakDialog();
   void RecursiveRemove(TObject * obj);
   void ExecuteFindPeak();
   void ClearList();
   Int_t FindPeakDone() { return fFindPeakDone; };
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDialog();
   void CloseDown();
   void SetFrom( Double_t from) { fFrom = from; };
   void SetTo( Double_t to) { fTo = to; };
   void SetThreshold( Double_t threshold) { fThreshold = threshold; };
   void SetSigma( Double_t sigma) { fSigma = sigma; };
   void SetTwoPeakSeparation( Double_t twopeakseparation) { fTwoPeakSeparation = twopeakseparation; };
   void SetClearList( Int_t clearlist) { fClearList = clearlist; };
   void SetMarkow( Int_t markow) { fMarkow = markow; };
   void SetRemoveBG( Int_t removebg) { fRemoveBG = removebg; };
   void SetShowMarkers( Int_t showmarkers) { fShowMarkers = showmarkers; };
   Double_t GetFrom() { return fFrom; };
   Double_t GetTo() { return fTo; };
   Double_t GetThreshold() { return fThreshold; };
   Double_t GetSigma() { return fSigma; };
   Double_t GetTwoPeakSeparation() { return fTwoPeakSeparation; };
   Int_t GetClearList() { return fClearList; };
   Int_t GetMarkow() { return fMarkow; };
   Int_t GetRemoveBG() { return fRemoveBG; };
   Int_t GetShowMarkers() { return fShowMarkers; };

ClassDef(FindPeakDialog,0)
};
#endif
