//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             e.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .x e.C(Int_t CalSource,
//                           const Char_t * Energies,
//                           Bool_t VerboseMode,
//                           TObjArray * HistoFile,
//                           const Char_t * CalFile,
//                           const Char_t * ResFile,
//                           const Char_t * FitFile,
//                           Bool_t ClearFlag,
//                           Int_t Rebin,
//                           Double_t Region1,
//                           Double_t Region2,
//                           Double_t Region3,
//                           Double_t Efficiency,
//                           Int_t PeakFrac,
//                           Double_t Sigma,
//                           Double_t TwoPeakSep,
//                           Int_t FitMode,
//                           Int_t FitGrouping,
//                           Int_t FitBackground,
//                           Double_t FitRange,
//                           Double_t MatchOffset,
//                           Double_t MatchGain,
//                           Int_t DisplayMode,
// Arguments:        Int_t CalSource           -- Calibration source
//                   Char_t * Energies         -- Calibration energies
//                   Bool_t VerboseMode        -- Verbose mode
//                   TObjArray * HistoFile     -- Histogram file (.root)
//                   Char_t * CalFile          -- Calibration data (.cal)
//                   Char_t * ResFile          -- Calibration results (.res)
//                   Char_t * FitFile          -- Fit results (.root)
//                   Bool_t ClearFlag          -- Clear output files (.cal, .res)
//                   Int_t Rebin               -- Rebin histograms
//                   Double_t Region1          -- Region-1: lower, upper
//                   Double_t Region2          -- Region-2: lower, upper
//                   Double_t Region3          -- Region-3: lower, upper
//                   Double_t Efficiency       -- Efficiency function: yeff(x) = a0 * exp(a1 * y(x))
//                   Int_t PeakFrac            -- Threshold [% of max peak]
//                   Double_t Sigma            -- Sigma
//                   Double_t TwoPeakSep       -- Two-peak separation [sigma]
//                   Int_t FitMode             -- Fit mode
//                   Int_t FitGrouping         -- Fit grouping
//                   Int_t FitBackground       -- Background
//                   Double_t FitRange         -- Range [sigma]
//                   Double_t MatchOffset      -- Offset for peak match: min, step, max
//                   Double_t MatchGain        -- Gain for peak match: min, step, max
//                   Int_t DisplayMode         -- Display results
// Description:      Energy calibration for 1-dim histograms
// Author:           Marabou
// Mail:             
// URL:              
// Revision:         $Id: Encal.C,v 1.22 2007-08-22 13:43:28 Rudolf.Lutter Exp $
// Date:             Tue Aug 21 10:40:52 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               1200
//                   Aclic:               +g
//                   Modify:              yes
//                   AddGuiPtr:           yes
//                   RcFile:              .EncalLoadLibs.C
//                   NofArgs:             32
//                   Arg1.Name:           Section_IC
//                   Arg1.Title:          1. Initialization
//                   Arg1.Type:           Int_t
//                   Arg1.EntryType:      GroupFrame
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           CalSource
//                   Arg2.Title:          Calibration source
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        1
//                   Arg2.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source=2:3Alpha|calibrate using alpha source (Pu239/Am241/Cm244)=4:Other|user-defined calibraion=8
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Energies
//                   Arg3.Title:          Calibration energies
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      File
//                   Arg3.Default:        $MARABOU/data/encal/energies.dat
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           VerboseMode
//                   Arg4.Title:          Verbose mode
//                   Arg4.Type:           Bool_t
//                   Arg4.EntryType:      YesNo
//                   Arg4.Default:        noGetArg
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           Section_FH
//                   Arg5.Title:          2. Files & histograms
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      GroupFrame
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           HistoFile
//                   Arg6.Title:          Histogram file (.root)
//                   Arg6.Type:           TObjArray *
//                   Arg6.EntryType:      FObjListBox
//                   Arg6.Width:          300
//                   Arg6.Default:        none.root
//                   Arg6.Values:         ROOT files:*.root
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           CalFile
//                   Arg7.Title:          Calibration data (.cal)
//                   Arg7.Type:           Char_t *
//                   Arg7.EntryType:      File
//                   Arg7.Width:          150
//                   Arg7.Default:        Encal.cal
//                   Arg7.Values:         Calib files:*.cal
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           ResFile
//                   Arg8.Title:          Calibration results (.res)
//                   Arg8.Type:           Char_t *
//                   Arg8.EntryType:      File
//                   Arg8.Width:          150
//                   Arg8.Default:        Encal.res
//                   Arg8.Values:         Result files:*.res
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           FitFile
//                   Arg9.Title:          Fit results (.root)
//                   Arg9.Type:           Char_t *
//                   Arg9.EntryType:      File
//                   Arg9.Width:          150
//                   Arg9.Default:        Encal.root
//                   Arg9.Values:         Fit data files:*.root
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          ClearFlag
//                   Arg10.Title:         Clear output files (.cal, .res)
//                   Arg10.Type:          Bool_t
//                   Arg10.EntryType:     YesNo
//                   Arg10.Default:       no
//                   Arg10.AddLofValues:  No
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          NP1
//                   Arg11.Title:         NP1
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     Pad
//                   Arg11.AddLofValues:  No
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          Section_PFND
//                   Arg12.Title:         3. Peak Finder
//                   Arg12.Type:          Int_t
//                   Arg12.EntryType:     GroupFrame
//                   Arg12.AddLofValues:  No
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          Rebin
//                   Arg13.Title:         Rebin histograms
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     UpDown
//                   Arg13.Default:       1
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          Region1
//                   Arg14.Title:         Region-1: lower, upper
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     UpDown-MXC
//                   Arg14.NofEntryFields:2
//                   Arg14.Width:         150
//                   Arg14.Default:       0:0:F
//                   Arg14.AddLofValues:  No
//                   Arg14.LowerLimit:    0:0
//                   Arg14.UpperLimit:    65535:65535
//                   Arg14.Increment:     100:100
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          Region2
//                   Arg15.Title:         Region-2: lower, upper
//                   Arg15.Type:          Int_t
//                   Arg15.EntryType:     UpDown-MXC
//                   Arg15.NofEntryFields:2
//                   Arg15.Width:         150
//                   Arg15.Default:       0:0:F
//                   Arg15.AddLofValues:  No
//                   Arg15.LowerLimit:    0:0
//                   Arg15.UpperLimit:    65535:65535
//                   Arg15.Increment:     100:100
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//                   Arg16.Name:          Region3
//                   Arg16.Title:         Region-3: lower, upper
//                   Arg16.Type:          Int_t
//                   Arg16.EntryType:     UpDown-MXC
//                   Arg16.NofEntryFields:2
//                   Arg16.Width:         150
//                   Arg16.Default:       0:0:F
//                   Arg16.AddLofValues:  No
//                   Arg16.LowerLimit:    0:0
//                   Arg16.UpperLimit:    65535:65535
//                   Arg16.Increment:     100:100
//                   Arg16.Base:          dec
//                   Arg16.Orientation:   horizontal
//                   Arg17.Name:          Efficiency
//                   Arg17.Title:         Efficiency function: yeff(x) = a0 * exp(a1 * y(x))
//                   Arg17.Type:          Double_t
//                   Arg17.EntryType:     Entry-MC
//                   Arg17.NofEntryFields:2
//                   Arg17.Width:         100
//                   Arg17.Default:       0:0:F
//                   Arg17.AddLofValues:  No
//                   Arg17.LowerLimit:    0:0
//                   Arg17.UpperLimit:    1000:0.1
//                   Arg17.Increment:     10:0.05
//                   Arg17.Base:          dec
//                   Arg17.Orientation:   horizontal
//                   Arg18.Name:          PeakFrac
//                   Arg18.Title:         Threshold [% of max peak]
//                   Arg18.Type:          Int_t
//                   Arg18.EntryType:     UpDown-X
//                   Arg18.Default:       1
//                   Arg18.AddLofValues:  No
//                   Arg18.LowerLimit:    1
//                   Arg18.UpperLimit:    50
//                   Arg18.Increment:     1
//                   Arg18.Base:          dec
//                   Arg18.Orientation:   horizontal
//                   Arg19.Name:          Sigma
//                   Arg19.Title:         Sigma
//                   Arg19.Type:          Double_t
//                   Arg19.EntryType:     UpDown-X
//                   Arg19.Default:       3
//                   Arg19.AddLofValues:  No
//                   Arg19.LowerLimit:    0
//                   Arg19.UpperLimit:    10
//                   Arg19.Increment:     .2
//                   Arg19.Base:          dec
//                   Arg19.Orientation:   horizontal
//                   Arg20.Name:          TwoPeakSep
//                   Arg20.Title:         Two-peak separation [sigma]
//                   Arg20.Type:          Double_t
//                   Arg20.EntryType:     UpDown-X
//                   Arg20.Default:       1
//                   Arg20.AddLofValues:  No
//                   Arg20.LowerLimit:    1
//                   Arg20.UpperLimit:    10
//                   Arg20.Base:          dec
//                   Arg20.Orientation:   horizontal
//                   Arg21.Name:          Section_PFIT
//                   Arg21.Title:         4. Peak Fit
//                   Arg21.Type:          Int_t
//                   Arg21.EntryType:     GroupFrame
//                   Arg21.AddLofValues:  No
//                   Arg21.Base:          dec
//                   Arg21.Orientation:   horizontal
//                   Arg22.Name:          FitMode
//                   Arg22.Title:         Fit mode
//                   Arg22.Type:          Int_t
//                   Arg22.EntryType:     Radio
//                   Arg22.Default:       1
//                   Arg22.Values:        gauss|fit symmetric gaussian=1:left tail|fit gaussian + exp tail on left side=2:right tail|fit gaussian + exp tail on right side=4
//                   Arg22.AddLofValues:  No
//                   Arg22.Base:          dec
//                   Arg22.Orientation:   horizontal
//                   Arg23.Name:          FitGrouping
//                   Arg23.Title:         Fit grouping
//                   Arg23.Type:          Int_t
//                   Arg23.EntryType:     Radio
//                   Arg23.Default:       1
//                   Arg23.Values:        single peak|fit each peak separately=1:ensemble|group peaks before fitting=2
//                   Arg23.AddLofValues:  No
//                   Arg23.Base:          dec
//                   Arg23.Orientation:   horizontal
//                   Arg24.Name:          FitBackground
//                   Arg24.Title:         Background
//                   Arg24.Type:          Int_t
//                   Arg24.EntryType:     Radio
//                   Arg24.Default:       1
//                   Arg24.Values:        linear|fit linear Background=1:const|fit constant background=2
//                   Arg24.AddLofValues:  No
//                   Arg24.Base:          dec
//                   Arg24.Orientation:   horizontal
//                   Arg25.Name:          FitRange
//                   Arg25.Title:         Range [sigma]
//                   Arg25.Type:          Double_t
//                   Arg25.EntryType:     UpDown
//                   Arg25.Default:       3
//                   Arg25.AddLofValues:  No
//                   Arg25.LowerLimit:    0
//                   Arg25.UpperLimit:    10
//                   Arg25.Increment:     .5
//                   Arg25.Base:          dec
//                   Arg25.Orientation:   horizontal
//                   Arg26.Name:          Section_PM
//                   Arg26.Title:         5. Peak Matching
//                   Arg26.Type:          Int_t
//                   Arg26.EntryType:     GroupFrame
//                   Arg26.AddLofValues:  No
//                   Arg26.Base:          dec
//                   Arg26.Orientation:   horizontal
//                   Arg27.Name:          MatchOffset
//                   Arg27.Title:         Offset: min, step, max
//                   Arg27.Type:          Double_t
//                   Arg27.EntryType:     UpDown-M
//                   Arg27.NofEntryFields:3
//                   Arg27.Width:         100
//                   Arg27.Default:       0:0:0
//                   Arg27.AddLofValues:  No
//                   Arg27.LowerLimit:    0:1:10000
//                   Arg27.UpperLimit:    10000:100:10000
//                   Arg27.Increment:     1:.1:1
//                   Arg27.Base:          dec
//                   Arg27.Orientation:   horizontal
//                   Arg28.Name:          MatchGain
//                   Arg28.Title:         Gain: min, step, max
//                   Arg28.Type:          Double_t
//                   Arg28.EntryType:     UpDown-M
//                   Arg28.NofEntryFields:3
//                   Arg28.Default:       1:1:100
//                   Arg28.Width:         100
//                   Arg28.AddLofValues:  No
//                   Arg28.LowerLimit:    1:.1:100
//                   Arg28.UpperLimit:    100:1:100
//                   Arg28.Increment:     1:.1:1
//                   Arg28.Base:          dec
//                   Arg28.Orientation:   horizontal
//                   Arg29.Name:          Section_CAL
//                   Arg29.Title:         6. Calibration
//                   Arg29.Type:          Int_t
//                   Arg29.EntryType:     GroupFrame
//                   Arg29.AddLofValues:  No
//                   Arg29.Base:          dec
//                   Arg29.Orientation:   horizontal
//                   Arg30.Name:          DisplayMode
//                   Arg30.Title:         Display results
//                   Arg30.Type:          Int_t
//                   Arg30.EntryType:     Check
//                   Arg30.Default:       1
//                   Arg30.Values:        step|show each fit=1:2dim|show 2-dim histo after calibration=2
//                   Arg30.AddLofValues:  No
//                   Arg30.Base:          dec
//                   Arg30.Orientation:   horizontal
//                   Arg31.Name:          Section_CTRL
//                   Arg31.Title:         Control
//                   Arg31.Type:          Int_t
//                   Arg31.EntryType:     GroupFrame
//                   Arg31.AddLofValues:  No
//                   Arg31.Base:          dec
//                   Arg31.Orientation:   horizontal
//                   Arg32.Name:          CtrlButtons
//                   Arg32.Title:         @
//                   Arg32.Type:          Int_t
//                   Arg32.EntryType:     TextButton
//                   Arg32.Values:        Prev:Same:Next/ok:Next/discard:Stop
//                   Arg32.AddLofValues:  No
//                   Arg32.Base:          dec
//                   Arg32.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <fstream>
#include "TMath.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TVirtualX.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TEnv.h"
#include "TGFrame.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TPolyMarker.h"
#include "TGMrbMacroBrowser.h"
#include "TMrbLogger.h"
#include "FhPeak.h"
#include "FindPeakDialog.h"
#include "FitOneDimDialog.h"
#include "CalibrationDialog.h"
#include "SetColor.h"

