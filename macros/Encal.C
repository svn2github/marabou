//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Encal.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .L Encal.C
//                   Encal(TGMrbMacroFrame * GuiPtr)
// Arguments:        TGMrbMacroFrame * GuiPtr -- pointer to MacroBrowser GUI
// Description:      Energy calibration for 1-dim histograms
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: Encal.C,v 1.36 2007-10-12 10:14:20 Rudolf.Lutter Exp $
// Date:             Fri Oct 12 09:43:56 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               780
//                   Aclic:               +g
//                   Modify:              no
//                   GuiPtrMode:          GuiPtr
//                   UserStart:           on
//                   RcFile:              .EncalLoadLibs.C
//                   NofArgs:             39
//                   Arg1.Name:           Tab_1
//                   Arg1.Title:          Init & Files
//                   Arg1.Type:           Int_t
//                   Arg1.EntryType:      Tab
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           Section_STP
//                   Arg2.Title:          0. Setup
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Group
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           LoadSetup
//                   Arg3.Title:          Load setup from file
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      File
//                   Arg3.Default:        .Encal.env
//                   Arg3.Values:         Encal environment files:.Encal*.env
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           Section_IC
//                   Arg4.Title:          1. Initialization
//                   Arg4.Type:           Int_t
//                   Arg4.EntryType:      Group
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           CalSource
//                   Arg5.Title:          Calibration source
//                   Arg5.Type:           Int_t
//                   Arg5.EntryType:      Radio
//                   Arg5.Default:        1
//                   Arg5.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source=2:3Alpha|calibrate using alpha source (Pu239/Am241/Cm244)=4:Other|user-defined calibraion=8
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           Energies
//                   Arg6.Title:          Calibration energies
//                   Arg6.Type:           Char_t *
//                   Arg6.EntryType:      File
//                   Arg6.Default:        $MARABOU/data/encal/calibEnergies.all
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           VerboseMode
//                   Arg7.Title:          Verbose mode
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.Default:        no
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           Section_FH
//                   Arg8.Title:          2. Files & histograms
//                   Arg8.Type:           Int_t
//                   Arg8.EntryType:      Group
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           HistoFile
//                   Arg9.Title:          Histogram file (.root)
//                   Arg9.Type:           TObjArray *
//                   Arg9.EntryType:      FObjListBox
//                   Arg9.Width:          300
//                   Arg9.Default:        none.root
//                   Arg9.Values:         ROOT files:*.root
//                   Arg9.AddLofValues:   No
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          HistoMask
//                   Arg10.Title:         Histogram selection mask(s)
//                   Arg10.Type:          Char_t *
//                   Arg10.EntryType:     Entry
//                   Arg10.Width:         350
//                   Arg10.Default:       *
//                   Arg10.AddLofValues:  No
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          CalFile
//                   Arg11.Title:         Calibration data (.cal)
//                   Arg11.Type:          Char_t *
//                   Arg11.EntryType:     File
//                   Arg11.Width:         150
//                   Arg11.Default:       Encal.cal
//                   Arg11.Values:        Calib files:*.cal
//                   Arg11.AddLofValues:  No
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          ResFile
//                   Arg12.Title:         Calibration results (.res)
//                   Arg12.Type:          Char_t *
//                   Arg12.EntryType:     File
//                   Arg12.Width:         150
//                   Arg12.Default:       Encal.res
//                   Arg12.Values:        Result files:*.res
//                   Arg12.AddLofValues:  No
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          FitFile
//                   Arg13.Title:         Fit results (.root)
//                   Arg13.Type:          Char_t *
//                   Arg13.EntryType:     File
//                   Arg13.Width:         150
//                   Arg13.Default:       Encal.root
//                   Arg13.Values:        Fit data files:*.root
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          ClearFlag
//                   Arg14.Title:         Clear output files (.cal, .res)
//                   Arg14.Type:          Bool_t
//                   Arg14.EntryType:     YesNo
//                   Arg14.Default:       no
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          Tab_2
//                   Arg15.Title:         Peaks & Calibration
//                   Arg15.Type:          Int_t
//                   Arg15.EntryType:     Tab
//                   Arg15.AddLofValues:  No
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//                   Arg16.Name:          Section_PFND
//                   Arg16.Title:         3. Peak Finder
//                   Arg16.Type:          Int_t
//                   Arg16.EntryType:     Group
//                   Arg16.AddLofValues:  No
//                   Arg16.Base:          dec
//                   Arg16.Orientation:   horizontal
//                   Arg17.Name:          Rebin
//                   Arg17.Title:         Rebin histograms
//                   Arg17.Type:          Int_t
//                   Arg17.EntryType:     Entry-C
//                   Arg17.Width:         100
//                   Arg17.Default:       1:F
//                   Arg17.AddLofValues:  No
//                   Arg17.Base:          dec
//                   Arg17.Orientation:   horizontal
//                   Arg18.Name:          Region1
//                   Arg18.Title:         Regions (1): lower, upper, fraction
//                   Arg18.Type:          Int_t
//                   Arg18.EntryType:     Entry-MC
//                   Arg18.NofEntryFields:3
//                   Arg18.Width:         100
//                   Arg18.Default:       0:0:10:F
//                   Arg18.AddLofValues:  No
//                   Arg18.LowerLimit:    0:65000:1
//                   Arg18.UpperLimit:    0:65000:100
//                   Arg18.Increment:     100:100:10
//                   Arg18.Base:          dec
//                   Arg18.Orientation:   horizontal
//                   Arg19.Name:          Region2
//                   Arg19.Title:         Regions (2)
//                   Arg19.Type:          Int_t
//                   Arg19.EntryType:     Entry-MC
//                   Arg19.NofEntryFields:3
//                   Arg19.Width:         100
//                   Arg19.Default:       0:0:10
//                   Arg19.AddLofValues:  No
//                   Arg19.LowerLimit:    0:65000:1
//                   Arg19.UpperLimit:    0:65000:100
//                   Arg19.Increment:     100:100:10
//                   Arg19.Base:          dec
//                   Arg19.Orientation:   horizontal
//                   Arg20.Name:          Region3
//                   Arg20.Title:         Regions (3)
//                   Arg20.Type:          Int_t
//                   Arg20.EntryType:     Entry-MC
//                   Arg20.NofEntryFields:3
//                   Arg20.Width:         100
//                   Arg20.Default:       0:0:10
//                   Arg20.AddLofValues:  No
//                   Arg20.LowerLimit:    0:65000:1
//                   Arg20.UpperLimit:    0:65000:100
//                   Arg20.Increment:     100:100:10
//                   Arg20.Base:          dec
//                   Arg20.Orientation:   horizontal
//                   Arg21.Name:          Efficiency
//                   Arg21.Title:         Normalize with efficiency: yeff(x) = y(x) / (a0 * exp(a1 * x))
//                   Arg21.Type:          Double_t
//                   Arg21.EntryType:     Entry-MC
//                   Arg21.NofEntryFields:2
//                   Arg21.Width:         100
//                   Arg21.Default:       0:0:F
//                   Arg21.AddLofValues:  No
//                   Arg21.LowerLimit:    0:0
//                   Arg21.UpperLimit:    1000:0.1
//                   Arg21.Increment:     10:0.05
//                   Arg21.Base:          dec
//                   Arg21.Orientation:   horizontal
//                   Arg22.Name:          Sigma
//                   Arg22.Title:         Sigma
//                   Arg22.Type:          Double_t
//                   Arg22.EntryType:     Entry
//                   Arg22.Width:         100
//                   Arg22.Default:       3
//                   Arg22.AddLofValues:  No
//                   Arg22.LowerLimit:    0
//                   Arg22.UpperLimit:    10
//                   Arg22.Increment:     .2
//                   Arg22.Base:          dec
//                   Arg22.Orientation:   horizontal
//                   Arg23.Name:          TwoPeakSep
//                   Arg23.Title:         Two-peaks separation [sigma]
//                   Arg23.Type:          Double_t
//                   Arg23.EntryType:     Entry
//                   Arg23.Width:         100
//                   Arg23.Default:       1
//                   Arg23.AddLofValues:  No
//                   Arg23.LowerLimit:    1
//                   Arg23.UpperLimit:    10
//                   Arg23.Base:          dec
//                   Arg23.Orientation:   horizontal
//                   Arg24.Name:          Section_PFIT
//                   Arg24.Title:         4. Peak Fit
//                   Arg24.Type:          Int_t
//                   Arg24.EntryType:     Group
//                   Arg24.AddLofValues:  No
//                   Arg24.Base:          dec
//                   Arg24.Orientation:   horizontal
//                   Arg25.Name:          FitMode
//                   Arg25.Title:         Fit mode
//                   Arg25.Type:          Int_t
//                   Arg25.EntryType:     Radio
//                   Arg25.Default:       1
//                   Arg25.Values:        gauss|fit symmetric gaussian=1:left tail|fit gaussian + exp tail on left side=2:right tail|fit gaussian + exp tail on right side=4
//                   Arg25.AddLofValues:  No
//                   Arg25.Base:          dec
//                   Arg25.Orientation:   horizontal
//                   Arg26.Name:          FitGrouping
//                   Arg26.Title:         Fit grouping
//                   Arg26.Type:          Int_t
//                   Arg26.EntryType:     Radio
//                   Arg26.Default:       1
//                   Arg26.Values:        single peak|fit each peak separately=1:ensemble|group peaks before fitting=2
//                   Arg26.AddLofValues:  No
//                   Arg26.Base:          dec
//                   Arg26.Orientation:   horizontal
//                   Arg27.Name:          FitBackground
//                   Arg27.Title:         Background
//                   Arg27.Type:          Int_t
//                   Arg27.EntryType:     Radio
//                   Arg27.Default:       1
//                   Arg27.Values:        linear|fit linear Background=1:const|fit constant background=2
//                   Arg27.AddLofValues:  No
//                   Arg27.Base:          dec
//                   Arg27.Orientation:   horizontal
//                   Arg28.Name:          FitRange
//                   Arg28.Title:         Range [sigma]
//                   Arg28.Type:          Double_t
//                   Arg28.EntryType:     Entry
//                   Arg28.Width:         100
//                   Arg28.Default:       3
//                   Arg28.AddLofValues:  No
//                   Arg28.LowerLimit:    0
//                   Arg28.UpperLimit:    10
//                   Arg28.Increment:     .5
//                   Arg28.Base:          dec
//                   Arg28.Orientation:   horizontal
//                   Arg29.Name:          Section_PM
//                   Arg29.Title:         5. Peak Matching
//                   Arg29.Type:          Int_t
//                   Arg29.EntryType:     Group
//                   Arg29.AddLofValues:  No
//                   Arg29.Base:          dec
//                   Arg29.Orientation:   horizontal
//                   Arg30.Name:          PeakMatch
//                   Arg30.Title:         Peak matching
//                   Arg30.Type:          Bool_t
//                   Arg30.EntryType:     YesNo
//                   Arg30.Default:       no
//                   Arg30.AddLofValues:  No
//                   Arg30.Base:          dec
//                   Arg30.Orientation:   horizontal
//                   Arg31.Name:          TestbedHisto
//                   Arg31.Title:         Testbed histogram: nbins, xmin, xmax
//                   Arg31.Type:          Int_t
//                   Arg31.EntryType:     Entry-M
//                   Arg31.NofEntryFields:3
//                   Arg31.Width:         100
//                   Arg31.Default:       0:0:0
//                   Arg31.AddLofValues:  No
//                   Arg31.LowerLimit:    1:0:10000
//                   Arg31.UpperLimit:    10000:10000:10000
//                   Arg31.Increment:     100:100:100
//                   Arg31.Base:          dec
//                   Arg31.Orientation:   horizontal
//                   Arg32.Name:          MatchOffset
//                   Arg32.Title:         Offset: min, step, max
//                   Arg32.Type:          Double_t
//                   Arg32.EntryType:     Entry-M
//                   Arg32.NofEntryFields:3
//                   Arg32.Width:         100
//                   Arg32.Default:       0:0:0
//                   Arg32.AddLofValues:  No
//                   Arg32.LowerLimit:    0:1:10000
//                   Arg32.UpperLimit:    10000:100:10000
//                   Arg32.Increment:     1:.1:1
//                   Arg32.Base:          dec
//                   Arg32.Orientation:   horizontal
//                   Arg33.Name:          MatchGain
//                   Arg33.Title:         Gain: min, step, max
//                   Arg33.Type:          Double_t
//                   Arg33.EntryType:     Entry-M
//                   Arg33.NofEntryFields:3
//                   Arg33.Width:         100
//                   Arg33.Default:       1:1:100
//                   Arg33.AddLofValues:  No
//                   Arg33.LowerLimit:    1:.1:100
//                   Arg33.UpperLimit:    100:1:100
//                   Arg33.Increment:     1:.1:1
//                   Arg33.Base:          dec
//                   Arg33.Orientation:   horizontal
//                   Arg34.Name:          MatchAccept
//                   Arg34.Title:         Acceptance window [chns]
//                   Arg34.Type:          Int_t
//                   Arg34.EntryType:     UpDown
//                   Arg34.NofEntryFields:1
//                   Arg34.Default:       2
//                   Arg34.AddLofValues:  No
//                   Arg34.Base:          dec
//                   Arg34.Orientation:   horizontal
//                   Arg35.Name:          NewFrame_1
//                   Arg35.Title:         Control
//                   Arg35.Type:          Int_t
//                   Arg35.EntryType:     Frame
//                   Arg35.AddLofValues:  No
//                   Arg35.Base:          dec
//                   Arg35.Orientation:   horizontal
//                   Arg36.Name:          Section_CTRL
//                   Arg36.Title:         6. Control
//                   Arg36.Type:          Int_t
//                   Arg36.EntryType:     Group
//                   Arg36.AddLofValues:  No
//                   Arg36.Base:          dec
//                   Arg36.Orientation:   horizontal
//                   Arg37.Name:          DisplayMode
//                   Arg37.Title:         Display results
//                   Arg37.Type:          Int_t
//                   Arg37.EntryType:     Check
//                   Arg37.Default:       1
//                   Arg37.Values:        step|show each fit=1:2dim|show 2-dim histo after calibration=2:thumbs|show thumbnail plot=4
//                   Arg37.AddLofValues:  No
//                   Arg37.Base:          dec
//                   Arg37.Orientation:   horizontal
//                   Arg38.Name:          SaveSetup
//                   Arg38.Title:         Save setup to file
//                   Arg38.Type:          Int_t
//                   Arg38.EntryType:     File
//                   Arg38.Default:       .Encal.env
//                   Arg38.Values:        Encal environment files:.Encal*.env
//                   Arg38.AddLofValues:  No
//                   Arg38.Base:          dec
//                   Arg38.Orientation:   horizontal
//                   Arg39.Name:          CtrlButtons
//                   Arg39.Title:         What to be done next?
//                   Arg39.Type:          Int_t
//                   Arg39.EntryType:     TextButton
//                   Arg39.Values:        Start=0:Prev=1:Same=2:Next=3:Stop=4:Quit=5
//                   Arg39.AddLofValues:  No
//                   Arg39.Base:          dec
//                   Arg39.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////

