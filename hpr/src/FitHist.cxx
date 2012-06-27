#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "TROOT.h"
#include "TQObject.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TCutG.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TGObject.h"
#include "TObjectTable.h"
#include "TTimer.h"
#include "TProfile.h"
#include "TPad.h"
#include "TMath.h"
#include "TPaletteAxis.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TPolyLine.h"
#include "TPaveLabel.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "Buttons.h"
#include "TButton.h"
#include "TDiamond.h"
#include "HTCanvas.h"
#include "TContextMenu.h"
#include "TArray.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TRint.h"
#include "TList.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TArrayC.h"
#include "TArrayI.h"
#include "TExec.h"
#include "TLegendEntry.h"
#include "THistPainter.h"

#include "TGWidget.h"
#include "HprGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitHist_Help.h"
#include "CmdListEntry.h"
#include "HistPresent.h"
#include "FhContour.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
#include "TMrbNamedArray.h"
#include "TMrbString.h"
#include "TMrbArrayD.h"
#include "TGMrbSliders.h"
#include "TGMrbInputDialog.h"
#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "FitOneDimDialog.h"
#include "Fit2DimDialog.h"
#include "SetHistOptDialog.h"
#include "SetColorModeDialog.h"
#include "WindowSizeDialog.h"
#include "GeneralAttDialog.h"
#include "HandleMenus.h"

#include "hprbase.h"

extern HistPresent *gHpr;
extern Int_t nHists;

enum dowhat { expand, projectx, projecty, statonly, projectf,
       projectboth , profilex, profiley};

ClassImp(FitHist)

//_______________________________________________________________________________
// constructor

FitHist::FitHist(const Text_t * name, const Text_t * title, TH1 * hist,
               const Text_t * hname, Int_t win_topx, Int_t win_topy,
               Int_t win_widx, Int_t win_widy)
        :TNamed(name, title)
{
   if (!hist) {
      cout << "NULL pointer in: " << name << endl;
      return;
   };

   if (gDirectory) {
      FitHist *hold =
          (FitHist *) gDirectory->GetList()->FindObject(GetName());
      if (hold) {
//         Warning("Build","Replacing existing : %s",GetName());
			cout << "FitHist ctor: this " <<this << " Delete hold: " << hold << endl;
         gDirectory->GetList()->Remove(hold);
			gROOT->GetListOfCanvases()->Remove(hold->GetCanvas());
         delete hold->GetCanvas();
      }
//      AppendDirectory();
      gDirectory->Append(this);
   }
//   hp = (HistPresent *) gROOT->FindObject("mypres");
   if (!gHpr)
      cout << "running without HistPresent" << endl;
   fHname = hname;
//   Int_t pp = fHname.Index(".");
//   if(pp) fHname.Remove(0,pp+1);
	if (gDebug > 0)
		cout << "FitHist ctor: " << this << " name: " << name << " title: " << name
		<< " hname: " << fHname.Data()
		<< endl << flush;
   fCutPanel = NULL;
//   fDialog  = NULL;
   fSetRange = kFALSE;
   fRangeLowX = 0;
   fRangeUpX = 0;
   fRangeLowY = 0;
   fRangeUpY = 0;
   fOrigLowX = hist->GetXaxis()->GetXmin();
   fOrigUpX  = hist->GetXaxis()->GetXmax();
   fOrigLowY = hist->GetYaxis()->GetXmin();
   fOrigUpY =  hist->GetYaxis()->GetXmax();
	fBinX_1 = fBinX_2 = fBinY_1 = fBinY_2 = 0;
	fBinlx  = fBinux = fBinly = fBinuy = 0;    
	fExplx  = fExpux = fExply = fExpuy = 0;    
	fX_1    =   fX_2 =   fY_1 =   fY_2 = 0;    
	fRangeLowX = fRangeUpX = fRangeLowY = fRangeUpY = 0;
//	fOrigLowX  = fOrigUpX  = fOrigLowY  = fOrigUpY  = 0;
	fFrameX1   = fFrameX2  = fFrameY1   = fFrameY2  = 0;
   fBinX_1 = fBinX_2 = -1;
   fLogx = 0;
   fLogy = 0;
   fLogz = 0;
   fUserContourLevels = 0;
//   datebox = 0;
   fExpInd = 0;
	fLiveConstBG = 0;
   fSelHist = hist;
   fFit1DimD = 0;
   fFit2DimD = 0;
   fCalHist = NULL;
   fCalFitHist = NULL;
   fCalFunc = NULL;
   fSelPad = NULL;
   fCanvas   = NULL;
   fExpHist = NULL;
   fDateText = NULL;
   fProjHistX = NULL;
	fProjHistY = NULL;
	fTofLabels = NULL;
	fMarkers = NULL;
	fDialog = NULL;
	fSerialRot = 0;
	fFirstUse = 0;
	fSetColors = kFALSE;
	fSetLevels = kFALSE;
	
   fSelInside = kTRUE;
   fDeleteCalFlag = kFALSE;

   fActiveWindows = new TList();
   fPeaks = new TList();
   fActiveFunctions = new TList();
   fActiveCuts = new TList();
   fCmdLine = new TList();
   gROOT->GetListOfCleanups()->Add(this);

   if (gHpr) {
      fAllFunctions = gHpr->GetFunctionList();
      fAllWindows = gHpr->GetWindowList();
      fAllCuts = gHpr->GetCutList();
   }
   fTimer.SetTimerID(0);
	fTimer.Connect("Timeout()", "FitHist", this, "DoSaveLimits()");

//   fMarkers = new FhMarkerList(0);
   peaks = new TObjArray(0);
   fDimension = hist->GetDimension();
//  look for cuts and windows

   CheckList(fActiveCuts);
   CheckList(fActiveWindows);
   CheckList(fAllWindows);


   TList *lof = hist->GetListOfFunctions();
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
//         cout << "$$$$$$$$$$$$   "  << p << "  $$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
//         p->Dump();
 //        cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
         if (p->InheritsFrom(TMrbWindow::Class())) {
            TMrbWindow *w = dynamic_cast<TMrbWindow*>(p);
         	if (fDimension == 2)
            	fActiveCuts->Add(w);

         	if (fDimension == 1) {
            	if (fAllWindows->FindObject(w->GetName()));
	//               cout << p->GetName() << " already existing" <<endl;
            	else
               	fAllWindows->Add(w);
            	if (!fActiveWindows->FindObject(w->GetName()))
               	fActiveWindows->Add(w);
         	}
      	}
      }
   }

   TEnv env(".hprrc");         // inspect ROOT's environment
   fFitMacroName =
       env.GetValue("HistPresent.FitMacroName", "fit_user_function.C");

//   RestoreDefaultRanges();
//   cout << "FitMacroName " << fFitMacroName.Data()<< endl;

   fTemplateMacro = "TwoGaus";
   fFill1Dim      = env.GetValue("Set1DimOptDialog.fFill1Dim",      0);
   fFillColor     = env.GetValue("Set1DimOptDialog.fFillColor", 1);
   fLineColor     = env.GetValue("Set1DimOptDialog.fLineColor", 1);
   fLineWidth     = env.GetValue("Set1DimOptDialog.fLineWidth", 1);
   fFillStyle     = env.GetValue("Set1DimOptDialog.fFillStyle", 3001);
   fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
   fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
   fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
   fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
   fDrawAxisAtTop = env.GetValue("Set1DimOptDialog.fDrawAxisAtTop", 0);
   fShowContour   = env.GetValue("Set1DimOptDialog.fShowContour", 0);
   fErrorMode     = env.GetValue("Set1DimOptDialog.fErrorMode", "none");
   fMarkerSize    = env.GetValue("Set1DimOptDialog.fMarkerSize", 1);
	fShowMarkers    = env.GetValue("Set1DimOptDialog.fShowMarkers", 0);
	gStyle->SetErrorX(env.GetValue("Set1DimOptDialog.fErrorX", 0.));
	fSmoothLine  = env.GetValue("Set1DimOptDialog.fSmoothLine",    0);
	fSimpleLine  = env.GetValue("Set1DimOptDialog.fSimpleLine",    0);
	fBarChart    = env.GetValue("Set1DimOptDialog.fBarChart",      0);
	fBarChart3D  = env.GetValue("Set1DimOptDialog.fBarChart3D",    0);
	fBarChartH   = env.GetValue("Set1DimOptDialog.fBarChartH",     0);
	fPieChart    = env.GetValue("Set1DimOptDialog.fPieChart",      0);
	fText        = env.GetValue("Set1DimOptDialog.fText",          0);
	fTextAngle   = env.GetValue("Set1DimOptDialog.fTextAngle",     0);
	fLabelsTopX  = env.GetValue("Set1DimOptDialog.fLabelsTopX",    0);
	fLabelsRightY= env.GetValue("Set1DimOptDialog.fLabelsRightY",  0);

   fDrawOpt2Dim   = env.GetValue("Set2DimOptDialog.fDrawOpt2Dim", "COLZ");
   f2DimBackgroundColor = env.GetValue("Set2DimOptDialog.f2DimBackgroundColor", 0);
	fHistFillColor2Dim = env.GetValue("Set2DimOptDialog.fHistFillColor2Dim", 1);
	fHistFillStyle2Dim = env.GetValue("Set2DimOptDialog.fHistFillStyle2Dim", 0);
	fHistLineColor2Dim = env.GetValue("Set2DimOptDialog.fHistLineColor2Dim", 1);
	fHistLineStyle2Dim = env.GetValue("Set2DimOptDialog.fHistLineStyle2Dim", 1);
	fHistLineWidth2Dim = env.GetValue("Set2DimOptDialog.fHistLineWidth2Dim", 1);
	fMarkerColor2Dim   = env.GetValue("Set2DimOptDialog.fMarkerColor2Dim",   1);
	fMarkerStyle2Dim   = env.GetValue("Set2DimOptDialog.fMarkerStyle2Dim",   1);
	fMarkerSize2Dim    = env.GetValue("Set2DimOptDialog.fMarkerSize2Dim",    1);
	
   fShowZScale        = env.GetValue("Set2DimOptDialog.fShowZScale", 1);
	if ( fShowZScale != 0 && !fDrawOpt2Dim.Contains("GL") )fDrawOpt2Dim += "Z";
   if ( fSelHist->GetDimension() == 1 ) {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat1Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox1Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox1Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay1Dim", 1);
      fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle1Dim", 1);
      fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox1Dim",1);
   } else {
		fOptStat          = env.GetValue("WhatToShowDialog.fOptStat2Dim", 1);
		fShowDateBox      = env.GetValue("WhatToShowDialog.fShowDateBox2Dim", 1);
		fShowStatBox      = env.GetValue("WhatToShowDialog.fShowStatBox2Dim", 1);
		fUseTimeOfDisplay = env.GetValue("WhatToShowDialog.fUseTimeOfDisplay2Dim", 1);
      fShowTitle        = env.GetValue("WhatToShowDialog.fShowTitle2Dim", 1);
      fShowFitBox       = env.GetValue("WhatToShowDialog.fShowFitBox2Dim",1);
   }
   fTitleCenterX  = env.GetValue("SetHistOptDialog.fTitleCenterX", 0);
   fTitleCenterY  = env.GetValue("SetHistOptDialog.fTitleCenterY", 0);
   fTitleCenterZ  = env.GetValue("SetHistOptDialog.fTitleCenterZ", 0);
	fOneDimLogX = env.GetValue("Set1DimOptDialog.fOneDimLogX", 0);
	fOneDimLogY = env.GetValue("Set1DimOptDialog.fOneDimLogY", 0);
	fTwoDimLogX = env.GetValue("Set2DimOptDialog.fTwoDimLogX", 0);
	fTwoDimLogY = env.GetValue("Set2DimOptDialog.fTwoDimLogY", 0);
	fTwoDimLogZ = env.GetValue("Set2DimOptDialog.fTwoDimLogZ", 0);
	
   fDrawOpt3Dim   = env.GetValue("Set3DimOptDialog.fDrawOpt3Dim", "");
   f3DimBackgroundColor = env.GetValue("Set3DimOptDialog.f3DimBackgroundColor", 0);
	fHistFillColor3Dim = env.GetValue("Set3DimOptDialog.fHistFillColor3Dim", 1);
	fHistLineColor3Dim = env.GetValue("Set3DimOptDialog.fHistLineColor3Dim", 1);
	fMarkerColor3Dim   = env.GetValue("Set3DimOptDialog.fMarkerColor3Dim",   1);
	fMarkerStyle3Dim   = env.GetValue("Set3DimOptDialog.fMarkerStyle3Dim",   1);
	fMarkerSize3Dim    = env.GetValue("Set3DimOptDialog.fMarkerSize3Dim",    1);
	
   fSerialPx = 0;
   fSerialPy = 0;
   fSerialPf = 0;
   fFuncNumb = 0;
   fCutNumber = 0;
	fProjectedBoth = 0;
   wdw_numb = 0;
   if (env.GetValue("HistPresent.FitOptMinos", 0) != 0) fCallMinos = kTRUE;
   else                                                 fCallMinos = kFALSE;
   if (env.GetValue("HistPresent.FitOptKeepPar", 0) != 0)fKeepParameters  = kTRUE;
   else                                                  fKeepParameters = kFALSE;
   fOldMode = -1;
   fColSuperimpose = 1;
   fMax = hist->GetMaximum();
   fMin = hist->GetMinimum();
   fXmin = hist->GetXaxis()->GetXmin();
   fXmax = hist->GetXaxis()->GetXmax();
	fYmax = fYmin = fLogz = 0;
   if (fDimension == 2) {
      fYmin = hist->GetYaxis()->GetXmin();
      fYmax = hist->GetYaxis()->GetXmax();
		fLogx = fTwoDimLogX;
		fLogy = fTwoDimLogY;
		fLogz = fTwoDimLogZ;
	}
	if (fDimension == 1) {
		fLogx = fOneDimLogX;
		fLogy = fOneDimLogY;
	}
	TObject * obj = fSelHist->GetListOfFunctions()->FindObject("palette");
   if (obj) {
      cout << " Removing obj TPaletteAxis" << endl;
      fSelHist->GetListOfFunctions()->Remove(obj);
   }
   RestoreDefaultRanges();
	DisplayHist(hist, win_topx, win_topy, win_widx, win_widy);
//	RestoreDefaultRanges();
	fLogx = fCanvas->GetLogx();
   fLogy = fCanvas->GetLogy();
   fLogz = fCanvas->GetLogz();
	fHasExtraAxis = kFALSE;
	if ( !gStyle->GetCanvasPreferGL() )
		TQObject::Connect((TPad*)fCanvas, "Modified()",
						   "FitHist", this, "HandlePadModified()");
	
};
//------------------------------------------------------
// destructor
FitHist::~FitHist()
{
	fTimer.Stop();
	DisconnectFromPadModified();
//	TQObject::Disconnect((TPad*)fCanvas, "Modified()", this, "HandlePadModified()");
	if ( gDebug > 0 ) {
		cout << " ~FitHist()this : " << this<< " fSelHist " <<fSelHist
		<< " fCanvas " <<fCanvas << endl;
//		fSelHist->Print();
	}
//   cout<< "enter FitHist  dtor "<< GetName()<<endl;
   if (!fExpHist && gHpr && GeneralAttDialog::fRememberZoom) SaveDefaults(kTRUE);
   gDirectory->GetList()->Remove(this);
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fFit1DimD ) fFit1DimD->CloseDialog();
   if ( fFit2DimD ) fFit2DimD->CloseDialog();
	if ( WindowSizeDialog::fNwindows > 0 ) 
		WindowSizeDialog::fNwindows -= 1;
   if ( fExpHist ) {
//      cout << "fExpHist " << fExpHist->GetName() << endl;
//      dont delete possible windows
      fExpHist->GetListOfFunctions()->Clear("nodelete");
      gDirectory->GetList()->Remove(fExpHist);
      delete fExpHist;
      fExpHist = 0;
   }
   if (fTofLabels) { delete fTofLabels; fTofLabels=NULL;}
   if (fCalFunc) delete fCalFunc;
//   if (fDateText) delete fDateText;
   if (!fCanvas || !fCanvas->TestBit(TObject::kNotDeleted) ||
       fCanvas->TestBit(0xf0000000)) {
      cout << "~FitHist: " << this << " Canvas : " << fCanvas << " is deleted" << endl;

      fCanvas = 0;
   }

   if (fCanvas) {
      fCanvas->SetFitHist(NULL);
      if (fCanvasIsAlive) {
//         cout << " deleting " << fCanvas->GetName() << endl;
         delete fCanvas;
         fCanvas = 0;
      }
   }
   if (fCutPanel && fCutPanel->TestBit(TObject::kNotDeleted))
      delete fCutPanel;
//   if (fMarkers)
//      ClearMarks();
//      delete fMarkers;
   if (fActiveCuts)
      delete fActiveCuts;
   if (peaks)
      delete peaks;
   if (fCmdLine)
      delete fCmdLine;
};

//_______________________________________________________________________________

void FitHist::DisconnectFromPadModified()
{
	TQObject::Disconnect((TPad*)fCanvas, "Modified()", this, "HandlePadModified()");
	if (gDebug > 0)
		cout << " DisconnectFromPadModified this : " << this << " fCanvas " <<fCanvas << endl;
}
//_______________________________________________________________________________