//______________________________________________________[C++ ENUM DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           Enum definitions
//////////////////////////////////////////////////////////////////////////////

enum EEncalBrowserBits {	kCalSourceCo60 = 1,
							kCalSourceEu152 = 2,
							kCalSource3Alpha = 4,
							kCalSourceOther = 8,
							kFitModeGauss = 1,
							kFitModeLeftTail = 2,
							kFitModeRightTail = 4,
							kFitModeBackgroundLinear = 1,
							kFitModeBackgroundConst = 2,
							kFitGroupingSinglePeak = 1,
							kFitGroupingEnsemble = 2,
							kDisplayFlagStep = 1,
							kDisplayFlag2dim = 2
						};

enum EEncalColors		{	kColorBlack = 1,
							kColorWhite = 10,
							kColorRed = 2,
							kColorGreen = 3,
							kColorBlue = 4,
							kColorGray = 32
						};

enum					{	kCanvasWidth = 800 };
enum					{	kCanvasHeight = 600 };

enum					{	kNofRegions = 4 };

enum EEncalFitStatus	{	kFitDiscard = 0,
							kFitOk,
							kFitAuto
						};

enum EEncalButtons		{	kButtonPrev = TGMrbMacroFrame::kGMrbMacroIdUserButton,
							kButtonSame,
							kButtonNext,
							kButtonDiscard,
							kButtonStop
						};

