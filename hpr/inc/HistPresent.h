#ifndef HPR_HPR
#define HPR_HPR
//#define MAXHIST 10
#include "TROOT.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TButton.h>
#include <TCanvas.h>
#include <TControlBar.h>
#include <TTree.h>
#include <TList.h>
#include <TSocket.h>
#include "TRootCanvas.h"
#include <iostream>

#include "FitHist.h"
#include "TMrbHelpBrowser.h"

namespace std {} using namespace std;


enum e_HsOp {kHsOp_None, kHsOp_And, kHsOp_Or, kHsOp_Not};
const Int_t MAXCAN=10;
class FitHist;
 
class HistPresent : public TNamed {

friend class FitHist;
friend class HTCanvas;
friend class HandleMenus;

protected:
   TCanvas *cHPr;                 // the main canvas
   TCanvas *filelist;                 // the main canvas
   TFile       *fRootFile;        // root file
   TH1 * fCurrentHist;
   TRootCanvas *maincanvas;
   TRootCanvas *lastcanvas;
   Bool_t fByTitle;
   Int_t fRebin, fRMethod;
   TVirtualPad *activeFile;
   TVirtualPad *activeHist;
   Float_t fOpfac;
   TString *fLeafCut;
   TString *fGraphCut;
   TString *fCutVarX;
   TString *fCutVarY;
   TString *fExpression;
   TString *fFileSelMask;
   TString *fHistSelMask;
   TString *fHistSelMask_1;
   TString *fHistSelMask_2;
   Int_t   fHistSelOp;
   Bool_t fApplyLeafCut;
   Bool_t fUseHist;
   Bool_t fApplyGraphCut;
   Bool_t fApplyExpression;
   TControlBar * fControlBar;
   TList *fCanvasList;
   TList *fAllWindows;
   TList *fAllFunctions;
   TList *fAllCuts;
   TList *fHistLists;
   TList *fSelectHist;
   TList *fHistListList;
   TList *fSelectLeaf;
   TList *fSelectCut;
   TList *fSelectWindow;
   TList *fCmdLine;
   TList *fSelectContour;
   TList *fSelectGraph;
   TList *fAllContours;

   TMrbHelpBrowser * fHelpBrowser;

   Int_t fNwindows; 
   Int_t fWintopx;     // origin of window
   Int_t fWintopy; 
   Int_t fWincury;
   Int_t fWincurx;
   Int_t fWinwidx_2dim;
   Int_t fWinwidy_2dim; 
   Int_t fWinwidx_1dim;
   Int_t fWinwidy_1dim; 
   Int_t fWinshiftx;
   Int_t fWinshifty;
   Int_t fWinwidx_hlist; 
   Int_t fSeqNumberMany;
   Int_t fOptStat;
   Int_t fShowDateBox;
   Int_t fShowStatBox;
   Int_t fUseTimeOfDisplay;
   Int_t fUseRegexp;
   Int_t fShowListsOnly;
   Int_t fShowTitle;
   Int_t fShowFitBox;
   Int_t fLikelyHood;
   Int_t fShowContour; 
   Int_t fFill1Dim;    
   Int_t fFill2Dim;
   Int_t f2DimBackgroundColor;
   Int_t f1DimFillColor;

   Int_t fFitOptLikelihood;  
   Int_t fFitOptQuiet;  
   Int_t fFitOptVerbose;  
   Int_t fFitOptMinos;  
   Int_t fFitOptErrors1;  
   Int_t fFitOptIntegral;  
   Int_t fFitOptNoDraw;  
   Int_t fFitOptAddAll; 
   Int_t fFitOptKeepParameters; 
   Int_t fFitOptUseLinBg;
   TString  fGraphFile;    
   TString  fDrawOptGraph;    
   TString * fDrawOpt2Dim;    
   TString * f2DimColorPalette;    
   Int_t fShowErrors;  
   Int_t fDrawAxisAtTop;
   Int_t fEnableCalibration;
   Int_t fDisplayCalibrated;
   Int_t fShowPSFile;
   Int_t fShowFittedCurves;
   Int_t fRememberTreeHists;
   Int_t fRememberLastSet; 
   Int_t fRememberZoom; 
   Int_t fUseAttributeMacro;
   Int_t fShowAllAsFirst; 
   Int_t fAlwaysNewLimits; 