void FitHist::HandlePadModified()
{
//	DoSaveLimits();
	fTimer.Start(20, kTRUE);   // 2 seconds single-shot
//	TTimer::SingleShot(20, "FitHist", this, "DoSaveLimits()");
}
//_______________________________________________________________________________

void FitHist::DoSaveLimits()
{
	if ( gDebug > 0 ) {
		cout << "FitHist::DoSaveLimits: " << gPad 
				<< " fCanvas: " << fCanvas 
				<< " gPad  "  << gPad << " fSelPad->GetLogy() " <<  fSelPad->GetLogy()
				<< " fExpHist " << fExpHist<< endl;
		cout << "x1, x2, y1, y2: "<<fCanvas->GetFrame()->GetX1()<<" "
		<<fCanvas->GetFrame()->GetX2()<< " " <<fCanvas->GetFrame()->GetY1()<<" "
		<<fCanvas->GetFrame()->GetY2()<<endl;
//		fCanvas->Dump();
	}
//	gSystem->Sleep(1000);
//	if (gPad == fCanvas) {
		if ( fLogy != fSelPad->GetLogy() ){
			fLogy = fSelPad->GetLogy();
			if ( fSelHist->GetDimension() == 1 ) {
				fCanvas->GetHandleMenus()->SetLog(fLogy);
			}
//			return;
		}
		if ( fLogx != fSelPad->GetLogx() ){
			fLogx = fSelPad->GetLogx();
			if ( fSelHist->GetDimension() == 1 ) {
			}
//			return;
		}
		if ( fSelHist->GetDimension() == 2) {
			if ( fLogz != fSelPad->GetLogz() ){
				fLogz = fSelPad->GetLogz();
				fCanvas->GetHandleMenus()->SetLog(fLogz);
			}
//			return;
		}
		fXtitle = fSelHist->GetXaxis()->GetTitle();
		fYtitle = fSelHist->GetYaxis()->GetTitle();
		if ( fSelHist->GetDimension() == 3) {
			fYtitle = fSelHist->GetZaxis()->GetTitle();
		}
		if ( fExpHist == NULL ) {
			fBinlx = fSelHist->GetXaxis()->GetFirst();
			fBinux = fSelHist->GetXaxis()->GetLast();
			if ( gDebug > 0 ) {
				cout << "fBinlx,ux " <<fBinlx << " " <<fBinux << endl;
			}
			if (fDimension == 2) {
				fBinly = fSelHist->GetYaxis()->GetFirst();
				fBinuy = fSelHist->GetYaxis()->GetLast();
				if ( gDebug > 0 ) {
					cout << "fBinly,uy " <<fBinly << " " <<fBinuy << endl;
				}
			}
		}
//	}
}
//________________________________________________________________

void FitHist::RecursiveRemove(TObject * obj)
{
	if (gDebug > 0)
		cout << "FitHist:: " << this << " fSelHist " <<  fSelHist
		<< " RecursiveRemove: " << obj  << endl;
	//fSelHist->Print();
   if (fSelHist && obj != fSelHist)
      fSelHist->GetListOfFunctions()->Remove(obj);
   fActiveCuts->Remove(obj);
   fActiveWindows->Remove(obj);
   fActiveFunctions->Remove(obj);
   if (obj == fFit1DimD) fFit1DimD = NULL;
   if (obj == fFit2DimD) fFit2DimD = NULL;
}

//________________________________________________________________

void FitHist::SaveDefaults(Bool_t /*recalculate*/)
{
   if (!gHpr) return;      // not called from  Histpresenter
	if (!GeneralAttDialog::fRememberLastSet &&  !GeneralAttDialog::fRememberZoom) return;
		
//   if (!GeneralAttDialog::fRememberLastSet &&  !GeneralAttDialog::fRememberZoom) return;
//   cout << "Enter SaveDefaults " << endl;

   Bool_t checkonly = kFALSE;
   if ( (!CreateDefaultsDir(mycanvas, checkonly)) ) return;

   TEnv * env = GetDefaults(fHname, kFALSE, kTRUE); // fresh values
	if (!env) return;
	
   env->SetValue("FitMacroName", fFitMacroName);
	if (fDimension == 1) {
		if ( fLogx != fOneDimLogX ) 
			env->SetValue("LogX", fLogx);
		if ( fLogy != fOneDimLogY ) 
			env->SetValue("LogY", fLogy);
	}
	if (fDimension == 2) {
		if ( fLogx != fTwoDimLogX ) 
			env->SetValue("LogX", fLogx);
		if ( fLogy != fTwoDimLogY ) 
			env->SetValue("LogY", fLogy);
		if ( fLogz != fTwoDimLogZ ) 
			env->SetValue("LogZ", fLogz);
		env->SetValue("fBinly", fBinly);
		env->SetValue("fBinuy", fBinuy);
	}
   env->SetValue("fBinlx", fBinlx);
   env->SetValue("fBinux", fBinux);
	if (fSetRange) {
		env->SetValue("fRangeLowX", fRangeLowX);
		env->SetValue("fRangeUpX",  fRangeUpX);
		if (fDimension == 2) {
			env->SetValue("fRangeLowY", fRangeLowY);
			env->SetValue("fRangeUpY",  fRangeUpY);
		}
	}
   if (fSelHist->TestBit(TObject::kNotDeleted)) {
      if (fXtitle.Length() > 0)
         env->SetValue("fXtitle", fXtitle);
		if (fYtitle.Length() > 0)
			env->SetValue("fYtitle", fYtitle);
		if (fZtitle.Length() > 0)
			env->SetValue("fZtitle", fZtitle);
   }
	if (gDebug > 0)
		cout << "env->SaveLevel(kEnvLocal): " << fBinlx << " " << fBinux << fLogy << endl;
   env->SaveLevel(kEnvLocal);
   delete env;
   return;
}
//________________________________________________________________

void FitHist::RestoreDefaultRanges()
{
   TEnv * lastset = GetDefaults(fHname);
	if (!lastset) return;
   fFitMacroName = lastset->GetValue("FitMacroName",fFitMacroName.Data());
   if (gHpr && (GeneralAttDialog::fRememberLastSet || GeneralAttDialog::fRememberZoom)) {
		fLogx = lastset->GetValue("LogX", fOneDimLogX);
		fLogy = lastset->GetValue("LogY", fOneDimLogY);
		fRangeLowX = fSelHist->GetXaxis()->GetXmin();
      fRangeUpX = fSelHist->GetXaxis()->GetXmax();
      if (lastset->Lookup("fRangeLowX")) {
         fRangeLowX = lastset->GetValue("fRangeLowX", fRangeLowX);
         fRangeUpX = lastset->GetValue("fRangeUpX", fRangeUpX);
         fSelHist->GetXaxis()->Set(fSelHist->GetNbinsX(), fRangeLowX, fRangeUpX);
         fSetRange = kTRUE;
//         cout << "fRangeLowX " << fRangeLowX << endl;
      }
      fBinlx = fSelHist->GetXaxis()->GetFirst();
      fBinux = fSelHist->GetXaxis()->GetLast();
      if (lastset->Lookup("fBinlx")) {
         fBinlx = lastset->GetValue("fBinlx", fBinlx);
         fBinux = lastset->GetValue("fBinux", fBinux);
         fSelHist->GetXaxis()->SetRange(fBinlx, fBinux);
      }

      if (lastset->Lookup("fXtitle"))
         fSelHist->GetXaxis()->SetTitle(lastset->GetValue("fXtitle", ""));
      if (lastset->Lookup("fYtitle"))
         fSelHist->GetYaxis()->SetTitle(lastset->GetValue("fYtitle", ""));

      if (is2dim(fSelHist)) {
			fLogx = lastset->GetValue("LogX", fTwoDimLogX);
			fLogy = lastset->GetValue("LogY", fTwoDimLogY);
			fLogz = lastset->GetValue("LogZ", fTwoDimLogZ);
			fRangeLowY = fSelHist->GetYaxis()->GetXmin();
         fRangeUpY = fSelHist->GetYaxis()->GetXmax();
         if (lastset->Lookup("fRangeLowY")) {
            fRangeLowY = lastset->GetValue("fRangeLowY", fRangeLowY);
            fRangeUpY = lastset->GetValue("fRangeUpY", fRangeUpY);
            fSelHist->GetYaxis()->Set(fSelHist->GetNbinsY(), fRangeLowY,
                                  fRangeUpY);
            fSetRange = kTRUE;
         }
         fBinly = fSelHist->GetYaxis()->GetFirst();
         fBinuy = fSelHist->GetYaxis()->GetLast();
         if (lastset->Lookup("fBinly")) {
            fBinly = lastset->GetValue("fBinly", fBinly);
            fBinuy = lastset->GetValue("fBinuy", fBinuy);
            fSelHist->GetYaxis()->SetRange(fBinly, fBinuy);
         }
         if (lastset->Lookup("Contours")) {
            fUserContourLevels = lastset->GetValue("Contours", 0);
            if (fUserContourLevels > 0) {
               TString temp;
         		Double_t * uc = new Double_t[fUserContourLevels];
          		TMrbNamedArrayI * colors = new TMrbNamedArrayI("Pixel", fHname.Data());
               colors->Set(fUserContourLevels);
               Double_t all_levels = 0;
         		for (Int_t i = 0; i < fUserContourLevels; i++) {
                  temp = "Cont";
                  temp += i;
                  uc[i] = lastset->GetValue(temp.Data(), (Double_t)0);
                  all_levels += uc[i];
                  temp = "Pix";
                  temp += i;
                  (*colors)[i] = lastset->GetValue(temp.Data(), (Int_t)0);
         		}
   				if (all_levels != 0) fSelHist->SetContour(fUserContourLevels, uc);

//   				Int_t allcolors = 0;
//   				for (Int_t i=0; i< fUserContourLevels; i++) allcolors += (*colors)[i];
					if (colors->GetSum() != 0) {
				 //  set new Palette
     			   	fSelHist->GetListOfFunctions()->Add(colors);
//                  colors->Dump();
      				SetUserPalette(1001, colors);
					}
         		if (uc) delete [] uc;
            }
         }
      }
   }
   if (lastset)
      delete lastset;
//	cout << "RestoreDefaultRanges() fLogz " << fLogz<< endl;
}
//______________________________________________________________________________________