//__________________________________________________________[C++ HEADER FILES]
//////////////////////////////////////////////////////////////////////////////
//+IncludeFiles
#include <iostream>
#include <iomanip>
#include <fstream>
#include "TMath.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TRegexp.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TLatex.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TGMsgBox.h"
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
//-IncludeFiles

//______________________________________________________[C++ ENUM DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:		   E<MacroName>ArgNums
// Format:  	   kArg<ArgName> = <ArgNumber>
// Description:    Enums to address macro arguments by their numbers
//////////////////////////////////////////////////////////////////////////////
//+ArgNums

enum EEncalArgNums {
 		kArgTab_1 = 1,
 		kArgSection_STP = 2,
 		kArgLoadSetup = 3,
 		kArgSection_IC = 4,
 		kArgCalSource = 5,
 		kArgEnergies = 6,
 		kArgVerboseMode = 7,
 		kArgSection_FH = 8,
 		kArgHistoFile = 9,
 		kArgHistoMask = 10,
 		kArgCalFile = 11,
 		kArgResFile = 12,
 		kArgFitFile = 13,
 		kArgClearFlag = 14,
 		kArgTab_2 = 15,
 		kArgSection_PFND = 16,
 		kArgRebin = 17,
 		kArgRegion1 = 18,
 		kArgRegion2 = 19,
 		kArgRegion3 = 20,
 		kArgEfficiency = 21,
 		kArgSigma = 22,
 		kArgTwoPeakSep = 23,
 		kArgSection_PFIT = 24,
 		kArgFitMode = 25,
 		kArgFitGrouping = 26,
 		kArgFitBackground = 27,
 		kArgFitRange = 28,
 		kArgSection_PM = 29,
 		kArgPeakMatch = 30,
 		kArgTestbedHisto = 31,
 		kArgMatchOffset = 32,
 		kArgMatchGain = 33,
 		kArgMatchAccept = 34,
 		kArgNewFrame_1 = 35,
 		kArgSection_CTRL = 36,
 		kArgDisplayMode = 37,
 		kArgSaveSetup = 38,
 		kArgCtrlButtons = 39,
 	};
//-ArgNums

//______________________________________________________[C++ ENUM DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:		   E<MacroName>Enums
// Format:  	   k<ArgName><ArgState> = <StateValue>
// Description:    Enums defining possible states of radio/check buttons
//////////////////////////////////////////////////////////////////////////////
//+Enums

enum EEncalEnums {
 		kCalSourceCo60 = 1,
 		kCalSourceEu152 = 2,
 		kCalSource3Alpha = 4,
 		kCalSourceOther = 8,
 		kFitModeGauss = 1,
 		kFitModeLeftTail = 2,
 		kFitModeRightTail = 4,
 		kFitGroupingSinglePeak = 1,
 		kFitGroupingEnsemble = 2,
 		kFitBackgroundLinear = 1,
 		kFitBackgroundConst = 2,
 		kDisplayModeStep = 1,
 		kDisplayMode2dim = 2,
 		kDisplayModeThumbs = 4,
 	};
//-Enums

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

enum EEncalFitStatus	{	kFitUndef = 0,
							kFitDiscard,
							kFitOk,
							kFitAuto
						};

