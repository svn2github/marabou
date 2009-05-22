#ifndef HPR_FITHIST
#define HPR_FITHIST
#include "TArrayI.h"
#include "TArrayF.h"
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TCutG.h>
#include <TTree.h>
#include <TStyle.h>
#include "TUnixSystem.h"
#include <TList.h>
#include <TPaveText.h>
#include "TGWindow.h"
#include "TVirtualPad.h"
#include "HTCanvas.h"
#include "TRootCanvas.h"
#include "FhMarker.h"
#include "FhPeak.h"
#include "TableOfLabels.h"
#include "FindPeakDialog.h"
#include "CalibrationDialog.h"
#include "Save2FileDialog.h"

namespace std {} using namespace std;


class TMrbWindow2D;
class TMrbWindow;
class FitOneDimDialog;
//______________________________ globals ____________________________________

static const char attrname[]="FH_setdefaults.C";
static const char fitmacroname[]="fit_user_function.C";

//______________________________________________________________________

class FitHist : public TNamed {
private:
   Int_t   fBinX_1, fBinX_2, fBinY_1, fBinY_2;  // lower, upper bin selected
   Int_t   fBinlx, fBinux, fBinly, fBinuy;     // lower, upper bin in expanded
   Axis_t  fExplx, fExpux, fExply, fExpuy;     // lower, upper edge in expanded
   Axis_t  fX_1,   fX_2,   fY_1,   fY_2;     // lower, upper lim selected
   Axis_t  fRangeLowX, fRangeUpX, fRangeLowY, fRangeUpY;
   Axis_t  fOrigLowX, fOrigUpX, fOrigLowY, fOrigUpY;
   Bool_t fSetRange;
   Bool_t fKeepParameters;
   Bool_t fCallMinos;
   Int_t  fOldMode;
   TH1    *fSelHist, *fOrigHist;          // pointer to the selected histogram
   TH1    *fCalHist;
   FitHist  *fCalFitHist;
   TF1      *fCalFunc;
   FitOneDimDialog *fFit1DimD;
   HistPresent* hp;
   HTCanvas *cHist;
   TH1     *expHist, *projHist, *projHistX, *projHistY,*projHistF;
   TString fHname;
   TString fCname;
   TString fCtitle;
   TString fEname;
   TString fCutname;

   TVirtualPad *fSelPad;          // pointer to the selected pad

   TList *fActiveFunctions;
   TList *fAllFunctions;
   TList *fActiveWindows;
   TList *fAllWindows;
   TList *fAllCuts;
   TList *fPeaks;
   TList *fActiveCuts;
   FhMarkerList *fMarkers;
   TObjArray *peaks;
   TList *fCmdLine;
   Int_t fExpInd;
   Int_t fSerialPx;
   Int_t fSerialPy;
   Int_t fSerialPf;
   Int_t fSerialRot;
   Int_t fFuncNumb;
   Int_t fCutNumber;
   Int_t wdw_numb;
   Int_t fColSuperimpose;
   void ExpandProject(Int_t);
   Int_t fLogx;
   Int_t fLogy;
   Int_t fLogz;
   Bool_t fSelInside;
   Int_t fUserContourLevels;
   Bool_t fSetLevels;
   Bool_t fSetColors;

   Float_t fMax, fMin, fXmax, fXmin, fYmax, fYmin;
   TRootCanvas *mycanvas;
//   TPaveText *datebox;
   Int_t fDimension;
   Bool_t fCanvasIsAlive;
   TString fXtitle;
   TString fYtitle;
   TString fFitMacroName;
   TString fTemplateMacro;
   TString fFitSliceYMacroName;
   Int_t fFirstUse;
   Bool_t fDeleteCalFlag;
   HTCanvas * fCutPanel;
   TableOfLabels * fTofLabels;