   Int_t fNofColorLevels;
   Int_t * fPalette;

   Int_t   fNofGreyLevels;
   Int_t * fGreyPalette;
   Int_t * fGreyPaletteInv;
   Int_t   fMaxListEntries;
   TString * fHelpDir;
   Bool_t  fAutoExec_1;
   Bool_t  fAutoExec_2;
   Bool_t  fAutoProj_X;
   Bool_t  fAutoProj_Y;
   Double_t fProjectBothRatio;
   Double_t fLogScaleMin;
   Double_t fLinScaleMin;
   Double_t fAutoUpdateDelay;
   Double_t fPeakThreshold;
   Int_t    fPeakMwidth;
   Int_t    fLiveStat1dim;
   Int_t    fLiveStat2dim;
   Int_t    fLiveGauss;
   Int_t    fLiveBG;

   TButton * fRebin2;
   TButton * fRebin4;
   TButton * fRebinOth;
   TButton * fRebinSumAvg;
   TButton * fValButton;
   TString * fHostToConnect;
   Bool_t    fConnectedOnce;
   Int_t     fSocketToConnect;
   TSocket * fComSocket;
   Bool_t  fAskedFor;
   Bool_t  fSocketIsOpen;
   Bool_t  fAnyFromSocket;
   TGWindow * fLastWindow;
//  Graphics attributes

   Int_t         fLabelMaxDigits;  

   Int_t        fNdivisionsX; 
   Color_t      fAxisColorX;  
   Color_t      fLabelColorX; 
   Style_t      fLabelFontX;  
   Float_t      fLabelOffsetX;
   Float_t      fLabelSizeX;  
   Float_t      fTickLengthX; 
   Float_t      fTitleOffsetX;
   Float_t      fTitleSizeX;  
   Color_t      fTitleColorX; 
   Style_t      fTitleFontX;  

   Int_t        fNdivisionsY; 
   Color_t      fAxisColorY;  
   Color_t      fLabelColorY; 
   Style_t      fLabelFontY;  
   Float_t      fLabelOffsetY;
   Float_t      fLabelSizeY;  
   Float_t      fTickLengthY; 
   Float_t      fTitleOffsetY;
   Float_t      fTitleSizeY;  
   Color_t      fTitleColorY; 
   Style_t      fTitleFontY;  

   Int_t        fNdivisionsZ; 
   Color_t      fAxisColorZ;  
   Color_t      fLabelColorZ; 
   Style_t      fLabelFontZ;  
   Float_t      fLabelOffsetZ;
   Float_t      fLabelSizeZ;  
   Float_t      fTickLengthZ; 
   Float_t      fTitleOffsetZ;
   Float_t      fTitleSizeZ;  
   Color_t      fTitleColorZ; 
   Style_t      fTitleFontZ;  

   Color_t       fHistFillColor;
   Color_t       fHistLineColor;
   Style_t       fHistFillStyle;
   Style_t       fHistLineStyle;
   Width_t       fHistLineWidth;
   Float_t       fEndErrorSize;
   Float_t       fErrorX;      
   Color_t       fFuncColor;   
   Style_t       fFuncStyle;   
   Width_t       fFuncWidth;   

   Color_t       fStatColor;     
   Color_t       fStatTextColor; 
   Width_t       fStatBorderSize;
   Style_t       fStatFont;      
   Float_t       fStatFontSize;  
   Style_t       fStatStyle;     
   Float_t       fStatX;         
   Float_t       fStatY;         
   Float_t       fStatW;         
   Float_t       fStatH;

