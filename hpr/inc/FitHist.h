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
#include "HTRootCanvas.h"

//#include "SetColor.h"
//#include "TMrbWdw.h"
//#include "TMrbVarWdwCommon.h"
#include "FhPeak.h"

class TMrbWindow2D;
class TMrbWindow;
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
   Int_t fOldMode;
   TH1         *fSelHist, *fOrigHist;          // pointer to the selected histogram 
   HistPresent* hp;
   HTCanvas *cHist;
   TH1     *expHist, *projHist, *projHistX, *projHistY,*projHistF;
   TString fHname;
   TString fCname;
   TString fCtitle; 
   TString fEname; 
   TString fCutname;
   TString fWdwname;

   TVirtualPad *fSelPad;          // pointer to the selected pad

   TList *fActiveFunctions;
   TList *fAllFunctions;
   TList *fActiveWindows;
   TList *fAllWindows;
   TList *fAllCuts;
   TList *fPeaks;
   TList *fActiveCuts;
   TObjArray *markers;
   TObjArray *peaks;
   TList *fCmdLine;
   Int_t fSerialPx;
   Int_t fSerialPy;
   Int_t fSerialPf;
   Int_t func_numb;
   Int_t cut_numb;
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
   HTRootCanvas *mycanvas;
   TPaveText *datebox;
   Int_t fDimension;
   Bool_t fCanvasIsAlive;
   TString fXtitle; 
   TString fYtitle; 
   TString fFitMacroName; 
   TString fTemplateMacro;
   Int_t fFirstUse; 
   Bool_t fDeleteCalFlag;
   Double_t fLinBgConst;
   Double_t fLinBgSlope;
   HTCanvas * fCutPanel;
public:
   FitHist(const Text_t *name, const Text_t *title, TH1 *hist,
           const Text_t *hname, Int_t win_topx = 520, Int_t win_topy = 5, 
           Int_t win_widx = 750, Int_t win_widy = 550);
   ~FitHist();
   void SaveDefaults(Bool_t recalculate = kTRUE);
   void RestoreDefaults();
   void DisplayHist(TH1*, Int_t, Int_t, Int_t, Int_t);  // display hists
   void DrawHist();                      // 
   void Draw1Dim();                      // 
   void Draw2Dim();                      // 
   void DrawColors();                      // 
   const Char_t * GetCanvasName(){return fCname.Data();};             // 
   HTRootCanvas* GetMyCanvas(){return mycanvas;};  
   HTCanvas* GetCanvas(){return cHist;};  
   void  SetMyCanvas(HTRootCanvas *myc){mycanvas = myc;}; 
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
   void RebinOne();                  // 
   void ExecDefMacro();                  // 
   void ExecFitMacro();                  // 
   void EditFitMacro();                  // 
   void SetTemplateMacro(const char *);                  // 
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
   TObjArray* GetPointertoMarks(){return markers;};
   void AddFunctionsToHist();

   void MarksToWindow();               // 
   void ClearWindows();               // 
   void DrawWindows();               // 
   void WriteOutWindows();               // 
   void ListWindows();                       // 
   void AddWindow(const char *, const char * bp=0);            // 
   void AddWindowsToHist();
   void EditWindow(const char *, const char * bp=0); 

   Int_t Nwindows(){return fActiveWindows->GetSize();};    
   Int_t Ncuts(){return fActiveCuts->GetSize();};                  // 
   void InitCut();                       // 
   void EditCut(const char *, const char * bp); 
   void PrintOneCut(TMrbWindow2D * const);      // 
   void CloseCuts();                       // 
   void ListCuts();                       // 
   void AddCut(const char *, const char * bp);            // 
   Float_t DrawCut();                      // 
   void DrawCutName();                      // 
   void MarksToCut();               // 
   void AddCutsToHist();
   void DrawDateBox(TH1*, Float_t);                      // 
   Bool_t UseCut(TMrbWindow2D * const);         // 
   Bool_t UseWindow(TMrbWindow * const);         // 
   Bool_t InsideCut(Float_t, Float_t);               // 
   void ClearCut();                   // 
   void UpdateCut();                  // 
   Bool_t Its2dim(){return fOrigHist->InheritsFrom("TH2");};               // 
   void FitGBgOnly();                      // fit gaus + lin bg
   void FitGBgTailLow();                      // fit gaus + lin bg + tail
   void FitGBgTailHigh();                      // fit gaus + lin bg + tail
   void FitGBg(Int_t);                     // do the fit 
   Int_t Fit1dim(Int_t, Int_t);            // fit polynoms
   Int_t Fit2dim(Int_t, Int_t);            // fit polynoms
   Int_t FitPolyHist(Int_t);               // fit polynoms hist 
   Int_t FitPolyMarks(Int_t);              // fit  polynoms to marks
   Bool_t SetLinBg();
   Int_t FindPeaks();
   Int_t ShowPeaks();
   void  Expand();                     // expand
   void  ProjectX();
   void  ProjectY();
   void  ProjectBoth();
   void  ProjectF();                   // project along a function 
   void OutputStat();
//   void ListFunctions();
   void WriteFunctions();
   void WriteFunctionList();
   void PictToPSFile(Int_t);
   void PictToLP();
   void WriteOutCanvas();
   void WriteOutHist();
   void WriteOutCut();
//   void WriteHistasASCII(const char *, Bool_t);
   void WriteHistasASCII();
   void Superimpose(Int_t);               
   void WarnBox(const char *);
   void PrintCalib();
   void ClearCalib(){fPeaks->Clear();};
   void SetDeleteCalFlag(){fDeleteCalFlag = kTRUE; SaveDefaults();};
   Bool_t Calibrate();
   void ClearCalibration();
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
  ClassDef(FitHist,0)      // A histogram presenter
};
#endif