enum EEncalButtons		{	kButtonStart,
							kButtonPrev,
							kButtonSame,
							kButtonNext,
							kButtonStop,
							kButtonQuit
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

TGMrbMacroFrame * fGuiPtr = NULL;

Int_t fitNofPeaks = 1;
Int_t fitBinWidth = 1;
Int_t fitTailSide = 1;

Bool_t fButtonFlag = kFALSE;
Bool_t fButtonNext = kTRUE;
Bool_t fButtonStop = kFALSE;
Bool_t fButtonQuit = kFALSE;
Bool_t fStarted = kFALSE;

EEncalHisto fNextHisto = kNextHisto;

TObjArray fLofHistos;
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
Bool_t fNormHistoEff;
Double_t fEfficiencyA0;
Double_t fEfficiencyA1;
Int_t fPeakFrac[4];
Double_t fSigma;
Double_t fTwoPeakSep;
Int_t fFitMode;
Int_t fFitGrouping;
Int_t fFitBackground;
Double_t fFitRange;
Int_t fMatchNbins;				// peak match
Int_t fMatchEmin;
Int_t fMatchEmax;
Bool_t fMatchFlag;
Double_t fMatchOffsetMin;
Double_t fMatchOffsetStep;
Double_t fMatchOffsetMax;
Double_t fMatchGainMin;
Double_t fMatchGainStep;
Double_t fMatchGainMax;
Int_t fMatchAccept;
Int_t fDisplayMode;
Bool_t fVerboseMode;

TString fSourceName; 	// calibration source
TEnv * fEnvEnergies;	// calibration energies
TEnv * fEnvCalib;		// calibration data
TEnv * fEnvResults;		// detailed calibration results
TFile * fFitResults;	// histograms + fits

TCanvas * fMainCanvas;	// canvas to display histo + fit
TCanvas * f2DimCanvas;	// 2nd canvas: 2-dim histo
TCanvas * fThumbCanvas;	// thumbnails
TCanvas * fPopupCanvas;

TFile * fHistoFile; 	// root file containing histograms
TString fHistoFileName; // name of root file
TString fHistoMask; 	// wildcard mask
TH1F * fCurHisto;		// current histogram
Int_t fCurHistoNo; 		// position of current histo in list
Int_t fNofHistos;		// number of histograms (selected from root file)
Int_t fNofHistosCalibrated;	// number of histograms calibrated

TString fEnvFile;		// where to save arguments

TMrbLofNamedX fLofFitStatusFlags;

TH2S * f2DimFitResults;	// 2-dim histo to show re-calibrated histograms

TH1F * fThumbHisto; 	// thumbnail histo, blown up

Int_t fNofPeaks;		// peak finder results
TList * fPeakList;
Int_t fNofCalibrationLines; // peaks present in given calibration
Int_t fNofPeaksNeeded; 		// peaks needed for given calibration
Int_t fNofPeaksUsed; 		// peaks used for calibration
Bool_t fEnableCalib;		// kTRUE if we have to calibrate
TF1 * fCalibFct;			// calibration function

TList * fFitList;		// gauss fit results
Int_t fFitStatus;		// fit quality
TString fReason;

TObjArray * fParamNames;	// param names for gauss/tail/background fit

FindPeakDialog * fPeakFinder = NULL;
FitOneDimDialog * fFitOneDim = NULL;
CalibrationDialog * fCalibration = NULL;

TString fGaugeFile = "gauge.dat";	// file name to be used with CalibrationDialog()

//____________________________________________________________[C++ PROTOTYPES]
//////////////////////////////////////////////////////////////////////////////
// Name:           Prototype definitions
//////////////////////////////////////////////////////////////////////////////

void Encal(TGMrbMacroFrame * GuiPtr);
void ProcessSignal(TGMrbMacroFrame * GuiPtr, Int_t ArgNo, const Char_t * ArgName, Int_t Signal);
void GetArguments(TGMrbMacroFrame * GuiPtr);
void SetArguments(TGMrbMacroFrame * GuiPtr, const Char_t * EnvFile = ".Encal.env");
Bool_t OpenHistoFile();
Bool_t OpenCalFiles();
Bool_t SetCalSource();
TH1F * GetNextHisto();
Bool_t WithinHistoMask(const Char_t * HistoName);
Bool_t FindPeaks();
void MarkPeak(Double_t X, Double_t E, const Char_t * GaugeName);
Bool_t Calibrate();
Int_t GetNofHistosCalibrated();
void WaitForSignal(Bool_t StepFlag = kFALSE);
void ClearCanvas(Int_t PadNo);
void OutputMessage(const Char_t * Method, const Char_t * Text, const Char_t * MsgType = "o", Int_t ColorIndex = -1);
TCanvas * OpenCanvas();
void CloseCanvas() { if (fMainCanvas) fMainCanvas->Close(); if (f2DimCanvas) f2DimCanvas->Close(); }
void WriteCalibration();
void ShowResults2dim();
void ShowThumbNails();
void UpdateStatusLine();
void SetFitStatus(Int_t FitStatus, const Char_t * Reason = NULL);
void WriteResults();
Bool_t WriteGaugeFile();
void CloseEnvFiles();
void CloseRootFile();
void SetFullRange(TH1F * Histo);
void SaveEnvFile();
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

TObjArray * GetLofHistos() { return(&fLofHistos); };
void ResetLofHistos() { fCurHistoNo = -1; };
void SetNofRegions(Int_t NofRegions) { fNofRegions = NofRegions; };
Int_t GetNofRegions() { return(fNofRegions); };
Int_t GetNofPeaks() { return(fNofPeaks); };
Int_t GetNofCalibrationLines() { return(fNofCalibrationLines); };
Int_t GetNofPeaksNeeded() { return(fNofPeaksNeeded); };
Int_t GetFitMode() { return(fFitMode); };
Int_t GetFitGrouping() { return(fFitGrouping); };
Bool_t StepMode() { return((fDisplayMode & kDisplayModeStep) != 0); };
Bool_t TwoDimMode() { return((fDisplayMode & kDisplayMode2dim) != 0); };
Bool_t ThumbNailMode() { return((fDisplayMode & kDisplayModeThumbs) != 0); };
void SetVerboseMode(Bool_t Flag) { fVerboseMode = Flag; };
Int_t GetLowerLim(Int_t Region = 0) { return(fLowerLim[Region]); };
Int_t GetUpperLim(Int_t Region = 0) { return(fUpperLim[Region]); };
TFile * HistoFile() { return(fHistoFile); };
void SetCurHisto(TH1F * Histo) { fCurHisto = Histo; };

void OutputMessage(const Char_t * Method, const Char_t * Text, const Char_t * MsgType, Int_t ColorIndex) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           OutputMessage
// Purpose:        Write message to cout/cerr
// Arguments:      Char_t * Method   -- method
//                 Char_t * Text     -- message text
//                 Char_t * MsgType  -- e(rror), w(arning), or o(ut)
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

	if (msg == NULL) msg = new TMrbLogger("Encal.log");

	if (*MsgType == 'e') {
		msg->Err() << Text << endl;
		msg->Flush(cName.Data(), method.Data());
		if (StepMode()) new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(), Form("%s: Error", method.Data()), Text, kMBIconStop);
	} else if (*MsgType == 'w') {
		msg->Wrn() << Text << endl;
		msg->Flush(cName.Data(), method.Data());
	} else if (*MsgType == 'o') {
		msg->Out() << Text << endl;
		msg->Flush(cName.Data(), method.Data(), setblue);
	} else {
		msg->Out() << Text << endl;
		msg->Flush(cName.Data(), method.Data(), tCol);
	}
}