void FitHist::handle_mouse()
{
   Double_t fChi2Limit = 10;
	TString likelyhood = "";
	TString fitopt;
   static TF1 * gFitGauss = NULL;
   static TF1 * gFitBG = NULL;
	static Bool_t gFitGaussNotYetDrawn = kTRUE;
	static Bool_t gFitBGNotYetDrawn = kTRUE;
   Double_t cont, sum, mean;
   static Double_t gconst, center, sigma, bwidth,  esigma;
   static Double_t startbinX_lowedge, startbinY_lowedge;
   static Double_t chi2 = 0;
   static Int_t startbinX, startbinY;
   static Double_t ratio1, ratio2;
   static Int_t px1old, py1old, px2old, py2old;

   static Bool_t is2dim = kFALSE;
   static Bool_t first_time = kTRUE;
   static Bool_t skip_after_TCUTG = kFALSE;
	static Bool_t TCUTG_moved = kFALSE;
   static TH1 * hist = 0;
   static Int_t npar = 0;
   static TLine * lowedge = 0;
   static TLine * upedge = 0;
   static TBox * box = 0;
   static Int_t nrows = 4;
	static Double_t sqrt2pi = TMath::Sqrt(2 * TMath::Pi());
	Int_t px, py;
	
   if (gROOT->GetEditorMode() != 0) return;
	if (gStyle->GetCanvasPreferGL()) return;
   Int_t event = gPad->GetEvent();
//	cout << "px: "  << (char)gPad->GetEventX() << endl;
   if (event ==  kKeyPress) {
//    cout << "px: "  << (char)gPad->GetEventX() << endl;
      char ch = (char)gPad->GetEventX();
      if ( ch == 'C' ||ch == 'c') {
         TRootCanvas *rc =  (TRootCanvas*)fCanvas->GetCanvasImp();
//        if (fCanvas->GetAutoExec()) fCanvas->ToggleAutoExec();
//         gSystem->Sleep(200);
         rc->ShowEditor(kFALSE);
         rc->SendCloseMessage();
         return;
      }
   }
   if ( TCUTG_moved && event == kButton1Up ) {
		if ( gDebug > 0 )
			cout << " ProjectBoth " << endl;
		ProjectBoth();
		TCUTG_moved = kFALSE;
	}
   TObject *select = gPad->GetSelected();
   if (!select) return;
//	cout << select->ClassName() << " event " << event << endl;
   if (event == kButton1Up && skip_after_TCUTG) {
      skip_after_TCUTG = kFALSE;
      return;
   }
   if (event == kButton1Down) {
      if (gROOT->FindObject("CUTG")) {
         cout << "handle_mouse(): UpdateCut();" << endl;
         UpdateCut();
         skip_after_TCUTG = kTRUE;
         return;
      }
   }
   if (select->InheritsFrom("TPave")) return;
	if (event == 21 && select->InheritsFrom("TCutG") && fProjectedBoth == 1) {
		TCUTG_moved = kTRUE;
		if ( gDebug > 0 )
			cout << select->ClassName() << " event " << event << endl;
		return;
	}
			
   px = gPad->GetEventX();
   py = gPad->GetEventY();
   if (event == kButton1Down) {
//      cout << "handle_mouse select " << select->ClassName() << endl;
      if (select->InheritsFrom("TF1")) {
         TF1 *f = (TF1*)select;
         cout  << endl << "------  Function " << f->GetName() << " --------" << endl;
         for (Int_t i = 0; i < f->GetNpar(); i++) {
            cout << f->GetParName(i) << "  " << f->GetParameter(i) << endl;
         }
         cout  << endl << "----------------------------" << endl;
         TH1 * h = Hpr::FindHistOfTF1(gPad, f->GetName(), 1);
         if (h) cout << "popped " << f << endl;
         return;
      }
   }
//   if ( (!fLiveStat1Dim && fDimension == 1) || (!fLiveStat2Dim && fDimension == 2) ||
//         fDimension == 3 )  return;
   if ( fDimension == 3 )  return;
   TEnv env(".hprrc");
   if ( fDimension == 1) { 
      fLiveStat1Dim = (Int_t)env.GetValue("Set1DimOptDialog.fLiveStat1Dim",0);
      if (fLiveStat1Dim == 0)
			return;
	}
   if ( fDimension == 2) { 
      fLiveStat2Dim = (Int_t)env.GetValue("Set2DimOptDialog.fLiveStat2Dim",0);
		if (fLiveStat2Dim == 0)
			return;
	}
   
   if (event == kButton1Down) {
      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(fCanvas, px, py))) {
         TList * lofp = gPad->GetListOfPrimitives();
         if (!lofp) return;
         TIter next(lofp);
         TObject * obj = 0;
         while ( (obj = next()) ) {
            if (obj->InheritsFrom("TH3")) return;
            if (obj->InheritsFrom("TH1")) hist = (TH1*)obj;
         }
         if (!hist) {
            cout << "No histogram found" << endl;
            return;
         }
         TString sopt = hist->GetOption();
//         cout << " sopt " << sopt << endl;
         if (sopt.Contains("SURF", TString::kIgnoreCase) ||
             sopt.Contains("LEGO", TString::kIgnoreCase)) {
             return;
         }

         if (hist->GetDimension() == 3) {
//            cout << "3d histogram not supported" << endl;
            return;
         } else if (hist->GetDimension() == 2){
            is2dim = kTRUE;
         } else {
            is2dim = kFALSE;
         }

         if (lowedge) {
            delete lowedge; lowedge = 0;
            delete upedge; upedge = 0;
         }

         if (box) {delete box; box = 0;};
			if (gFitGauss) { delete gFitGauss; gFitGauss = 0;}
			if (gFitBG)   { delete gFitBG; gFitBG = 0;}
			first_time = kTRUE;
         Axis_t xx = gPad->AbsPixeltoX(px);
         startbinX = hist->GetXaxis()->FindBin(xx);
         startbinX_lowedge = hist->GetXaxis()->GetBinLowEdge(startbinX);

         if (is2dim) {
            Axis_t yy = gPad->AbsPixeltoY(py);
            startbinY = hist->GetYaxis()->FindBin(yy);
            startbinY_lowedge = hist->GetYaxis()->GetBinLowEdge(startbinY);
            cont = hist->GetBinContent(startbinX, startbinY);
            nrows = 4;
         } else {
            bwidth = hist->GetBinWidth(startbinX);
            esigma = hist->GetBinWidth(startbinX) * TMath::Sqrt(12.);
            cont = hist->GetBinContent(startbinX);
            gconst = cont * bwidth / sqrt2pi * esigma;
            center = hist->GetBinCenter(startbinX);
				fLiveGauss     = env.GetValue("Set1DimOptDialog.fLiveGauss", 0);
				fLiveBG        = env.GetValue("Set1DimOptDialog.fLiveBG", 0);
				fLiveConstBG   = env.GetValue("Set1DimOptDialog.fLiveConstBG", 0);

            if (fLiveGauss) {
               if (fLiveBG) {
                  gFitGauss = new TF1("fitfunc", "pol1+gaus(2)");
                  gFitBG = new TF1("bgfunc", "pol1");
                  npar = 5;
                  if (nrows != 6) {delete fTofLabels; fTofLabels = 0;}
                  nrows = 6;
               } else if (fLiveConstBG) {
                  gFitGauss = new TF1("fitfunc", "pol0+gaus(1)");
						gFitGauss->SetParLimits(0, hist->GetMinimum(), hist->GetMaximum());
                  gFitBG = new TF1("bgfunc", "pol0");
                 npar = 5;
                  if (nrows != 6) {delete fTofLabels; fTofLabels = 0;}
                  nrows = 6;
               } else {
                  gFitGauss = new TF1("fitfunc", "gaus");
 //                 gFitGauss ->SetParameters(gconst, center, esigma);
                 npar = 3;
                 if (nrows != 5) {delete fTofLabels; fTofLabels = 0;}
                 nrows = 5;
               }
					gFitGaussNotYetDrawn = kTRUE;
					gFitBGNotYetDrawn = kTRUE;
					gFitGauss->SetLineColor(kRed);
					gFitGauss->SetLineWidth(3);
					if ( gFitBG ) {
						gFitBG->SetLineColor(kCyan);
						gFitBG->SetLineWidth(2);
					}
            } else {
               if (nrows != 4) {delete fTofLabels; fTofLabels = 0;}
               nrows = 4;
            }
         }
//         if ( kTRUE ) {
/*         if ( !is2dim )
            fLiveStat1Dim  = env.GetValue("Set1DimOptDialog.fLiveStat1Dim", 0);
         if ( is2dim )
            fLiveStat2Dim  = env.GetValue("Set2DimOptDialog.fLiveStat2Dim", 0);
*/
         if ( (fLiveStat1Dim && !is2dim ) || (fLiveStat2Dim && is2dim) ) {
         	if (!fTofLabels) {
            	TOrdCollection rowlabels;
            	if (is2dim) {
               	rowlabels.Add(new TObjString("Low left Bin: Number (x,y), Low edge (x,y)"));
               	rowlabels.Add(new TObjString("Up right Bin: Number (x,y), Up edge (x,y)"));
            	} else {
               	rowlabels.Add(new TObjString("Low left Bin: Number, Low edge"));
               	rowlabels.Add(new TObjString("Up right Bin: Number, Up edge"));
            	}
            	rowlabels.Add(new TObjString("Current Bin: Content "));
               if (is2dim)
            	   rowlabels.Add(new TObjString("Integral, Mean Z"));
               else
            	   rowlabels.Add(new TObjString("Integral, Mean"));
            	TOrdCollection values;
            	if (is2dim) {
               	values.Add(new TObjString(Form("%d,  %d,  %lg,  %lg",
                        	 startbinX, startbinY, startbinX_lowedge , startbinY_lowedge)));
               	values.Add(new TObjString(Form("%d,  %d,  %lg,  %lg",
                        	 startbinX, startbinY,
                        	 startbinX_lowedge + hist->GetBinWidth(startbinX),
                        	 startbinY_lowedge + hist->GetYaxis()->GetBinWidth(startbinY))));
               	values.Add(new TObjString(Form("%lg", cont)));
               	values.Add(new TObjString(Form("%lg", cont)));

            	} else {
                  TString timestring;
                  if (hist->GetXaxis()->GetTimeDisplay()) {
                     ConvertTimeToString((time_t)startbinX_lowedge, hist->GetXaxis(), &timestring);
                     timestring.Prepend(Form("%d, ", startbinX));
                     values.Add(new TObjString(timestring.Data()));
                     ConvertTimeToString((time_t)(startbinX_lowedge + hist->GetBinWidth(startbinX)),
                                                  hist->GetXaxis(), &timestring);
                     timestring.Prepend(Form("%d, ", startbinX));
                     values.Add(new TObjString(timestring.Data()));

                  } else {
               	   values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge )));
               	   values.Add(new TObjString(Form("%d,  %lg", startbinX, startbinX_lowedge
                     	  + hist->GetBinWidth(startbinX))));
                  }
               	values.Add(new TObjString(Form("%lg", cont)));
               	values.Add(new TObjString(Form("%lg", cont)));
               	 if (fLiveGauss) {
                  	 if (fLiveBG) {
                     	 rowlabels.Add(new TObjString("Backgd: content, const, slope"));
                     	 values.Add(new TObjString("no fit done yet"));
                  	 }  else if (fLiveConstBG) {
                     	 rowlabels.Add(new TObjString("Backgd: content, const"));
                     	 values.Add(new TObjString("no fit done yet"));
                  	 }

                  	 rowlabels.Add(new TObjString("Gaus: Cont, Mean, Sig, Xi2"));
                  	 values.Add(new TObjString("no fit done yet"));
               	}
            	}
            	TString title("Stat. in sel. region of: ");
            	title += fHname.Data();
            	Int_t xw = gPad->GetCanvas()->GetWindowTopX();
            	Int_t yw = gPad->GetCanvas()->GetWindowTopY() + gPad->GetCanvas()->GetWindowHeight() + 28;
            	fTofLabels = new TableOfLabels(0, &title, 1, nrows, &values,
                        	  0, &rowlabels, xw, yw);
            	fTofLabels->Connect("Destroyed()", "FitHist", this, "ClearTofl()");
	//            pressed = kTRUE;
         	} else {
            	fTofLabels->SetLabelText(0,0,Form("%d", startbinX));
            	fTofLabels->SetLabelText(0,1,Form("%d", startbinX));
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg", cont));
         	}
         }
         ratio1 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
         px1old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio1*(gPad->GetUxmax() - gPad->GetUxmin()));
         if (is2dim) {
            px2old = gPad->XtoAbsPixel(gPad->GetUxmax());
            Double_t yru = gPad->GetUymax() - gPad->GetUymin();
            ratio1 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru ;
            Double_t yu = gPad->GetUymin() + ratio1 * yru;
            py1old = gPad->YtoAbsPixel(yu);
            py2old = gPad->YtoAbsPixel(gPad->GetUymax());
         } else {
            px2old = px1old;
            py1old = gPad->YtoAbsPixel(gPad->GetUymin());
            py2old = gPad->YtoAbsPixel(gPad->GetUymax());
         }
         gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
         gVirtualX->SetLineColor(-1);
      }
   }  else if (event == kButton1Motion || event == kButton1Up) {
//      if(!hist || !fTofLabels) return;
      if(!hist) return;
      if (select->InheritsFrom("TH1")) {
         TString sopt = hist->GetOption();
         if (sopt.Contains("SURF", TString::kIgnoreCase) ||
             sopt.Contains("LEGO", TString::kIgnoreCase)) {
             return;
         }
      }

      if(select->IsA() == TFrame::Class() || select->InheritsFrom("TH1")
         ||  (select->InheritsFrom("TCanvas") && IsInsideFrame(fCanvas, px, py))){
         if (select->InheritsFrom("TH3")) return;
         px = gPad->GetEventX();
         Axis_t xx = gPad->AbsPixeltoX(px);
         Axis_t yy = gPad->AbsPixeltoY(py);
         Int_t binX = hist->GetXaxis()->FindBin(xx);
         Int_t binY;
         Int_t binXlow, binXup, binYlow, binYup;
         Axis_t XlowEdge, XupEdge, YlowEdge = 0, YupEdge = 0;
         binXlow = TMath::Min(startbinX, binX);
         binXup  = TMath::Max(startbinX, binX);
         XlowEdge = hist->GetXaxis()->GetBinLowEdge(binXlow);
         XupEdge  = hist->GetXaxis()->GetBinLowEdge(binXup) + hist->GetXaxis()->GetBinWidth(binXup);

         if (is2dim) {
            binY = hist->GetYaxis()->FindBin(yy);
            binYlow = TMath::Min(startbinY, binY);
            binYup  = TMath::Max(startbinY, binY);
            YlowEdge = hist->GetYaxis()->GetBinLowEdge(binYlow);
            YupEdge  = hist->GetYaxis()->GetBinLowEdge(binYup) + hist->GetYaxis()->GetBinWidth(binYup);
            if (fTofLabels) {
//            if (fLiveStat2Dim && fTofLabels) {
             	cont = hist->GetBinContent(binX, binY);
            	sum = ((TH2*)hist)->Integral(binXlow, binXup, binYlow, binYup);
            	Int_t totbins = (binYup - binYlow +1) * (binXup - binXlow +1);
            	if (totbins > 0) mean = sum / (Double_t)totbins;
            	else             mean = 0;

            	fTofLabels->SetLabelText(0,0,Form("%d,  %d,  %lg,  %lg", binXlow, binYlow, XlowEdge, XupEdge));
            	fTofLabels->SetLabelText(0,1,Form("%d,  %d,   %lg,  %lg", binXup,  binYup,  YlowEdge, YupEdge));
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg,  %lg", sum, mean));
            }
         } else {
				if (fTofLabels) {
//              if (fLiveStat1Dim && fTofLabels) {
					Double_t sumx = 0;
            	cont = hist->GetBinContent(binX);
            	sum = hist->Integral(binXlow, binXup);
//           	Int_t totbins = (binXup - binXlow +1);
               for (Int_t ibin = binXlow; ibin <= binXup; ibin++)
                  sumx = sumx + hist->GetBinContent(ibin) * hist->GetBinCenter(ibin);
            	if (sum > 0) mean = sumx / sum;
            	else             mean = 0;
               TString timestring;
               if (hist->GetXaxis()->GetTimeDisplay()) {
                  ConvertTimeToString((time_t)XlowEdge, hist->GetXaxis(), &timestring);
                  timestring.Prepend(Form("%d, ", binXlow));
            	   fTofLabels->SetLabelText(0, 0, timestring.Data());
                  ConvertTimeToString((time_t)XupEdge, hist->GetXaxis(), &timestring);
                  timestring.Prepend(Form("%d, ", binXup));
            	   fTofLabels->SetLabelText(0, 1, timestring.Data());
               } else {
            	   fTofLabels->SetLabelText(0,0,Form("%d,  %lg", binXlow, XlowEdge));
               	fTofLabels->SetLabelText(0,1,Form("%d,  %lg", binXup, XupEdge ));
               }
            	fTofLabels->SetLabelText(0,2,Form("%lg", cont));
            	fTofLabels->SetLabelText(0,3,Form("%lg,  %lg", sum, mean));
					// set initial parameters
            	if (fLiveGauss &&  (binXup - binXlow - npar) > 0) {
               	Int_t ndf = binXup - binXlow - npar;
               	if (!first_time) chi2 = gFitGauss->GetChisquare();
						
						Int_t paroff = 0;
						if ( fLiveBG )
							paroff = 2;
						else if ( fLiveConstBG )
							paroff = 1;
						if (first_time                    ||
						gFitGauss->GetParameter(0 + paroff) < 0 ||
						gFitGauss->GetParameter(2 + paroff) < 0 ||
						chi2 / ndf > fChi2Limit) {
//                     	 cout << " old par at bin " << binX << ": ";
//                     	 for (Int_t i = 0; i < gFitGauss->GetNpar(); i++)
//                        	cout  << gFitGauss->GetParameter(i) << " ";
//                     	 cout << chi2 << " " << ndf << endl;
							if ( paroff > 0 ) gFitGauss->SetParameter(0, cont);
							if ( paroff > 1 ) gFitGauss->SetParameter(1, 0);
							gFitGauss->SetParameter(0 + paroff, sum * bwidth);
							gFitGauss->SetParameter(1 + paroff, hist->GetMean() );
							gFitGauss->SetParameter(2 + paroff, hist->GetRMS() );
/*                    	 cout << " new par: bg cont " << cont << " gaus cont "  << sum * bwidth << " mean "
                        	  << hist->GetMean()<< "  sig "
                        	  << hist->GetRMS() << endl;*/
						}
						fitopt = "RNQ";
						fitopt += likelyhood;
               	hist->Fit(gFitGauss, fitopt, "",XlowEdge ,XupEdge );
               	first_time = kFALSE;
                  gFitGauss->SetRange(XlowEdge ,XupEdge);
						if ( gFitGaussNotYetDrawn ) {
							gFitGauss->Draw("same");
							gFitGaussNotYetDrawn = kFALSE;
						}
						Double_t bcont = 0;
						if (fLiveBG || fLiveConstBG) {
							gFitBG->SetParameter(0, gFitGauss->GetParameter(0));
							if ( fLiveBG )
								gFitBG->SetParameter(1, gFitGauss->GetParameter(1));
							gFitBG->SetRange(XlowEdge ,XupEdge);
							if ( gFitBGNotYetDrawn ) {
								gFitBG->Draw("same");
								gFitBGNotYetDrawn = kFALSE;
							}
						}
               	gPad->Modified();
               	gPad->Update();
               	gPad->GetCanvas()->GetFrame()->SetBit(TBox::kCannotMove);
						chi2 = gFitGauss->GetChisquare() / (Double_t)TMath::Max(gFitGauss->GetNDF(),1);

						if ( fLiveBG ) {
							Double_t a = gFitGauss->GetParameter(0);
							Double_t b = gFitGauss->GetParameter(1);
							bcont = (a + 0.5 * b * (XupEdge+XlowEdge))*(XupEdge-XlowEdge) / bwidth;
							fTofLabels->SetLabelText(0,4,Form("%lg,  %lg,  %lg", bcont, a, b));
						}		
						else if (fLiveConstBG) {
							Double_t a = gFitGauss->GetParameter(0);
							bcont = a * (XupEdge-XlowEdge) / bwidth;
							fTofLabels->SetLabelText(0,4,Form("%lg,  %lg", bcont, a));
						}		
                  sigma = gFitGauss->GetParameter(paroff + 2);
                  gconst = gFitGauss->GetParameter(paroff + 0) * sqrt2pi * sigma / bwidth;
                  center = gFitGauss->GetParameter(paroff + 1);
						Int_t laboff = 0;
						if (paroff > 0) laboff =1;
                  fTofLabels ->SetLabelText(0,laboff + 4, Form("%lg,  %lg,  %lg,  %lg",
                              	 gconst, center, sigma, chi2));
            	}
            }
         }
         if ( event == kButton1Motion) {
      	   gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
         	ratio2 = (gPad->AbsPixeltoX(px) - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
      	   px2old = gPad->XtoAbsPixel(gPad->GetUxmin()+ratio2*(gPad->GetUxmax() - gPad->GetUxmin()));
            if (is2dim) {
               Double_t yru = gPad->GetUymax() - gPad->GetUymin();
         	   ratio2 = (gPad->AbsPixeltoY(py) - gPad->GetUymin()) / yru;
      	      py2old = gPad->YtoAbsPixel(gPad->GetUymin() + ratio2 * yru );
            }
      	   gVirtualX->DrawBox(px1old, py1old, px2old, py2old, TVirtualX::kHollow);
      	   gVirtualX->SetLineColor(-1);
         } else {
            ClearMarks();
            fMarkers = new FhMarkerList();
            fSelHist->GetListOfFunctions()->Add(fMarkers);
            if (is2dim) {
               box = new TBox(XlowEdge, YlowEdge, XupEdge, YupEdge);
               box->SetLineColor(-1);
               box->SetFillStyle(0);
               box->Draw();
               fMarkers->Add(new FhMarker(XlowEdge, YlowEdge, 28));
               fMarkers->Add(new FhMarker(XupEdge,  YupEdge, 28));
               PaintMarks();
            } else {
            	lowedge = new TLine(XlowEdge, hist->GetYaxis()->GetXmin(),
                                   XlowEdge, hist->GetYaxis()->GetXmax());
         	   upedge  = new TLine(XupEdge, hist->GetYaxis()->GetXmin(),
                                   XupEdge, hist->GetYaxis()->GetXmax());
         	   lowedge->SetLineColor(4); lowedge->Draw();
         	   upedge->SetLineColor(4); upedge->Draw();
               fMarkers->Add(new FhMarker(XlowEdge, yy, 28));
               fMarkers->Add(new FhMarker(XupEdge,  yy, 28));
               PaintMarks();
            }
         }
         if (fTofLabels)fTofLabels->RaiseWindow();
      }
   }
}
//_____________________________________________________________________________________

void FitHist::DrawTopAxis() {
 //  cout << "  add axis (channels) on top" << endl;
   TGaxis *naxis;
   Axis_t x1, y1, x2, y2;
//      TString side("-");
   Float_t loff;
   Float_t lsize;
   Int_t ndiv;

   x1 = fCanvas->GetUxmin();
   x2 = fCanvas->GetUxmax();
	if ( fCanvas->GetLogx() ) {
		x1 = TMath::Power(10, x1);
		x2 = TMath::Power(10, x2);
	}
   y1 = fCanvas->GetUymax();
   y2 = y1;
   loff = 0.0;
   ndiv = fSelHist->GetXaxis()->GetNdivisions();
   lsize = fSelHist->GetXaxis()->GetLabelSize();
   fCanvas->cd();
   Axis_t blow = (Axis_t)(fSelHist->GetXaxis()->GetFirst() - 1);
   Axis_t bup  = (Axis_t)(fSelHist->GetXaxis()->GetLast());
   naxis = new TGaxis(x1, y1, x2, y2,blow, bup, ndiv, "-W");
   naxis->SetGridLength(-0.8);
   naxis->SetLabelOffset(loff);
   naxis->SetLabelSize(lsize);
   naxis->Paint();
}
//_____________________________________________________________________________________
// Show histograms