   Int_t fFill1Dim;
   Color_t fHistFillColor;
   Color_t f2DimBackgroundColor;
   Color_t fHistLineColor;
   Float_t fHistLineWidth;
   Float_t fMarkerSize;
	Int_t fHistFillStyle;
   Int_t fShowContour;
   Int_t fShowDateBox;
   Int_t fShowStatBox;
   Int_t fOptStat;
   Int_t fUseTimeOfDisplay;
   Int_t fShowTitle;
   Int_t fShowFitBox;
   Int_t fLiveStat1Dim;
   Int_t fLiveStat2Dim;
   Int_t fLiveGauss;
   Int_t fLiveBG;
   Int_t fShowZScale;
   Int_t fDrawAxisAtTop;
   TString fDrawOpt2Dim;
   Int_t fTitleCenterX;
   Int_t fTitleCenterY;
   Int_t fTitleCenterZ;
   TText * fDateText;
   Save2FileDialog *fDialog;
   TString fErrorMode;

public:
   FitHist(const Text_t *name, const Text_t *title, TH1 *hist,
           const Text_t *hname, Int_t win_topx = 520, Int_t win_topy = 5,
           Int_t win_widx = 750, Int_t win_widy = 550);
   ~FitHist();
   void SaveDefaults(Bool_t recalculate = kTRUE);
   void RestoreDefaultRanges();
   void DisplayHist(TH1*, Int_t, Int_t, Int_t, Int_t);  // display hists
//   void DrawHist();                      //
   void Draw1Dim();                      //
   void Draw2Dim();                      //
   void Draw3Dim();                      //
   const Char_t * GetCanvasName(){return fCname.Data();};             //
   TRootCanvas* GetMyCanvas(){return mycanvas;};
   HTCanvas* GetCanvas(){return cHist;};
   void  SetMyCanvas(TRootCanvas *myc){mycanvas = myc;};
   TH1* GetSelHist(){return fSelHist;};
   TH1* SetHist(TH1* newhist){
      TH1* temp = fSelHist;
      fSelHist = newhist;
      if(newhist->InheritsFrom("TH2"))Draw2Dim();
      else                            Draw1Dim();
      return temp;
   };
   TH1* GetOneHist();
   Bool_t IsThereAnyHist();
   void Entire();                  //
   void SetXaxisRange();                  //
   void SetYaxisRange();                  //
   void SetZaxisRange();                  //
   void ProfileX();                  //
   void ProfileY();                  //
   void RebinOne();                  //
   void ExecDefMacro();                  //
   void ExecFitMacro();                  //
   void EditFitMacro();                  //
   void ExecFitSliceYMacro();                  //
   void EditFitSliceYMacro();                  //
   void SetTemplateMacro(const char * name){fTemplateMacro = name;};
   void SetFitSliceYMacroName(const char *name){fFitSliceYMacroName = name;};                  //
   void RedefineAxis();                  //
   void AddAxis(Int_t where);           //
   void Magnify();                 //
   void GetLimits();               //
   void GetRange();                // get range from another hist
   void AddMark(TPad *, Int_t, Int_t);

   Int_t GetMarks(TH1*);
   void ClearMarks();               //
   Bool_t GetKeepPar(){return fKeepParameters;};
   void SetKeepPar(Bool_t keeppar)
        {fKeepParameters=keeppar;if(!keeppar)fOldMode=-1;};
   void SetCallMinos(Bool_t cminos) {fCallMinos=cminos;};
   Bool_t GetCallMinos() {return fCallMinos;};
   void SetInside(Bool_t inside){fSelInside=inside;};

   void SetLogx(Int_t state) {fLogx = state; cHist->SetLogx(state);};
   void SetLogy(Int_t state) {fLogy = state; cHist->SetLogy(state);};
   void SetLogz(Int_t state);
   void SaveUserContours();               //
   void SetUserContours();               //
   void ClearUserContours();               //
   void UseSelectedContour();