Bool_t WithinHistoMask(const Char_t * HistoName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WithinHistoMask
// Purpose:        Apply mask to histogram
// Arguments:      Char_t * HistoName     -- histogram
// Results:        kTRUE/kFALSE
// Description:    Check if histo is within given mask(s)
//////////////////////////////////////////////////////////////////////////////

	if (fHistoMask.CompareTo("*") == 0) return(kTRUE);

	if (!fHistoMask.Contains(":")) fHistoMask += ":";

	TString hName = HistoName;
	TString hMask;
	Int_t from = 0;
	while (fHistoMask.Tokenize(hMask, from, ":")) {
		if (hMask.BeginsWith("!")) {
			hMask = hMask(1, 1000);
			TRegexp w(hMask.Data(), kTRUE);
			if (hName.Index(w, 0) == -1) return(kTRUE);
		} else {
			TRegexp w(hMask.Data(), kTRUE);
			if (hName.Index(w, 0) != -1) return(kTRUE);
		}
	}
	return(kFALSE);
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

	if (fHistoFileName.IsNull()) {
		OutputMessage("OpenHistoFile", "No histogram file given", "e");
		return(kFALSE);
	}

	fHistoFile = new TFile(fHistoFileName.Data());
	if (!fHistoFile->IsOpen()) {
		OutputMessage("OpenHistoFile", Form("Can't open histogram file - %s", fHistoFileName.Data()), "e");
		return(kFALSE);
	}

	fNofHistos = fLofHistos.GetEntriesFast();
	fCurHistoNo = -1;

	if (fNofHistos == 0) {
		OutputMessage("OpenHistoFile", Form("File %s - no histogram(s) selected", fHistoFileName.Data()), "e");
		return(kFALSE);
	}

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

	if (fCurHistoNo == -1) {
		fCurHistoNo = 0;
	} else if (fNextHisto == kPrevHisto) {
		fCurHistoNo--;
		if (fCurHistoNo < 0) {
			OutputMessage("GetNextHisto", Form("Reached beginning of histo list - %s", fHistoFile->GetName()), "w");
			fCurHistoNo = 0;
		}
	} else if (fNextHisto == kNextHisto) {
		fCurHistoNo++;
	}
	fPeakFinder = NULL;
	fFitOneDim = NULL;
	fCalibration = NULL;
	if (fCurHistoNo >= 0 && fCurHistoNo < fNofHistos) {
		TString hName = ((TObjString *) fLofHistos[fCurHistoNo])->GetString();
		if (fFitResults) {
			TString hNameAll = hName;
			hNameAll += "*";
			fFitResults->Delete(hNameAll.Data());
		}
		TH1F * h = (TH1F *) fHistoFile->Get(hName.Data());
		if (h) {
			h->GetListOfFunctions()->Delete();
		} else {
			OutputMessage("GetNextHisto", Form("No such histogram - %s:%s", fHistoFile->GetName(), hName.Data()), "e");
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
		TString rmCmd = Form("rm -f %s %s %s", fCalFile.Data(), fResFile.Data(), fFitFile.Data());
		gSystem->Exec(rmCmd.Data());
		if (fVerboseMode) OutputMessage("OpenCalFiles", Form("Removing existing files - %s %s", fCalFile.Data(), fResFile.Data()));
	}

	fEnvCalib = new TEnv(fCalFile.Data());
	if (fVerboseMode) OutputMessage("OpenCalFiles", Form("Writing calibration data to file %s", fCalFile.Data()));

	fEnvCalib->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	fEnvCalib->SetValue("Calib.Source", fSourceName.Data());
	fEnvCalib->SetValue("Calib.Energies", fEnergies.Data());

	fEnvResults = new TEnv(fResFile.Data());
	if (fVerboseMode) OutputMessage("OpenCalFiles", Form("Writing results to file %s", fResFile.Data()));

	fEnvResults->SetValue("Calib.ROOTFile", fHistoFile->GetName());
	fEnvResults->SetValue("Calib.Source", fSourceName.Data());
	fEnvResults->SetValue("Calib.Energies", fEnergies.Data());

	fFitResults = new TFile(fFitFile.Data(), "RECREATE");
	if (!fFitResults->IsOpen()) {
		OutputMessage("OpenCalFiles", Form("Can't open file - %s", fFitFile.Data()), "e");
		fFitResults = NULL;
	} else if (fVerboseMode) {
		OutputMessage("OpenCalFiles", Form("Writing fit data to file %s", fFitFile.Data()));
	}

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

void WaitForSignal(Bool_t StepFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           WaitForSignal
// Purpose:        Control buttons
// Arguments:      Bool_t StepFlag    -- wait if kTRUE (overwrites StepMode bit)
// Results:        --
// Description:    Waits for button flag to change
//////////////////////////////////////////////////////////////////////////////

	if (StepFlag || StepMode()) {
		fButtonNext = kFALSE;
		fButtonStop = kFALSE;
		fButtonQuit = kFALSE;
		fButtonFlag = kFALSE;
		fNextHisto = kNextHisto;
		while (!fButtonFlag) {
			gSystem->Sleep(50);
			gSystem->ProcessEvents();
		}
	}
	fButtonFlag = kFALSE;
}

void ProcessSignal(TGMrbMacroFrame * GuiPtr, Int_t ArgNo, const Char_t * ArgName, Int_t Signal) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           ProcessSignal
// Purpose:        Notify user buttons
// Arguments:      TGMrbMacroFrame * GuiPtr   -- pointer to gui
//                 Int_t ArgNo                -- argument number
//                 Int_t ArgName              -- argument name
//                 Int_t Signal               -- signal / button id
// Results:        --
// Description:    Accepts user buttons and sets flags
//////////////////////////////////////////////////////////////////////////////

	TString argName = ArgName;
	if (ArgNo == kArgCtrlButtons) {
		switch (Signal) {
			case kButtonStart:		fStarted = kTRUE; GuiPtr->ExecMacro(); break;
			case kButtonPrev:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fNextHisto = kPrevHisto; break;
			case kButtonSame:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fNextHisto = kSameHisto; break;
			case kButtonNext:		fButtonFlag = kTRUE; fButtonNext = kTRUE; fNextHisto = kNextHisto; break;
			case kButtonStop:		fButtonFlag = kTRUE; fButtonStop = kTRUE; break;
			case kButtonQuit:		if (!fStarted) gSystem->Exit(0); fButtonFlag = kTRUE; fButtonQuit = kTRUE; break;
		}
	} else if (ArgNo == kArgLoadSetup) {
		TString setupPath;
		GuiPtr->GetArgValue("LoadSetup", setupPath);
		TString setupFile = gSystem->BaseName(setupPath.Data());
		if (!setupFile.BeginsWith(".Encal") || !setupFile.EndsWith(".env")) {
			OutputMessage("ProcessSignal", Form("Not an Encal environment file - %s", setupFile.Data()), "e");
		} else {
			SetArguments(GuiPtr, setupFile.Data());
		}
	} else if (ArgNo == kArgSaveSetup) {
		SaveEnvFile();
	}
}

void SaveEnvFile() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SaveEnvFile
// Purpose:        Save arguments to given .env file
// Arguments:      --
// Results:        --
// Description:    Stops calibration loop
//////////////////////////////////////////////////////////////////////////////

	if (fGuiPtr) {
		fGuiPtr->GetArgValue("SaveSetup", fEnvFile);
		TString envFile = gSystem->BaseName(fEnvFile.Data());
		if (!envFile.BeginsWith(".Encal") || !envFile.EndsWith(".env")) {
			OutputMessage("SaveEnvFile", Form("Not an Encal environment file - %s", envFile.Data()), "e");
		} else {
			gSystem->Exec(Form("cp .Encal.env %s", fEnvFile.Data()));
			OutputMessage("SaveEnvFile", Form("Arguments saved to file %s", envFile.Data()));
		}
	} else{
		OutputMessage("SaveEnvFile", "Can't save setup - press \"Start\" first", "e");
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
	SaveEnvFile();
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

	fNofCalibrationLines = 0;
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
			OutputMessage("SetCalSource", "Wrong calibration source - performing peak fitting only", "e");
			return(kFALSE);
	}

	gSystem->ExpandPathName(fEnergies);
	if (gSystem->AccessPathName(fEnergies.Data())) {
		OutputMessage("SetCalSource", Form("No such file - %s, can't read calibration energies", fEnergies.Data()), "e");
		return(kFALSE);
	}

	fEnvEnergies = new TEnv(fEnergies.Data());

	TString calNames = fEnvEnergies->GetValue("Calib.SourceNames", "");
	if (calNames.IsNull()) {
		OutputMessage("SetCalSource", Form("File %s doesn't contain calibration energies for source \"%s\"", fEnergies.Data(), fSourceName.Data()), "e");
		delete fEnvEnergies;
		fEnvEnergies = NULL;
		return(kFALSE);
	}	
		
	fNofCalibrationLines = fEnvEnergies->GetValue(Form("Calib.%s.NofLines", fSourceName.Data()), 0);
	fNofPeaksNeeded = fNofCalibrationLines;
	if (fNofPeaksNeeded == 0) {
		OutputMessage(	"SetCalSource",
								Form("Error reading \"Calib.%s.NofLines\" from file %s - can't calibrate using %s source",
								fSourceName.Data(), fEnergies.Data(), fSourceName.Data()), "e");
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
	Double_t emin = 1000000.;
	Double_t emax = 0;
	for (Int_t i = 0; i < fNofPeaksNeeded; i++) {
		e = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.E", fSourceName.Data(), i), -1.0);
		if (e == -1) {
			OutputMessage(	"WriteGaugeFile",
								Form("Error reading \"Calib.%s.Line.%d.E\" from file %s - energy not given",
								fSourceName.Data(), i, fEnergies.Data()), "e");
		} else {
			de = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Eerr", fSourceName.Data(), i), 1.0);
			intens = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Intensity", fSourceName.Data(), i), 1.0);
			source = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Source", fSourceName.Data(), i), fSourceName.Data());
			gauge << source << " " << e << " " << de << " " << intens << endl;
			if (e < emin) emin = e;
			if (e > emax) emax = e;
			nofLines++;
		}
	}

	if (fVerboseMode) {
		OutputMessage("WriteGaugeFile", Form("%d calibration energies written to file %s", nofLines, fGaugeFile.Data()));
	}
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

	if (fRebin > 1) fCurHisto->Rebin(fRebin);

	TH1F * pfHisto;
	TH1F hEff;
	if (fNormHistoEff) {
		fCurHisto->Copy(hEff);
		pfHisto = &hEff;
		for (Int_t i = fMinX; i < fMaxX; i++) {
			Int_t bCont = (Int_t) hEff.GetBinContent(i);
			Double_t eff = 1. / (fEfficiencyA0 * exp(fEfficiencyA1 * i));
			hEff.SetBinContent(i, bCont * eff);
			hEff.SetBinError(i, sqrt(bCont) * eff);
		}
	} else {
		pfHisto = fCurHisto;
	}

	if (fPeakFinder) delete fPeakFinder;
	fPeakFinder = new FindPeakDialog(pfHisto, kFALSE);

	Bool_t sts = kTRUE;
	Int_t npeaks = 0;
	TList * pfList = NULL;
	Double_t pmaxSave = 0.1;
	for (Int_t i = 0; i < fNofRegions; i++) {
		fPeakFinder->SetFrom(fLowerLim[i]);
		fPeakFinder->SetTo(fUpperLim[i]);
		Double_t pmax = (Double_t) fPeakFrac[i] / 100.;
		if (pmax < .01) pmax = pmaxSave; else pmaxSave = pmax;
		fPeakFinder->SetThreshold(pmax);
		Double_t sigmaPeakFinder = fSigma * fTwoPeakSep / 3;
		fPeakFinder->SetSigma(sigmaPeakFinder);
		fPeakFinder->SetTwoPeakSeparation(1);
		fPeakFinder->ExecuteFindPeak();
		if (i == 0) pfList = (TList *) pfHisto->GetListOfFunctions()->FindObject("spectrum_peaklist");
		fNofPeaks = pfList->GetEntries();
		if ((fNofPeaks - npeaks) >= 200) {
			OutputMessage("FindPeaks", Form("Too many peaks in region [%d, %d] - max 200", fLowerLim[i], fUpperLim[i]), "e");
			sts = kFALSE;
			break;
		}
		npeaks = fNofPeaks;
	}

	fNofPeaks = pfList ? pfList->GetEntries() : 0;
	if (fNofPeaks == 0) {
		OutputMessage("FindPeaks", Form("No peaks found in histogram - %s", fCurHisto->GetName()), "e");
		sts = kFALSE;
	}

	SetFullRange(fCurHisto);

	if (fNormHistoEff) {
		if (fPeakList) delete fPeakList;
		fPeakList = new TList();
		fPeakList->SetName("spectrum_peaklist");
		TIterator * pIter = pfList->MakeIterator();
		FhPeak * p;
		while (p = (FhPeak *) pIter->Next()) fPeakList->Add(p);
		fCurHisto->GetListOfFunctions()->Add(fPeakList);
		TPolyMarker * pm = (TPolyMarker *) pfHisto->GetListOfFunctions()->FindObject("TPolyMarker");
		TArrayD y(pm->GetN());
		Double_t *x = pm->GetX();
		for (Int_t i = 0; i < pm->GetN(); i++) y[i] = fCurHisto->GetBinContent((Int_t) *x++);
		TPolyMarker * pmnew = new TPolyMarker(pm->GetN(), pm->GetX(), y.GetArray());
		pmnew->SetMarkerStyle(23);
		pmnew->SetMarkerColor(kRed);
		pmnew->SetMarkerSize(1.3);
		fCurHisto->GetListOfFunctions()->Add(pmnew);
	} else {
		fPeakList = pfList;
	}

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

	if (fFitOneDim) delete fFitOneDim;
	fFitOneDim = new FitOneDimDialog(fCurHisto, 1, kFALSE);
	fFitOneDim->SetPeakSep(fTwoPeakSep);
	fFitOneDim->SetFitWindow(fFitRange);
	fFitOneDim->SetBackg0(kFALSE);
	fFitOneDim->SetSlope0(fFitBackground == kFitBackgroundConst);
	fFitOneDim->SetLowtail(fFitMode == kFitModeLeftTail);
	fFitOneDim->SetHightail(fFitMode == kFitModeRightTail);
	fFitOneDim->SetConfirmStartValues(kFALSE);
	fFitOneDim->SetShowcof(kTRUE);
	fFitOneDim->SetFitOptVerbose(fVerboseMode);

	return(fFitOneDim->FitPeakList());
}

Bool_t Calibrate() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           Calibrate
// Purpose:        Perform energy calibration
// Arguments:      --
// Results:        --
// Description:    Starts energy calibration
//////////////////////////////////////////////////////////////////////////////

	TIterator * pIter;
	FhPeak * p;

	if (!fEnableCalib) return(kFALSE);

	if (fNofPeaks < fNofPeaksNeeded) { 
		OutputMessage("Calibrate", Form("Too few peaks - %d (%s calibration needs at least %d peaks)", fNofPeaks, fSourceName.Data(), fNofPeaksNeeded), "w");
		SetFitStatus(kFitDiscard, "Too few peaks");
	}

	if (fCalibration) delete fCalibration;
	fCalibration = new CalibrationDialog(fCurHisto, kFALSE);
	if (fMatchFlag) {
		fCalibration->SetVerbose(fVerboseMode);
		fCalibration->SetCustomGauge(kTRUE);
		fCalibration->SetCustomGaugeFile(fGaugeFile);
		fCalibration->SetMatchNbins(fMatchNbins);
		fCalibration->SetMatchMin(fMatchEmin);
		fCalibration->SetMatchMax(fMatchEmax);
		fCalibration->SetOffMin(fMatchOffsetMin);
		fCalibration->SetOffStep(fMatchOffsetStep);
		fCalibration->SetOffMax(fMatchOffsetMax);
		fCalibration->SetGainMin(fMatchGainMin);
		fCalibration->SetGainStep(fMatchGainStep);
		fCalibration->SetGainMax(fMatchGainMax);
		fCalibration->SetAccept((Double_t) fMatchAccept);
		if (!fCalibration->ExecuteAutoSelect()) return(kFALSE);
		fFitList = fCalibration->GetPeakList();
	} else {
		Int_t np = 0;
		Int_t npg = 0;
		fFitList = fCalibration->UpdatePeakList();
		pIter = fFitList->MakeIterator();
		while (p = (FhPeak *) pIter->Next()) {
			if (np < fNofPeaks - fNofPeaksNeeded) {
				p->SetUsed(0);
				fCalibration->SetGaugePoint(np, 0);
			} else {
				Double_t e = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.E", fSourceName.Data(), npg), 0.0);
				Double_t eerr = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Eerr", fSourceName.Data(), npg), 1.0);
				Double_t intens = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Intensity", fSourceName.Data(), npg), 0.0);
				TString gauge = fEnvEnergies->GetValue(Form("Calib.%s.Line.%d.Source", fSourceName.Data(), npg), "??");
				fCalibration->SetGaugePoint(np, 1, p->GetMean(), e, p->GetMeanError(), eerr);
				p->SetUsed(1);
				p->SetGaugeName(gauge.Data());
				p->SetNominalEnergy(e);
				p->SetNominalEnergyError(eerr);
				p->SetIntensity(intens);
				npg++;
			}
			np++;
		}
	}
	fCalibFct = fCalibration->CalculateFunction();
	if (fCalibFct == NULL) return(kFALSE);
	fMainCanvas->cd(2);
	TArrayD x(fNofPeaks);
	TArrayD xerr(fNofPeaks);
	TArrayD y(fNofPeaks);
	TArrayD yerr(fNofPeaks);
	pIter = fFitList->MakeIterator();
	fNofPeaksUsed = 0;
	while (p = (FhPeak *) pIter->Next()) {
		if (p->GetUsed()) {
			x[fNofPeaksUsed] = p->GetMean();
			xerr[fNofPeaksUsed] = p->GetMeanError();
			y[fNofPeaksUsed] = p->GetNominalEnergy();
			yerr[fNofPeaksUsed] = p->GetNominalEnergyError();
			fNofPeaksUsed++;
		}
	}
	if (fNofPeaksUsed < fNofPeaksNeeded) {
		OutputMessage("Calibrate", Form("Too few peaks - %d (%s calibration needs at least %d peaks)", fNofPeaksUsed, fSourceName.Data(), fNofPeaksNeeded), "w");
		SetFitStatus(kFitDiscard, "Too few peaks");
	}
	if (!fMatchFlag) {
		fMainCanvas->cd(1);
		pIter = fFitList->MakeIterator();
		while (p = (FhPeak *) pIter->Next()) {
			if (p->GetUsed()) MarkPeak(p->GetMean(), p->GetNominalEnergy(), p->GetGaugeName());
		}
	}
	ClearCanvas(2);
	TGraphErrors * gr = new TGraphErrors(fNofPeaksUsed, x.GetArray(), y.GetArray(), xerr.GetArray(), yerr.GetArray()); 
	gr->SetMarkerStyle(4);
	gr->SetMarkerSize(1);
	gr->Draw("A*");
	if (fCalibFct) {
		fCalibFct->Draw("SAME"); 
		fCalibFct->SetLineWidth(1);
		fCalibFct->SetLineColor(7);
		TString grName = Form("%s Calibration, histo %s: %g + %g * x",
											fSourceName.Data(), fCurHisto->GetName(),
											fCalibFct->GetParameter(0), fCalibFct->GetParameter(1));
		gr->SetTitle(grName.Data());
	}
	gr->GetHistogram()->SetBins(fMaxX - fMinX, fMinX, fMaxX);
	fCalibFct->SetName(Form("Cal_Fct_%s", fCurHisto->GetName()));
	gr->GetListOfFunctions()->Add(fCalibFct);
	gr->SetName(Form("Cal_Graph_%s", fCurHisto->GetName()));
	if (fFitResults) fFitResults->Append(gr);
	gPad->Modified();
	gPad->Update();
	return(kTRUE);
}