void FitHist::DisplayHist(TH1 * hist, Int_t win_topx, Int_t win_topy,
                          Int_t win_widx, Int_t win_widy)
{
   if (hist != NULL) {
      fSelHist = hist;
   } else {
      cout << "Null pointer in DisplayHist" << endl;
      return;
   }
   fOrigHist = hist;

//  construct name of canvas and pad
   fCname = "C_";
   fCname += GetName();
   fEname = "E_";
   fEname += GetName();
   fCtitle = fSelHist->GetName();
   fCtitle += ": ";
   fCtitle += fSelHist->GetTitle();
	if ( fSelHist->GetDimension() == 2 ) {
		if (fDrawOpt2Dim.BeginsWith("GL")) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}
	}
	if ( fSelHist->GetDimension() == 3 ) {
		if (fDrawOpt3Dim.BeginsWith("GL")) {
			gStyle->SetCanvasPreferGL(kTRUE);
		} else {
			gStyle->SetCanvasPreferGL(kFALSE);
		}
	}
   fCanvas = new HTCanvas(fCname.Data(), fCtitle.Data(),
                        win_topx, win_topy, win_widx, win_widy, gHpr, this);
   fCanvasIsAlive = kTRUE;

   fCanvas->SetEditable(kTRUE);

   hist->SetDirectory(gROOT);
//   if ( (is2dim(hist) && fLiveStat2Dim) || (!is2dim(hist) && fLiveStat1Dim) ){
	if (gStyle->GetCanvasPreferGL() == kFALSE) {
		TString cmd("((FitHist*)gROOT->FindObject(\"");
      cmd += GetName();
      cmd += "\"))->handle_mouse()";
//      cout << "FitHist::DisplayHist cmd: " << cmd << endl;
      fCanvas->AddExec("handle_mouse", cmd.Data());
		fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
	}
   gDirectory = gROOT;
   fSelPad = fCanvas;
   fSelPad->cd();
   fCanvas->ToggleEventStatus();
	
   if (is3dim(hist)) {
      Draw3Dim();
   } else if (is2dim(hist)) {
		fCanvas->SetLogx(fLogx);
		fCanvas->SetLogy(fLogy);
		fCanvas->SetLogz(fLogz);
		Draw2Dim();
   } else {
		fCanvas->SetLogx(fLogx);
		fCanvas->SetLogy(fLogy);
		if (gDebug > 0) {
			cout<< "DisplayHist:: fCanvas->GetLogy(): " ;
			if (fCanvas->GetLogy()) 
				cout << "true";
			else
				cout << "false";
			cout << " StatY " << gStyle->GetStatY() << " StatH " << gStyle->GetStatH();
			cout << endl;
		}
		fCanvas->GetHandleMenus()->SetLog(fLogy);
      Draw1Dim();
   }
	fFrameX1 = fCanvas->GetFrame()->GetX1();
	fFrameX2 = fCanvas->GetFrame()->GetX2();
	fFrameY1 = fCanvas->GetFrame()->GetY1();
	fFrameY2 = fCanvas->GetFrame()->GetY2();
};

//------------------------------------------------------
// Magnify

void FitHist::Magnify()
{
   if (is2dim(fSelHist)) {
      Hpr::WarnBox("Magnify 2 dim not yet supported ");
      return;
   }
   Int_t newx = (Int_t) (2.1 * fSelHist->GetNbinsX());
   Int_t wx = fCanvas->GetWw();
   if (newx <= wx) {
      cout << "Screen resolution already good enough" << endl;
      return;
   } else {
      Float_t fac = (Float_t) newx / (Float_t) wx;
      TAxis *xaxis = fSelHist->GetXaxis();
      TAxis *yaxis = fSelHist->GetYaxis();

      xaxis->SetNdivisions(2080);
      yaxis->SetTickLength(0.03 / fac);
      fCanvas->SetLeftMargin(0.1 / fac);
      fCanvas->SetRightMargin(0.1 / fac);
      TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
      if (st) {
        st->SetX1NDC(1 - 0.4 / fac);
        st->SetX2NDC(0.99);
      }
//      gStyle->SetStatW(0.19 / fac);
//      gStyle->SetTitleXSize(0.02 / fac);
//      gStyle->SetLabelOffset(0.001 / fac, "y");

   }
   Int_t newy = (Int_t) (0.95 * fCanvas->GetWh());
//   fCanvas->SetWindowSize(wx, fCanvas->GetWh());
//   cout << "newx, newy " << newx << " " << newy<< endl;
   fCanvas->SetCanvasSize(newx, newy);
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
   fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
}
//_______________________________________________________________________________________

void FitHist::Entire()
{
//   fSelHist->GetListOfFunctions()->Print();
//   fOrigHist->GetListOfFunctions()->Print();

   if (fExpHist) {
      cout << " Entire() fExpHist->Delete()" <<endl;
      fExpHist->GetListOfFunctions()->Clear("nodelete");
      TF1 *ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("backf");
      if (ff) delete ff;
           ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("tailf");
      if (ff) delete ff;
           ff= (TF1*)gROOT->GetListOfFunctions()->FindObject("gausf");
      if (ff) delete ff;
		if (fExpHist == fSelHist )
		   fSelHist = NULL;
      fExpHist->Delete();
      fExpHist = NULL;
   }
   fSelHist = fOrigHist;
   fSelHist->SetMinimum(-1111);
   fSelHist->SetMaximum(-1111);
   fSelHist->GetXaxis()->SetRange(1, fSelHist->GetNbinsX());
   fBinlx = 1;
   fBinux =  fSelHist->GetNbinsX();
   if (is2dim(fSelHist)) {
      fBinly = 1;
      fBinuy =  fSelHist->GetNbinsY();
   }
   SaveDefaults(kTRUE);

   ClearMarks();
   fSelPad->cd();
   if (is2dim(fSelHist)) {
      fSelHist->GetYaxis()->SetRange(1, fSelHist->GetNbinsY());
      Draw2Dim();
   } else if (is3dim(fSelHist)){
      Draw3Dim();
   } else {
      Draw1Dim();
      TIter next(fSelHist->GetListOfFunctions());
      while (TObject *o = next()) {
         if (o->InheritsFrom("TF1")) {
           ((TF1*)o)->Draw("same");
         }
      }
   }
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
   fCanvas->GetFrame()->SetBit(TBox::kCannotMove);
};
//_______________________________________________________________________________________

void FitHist::RebinOne()
{
   if (fSelHist == NULL) return;

   TString title(fSelHist->GetTitle());
   title += "_rebin";
   TString name(fSelHist->GetName());
   name += "_rebin";

   static Int_t ngroupX = 2;
   static Int_t ngroupY = 2;
   Bool_t ok;
   TH1 *newhist = NULL;
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("StringValue_Name of rebinned hist"));
   row_lab->Add(new TObjString("StringValue_Title of rebinned hist"));
   row_lab->Add(new TObjString("PlainIntVal_Rebin value X"));
   if (is2dim(fSelHist))
      row_lab->Add(new TObjString("PlainIntVal_Rebin value Y"));

   valp[ind++] = &name;
   valp[ind++] = &title;
   valp[ind++] = &ngroupX;
   if (is2dim(fSelHist))
      valp[ind++] = &ngroupY;

   Int_t itemwidth = 400;
   ok = GetStringExt("Rebin parameters", NULL, itemwidth, mycanvas,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   title += ngroupX;
   name  += ngroupX;
   if (is2dim(fSelHist)) {
//      cout << "nyi" << endl;
      title += "_";
      name  += "_";
      title += ngroupY;
      name  += ngroupY;

      Int_t nbinsXnew = fSelHist->GetNbinsX() / ngroupX;
      Int_t nbinsYnew = fSelHist->GetNbinsY() / ngroupY;
      if (nbinsXnew * ngroupX != fSelHist->GetNbinsX()) {
         nbinsXnew += 1;
         TString warn("Warning: Number of bins X not multiple of ");
         warn += ngroupX;
         Hpr::WarnBox(warn.Data());
      }
      if (nbinsYnew * ngroupY != fSelHist->GetNbinsY()) {
         nbinsYnew += 1;
         TString warn("Warning: Number of bins Y not multiple of ");
         warn += ngroupY;
         Hpr::WarnBox(warn.Data());
      }
      TH2* h2 = (TH2*)fSelHist;
      newhist = h2->Rebin2D(ngroupX, ngroupY, name.Data());
   } else {
      newhist = (TH1*)fSelHist->Clone();
      newhist->GetListOfFunctions()->Clear();
      Int_t first = fSelHist->GetXaxis()->GetFirst();
      Int_t last = fSelHist->GetXaxis()->GetLast();
      newhist->Rebin(ngroupX);
      first /= ngroupX;
      last /= ngroupY;
      newhist->GetXaxis()->SetRange(first, last);
      newhist->SetName(name);
   }
   if (newhist) {
      newhist->SetTitle(title);
      if (gHpr) gHpr->ShowHist(newhist);
      else    newhist->Draw();
   }
}
//_______________________________________________________________________________________

void FitHist::RedefineAxis()
{
   if (fExpHist) {
      Hpr::WarnBox("RedefineAxis not implemented \
for expanded Histogram");
      return;
   }
   Int_t n;
   if (is2dim(fSelHist))
      n = 2;
   else
      n = 1;
   TArrayD xyvals(2 * n);
   //   Double_t *xyvals = new Double_t[2*n];
//  TString title("Set new axis limits");
   TOrdCollection *row_lab = new TOrdCollection();
   row_lab->Add(new TObjString("new xlow, xup"));
   xyvals[0] = fSelHist->GetXaxis()->GetXmin();
   if (!is2dim(fSelHist))
      xyvals[1] = fSelHist->GetXaxis()->GetXmax();
   else {
      xyvals[1] = fSelHist->GetYaxis()->GetXmin();
      xyvals[2] = fSelHist->GetXaxis()->GetXmax();
      xyvals[3] = fSelHist->GetYaxis()->GetXmax();
      row_lab->Add(new TObjString("new ylow, yup"));
   }
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Set new axis limits", itemwidth,
                       ncols, n, xyvals, precission, NULL, row_lab);
//   cout << ret << endl;
   if (ret >= 0) {
      if (!is2dim(fSelHist)) {
         fRangeLowX = xyvals[0];
         fRangeUpX = xyvals[1];
      } else {
         fRangeLowX = xyvals[0];
         fRangeUpX = xyvals[2];
         fRangeLowY = xyvals[1];
         fRangeUpY = xyvals[3];
      }
      TIter next(fCanvas->GetListOfPrimitives());
		TObject *obj;
		while ( obj = next() ) {
			if (obj->InheritsFrom("TH1") ){
				TH1* h = ((TH1*)obj);
				h->GetXaxis()->Set(h->GetNbinsX(), fRangeLowX, fRangeUpX);
				if (h->GetDimension() == 2)
					h->GetYaxis()->Set(h->GetNbinsY(), fRangeLowY, fRangeUpY);
			}
		}
      fSetRange = kTRUE;
      fCanvas->Modified(kTRUE);
      fCanvas->Update();
   }
//   if (xyvals) delete [] xyvals;
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
};
//_______________________________________________________________________________________

void FitHist::RestoreAxis()
{
   if (fExpHist) {
      Hpr::WarnBox("RedefineAxis not implemented \
for expanded Histogram");
      return;
   }

	TIter next(fCanvas->GetListOfPrimitives());
	TObject *obj;
	while ( obj = next() ) {
		if (obj->InheritsFrom("TH1") ){
			TH1* h = ((TH1*)obj);
			h->GetXaxis()->Set(h->GetNbinsX(), fOrigLowX, fOrigUpX);
			if (h->GetDimension() == 2)
				h->GetYaxis()->Set(h->GetNbinsY(), fOrigLowY, fOrigUpY);
		}
	}
	fSetRange = kFALSE;
	fCanvas->Modified(kTRUE);
	fCanvas->Update();
}
//_______________________________________________________________________________________

void FitHist::AddAxis(Int_t where)
{
//   Double_t *xyvals = new Double_t[2];
   TArrayD xyvals(2);
//   TString title("Define axis limits");
   TOrdCollection *row_lab = new TOrdCollection();
   row_lab->Add(new TObjString("Lower, Upper"));
   if (where == 1) {            // xaxis
      xyvals[0] = fCanvas->GetFrame()->GetX1();
      xyvals[1] = fCanvas->GetFrame()->GetX2();
      if (fCanvas->GetLogx()) {
         xyvals[0] = TMath::Power(10, xyvals[0]);
         xyvals[1] = TMath::Power(10, xyvals[1]);
      }
   } else {
      xyvals[0] = fCanvas->GetFrame()->GetY1();
      xyvals[1] = fCanvas->GetFrame()->GetY2();
      if (fCanvas->GetLogy()) {
         xyvals[0] = TMath::Power(10, xyvals[0]);
         xyvals[1] = TMath::Power(10, xyvals[1]);
      }
   }
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, nrows = 1, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Define axis limits", itemwidth,
                       ncols, nrows, xyvals, precission, NULL, row_lab);
	
   if (ret >= 0) {
		Hpr::DoAddAxis(fCanvas, fSelHist, where, xyvals[0],xyvals[1]);
		fHasExtraAxis = kTRUE;
	}
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
}
//_______________________________________________________________________________________

void FitHist::ObjMoved(Int_t /*px*/, Int_t /*py*/, TObject *obj)
{
	if ( gDebug > 0 )
		cout << "ObjMoved: " << obj->ClassName() <<endl;
	if (obj->IsA() == TAxis::Class()) {
	}
}
//_______________________________________________________________________________________

//_______________________________________________________________________________________

void FitHist::HandleLinLogChanged(TObject *obj)
{
	if ( gDebug > 0 ) {
		cout << "FitHist::HandleLinLogChanged: " << obj 
				<< " fCanvas: " << fCanvas <<endl;
	}
}
//_______________________________________________________________________________________

void FitHist::PictToPSFile(Int_t plain_flag)
{
   if (fSelHist) {
      TString hname = fSelHist->GetName();
      hname += ".ps";
      Bool_t ok;
      hname =
          GetString("Write PS-file with name", hname.Data(), &ok,
                    mycanvas);
      if (!ok) {
         cout << " cancelled " << endl;
         return;
      }

      if (plain_flag > 0) {
         fCanvas->SetFillStyle(0);
         fCanvas->SetBorderMode(0);
         fCanvas->GetFrame()->SetFillStyle(0);
         fCanvas->GetFrame()->SetBorderMode(0);

         TPaveText *tpt = (TPaveText *) fCanvas->GetPrimitive("stats");
         if (tpt)
            tpt->SetFillStyle(0);
         tpt = (TPaveText *) fCanvas->GetPrimitive("title");
         if (tpt)
            tpt->SetFillStyle(0);
      }
      gStyle->SetPaperSize(19., 28.);
      fCanvas->SaveAs(hname.Data());
   }
};
//_______________________________________________________________________________________