enum EEncalHisto		{	kPrevHisto,
							kSameHisto,
							kNextHisto
						};

//__________________________________________________________[C++ DATA SECTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           (Global) data section
//////////////////////////////////////////////////////////////////////////////

TMrbLogger * msg = NULL;

Int_t fitNofPeaks = 1;
Int_t fitBinWidth = 1;
Int_t fitTailSide = 1;

Bool_t fButtonFlag = kFALSE;
Bool_t fButtonNext = kTRUE;
Bool_t fButtonOk = kTRUE;
Bool_t fButtonStop = kFALSE;

EEncalHisto fNextHisto = kNextHisto;

TObjArray * fLofHistos;
Int_t fCalSource;
TString fEnergies;
TString fCalFile;
TString fResFile;
TString fFitFile;
Bool_t fClearFlag;
Int_t fRebin;
Int_t fNofRegions;
Int_t fLowerLim[kNofRegions];
Int_t fUpperLim[kNofRegions];
Int_t fMinX;
Int_t fMaxX;
Double_t fEfficiencyA0;
Double_t fEfficiencyA1;
Int_t fPeakFrac;
Double_t fSigma;
Double_t fTwoPeakSep;
Int_t fFitMode;
Int_t fFitGrouping;
Int_t fFitBackground;
Double_t fFitRange;
Double_t fMatchOffsetMin;	// peak match
Double_t fMatchOffsetStep;
Double_t fMatchOffsetMax;
Double_t fMatchGainMin;
Double_t fMatchGainStep;
Double_t fMatchGainMax;
Int_t fDisplayMode;
Bool_t fVerboseMode;

TGMrbMacroFrame * fGuiPtr = NULL;

TString fSourceName; 	// calibration source
TEnv * fEnvEnergies;	// calibration energies
TEnv * fEnvCalib;		// calibration data
TEnv * fEnvResults;		// detailed calibration results
TFile * fFitResults;	// histograms + fits

Int_t fNofPeaksNeeded; 	// peaks needed for given calibration
Bool_t fEnableCalib;	// kTRUE if we have to calibrate

TCanvas * fMainCanvas;	// canvas to display histo + fit
TCanvas * f2DimCanvas;	// 2nd canvas: 2-dim histo

TFile * fHistoFile; 	// root file containing histograms
TObjString * fHistoPtr = NULL; // pointer to list of histo names
TH1F * fCurHisto;		// current histogram
Int_t fNofHistos;		// number of histograms (selected from root file)
Int_t fNofHistosCalibrated;	// number of histograms calibrated

TH2S * f2DimFitResults;	// 2-dim histo to show re-calibrated histograms

Int_t fNofPeaks;		// peak finder results
TList * fPeakList;

Int_t fXmin;			// energy calibration
Double_t fEmin;
Int_t fXmax;
Double_t fEmax;
TF1 * fPoly;

TArrayF fCalX;
TArrayF fCalE;
TArrayF fCalXerr;
TArrayF fCalEerr;

Int_t fFitStatus;		// fit quality
TString fReason;

TObjArray * fParamNames;	// param names for gauss/tail/background fit

FindPeakDialog * fPeakFinder = NULL;
FitOneDimDialog * fFitOneDim = NULL;
CalibrationDialog * fCalibration = NULL;

TString fGaugeFile = "gauge.dat"; // file name to be used with CalibrationDialog()

//____________________________________________________________[C++ PROTOTYPES]
//////////////////////////////////////////////////////////////////////////////
// Name:           Prototype definitions
//////////////////////////////////////////////////////////////////////////////

void UpdateArguments();
Bool_t OpenHistoFile();
Bool_t OpenCalFiles();
Bool_t SetCalSource();
TH1F * GetNextHisto();
Bool_t FindPeaks();
void Calibrate();
Int_t GetNofHistosCalibrated();
void WaitForButtonPressed(Bool_t StepFlag = kFALSE);
void ClearCanvas(Int_t PadNo);
void OutputMessage(const Char_t * Method, const Char_t * Text, Bool_t ErrFlag = kFALSE, Int_t ColorIndex = -1);
TCanvas * OpenCanvas();
void CloseCanvas() { if (fMainCanvas) fMainCanvas->Close(); if (f2DimCanvas) f2DimCanvas->Close(); }
#if 0
void WriteResults();
void WriteCalibration();
#endif
Bool_t WriteGaugeFile();
void UpdateStatusLine();
void WriteMinMax();
void SetFitStatus(Int_t FitStatus, const Char_t * Reason = NULL);
void ShowResults2dim();
void CloseEnvFiles();
void CloseRootFile();
void SetFullRange(TH1F * Histo);
void Stop();
void Exit();

//______________________________________________________[C++ INLINE FUNCTIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           Inline functions
//////////////////////////////////////////////////////////////////////////////

Int_t GetNofHistos() { return(fNofHistos); };
const Char_t * SourceName() { return(fSourceName.Data()); };
Bool_t IsTripleAlpha() { return(fCalSource == kCalSource3Alpha); };
Bool_t IsCo60() { return(fCalSource == kCalSourceCo60); };
Bool_t IsEu152() { return(fCalSource == kCalSourceEu152); };
Bool_t IsOtherCalSource() { return(fCalSource == kCalSourceOther); };

TObjArray * GetLofHistos() { return(fLofHistos); };
void ResetLofHistos() { fHistoPtr = NULL; };
void SetNofRegions(Int_t NofRegions) { fNofRegions = NofRegions; };
Int_t GetNofRegions() { return(fNofRegions); };
Int_t GetNofPeaks() { return(fNofPeaks); };
Int_t GetNofPeaksNeeded() { return(fNofPeaksNeeded); };
Int_t GetFitMode() { return(fFitMode); };
Int_t GetFitGrouping() { return(fFitGrouping); };
Bool_t StepMode() { return((fDisplayMode & kDisplayFlagStep) != 0); };
Bool_t TwoDimMode() { return((fDisplayMode & kDisplayFlag2dim) != 0); };
Bool_t IsVerbose() { return(fVerboseMode); };
void SetVerboseMode(Bool_t Flag) { fVerboseMode = Flag; };
Int_t GetLowerLim(Int_t Region = 0) { return(fLowerLim[Region]); };
Int_t GetUpperLim(Int_t Region = 0) { return(fUpperLim[Region]); };
TFile * HistoFile() { return(fHistoFile); };
void SetCurHisto(TH1F * Histo) { fCurHisto = Histo; };