void MarkPeak(Double_t X, Double_t E, const Char_t * GaugeName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           MarkCalibratedPeaks
// Purpose:        Mark a peak
// Arguments:      Double_t X          -- x (fitted mean value)
//                 Double_t E          -- energy (nominal gauge value)
//                 Char_t * GaugeName  -- source name
// Results:        --
// Description:    Marks a peak with energy and gauge name
//////////////////////////////////////////////////////////////////////////////

	Int_t bin = fCurHisto->FindBin(X);
	Double_t yv = fCurHisto->GetBinContent(bin);
	Double_t yr = fCurHisto->GetMaximum();
	Double_t xr = fCurHisto->GetBinCenter(fCurHisto->GetXaxis()->GetLast()) 
					- fCurHisto->GetBinCenter(fCurHisto->GetXaxis()->GetFirst());
	TLine * l = new TLine(X + 0.025 * xr, yv + 0.025*yr, X, yv);
	l->SetLineWidth(2);
	l->Draw();
         
	TString t = GaugeName;

	Int_t ie = (Int_t) E;
	t +=  "(";
	t += ie;
	t += ")";
	TLatex latex; 
	latex.SetTextSize(0.05);
	latex.SetTextColor(kBlue);
	latex.DrawLatex(X + 0.025 * xr, yv + 0.025*yr, t);
	gPad->Modified();
	gPad->Update();
}

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
	fEnvResults->SetValue(Form("Calib.%s.NofPeaksUsed", fCurHisto->GetName()), fNofPeaksUsed);
	TIterator * p1Iter = fPeakList->MakeIterator();
	TIterator * p2Iter = fFitList->MakeIterator();
	FhPeak * p1;
	FhPeak * p2;
	Int_t np = 0;
	while ((p1 = (FhPeak *) p1Iter->Next()) && (p2 = (FhPeak *) p2Iter->Next())) {
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Centroid", fCurHisto->GetName(), np), p1->GetMean());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Mean", fCurHisto->GetName(), np), p2->GetMean());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.MeanError", fCurHisto->GetName(), np), p2->GetMeanError());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Content", fCurHisto->GetName(), np), p2->GetContent());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.ContentError", fCurHisto->GetName(), np), p2->GetContentError());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Width", fCurHisto->GetName(), np), p2->GetWidth());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.WidthError", fCurHisto->GetName(), np), p2->GetWidthError());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.TailContent", fCurHisto->GetName(), np), p2->GetTailContent());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.TailWidth", fCurHisto->GetName(), np), p2->GetTailWidth());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Chi2", fCurHisto->GetName(), np), p2->GetChi2oNdf());
		fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Used", fCurHisto->GetName(), np), p2->GetUsed() ? "TRUE" : "FALSE");
		if (p2->GetUsed()) {
			fEnvResults->SetValue(Form("Calib.%s.Peak.%d.NominalEnergy", fCurHisto->GetName(), np), p2->GetNominalEnergy());
			fEnvResults->SetValue(Form("Calib.%s.Peak.%d.NominalEnergyError", fCurHisto->GetName(), np), p2->GetNominalEnergyError());
			fEnvResults->SetValue(Form("Calib.%s.Peak.%d.Intensity", fCurHisto->GetName(), np), p2->GetIntensity());
			fEnvResults->SetValue(Form("Calib.%s.Peak.%d.CalibratedEnergy", fCurHisto->GetName(), np), fCalibFct->Eval(p2->GetMean()));
			fEnvResults->SetValue(Form("Calib.%s.Peak.%d.RelEfficiency", fCurHisto->GetName(), np), p2->GetRelEfficiency());
		}
		np++;
	}
	if (fFitResults) fFitResults->Append(fCurHisto);
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

	if (fCalibFct && fEnableCalib && (fNofPeaks >= fNofPeaksNeeded)) {
		fEnvCalib->SetValue(Form("Calib.%s.Xmin", fCurHisto->GetName()), fMinX);
		fEnvCalib->SetValue(Form("Calib.%s.Xmax", fCurHisto->GetName()), fMaxX);
		fEnvCalib->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fCalibFct->GetParameter(1));
		fEnvCalib->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fCalibFct->GetParameter(0));
		fEnvCalib->SetValue(Form("Calib.%s.GainError", fCurHisto->GetName()), fCalibFct->GetParError(1));
		fEnvCalib->SetValue(Form("Calib.%s.OffsetError", fCurHisto->GetName()), fCalibFct->GetParError(0));
		fEnvResults->SetValue(Form("Calib.%s.Gain", fCurHisto->GetName()), fCalibFct->GetParameter(1));
		fEnvResults->SetValue(Form("Calib.%s.Offset", fCurHisto->GetName()), fCalibFct->GetParameter(0));
		fEnvResults->SetValue(Form("Calib.%s.GainError", fCurHisto->GetName()), fCalibFct->GetParError(1));
		fEnvResults->SetValue(Form("Calib.%s.OffsetError", fCurHisto->GetName()), fCalibFct->GetParError(0));
		SetFitStatus(StepMode() ? kFitOk : kFitAuto);
	} else {
		SetFitStatus(kFitDiscard, "Too few peaks");
	}
}

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
			sts = Form("File %s, histo %s: E(x) = %5.2f + %5.2f * x",
									fHistoFile->GetName(),
									fCurHisto->GetName(),
									fCalibFct->GetParameter(0),
									fCalibFct->GetParameter(1));
		} else {
			sts = Form("File %s, histo %s: %d peak(s)",
									fHistoFile->GetName(),
									fCurHisto->GetName(),
									fNofPeaks);
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

	if (FitStatus != kFitUndef && fFitStatus == kFitUndef) {
		fFitStatus = FitStatus;
		fReason = (Reason == NULL || *Reason == '\0') ? "" : Reason;
		TMrbNamedX * fs = fLofFitStatusFlags.FindByIndex(fFitStatus);
		TString fsStr = (fs != NULL) ? fs->GetName() : "";
		if (!fReason.IsNull()) fsStr += Form(": %s", fReason.Data());
		if (fEnvResults) fEnvResults->SetValue(Form("Calib.%s.FitStatus", fCurHisto->GetName()), fsStr.Data());
		if (fEnvCalib) fEnvCalib->SetValue(Form("Calib.%s.FitStatus", fCurHisto->GetName()), fsStr.Data());
	}
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

	Int_t nh = GetNofHistosCalibrated();
	if (nh <= 1) return;

	TList * lofEntries = (TList *) fEnvCalib->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	nh = 0;
	TObjArray lofCalibs;
	Double_t emin = 1000000;
	Double_t emax = 0;
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
					const Char_t * formula = Form("%g + %g * x[0]", a0, a1);
					TFormula * poly = new TFormula(hn.Data(), formula);
					Double_t e = poly->Eval(fMinX);
					if (e < emin) emin = e;
					e = poly->Eval(fMaxX);
					if (e > emax) emax = e;
					lofCalibs.Add(poly);
					nh++;
				}
			}
		}
	}

	if (nh == 0) {
		OutputMessage("ShowResults2dim", Form("File %s - No histograms found", fHistoFile->GetName()), "e");
	} else {
 		f2DimCanvas = new TCanvas();
		TString hTitle = Form("Calibration restults: file %s, %d histogram(s)", fHistoFile->GetName(), nh);
		Int_t llim = (Int_t) emin;
		Int_t ulim = (Int_t) emax;
		f2DimFitResults = new TH2S("hCalResults", hTitle.Data(), ulim - llim, llim, ulim, nh, 0, nh);
		if (fFitResults) fFitResults->Append(f2DimFitResults);
		TAxis * yAxis = f2DimFitResults->GetYaxis();

		for (Int_t n = 0; n < nh; n++) {
			TFormula * poly = (TFormula *) lofCalibs[n];
			TString hn = poly->GetName();
			TH1F * h = (TH1F *) fHistoFile->Get(hn.Data());
			if (h != NULL) {
				for (Int_t i = fMinX; i < fMaxX; i++) {
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
		f2DimCanvas->SetLeftMargin(.1);
		f2DimFitResults->Draw();
		f2DimCanvas->Update();
	}
}

void ShowThumbNails() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           ShowThumbNails
// Purpose:        Show a thumbnail plot
// Arguments:      --
// Results:        --
// Description:    Shows all histos and fits in one canvas
//////////////////////////////////////////////////////////////////////////////

	TObjArray lofHistos;
	lofHistos.Delete();

	TIterator * hIter = fLofHistos.MakeIterator();
	TObjString * oh = NULL;
	while (oh = (TObjString *) hIter->Next()) {
		if (WithinHistoMask(oh->GetString())) lofHistos.Add(new TObjString(oh->GetString()));
	}
	Int_t nh = lofHistos.GetEntriesFast();

	if (ThumbNailMode() && (nh > 1)) {

		fThumbCanvas = new TCanvas(Form("File %s: thumbsnails", fHistoFile->GetName()));
		Int_t nx = (Int_t) (sqrt((Float_t) nh) + .5);
		Int_t ny = (Int_t) (((Float_t) nh) / nx + .5);
		fThumbCanvas->Divide(nx, ny);
		Int_t nc = 1;
		for (Int_t i = 0; i < nx; i++) {
			for (Int_t j = 0; j < ny; j++) {
				if (nc > nh) break;
				TString hName = ((TObjString *) lofHistos[nc - 1])->GetString();
				fThumbCanvas->cd(nc++);
				if (WithinHistoMask(hName.Data())) {
					TH1F * fThumbHisto = (TH1F *) fHistoFile->Get(hName.Data());
					if (fThumbHisto) {
						TString cmd(Form("PopupHisto((TH1F*) %#x);", fThumbHisto)); 
						gPad->SetToolTipText(hName.Data(), 250);
						gPad->AddExec("popupHisto", cmd.Data());
						fThumbHisto->Draw();
						gPad->Modified();
						gPad->Update();
					}
				}
			}
			if (nc > nh) break;
		}
		fThumbCanvas->Modified();
		fThumbCanvas->Update();
	}

	TString hlName = fFitFile;
	hlName.ReplaceAll(".root", 5, ".histlist", 9);
	gSystem->Exec(Form("rm -f %s", hlName.Data()));
	if (nh > 1) {
		ofstream hl(hlName.Data());
		for (Int_t i = 0; i < nh; i++) hl << ((TObjString *) lofHistos[i])->GetString() << endl;
		hl.close();
	} 
}

void PopupHisto(TH1F * Histo) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           PopupHisto
// Purpose:        Popup canvas and show histo
// Arguments:      TH1F * Histo   -- histo address
// Results:        --
// Description:    Pops up a new canvas and displays histo.
//////////////////////////////////////////////////////////////////////////////

	Int_t event = gPad->GetEvent();
	if (event == kButton1Down) {
		TCanvas * c = new TCanvas(Histo->GetName());
		c->Divide(1,2);
		c->cd(1);
		Histo->Draw();
		TGraphErrors * gr = (TGraphErrors *) gROOT->FindObject(Form("Cal_Graph_%s", Histo->GetName()));
		if (gr) {
			c->cd(2);
			gr->Draw("A*");
		}
	}
}