void FitHist::PictToLP()
{
   if (fSelHist) {
      TString cmd = "lpr -P";
      cmd += gSystem->Getenv("PRINTER");
      TEnv env(".rootrc");      // inspect ROOT's environment
      const char *ccmd =
          env.GetValue("HistPresent.PrintCommand", cmd.Data());
      cmd = ccmd;
      Bool_t ok;
      cmd = GetString("Printer command: ", cmd.Data(), &ok, mycanvas);
      if (!ok) {
         //      cout << "canceled" << endl;
         return;
      }
      fCanvas->SetFillStyle(0);
      fCanvas->SetBorderMode(0);
      gStyle->SetPaperSize(19., 28.);
      fCanvas->SaveAs("temp_pict.ps");
      TString prcmd = cmd;
      prcmd += " ";
      prcmd += "temp_pict.ps";
      gSystem->Exec(prcmd.Data());
      if (cmd.Contains("lpr ")) {
         cmd(2, 1) = "q";
         gSystem->Exec(cmd.Data());
      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteFunctions()
{
   if (fSelHist) {
      ClearMarks();
      new Save2FileDialog(fSelHist->GetListOfFunctions(), NULL, GetMyCanvas());
   }
};
//_______________________________________________________________________________________

void FitHist::WriteOutCanvas()
{
   if (fSelHist) {
      TString hname = fCanvas->GetName();
      hname += ".canvas";
      Bool_t ok;
      TObject *obj;
      hname =
          GetString("Save canvas with name", hname.Data(), &ok, mycanvas);
      if (!ok)
         return;
      if ((obj = gROOT->FindObject(hname.Data()))) {
         Hpr::WarnBox("Object with this name already exists");
         cout << setred << "Object with this name already exists: "
             << obj->ClassName() << setblack << endl;
         return;
      }
//      if (OpenWorkFile()) {
         if (fCanvas->GetAutoExec())
            fCanvas->ToggleAutoExec();
//         fCanvas->SetName(hname.Data());
         TCanvas *nc =
             new TCanvas(hname.Data(), fCanvas->GetTitle(), 50, 500, 720,
                         500);
//         TCanvas * nc = new TCanvas(hname.Data(), "xxx" ,720, 500);
         TIter next(fCanvas->GetListOfPrimitives());
         while ((obj = next())) {
            if (is_a_hist(obj)) {
               TH1 *hi = (TH1 *) obj;
               if (is2dim(hi)) {
                  hi->SetOption(fDrawOpt2Dim);
               } else {
                  TString drawopt;
                  if (fShowContour)
                     drawopt = "hist";
                     if ( fErrorMode != "none")
                        drawopt += "e1";
//                  hi->SetOption(drawopt.Data());
                  if (fFill1Dim) {
                     if (fFillStyle == 0) fFillStyle = 1001;
                     hi->SetFillStyle(fFillStyle);
                     hi->SetFillColor(fFillColor);
                  } else
                     hi->SetFillStyle(0);
               }
            }
            gROOT->SetSelectedPad(nc);
            nc->GetListOfPrimitives()->Add(obj->Clone());
         }
         if (nc->GetAutoExec())
            nc->ToggleAutoExec();
//         nc->Write();
//         CloseWorkFile();
         new Save2FileDialog(nc, NULL, GetMyCanvas());
         delete nc;
//      }
   }
};
//_______________________________________________________________________________________

void FitHist::WriteOutHist()
{
   if (fSelHist) {
      new Save2FileDialog(fSelHist, NULL, GetMyCanvas());
   }
};

//_______________________________________________________________________________________

#include "time.h"

void AddAsString(Double_t x, TAxis * a, Int_t  prec, TOrdCollection *entries)
{
   struct tm * utctis;
   char CHLABEL[256];
   char *LABEL;
   LABEL  = &CHLABEL[0];
	TObjString * os;
   if (a == NULL || !(a->GetTimeDisplay())) {
     	TMrbString s(x, prec);		      // convert double to string
		os = new TObjString(s.Data());   // wrap TObject around it
   } else {
      time_t t = (time_t)x;
      TString tf(a->GetTimeFormat());
      Int_t itoff = tf.Index("%F");
      if (itoff > 0){
         TString tos = tf;
         tos.Remove(0,itoff + 2);
         tf.Resize(itoff);
      }
      utctis = gmtime(&t);
//      cout << "time " << t <<  " tf.Data() " <<  tf.Data() << endl;
      Int_t nchar = strftime(LABEL, 256, tf.Data(), utctis);
      if (nchar <= 0)os = new TObjString("xxx");
		else           os = new TObjString(LABEL);   // wrap TObject around it
   }
   entries->Add((TObject *) os);	   // add to collection of string
}
//_______________________________________________________________________________________

void FitHist::SetXaxisRange()
{
   SetAxisHistX(fCanvas, fSelHist->GetXaxis());
}
//_______________________________________________________________________________________

void FitHist::SetYaxisRange()
{
   SetAxisHistX(fCanvas, fSelHist->GetYaxis());
}
//_______________________________________________________________________________________

void FitHist::SetZaxisRange()
{
   SetAxisHistX(fCanvas, fSelHist->GetZaxis());
}
//_______________________________________________________________________________________

void FitHist::Set2Marks()
{
   Int_t n = 2;
   static Float_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
//   Double_t *xyvals = new Double_t[n*2];
   TArrayD xyvals(2 * n);
   TOrdCollection *row_lab = new TOrdCollection();

   xyvals[0] = x1;
   xyvals[1] = x2;
   xyvals[2] = y1;
   xyvals[3] = y2;
   row_lab->Add(new TObjString("x,y value"));
   row_lab->Add(new TObjString("x,y value"));
// show values to caller and let edit
   Int_t ret = 0, ncols = 2, nrows = n, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, "Marks", itemwidth,
                       ncols, nrows, xyvals, precission, NULL, row_lab);
   if (ret >= 0) {
      x1 = xyvals[0];
      x2 = xyvals[1];
      y1 = xyvals[2];
      y2 = xyvals[3];
      fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
      if (fMarkers == NULL) {
         fMarkers = new  FhMarkerList();
         fSelHist->GetListOfFunctions()->Add(fMarkers);
      }
      fMarkers->Add(new FhMarker(x1, y1, 28));
      fMarkers->Add(new FhMarker(x2, y2, 28));
   }
//   if (xyvals) delete [] xyvals;
   if (row_lab) {
      row_lab->Delete();
      delete row_lab;
   }
   PaintMarks();
};
//_______________________________________________________________________________________

void FitHist::AddMark(TPad * pad, Int_t px, Int_t py)
{
   Float_t x, y;
   if (pad != (TVirtualPad *) fCanvas) {
      cout << " its not us " << endl;
      return;
   }
//   cout << " FitHist::AddMark " << endl;
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
 //     cout << "new  FhMarkerList()  " << endl;
      fMarkers = new  FhMarkerList();
      fSelHist->GetListOfFunctions()->Add(fMarkers);
   }
   x = pad->AbsPixeltoX(px);
   y = pad->AbsPixeltoY(py);
   if (pad->GetLogx()) x = TMath::Power(10, x);
   if (pad->GetLogy()) y = TMath::Power(10, y);
   FhMarker *m = new FhMarker(x, y, 28);
   m->SetMarkerColor(6);
   m->SetMarkerSize(1);
   fMarkers->Add(m);
	PaintMarks();

}
//_______________________________________________________________________________________

void FitHist::ClearMarks()
{
//
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL ) return;
   fSelHist->GetListOfFunctions()->Remove(fMarkers);
   TIter next(fMarkers);
   FhMarker * ti;
   while ( (ti = (FhMarker *) next()) ) {
      fCanvas->GetListOfPrimitives()->Remove(ti);
   }

   fMarkers->Delete();
//   fMarkers->Clear();
   fCanvas->Modified(kTRUE);
   fCanvas->Update();
};
//_______________________________________________________________________________________

Int_t FitHist::GetMarks(TH1 * hist)
{
   Int_t nval = 0;
   Axis_t xmin, xmax, ymin = 0, ymax = 0;
   if (hist) {
      TAxis *xaxis = hist->GetXaxis();
      Int_t first = xaxis->GetFirst();
      Int_t last = xaxis->GetLast();
//      xmin = xaxis->GetBinLowEdge(first);
//      xmax = xaxis->GetBinUpEdge(last);
      xmin = xaxis->GetBinLowEdge(first) + 0.5 * xaxis->GetBinWidth(first);
      xmax = xaxis->GetBinUpEdge(last)   - 0.5 * xaxis->GetBinWidth(last);
      if (is2dim(hist)) {
         TAxis *yaxis = hist->GetYaxis();
         first = yaxis->GetFirst();
         last = yaxis->GetLast();
//         ymin = yaxis->GetBinLowEdge(first);
 //        ymax = yaxis->GetBinUpEdge(last);
         ymin = yaxis->GetBinLowEdge(first) + 0.5 * yaxis->GetBinWidth(first);
         ymax = yaxis->GetBinUpEdge(last)   - 0.5 * yaxis->GetBinWidth(last);
      }
   } else {
      cout << "Null pointer to hist" << endl;
      return -1;
   }
   fMarkers = (FhMarkerList*)hist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) {
      fMarkers = new  FhMarkerList();
      hist->GetListOfFunctions()->Add(fMarkers);
   }

   nval = fMarkers->GetEntries();
   if (nval == 1) {
      if (fSelInside)
         fMarkers->Add(new FhMarker(xmin, ymin, 30));
      else
         fMarkers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   if (nval == 0) {
      fMarkers->Add(new FhMarker(xmin, ymin, 30));
      fMarkers->Add(new FhMarker(xmax, ymax, 30));
      nval = 2;
   }
   fMarkers->Sort();
   return nval;
};
//_______________________________________________________________________________________

void FitHist::PrintMarks()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
      cout << "--- Current fMarkers --------------" << endl;
      FhMarker *ti;
      TIter next(fMarkers);
      while ( (ti = (FhMarker *) next()) ) {
         cout << " x, y " << ti->GetX() << "\t" << ti->GetY() << endl;
      }
   } else {
      cout << "--- No marks set ---" << endl;
   }
};
//_______________________________________________________________________________________

void FitHist::PaintMarks()
{
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers != NULL && fMarkers->GetEntries() > 0) {
//      cout << "--- Current fMarkers --------------" << endl;
      FhMarker *ti;
      TIter next(fMarkers);
      fCanvas->cd();
      while ( (ti = (FhMarker *) next()) ) {
         ti->Draw();
      }
      fCanvas->Modified(kTRUE);
      fCanvas->Update();
   }
};

//____________________________________________________________________________________

Bool_t FitHist::IsThereAnyHist()
{
   TList *tl = gDirectory->GetList();
   TIter next(tl);
   while (TObject * obj = next()) {
      if (is_a_hist(obj))
         return kTRUE;
   }
   return kFALSE;
}


//____________________________________________________________________________________

void FitHist::GetRange()
{
   TH1 *hist = Hpr::GetOneHist(fSelHist);
   if (!hist)
      return;

   fSelHist->GetXaxis()->SetRange(hist->GetXaxis()->GetFirst(),
                                  hist->GetXaxis()->GetLast());
   if ( fSelHist->GetDimension() > 1 )
      fSelHist->GetYaxis()->SetRange(hist->GetYaxis()->GetFirst(),
                                     hist->GetYaxis()->GetLast());
   if ( fSelHist->GetDimension() > 2 )
      fSelHist->GetZaxis()->SetRange(hist->GetZaxis()->GetFirst(),
                                     hist->GetZaxis()->GetLast());
   fCanvas->Modified();
   fCanvas->Update();
}

//____________________________________________________________________________________

void FitHist::KolmogorovTest()
{
   TH1 *hist;
   if (gHpr->GetSelectedHist()->GetSize() > 0) {	//  choose from hist list
      if (gHpr->GetSelectedHist()->GetSize() > 1) {
         Hpr::WarnBox("More than 1 selection");
         return;
      }
      hist = gHpr->GetSelHistAt(0);
   } else
      hist = Hpr::GetOneHist(fSelHist);      // look in memory
   if (hist) {
      fSelHist->KolmogorovTest(hist, "D");
   }
}
//____________________________________________________________________________________

void FitHist::Superimpose(Int_t mode)
{
	Hpr::SuperImpose(fCanvas, fSelHist, mode);
}
//____________________________________________________________________________________
//  find limits

void FitHist::GetLimits()
{
   Int_t inp;
   fBinX_1 = 0;
   fBinY_1 = 0;
   fBinX_2 = 0;
   fBinY_2 = 0;

   FhMarker *p;
   fMarkers = (FhMarkerList*)fSelHist->GetListOfFunctions()->FindObject("FhMarkerList");
   if (fMarkers == NULL) return;
   TIter next(fMarkers);
//   fMarkers->Sort();
   while ( (p = (FhMarker *) next()) ) {
      inp = fSelHist->GetXaxis()->FindBin(p->GetX());
//      inp = XBinNumber(fSelHist, p->GetX());
      if (inp > 0) {
//         cout << "p->GetX() " << p->GetX()<<  " xbin " << inp << endl;
         if (fBinX_1 <= 0) {
            fBinX_1 = inp;
            fX_1 = p->GetX();
         };
         if (fBinX_1 > 0 && inp < fBinX_1) {
            fBinX_1 = inp;
            fX_1 = p->GetX();
         }
         if (inp > fBinX_2) {
            fBinX_2 = inp;
            fX_2 = p->GetX();
         }
      }
      if (is2dim(fSelHist)) inp = fSelHist->GetYaxis()->FindBin(p->GetY());
      else                  inp = 1;

//      inp = YBinNumber(fSelHist, p->GetY());
//      inp = (Int_t)fYbins->At(i);
      if (inp > 0) {
//         cout << "ybin " << inp << endl;
         if (fBinY_1 <= 0) {
            fBinY_1 = inp;
            fY_1 = p->GetY();
         }
         if (fBinY_1 > 0 && inp < fBinY_1) {
            fBinY_1 = inp;
            fY_1 = p->GetY();
         }
         if (inp > fBinY_2) {
            fBinY_2 = inp;
            fY_2 = p->GetY();
         }
      }
   }
   if (fBinX_1 > fBinX_2) {
      int bin = fBinX_1;
      fBinX_1 = fBinX_2;
      fBinX_2 = bin;
      Axis_t val = fX_1;
      fX_1 = fX_2;
      fX_2 = val;
   }
   if (fBinY_1 > fBinY_2) {
      int bin = fBinY_1;
      fBinY_1 = fBinY_2;
      fBinY_2 = bin;
      Axis_t val = fY_1;
      fY_1 = fY_2;
      fY_2 = val;
   }
}

//____________________________________________________________________________________
//

void FitHist::OutputStat(Int_t fill_hist)
{
//   enum dowhat {expand, projectx, projecty, statonly};

//   ExpandProject(statonly);
   cout <<
       "============================================================="
       << endl;

   if (!fSelPad) {
      cout << "Cant find pad, call Debugger" << endl;
      return;
   }
   if (GetMarks(fSelHist) <= 0 && !Nwindows()) {
      cout << "No marks, no windows" << endl;
      return;
   }
	TH1F* h_bincont = NULL;
	Double_t h_min = 1E20;
	Double_t h_max = -1E20;
	
	
//  1-dim case
   if (!is2dim(fSelHist)) {
      Double_t x = 0, cont = 0;
      Double_t sum   = 0;
      Double_t sumx  = 0;
      Double_t sumx2 = 0;
      Double_t mean  = 0;
      Double_t sigma = 0;
      Float_t xlow, xup;
      if (Nwindows() == 0) {
         if (GetMarks(fSelHist) != 2) {
            Hpr::WarnBox("Please select 1 window or exactly 2 marks");
            return;
         }
         GetLimits();
         xlow = fX_1;
         xup = fX_2;
         cout << "Using marks " << endl;
      } else {
         if (Nwindows() > 1) {
            Hpr::WarnBox("Please select one window");
            return;
         }
         TMrbWindow *wdw = (TMrbWindow *) fActiveWindows->First();
         xlow = wdw->GetX1();
         xup = wdw->GetX2();
         cout << "Using window " << wdw->GetName() << endl;
      }
      for (Int_t i = 1; i <= fSelHist->GetNbinsX(); i++) {
         x = fSelHist->GetBinCenter(i);
         if (x >= xlow && x < xup) {
            cont = fSelHist->GetBinContent(i);
				if (cont < h_min) h_min = cont;
				if (cont > h_max) h_max = cont;
            sum += cont;
            sumx += x * cont;
            sumx2 += x * x * cont;
         }
		}
		if ( fill_hist == 1 ) {
			TString pname(fOrigHist->GetName());
			pname += "_BinCont_";
			pname += fSerialPx;
			h_max += 1;
			Int_t nbins = (Int_t)(h_max - h_min);
			h_bincont = new TH1F(pname, fOrigHist->GetTitle(), nbins, h_min, h_max);
			fSerialPx++;
			for (Int_t i = 1; i <= fSelHist->GetNbinsX(); i++) {
				x = fSelHist->GetBinCenter(i);
				if (x >= xlow && x < xup) {
					cont = fSelHist->GetBinContent(i);
					h_bincont->Fill(cont);
				}
			}
		}
      if (sum > 0.) {
         mean = sumx / sum;
         sigma = sqrt(sumx2 / sum - mean * mean);
      }
      cout << "Statistics for Histogram:" << fSelHist->GetName()
          << " from " << xlow << " to " << xup << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
      cout << "Content    " << sum << endl;
      cout << "Mean       " << mean << endl;
      cout << "Sigma      " << sigma << endl;
      cout << "2.35*Sigma " << 2.35 * sigma << endl;	// 2*sqrt(2ln2)
      cout <<
          "-------------------------------------------------------------"
          << endl;
/*
      TOrdCollection *row_lab = new TOrdCollection();
      row_lab->Add(new TObjString("Content"));
      row_lab->Add(new TObjString("Mean"));
      row_lab->Add(new TObjString("Sigma"));
      row_lab->Add(new TObjString("2.35*Sigma"));

      TOrdCollection *col_lab = new TOrdCollection();
      col_lab->Add(new TObjString((char *) fOrigHist->GetName()));
      TArrayD par(4);
//      double par[4];
      par[0] = sum;
      par[1] = mean;
      par[2] = sigma;
      par[3] = 2.35 * sigma;
      TString title("Statistics in window: ");
      title += Form("%f",xlow);
      title += " to ";
      title += Form("%f",xup);
      Int_t ret = 0, ncols = 1, nrows = 4, itemwidth = 240, precission = 5;
      TGMrbTableOfDoubles(mycanvas, &ret, title.Data(), itemwidth,
                          ncols, nrows, par, precission, col_lab, row_lab);
      if (col_lab)
         col_lab->Delete();
      if (row_lab)
         row_lab->Delete();
      if (col_lab)
         delete col_lab;
      if (row_lab)
         delete row_lab;
*/
   } else {
      GetLimits();
      double sum = 0;
      UpdateCut();
//      CheckList(fActiveCuts);
      int nc = Ncuts();
      TAxis *xaxis = fSelHist->GetXaxis();
      TAxis *yaxis = fSelHist->GetYaxis();
      for (int i = 1; i <= fSelHist->GetNbinsX(); i++) {
         Axis_t xcent = xaxis->GetBinCenter(i);
			for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
				Axis_t ycent = yaxis->GetBinCenter(j);
				if (nc <= 0 ||(nc > 0 && InsideCut((float) xcent, (float) ycent))) {
					Double_t cont = fSelHist->GetCellContent(i, j);
					sum += cont;
					if (cont < h_min) h_min = cont;
					if (cont > h_max) h_max = cont;
				}
			}
      }
		if ( fill_hist == 1 ) {
			TString pname(fOrigHist->GetName());
			pname += "_BinCont_";
			pname += fSerialPx;
			h_max += 1;
			Int_t nbins = (Int_t)(h_max - h_min);
			h_bincont = new TH1F(pname, fOrigHist->GetTitle(), nbins, h_min, h_max);
			fSerialPx++;
			for (int i = 1; i <= fSelHist->GetNbinsX(); i++) {
				Axis_t xcent = xaxis->GetBinCenter(i);
				for (int j = 1; j <= fSelHist->GetNbinsY(); j++) {
					Axis_t ycent = yaxis->GetBinCenter(j);
					if (nc <= 0 ||(nc > 0 && InsideCut((float) xcent, (float) ycent))) {
						sum += fSelHist->GetCellContent(i, j);
						if ( fill_hist == 1 ) {
							h_bincont->Fill(fSelHist->GetCellContent(i, j));
						}
					}
				}
			}
		}
		
      cout << "Statistics for 2 dim Histogram:" << fSelHist->
          GetName() << endl;

//      cout << " X from " << fX_1 << " to " << fX_2 << endl;
//      cout << " Y from " << fY_1 << " to " << fY_2 << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
      if (nc) {
         cout << "Cuts applied: " << endl;
         TIter next(fActiveCuts);
         while (TMrbWindow2D * cut = (TMrbWindow2D *) next())
            PrintOneCut(cut);
         cout <<
             "-------------------------------------------------------------"
             << endl;
      } else {
         cout << "No cuts applied!!!" << endl;
      }
      cout << "Content ";
      if (fSelInside) cout << "inside: ";
      else            cout << "outside: ";
      cout << sum << endl;
      cout <<
          "-------------------------------------------------------------"
          << endl;
   }
	if ( h_bincont != NULL ) {
		gHpr->ShowHist(h_bincont);
	}
}