   Color_t       fPadColor;       
   Width_t       fPadBorderSize;  
   Int_t         fPadBorderMode;  
   Float_t       fPadBottomMargin;
   Float_t       fPadTopMargin;   
   Float_t       fPadLeftMargin;  
   Float_t       fPadRightMargin; 
   Bool_t        fPadGridX;       
   Bool_t        fPadGridY;       
   Int_t         fPadTickX;       
   Int_t         fPadTickY;  
	Color_t       fFrameFillColor;    //pad frame fill color
	Color_t       fFrameLineColor;    //pad frame line color
	Style_t       fFrameFillStyle;    //pad frame fill style
	Style_t       fFrameLineStyle;    //pad frame line style
	Width_t       fFrameLineWidth;    //pad frame line width
	Width_t       fFrameBorderSize;   //pad frame border size
	Int_t         fFrameBorderMode;   //pad frame border mode
	Color_t       fCanvasColor;       //canvas color
	Width_t       fCanvasBorderSize;  //canvas border size
	Int_t         fCanvasBorderMode;  //canvas border mode
	Int_t         fCanvasDefH;        //default canvas height
	Int_t         fCanvasDefW;        //default canvas width
	Int_t         fCanvasDefX;        //default canvas top X position
	Int_t         fCanvasDefY;        //default canvas top Y position

   Color_t       fTitleColor;     
   Color_t       fTitleTextColor; 
   Width_t       fTitleBorderSize;
   Style_t       fTitleFont;      
   Float_t       fTitleFontSize;  
   Style_t       fTitleStyle;     
   Float_t       fTitleX;         
   Float_t       fTitleY;         
   Float_t       fTitleW;         
   Float_t       fTitleH;  
   Int_t         fForceStyle;  
                       
    Float_t fXXXXX;

public:
enum EHfromASCIImode { kNotDefined, kSpectrum, kSpectrumError, k1dimHist, k1dimHistWeight,
                       k2dimHist, k2dimHistWeight, kGraph, kGraphError, kGraphAsymmError};

   HistPresent(const Text_t *name = "mypres" , const Text_t *title = "mypres");
   ~HistPresent();
   void RecursiveRemove(TObject *);
   TRootCanvas* GetMyCanvas(){return lastcanvas;};  
   void  SetMyCanvas(TRootCanvas *myc){lastcanvas = myc;}; 
   void SaveOptions();
   void RestoreOptions();
   void EditAttrFile();
   void Editrootrc();
   void CloseAllCanvases();
   void CloseHistLists();
   void SetRebinValue(Int_t);               // 
   void SetRebinMethod();               // 
   void DiffHist();                 // 
   void RebinHist();               // 
   void OperateHist(Int_t);               // 
   void SetOperateVal();               // 
   void ListSelect(); 
   void SetPadAtt();
   void SetTitleAtt();
   void SetHistAtt();
   void SetStatAtt();
   void SetAxisAtt();
   void SetPadAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetTitleAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetHistAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetStatAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetXaxisAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetYaxisAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetZaxisAttributes(TGWindow * win = 0, FitHist * fh = 0);
   void SetDisplayOptions(TGWindow * win = 0, FitHist * fh = 0);  
   void Set1DimOptions(TGWindow * win = 0, FitHist * fh = 0);  
   void Set2DimOptions(TGWindow * win = 0, FitHist * fh = 0);  
   void SetGraphOptions(TGWindow * win = 0, TCanvas * ca = 0);  
   void SetVariousOptions(TGWindow * win = 0, FitHist * fh = 0);
   void SetWindowSizes(TGWindow * win = 0, FitHist * fh = 0);
   void SetFontsAndColors(TGWindow * win = 0, FitHist * fh = 0);
   void SetNumericalOptions(TGWindow * win = 0, FitHist * fh = 0);
   void SetFittingOptions(TGWindow * win = 0, FitHist * fh = 0);
   void SetStatDefaults(TCanvas *);
//   void CheckAutoExecFiles();
   void ClearSelect(); 
   void ShowMain();               // 