void GetArguments(TGMrbMacroFrame * GuiPtr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           GetArguments
// Purpose:        Read arguments from GUI
// Arguments:      TGMrbMacroFrame * GuiPtr  -- pointer to MacroBrowser GUI
// Results:        --
// Description:    Imports arguments from GUI
//////////////////////////////////////////////////////////////////////////////

	if (GuiPtr == NULL) {
		OutputMessage("GetArguments", "Pointer to MacroBrowser GUI is NULL - can't continue", "e");
		gSystem->Exit(1);
	}

	GuiPtr->GetArgValue("HistoFile", fLofHistos);
	GuiPtr->GetArgValue("HistoMask", fHistoMask);
	fHistoFileName = ((TObjString *) fLofHistos[0])->GetString();
	fLofHistos.RemoveAt(0);
	fLofHistos.Compress();

	GuiPtr->GetArgValue("CalSource", fCalSource);
	GuiPtr->GetArgValue("Energies", fEnergies);
	if (fEnergies.IsNull()) fEnergies = "$MARABOU/data/encal/calibEnergies.all";
	GuiPtr->GetArgValue("CalFile", fCalFile);
	if (fCalFile.IsNull()) fCalFile = "Encal.cal";
	GuiPtr->GetArgValue("ResFile", fResFile);
	if (fResFile.IsNull()) { fResFile = fCalFile; fResFile.ReplaceAll(".cal", 4, ".res", 4); }
	GuiPtr->GetArgValue("FitFile", fFitFile);
	if (fFitFile.IsNull()) { fFitFile = fCalFile; fFitFile.ReplaceAll(".cal", 4, ".root", 5); }
	GuiPtr->GetArgValue("ClearFlag", fClearFlag);

	if (GuiPtr->ArgIsChecked("Rebin")) GuiPtr->GetArgValue("Rebin", fRebin); else fRebin = 1;


	fNofRegions = 0;
	for (Int_t i = 1; i <= 3; i++) {
		TString arg = Form("Region%d", i);
		if (GuiPtr->ArgIsChecked(arg.Data())) {
			Int_t chnlow;
			GuiPtr->GetArgValue(arg.Data(), chnlow, 0);
			fLowerLim[fNofRegions] = chnlow;
			Int_t chnup;
			GuiPtr->GetArgValue(arg.Data(), chnup, 1);
			fUpperLim[fNofRegions] = chnup;
			if (chnup < chnlow) {
				OutputMessage("GetArguments",
					Form("Upper limit region %d out of range - %d (should be > %d)", i, chnup, chnlow), "e");
				continue;
			}
			Int_t pfrac;
			GuiPtr->GetArgValue(arg.Data(), pfrac, 2);
			fPeakFrac[fNofRegions] = pfrac;
			fNofRegions++;
		}
	}

	if (fNofRegions == 0) {
		OutputMessage("GetArguments", "At least 1 region [lower,upper] necessary", "e");
		gSystem->Exit(1);
	}

	fMinX = 1000000;
	fMaxX = 0;
	for (Int_t i = 0; i < fNofRegions; i++) {
		if (fMinX > fLowerLim[i]) fMinX = fLowerLim[i];
		if (fMaxX < fUpperLim[i]) fMaxX = fUpperLim[i];
	}

	Double_t a0, a1;
	if (GuiPtr->ArgIsChecked("Efficiency")) {
		GuiPtr->GetArgValue("Efficiency", a0, 0);
		GuiPtr->GetArgValue("Efficiency", a1, 1);
		fNormHistoEff = kTRUE;
		fEfficiencyA0 = a0;
		fEfficiencyA1 = a1;
	} else {
		fNormHistoEff = kFALSE;
		fEfficiencyA0 = 0;
		fEfficiencyA1 = 0;
	}

	GuiPtr->GetArgValue("Sigma", fSigma);
	GuiPtr->GetArgValue("TwoPeakSep", fTwoPeakSep);
	GuiPtr->GetArgValue("FitMode", fFitMode);
	GuiPtr->GetArgValue("FitGrouping", fFitGrouping);
	GuiPtr->GetArgValue("FitBackground", fFitBackground);
	GuiPtr->GetArgValue("FitRange", fFitRange);
	GuiPtr->GetArgValue("TestbedHisto", fMatchNbins, 0);
	GuiPtr->GetArgValue("TestbedHisto", fMatchEmin, 1);
	GuiPtr->GetArgValue("TestbedHisto", fMatchEmax, 2);
	GuiPtr->GetArgValue("PeakMatch", fMatchFlag);
	if (fMatchFlag) {
		GuiPtr->GetArgValue("MatchOffset", fMatchOffsetMin, 0);
		GuiPtr->GetArgValue("MatchOffset", fMatchOffsetStep, 1);
		GuiPtr->GetArgValue("MatchOffset", fMatchOffsetMax, 2);
		if ((fMatchOffsetMax - fMatchOffsetMin) <= 0) {
			OutputMessage("GetArguments", Form("Wrong offset range - [%d,%d]", fMatchOffsetMin, fMatchOffsetMax), "e");
			fMatchFlag = kFALSE;
		}
		if (fMatchOffsetStep <= 0) {
			OutputMessage("GetArguments", Form("Wrong offset step size - %d", fMatchOffsetStep), "e");
			fMatchFlag = kFALSE;
		}
		GuiPtr->GetArgValue("MatchGain", fMatchGainMin, 0);
		GuiPtr->GetArgValue("MatchGain", fMatchGainStep, 1);
		GuiPtr->GetArgValue("MatchGain", fMatchGainMax, 2);
		if ((fMatchGainMax - fMatchGainMin) <= 0) {
			OutputMessage("GetArguments", Form("Wrong gain range - [%d,%d]", fMatchGainMin, fMatchGainMax), "e");
			fMatchFlag = kFALSE;
		}
		if (fMatchGainStep <= 0) {
			OutputMessage("GetArguments", Form("Wrong gain step size - %d", fMatchGainStep), "e");
			fMatchFlag = kFALSE;
		}
		GuiPtr->GetArgValue("MatchAccept", fMatchAccept);
	}
	GuiPtr->GetArgValue("DisplayMode", fDisplayMode);
	GuiPtr->GetArgValue("VerboseMode", fVerboseMode);
}