//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::Expand()
{
//   enum dowhat {expand, projectx, projecty, statonly};
   ExpandProject(expand);
   SaveDefaults(kFALSE);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectBoth()
{
//   enum dowhat {expand, projectx, projecty, statonly,projectboth};
	TString opt = fSelHist->GetOption();
	if ( opt.Contains("LEGO", TString::kIgnoreCase) ||  
		  opt.Contains("SURF", TString::kIgnoreCase)) {
		cout << "Cant do that with LEGO or SURF" << endl;
		return;
	}
   ExpandProject(projectboth);
	fProjectedBoth = 1;
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectX()
{
//   enum dowhat {expand, projectx, projecty, statonly};
   ExpandProject(projectx);
}

//____________________________________________________________________________________

// Show projection of histograms

void FitHist::ProjectY()
{
   ExpandProject(projecty);
}

//____________________________________________________________________________________

// Show projection of histograms along a function

void FitHist::ProjectF()
{
   ExpandProject(projectf);
}

//____________________________________________________________________________________

// Fast Fourier Transform

void FitHist::FastFT()
{
#if ROOTVERSION < 51100
   cout << "FFT not yet available" << endl;
#else
   const char helpText[] =
" This function allows to do discrete Fourier transforms of TH1 and TH2.\n\
 Available transform types and flags are described below.\n\
\n\
 To extract more information about the transform, use the function\n\
  TVirtualFFT::GetCurrentTransform() to get a pointer to the current\n\
  transform object.\n\
\n\
 Parameters:\n\
  1st - histogram for the output. If a null pointer is passed, a new histogram is created\n\
  and returned, otherwise, the provided histogram is used and should be big enough\n\
\n\
  Options: option parameters consists of 3 parts:\n\
    - option on what to return\n\
   \"RE\" - returns a histogram of the real part of the output\n\
   \"IM\" - returns a histogram of the imaginary part of the output\n\
   \"MAG\"- returns a histogram of the magnitude of the output\n\
   \"PH\" - returns a histogram of the phase of the output\n\
\n\
    - option of transform type\n\
   \"R2C\"  - real to complex transforms - default\n\
   \"R2HC\" - real to halfcomplex (special format of storing output data,\n\
          results the same as for R2C)\n\
   \"DHT\" - discrete Hartley transform\n\
         real to real transforms (sine and cosine):\n\
   \"R2R_0\", \"R2R_1\", \"R2R_2\", \"R2R_3\" - discrete cosine transforms of types I-IV\n\
   \"R2R_4\", \"R2R_5\", \"R2R_6\", \"R2R_7\" - discrete sine transforms of types I-IV\n\
    To specify the type of each dimension of a 2-dimensional real to real\n\
    transform, use options of form \"R2R_XX\", for example, \"R2R_02\" for a transform,\n\
    which is of type \"R2R_0\" in 1st dimension and  \"R2R_2\" in the 2nd.\n\
\n\
    - option of transform flag\n\
    \"ES\" (from \"estimate\") - no time in preparing the transform, but probably sub-optimal\n\
       performance\n\
    \"M\" (from \"measure\")   - some time spend in finding the optimal way to do the transform\n\
    \"P\" (from \"patient\")   - more time spend in finding the optimal way to do the transform\n\
    \"EX\" (from \"exhaustive\") - the most optimal way is found\n\
     This option should be chosen depending on how many transforms of the same size and\n\
     type are going to be done. Planning is only done once, for the first transform of this\n\
     size and type. Default is \"ES\".\n\
";

   static Int_t mag = 1, re = 0 , img = 0 , ph = 0,
                r2c = 1, dht = 0;
   static TString opt_r2r("R2R_0");

   static void *valp[50];
   Int_t ind = 0;
   Bool_t ok = kTRUE;
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("CheckButton_              Magnitude"));
   row_lab->Add(new TObjString("CheckButton_              Real Part"));
   row_lab->Add(new TObjString("CheckButton_         Imaginary Part"));
   row_lab->Add(new TObjString("CheckButton_                  Phase"));
   row_lab->Add(new TObjString("RadioButton_TType:  Real to Complex"));
   row_lab->Add(new TObjString("RadioButton_TType: Discrete Hartley"));
//   row_lab->Add(new TObjString("RadioButton_TType: Real to Real"));
//   row_lab->Add(new TObjString("StringValue_Option for Real to Real"));

   valp[ind++] = &mag;
   valp[ind++] = &re;
   valp[ind++] = &img;
   valp[ind++] = &ph;
   valp[ind++] = &r2c;
   valp[ind++] = &dht;
//   valp[ind++] = &r2r;
//   valp[ind++] = &opt_r2r;

   Int_t itemwidth = 320;
   ok = GetStringExt("FFT Parameters", NULL, itemwidth, mycanvas,
                   NULL, NULL, row_lab, valp,
                   NULL, NULL, &helpText[0]);
	if ( !ok )
		return;
   TString option;
   TString name;
   TString title;

   if (r2c) option = "R2C";
   if (dht) option = "DHT";
//   if (r2r) option = opt_r2r;
   if (mag != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "MAG";
      title = fSelHist->GetTitle();
      title += "-Magnitude of FFT, ";
      title += option;
      option += " MAG";
      cout << "Option: " << option << endl;
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (gHpr) gHpr->ShowHist(hresult);
   }
   if (re != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "RE";
      title = fSelHist->GetTitle();
      title += "-Real part of FFT, ";
      title += option;
      option += " RE";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (gHpr) gHpr->ShowHist(hresult);
   }
   if (img != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "IM";
      title = fSelHist->GetTitle();
      title += "-Imaginary part of FFT, ";
      title += option;
      option += " IM";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (gHpr) gHpr->ShowHist(hresult);
   }
   if (ph != 0) {
      TH1 * hresult = NULL;
      name = fSelHist->GetName();
      name = name + "_" + option + "_" + "PH";
      title = fSelHist->GetTitle();
      title += "-Phase of FFT, ";
      title += option;
      option += " PH";
      hresult = fSelHist->FFT(hresult, option);
      hresult->SetName(name);
      hresult->SetTitle(title);
      hresult->SetStats(kFALSE);
      if  (gHpr) gHpr->ShowHist(hresult);
   }
#endif
}

//____________________________________________________________________________________

// Rotate 2dim histograms

void FitHist::Rotate(Int_t sense)
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be rotated");
      return;
   }
   Double_t tan_alpha = 0;
   Double_t alpha_deg = 0;
   TList * fl = fSelHist->GetListOfFunctions();
   if (fl->GetSize() > 0) {
      TIter next(fl);
      while (TObject * obj = next()){
         if(obj->IsA() == TF1::Class()) {
            TF1 * func = (TF1*)obj;
            if (func->GetNpar() == 2) {
               if (tan_alpha != 0)
                cout << setred << "More than 1 Pol1 defined, take last"
                     << setblack << endl;
              tan_alpha = func->GetParameter(1);
            }
         }
      }
   }
   if (tan_alpha == 0) {
      Bool_t ok;
      alpha_deg = (Double_t)GetFloat("Enter angle in degrees or 0 to define Pol1",
         0, &ok, mycanvas);
      if (!ok ||  alpha_deg == 0) {
         Hpr::WarnBox("Please fit a Pol1 to define rotation angle");
         return;
      }
      alpha_deg = TMath::Abs(alpha_deg);
      if (sense == 1) alpha_deg = - alpha_deg;
   } else {
      if (sense == 1) tan_alpha = - 1 / tan_alpha;
      alpha_deg = TMath::ATan(tan_alpha) * 180. / TMath::Pi();
   }
   TH2 * hrot = rotate_hist((TH2*)fSelHist, alpha_deg, fSerialRot);
   fSerialRot++;
   fCanvas->cd();
   if  (gHpr) gHpr->ShowHist(hrot);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Transpose 2dim histograms

void FitHist::Transpose()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be transposed");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_transp";
   TString htitle(h2->GetTitle());
   htitle += "_transp";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TH2F *h2_transp = new TH2F(hname, htitle,
                              ya->GetNbins(), ya->GetXmin(), ya->GetXmax(),
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   for (Int_t ix = 0; ix <= xa->GetNbins(); ix++) {
      for (Int_t iy = 0; iy <= ya->GetNbins(); iy++) {
         h2_transp->SetCellContent(iy, ix, h2->GetCellContent(ix, iy));
         h2_transp->SetCellError  (iy, ix, h2->GetCellError  (ix, iy));
      }
   }
   h2_transp->SetEntries(h2->GetEntries());
   h2_transp->GetXaxis()->SetTitle(h2->GetYaxis()->GetTitle());
   h2_transp->GetYaxis()->SetTitle(h2->GetXaxis()->GetTitle());
//   fSelHist = h2_transp;
   fCanvas->cd();
   if  (gHpr) gHpr->ShowHist(h2_transp);
//   Draw2Dim();
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileX()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be profiled");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_profX";
   hname += fSerialPx;
   fSerialPx++;
   TString htitle(h2->GetTitle());
   htitle += "_profX";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TMrbArrayD yslice(ya->GetNbins());
   yslice.SetBinWidth(ya->GetBinWidth(1));
   yslice.SetLowEdge(ya->GetXmin());
//   Int_t non_zero = 0;
   TH1D *h_prof = new TH1D(hname, htitle,
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   UpdateCut();
   Int_t nc = Ncuts();
   Double_t max_error = 0;
   for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
      yslice.Reset();
      for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
         if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
                              ya->GetBinCenter(iy)))
            continue;
         yslice[iy-1] = h2->GetCellContent(ix, iy);
      }
//      cout << yslice.GetMean() << endl;
      if (yslice.GetSum() != 0) {
         h_prof->SetBinContent(ix, yslice.GetMean());
         Double_t error = yslice.GetRMS() / TMath::Sqrt(yslice.GetSum());
         h_prof->SetBinError(ix, error);
         max_error = TMath::Max(error, max_error);
      }
   }
 //     cout << "max_error " << max_error << endl;
   for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
      if (h_prof->GetBinContent(ix) != 0 &&
          h_prof->GetBinError(ix) == 0)
            h_prof->SetBinError(ix, max_error);
   }
   h_prof->SetEntries(h2->GetEntries());
   fCanvas->cd();
   if  (gHpr) gHpr->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Profile 2dim histograms

void FitHist::ProfileY()
{
   if (!is2dim(fSelHist)) {
      Hpr::WarnBox("Only 2dim hist can be profiled");
      return;
   }
   TH2F *h2 = (TH2F *)fOrigHist;
   TString hname(h2->GetName());
   hname += "_profY";
   hname += fSerialPx;
   fSerialPx++;
   TString htitle(h2->GetTitle());
   htitle += "_profY";
   TAxis *xa = h2->GetXaxis();
   TAxis *ya = h2->GetYaxis();
   TMrbArrayD xslice(xa->GetNbins());
   xslice.SetBinWidth(xa->GetBinWidth(1));
   xslice.SetLowEdge(xa->GetXmin());
//   Int_t non_zero = 0;
   TH1D *h_prof = new TH1D(hname, htitle,
                              xa->GetNbins(), xa->GetXmin(), xa->GetXmax());
   UpdateCut();
   Int_t nc = Ncuts();
   Double_t max_error = 0;
   for (Int_t iy = 1; iy <= ya->GetNbins(); iy++) {
      xslice.Reset();
      for (Int_t ix = 1; ix <= xa->GetNbins(); ix++) {
         if (nc > 0 && !InsideCut(xa->GetBinCenter(ix),
                              ya->GetBinCenter(iy)))
            continue;
         xslice[ix-1] = h2->GetCellContent(ix, iy);
      }
//      cout << xslice.GetMean() << endl;
      if (xslice.GetSum() != 0) {
         h_prof->SetBinContent(iy, xslice.GetMean());
         Double_t error = xslice.GetRMS() / TMath::Sqrt(xslice.GetSum());
         h_prof->SetBinError(iy, error);
         max_error = TMath::Max(error, max_error);
      }
   }
//      cout << "max_error " << max_error << endl;
   for (Int_t ix = 1; ix <= h_prof->GetXaxis()->GetNbins(); ix++) {
      if (h_prof->GetBinContent(ix) != 0 &&
          h_prof->GetBinError(ix) == 0)
            h_prof->SetBinError(ix, max_error);
   }
   h_prof->SetEntries(h2->GetEntries());
   fCanvas->cd();
   if  (gHpr) gHpr->ShowHist(h_prof);
}
//____________________________________________________________________________________

// Show expanded part of histograms

void FitHist::ExpandProject(Int_t what)
{
//   enum dowhat {expand, projectx, projecty, statonly, profilex, profiley};

//   cout << "enter ExpandProject(Int_t what)" << endl;
   if (!fSelPad) {
      cout << "Cant find pad, call Debugger" << endl;
   }

   if (GetMarks(fSelHist) <= 0) {
      cout << "No selection" << endl;
   }
   TString expname;
   GetLimits();
//   cout << "fBinX_1,2 " << fBinX_1 << " " << fBinX_2 << endl;
//   cout << "fBinY_1,2 " << fBinY_1 << " " << fBinY_2 << endl;
//  1-dim case
   if (!is2dim(fSelHist)) {
      Double_t x = 0, cont = 0;
      Double_t sum = 0;
      Double_t sumx = 0;
      Double_t sumx2 = 0;
//      Double_t mean;
//     Double_t sigma;
//     get values in expanded hist
      TAxis *xaxis_new = fSelHist->GetXaxis();
      Axis_t bw = xaxis_new->GetBinWidth(fBinX_1);
      fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
      fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + bw;

      fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * bw);
      fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * bw);
      Int_t NbinX = fBinux - fBinlx + 1;
//      cout << "binlx,ux " << binlx << " " << binux << endl;

      if (what == expand) {
         if (fExpHist)
            fExpHist->GetListOfFunctions()->Clear();
         expname = fEname;
         expname += fExpInd;
         fExpInd++;
         fExpHist = new TH1D(expname, fOrigHist->GetTitle(),
                            NbinX, fExplx, fExpux);
         fExpHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         fExpHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
      }
      for (Int_t i = 0; i <= fOrigHist->GetNbinsX() + 1; i++) {
         cont = fOrigHist->GetBinContent(i);
         x = fOrigHist->GetBinCenter(i);
         if (i >= fBinlx && i <= fBinux) {
            sum += cont;
            sumx += x * cont;
            sumx2 += x * x * cont;
         }
         if (what == expand) {
            Int_t newbin = i - fBinlx + 1;
            if (fOrigHist->GetSumw2N()) {
               fExpHist->Fill(x, cont);
            } else {
               fExpHist->SetBinContent(newbin, cont);
            }
            if (i >= fBinlx && i <= fBinux && fOrigHist->GetSumw2N())
               fExpHist->SetBinError(newbin, fOrigHist->GetBinError(i));
         }
      }