void OutputMessage(const Char_t * Method, const Char_t * Text, Bool_t ErrFlag, Int_t ColorIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           OutputMessage
// Purpose:        Write message to cout/cerr
// Arguments:      Char_t * Method   -- method
//                 Char_t * Text     -- message text
//                 Bool_t ErrFlag    -- kTRUE if error message
//                 Int_t ColorIndex  -- foreground color
// Results:        --
// Description:    Outputs a message
//////////////////////////////////////////////////////////////////////////////

	const Char_t * tCol = setblack;
	Int_t fgCol = 1;
	TString method;
	TString cName;
	if (Method == NULL || *Method == '\0') {
		cName = "Encal.C";
		method = "";
	} else {
		cName = "Encal";
		method = Method;
	}

	switch (ColorIndex) {
		case kColorRed: tCol = setred; fgCol = 1; break;
		case kColorGreen: tCol = setgreen; fgCol = 1; break;
		case kColorBlue: tCol = setblue; fgCol = 10; break;
	}
	if (ErrFlag) {
		msg->Err() << Text << endl;
		msg->Flush(cName.Data(), method.Data());
	} else {
		msg->Out() << Text << endl;
		msg->Flush(cName.Data(), method.Data(), tCol);
	}
}

Double_t gauss_lbg(Double_t * x, Double_t * par) {
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gauss_lbg
// Purpose:        fit linear/const background + N gaussians
// Description:    par[0]   background constant
//                 par[1]   background slope
//                 par[2]   gauss width
//                 par[3]   gauss0 constant
//                 par[4]   gauss0 mean
//                 par[5]   gauss1 constant
//                 par[6]   gauss1 mean
//                 par[7]   gauss2 constant
//                 par[8]   gauss2 mean
//                 ...
//////////////////////////////////////////////////////////////////////////////

   static Float_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[0];
   Double_t bgslope = par[1];
   Double_t sigma   = par[2];
   if (sigma == 0) sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i ++) {
      arg = (x[0] - par[4 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

Double_t gauss_tail(Double_t * x, Double_t * par) {
//___________________________________________________________[STATIC FUNCTION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gauss_tail
// Purpose:        fit linear/const background + tail on left/right side + N gaussians
// Description:    par[0]   tail fraction
//                 par[1]   tail width
//                 par[2]   background constant
//                 par[3]   background slope
//                 par[4]   gauss0 width
//                 par[5]   gauss0 constant
//                 par[6]   gauss0 mean
//                 par[7]   gauss1 constant
//                 par[8]   gauss1 mean
//                 par[9]   gauss2 constant
//                 par[10]  gauss2 mean
//                 ...
//////////////////////////////////////////////////////////////////////////////

   static Double_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t xij;
   Double_t arg;
   Double_t tail;
   Double_t g2b;
   Double_t err;
   Double_t norm;
   Double_t fitval = 0;
   Double_t tailfrac  = par[0];
   Double_t tailwidth = par[1];
   Double_t bgconst   = par[2];
   Double_t bgslope   = par[3];
   Double_t sigma = par[4];

//  force widths /= 0
   if (tailwidth == 0)
      tailwidth = 1;
   if (sigma == 0)
      sigma = 1;

   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i++) {
      xij = (x[0] - par[6 + 2 * i]);
      arg = xij / (sqrt2 * sigma);
      xij *= fitTailSide;
      tail = exp(xij / tailwidth) / tailwidth;
      g2b = 0.5 * sigma / tailwidth;
      err = 0.5 * tailfrac * par[5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
      norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
      fitval = fitval + norm * err * tail 
                      + par[5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

Bool_t OpenHistoFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           OpenHistoFile
// Purpose:        Open ROOT file and extract histo list
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Extracts file name and histo names
//////////////////////////////////////////////////////////////////////////////

	Int_t fNofHistos = fLofHistos->GetEntriesFast();
	if (fNofHistos == 0) {
		OutputMessage("OpenHistoFile", "No histogram file given", kTRUE);
		return(kFALSE);
	}

	TObjString * hStr = (TObjString *) fLofHistos->At(0);
	TString hFileName = hStr->GetString();
	hFileName = hFileName.Strip(TString::kBoth);

	if (hFileName.IsNull()) {
		OutputMessage("OpenHistoFile", "No histogram file given", kTRUE);
		return(kFALSE);
	}

	fHistoFile = new TFile(hFileName.Data());
	if (!fHistoFile->IsOpen()) {
		OutputMessage("OpenHistoFile", Form("Can't open histogram file - %s", hFileName.Data()), kTRUE);
		return(kFALSE);
	}

	if (fNofHistos == 1) {
		OutputMessage("OpenHistoFile", Form("File %s - no histogram(s) selected", hFileName.Data()), kTRUE);
		return(kFALSE);
	}

	fLofHistos->Remove(hStr);
	fNofHistos--;
	fNofHistosCalibrated = 0;

	ResetLofHistos();

	return(kTRUE);
}

TH1F * GetNextHisto() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           GetNextHisto
// Purpose:        Fetch next histogram from list
// Arguments:      --
// Results:        TH1F * Histo    -- ptr to histo
// Description:    Reads next histo name from list
//////////////////////////////////////////////////////////////////////////////

	if (fHistoPtr) UpdateArguments();

	if (fHistoPtr == NULL) {
		fHistoPtr = (TObjString *) fLofHistos->At(1);
	} else if (fNextHisto == kPrevHisto) {
		fHistoPtr = (TObjString *) fLofHistos->Before(fHistoPtr);
		if (fHistoPtr == NULL) fHistoPtr = (TObjString *) fLofHistos->At(1);
	} else if (fNextHisto == kNextHisto) {
		fHistoPtr = (TObjString *) fLofHistos->After(fHistoPtr);
	}
	if (fHistoPtr) {
		TString hName = fHistoPtr->GetString();
		if (fFitResults) {
			TString hNameAll = hName;
			hNameAll += "*";
			fFitResults->Delete(hNameAll.Data());
		}
		TH1F * h = (TH1F *) fHistoFile->Get(hName.Data());
		if (h) {
			h->GetListOfFunctions()->Delete();
		} else {
			OutputMessage("GetNextHisto", Form("No such histogram - %s:%s", fHistoFile->GetName(), hName.Data()), kTRUE);
		}
		return(h);
	}
	return(NULL);
}

Bool_t OpenCalFiles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           OpenCalFiles
// Purpose:        Open calibration files
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Opens calibration files
//////////////////////////////////////////////////////////////////////////////

	if (fClearFlag) {
		TString rmCmd = Form("rm -f %s %s", fCalFile.Data(), fResFile.Data());
		gSystem->Exec(rmCmd.Data());
		if (IsVerbose()) OutputMessage("OpenCalFiles", Form("Removing existing files - %s %s", fCalFile.Data(), fResFile.Data()));
	}

	fEnvCalib = new TEnv(fCalFile.Data());
	if (IsVerbose()) OutputMessage("OpenCalFiles", Form("Writing calibration data to file %s", fCalFile.Data()));

	fEnvCalib->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	
	fEnvCalib->SetValue("Calib.Source", fSourceName.Data());
	fEnvCalib->SetValue("Calib.Energies", fEnergies.Data());

	fEnvResults = new TEnv(fResFile.Data());
	if (IsVerbose()) OutputMessage("OpenCalFiles", Form("Writing results to file %s", fResFile.Data()));

	fEnvResults->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	fEnvResults->SetValue("Calib.Energies", fEnergies.Data());

	fFitResults = new TFile(fFitFile.Data(), "RECREATE");
	if (!fFitResults->IsOpen()) {
		OutputMessage("OpenCalFiles", Form("Can't open file - %s", fFitFile.Data()), kTRUE);
		fFitResults = NULL;
	} else if (IsVerbose()) {
		OutputMessage("OpenCalFiles", Form("Writing fit data to file %s", fFitFile.Data()));
	}

	fEmin = fEnvResults->GetValue("Calib.Emin", 1e+20);
	fEmax = fEnvResults->GetValue("Calib.Emax", 0);
	fXmin = fEnvResults->GetValue("Calib.Xmin", 1000000);
	fXmax = fEnvResults->GetValue("Calib.Xmax", 0);

	return(kTRUE);
}

void SetFullRange(TH1F * Histo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SetFullRange
// Purpose:        Set to full range
// Arguments:      TH1F * Histo    -- histogram
// Results:        --
// Description:    Adjusts histo limits
//////////////////////////////////////////////////////////////////////////////

	if (Histo == NULL) Histo = fCurHisto;

	Int_t xmax = (Int_t) fCurHisto->GetXaxis()->GetXmax();
	for (Int_t i = 0; i < fNofRegions; i++) {
		if (fUpperLim[i] == 0) fUpperLim[i] = xmax;
		if (fMinX > fLowerLim[i]) fMinX = fLowerLim[i];
		if (fMaxX < fUpperLim[i]) fMaxX = fUpperLim[i];
	}
	if (fMaxX == 0 || fMaxX < fMinX) fMaxX = xmax;
	if (fMinX != 0 || fMaxX < xmax) fCurHisto->GetXaxis()->SetRange(fMinX, fMaxX);
}

void WriteMinMax() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WriteMinMax
// Purpose:        Set minimum and maximum
// Arguments:      --
// Results:        --
// Description:    Writes minimum/maximum values to .res file
//////////////////////////////////////////////////////////////////////////////

	fEnvResults->SetValue("Calib.Emin", fEmin);
	fEnvResults->SetValue("Calib.Emax", fEmax);
	fEnvResults->SetValue("Calib.Xmin", fXmin);
	fEnvResults->SetValue("Calib.Xmax", fXmax);

}

void WaitForButtonPressed(Bool_t StepFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WaitForButtonPressed
// Purpose:        Control buttons
// Arguments:      Bool_t StepFlag    -- wait if kTRUE (overwrites StepMode bit)
// Results:        --
// Description:    Waits for button flag to change
//////////////////////////////////////////////////////////////////////////////

	if (StepFlag || StepMode()) {
		fButtonNext = kFALSE;
		fButtonOk = kTRUE;
		fButtonStop = kFALSE;
		fButtonFlag = kFALSE;
		fNextHisto = kNextHisto;
		while (!fButtonFlag) {
			gSystem->Sleep(50);
			gSystem->ProcessEvents();
		}
	} else {
		fButtonOk = kTRUE;
	}
	fButtonFlag = kFALSE;
}

void UserButtonPressed(Int_t ButtonId) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           UserButtonPressed
// Purpose:        Notify user buttons
// Arguments:      Int_t ButtonId    -- button number
// Results:        --
// Description:    Accepts user buttons and sets flags
//////////////////////////////////////////////////////////////////////////////

	switch (ButtonId) {
		case kButtonPrev:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fButtonOk = kTRUE; fNextHisto = kPrevHisto; return;
		case kButtonSame:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fButtonOk = kTRUE; fNextHisto = kSameHisto; return;
		case kButtonNext:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fButtonOk = kTRUE; fNextHisto = kNextHisto; return;
		case kButtonDiscard:	fButtonFlag = kTRUE; fButtonOk = kFALSE; return;
		case kButtonStop:		fButtonFlag = kTRUE; fButtonStop = kTRUE; return;
	}
}

void Stop() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           Stop
// Purpose:        Close files and return from calibration
// Arguments:      --
// Results:        --
// Description:    Stops calibration loop
//////////////////////////////////////////////////////////////////////////////

	if (fHistoFile && fHistoFile->IsOpen()) fHistoFile->Close();
	CloseRootFile();
}