void SetArguments(TGMrbMacroFrame * GuiPtr, const Char_t * EnvFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           SetArguments
// Purpose:        Set GUI args from env file
// Arguments:      TGMrbMacroFrame * GuiPtr  -- pointer to MacroBrowser GUI
//                 Char_t * EnvFile          -- env file
// Results:        --
// Description:    Sets gui values from environment file
//////////////////////////////////////////////////////////////////////////////

	if (GuiPtr == NULL) {
		OutputMessage("SetArguments", "Pointer to MacroBrowser GUI is NULL - no action", "e");
		return;
	}

	if (gSystem->AccessPathName(EnvFile)) {
		OutputMessage("SetArguments", Form("No such file - %s", EnvFile), "e");
		return;
	}

	TEnv * env = new TEnv(EnvFile);

	Int_t nofArgs = env->GetValue("NofArgs", 0);
	if (nofArgs == 0) {
		OutputMessage("SetArguments", Form("No arguments found on file %s", EnvFile), "e");
		return;
	}

	TMrbLofNamedX lofArgs;
	lofArgs.Delete();
	for (Int_t i = 1; i <= nofArgs; i++) lofArgs.AddNamedX(i, env->GetValue(Form("Arg%d.Name", i), "NoName"));
	TMrbNamedX * argX;

	Int_t envInt;
	Double_t envDbl;
	TString envStr;

	argX = lofArgs.FindByName("HistoFile");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current.NofValues", argX->GetIndex()), 0);
		TObjArray ha;
		for (Int_t i = 0; i < envInt; i++) ha.Add(new TObjString(env->GetValue(Form("Arg%d.Current.%d", kArgHistoFile, i), "")));
		GuiPtr->SetArgValue("HistoFile", ha);
	}

	argX = lofArgs.FindByName("HistoMask");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "*");
		if (envStr.IsNull()) envStr = "*";
		GuiPtr->SetArgValue("HistoMask", envStr.Data());
	}

	argX = lofArgs.FindByName("CalSource");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), 0);
		GuiPtr->SetArgValue("CalSource", envInt);
	}

	argX = lofArgs.FindByName("Energies");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		if (envStr.IsNull()) envStr = "$MARABOU/data/encal/calibEnergies.all";
		GuiPtr->SetArgValue("Energies", envStr.Data());
	}

	argX = lofArgs.FindByName("CalFile");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		if (envStr.IsNull()) envStr = "Encal.cal";
		GuiPtr->SetArgValue("CalFile", envStr.Data());
	}

	argX = lofArgs.FindByName("ResFile");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		if (envStr.IsNull()) { envStr = fCalFile; envStr.ReplaceAll(".cal", 4, ".res", 4); }
		GuiPtr->SetArgValue("ResFile", envStr.Data());
	}

	argX = lofArgs.FindByName("FitFile");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		if (envStr.IsNull()) { envStr = fCalFile; envStr.ReplaceAll(".cal", 4, ".root", 5); }
		GuiPtr->SetArgValue("FitFile", envStr.Data());
	}

	argX = lofArgs.FindByName("ClearFlag");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kTRUE);
		GuiPtr->SetArgValue("ClearFlag", envInt);
	}

	argX = lofArgs.FindByName("Rebin");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		TString tf = ((TObjString *) a->At(1))->GetString();
		if (tf.CompareTo("T") == 0) {
			GuiPtr->SetArgValue("Rebin", 1);
		} else {
			envStr = ((TObjString *) a->At(0))->GetString();
			GuiPtr->SetArgValue("Rebin", envStr.Atoi(), 0);
		}
		delete a;
	}

	for (Int_t reg = 1; reg <= 3; reg++) {
		TString arg = Form("Region%d", reg);
		argX = lofArgs.FindByName(arg.Data());
		if (argX) {
			envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
			TObjArray * a = envStr.Tokenize(":");
			TString tf = ((TObjString *) a->At(3))->GetString();
			if (tf.CompareTo("T") == 0) {
				GuiPtr->SetArgCheck(arg.Data(), 1);
				for (Int_t i = 0; i < 3; i++) {
					envStr = ((TObjString *) a->At(i))->GetString();
					GuiPtr->SetArgValue(arg.Data(), envStr.Atoi(), i);
				}
			} else {
				GuiPtr->SetArgCheck(arg.Data(), 0);
				for (Int_t i = 0; i < 2; i++)  GuiPtr->SetArgValue(arg.Data(), 0, i);
				GuiPtr->SetArgValue(arg.Data(), 10, 2);
			}
			delete a;
		}
	}

	argX = lofArgs.FindByName("Efficiency");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		TString tf = ((TObjString *) a->At(2))->GetString();
		if (tf.CompareTo("T") == 0) {
			GuiPtr->SetArgCheck("Efficiency", 1);
			for (Int_t i = 0; i < 2; i++) {
				envStr = ((TObjString *) a->At(i))->GetString();
				GuiPtr->SetArgValue("Efficiency", envStr.Atof(), i);
			}
		} else {
			GuiPtr->SetArgCheck("Efficiency", 0);
			for (Int_t i = 0; i < 2; i++)  GuiPtr->SetArgValue("Efficiency", 0.0, i);
		}
		delete a;
	}
	argX = lofArgs.FindByName("PeakFrac");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		for (Int_t k = 0; k < 4; k++) {
			envStr = ((TObjString *) a->At(k))->GetString();
			GuiPtr->SetArgValue("PeakFrac", envStr.Atoi(), k);
		}
		delete a;
	}
	argX = lofArgs.FindByName("Sigma");
	if (argX) {
		envDbl = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), 3.0);
		GuiPtr->SetArgValue("Sigma", envDbl);
	}
	argX = lofArgs.FindByName("TwoPeakSep");
	if (argX) {
		envDbl = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), 1.0);
		GuiPtr->SetArgValue("TwoPeakSep", envDbl);
	}
	argX = lofArgs.FindByName("FitMode");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kFitModeGauss);
		GuiPtr->SetArgValue("FitMode", envInt);
	}
	argX = lofArgs.FindByName("FitGrouping");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kFitGroupingSinglePeak);
		GuiPtr->SetArgValue("FitGrouping", envInt);
	}
	argX = lofArgs.FindByName("FitBackground");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kFitBackgroundLinear);
		GuiPtr->SetArgValue("FitBackground", envInt);
	}
	argX = lofArgs.FindByName("FitRange");
	if (argX) {
		envDbl = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), 3.0);
		GuiPtr->SetArgValue("FitRange", envDbl);
	}
	argX = lofArgs.FindByName("PeakMatch");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kTRUE);
		GuiPtr->SetArgValue("PeakMatch", envInt);
	}
	argX = lofArgs.FindByName("TestbedHisto");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		for (Int_t k = 0; k < 3; k++) {
			envStr = ((TObjString *) a->At(k))->GetString();
			GuiPtr->SetArgValue("TestbedHisto", envStr.Atoi(), k);
		}
		delete a;
	}
	argX = lofArgs.FindByName("MatchOffset");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		for (Int_t k = 0; k < 3; k++) {
			envStr = ((TObjString *) a->At(k))->GetString();
			GuiPtr->SetArgValue("MatchOffset", envStr.Atof(), k);
		}
		delete a;
	}
	argX = lofArgs.FindByName("MatchGain");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		TObjArray * a = envStr.Tokenize(":");
		for (Int_t k = 0; k < 3; k++) {
			envStr = ((TObjString *) a->At(k))->GetString();
			GuiPtr->SetArgValue("MatchGain", envStr.Atof(), k);
		}
		delete a;
	}
	argX = lofArgs.FindByName("MatchAccept");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), 2);
		GuiPtr->SetArgValue("MatchAccept", envInt);
	}
	argX = lofArgs.FindByName("DisplayMode");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kDisplayModeStep);
		GuiPtr->SetArgValue("DisplayMode", envInt);
	}
	argX = lofArgs.FindByName("VerboseMode");
	if (argX) {
		envInt = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), kFALSE);
		GuiPtr->SetArgValue("VerboseMode", envInt);
	}
	argX = lofArgs.FindByName("SaveSetup");
	if (argX) {
		envStr = env->GetValue(Form("Arg%d.Current", argX->GetIndex()), "");
		if (envStr.IsNull()) envStr = "Encal.cal";
		GuiPtr->SetArgValue("SaveSetup", envStr.Data());
	}
}