//      if (sum > 0.) {
//         mean = sumx / sum;
//         sigma = sqrt(sumx2 / sum - mean * mean);
//      }
      if (fExpHist)
         fExpHist->SetEntries(Int_t(sum));

      if (what == statonly) {
         return;
      }
   } else {

//  2-dim case
      Double_t sum = 0;
//      Double_t x = 0;
      TF1 * func = NULL;
      if (what == projectf) {
         TIter next(fSelHist->GetListOfFunctions());
         while (TObject * obj =(TObject *)next()) {
            if (is_a_function(obj)) {
               if (func != NULL)
                  Hpr::WarnBox("More than 1 function defined, take first");
                else func =(TF1 *)obj;
            }
         }
         if (!func) {
            Hpr::WarnBox("No function found");
            return;
         }
      }
      TAxis *xaxis = fOrigHist->GetXaxis();
      TAxis *yaxis = fOrigHist->GetYaxis();
//     get values in expanded hist
      TAxis *xaxis_new = fSelHist->GetXaxis();
      fExplx = xaxis_new->GetBinLowEdge(fBinX_1);
      Axis_t xbw = xaxis_new->GetBinWidth(fBinX_1);
      fExpux = xaxis_new->GetBinLowEdge(fBinX_2) + xbw;

      fBinlx = XBinNumber(fOrigHist, fExplx + 0.5 * xbw);
      fBinux = XBinNumber(fOrigHist, fExpux - 0.5 * xbw);
      Int_t NbinX = fBinux - fBinlx + 1;

      TAxis *yaxis_new = fSelHist->GetYaxis();
      fExply = yaxis_new->GetBinLowEdge(fBinY_1);
      Axis_t ybw = yaxis_new->GetBinWidth(fBinY_1);
      fExpuy = yaxis_new->GetBinLowEdge(fBinY_2) + ybw;
      fBinly = YBinNumber(fOrigHist, fExply + 0.5 * ybw);
      fBinuy = YBinNumber(fOrigHist, fExpuy - 0.5 * ybw);
      Int_t NbinY = fBinuy - fBinly + 1;
//      cout << "fExplx, ux, ly, uy "
//       <<fExplx << " "  << fExpux<< " "  << fExply<< " "  << fExpuy<< endl;
//      cout << "fBinlx, ux, ly, uy "
//       <<fBinlx << " "  << fBinux<< " "  << fBinly<< " "  << fBinuy<< endl;
      Int_t nperbinX = 1, nperbinY = 1;
      TString pname(fOrigHist->GetName());
      if (what == projectx ||what == projectboth) {
         pname += "_ProjX";
         pname += fSerialPx;

         fProjHistX = new TH1F(pname, fOrigHist->GetTitle(),
                              NbinX, fExplx, fExpux);
         fSerialPx++;
      }
      if (what == projecty || what == projectf || what == projectboth) {
         Axis_t low = fExply;
         Axis_t up = fExpuy;
         if (what == projectf) {
            up = 0.5 * (fExpuy - fExply);
            low = -up;
            pname += "_Pfunc";
            pname += fSerialPf;
            fSerialPf++;
         } else {
            pname += "_ProjY";
            pname += fSerialPf;
            fSerialPy++;
         }
         fProjHistY = new TH1F(pname, fOrigHist->GetTitle(),
                              NbinY, low, up);
      }
      if (what == expand) {
         Int_t MaxBin2dim = 1000;
         nperbinX = (NbinX - 1) / MaxBin2dim + 1;
         Int_t NbinXrebin = (NbinX - 1) / nperbinX + 1;
         nperbinY = (NbinY - 1) / MaxBin2dim + 1;
         Int_t NbinYrebin = (NbinY - 1) / nperbinY + 1;
         if (fExpHist)
            fExpHist->GetListOfFunctions()->Clear();
         expname = fEname;
         expname += fExpInd;
         fExpInd++;
         fExpHist = new TH2F(expname, fOrigHist->GetTitle(),
                            NbinXrebin, fExplx, fExpux, NbinYrebin, fExply,
                            fExpuy);
         fExpHist->GetXaxis()->SetTitle(fOrigHist->GetXaxis()->GetTitle());
         fExpHist->GetYaxis()->SetTitle(fOrigHist->GetYaxis()->GetTitle());
         fSelHist = fExpHist;
      }
      TAxis *xa = fSelHist->GetXaxis();
      TAxis *ya = fSelHist->GetYaxis();
//     is there a cut active
      Axis_t xcent;
      Axis_t ycent;
      UpdateCut();
      int nc = Ncuts();
      for (Int_t i = 0; i <= fOrigHist->GetNbinsX() + 1; i++) {
         for (Int_t j = 0; j <= fOrigHist->GetNbinsY() + 1; j++) {
            xcent = xaxis->GetBinCenter(i);
            ycent = yaxis->GetBinCenter(j);
            if (nc && !InsideCut((float) xcent, (float) ycent))
               continue;
            Stat_t cont = fOrigHist->GetCellContent(i, j);
            if (what == projectx || what == projectboth) {
               if (j >= fBinly && j <= fBinuy)
                  fProjHistX->Fill(xcent, cont);
            }
            if (what == projecty || what == projectboth) {
               if (i >= fBinlx && i <= fBinux)
                  fProjHistY->Fill(ycent, cont);
            }
            if (what == projectf) {
               if (i >= fBinlx && i <= fBinux) {
                  ycent -= func->Eval(xcent);
                  fProjHistY->Fill(ycent, cont);
               }
            }
            if (what == expand) {
               Int_t irebin = Int_t((i - fBinlx) / nperbinX + 1);
               if (irebin < 0)
                  irebin = 0;
               if (irebin > fSelHist->GetNbinsX() + 1)
                  irebin = fSelHist->GetNbinsX() + 1;
               Int_t jrebin = Int_t((j - fBinly) / nperbinY + 1);
               if (jrebin < 0)
                  jrebin = 0;
               if (jrebin > fSelHist->GetNbinsY() + 1)
                  jrebin = fSelHist->GetNbinsY() + 1;
               Stat_t oldcont = fSelHist->GetCellContent(irebin, jrebin);
//                 cout << i << " " << j << " "<< irebin << " "<< jrebin << endl;
//                 cout << xcent << " " << ycent << " " << oldcont << " " << cont<< endl;
               oldcont += cont;
               xcent = xa->GetBinCenter(irebin);
               ycent = ya->GetBinCenter(jrebin);

               fSelHist->SetCellContent(irebin, jrebin, 0);
               TH2 *ph2 = (TH2 *) fSelHist;
               ph2->Fill(xcent, ycent, oldcont);
            }
            if (i >= fBinlx && i <= fBinux && j >= fBinly && j <= fBinuy)
               sum += cont;
         }
      }
      if (what == statonly) {
         return;
      }
      ClearMarks();
      fSelPad->cd();
      if (what == projectboth) {
			TList temp;
			TList * lof = fSelPad->GetListOfPrimitives();
			TIter next( lof );
			TObject * o;
			while ( o = next() ) {
				if ( o->IsA() == TPolyLine::Class() ||
					o->IsA() == TGaxis::Class() ) {
					temp.Add(o);
				}
			}
			TIter next1(&temp);
			while ( o = next1() ) {
				lof->Remove(o);
//				delete o;
			}
         Double_t bothratio = WindowSizeDialog::fProjectBothRatio;

         if (bothratio > 1 || bothratio <= 0)
            bothratio = 0.8;
         fCanvas->SetTopMargin(1 - bothratio);
         fCanvas->SetRightMargin(1 - bothratio);
         Double_t xmin = xa->GetBinLowEdge(xa->GetFirst());
         Double_t xmax = xa->GetBinUpEdge(xa->GetLast());
         Double_t dx = (xmax - xmin) / (Double_t) fProjHistX->GetNbinsX();
         Double_t ymin = ya->GetBinLowEdge(ya->GetFirst());
         Double_t ymax = ya->GetBinUpEdge(ya->GetLast());
         Double_t y0 = ymax + 0.005 * (ymax - ymin);
         Double_t y1 = ymin + 0.9 * (ymax - ymin) / bothratio;
//         cout << "ymax " << ymax << " ymin " << ymin<< " y1 " << y1<< endl;
         Double_t maxcont = -10000000;
         Int_t nbins = fProjHistX->GetNbinsX();
         for (Int_t i = 1; i <= nbins; i++) {
            if (fProjHistX->GetBinContent(i) > maxcont)
               maxcont = fProjHistX->GetBinContent(i);
         }
         if (maxcont == 0)
            return;
         Double_t yfac = (y1 - y0) / maxcont;
         Int_t np = 0;
         Double_t x = xmin;
         Double_t y = y0;
         TPolyLine *lpx = new TPolyLine(nbins * 2 + 3);
         for (Int_t i = 1; i <= nbins; i++) {
            lpx->SetPoint(np, x, y);
            np++;
            y = y0 + yfac * fProjHistX->GetBinContent(i);
            lpx->SetPoint(np, x, y);
            np++;
            x += dx;
         }
         lpx->SetPoint(np, x, y);
         np++;
         lpx->SetPoint(np, x, y0);
         np++;
         lpx->SetPoint(np, xmin, y0);
 //        cout << "np " << np << endl;
         lpx->Draw("F");
         lpx->SetFillStyle(1001);
         lpx->SetFillColor(38);
         lpx->Draw();
         TGaxis *naxis =
             new TGaxis(xmin + dx * nbins, y0, xmin + dx * nbins, y1,
                        0, maxcont, 404, "-");
         naxis->SetLabelOffset(0.01);
         naxis->SetLabelSize(0.02);
         naxis->Draw();

//         xmin = xa->GetXmin();
//         xmax = xa->GetXmax();
//         ymin = ya->GetXmin();
//         ymax = ya->GetXmax();
         Double_t dy = (ymax - ymin) / (Double_t) fProjHistY->GetNbinsX();
         Double_t x0 = xmax + 0.005 * (xmax - xmin);
         Double_t x1 = xmin + 0.9 * (xmax - xmin) / bothratio;
         maxcont = -10000000;
         for (Int_t i = 1; i <= fProjHistY->GetNbinsX(); i++) {
            if (fProjHistY->GetBinContent(i) > maxcont)
               maxcont = fProjHistY->GetBinContent(i);
         }
         if (maxcont == 0)
            return;
         Double_t xfac = (x1 - x0) / maxcont;
         np = 0;
         x = x0;
         y = ymin;
         TPolyLine *lpy = new TPolyLine(fProjHistY->GetNbinsX() * 2 + 3);
         nbins = fProjHistY->GetNbinsX();
         for (Int_t i = 1; i <= nbins; i++) {
            lpy->SetPoint(np, x, y);
            np++;
            x = x0 + xfac * fProjHistY->GetBinContent(i);
            lpy->SetPoint(np, x, y);
            np++;
            y += dy;
         }
         lpy->SetPoint(np, x, y);
         np++;
         lpy->SetPoint(np, x0, y);
         np++;
         lpy->SetPoint(np, x0, ymin);
 //        cout << "np " << np << endl;
         lpy->Draw("F");
         lpy->SetFillStyle(1001);
         lpy->SetFillColor(45);
         lpy->Draw();
         naxis = new TGaxis(x0, ymin + nbins * dy, x1, ymin + nbins * dy,
                            0, maxcont, 404, "-");
         naxis->SetLabelOffset(0.01);
         naxis->SetLabelSize(0.02);
         naxis->Draw();
         fCanvas->Update();
         return;
      } else if (what == projectx) {
         gHpr->ShowHist(fProjHistX);
         return;
      } else if (what == projecty || what == projectf) {
         gHpr->ShowHist(fProjHistY);
         return;
      } else {
         fSelHist->SetEntries(sum);
      }
   }
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {
      TIter next(lof);
		TObject * p;
      while ( (p = next()) ) {
//            p->Print();
//  dont add TPaletteAxis
         if (strcmp(p->GetName(),"palette"))
            fExpHist->GetListOfFunctions()->Add(p);
      }
   }
   fSelHist = fExpHist;
   fCanvas->cd();
   if (is2dim(fSelHist))
      Draw2Dim();
   else if (is3dim(fSelHist))
      Draw3Dim();
   else
      Draw1Dim();

//   ClearMarks();
   fCanvas->Modified();
}

//____________________________________________________________________________

void FitHist::ExecDefMacro()
{
   if (!gSystem->AccessPathName(attrname, kFileExists)) {
      gROOT->LoadMacro(attrname);
      TString cmd = attrname;
      Int_t p = cmd.Index(".");
      cmd.Remove(p, 2);
      cmd =
          cmd + "(\"" + fCname.Data() + "\",\"" + fSelHist->GetName() +
          "\")";
//      cout << cmd << endl;
      gHpr->SetCurrentHist(fSelHist);
      gROOT->ProcessLine((const char *) cmd);
   }
}

//____________________________________________________________________________

void FitHist::Draw3Dim()
{
//   TString drawopt("iso");
	cout << "fDrawOpt3Dim " << fDrawOpt3Dim << endl;
   fSelHist->SetDrawOption(fDrawOpt3Dim);
   fSelHist->SetOption(fDrawOpt3Dim);
   fSelHist->Draw(fDrawOpt3Dim);
	fSelHist->SetFillColor(fHistFillColor3Dim);
	fSelHist->SetLineColor(fHistLineColor3Dim);
	fSelHist->SetLineStyle(1);
	fSelHist->SetLineWidth(1);
	fSelHist->SetMarkerColor(fMarkerColor3Dim);
	fSelHist->SetMarkerStyle(fMarkerStyle3Dim);
	fSelHist->SetMarkerSize (fMarkerSize3Dim);
}
//____________________________________________________________________________

void FitHist::Draw1Dim()
{
   TString drawopt;
	fCanvas->cd();
   fSelHist->Draw();
   if ( gROOT->GetForceStyle() ) {
		fCanvas->UseCurrentStyle();
		if (fErrorMode != "none") {
			 drawopt += fErrorMode;
		} 
//		if ( fMarkerSize > 0 && fShowContour == 0 ) {
//			 drawopt += "P";
//		}
		if (drawopt.Length() == 0 || fShowContour != 0) drawopt += "HIST";
		if (fShowMarkers != 0) drawopt += "P";
		gStyle->SetOptTitle(fShowTitle);
		if (fFill1Dim && fSelHist->GetNbinsX() < 50000) {
			 fSelHist->SetFillStyle(fFillStyle);
			 fSelHist->SetFillColor(fFillColor);
		} else
			 fSelHist->SetFillStyle(0);
		if (fSimpleLine)
			drawopt += "L";
		else if (fSmoothLine)
			drawopt += "C";
		else if (fBarChart)
			drawopt += "B";
		else if (fBarChartH)
			drawopt += "BARH";
		else if (fBarChart3D)
			drawopt += "BAR";
		else if (fPieChart)
			drawopt += "PIE";
		else if (fText) {
			drawopt +="TEXT";
			if (fTextAngle > 0){
				drawopt += fTextAngle;
			}
		}
		if (gPad->GetTickx() < 2 && fLabelsTopX)
			drawopt += "X+";
		if (gPad->GetTicky() < 2 && fLabelsRightY)
			drawopt += "Y+";
		fSelHist->SetDrawOption(drawopt.Data());
	 //   fSelHist->DrawCopy();
	 //   fSelHist->Draw();
		if (fTitleCenterX)
			 fSelHist->GetXaxis()->CenterTitle(kTRUE);
		if (fTitleCenterY)
			 fSelHist->GetYaxis()->CenterTitle(kTRUE);
		fCanvas->GetFrame()->SetFillStyle(0);
	 //   Int_t save_optstat = gStyle->GetOptStat();
		if (fShowStatBox) {
			 gStyle->SetOptStat(fOptStat);
			 fSelHist->SetStats(1);
//			 cout << "fSelHist->SetStats(1); " << fOptStat << endl;
		} else {
			 fSelHist->SetStats(0);
//			 cout << "fSelHist->SetStats(0); " << endl;
		} 
   }
   if ( gDebug > 0 ) {
		cout << "Draw1Dim() " << drawopt << " fSelHist->Draw() " 
		<<fSelHist->GetDrawOption() 
		<< " logy " ;
		if (fCanvas->GetLogy())
			cout << " true";
		else
			cout << " false";
		cout <<  endl;
	}
	if (fShowMarkers == 0 && fText == 0)
		fSelHist->SetMarkerSize(0.01);
//   fSelHist->Draw(drawopt);
   TList *lof = fOrigHist->GetListOfFunctions();

   Double_t ymax  = fSelHist->GetMaximum();
   Double_t ymove = 0.05 * ymax;
   Double_t y0    = 6 * ymove;
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
         if (p->InheritsFrom(TMrbWindow::Class())) {
            TMrbWindow *w = (TMrbWindow *) p;
            w->SetParent(fSelHist);
            if (w->GetY1() == 0) {
               w->SetY1(y0);
               w->SetY2(y0);
               y0 += ymove;
               if (y0 >= ymax) y0 = 3 * ymove;
            }
            w->Draw();
         }
      }