void Exit() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           Exit
// Purpose:        Close files and exit
// Arguments:      --
// Results:        --
// Description:    Exits from MacroBrowser
//////////////////////////////////////////////////////////////////////////////

	Stop();
	gSystem->Exit(0);
}

TCanvas * OpenCanvas() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           OpenCanvas
// Purpose:        Create canvas
// Arguments:      --
// Results:        TCanvas * Canvas   -- canvas
// Description:    Opens the canvas
//////////////////////////////////////////////////////////////////////////////

	gStyle->SetPadLeftMargin(0.025);
	gStyle->SetPadRightMargin(0.025);

	fMainCanvas = new TCanvas("EncalCanv", "Energy Calibration Tool", kCanvasWidth, kCanvasHeight);
	fMainCanvas->Divide(1,2);

	fMainCanvas->cd(2);
	gPad->SetBottomMargin(.3);

	fMainCanvas->cd();
	fMainCanvas->Update();
	return(fMainCanvas);
}

void ClearCanvas(Int_t PadNo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           ClearCanvas
// Purpose:        Clear canvas
// Arguments:      Int_t PadNo     -- pad number
// Results:        --
// Description:    Clears whole canvas or specified pads
//////////////////////////////////////////////////////////////////////////////

	switch (PadNo) {
		case 0:
		case 1:
			fMainCanvas->cd(1);
			gPad->Clear();
			if (PadNo == 1) break;
		case 2:
			fMainCanvas->cd(2);
			gPad->Clear();
			break;
	}
	fMainCanvas->cd(PadNo);
	fMainCanvas->Update();
}

Bool_t SetCalSource() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SetCalSource
// Purpose:        Set calibration source
// Arguments:      --
//                 --
// Results:        kTRUE/kFALSE
// Description:    Define calibration source
//////////////////////////////////////////////////////////////////////////////

	fNofPeaksNeeded = 0;
	fEnableCalib = kFALSE;

	switch (fCalSource) {
		case kCalSource3Alpha:
			fSourceName = "TripleAlpha";
			break;
		case kCalSourceCo60:
			fSourceName = "Co60";
			break;
		case kCalSourceEu152:
			fSourceName = "Eu152";
			break;
		default:
			OutputMessage("SetCalSource", "Wrong calibration source - performing peak fitting only", kTRUE);
			return(kFALSE);
	}

	gSystem->ExpandPathName(fEnergies);
	if (gSystem->AccessPathName(fEnergies.Data())) {
		OutputMessage("SetCalSource", Form("No such file - %s, can't read calibration energies", fEnergies.Data()), kTRUE);
		return(kFALSE);
	}

	fEnvEnergies = new TEnv(fEnergies.Data());

	TString calNames = fEnvEnergies->GetValue("Calib.SourceNames", "");
	if (calNames.IsNull()) {
		OutputMessage("SetCalSource", Form("File %s doesn't contain calibration energies for source \"%s\"", fEnergies.Data(), fSourceName.Data()), kTRUE);
		delete fEnvEnergies;
		fEnvEnergies = NULL;
		return(kFALSE);
	}	
		
	fNofPeaksNeeded = fEnvEnergies->GetValue(Form("Calib.%s.NofLines", fSourceName.Data()), 0);
	if (fNofPeaksNeeded == 0) {
		OutputMessage(	"SetCalSource",
								Form("Error reading \"Calib.%s.NofLines\" from file %s - can't calibrate using %s source",
								fSourceName.Data(), fEnergies.Data(), fSourceName.Data()), kTRUE);
		delete fEnvEnergies;
		fEnvEnergies = NULL;
		return(kFALSE);
	}
	
	fEnableCalib = WriteGaugeFile();

	return(kTRUE);
}