   Int_t GetLogx() {return fLogx;};
   Int_t GetLogy() {return fLogy;};
   Int_t GetLogz() {return fLogz;};

   Bool_t InsideState(){return fSelInside;};
   void PaintMarks();               //
   void PrintMarks();               //
   void Set2Marks();               //
   TObjArray* GetPointertoMarks(){return fMarkers;};
   void AddFunctionsToHist();
   void ClearRegion();
   void MarksToWindow();               //
   void ClearWindows();               //
   void DrawWindows();               //
   void WriteOutWindows();               //
   void ListWindows();                       //
   void AddWindow(const char *, const char * bp=NULL);            //
   void AddWindowsToHist();
   void RemoveWindowsFromHist();
   void EditWindow(const char *);

   Int_t Nwindows(){return fActiveWindows->GetSize();};
   Int_t Ncuts(){return fActiveCuts->GetSize();};                  //
   void InitCut();                       //
   void EditCut(const char *);
   void PrintOneCut(TMrbWindow2D *);      //
   void CloseCuts();                       //
   void ListCuts();                       //
   void AddCut(const char *, const char * bp=NULL);            //
   Float_t DrawCut();                      //
   void DrawCutName();                      //
   void MarksToCut();               //
   void AddCutsToHist();
   void DrawDate();                      //
   Bool_t UseCut(TMrbWindow2D *);         //
   Bool_t UseWindow(TMrbWindow *);         //
   Bool_t InsideCut(Float_t, Float_t);               //
   void ClearCut();                   //
   void RemoveAllCuts();
   void UpdateCut();                  //
   void UpdateDrawOptions();
   Bool_t IsCalibrated() {return fSetRange;}                       //
   Bool_t Its2dim(){return fOrigHist->InheritsFrom("TH2");};               //
//   void FitGBg(Int_t tail, Int_t force_zero_bg =0);  // fit gaus + lin bg max 3 peaks
   void Fit1DimDialog(Int_t type = 1);       // fit gaus + lin bg any number of peaks
//   Int_t Fit1dim(Int_t, Int_t);            // fit polynoms
   Int_t Fit2dim(Int_t, Int_t);            // fit polynoms
   Int_t FitPolyHist(Int_t);               // fit polynoms hist
   Int_t FitPolyMarks(Int_t);              // fit  polynoms to marks
   void  Expand();                     // expand
   void  ProjectX();
   void  ProjectY();
   void  ProjectBoth();
   void  ProjectF();                   // project along a function
   void  Transpose();
   void  Rotate(Int_t sense);
   void OutputStat();
//   void ListFunctions();
   void WriteFunctions();
//   void WriteFunctionList();
   void PictToPSFile(Int_t);
   void PictToLP();
   void WriteOutCanvas();
   void WriteOutHist();
   void WriteOutCut();
   void Superimpose(Int_t);
   void KolmogorovTest();
   void SetCanvasIsDeleted(){fCanvasIsAlive = kFALSE;};
   void SetSelectedPad(){cHist->cd();};
   void UpdateCanvas(){
      if(!gStyle->GetOptStat()){
         if(cHist->GetPrimitive("stats"))delete cHist->GetPrimitive("stats");
      }
      if(!gStyle->GetOptTitle()){
         if(cHist->GetPrimitive("title"))delete cHist->GetPrimitive("title");
      }
      gPad = (TVirtualPad*)cHist;
      cHist->cd(); cHist->Modified(kTRUE); cHist->Update();
//      cout << " UpdateCanvas() done " << endl;
   };
   void RecursiveRemove(TObject * obj);
   void handle_mouse();
   void DrawTopAxis();
   void ColorMarked();

   void ClearTofl() {
//      cout << "ClearTofl()" << endl;
      fTofLabels = 0;
   }
	void DrawSelectedFunctions();
   void FindPeaks();
   void Calibrate();

   void FastFT();

ClassDef(FitHist,0)      // A histogram presenter
};
#endif