//      DrawWindows();
   }
   if (GeneralAttDialog::fUseAttributeMacro) {
      ExecDefMacro();
//      fSelPad->Modified(kTRUE);
   }
   DrawDate();
	TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
	if ( st && GeneralAttDialog::fRememberStatBox ) {
		TEnv env(".hprrc");
		if ( env.Lookup("StatBox1D.fX1") ) {
			st->SetX1NDC(env.GetValue("StatBox1D.fX1", 0.7));
			st->SetX2NDC(env.GetValue("StatBox1D.fX2", 0.9));
			st->SetY1NDC(env.GetValue("StatBox1D.fY1", 0.74));
			st->SetY2NDC(env.GetValue("StatBox1D.fY2", 0.9));
			if (gDebug > 0) {
				cout << "Draw1Dim() StatBox1D.fY1, 2: " << st->GetY1NDC() << " " << st->GetY2NDC() << endl;
			}
		}	
	}
	//  add extra axis (channels) at top
   if (fDrawAxisAtTop) {
      if (st) {
         st->SetY1NDC(st->GetY1NDC()-0.15);
         st->SetY2NDC(st->GetY2NDC()-0.15);
      }
//      DrawTopAxis();

      TString cmd("((FitHist*)gROOT->FindObject(\"");
      cmd += GetName();
      cmd += "\"))->DrawTopAxis()";

      TExec * exec = new TExec("exec", cmd.Data());
      exec->Draw();

   }
   UpdateDrawOptions();
	if (fLogy)
		fCanvas->SetLogy();
	if (fLogx)
		fCanvas->SetLogx();
   fCanvas->Update();
	if (gDebug > 0) {
		cout << "exit Draw1Dim() " <<fSelHist->GetDrawOption()
		<< " logy " ;
		if (fCanvas->GetLogy())
			cout << " true";
		else
			cout << " false";
		cout <<  endl;
	}
//   gStyle->SetOptStat(save_optstat);
}
//____________________________________________________________________________

void FitHist::DrawDate()
{
   if (fShowDateBox ) {
      TDatime dt;
      TString dtext("Drawn at: ");
      UInt_t da = fSelHist->GetUniqueID();
      if (da != 0 && !fUseTimeOfDisplay) {
         dt.Set(da);
         dtext = "Created at: ";
      }
      dtext += dt.AsSQLString();
      fDateText = new TText(gStyle->GetDateX(),gStyle->GetDateY(),dtext);
      fDateText->SetName("	DateBox");	
      fDateText->SetTextSize( gStyle->GetAttDate()->GetTextSize());
      fDateText->SetTextFont( gStyle->GetAttDate()->GetTextFont());
      fDateText->SetTextColor(gStyle->GetAttDate()->GetTextColor());
      fDateText->SetTextAlign(gStyle->GetAttDate()->GetTextAlign());
      fDateText->SetTextAngle(gStyle->GetAttDate()->GetTextAngle());
      fDateText->SetNDC();
      fDateText->Draw();
   }
}
//____________________________________________________________________________

void FitHist::Draw2Dim()
{
   fCanvas->cd();
	if (gDebug > 0)
		cout << "Draw2Dim:" << " GetLogy " << fCanvas->GetLogy() << endl;
	if ( gDebug > 0 ) {
		cout << "FitHist::DrawOpt2Dim: " <<fDrawOpt2Dim 
		<< " gStyle->GetHistFillColor() :" <<gStyle->GetFillColor() << endl
		<< " fHistFillColor2Dim :" <<fHistFillColor2Dim<< endl
		<< " fHistFillStyle2Dim :" <<fHistFillStyle2Dim<< endl;
	}
   if (fShowStatBox) {
      gStyle->SetOptStat(fOptStat);
      fSelHist->SetStats(1);
//      cout << "fSelHist->SetStats(1); " << fOptStat << endl;
   } else {
      fSelHist->SetStats(0);
   } 
   fSelHist->Draw(fDrawOpt2Dim);
	if (gDebug > 0)
		cout << "Draw2Dim:" << " GetLogy " << fCanvas->GetLogy() << endl;
	if ( gROOT->GetForceStyle() ) {
		fCanvas->UseCurrentStyle();
	}
	fCanvas->SetLogx(fLogx);
	fCanvas->SetLogy(fLogy);
	SetLogz(fLogz);
	fCanvas->GetHandleMenus()->SetLog(fLogz);
	if (gDebug > 0)
		cout << "Draw2Dim:" << " GetLogy " << fCanvas->GetLogy() << endl;
	fSelHist->SetOption(fDrawOpt2Dim);
   fSelHist->SetDrawOption(fDrawOpt2Dim);
   if (fTitleCenterX)
      fSelHist->GetXaxis()->CenterTitle(kTRUE);
   if (fTitleCenterY)
      fSelHist->GetYaxis()->CenterTitle(kTRUE);
   if (fTitleCenterY)
		fSelHist->GetYaxis()->CenterTitle(kTRUE);
	fSelHist->SetFillColor(fHistFillColor2Dim);
	fSelHist->SetFillStyle(fHistFillStyle2Dim);
	fSelHist->SetLineColor(fHistLineColor2Dim);
	fSelHist->SetLineStyle(fHistLineStyle2Dim);
	fSelHist->SetLineWidth(fHistLineWidth2Dim);
	fSelHist->SetMarkerColor(fMarkerColor2Dim);  
	fSelHist->SetMarkerStyle(fMarkerStyle2Dim);  
	fSelHist->SetMarkerSize (fMarkerSize2Dim);   			
   DrawCut();
   TList *lof = fOrigHist->GetListOfFunctions();
   if (lof) {
      TObject *p;
      TIter next(lof);
      while ( (p = (TObject *) next()) ) {
//         p->Print();
         if (p->InheritsFrom(TF1::Class())) {
            TF1 *f =  dynamic_cast<TF1*>(p);
             f->Draw("same");
            f->SetParent(fSelHist);
         } else if (p->InheritsFrom(TMrbWindow2D::Class())) {
            TMrbWindow2D *w = dynamic_cast<TMrbWindow2D*>(p);
            w->Draw("same");
            w->SetParent(fSelHist);
            fActiveCuts->Add(w);
         } else if  (p->InheritsFrom(TMrbNamedArrayI::Class())) {
             TMrbNamedArrayI * nai = dynamic_cast<TMrbNamedArrayI*>(p);
             TString name(nai->GetName());
             if (name.BeginsWith("Pixel")) {
                cout << "SetUserPalette" << endl;
                SetUserPalette(1001, nai);
             }
         }
      }
   }
   if (GeneralAttDialog::fUseAttributeMacro) {
      ExecDefMacro();
      fSelPad->Modified(kTRUE);
   }
   UpdateCanvas();
   if (f2DimBackgroundColor == 0) {
      if (gStyle->GetCanvasColor() == 0) {
         fCanvas->GetFrame()->SetFillStyle(0);
      } else {
      	fCanvas->GetFrame()->SetFillStyle(1001);
      	fCanvas->GetFrame()->SetFillColor(10);
   	}
   } else {
      fCanvas->GetFrame()->SetFillStyle(1001);
      fCanvas->GetFrame()->SetFillColor(f2DimBackgroundColor);
   }
   DrawDate();
	fCanvas->Modified();
   fCanvas->Update();
	TPaveStats * st = (TPaveStats *)fCanvas->GetPrimitive("stats");
//   cout << "Draw2Dim: st " << st << endl;
	if ( st && GeneralAttDialog::fRememberStatBox ) {
		TEnv env(".hprrc");
		if ( env.Lookup("StatBox2D.fX1") ) {
			st->SetX1NDC(env.GetValue("StatBox2D.fX1", 0.78));
			st->SetX2NDC(env.GetValue("StatBox2D.fX2", 0.98));
			st->SetY1NDC(env.GetValue("StatBox2D.fY1", 0.835));
			st->SetY2NDC(env.GetValue("StatBox2D.fY2", 0.995));
			if (gDebug > 0)
			cout << "Draw2Dim: StatBox2D.fX1 " << env.GetValue("StatBox2D.fX1", 0.78)
			<< " fTwoDimLogY " << fTwoDimLogY<< endl;
			fCanvas->Modified();
		}	
	}
	if (!fCanvas->GetAutoExec())
		fCanvas->ToggleAutoExec();
   fCanvas->Update();
}

//__________________________________________________________________________________

void FitHist::AddFunctionsToHist()
{
   Int_t nval = CheckList(fAllFunctions);
   if (nval>0) {
      TF1 *fun;
      TIter next(fAllFunctions);
      while ( (fun = (TF1*)next()) ) {
         TF1 * funcp= new TF1(*fun);
         fSelHist->GetListOfFunctions()->Add(funcp);
         fCanvas->cd();
//         funcp->Draw("same");
         fun->Draw("same");
         fCanvas->Modified(kTRUE);
         fCanvas->Update();
      }
   } else Hpr::WarnBox("No function selected");
}
//______________________________________________________________________________________

void FitHist::SetLogz(Int_t state)
{
//	cout << "SetLogz(Int_t state) " << state<< endl;
   fLogz = state;
   if (gHpr && gHpr->fLogScaleMin > 0){
      if(state > 0)
         fSelHist->SetMinimum(gHpr->fLogScaleMin);
      else
         fSelHist->SetMinimum(gHpr->fLinScaleMin);
   }
   fCanvas->SetLogz(state);
//   SaveDefaults();
}
//______________________________________________________________________________________

void FitHist::UpdateDrawOptions()
{
//   if (!gHpr) return;
//   SetSelectedPad();
	fSelPad->cd();
   TString drawopt;
   if (fDimension == 1) {
/*   	if (fShowContour) {
      	drawopt = "";
         if (!HasFunctions(fSelHist)) {
            if (fSelHist->GetSumw2N()) {
      	      drawopt = "hist";
            }
         }
      }
      if ( fErrorMode != "none") {
         drawopt += fErrorMode;
         if (fErrorMode == "E1" && gStyle->GetMarkerSize() == 0) {
            fSelHist->SetMarkerSize(0.01);
         }
      }
      if (fShowContour)
         drawopt += "hist";
   	if (fFill1Dim && fSelHist->GetNbinsX() < 50000) {
      	fSelHist->SetFillStyle(fHistFillStyle);
      	fSelHist->SetFillColor(fHistFillColor);
   	} else
      	fSelHist->SetFillStyle(0);
      cout << "UpdateDrawOptions() " << drawopt.Data() << endl;
//      fSelHist->SetOption(drawopt.Data());
      fSelHist->SetDrawOption(drawopt.Data());*/
   } else if (fDimension  == 2) {
      fSelHist->SetOption(fDrawOpt2Dim);
      fSelHist->SetDrawOption(fDrawOpt2Dim);
   }
   if (gROOT->GetForceStyle()) {
      SetHistOptDialog::SetDefaults();
   }
//   gPad->Modified();
//   gPad->Update();
}
//______________________________________________________________________________________

void FitHist::ColorMarked()
{
   if (fSelHist->GetDimension() != 1) {
      cout << "Currently only 1-dim supported" << endl;
      return;
   }
   GetLimits();
   Int_t color = getcol();
   if (color <= 0) return;
   TGraph * pl = PaintArea(fSelHist, fBinX_1, fBinX_2, color);
   if (pl) {
      fCanvas->cd();
      pl->Draw("LF");
      fSelHist->GetListOfFunctions()->Add(pl, "LF");
   }
   fCanvas->Modified();
   fCanvas->Update();
}
//____________________________________________________________________________________

void FitHist::Fit2DimD(Int_t type)
{
   if (fFit2DimD == NULL)
      fFit2DimD = new Fit2DimDialog((TH2*)fSelHist, type);
}
//____________________________________________________________________________________

void FitHist::Fit1DimDialog(Int_t type)
{
   if (fFit1DimD == NULL)
      fFit1DimD = new FitOneDimDialog(fSelHist, type);
}
///__________________________________________________________________

void FitHist::FindPeaks()
{
   new FindPeakDialog(fSelHist);
};

//__________________________________________________________________

void FitHist::Calibrate()
{
      new CalibrationDialog(fSelHist, 1);
};
//__________________________________________________________________

void FitHist::SetLogx(Int_t state)
{
	fLogx = state;
	fCanvas->SetLogx(state);
	fCanvas->Modified();
	fCanvas->Update();
};
//__________________________________________________________________

void FitHist::SetLogy(Int_t state)
{
	fLogy = state;
	fCanvas->SetLogy(state);
	fCanvas->Modified();
	fCanvas->Update();
};
//    Int_t   fBinX_1, fBinX_2, fBinY_1, fBinY_2;  // lower, upper bin selected
//    Int_t   fBinlx, fBinux, fBinly, fBinuy;     // lower, upper bin in expanded
//    Axis_t  fExplx, fExpux, fExply, fExpuy;     // lower, upper edge in expanded
//    Axis_t  fX_1,   fX_2,   fY_1,   fY_2;     // lower, upper lim selected
//    Axis_t  fRangeLowX, fRangeUpX, fRangeLowY, fRangeUpY;
//    Axis_t  fOrigLowX, fOrigUpX, fOrigLowY, fOrigUpY;
// 	Double_t fFrameX1, fFrameX2,fFrameY1,fFrameY2;
//    Bool_t fSetRange;
//    Bool_t fKeepParameters;
//    Bool_t fCallMinos;
//    Int_t  fOldMode;
//    TH1    *fSelHist, *fOrigHist;          // pointer to the selected histogram
//    TH1    *fCalHist;
//    FitHist  *fCalFitHist;
//    TF1      *fCalFunc;
//    FitOneDimDialog *fFit1DimD;
//    Fit2DimDialog *fFit2DimD;
// //   HistPresent* hp;
//    HTCanvas *fCanvas;
//    TH1     *fExpHist, *fProjHistX, *fProjHistY;
//    TString fHname;
//    TString fCname;
//    TString fCtitle;
//    TString fEname;
//    TString fCutname;
// 
//    TVirtualPad *fSelPad;          // pointer to the selected pad
// 
//    TList *fActiveFunctions;
//    TList *fAllFunctions;
//    TList *fActiveWindows;
//    TList *fAllWindows;
//    TList *fAllCuts;
//    TList *fPeaks;
//    TList *fActiveCuts;
//    FhMarkerList *fMarkers;
//    TObjArray *peaks;
//    TList *fCmdLine;
//    Int_t fExpInd;
//    Int_t fSerialPx;
//    Int_t fSerialPy;
//    Int_t fSerialPf;
//    Int_t fSerialRot;
//    Int_t fFuncNumb;
//    Int_t fCutNumber;
// 	Int_t fProjectedBoth;
//    Int_t wdw_numb;
//    Int_t fColSuperimpose;
//    void ExpandProject(Int_t);
//    Int_t fLogx;
//    Int_t fLogy;
//    Int_t fLogz;
//    Bool_t fSelInside;
//    Int_t fUserContourLevels;
//    Bool_t fSetLevels;
//    Bool_t fSetColors;
//    Bool_t fHasExtraAxis;
// 
//    Float_t fMax, fMin, fXmax, fXmin, fYmax, fYmin;
//    TRootCanvas *mycanvas;
// //   TPaveText *datebox;
//    Int_t fDimension;
//    Bool_t fCanvasIsAlive;
//    TString fXtitle;
//    TString fYtitle;
// 	TString fZtitle;
// 	TString fFitMacroName;
//    TString fTemplateMacro;
//    TString fFitSliceYMacroName;
//    Int_t fFirstUse;
//    Bool_t fDeleteCalFlag;
//    HTCanvas * fCutPanel;
//    TableOfLabels * fTofLabels;
// 
//    Int_t fFill1Dim;
//    Color_t fFillColor;
//    Color_t fLineColor;
//    Float_t fLineWidth;
//    Float_t fMarkerSize;
// 	Int_t fShowMarkers;
// 	Int_t fFillStyle;
//    Int_t fShowContour;
//    Int_t fShowDateBox;
//    Int_t fShowStatBox;
//    Int_t fOptStat;
//    Int_t fUseTimeOfDisplay;
//    Int_t fShowTitle;
//    Int_t fShowFitBox;
//    Int_t fLiveStat1Dim;
//    Int_t fLiveStat2Dim;
//    Int_t fLiveGauss;
//    Int_t fLiveBG;
// 	Int_t   fSmoothLine;
// 	Int_t   fSimpleLine;
// 	Int_t   fBarChart;
// 	Int_t   fBarChart3D;
// 	Int_t   fBarChartH;
// 	Int_t   fPieChart;
// 	Int_t   fText;
// 	Int_t   fTextAngle;
//    Int_t fShowZScale;
//    Int_t fDrawAxisAtTop;
//    TString fDrawOpt2Dim;
//    Int_t fTitleCenterX;
//    Int_t fTitleCenterY;
//    Int_t fTitleCenterZ;
//    TText * fDateText;
//    Save2FileDialog *fDialog;
//    Color_t fHistFillColor2Dim;
// 	Color_t fHistFillStyle2Dim;
// 	Color_t fHistLineColor2Dim;
// 	Color_t fHistLineStyle2Dim;
// 	Color_t fHistLineWidth2Dim;
// 	Color_t fMarkerColor2Dim; 
// 	Color_t f2DimBackgroundColor;
//    Style_t fMarkerStyle2Dim;  
//    Size_t  fMarkerSize2Dim; 
//    TString fErrorMode;
// 	Int_t   fOneDimLogX;
// 	Int_t   fOneDimLogY;
// 	Int_t   fTwoDimLogX;
// 	Int_t   fTwoDimLogY;
// 	Int_t   fTwoDimLogZ;
// 	Int_t   fLabelsTopX;
// 	Int_t   fLabelsRightY;
// 	TString fDrawOpt3Dim;
//    Color_t fHistFillColor3Dim;
//    Color_t fHistLineColor3Dim;
//    Color_t fMarkerColor3Dim; 
// 	Color_t f3DimBackgroundColor;
//    Style_t fMarkerStyle3Dim;  
//    Size_t  fMarkerSize3Dim; 