Bool_t WriteGaugeFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WriteGaugeFile
// Purpose:        Write cal energies to gauge file
// Arguments:      --
//                 --
// Results:        kTRUE/kFALSE
// Description:    Interface to Otto's CalibrationDialog object
//////////////////////////////////////////////////////////////////////////////

	ofstream gauge(fGaugeFile.Data());
	Int_t nofLines = 0;
	Double_t e, de, intens;
	TString source;
	for (Int_t i = 0; i < fNofPeaksNeeded; i++) {
		e = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.E", fSourceName.Data(), i), -1.0);
		if (e == -1) {
			OutputMessage(	"WriteGaugeFile",
								Form("Error reading \"Calib.%s.Line.%d.E\" from file %s - energy not given",
								fSourceName.Data(), i, fEnergies.Data()), kTRUE);
		} else {
			de = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Eerr", fSourceName.Data(), i), 1.0);
			intens = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Intensity", fSourceName.Data(), i), 1.0);
			source = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Source", fSourceName.Data(), i), fSourceName.Data());
		}
		gauge << source << " " << e << " " << de << " " << intens << endl;
		nofLines++;
	}
	OutputMessage("WriteGaugeFile", Form("%d calibration energies written to file %s", nofLines, fGaugeFile.Data()));
	gauge.close();
	return(kTRUE);
}

void CloseEnvFiles() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CloseEnvFiles
// Purpose:        Close calibration files
// Arguments:      --
// Results:        --
// Description:    Close TEnv files
//////////////////////////////////////////////////////////////////////////////

	TList * lofEntries = (TList *) fEnvResults->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	fEnvResults->SetValue("Calib.NofHistograms", nh);
	fEnvResults->SaveLevel(kEnvLocal);

	fEnvCalib->SetValue("Calib.NofHistograms", GetNofHistosCalibrated());
	fEnvCalib->SaveLevel(kEnvLocal);
}

void CloseRootFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           CloseRootFile
// Purpose:        Write histo results and close file
// Arguments:      --
// Results:        --
// Description:    Closes root file containing histo + fit data
//////////////////////////////////////////////////////////////////////////////

	if (fFitResults && fFitResults->IsOpen()) {
		fFitResults->Write();
		fFitResults->Close();
	}
}

Int_t GetNofHistosCalibrated() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           GetNofHistosCalibrated
// Purpose:        Count histos in .cal file
// Arguments:      --
// Results:        Int_t NofHistos   -- number of calibrated histos
// Description:    Counts histos in calibration file
//////////////////////////////////////////////////////////////////////////////

	TList * lofEntries = (TList *) fEnvCalib->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	fNofHistosCalibrated = nh;
	return(nh);
}

Bool_t FindPeaks() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           FindPeaks
// Purpose:        Start the peak finder
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Peak finding
//////////////////////////////////////////////////////////////////////////////

	if (fPeakFinder == NULL) fPeakFinder = new FindPeakDialog(fCurHisto, kFALSE);

	Double_t pmax = (Double_t) fPeakFrac / 100.;

	if (fRebin > 1) fCurHisto->Rebin(fRebin);

	Bool_t sts = kTRUE;
	Int_t npeaks = 0;
	for (Int_t i = 0; i < fNofRegions; i++) {
		fPeakFinder->SetFrom(fLowerLim[i]);
		fPeakFinder->SetTo(fUpperLim[i]);
		fPeakFinder->SetThreshold(pmax);
		fPeakFinder->SetSigma(fSigma);
		fPeakFinder->SetTwoPeakSeparation(fTwoPeakSep);
		fPeakFinder->ExecuteFindPeak();
		if (i == 0) fPeakList = (TList *) fCurHisto->GetListOfFunctions()->FindObject("spectrum_peaklist");
		fNofPeaks = fPeakList->GetEntries();
		if ((fNofPeaks - npeaks) >= 200) {
			OutputMessage("FindPeaks", Form("Too many peaks in region [%d, %d] - max 200", fLowerLim[i], fUpperLim[i]), kTRUE, kColorRed);
			sts = kFALSE;
			break;
		}
		npeaks = fNofPeaks;
	}

	fNofPeaks = fPeakList ? fPeakList->GetEntries() : 0;
	if (fNofPeaks == 0) {
		OutputMessage("FindPeaks", Form("No peaks found in histogram - %s", fCurHisto->GetName()), kTRUE, kColorRed);
		SetFitStatus(kFitDiscard, "No peaks found");
		sts = kFALSE;
	}

	SetFullRange(fCurHisto);

	fCurHisto->Draw();
	return(sts);
}

Bool_t FitPeaks() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           FitPeaks
// Purpose:        Fit peaks
// Arguments:      --
// Results:        kTRUE/kFALSE
// Description:    Peak fitting
//////////////////////////////////////////////////////////////////////////////

	if (fFitOneDim == NULL) fFitOneDim = new FitOneDimDialog(fCurHisto, 1, kFALSE);
	fFitOneDim->SetPeakSep(fTwoPeakSep);
	fFitOneDim->SetFitWindow(fFitRange);
	fFitOneDim->SetBackg0(kFALSE);
	fFitOneDim->SetSlope0(fFitBackground == kFitModeBackgroundConst);
	fFitOneDim->SetLowtail(fFitMode == kFitModeLeftTail);
	fFitOneDim->SetHightail(fFitMode == kFitModeRightTail);
	fFitOneDim->SetConfirmStartValues(kFALSE);
	fFitOneDim->SetShowcof(kTRUE);
	fFitOneDim->FitPeakList();
	return(kTRUE);
}

void Calibrate() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           Calibrate
// Purpose:        Perform energy calibration
// Arguments:      --
// Results:        --
// Description:    Starts energy calibration
//////////////////////////////////////////////////////////////////////////////

	if (!fEnableCalib) return;

	if (fNofPeaks >= fNofPeaksNeeded) { 
		fCurHisto->GetListOfFunctions()->Print();
		getchar();
		if (fCalibration == NULL) fCalibration = new CalibrationDialog(fCurHisto, kFALSE);
		fCalibration->SetVerbose(kFALSE);
		fCalibration->SetCustomGauge(kTRUE);
		fCalibration->SetCustomGaugeFile(fGaugeFile);
		fCalibration->SetOffMin(fMatchOffsetMin);
		fCalibration->SetOffStep(fMatchOffsetStep);
		fCalibration->SetOffMax(fMatchOffsetMax);
		fCalibration->SetGainMin(fMatchGainMin);
		fCalibration->SetGainStep(fMatchGainStep);
		fCalibration->SetGainMax(fMatchGainMax);
		fCalibration->ExecuteAutoSelect();
	} else {
		OutputMessage("Calibrate", Form("Too few peaks - %d (%s calibration needs at least %d peaks)", fNofPeaks, fSourceName.Data(), fNofPeaksNeeded), kTRUE);
		SetFitStatus(kFitDiscard, "Too few peaks");
	}
}

#if 0
void WriteResults() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WriteResults
// Purpose:        Write results to file
// Arguments:      --
// Results:        --
// Description:    Writes results to .res file
//////////////////////////////////////////////////////////////////////////////

	fEnvResults->SetValue(Form("Calib.%s.Xmin", fCurHisto->GetName()), fMinX);
	fEnvResults->SetValue(Form("Calib.%s.Xmax", fCurHisto->GetName()), fMaxX);

	fEnvResults->SetValue(Form("Calib.%s.NofPeaks", fCurHisto->GetName()), fNofPeaks);
	for (Int_t i = 0; i < fNofPeaks; i++) {
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.X", fCurHisto->GetName(), i), fPeakX[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Xfit", fCurHisto->GetName(), i), fXfit[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Xerr", fCurHisto->GetName(), i), fXerr[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Y", fCurHisto->GetName(), i), fPeakY[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Yfit", fCurHisto->GetName(), i), fYfit[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Yerr", fCurHisto->GetName(), i), fYerr[i]);
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Chi2", fCurHisto->GetName(), i), fChi2[i]);
	}
}