//+CallingSequence
void Encal(TGMrbMacroFrame * GuiPtr)
//-CallingSequence
//_____________________________________________________________[MAIN FUNCTION]
//
{
	fGuiPtr = GuiPtr;

	GetArguments(GuiPtr);

	fNofPeaksNeeded = 0;
	fNofCalibrationLines = 0;
	fEnableCalib = kFALSE;
	fEnvEnergies = NULL;
	fEnvCalib = NULL;
	fFitResults = NULL;

	TString pNames = "Ta_Frac:Ta_Width:Bg_Const:Bg_Slope:Ga_Sigma:Ga_Const:Ga_Mean";
	fParamNames = pNames.Tokenize(":");

	fPeakFinder = NULL;
	fFitOneDim = NULL;
	fCalibration = NULL;

	if (fMainCanvas) { delete fMainCanvas; fMainCanvas = NULL; }
	if (f2DimCanvas) { delete f2DimCanvas; f2DimCanvas = NULL; }
	if (fThumbCanvas) { delete fThumbCanvas; fThumbCanvas = NULL; }
	if (fPopupCanvas) { delete fPopupCanvas; fPopupCanvas = NULL; }

	fLofFitStatusFlags.AddNamedX(kFitDiscard, "Discarded");
	fLofFitStatusFlags.AddNamedX(kFitOk, "Ok");
	fLofFitStatusFlags.AddNamedX(kFitAuto, "Auto");

	if (!OpenHistoFile()) return;

	if (!SetCalSource()) return;

	OpenCalFiles();

	OpenCanvas();

	ResetLofHistos();
	TH1F * h;
	fNextHisto = kNextHisto;
	while (h = GetNextHisto()) {

		GetArguments(GuiPtr);

		if (h == NULL) {
			ClearCanvas(0);
			WaitForSignal();
			if (fButtonStop || fButtonQuit) { Stop(); break; }
			continue;
		}

		if (!WithinHistoMask(h->GetName())) continue;

		SetCurHisto(h);
		SetFullRange(h);
		ClearCanvas(2);
		fMainCanvas->cd(1);
		h->Draw("");

		fFitStatus = kFitUndef;

		if (FindPeaks()) {

			if (FitPeaks()) {

				if (Calibrate()) {
					WriteCalibration();
					WriteResults();
				}
			}
			fMainCanvas->Update();
			gSystem->ProcessEvents();
		} else {
			fMainCanvas->cd(1);
			h->Draw();
			ClearCanvas(2);
		}

		WaitForSignal();
		if (fButtonStop || fButtonQuit) { Stop(); break; }
	}

	if (!fButtonQuit) {
		ShowResults2dim();
		ShowThumbNails();
	}

	CloseEnvFiles();

	OutputMessage(NULL, Form("%s - %d histogram(s), press \"execute\" to restart or \"quit\" to exit", HistoFile()->GetName(), GetNofHistosCalibrated()), "o");

	if (!fButtonQuit) WaitForSignal(kTRUE);

	CloseRootFile();
	if (fButtonQuit) { Exit(); }

	return;
}