   void ListMacros(const char* bp =0);               // 
   void GetFileSelMask(const char* bp =0);               // 
   void GetHistSelMask(const char* bp =0);               // 
   void SelectHist(const char* , const char*, const char* bp =0); 
   void SelectCut(const char* , const char*, const char* bp =0); 
   void SelectContour(const char* , const char*, const char* bp =0); 
   void SelectGraph(const char* , const char*, const char* bp =0); 
   void SelectLeaf(const char*, const char* bp =0); 
   void EditLeafCut(const char* bp = 0); 
   void ToggleLeafCut(const char* bp = 0); 
   void UseHist(const char* bp =0);  
   void ToggleUseHist(const char* bp = 0);  
   void DefineGraphCut(const char* bp =0);  
   void ToggleGraphCut(const char* bp =0);  
   void EditExpression(const char* bp =0);  
   void ToggleExpression(const char* bp =0);  
   void ShowFiles(const char* , const char* bp =0);               // 
   void ShowContents(const char* , const char* bp =0);    // 
   void ShowFunction(const char* , const char*, const char* bp =0); 
   void LoadFunction(const char* , const char*, const char* bp =0); 
   void ShowCanvas(const char* , const char*, const char* bp =0); 
   void ShowContour(const char* , const char*, const char* bp =0); 
   void ShowGraph(const char* , const char*, const char* bp =0); 
   void ComposeList(const char* bp =0); 
   void ShowList(const char* , const char*, const char* bp =0); 
   void PrintCut(const char* , const char*, const char* bp =0); 
   void LoadCut(const char* , const char*, const char* bp =0); 
   void CutsToASCII(const char *, const char* bp =0); 
   void PrintWindow(const char* , const char*, const char* bp =0); 
   void LoadWindow(const char* , const char*, const char* bp =0); 
//   void ShowMap(const char* , const char*);  
   void ShowHist(const char* , const char*, const char* bp =0); 
   void ShowStatOfAll(const char* , const char* bp =0); 
   void PurgeEntries(const char* , const char* bp =0); 
   void DeleteSelectedEntries(const char* , const char* bp =0); 
   void ShowSelectedHists(const char* bp =0);
   void ShowInOneCanvas(const char* bp =0);
   void ShowTree(const char* , const char*, const char* bp =0);
   void ShowLeaf(const char* , const char*, const char* l=0, const char* bp =0);  // display hist of leaf

   void CutsFromASCII(TGWindow * win = 0); 
   void HistFromASCII(TGWindow * win = 0, EHfromASCIImode mode = kNotDefined); 
   void SaveMap(const char*, const char* bp =0);  
   void SaveFromSocket(const char*, const char* bp =0);  
   TH1*  GetHist(const char* , const char*); 
   FitHist * ShowHist(TH1*, const char* origname=0);  
   TList* GetCanvasList(){return fCanvasList;};
   TList* GetHistList(){return fHistLists;};
   TList* GetSelectedHist(){return fSelectHist;};
   TList* GetWindowList(){return fAllWindows;};
   TList* GetFunctionList(){return fAllFunctions;};
   TList* GetCutList(){return fAllCuts;};
   TH1* GetSelHistAt(Int_t, TList * hl = 0);
   void TurnButtonGreen(TVirtualPad **);
   TList* GetSelections(){return fSelectHist;};
   void ShowSelectedHists(TList *, const char* title =0);

   TH1 *  GetCurrentHist(){return fCurrentHist;};
   void   SetCurrentHist(TH1 * hist){fCurrentHist = hist;};
   Int_t  GetOptStat(){return fOptStat;};
   Bool_t GetRememberLastSet(){return fRememberLastSet;};
   Bool_t GetShowDateBox(){return fShowDateBox;};
   Bool_t GetShowStatBox(){return fShowStatBox;};
   Bool_t GetUseTimeOfDisplay(){return fUseTimeOfDisplay;};
   Bool_t GetShowTitle(){return fShowTitle;};
   Bool_t GetShowFit(){return fShowFitBox;};
   Bool_t GetShowPSFile(){return fShowPSFile;};
   Bool_t GetShowFittedCurves(){return fShowFittedCurves;};
   Bool_t GetEnableCalibration(){return fEnableCalibration;};
   Bool_t GetAutoExec_1(){return fAutoExec_1;};
   Double_t GetAutoUpdateDelay(){return fAutoUpdateDelay;};
   Bool_t GetAutoExec_2(){return fAutoExec_2;};
   Bool_t GetAutoProj_X(){return fAutoProj_X;};
   Bool_t GetAutoProj_Y(){return fAutoProj_Y;};
   TMrbHelpBrowser * GetHelpBrowser(){return fHelpBrowser;};
   void auto_exec_1();
   void auto_exec_2();
   void WarnBox(const char *);
   void CleanWindowLists(TH1* hist);

ClassDef(HistPresent,0)      // A histogram presenter
};
#endif