void WriteCalibration() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WriteCalibration
// Purpose:        Write calibration data to file
// Arguments:      --
// Results:        --
// Description:    Writes calibration to .cal file
//////////////////////////////////////////////////////////////////////////////

	if (fEnableCalib && (fNofPeaks >= fNofPeaksNeeded)) {
		fEnvCalib->SetValue(Form("Calib.%s.Xmin", fCurHisto->GetName()), fMinX);
		fEnvCalib->SetValue(Form("Calib.%s.Xmax", fCurHisto->GetName()), fMaxX);
		fEnvCalib->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fPoly->GetParameter(1));
		fEnvCalib->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fPoly->GetParameter(0));
		fEnvResults->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fPoly->GetParameter(1));
		fEnvResults->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fPoly->GetParameter(0));
		SetFitStatus(StepMode() ? kFitOk : kFitAuto);
	} else {
		SetFitStatus(kFitDiscard, "Too few peaks");
	}
}
#endif

void UpdateStatusLine() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           UpdateStatusLine
// Purpose:        Update status line
// Arguments:      --
// Results:        --
// Description:    Writes results status line
//////////////////////////////////////////////////////////////////////////////

	if (fNofPeaks >= fNofPeaksNeeded) {
		TString sts;
		if (fEnableCalib) {
			sts = Form("File %s, histo %s: E(x) = %5.2f + %5.2f * x", fHistoFile->GetName(), fCurHisto->GetName(), fPoly->GetParameter(0), fPoly->GetParameter(1));
		} else {
			sts = Form("File %s, histo %s: %d peak(s)", fHistoFile->GetName(), fCurHisto->GetName(), fNofPeaks);
		}
		OutputMessage("UpdateStatusLine", sts.Data(), kFALSE, 3);
	}
}

void SetFitStatus(Int_t FitStatus, const Char_t * Reason) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SetFitStatus
// Purpose:        Set fit status
// Arguments:      Int_t FitStatus   -- status
//                 Char_t * Reason   -- reason
// Results:        --
// Description:    stores fit status
//////////////////////////////////////////////////////////////////////////////

	fFitStatus = FitStatus;
	fReason = (Reason == NULL || *Reason == '\0') ? "" : Reason;
}

void ShowResults2dim() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           ShowResults2dim
// Purpose:        Fill 2-dim histogram
// Arguments:      --
// Results:        --
// Description:    Shows fit results in 2-dim histogram
//////////////////////////////////////////////////////////////////////////////

	if (!TwoDimMode()) return;

	TList * lofEntries = (TList *) fEnvCalib->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	TObjArray lofCalibs;
	while (r = (TEnvRec *) iter->Next()) {
		TString str = r->GetName();
		TObjArray * a = str.Tokenize(".");
		TString hn = ((TObjString *) a->At(1))->GetString();
		if (lofCalibs.FindObject(hn.Data()) == NULL) {
			TH1F * h = (TH1F *) fHistoFile->Get(hn.Data());
			if (h) {
				TString testIt = fEnvCalib->GetValue(Form("Calib.%s.Gain", hn.Data()), "");
				if (!testIt.IsNull()) {
					Double_t a1 = fEnvCalib->GetValue(Form("Calib.%s.Gain", hn.Data()), 1.);
					Double_t a0 = fEnvCalib->GetValue(Form("Calib.%s.Offset", hn.Data()), 0.);
					const Char_t * formula = Form("%f + %f * x[0]", a0, a1);
					TFormula * poly = new TFormula(hn.Data(), formula);
					lofCalibs.Add(poly);
					nh++;
				}
			}
		}
	}

	if (nh == 0) {
		OutputMessage("ShowResults2dim", Form("File %s - No histograms found", fHistoFile->GetName()), kTRUE);
	} else {
 		f2DimCanvas = new TCanvas();
		TString hTitle = Form("Calibration restults: file %s, %d histogram(s)", fHistoFile->GetName(), nh);
		f2DimFitResults = new TH2S("hCalResults", hTitle.Data(), (fXmax - fXmin), fEmin, fEmax, nh, 0, nh);
		if (fFitResults) fFitResults->Append(f2DimFitResults);
		TAxis * yAxis = f2DimFitResults->GetYaxis();

		for (Int_t n = 0; n < nh; n++) {
			TFormula * poly = (TFormula *) lofCalibs[n];
			TString hn = poly->GetName();
			TH1F * h = (TH1F *) fHistoFile->Get(hn.Data());
			if (h != NULL) {
				for (Int_t i = fXmin; i < fXmax; i++) {
					Int_t bCont = (Int_t) h->GetBinContent(i);
					Double_t bCent = h->GetBinCenter(i);
					for (Int_t j = 0; j < bCont; j++) {
						Double_t x = bCent + gRandom->Rndm() - 0.5;
						f2DimFitResults->Fill((Axis_t) poly->Eval(x), (Axis_t) n);
					}
				}
				yAxis->SetBinLabel(n + 1, hn.Data());
			}
		}
		f2DimFitResults->Draw();
		f2DimCanvas->Update();
	}
}

void UpdateArguments() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           UpdateArguments
// Purpose:        Read arguments from GUI
// Arguments:      --
// Results:        --
// Description:    Xfers arguments from GUI
//////////////////////////////////////////////////////////////////////////////

	fGuiPtr->GetArgValue("CalSource", fCalSource);
	fGuiPtr->GetArgValue("Energies", fEnergies);
	if (fEnergies.IsNull()) fEnergies = "$MARABOU/data/encal/calibEnergies.all";
	fGuiPtr->GetArgValue("CalFile", fCalFile);
	if (fCalFile.IsNull()) fCalFile = "Encal.cal";
	fGuiPtr->GetArgValue("ResFile", fResFile);
	if (fResFile.IsNull()) { fResFile = fCalFile; fResFile.ReplaceAll(".cal", 4, ".res", 4); }
	fGuiPtr->GetArgValue("FitFile", fFitFile);
	if (fFitFile.IsNull()) { fFitFile = fCalFile; fFitFile.ReplaceAll(".cal", 4, ".root", 5); }
	fGuiPtr->GetArgValue("ClearFlag", fClearFlag);

	fGuiPtr->GetArgValue("Rebin", fRebin);

	UInt_t flag;
	Int_t low, up;
	Double_t a0, a1;

	fNofRegions = 0;
	fMinX = 1000000;
	fMaxX = 0;
	fGuiPtr->GetArgCheck("Region1", flag);
	if (flag != 0) {
		fGuiPtr->GetArgValue("Region1", low, 0);
		fGuiPtr->GetArgValue("Region1", up, 1);
		fLowerLim[0] = low;
		fUpperLim[0] = up;
		fNofRegions = 1;
	}
	fGuiPtr->GetArgCheck("Region2", flag);
	if (flag != 0) {
		fGuiPtr->GetArgValue("Region2", low, 0);
		fGuiPtr->GetArgValue("Region2", up, 1);
		fLowerLim[1] = low;
		fUpperLim[1] = up;
		fNofRegions = 2;
	}
	fGuiPtr->GetArgCheck("Region3", flag);
	if (flag != 0) {
		fGuiPtr->GetArgValue("Region3", low, 0);
		fGuiPtr->GetArgValue("Region3", up, 1);
		fLowerLim[2] = low;
		fUpperLim[2] = up;
		fNofRegions = 3;
	}

	fGuiPtr->GetArgCheck("Efficiency", flag);
	if (flag != 0) {
		fGuiPtr->GetArgValue("Efficiency", a0, 0);
		fGuiPtr->GetArgValue("Efficiency", a1, 1);
		fEfficiencyA0 = a0;
		fEfficiencyA1 = a1;
	} else {
		fEfficiencyA0 = 0;
		fEfficiencyA1 = 0;
	}

	for (Int_t i = 0; i < fNofRegions; i++) {
		if (fMinX > fLowerLim[i]) fMinX = fLowerLim[i];
		if (fMaxX < fUpperLim[i]) fMaxX = fUpperLim[i];
	}

	fGuiPtr->GetArgValue("PeakFrac", fPeakFrac);
	fGuiPtr->GetArgValue("Sigma", fSigma);
	fGuiPtr->GetArgValue("TwoPeakSep", fTwoPeakSep);
	fGuiPtr->GetArgValue("FitMode", fFitMode);
	fGuiPtr->GetArgValue("FitGrouping", fFitGrouping);
	fGuiPtr->GetArgValue("FitBackground", fFitBackground);
	fGuiPtr->GetArgValue("FitRange", fFitRange);
	fGuiPtr->GetArgValue("MatchOffsetMin", fMatchOffsetMin);
	fGuiPtr->GetArgValue("MatchOffsetStep", fMatchOffsetStep);
	fGuiPtr->GetArgValue("MatchOffsetMax", fMatchOffsetMax);
	fGuiPtr->GetArgValue("MatchGainMin", fMatchGainMin);
	fGuiPtr->GetArgValue("MatchGainStep", fMatchGainStep);
	fGuiPtr->GetArgValue("MatchGainMax", fMatchGainMax);
	fGuiPtr->GetArgValue("DisplayMode", fDisplayMode);
	fGuiPtr->GetArgValue("VerboseMode", fVerboseMode);

}

void Encal(Int_t CalSource = 1,
            const Char_t * Energies = "$MARABOU/data/encal/energies.dat",
            Bool_t VerboseMode = kFALSE,
            TObjArray * LofHistos = NULL,
            const Char_t * CalFile = "Encal.cal",
            const Char_t * ResFile = "Encal.res",
            const Char_t * FitFile = "Encal.root",
            Bool_t ClearFlag = kFALSE,
            Int_t Rebin = 1,
            Int_t Region1Low = 0,
            Int_t Region1Up = 65535,
            Bool_t Region1On = kFALSE,
            Int_t Region2Low = 0,
            Int_t Region2Up = 65535,
            Bool_t Region2On = kFALSE,
            Int_t Region3Low = 0,
            Int_t Region3Up = 65535,
            Bool_t Region3On = kFALSE,
            Double_t EfficiencyA0 = 0,
            Double_t EfficiencyA1 = 0,
            Bool_t EfficiencyOn = kFALSE,
            Int_t PeakFrac = 1,
            Double_t Sigma = 3,
            Double_t TwoPeakSep = 1,
            Int_t FitMode = 0,
            Int_t FitGrouping = 1,
            Int_t FitBackground = 1,
            Double_t FitRange = 3,
            Double_t MatchOffsetMin = 0,
            Double_t MatchOffsetStep = 0,
            Double_t MatchOffsetMax = 0,
            Double_t MatchGainMin = 1,
            Double_t MatchGainStep = 1,
            Double_t MatchGainMax = 100,
            Int_t DisplayMode = 1,
            TGMrbMacroFrame * GuiPtr = NULL)
//_____________________________________________________________[MAIN FUNCTION]
//
{
	msg = new TMrbLogger("Encal.log");

	fLofHistos = LofHistos; 								// xfer arguments
	fCalSource = CalSource;
	fEnergies = (Energies == NULL || *Energies == '\0') ? "$MARABOU/data/encal/calibEnergies.all" : Energies;
	fCalFile = (CalFile == NULL || *CalFile == '\0') ? "Encal.cal" : CalFile;
	fResFile = (ResFile == NULL || *ResFile == '\0') ? "" : CalFile;
	if (fResFile.IsNull()) { fResFile = fCalFile; fResFile.ReplaceAll(".cal", 4, ".res", 4); }
	fFitFile = (FitFile == NULL || *FitFile == '\0') ? "" : FitFile;
	if (fFitFile.IsNull()) { fFitFile = fCalFile; fFitFile.ReplaceAll(".cal", 4, ".root", 5); }
	fClearFlag = ClearFlag;

	fRebin = Rebin;

	fNofRegions = 0;
	fMinX = 1000000;
	fMaxX = 0;
	if (Region1On) {
		fLowerLim[0] = Region1Low;
		fUpperLim[0] = Region1Up;
		fNofRegions = 1;
	}
	if (Region2On) {
		fLowerLim[1] = Region2Low;
		fUpperLim[1] = Region2Up;
		fNofRegions = 2;
	}
	if (Region3On) {
		fLowerLim[2] = Region3Low;
		fUpperLim[2] = Region3Up;
		fNofRegions = 3;
	}
		if (EfficiencyOn) {
		fEfficiencyA0 = EfficiencyA0;
		fEfficiencyA1 = EfficiencyA1;
	} else {
		fEfficiencyA0 = 0;
		fEfficiencyA1 = 0;
	}

	for (Int_t i = 0; i < fNofRegions; i++) {
		if (fMinX > fLowerLim[i]) fMinX = fLowerLim[i];
		if (fMaxX < fUpperLim[i]) fMaxX = fUpperLim[i];
	}

	fPeakFrac = PeakFrac;
	fSigma = Sigma;
	fTwoPeakSep = TwoPeakSep;
	fFitMode = FitMode;
	fFitGrouping = FitGrouping;
	fFitBackground = FitBackground;
	fFitRange = FitRange;
	fMatchOffsetMin = MatchOffsetMin;
	fMatchOffsetStep = MatchOffsetStep;
	fMatchOffsetMax = MatchOffsetMax;
	fMatchGainMin = MatchGainMin;
	fMatchGainStep = MatchGainStep;
	fMatchGainMax = MatchGainMax;
	fDisplayMode = DisplayMode;
	fVerboseMode = VerboseMode;
	fNofPeaksNeeded = 0;
	fEnableCalib = kFALSE;
	fEnvEnergies = NULL;
	fEnvCalib = NULL;
	fFitResults = NULL;

	fGuiPtr = GuiPtr;

	TString pNames = "Ta_Frac:Ta_Width:Bg_Const:Bg_Slope:Ga_Sigma:Ga_Const:Ga_Mean";
	fParamNames = pNames.Tokenize(":");

	fPeakFinder = NULL;
	fFitOneDim = NULL;
	fCalibration = NULL;

	if (!OpenHistoFile()) return;

	if (!SetCalSource()) return;

	OpenCalFiles();

	TCanvas * mainCanvas = OpenCanvas();

	ResetLofHistos();
	TH1F * h;
	fNextHisto = kNextHisto;
	while (h = GetNextHisto()) {

		mainCanvas->cd(1);
		if (h == NULL) {
			ClearCanvas(0);
			WaitForButtonPressed();
			if (fButtonStop) { Stop(); break; }
			continue;
		}

		SetCurHisto(h);
		SetFullRange(h);
		h->Draw("");

		if (FindPeaks()) {

			FitPeaks();

			Calibrate();

			ClearCanvas(2);

			mainCanvas->Update();
			gSystem->ProcessEvents();
		} else {
			mainCanvas->cd(1);
			h->Draw();
			ClearCanvas(2);
		}

		WaitForButtonPressed();
		if (fButtonStop) { Stop(); break; }
		if (!fButtonOk) continue;
	}

	WriteMinMax();

	ShowResults2dim();

	CloseEnvFiles();

	OutputMessage(NULL, Form("%s - %d histogram(s), press \"execute\" to restart or \"quit\" to exit", HistoFile()->GetName(), GetNofHistosCalibrated()), kFALSE, kColorBlue);

	WaitForButtonPressed(kTRUE);

	CloseRootFile();

	return;
}
