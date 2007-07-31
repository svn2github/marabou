
#define ENABLE_CURLYDEFAULTS 40304

#include "TROOT.h"
#include "TEnv.h"
#include "TColor.h"
#include "TRootCanvas.h"
#include "TGaxis.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TObjString.h"
#include "TFrame.h"
#include "TF1.h"
#include "TPaveStats.h"
#include "TPaveLabel.h"
#include "TArrow.h"
#include "TArc.h"
#include "TMarker.h"
#include "TLatex.h"
#include "TCurlyArc.h"
#include "TCurlyLine.h"
#include "Buttons.h"

#include "TGMrbValuesAndText.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "TMrbHelpBrowser.h"
#include "support.h"

#include <iostream>
#include <fstream>
#include <sstream>

//_______________________________________________________________________

void HistPresent::RestoreOptions()
{
   TEnv env(".rootrc");
   fWintopx = 515;              // origin of window
   fWintopy = 5;
   fWinwidx_1dim = 750;
   fWinwidy_1dim = 550;
   fWinwidx_2dim = 750;
   fWinwidy_2dim = 750;
//   fWinwidy_2dim = 750 + 28 + 20;  // 28 for top, 20 for eventstatus
   fWinshifty = 30;
   fWinshiftx = 0;
   fWinwidx_hlist = 0; 
   fWinwidx_2dim =
       env.GetValue("HistPresent.WindowXWidth_2dim", fWinwidx_2dim);
   fWinwidy_2dim =
       env.GetValue("HistPresent.WindowYWidth_2dim", fWinwidy_2dim);
   fWinwidx_1dim =
       env.GetValue("HistPresent.WindowXWidth_1dim", fWinwidx_1dim);
   fWinwidy_1dim =
       env.GetValue("HistPresent.WindowYWidth_1dim", fWinwidy_1dim);
   if (fWinwidx_1dim <= 0)
      fWinwidx_1dim = 750;
   if (fWinwidy_1dim <= 0)
      fWinwidy_1dim = 550;
   if (fWinwidx_2dim <= 0)
      fWinwidx_2dim = 750;
   if (fWinwidy_2dim <= 0)
      fWinwidy_2dim = 750;
   fWinwidx_hlist = env.GetValue("HistPresent.WindowXWidth_List", fWinwidx_hlist);
   fWintopx = env.GetValue("HistPresent.WindowX", fWintopx);
   fWintopy = env.GetValue("HistPresent.WindowY", fWintopy);
   fWincurx = fWintopx;
   fWincury = fWintopy;
   fWinshiftx = env.GetValue("HistPresent.WindowXShift", fWinshiftx);
   fWinshifty = env.GetValue("HistPresent.WindowYShift", fWinshifty);
   fMainWidth = env.GetValue("HistPresent.MainWidth", 250);
   if (fMainWidth < 100 || fMainWidth > 1000) fMainWidth = 250;
   fGraphFile =
       env.GetValue("HistPresent.GraphFile", "gdata.asc");
   fDrawOptGraph =
       env.GetValue("HistPresent.DrawOptGraph", "A*");
   *fDrawOpt2Dim =
       env.GetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim->Data());
   *f2DimColorPalette =
       env.GetValue("HistPresent.ColorPalette", f2DimColorPalette->Data());
   fShowErrors    = env.GetValue("HistPresent.ShowErrors", fShowErrors);
   fDrawAxisAtTop = env.GetValue("HistPresent.DrawAxisAtTop", fDrawAxisAtTop);
   fFill1Dim      = env.GetValue("HistPresent.Fill1Dim", fFill1Dim);
   fShowContour   = env.GetValue("HistPresent.ShowContour", fShowContour);
   fOptStat       = env.GetValue("HistPresent.OptStat", 1001111);
   fShowDateBox   = env.GetValue("HistPresent.ShowDateBox", 21);
   fShowStatBox   = env.GetValue("HistPresent.ShowStatBox", 1);
   fUseTimeOfDisplay  = env.GetValue("HistPresent.UseTimeOfDisplay", 0);
   fShowTitle         = env.GetValue("HistPresent.ShowTitle", 1);
   fEnableCalibration = env.GetValue("HistPresent.EnableCalibration", 0);
   fShowFitBox        = env.GetValue("HistPresent.ShowFitBox", 1);
   fShowFittedCurves  = env.GetValue("HistPresent.ShowFittedCurves", 1);
   fShowPSFile        = env.GetValue("HistPresent.AutoShowPSFile", 0);
   fShowListsOnly     = env.GetValue("HistPresent.ShowListsOnly", 0);
   fRememberTreeHists = env.GetValue("HistPresent.RememberTreeHists", 0);
   fNtupleVersioning  = env.GetValue("HistPresent.NtupleVersioning", 1);
   fAlwaysNewLimits   = env.GetValue("HistPresent.AlwaysNewLimits", 0);
   fRememberLastSet   = env.GetValue("HistPresent.RememberLastSet", 1);
   fRememberZoom      = env.GetValue("HistPresent.RememberZoom", 1);
   fUseAttributeMacro = env.GetValue("HistPresent.UseAttributeMacro", 0);
   fShowAllAsFirst    = env.GetValue("HistPresent.ShowAllAsFirst", 0);
   fRealStack         = env.GetValue("HistPresent.RealStack", 1);
   fUseRegexp         = env.GetValue("HistPresent.UseRegexp", 0);
   fProjectBothRatio  = env.GetValue("HistPresent.ProjectBothRatio", 0.6);
   fDivMarginX        = env.GetValue("HistPresent.DivMarginX", 0.001);
   fDivMarginY        = env.GetValue("HistPresent.DivMarginY", 0.001);
   fLogScaleMin = atof(env.GetValue("HistPresent.LogScaleMin", "0.1"));
   fLinScaleMin = atof(env.GetValue("HistPresent.LinScaleMin", "0"));
   fAutoUpdateDelay  = env.GetValue("HistPresent.AutoUpdateDelay", 2);
   fPeakMwidth       = env.GetValue("HistPresent.fPeakMwidth", 11);
   fPeakThreshold    = env.GetValue("HistPresent.fPeakThreshold", 3.);
   fLiveStat1dim     = env.GetValue("HistPresent.LiveStat1dim", 0);
   fLiveStat2dim     = env.GetValue("HistPresent.LiveStat2dim", 0);
   fLiveGauss        = env.GetValue("HistPresent.LiveGauss", 0);
   fLiveBG           = env.GetValue("HistPresent.LiveBG", 0);

   fNofTransLevels   = env.GetValue("HistPresent.NofTransLevels", 20);
   fStartColor       = env.GetValue("HistPresent.StartColor", 2);
   fEndColor         = env.GetValue("HistPresent.EndColor", 3);
   fEnhenceRed       = env.GetValue("HistPresent.EnhenceRed", 1.);
   fEnhenceGreen     = env.GetValue("HistPresent.EnhenceGreen", 1.);
   fEnhenceBlue      = env.GetValue("HistPresent.EnhenceBlue", 1.);
   fStartHue         = env.GetValue("HistPresent.StartHue", 120);
   fEndHue           = env.GetValue("HistPresent.EndHue", 1);
   fLightness        = env.GetValue("HistPresent.Lightness", 0.5);
   fSaturation       = env.GetValue("HistPresent.Saturation ", 1.);

   f2DimBackgroundColor =
       env.GetValue("HistPresent.2DimBackgroundColor", 0);
   fStatFont = env.GetValue("HistPresent.StatBoxFont", 40);
   if (fTextFont < 12 || fTextFont > 123) fTextFont = 62;
   fTitleFont = env.GetValue("HistPresent.TitleFont", 62);
   if (fTitleFont < 12 || fTitleFont > 123) fTitleFont = 62;
   fMaxListEntries= env.GetValue("HistPresent.MaxListEntries", 333);
   fDisplayCalibrated= env.GetValue("HistPresent.DisplayCalibrated", 1);
   *fHostToConnect =
       env.GetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   fSocketToConnect =
       env.GetValue("HistPresent.SocketToConnect", fSocketToConnect);

   fLabelMaxDigits=  env.GetValue("HistPresent.LabelMaxDigits",  5);  

   fNdivisionsX= env.GetValue("HistPresent.NdivisionsX", 510); 
   fAxisColorX=  env.GetValue("HistPresent.AxisColorX",  1);  
   fLabelColorX= env.GetValue("HistPresent.LabelColorX", 1); 
   fLabelFontX=  env.GetValue("HistPresent.LabelFontX",  62);  
   if (fTextFont < 12 || fTextFont > 123) fTextFont = 62;
   fLabelOffsetX=env.GetValue("HistPresent.LabelOffsetX",0.005);
   fLabelSizeX=  env.GetValue("HistPresent.LabelSizeX",  0.03);  
   fTickLengthX= env.GetValue("HistPresent.TickLengthX", 0.03); 
   fTitleOffsetX=env.GetValue("HistPresent.TitleOffsetX",1.);
   fTitleSizeX=  env.GetValue("HistPresent.TitleSizeX",  0.03);  
   fTitleColorX= env.GetValue("HistPresent.TitleColorX", 1); 
   fTitleFontX=  env.GetValue("HistPresent.TitleFontX",   62);  
   if (fTitleFontX < 12 || fTitleFontX > 123) fTitleFontX = 62;

   fNdivisionsY= env.GetValue("HistPresent.NdivisionsY", 510); 
   fAxisColorY=  env.GetValue("HistPresent.AxisColorY",  1);  
   fLabelColorY= env.GetValue("HistPresent.LabelColorY", 1); 
   fLabelFontY=  env.GetValue("HistPresent.LabelFontY",  62);  
   if (fLabelFontY < 12 || fLabelFontY > 123) fLabelFontY = 62;
   fLabelOffsetY=env.GetValue("HistPresent.LabelOffsetY",0.005);
   fLabelSizeY=  env.GetValue("HistPresent.LabelSizeY",  0.03);  
   fTickLengthY= env.GetValue("HistPresent.TickLengthY", 0.03); 
   fTitleOffsetY=env.GetValue("HistPresent.TitleOffsetY",1);
   fTitleSizeY=  env.GetValue("HistPresent.TitleSizeY",  0.03);  
   fTitleColorY= env.GetValue("HistPresent.TitleColorY", 1); 
   fTitleFontY=  env.GetValue("HistPresent.TitleFontY",   62);  
   if (fTitleFontY < 12 || fTitleFontY > 123) fTitleFontY = 62;

   fNdivisionsZ= env.GetValue("HistPresent.NdivisionsZ", 510); 
   fAxisColorZ=  env.GetValue("HistPresent.AxisColorZ",  1);  
   fLabelColorZ= env.GetValue("HistPresent.LabelColorZ", 1); 
   fLabelFontZ=  env.GetValue("HistPresent.LabelFontZ",  62);  
   if (fLabelFontZ < 12 || fLabelFontZ > 123) fLabelFontZ = 62;
   fLabelOffsetZ=env.GetValue("HistPresent.LabelOffsetZ",0.005);
   fLabelSizeZ=  env.GetValue("HistPresent.LabelSizeZ",  0.03);  
   fTickLengthZ= env.GetValue("HistPresent.TickLengthZ", 0.03); 
   fTitleOffsetZ=env.GetValue("HistPresent.TitleOffsetZ",1.);
   fTitleSizeZ=  env.GetValue("HistPresent.TitleSizeZ",  0.03);  
   fTitleColorZ= env.GetValue("HistPresent.TitleColorZ", 1); 
   fTitleFontZ=  env.GetValue("HistPresent.TitleFontZ",   62);  
   if (fTitleFontZ < 12 || fTitleFontZ > 123) fTitleFontZ = 62;

   fHistFillColor=env.GetValue("HistPresent.HistFillColor", 0);
   fHistLineColor=env.GetValue("HistPresent.HistLineColor", 1);
   fHistFillStyle=env.GetValue("HistPresent.HistFillStyle", 0);
   fHistLineStyle=env.GetValue("HistPresent.HistLineStyle", 1);
   fHistLineWidth=env.GetValue("HistPresent.HistLineWidth", 2);
   fEndErrorSize= env.GetValue("HistPresent.EndErrorSize ", 1.);
   fErrorX=       env.GetValue("HistPresent.ErrorX",       0.0);
   fFuncColor=    env.GetValue("HistPresent.FuncColor", 4);
   fFuncStyle=    env.GetValue("HistPresent.FuncStyle", 1);
   fFuncWidth=    env.GetValue("HistPresent.FuncWidth", 2);

   fStatColor=     env.GetValue("HistPresent.StatColor", 19);
   fStatTextColor= env.GetValue("HistPresent.StatTextColor", 1);
   fStatBorderSize=env.GetValue("HistPresent.StatBorderSize", 1);
   fStatFont=      env.GetValue("HistPresent.StatFont", 62);
   if (fStatFont < 12 || fStatFont > 123) fStatFont = 62;
   fStatFontSize=  env.GetValue("HistPresent.StatFontSize", 0.02);
   fStatStyle=     env.GetValue("HistPresent.StatStyle", 1001);
   fStatX=         env.GetValue("HistPresent.StatX", 0.98);
   fStatY=         env.GetValue("HistPresent.StatY", 0.995);
   fStatW=         env.GetValue("HistPresent.StatW", 0.2);
   fStatH=         env.GetValue("HistPresent.StatH", 0.16);

   fTitleColor     = env.GetValue("HistPresent.TitleColor",     0);
   fTitleTextColor = env.GetValue("HistPresent.TitleTextColor", 1);
   fTitleBorderSize= env.GetValue("HistPresent.TitleBorderSize",1);
   fTitleFont      = env.GetValue("HistPresent.TitleFont",      62);
   if (fTitleFont < 12 || fTitleFont > 123) fTitleFont = 62;
   fTitleFontSize  = env.GetValue("HistPresent.TitleFontSize",  0.02);
   fTitleStyle     = env.GetValue("HistPresent.TitleStyle",     1001);
   fTitleX         = env.GetValue("HistPresent.TitleX",         0.01);
   fTitleY         = env.GetValue("HistPresent.TitleY",         .995);
   fTitleW         = env.GetValue("HistPresent.TitleW",         0.);
   fTitleH         = env.GetValue("HistPresent.TitleH",         0.);
   fTitleAlign     = env.GetValue("HistPresent.TitleAlign",    13);

   fPadColor       = env.GetValue("HistPresent.PadColor",       19);
   fPadBorderSize  = env.GetValue("HistPresent.PadBorderSize",  2);
   fPadBorderMode  = env.GetValue("HistPresent.PadBorderMode",  1);
   fPadBottomMargin= env.GetValue("HistPresent.PadBottomMargin",0.1);
   fPadTopMargin   = env.GetValue("HistPresent.PadTopMargin",   0.1);
   fPadLeftMargin  = env.GetValue("HistPresent.PadLeftMargin",  0.1);
   fPadRightMargin = env.GetValue("HistPresent.PadRightMargin", 0.1);
   fPadGridX       = env.GetValue("HistPresent.PadGridX",       0);
   fPadGridY       = env.GetValue("HistPresent.PadGridY",       0);
   fPadTickX       = env.GetValue("HistPresent.PadTickX",       0);
   fPadTickY       = env.GetValue("HistPresent.PadTickY",       0);
	fFrameFillColor   = env.GetValue("HistPresent.FrameFillColor",  0);  
	fFrameLineColor   = env.GetValue("HistPresent.FrameLineColor",  1);  
	fFrameFillStyle   = env.GetValue("HistPresent.FrameFillStyle",  0);  
	fFrameLineStyle   = env.GetValue("HistPresent.FrameLineStyle",  0);  
	fFrameLineWidth   = env.GetValue("HistPresent.FrameLineWidth",  1);  
	fFrameBorderSize  = env.GetValue("HistPresent.FrameBorderSize", 1); 
	fFrameBorderMode  = env.GetValue("HistPresent.FrameBorderMode", 0);
	fCanvasColor  	 	= env.GetValue("HistPresent.CanvasColor",     0);
	fCanvasBorderSize = env.GetValue("HistPresent.CanvasBorderSize",1);
	fCanvasBorderMode = env.GetValue("HistPresent.CanvasBorderMode",0);
	fCanvasDefH		 	= env.GetValue("HistPresent.CanvasDefH",	    0);  
	fCanvasDefW		   = env.GetValue("HistPresent.CanvasDefW",	    0);  
	fCanvasDefX		 	= env.GetValue("HistPresent.CanvasDefX",	    0);
	fCanvasDefY		 	= env.GetValue("HistPresent.CanvasDefY",		 0);
	fMarkerColor      = env.GetValue("HistPresent.MarkerColor",       1);
	fMarkerStyle      = env.GetValue("HistPresent.MarkerStyle",       7);
	fMarkerSize       = env.GetValue("HistPresent.MarkerSize",      1.);
	fLineColor        = env.GetValue("HistPresent.LineColor",       1);
	fLineStyle        = env.GetValue("HistPresent.LineStyle",       1);
	fLineWidth        = env.GetValue("HistPresent.LineWidth",       1);
	fTextSize         = env.GetValue("HistPresent.TextSize" ,    0.02);
	fTextAngle        = env.GetValue("HistPresent.TextAngle" ,      0);
	fTextAlign        = env.GetValue("HistPresent.TextAlign",       1);
	fTextColor        = env.GetValue("HistPresent.TextColor",       1);
	fTextFont         = env.GetValue("HistPresent.TextFont" ,       62);
   if (fTextFont < 12 || fTextFont > 123) fTextFont = 62;
	fFillColor        = env.GetValue("HistPresent.FillColor",       8);
	fFillStyle        = env.GetValue("HistPresent.FillStyle",       1);
	fArrowAngle       = env.GetValue("HistPresent.ArrowAngle" ,      60.);
	fArrowSize        = env.GetValue("HistPresent.ArrowSize" ,      0.05);
   fArrowColor       = env.GetValue("HistPresent.ArrowColor" ,        1);
   fArrowWidth       = env.GetValue("HistPresent.ArrowWidth" ,        1);
   fArrowStyle       = env.GetValue("HistPresent.ArrowStyle" ,        1);
	fArrowFill        = env.GetValue("HistPresent.ArrowFill" ,      1001);
	fArrowShape       = env.GetValue("HistPresent.ArrowShape" ,    1);
	fCurlyWaveLength  = env.GetValue("HistPresent.CurlyWaveLength",0.025);
	fCurlyAmplitude   = env.GetValue("HistPresent.CurlyAmplitude", 0.01);
   fCurlyColor       = env.GetValue("HistPresent.CurlyColor" ,       1);
   fCurlyWidth       = env.GetValue("HistPresent.CurlyWidth" ,       1);
   fCurlyStyle       = env.GetValue("HistPresent.CurlyStyle" ,       1);
	fIsCurly          = env.GetValue("HistPresent.IsCurly" ,          0);

	fArcFillColor     = env.GetValue("HistPresent.ArcFillColor" , 1);
	fArcFillStyle     = env.GetValue("HistPresent.ArcFillStyle" , 0);
   fArcLineColor     = env.GetValue("HistPresent.ArcLineColor" , 1);
   fArcLineStyle     = env.GetValue("HistPresent.ArcLineStyle" , 1);
   fArcLineWidth     = env.GetValue("HistPresent.ArcLineWidth" , 2);

   fEditUsXlow   = env.GetValue("HistPresent.EditUsXlow",    550);
   fEditUsYlow   = env.GetValue("HistPresent.EditUsYlow",    5  );
   fEditUsXwidth = env.GetValue("HistPresent.EditUsXwidth",  500);
   fEditUsYwidth = env.GetValue("HistPresent.EditUsYwidth",  524);
   fEditUsXRange = env.GetValue("HistPresent.EditUsXRange",  100);
//  numbers below are for 1600 x 1200 screens
   Int_t  screen_x, screen_y;
   UInt_t wwu, whu;
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
            	  screen_x, screen_y, wwu, whu);
   Float_t fac = (Float_t)wwu / 1600.;

   fEditLsXlow   = env.GetValue("HistPresent.EditLsXlow",   (Int_t)(fac * 350.));
   fEditLsYlow   = env.GetValue("HistPresent.EditLsYlow",   5   );
   fEditLsXwidth = env.GetValue("HistPresent.EditLsXwidth", (Int_t)(fac * 1004.));
   fEditLsYwidth = env.GetValue("HistPresent.EditLsYwidth", (Int_t)(fac * 745.) + 24);
   fEditLsXRange = env.GetValue("HistPresent.EditLsXRange", 262 );

   fEditPoXlow   = env.GetValue("HistPresent.EditPoXlow",   (Int_t)(fac * 550.));
   fEditPoYlow   = env.GetValue("HistPresent.EditPoYlow",   5   );
   fEditPoXwidth = env.GetValue("HistPresent.EditPoXwidth", (Int_t)(fac * 690.));
   fEditPoYwidth = env.GetValue("HistPresent.EditPoYwidth", (Int_t)(fac * 1028.) + 24);
   fEditPoXRange = env.GetValue("HistPresent.EditPoXRange", 175 );

   fForceStyle     = env.GetValue("HistPresent.ForceStyle",    0);

   TString defdir(gSystem->Getenv("MARABOU"));
   defdir += "/doc/hpr";
   *fHelpDir =
       env.GetValue("HistPresent.HelpDir",defdir.Data());
   if (!gSystem->AccessPathName(fHelpDir->Data()))
      fHelpBrowser = new TMrbHelpBrowser(fHelpDir->Data());
   else
      fHelpBrowser = NULL;
   fGreyPalette = NULL;
   fGreyPaletteInv = NULL;
   fTransPaletteRGB = NULL;
   fTransPaletteHLS = NULL;
   fNofGreyLevels = 20;
   SetGreyLevels();
   SetTransLevelsRGB();
   SetTransLevelsHLS();
   SetColorPalette();
//   CheckAutoExecFiles();
//
   if (fShowStatBox) {
      gStyle->SetOptStat(fOptStat);
      gStyle->SetOptFit(fShowFitBox);
   } else {
      gStyle->SetOptFit(0);
      gStyle->SetOptStat(0);
   }
//   if (fShowDateBox) {
//      gStyle->SetOptDate(fShowDateBox);
//   } else {
//      gStyle->SetOptDate(0);
//   }
   gStyle->SetOptTitle(fShowTitle);
//   gStyle->SetStatFont(fStatFont);
//   gStyle->SetTitleFont(fTitleFont);
   SetAxisAtt();
   SetHistAtt();
   SetTitleAtt();
   SetStatAtt();
   SetPadAtt();
   SetGeneralAtt();
   SetCurlyAtt();
   if (fForceStyle > 0) gROOT->ForceStyle();
   else                 gROOT->ForceStyle(kFALSE);

   gEnv->SetValue("Root.Stacktrace", 0);
//   cout << "RestoreOptions,  fUseAttributeMacro " << fUseAttributeMacro << endl;
}

//_______________________________________________________________________
void SetIntValue(TEnv & env, const Char_t * name, Int_t value)
{
   env.SetValue(name, Form("%d", value));
}

//_______________________________________________________________________

void HistPresent::SaveOptions()
{
   TEnv env(".rootrc");
   env.SetValue("HistPresent.WindowXWidth_2dim", fWinwidx_2dim);
   env.SetValue("HistPresent.WindowYWidth_2dim", fWinwidy_2dim);
   env.SetValue("HistPresent.WindowXWidth_1dim", fWinwidx_1dim);
   env.SetValue("HistPresent.WindowYWidth_1dim", fWinwidy_1dim);
   env.SetValue("HistPresent.WindowXShift", fWinshiftx);
   env.SetValue("HistPresent.WindowYShift", fWinshifty);
   env.SetValue("HistPresent.MainWidth", fMainWidth);
   env.SetValue("HistPresent.WindowX", fWintopx);
   env.SetValue("HistPresent.WindowY", fWintopy);
   env.SetValue("HistPresent.WindowXWidth_List", fWinwidx_hlist);
   env.SetValue("HistPresent.ShowErrors", fShowErrors);
   env.SetValue("HistPresent.DrawAxisAtTop", fDrawAxisAtTop);
   env.SetValue("HistPresent.Fill1Dim", fFill1Dim);
   env.SetValue("HistPresent.NofTransLevels", fNofTransLevels);
   env.SetValue("HistPresent.StartColor", fStartColor);
   env.SetValue("HistPresent.EndColor", fEndColor);
   env.SetValue("HistPresent.EnhenceRed",fEnhenceRed);
   env.SetValue("HistPresent.EnhenceGreen",fEnhenceGreen);
   env.SetValue("HistPresent.EnhenceBlue",fEnhenceBlue);
   env.SetValue("HistPresent.StartHue",fStartHue);
   env.SetValue("HistPresent.EndHue",fEndHue);
   env.SetValue("HistPresent.Lightness", fLightness);
   env.SetValue("HistPresent.Saturation",fSaturation);
   env.SetValue("HistPresent.2DimBackgroundColor", f2DimBackgroundColor);
   env.SetValue("HistPresent.StatBoxFont", fStatFont);
   env.SetValue("HistPresent.TitleBoxFont", fTitleFont);
   env.SetValue("HistPresent.ShowContour", fShowContour);
   env.SetValue("HistPresent.OptStat", fOptStat);
   env.SetValue("HistPresent.ShowDateBox", fShowDateBox);
   env.SetValue("HistPresent.ShowStatBox", fShowStatBox);
   env.SetValue("HistPresent.UseTimeOfDisplay", fUseTimeOfDisplay);
   env.SetValue("HistPresent.ShowTitle", fShowTitle);
   env.SetValue("HistPresent.EnableCalibration", fEnableCalibration);
   env.SetValue("HistPresent.ShowFitBox", fShowFitBox);
   env.SetValue("HistPresent.ShowFittedCurves", fShowFittedCurves);
   env.SetValue("HistPresent.AutoShowPSFile", fShowPSFile);
   env.SetValue("HistPresent.ShowListsOnly", fShowListsOnly);
   env.SetValue("HistPresent.RememberTreeHists", fRememberTreeHists);
   env.SetValue("HistPresent.AlwaysNewLimits", fAlwaysNewLimits);
   env.SetValue("HistPresent.NtupleVersioning", fNtupleVersioning);
   env.SetValue("HistPresent.RememberLastSet", fRememberLastSet);
   env.SetValue("HistPresent.RememberZoom", fRememberZoom);
   env.SetValue("HistPresent.UseAttributeMacro", fUseAttributeMacro);
   env.SetValue("HistPresent.ShowAllAsFirst",    fShowAllAsFirst);
   env.SetValue("HistPresent.RealStack",    fRealStack);
   env.SetValue("HistPresent.UseRegexp", fUseRegexp);
   env.SetValue("HistPresent.MaxListEntries", fMaxListEntries);
   env.SetValue("HistPresent.DisplayCalibrated", fDisplayCalibrated);
//  char options
   env.SetValue("HistPresent.ColorPalette", f2DimColorPalette->Data());
   env.SetValue("HistPresent.GraphFile", fGraphFile.Data());
   env.SetValue("HistPresent.DrawOptGraph", fDrawOptGraph.Data());
   env.SetValue("HistPresent.DrawOpt2Dim", fDrawOpt2Dim->Data());
   env.SetValue("HistPresent.HostToConnect", fHostToConnect->Data());
   env.SetValue("HistPresent.SocketToConnect", fSocketToConnect);
   env.SetValue("HistPresent.DivMarginX", fDivMarginX);
   env.SetValue("HistPresent.DivMarginY", fDivMarginY);
   env.SetValue("HistPresent.ProjectBothRatio", fProjectBothRatio);
   env.SetValue("HistPresent.LinScaleMin", fLinScaleMin);
   env.SetValue("HistPresent.LogScaleMin", fLogScaleMin);
   env.SetValue("HistPresent.AutoUpdateDelay", fAutoUpdateDelay);
   env.SetValue("HistPresent.fPeakMwidth", fPeakMwidth);
   env.SetValue("HistPresent.fPeakThreshold", fPeakThreshold);
   env.SetValue("HistPresent.LiveStat1dim", fLiveStat1dim);
   env.SetValue("HistPresent.LiveStat2dim", fLiveStat2dim);
   env.SetValue("HistPresent.LiveGauss", fLiveGauss);
   env.SetValue("HistPresent.LiveBG", fLiveBG);

   env.SetValue("HistPresent.LabelMaxDigits", fLabelMaxDigits);  
   env.SetValue("HistPresent.NdivisionsX", fNdivisionsX); 
   env.SetValue("HistPresent.AxisColorX",  fAxisColorX);  
   env.SetValue("HistPresent.LabelColorX", fLabelColorX); 
   env.SetValue("HistPresent.LabelFontX",  fLabelFontX);  
   env.SetValue("HistPresent.LabelOffsetX",fLabelOffsetX);
   env.SetValue("HistPresent.LabelSizeX",  fLabelSizeX);  
   env.SetValue("HistPresent.TickLengthX", fTickLengthX); 
   env.SetValue("HistPresent.TitleOffsetX",fTitleOffsetX);
   env.SetValue("HistPresent.TitleSizeX",  fTitleSizeX);  
   env.SetValue("HistPresent.TitleColorX", fTitleColorX); 
   env.SetValue("HistPresent.TitleFontX",   fTitleFontX);  

   env.SetValue("HistPresent.NdivisionsY", fNdivisionsY); 
   env.SetValue("HistPresent.AxisColorY",  fAxisColorY);  
   env.SetValue("HistPresent.LabelColorY", fLabelColorY); 
   env.SetValue("HistPresent.LabelFontY",  fLabelFontY);  
   env.SetValue("HistPresent.LabelOffsetY",fLabelOffsetY);
   env.SetValue("HistPresent.LabelSizeY",  fLabelSizeY);  
   env.SetValue("HistPresent.TickLengthY", fTickLengthY); 
   env.SetValue("HistPresent.TitleOffsetY",fTitleOffsetY);
   env.SetValue("HistPresent.TitleSizeY",  fTitleSizeY);  
   env.SetValue("HistPresent.TitleColorY", fTitleColorY); 
   env.SetValue("HistPresent.TitleFontY",   fTitleFontY);  

   env.SetValue("HistPresent.NdivisionsZ", fNdivisionsZ); 
   env.SetValue("HistPresent.AxisColorZ",  fAxisColorZ);  
   env.SetValue("HistPresent.LabelColorZ", fLabelColorZ); 
   env.SetValue("HistPresent.LabelFontZ",  fLabelFontZ);  
   env.SetValue("HistPresent.LabelOffsetZ",fLabelOffsetZ);
   env.SetValue("HistPresent.LabelSizeZ",  fLabelSizeZ);  
   env.SetValue("HistPresent.TickLengthZ", fTickLengthZ); 
   env.SetValue("HistPresent.TitleOffsetZ",fTitleOffsetZ);
   env.SetValue("HistPresent.TitleSizeZ",  fTitleSizeZ);  
   env.SetValue("HistPresent.TitleColorZ", fTitleColorZ); 
   env.SetValue("HistPresent.TitleFontZ",  fTitleFontZ);  

   env.SetValue("HistPresent.HistFillColor", fHistFillColor);
   env.SetValue("HistPresent.HistLineColor", fHistLineColor);
   env.SetValue("HistPresent.HistFillStyle", fHistFillStyle);
   env.SetValue("HistPresent.HistLineStyle", fHistLineStyle);
   env.SetValue("HistPresent.HistLineWidth", fHistLineWidth);
   env.SetValue("HistPresent.EndErrorSize",  fEndErrorSize );
   env.SetValue("HistPresent.ErrorX",        fErrorX       );
   env.SetValue("HistPresent.FuncColor",     fFuncColor    );
   env.SetValue("HistPresent.FuncStyle",     fFuncStyle    );
   env.SetValue("HistPresent.FuncWidth",     fFuncWidth    );

   env.SetValue("HistPresent.StatColor",      gStyle->GetStatColor()     );
   env.SetValue("HistPresent.StatTextColor",  gStyle->GetStatTextColor() );
   env.SetValue("HistPresent.StatBorderSize", gStyle->GetStatBorderSize());
   env.SetValue("HistPresent.StatFont",       gStyle->GetStatFont()      );
   env.SetValue("HistPresent.StatFontSize",   gStyle->GetStatFontSize()  );
   env.SetValue("HistPresent.StatStyle",      gStyle->GetStatStyle()     );
   env.SetValue("HistPresent.StatX",          gStyle->GetStatX()         );
   env.SetValue("HistPresent.StatY",          gStyle->GetStatY()         );
   env.SetValue("HistPresent.StatW",          gStyle->GetStatW()         );
   env.SetValue("HistPresent.StatH",          gStyle->GetStatH()         );

   env.SetValue("HistPresent.TitleColor",      fTitleColor     );
   env.SetValue("HistPresent.TitleTextColor",  fTitleTextColor );
   env.SetValue("HistPresent.TitleBorderSize", fTitleBorderSize);
   env.SetValue("HistPresent.TitleFont",       fTitleFont      );
   env.SetValue("HistPresent.TitleFontSize",   fTitleFontSize  );
   env.SetValue("HistPresent.TitleStyle",      fTitleStyle     );
   env.SetValue("HistPresent.TitleX",          fTitleX         );
   env.SetValue("HistPresent.TitleY",          fTitleY         );
   env.SetValue("HistPresent.TitleW",          fTitleW         );
   env.SetValue("HistPresent.TitleH",          fTitleH         );
   env.SetValue("HistPresent.TitleAlign",      fTitleAlign     );

   env.SetValue("HistPresent.PadColor",        fPadColor       );
   env.SetValue("HistPresent.PadBorderSize",   fPadBorderSize  );
   env.SetValue("HistPresent.PadBorderMode",   fPadBorderMode  );
   env.SetValue("HistPresent.PadBottomMargin", fPadBottomMargin);
   env.SetValue("HistPresent.PadTopMargin",    fPadTopMargin   );
   env.SetValue("HistPresent.PadLeftMargin",   fPadLeftMargin  );
   env.SetValue("HistPresent.PadRightMargin",  fPadRightMargin );
   env.SetValue("HistPresent.PadGridX",        fPadGridX       );
   env.SetValue("HistPresent.PadGridY",        fPadGridY       );
   env.SetValue("HistPresent.PadTickX",        fPadTickX       );
   env.SetValue("HistPresent.PadTickY",        fPadTickY       );
	env.SetValue("HistPresent.FrameFillColor",  fFrameFillColor  );  
	env.SetValue("HistPresent.FrameLineColor",  fFrameLineColor  );  
	env.SetValue("HistPresent.FrameFillStyle",  fFrameFillStyle  );  
	env.SetValue("HistPresent.FrameLineStyle",  fFrameLineStyle  );  
	env.SetValue("HistPresent.FrameLineWidth",  fFrameLineWidth  );  
	env.SetValue("HistPresent.FrameBorderSize", fFrameBorderSize ); 
	env.SetValue("HistPresent.FrameBorderMode", fFrameBorderMode );
	env.SetValue("HistPresent.CanvasColor",	  fCanvasColor 	 );
	env.SetValue("HistPresent.CanvasBorderSize",fCanvasBorderSize);
	env.SetValue("HistPresent.CanvasBorderMode",fCanvasBorderMode);
	env.SetValue("HistPresent.CanvasDefH", 	  fCanvasDefH  	 );  
	env.SetValue("HistPresent.CanvasDefW", 	  fCanvasDefW  	 );  
	env.SetValue("HistPresent.CanvasDefX", 	  fCanvasDefX  	 );
	env.SetValue("HistPresent.CanvasDefY", 	  fCanvasDefY  	 );
	env.SetValue("HistPresent.MarkerColor",     fMarkerColor     );
	env.SetValue("HistPresent.MarkerStyle",     fMarkerStyle     );
	env.SetValue("HistPresent.MarkerSize",      fMarkerSize      );
	env.SetValue("HistPresent.LineColor",       fLineColor       );
	env.SetValue("HistPresent.LineStyle",       fLineStyle       );
	env.SetValue("HistPresent.LineWidth",       fLineWidth       );
	env.SetValue("HistPresent.TextSize" ,       fTextSize        );
	env.SetValue("HistPresent.TextAngle" ,      fTextAngle       );
	env.SetValue("HistPresent.TextAlign",       fTextAlign       );
	env.SetValue("HistPresent.TextColor",       fTextColor       );
	env.SetValue("HistPresent.TextFont" ,       fTextFont        );
	env.SetValue("HistPresent.FillColor",       fFillColor       );
	env.SetValue("HistPresent.FillStyle",       fFillStyle       );

	env.SetValue("HistPresent.ArrowAngle" ,    fArrowAngle       );
	env.SetValue("HistPresent.ArrowSize"  ,    fArrowSize        );
	env.SetValue("HistPresent.ArrowColor"  ,   fArrowColor       );
	env.SetValue("HistPresent.ArrowWidth"  ,   fArrowWidth       );
	env.SetValue("HistPresent.ArrowStyle"  ,   fArrowStyle       );
	env.SetValue("HistPresent.ArrowFill"  ,    fArrowFill        );
	env.SetValue("HistPresent.ArrowShape",     fArrowShape       );
	env.SetValue("HistPresent.CurlyWaveLength",fCurlyWaveLength  );
	env.SetValue("HistPresent.CurlyAmplitude" ,fCurlyAmplitude   );
	env.SetValue("HistPresent.CurlyColor"  ,   fCurlyColor       );
	env.SetValue("HistPresent.CurlyWidth"  ,   fCurlyWidth       );
	env.SetValue("HistPresent.CurlyStyle"  ,   fCurlyStyle       );
	env.SetValue("HistPresent.IsCurly"    ,    fIsCurly          );
	env.SetValue("HistPresent.ArcFillColor" ,fArcFillColor);
	env.SetValue("HistPresent.ArcFillStyle" ,fArcFillStyle);
   env.SetValue("HistPresent.ArcLineColor" ,fArcLineColor);
   env.SetValue("HistPresent.ArcLineStyle" ,fArcLineStyle);
   env.SetValue("HistPresent.ArcLineWidth" ,fArcLineWidth);

   env.SetValue("HistPresent.EditUsXlow",   fEditUsXlow  );
   env.SetValue("HistPresent.EditUsYlow",   fEditUsYlow  );
   env.SetValue("HistPresent.EditUsXwidth", fEditUsXwidth);
   env.SetValue("HistPresent.EditUsYwidth", fEditUsYwidth);
   env.SetValue("HistPresent.EditUsXRange", fEditUsXRange);

   env.SetValue("HistPresent.EditLsXlow",   fEditLsXlow  );
   env.SetValue("HistPresent.EditLsYlow",   fEditLsYlow  );
   env.SetValue("HistPresent.EditLsXwidth", fEditLsXwidth);
   env.SetValue("HistPresent.EditLsYwidth", fEditLsYwidth);
   env.SetValue("HistPresent.EditLsXRange", fEditLsXRange);

   env.SetValue("HistPresent.EditPoXlow",   fEditPoXlow  );
   env.SetValue("HistPresent.EditPoYlow",   fEditPoYlow  );
   env.SetValue("HistPresent.EditPoXwidth", fEditPoXwidth);
   env.SetValue("HistPresent.EditPoYwidth", fEditPoYwidth);
   env.SetValue("HistPresent.EditPoXRange", fEditPoXRange);

   env.SetValue("HistPresent.ForceStyle",      fForceStyle     );
 
   env.SaveLevel(kEnvUser);
}

//_______________________________________________________________________

void HistPresent::SetCurlyAttributes(TGWindow * win, FitHist * fh)
{
//   Int_t nopt = 6;

   enum e_CurlySet { 
      kArrowAngle      ,
      kArrowSize       ,
      kArrowShape     ,
      kCurlyWaveLength ,
      kCurlyAmplitude  ,
      kCurlyWidth      ,
      kCurlyStyle      ,
      kCurlyColor      ,
      kIsCurly 
   };
   const char * ArrowOption[] = 
      {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("Float_Value_ArrowAngle "));     
   row_lab->Add(new TObjString("Float_Value_ArrowSize  "));     
   row_lab->Add(new TObjString("ArrowSelect_ArrowForm"));         
   row_lab->Add(new TObjString("DoubleValue_CurlyWaveLength"));         
   row_lab->Add(new TObjString("DoubleValue_CurlyAmplitude"));         
   row_lab->Add(new TObjString("PlainShtVal_CurlyLineWidth"));     
   row_lab->Add(new TObjString("LineSSelect_CurlyLineStyle"));     
   row_lab->Add(new TObjString("ColorSelect_CurlyColor "));     
   row_lab->Add(new TObjString("CheckButton_IsCurly    "));         

   valp[ind++] = &fArrowAngle ;
   valp[ind++] = &fArrowSize  ;
   valp[ind++] = &fArrowShape ;
   valp[ind++] = &fCurlyWaveLength ;
   valp[ind++] = &fCurlyAmplitude  ;
   valp[ind++] = &fCurlyWidth  ;
   valp[ind++] = &fCurlyStyle  ;
   valp[ind++] = &fCurlyColor  ;
   valp[ind++] = &fIsCurly     ;

   Int_t nrows = row_lab->GetSize();
   TArrayI flag (nrows);
   for (Int_t i = 0; i < nrows; i++) {
      flag[i] = 0;
   } 
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Feynman diagrams", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp, 
                      &flag, "Set all");
   if (!ok) return;

   SaveOptions();
   gEnv->SetValue("HistPresent.ArrowAngle" ,fArrowAngle);
   gEnv->SetValue("HistPresent.ArrowSize"  ,fArrowSize );
   gEnv->SetValue("HistPresent.ArrowShape" ,fArrowShape);
   gEnv->SetValue("HistPresent.CurlyWaveLength" ,fCurlyWaveLength);
   gEnv->SetValue("HistPresent.CurlyAmplitude"  ,fCurlyAmplitude);
   gEnv->SetValue("HistPresent.CurlyWidth" ,fCurlyWidth);
   gEnv->SetValue("HistPresent.CurlyStyle" ,fCurlyStyle);
   gEnv->SetValue("HistPresent.CurlyColor" ,fCurlyColor);
   gEnv->SetValue("HistPresent.IsCurly"    ,fIsCurly);

	gStyle->SetLineColor(fCurlyColor);
	gStyle->SetLineStyle(fCurlyStyle);
	gStyle->SetLineWidth(fCurlyWidth);
   SetCurlyAtt();

   if (win && flag.GetSum() > 0) {
      TRootCanvas * cimp = (TRootCanvas*)win;
      TCanvas * canvas = cimp->Canvas();
      TList * lop = 0;
      if (canvas) lop = canvas->GetListOfPrimitives();
      if (lop) {
         TIter next(lop);
         TObject * obj;
         while ( (obj = next()) ) {
            if (obj->InheritsFrom("TCurlyLine")) {
               TCurlyLine * tc = (TCurlyLine*)obj;
               if (flag[kCurlyWaveLength] != 0) tc->SetWaveLength(fCurlyWaveLength);
               if (flag[kCurlyAmplitude] != 0)  tc->SetAmplitude(fCurlyAmplitude);
               if (flag[kCurlyWidth] != 0)      tc->SetLineWidth(fCurlyWidth);
               if (flag[kCurlyColor] != 0)      tc->SetLineColor(fCurlyColor);
               if (flag[kCurlyStyle] != 0)      tc->SetLineStyle(fCurlyStyle);
            }
            if (obj->InheritsFrom("TArrow")) {
               TArrow * a = (TArrow*)obj;
               if (flag[kArrowAngle] != 0)      a->SetAngle(fArrowAngle);
               if (flag[kArrowSize]  != 0)      a->SetArrowSize(fArrowSize);
//               if (flag[kArrowWidth] != 0)      a->SetLineWidth(fArrowWidth);
//               if (flag[kArrowStyle] != 0)      a->SetLineStyle(fArrowStyle);
//               if (flag[kArrowFill]  != 0)      a->SetFillStyle(fArrowFill);
//               if (flag[kArrowColor] != 0)      a->SetFillColor(fArrowColor);
               if (flag[kArrowShape] != 0)     {a->SetDrawOption(ArrowOption[fArrowShape]); 
                                                a->SetOption(ArrowOption[fArrowShape]);};
            }
         }
         canvas->Modified();
         canvas->Update();
      } 
   }
}
//_______________________________________________________________________

void HistPresent::SetCurlyAtt()
{
#if ROOTVERSION > ENABLE_CURLYDEFAULTS
   const char * ArrowOption[] = 
      {" " , "|>", "<|", ">", "<", "->-", "-<-", "-|>-", "-<|-", "<>", "<|>"};
	TCurlyLine::SetDefaultWaveLength(fCurlyWaveLength);
	TCurlyLine::SetDefaultAmplitude(fCurlyAmplitude);
	TCurlyLine::SetDefaultIsCurly(fIsCurly);
	TCurlyArc::SetDefaultWaveLength(fCurlyWaveLength);
	TCurlyArc::SetDefaultAmplitude(fCurlyAmplitude);
	TCurlyArc::SetDefaultIsCurly(fIsCurly);

	TArrow::SetDefaultAngle(fArrowAngle);
	TArrow::SetDefaultArrowSize(fArrowSize);
	TArrow::SetDefaultOption(ArrowOption[fArrowShape]);
	gStyle->SetLineColor(fCurlyColor);
	gStyle->SetLineStyle(fCurlyStyle);
	gStyle->SetLineWidth(fCurlyWidth);
#endif
}
//_______________________________________________________________________

void HistPresent::SetGeneralAttributes(TGWindow * win, FitHist * fh)
{
   enum e_GenSet {
      kLineColor     ,
      kLineStyle     ,
      kLineWidth     ,
      kTextSize      ,
      kTextAngle      ,
      kTextAlign     ,
      kTextColor     ,
      kTextFont      ,
      kFillColor     ,
      kFillStyle     ,
      kMarkerColor   ,
      kMarkerStyle   ,
      kMarkerSize    ,
      kArcFillColor  ,
      kArcFillStyle	,
      kArcLineColor	,
      kArcLineStyle	,
      kArcLineWidth	
   };
  
   static void *valp[25];
   Int_t ind = 0;
   TList *row_lab = new TList();
   row_lab->Add(new TObjString("ColorSelect_LineColor"));     
   row_lab->Add(new TObjString("LineSSelect_LineStyle"));     
   row_lab->Add(new TObjString("PlainShtVal_LineWidth"));         
   row_lab->Add(new TObjString("Float_Value_TextSize "));         
   row_lab->Add(new TObjString("Float_Value_TextAngle "));         
   row_lab->Add(new TObjString("AlignSelect_TextAlign"));         
   row_lab->Add(new TObjString("ColorSelect_TextColor"));         
   row_lab->Add(new TObjString("CfontSelect_TextFont "));         
   row_lab->Add(new TObjString("ColorSelect_FillColor"));         
   row_lab->Add(new TObjString("Fill_Select_FillStyle"));         
   row_lab->Add(new TObjString("ColorSelect_MarkerColor"));     
   row_lab->Add(new TObjString("Mark_Select_MarkerStyle"));     
   row_lab->Add(new TObjString("Float_Value_MarkerSize"));         
   row_lab->Add(new TObjString("ColorSelect_ArcFillColor"));         
   row_lab->Add(new TObjString("Fill_Select_ArcFillStyle"));         
   row_lab->Add(new TObjString("ColorSelect_ArcLineColor"));     
   row_lab->Add(new TObjString("LineSSelect_ArcLineStyle"));     
   row_lab->Add(new TObjString("PlainShtVal_ArcLineWidth"));         

   Int_t nrows = row_lab->GetSize();
//   TList  * values = new TList;
	valp[ind++] = &fLineColor;	 
	valp[ind++] = &fLineStyle;	 
	valp[ind++] = &fLineWidth;
	valp[ind++] = &fTextSize;
	valp[ind++] = &fTextAngle;
	valp[ind++] = &fTextAlign;
	valp[ind++] = &fTextColor;
   fTextFont /= 10;
	valp[ind++] = &fTextFont;
	valp[ind++] = &fFillColor;
	valp[ind++] = &fFillStyle;
	valp[ind++] = &fMarkerColor;   
	valp[ind++] = &fMarkerStyle;   
	valp[ind++] = &fMarkerSize;
	valp[ind++] = &fArcFillColor;
	valp[ind++] = &fArcFillStyle;
	valp[ind++] = &fArcLineColor;
	valp[ind++] = &fArcLineStyle;
	valp[ind++] = &fArcLineWidth;
   TArrayI flag (nrows);
   for (Int_t i = 0; i < nrows; i++) {
      flag[i] = 0;
   } 
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Graphics defaults", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp, 
                      &flag, "Set all", NULL, NULL, NULL);
   fTextFont = 2 + 10 * fTextFont;

   if (!ok) return;
   SaveOptions();
   SetGeneralAtt();

   if (win && flag.GetSum() > 0) {
      TRootCanvas * cimp = (TRootCanvas*)win;
      TCanvas * canvas = cimp->Canvas();
      TList * lop = 0;
      if (canvas) lop = canvas->GetListOfPrimitives();
      if (lop) {
         TIter next(lop);
         TObject * obj;
         while ( (obj = next()) ) {
            if (obj->InheritsFrom("TLine")) {
               TLine * tc = (TLine*)obj;
               if (flag[kLineWidth] != 0)      tc->SetLineWidth(fLineWidth);
               if (flag[kLineColor] != 0)      tc->SetLineColor(fLineColor);
               if (flag[kLineStyle] != 0)      tc->SetLineStyle(fLineStyle);
            }
            if (obj->InheritsFrom("TPolyLine")) {
               TPolyLine * tc = (TPolyLine*)obj;
               if (flag[kLineWidth] != 0)      tc->SetLineWidth(fLineWidth);
               if (flag[kLineColor] != 0)      tc->SetLineColor(fLineColor);
               if (flag[kLineStyle] != 0)      tc->SetLineStyle(fLineStyle);
            }
            if (obj->InheritsFrom("TPave")) {
               TPave * tc = (TPave*)obj;
               if (flag[kLineWidth] != 0)      tc->SetLineWidth(fLineWidth);
               if (flag[kLineColor] != 0)      tc->SetLineColor(fLineColor);
               if (flag[kLineStyle] != 0)      tc->SetLineStyle(fLineStyle);
               if (flag[kFillColor] != 0)      tc->SetFillColor(fFillColor);
               if (flag[kFillStyle] != 0)      tc->SetFillStyle(fFillStyle);
            }
            if (obj->InheritsFrom("TPaveText")) {
               TPaveText * tc = (TPaveText*)obj;
               if (flag[kTextSize]  != 0)      tc->SetTextSize(fTextSize);
               if (flag[kTextColor] != 0)      tc->SetTextColor(fTextColor);
               if (flag[kTextFont]  != 0)      tc->SetTextFont(fTextFont);
               if (flag[kTextAlign] != 0)      tc->SetTextAlign(fTextAlign);
            }
            if (obj->InheritsFrom("TPaveLabel")) {
               TPaveLabel * tc = (TPaveLabel*)obj;
               if (flag[kTextSize]  != 0)      tc->SetTextSize(fTextSize);
               if (flag[kTextColor] != 0)      tc->SetTextColor(fTextColor);
               if (flag[kTextFont]  != 0)      tc->SetTextFont(fTextFont);
               if (flag[kTextAlign] != 0)      tc->SetTextAlign(fTextAlign);
            }
            if (obj->InheritsFrom("TText")) {
               TText * tc = (TText*)obj;
               if (flag[kTextSize]  != 0)      tc->SetTextSize(fTextSize);
               if (flag[kTextColor] != 0)      tc->SetTextColor(fTextColor);
               if (flag[kTextFont]  != 0)      tc->SetTextFont(fTextFont);
               if (flag[kTextAlign] != 0)      tc->SetTextAlign(fTextAlign);
            }
            if (obj->InheritsFrom("TMarker")) {
               TMarker * tc = (TMarker*)obj;
               if (flag[kMarkerSize] != 0)       tc->SetMarkerSize(fMarkerSize);
               if (flag[kMarkerColor] != 0)      tc->SetMarkerColor(fMarkerColor);
               if (flag[kMarkerStyle] != 0)      tc->SetMarkerStyle(fMarkerStyle);
            }
            if (obj->InheritsFrom("TEllipse")) {
               TEllipse * tc = (TEllipse*)obj;
               if (flag[kArcFillColor] != 0)    tc->SetFillColor(fArcFillColor);
               if (flag[kArcFillStyle] != 0)    tc->SetFillStyle(fArcFillStyle);
               if (flag[kArcLineColor] != 0)    tc->SetLineColor(fArcLineColor);
               if (flag[kArcLineStyle] != 0)    tc->SetLineStyle(fArcLineStyle);
               if (flag[kArcLineWidth] != 0)    tc->SetLineWidth(fArcLineWidth);
            }
         }
         canvas->Modified();
         canvas->Update();
      } 
   }
}
//_______________________________________________________________________

void HistPresent::SetGeneralAtt()
{
   gStyle->SetLineColor(fLineColor);    
   gStyle->SetLineStyle(fLineStyle);    
   gStyle->SetLineWidth(fLineWidth);    
   gStyle->SetTextSize (fTextSize );    
   gStyle->SetTextAngle(fTextAngle);    
   gStyle->SetTextAlign(fTextAlign);    
   gStyle->SetTextColor(fTextColor);    
   gStyle->SetTextFont (fTextFont );    
   gStyle->SetFillColor(fFillColor);    
   gStyle->SetFillStyle(fFillStyle);    
   gStyle->SetMarkerColor(fMarkerColor);    
   gStyle->SetMarkerStyle(fMarkerStyle);    
   gStyle->SetMarkerSize(fMarkerSize);    
}
//_______________________________________________________________________

void HistPresent::SetTitleAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("ColorSelect_TitleBackgroundColor"));     
   row_lab->Add(new TObjString("ColorSelect_TitleTextColor")); 
   row_lab->Add(new TObjString("PlainShtVal_TitleBorderSize"));
   row_lab->Add(new TObjString("CfontSelect_TitleFont"));      
   row_lab->Add(new TObjString("Float_Value_TitleFontSize"));  
   row_lab->Add(new TObjString("LineSSelect_TitleStyle"));     
   row_lab->Add(new TObjString("Float_Value_TitleX"));         
   row_lab->Add(new TObjString("Float_Value_TitleY"));         
   row_lab->Add(new TObjString("Float_Value_TitleW"));         
   row_lab->Add(new TObjString("Float_Value_TitleH"));  
   row_lab->Add(new TObjString("AlignSelect_TitleAlign"));  

   valp[ind++] = &fTitleColor;     
   valp[ind++] = &fTitleTextColor;
   valp[ind++] = &fTitleBorderSize;
   fTitleFont /= 10;
   valp[ind++] = &fTitleFont;      
   valp[ind++] = &fTitleFontSize; 
   valp[ind++] = &fTitleStyle;    
   valp[ind++] = &fTitleX;         
   valp[ind++] = &fTitleY;         
   valp[ind++] = &fTitleW;         
   valp[ind++] = &fTitleH;  
   valp[ind++] = &fTitleAlign;  
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Title Attr", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   fTitleFont = 2 + 10 * fTitleFont;
   SaveOptions();
   SetTitleAtt();
}
//_______________________________________________________________________

void HistPresent::SetTitleAtt()
{
   gStyle->SetTitleColor(     fTitleColor, "t");    
   gStyle->SetTitleTextColor( fTitleTextColor);
   gStyle->SetTitleBorderSize(fTitleBorderSize);
   gStyle->SetTitleFont(      fTitleFont, "t");     
   gStyle->SetTitleFontSize(  fTitleFontSize); 
   gStyle->SetTitleStyle(     fTitleStyle);    
   gStyle->SetTitleX(         fTitleX);        
   gStyle->SetTitleY(         fTitleY);        
   gStyle->SetTitleW(         fTitleW);        
   gStyle->SetTitleH(         fTitleH);
   gStyle->SetTitleAlign(     fTitleAlign);
}
//_______________________________________________________________________

void HistPresent::SetPadAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("ColorSelect_PadColor"));       
   row_lab->Add(new TObjString("PlainShtVal_PadBorderSize"));  
   row_lab->Add(new TObjString("PlainIntVal_PadBorderMode"));  
   row_lab->Add(new TObjString("Float_Value_PadBottomMargin"));
   row_lab->Add(new TObjString("Float_Value_PadTopMargin"));   
   row_lab->Add(new TObjString("Float_Value_PadLeftMargin"));  
   row_lab->Add(new TObjString("Float_Value_PadRightMargin")); 
   row_lab->Add(new TObjString("PlainIntVal_PadGridX"));       
   row_lab->Add(new TObjString("PlainIntVal_PadGridY"));       
   row_lab->Add(new TObjString("PlainIntVal_PadTickX"));       
   row_lab->Add(new TObjString("PlainIntVal_PadTickY"));
   row_lab->Add(new TObjString("ColorSelect_FrameFillColor"));   
   row_lab->Add(new TObjString("ColorSelect_FrameLineColor"));   
   row_lab->Add(new TObjString("Fill_Select_FrameFillStyle"));   
   row_lab->Add(new TObjString("LineSSelect_FrameLineStyle"));   
   row_lab->Add(new TObjString("PlainShtVal_FrameLineWidth"));   
   row_lab->Add(new TObjString("PlainShtVal_FrameBorderSize"));  
   row_lab->Add(new TObjString("PlainIntVal_FrameBorderMode"));  
   row_lab->Add(new TObjString("ColorSelect_CanvasColor"));  	 
   row_lab->Add(new TObjString("PlainShtVal_CanvasBorderSize")); 
   row_lab->Add(new TObjString("PlainIntVal_CanvasBorderMode")); 
   row_lab->Add(new TObjString("PlainIntVal_CanvasDefH"));		 
   row_lab->Add(new TObjString("PlainIntVal_CanvasDefW"));		 
   row_lab->Add(new TObjString("PlainIntVal_CanvasDefX"));		 
   row_lab->Add(new TObjString("PlainIntVal_CanvasDefY"));		   

   valp[ind++] = &fPadColor;       
   valp[ind++] = &fPadBorderSize;
   valp[ind++] = &fPadBorderMode; 
   valp[ind++] = &fPadBottomMargin;
   valp[ind++] = &fPadTopMargin;
   valp[ind++] = &fPadLeftMargin; 
   valp[ind++] = &fPadRightMargin;
   valp[ind++] = &fPadGridX;      
   valp[ind++] = &fPadGridY;      
   valp[ind++] = &fPadTickX;     
   valp[ind++] = &fPadTickY;
   valp[ind++] = &fFrameFillColor; 
   valp[ind++] = &fFrameLineColor; 
   valp[ind++] = &fFrameFillStyle; 
   valp[ind++] = &fFrameLineStyle; 
   valp[ind++] = &fFrameLineWidth;
   valp[ind++] = &fFrameBorderSize; 
   valp[ind++] = &fFrameBorderMode; 
   valp[ind++] = &fCanvasColor; 	
   valp[ind++] = &fCanvasBorderSize;
   valp[ind++] = &fCanvasBorderMode;
   valp[ind++] = &fCanvasDefH;     
   valp[ind++] = &fCanvasDefW;     
   valp[ind++] = &fCanvasDefX;     
   valp[ind++] = &fCanvasDefY;

   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Canvas, Pad, Frame", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   SaveOptions();
   SetPadAtt();
}
//_______________________________________________________________________

void HistPresent::SetPadAtt()
{
   gStyle->SetPadColor       (fPadColor       ); 
   gStyle->SetPadBorderSize  (fPadBorderSize  ); 
   gStyle->SetPadBorderMode  (fPadBorderMode  ); 
   gStyle->SetPadBottomMargin(fPadBottomMargin); 
   gStyle->SetPadTopMargin   (fPadTopMargin   ); 
   gStyle->SetPadLeftMargin  (fPadLeftMargin  ); 
   gStyle->SetPadRightMargin (fPadRightMargin ); 
   gStyle->SetPadGridX       ((Bool_t)fPadGridX); 
   gStyle->SetPadGridY       ((Bool_t)fPadGridY); 
   gStyle->SetPadTickX       (fPadTickX       ); 
   gStyle->SetPadTickY       (fPadTickY       ); 
   gStyle->SetFrameFillColor    (fFrameFillColor  );
   gStyle->SetFrameLineColor    (fFrameLineColor  );
   gStyle->SetFrameFillStyle    (fFrameFillStyle  );
   gStyle->SetFrameLineStyle    (fFrameLineStyle  );
   gStyle->SetFrameLineWidth    (fFrameLineWidth  );
   gStyle->SetFrameBorderSize   (fFrameBorderSize );
   gStyle->SetFrameBorderMode   (fFrameBorderMode );
   gStyle->SetCanvasColor  	  (fCanvasColor     );
   gStyle->SetCanvasBorderSize  (fCanvasBorderSize);
   gStyle->SetCanvasBorderMode  (fCanvasBorderMode);
   gStyle->SetCanvasDefH		  (fCanvasDefH		  );
   gStyle->SetCanvasDefW		  (fCanvasDefW		  );
   gStyle->SetCanvasDefX		  (fCanvasDefX		  );
   gStyle->SetCanvasDefY		  (fCanvasDefY		  );
}
//_______________________________________________________________________

void HistPresent::SetHistAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("ColorSelect_HistFillColor")); 
   row_lab->Add(new TObjString("ColorSelect_HistLineColor"));  
   row_lab->Add(new TObjString("Fill_Select_HistFillStyle")); 
   row_lab->Add(new TObjString("LineSSelect_HistLineStyle"));  
   row_lab->Add(new TObjString("PlainShtVal_HistLineWidth"));
   row_lab->Add(new TObjString("ColorSelect_MarkerColor"));     
   row_lab->Add(new TObjString("Mark_Select_MarkerStyle"));     
   row_lab->Add(new TObjString("Float_Value_MarkerSize"));         
   row_lab->Add(new TObjString("Float_Value_EndErrorSize "));  
   row_lab->Add(new TObjString("Float_Value_ErrorX")); 
   row_lab->Add(new TObjString("ColorSelect_FuncColor"));
   row_lab->Add(new TObjString("LineSSelect_FuncStyle"));  
   row_lab->Add(new TObjString("PlainShtVal_FuncWidth")); 
   
   valp[ind++] = &fHistFillColor;
   valp[ind++] = &fHistLineColor;
   valp[ind++] = &fHistFillStyle;
   valp[ind++] = &fHistLineStyle;
   valp[ind++] = &fHistLineWidth;
	valp[ind++] = &fMarkerColor;   
	valp[ind++] = &fMarkerStyle;   
	valp[ind++] = &fMarkerSize;
   valp[ind++] = &fEndErrorSize;
   valp[ind++] = &fErrorX;     
   valp[ind++] = &fFuncColor;
   valp[ind++] = &fFuncStyle;  
   valp[ind++] = &fFuncWidth;
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Histograms, Functions", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   SaveOptions();
   SetHistAtt();
}
//_______________________________________________________________________
void HistPresent::SetHistAtt()
{
   gStyle->SetHistFillColor(fHistFillColor);
   gStyle->SetHistLineColor(fHistLineColor);
   gStyle->SetHistFillStyle(fHistFillStyle);
   gStyle->SetHistLineStyle(fHistLineStyle);
   gStyle->SetHistLineWidth(fHistLineWidth);
   gStyle->SetMarkerColor(fMarkerColor);
   gStyle->SetMarkerSize(fMarkerSize);
   gStyle->SetMarkerStyle(fMarkerStyle);
   gStyle->SetEndErrorSize (fEndErrorSize );
   gStyle->SetErrorX       (fErrorX       );
   gStyle->SetFuncColor    (fFuncColor    );
   gStyle->SetFuncStyle    (fFuncStyle    );
   gStyle->SetFuncWidth    (fFuncWidth    );
}
//_______________________________________________________________________

void HistPresent::SetStatDefaults(TCanvas * c)
{
   TPaveStats * st = (TPaveStats *)c->GetPrimitive("stats");
   if (!st) {
      cout << "Statistics box not found" << endl;
      return;
   }
   gStyle->SetStatColor     (st->GetFillColor() );
   gStyle->SetStatTextColor (st->GetTextColor() );
   gStyle->SetStatBorderSize(st->GetBorderSize());
   gStyle->SetStatFont      (st->GetTextFont()  );
   gStyle->SetStatFontSize  (st->GetTextSize()  );
   gStyle->SetStatStyle     (st->GetFillStyle() );
   gStyle->SetStatX         (st->GetX2NDC()        );
   gStyle->SetStatY         (st->GetY2NDC()        );
   gStyle->SetStatW         (st->GetX2NDC() - st->GetX1NDC());
   Float_t stath = st->GetY2NDC() - st->GetY1NDC();
   Int_t dofit = gStyle->GetOptFit();
   Int_t dostat = gStyle->GetOptStat();
   if (dofit  == 1) dofit  =  111;
   if (dostat == 1) dostat = 1111;
   Int_t nl = 0;
   Int_t mask = 1;
//   cout << dostat << endl;
   if (dostat != 0) {
      mask = 1;
      for (Int_t i = 0; i < 7; i++) {
         if ( (dostat / mask) % 10 == 1) nl++;
         mask *= 10;
      }
      if (dofit != 0) {
         mask = 1;
         for (Int_t i = 0; i < 7; i++) {
            if ( (dofit / mask) % 10 == 1) nl++;
            mask *= 10;
         }
      } 
      cout << nl << endl;
      stath = 4 * stath / (Float_t)nl;
   }
   gStyle->SetStatH(stath );
}
//_______________________________________________________________________

void HistPresent::SetStatAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   row_lab->Add(new TObjString("ColorSelect_StatBackgroundColor")); 
   row_lab->Add(new TObjString("ColorSelect_StatTextColor "));  
   row_lab->Add(new TObjString("PlainShtVal_StatBorderSize")); 
   row_lab->Add(new TObjString("CfontSelect_StatFont      "));  
   row_lab->Add(new TObjString("Float_Value_StatFontSize  "));
   row_lab->Add(new TObjString("Fill_Select_StatStyle (0 transparent)"));  
   row_lab->Add(new TObjString("Float_Value_StatX (upper right)")); 
   row_lab->Add(new TObjString("Float_Value_StatY (upper right)"));
   row_lab->Add(new TObjString("Float_Value_StatW         "));  
   row_lab->Add(new TObjString("Float_Value_StatH         ")); 

   valp[ind++] = &fStatColor;	 
   valp[ind++] = &fStatTextColor; 
   valp[ind++] = &fStatBorderSize;
   fStatFont /= 10;
   valp[ind++] = &fStatFont;  	   
   valp[ind++] = &fStatFontSize;  
   valp[ind++] = &fStatStyle; 	 
   valp[ind++] = &fStatX;  		 
   valp[ind++] = &fStatY;  		 
   valp[ind++] = &fStatW; 		 
   valp[ind++] = &fStatH;  		 
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Statbox", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   fStatFont       = 2 + 10 * fStatFont;
   if (!ok) return;
   SetStatAtt();
   SaveOptions();
}
//_______________________________________________________________________________

void HistPresent::SetStatAtt()
{
   gStyle->SetStatColor     (fStatColor     );
   gStyle->SetStatTextColor (fStatTextColor );
   gStyle->SetStatBorderSize(fStatBorderSize);
   gStyle->SetStatFont      (fStatFont      );
   gStyle->SetStatFontSize  (fStatFontSize  );
   gStyle->SetStatStyle     (fStatStyle     );
   gStyle->SetStatX         (fStatX         );
   gStyle->SetStatY         (fStatY         );
   gStyle->SetStatW         (fStatW         );
   gStyle->SetStatH         (fStatH         );
}
//_______________________________________________________________________________

void HistPresent::SetXaxisAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   static Int_t div = TMath::Abs(fNdivisionsX);
   static Int_t pdiv = div%100;
   static Int_t sdiv = (div/100)%100;
   static Int_t tdiv = (div/10000)%100;
   static Int_t optimize = 1;
   if (fNdivisionsX < 0) optimize = 0;
 
   row_lab->Add(new TObjString("PlainIntVal_Primary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Secondary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Tertiary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Optimize Div")); 
   row_lab->Add(new TObjString("ColorSelect_AxisColorX"));  
   row_lab->Add(new TObjString("ColorSelect_LabelColorX")); 
   row_lab->Add(new TObjString("CfontSelect_LabelFontX"));  
   row_lab->Add(new TObjString("Float_Value_LabelOffsetX"));
   row_lab->Add(new TObjString("Float_Value_LabelSizeX"));  
   row_lab->Add(new TObjString("Float_Value_TickLengthX")); 
   row_lab->Add(new TObjString("Float_Value_TitleOffsetX"));
   row_lab->Add(new TObjString("Float_Value_TitleSizeX"));  
   row_lab->Add(new TObjString("ColorSelect_TitleColorX")); 
   row_lab->Add(new TObjString("CfontSelect_TitleFontX"));  

   valp[ind++] = &pdiv;
   valp[ind++] = &sdiv;
   valp[ind++] = &tdiv;
   valp[ind++] = &optimize;
   valp[ind++] = &fAxisColorX; 
   valp[ind++] = &fLabelColorX;
   fLabelFontX /= 10;
   valp[ind++] = &fLabelFontX;
   valp[ind++] = &fLabelOffsetX;
   valp[ind++] = &fLabelSizeX;  
   valp[ind++] = &fTickLengthX; 
   valp[ind++] = &fTitleOffsetX;
   valp[ind++] = &fTitleSizeX;  
   valp[ind++] = &fTitleColorX;
   fTitleFontX /= 10;
   valp[ind++] = &fTitleFontX;


   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("X Axis Att", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   fNdivisionsX  = tdiv*10000 + sdiv*100 + pdiv;
   if (optimize == 0) fNdivisionsX *= -1;
   fLabelFontX   = 2 + 10 * fLabelFontX;
   fTitleFontX   = 2 + 10 * fTitleFontX;
   SetAxisAtt();
   row_lab->Delete();
   delete row_lab;
}
//_______________________________________________________________________________

void HistPresent::SetYaxisAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   static Int_t div = TMath::Abs(fNdivisionsY);
   static Int_t pdiv = div%100;
   static Int_t sdiv = (div/100)%100;
   static Int_t tdiv = (div/10000)%100;
   static Int_t optimize = 1;
   if (fNdivisionsY < 0) optimize = 0;
 
   row_lab->Add(new TObjString("PlainIntVal_Primary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Secondary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Tertiary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Optimize Div")); 
   row_lab->Add(new TObjString("ColorSelect_AxisColorY"));  
   row_lab->Add(new TObjString("ColorSelect_LabelColorY")); 
   row_lab->Add(new TObjString("CfontSelect_LabelFontY"));  
   row_lab->Add(new TObjString("Float_Value_LabelOffsetY"));
   row_lab->Add(new TObjString("Float_Value_LabelSizeY"));  
   row_lab->Add(new TObjString("Float_Value_TickLengthY")); 
   row_lab->Add(new TObjString("Float_Value_TitleOffsetY"));
   row_lab->Add(new TObjString("Float_Value_TitleSizeY"));  
   row_lab->Add(new TObjString("ColorSelect_TitleColorY")); 
   row_lab->Add(new TObjString("CfontSelect_TitleFontY"));  

   valp[ind++] = &pdiv;
   valp[ind++] = &sdiv;
   valp[ind++] = &tdiv;
   valp[ind++] = &optimize;
   valp[ind++] = &fAxisColorY; 
   valp[ind++] = &fLabelColorY;
   fLabelFontY /= 10;
   valp[ind++] = &fLabelFontY;
   valp[ind++] = &fLabelOffsetY;
   valp[ind++] = &fLabelSizeY;  
   valp[ind++] = &fTickLengthY; 
   valp[ind++] = &fTitleOffsetY;
   valp[ind++] = &fTitleSizeY;  
   valp[ind++] = &fTitleColorY;
   fTitleFontY /= 10;
   valp[ind++] = &fTitleFontY;


   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Y Axis Att", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   fNdivisionsY  = tdiv*10000 + sdiv*100 + pdiv;
   if (optimize == 0) fNdivisionsY *= -1;
   fLabelFontY   = 2 + 10 * fLabelFontY;
   fTitleFontY   = 2 + 10 * fTitleFontY;
   SetAxisAtt();
   row_lab->Delete();
   delete row_lab;
}
//_______________________________________________________________________________

void HistPresent::SetZaxisAttributes(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   static Int_t div = TMath::Abs(fNdivisionsZ);
   static Int_t pdiv = div%100;
   static Int_t sdiv = (div/100)%100;
   static Int_t tdiv = (div/10000)%100;
   static Int_t optimize = 1;
   if (fNdivisionsZ < 0) optimize = 0;
 
   row_lab->Add(new TObjString("PlainIntVal_Primary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Secondary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Tertiary Div")); 
   row_lab->Add(new TObjString("PlainIntVal_Optimize Div")); 
   row_lab->Add(new TObjString("ColorSelect_AxisColorZ"));  
   row_lab->Add(new TObjString("ColorSelect_LabelColorZ")); 
   row_lab->Add(new TObjString("CfontSelect_LabelFontZ"));  
   row_lab->Add(new TObjString("Float_Value_LabelOffsetZ"));
   row_lab->Add(new TObjString("Float_Value_LabelSizeZ"));  
   row_lab->Add(new TObjString("Float_Value_TickLengthZ")); 
   row_lab->Add(new TObjString("Float_Value_TitleOffsetZ"));
   row_lab->Add(new TObjString("Float_Value_TitleSizeZ"));  
   row_lab->Add(new TObjString("ColorSelect_TitleColorZ")); 
   row_lab->Add(new TObjString("CfontSelect_TitleFontZ"));  

   valp[ind++] = &pdiv;
   valp[ind++] = &sdiv;
   valp[ind++] = &tdiv;
   valp[ind++] = &optimize;
   valp[ind++] = &fAxisColorZ; 
   valp[ind++] = &fLabelColorZ;
   fLabelFontZ /= 10;
   valp[ind++] = &fLabelFontZ;
   valp[ind++] = &fLabelOffsetZ;
   valp[ind++] = &fLabelSizeZ;  
   valp[ind++] = &fTickLengthZ; 
   valp[ind++] = &fTitleOffsetZ;
   valp[ind++] = &fTitleSizeZ;  
   valp[ind++] = &fTitleColorZ;
   fTitleFontZ /= 10;
   valp[ind++] = &fTitleFontZ;


   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Z Axis Att", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;

   fNdivisionsZ  = tdiv*10000 + sdiv*100 + pdiv;
   if (optimize == 0) fNdivisionsZ *= -1;
   fLabelFontZ   = 2 + 10 * fLabelFontZ;
   fTitleFontZ   = 2 + 10 * fTitleFontZ;
   SetAxisAtt();
   row_lab->Delete();
   delete row_lab;
}
//_______________________________________________________________________

void HistPresent::SetAxisAtt()
{
   if (fLabelMaxDigits > 0) TGaxis::SetMaxDigits(fLabelMaxDigits);
   gStyle->SetNdivisions( fNdivisionsX); 
   gStyle->SetAxisColor(  fAxisColorX);  
   gStyle->SetLabelColor( fLabelColorX); 
   gStyle->SetLabelFont(  fLabelFontX);  
   gStyle->SetLabelOffset(fLabelOffsetX);
   gStyle->SetLabelSize(  fLabelSizeX);  
   gStyle->SetTickLength( fTickLengthX); 
   gStyle->SetTitleOffset(fTitleOffsetX);
   gStyle->SetTitleSize(  fTitleSizeX);  
   gStyle->SetTitleColor( fTitleColorX); 
   gStyle->SetTitleFont(  fTitleFontX);  

   gStyle->SetNdivisions( fNdivisionsY, "Y"); 
   gStyle->SetAxisColor(  fAxisColorY, "Y");  
   gStyle->SetLabelColor( fLabelColorY, "Y"); 
   gStyle->SetLabelFont(  fLabelFontY, "Y");  
   gStyle->SetLabelOffset(fLabelOffsetY, "Y");
   gStyle->SetLabelSize(  fLabelSizeY, "Y");  
   gStyle->SetTickLength( fTickLengthY, "Y"); 
   gStyle->SetTitleOffset(fTitleOffsetY, "Y");
   gStyle->SetTitleSize(  fTitleSizeY, "Y");  
   gStyle->SetTitleColor( fTitleColorY, "Y"); 
   gStyle->SetTitleFont(  fTitleFontY, "Y");  

   gStyle->SetNdivisions( fNdivisionsZ, "Z"); 
   gStyle->SetAxisColor(  fAxisColorZ, "Z");  
   gStyle->SetLabelColor( fLabelColorZ, "Z"); 
   gStyle->SetLabelFont(  fLabelFontZ, "Z");  
   gStyle->SetLabelOffset(fLabelOffsetZ, "Z");
   gStyle->SetLabelSize(  fLabelSizeZ, "Z");  
   gStyle->SetTickLength( fTickLengthZ, "Z"); 
   gStyle->SetTitleOffset(fTitleOffsetZ, "Z");
   gStyle->SetTitleSize(  fTitleSizeZ, "Z");  
   gStyle->SetTitleColor( fTitleColorZ, "Z"); 
   gStyle->SetTitleFont(  fTitleFontZ, "Z");  
}

//_______________________________________________________________________

void HistPresent::Set1DimOptions(TGWindow * win, FitHist * fh)
{
   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
 
   row_lab->Add(new TObjString("CheckButton_Show contour"));
   valp[ind++] = &fShowContour; 
   row_lab->Add(new TObjString("CheckButton+Fill histogram")); 
   valp[ind++] = &fFill1Dim; 
   row_lab->Add(new TObjString("ColorSelect_FillColor"));
   valp[ind++] = &fHistFillColor; 
   row_lab->Add(new TObjString("Fill_Select_FillStyle"));
   valp[ind++] = &fHistFillStyle; 

   row_lab->Add(new TObjString("CheckButton_Show Errors")); 
   valp[ind++] = &fShowErrors; 
   row_lab->Add(new TObjString("CheckButton+Xaxis at top")); 
   valp[ind++] = &fDrawAxisAtTop; 
   row_lab->Add(new TObjString("CheckButton_Live statbox")); 
   valp[ind++] = &fLiveStat1dim; 
   row_lab->Add(new TObjString("CheckButton+Live Gauss fit")); 
   valp[ind++] = &fLiveGauss; 
   row_lab->Add(new TObjString("CheckButton_Use linear background in live fit"));
   valp[ind++] = &fLiveBG; 
   row_lab->Add(new TObjString("PlainIntVal_Width of response func in fpeak"));
   valp[ind++] = &fPeakMwidth;
   row_lab->Add(new TObjString("DoubleValue_Threshold in fpeak"));
    valp[ind++] = &fPeakThreshold;
 
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("How to draw a 1-dim hist", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   if (fFill1Dim) {
      if (fHistFillColor == 0) fHistFillColor = 1;
      if (fHistFillStyle == 0) fHistFillStyle = 1001;
   }   
   SaveOptions();
   if (fh) fh->UpdateDrawOptions();
}
//_______________________________________________________________________

void HistPresent::SetGraphOptions(TGWindow * win, TCanvas * ca)
{
   TList *row_lab = new TList();
   const Int_t nopt = 8;
   static void *valp[nopt];
   Int_t flags[nopt];
   Int_t ind = 0;
   const char *cdrawopt[] = {"L", "F", "A", "C", "*", "P", "B", "1"};
   const char *gDrawOptText[] = {
      "CheckButton_A simple polyline between every points is drawn",
      "CheckButton_A fill area is drawn ('CF' draw a smooth fill area)",
      "CheckButton_Axis are drawn around the graph",
      "CheckButton_A smooth Curve is drawn",
      "CheckButton_A Star is plotted at each point",
      "CheckButton_Idem with the current marker",
      "CheckButton_A Bar chart is drawn at each point",
      "CheckButton_ylow=rwymin"};
   for (Int_t i = 0; i < nopt; i++) {
      row_lab->Add(new TObjString(gDrawOptText[i]));
      if (fDrawOptGraph.Contains(cdrawopt[i]))
         flags[i] = 1;
      else
         flags[i] = 0;
      valp[ind++] = &flags[i];
   }
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("Graph Drawing Options", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   fDrawOptGraph = "";
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0)
         fDrawOptGraph += cdrawopt[i];
   }
   if (ca) {
      TList logr;
      Int_t ngr = FindGraphs((TPad*)ca, &logr);
      if (ngr > 0) {
         TIter next(&logr);
         TObject * obj;
         while ( (obj = next()) ) {
            TGraph *gr =(TGraph*)obj;
            gr->SetDrawOption(fDrawOptGraph.Data());
         }
         ca->Modified();
         ca->Update();
      }
   }
   SaveOptions();
}
//_______________________________________________________________________

void HistPresent::Set2DimOptions(TGWindow * win, FitHist * fh)
{
static const Char_t helptext[] =
"SCAT  : Draw a scatter-plot (default)\n\
BOX   : a box is drawn for each cell with surface proportional to the\n\
          content's absolute value. A negative content is marked with a X.\n\
BOX1  : a button is drawn for each cell with surface proportional to\n\
          content's absolute value. A sunken button is drawn for negative values\n\
          a raised one for positive.\n\
COL   : a box is drawn for each cell with a color scale varying with contents\n\
COLZ  : same as COL. In addition the color palette is also drawn\n\
CONT  : Draw a contour plot (same as CONT0)\n\
CONT0 : Draw a contour plot using surface colors to distinguish contours\n\
CONT1 : Draw a contour plot using line styles to distinguish contours\n\
CONT2 : Draw a contour plot using the same line style for all contours\n\
CONT3 : Draw a contour plot using fill area colors\n\
CONT4 : Draw a contour plot using surface colors (SURF option at theta = 0)\n\
CONT5 : (TGraph2D only) Draw a contour plot using Delaunay triangles\n\
LEGO  : Draw a lego plot with hidden line removal\n\
LEGO1 : Draw a lego plot with hidden surface removal\n\
LEGO2 : Draw a lego plot using colors to show the cell contents\n\
SURF  : Draw a surface plot with hidden line removal\n\
SURF1 : Draw a surface plot with hidden surface removal\n\
SURF2 : Draw a surface plot using colors to show the cell contents\n\
SURF3 : same as SURF with in addition a contour view drawn on the top\n\
SURF4 : Draw a surface using Gouraud shading\n\
SURF5 : Same as SURF3 but only the colored contour is drawn. Used with\n\
          option CYL, SPH or PSR it allows to draw colored contours on a\n\
          sphere, a cylinder or a in pseudo rapidy space. In cartesian\n\
          or polar coordinates, option SURF3 is used.\n\
ARR   : arrow mode. Shows gradient between adjacent cells\n\
TEXT  : Draw bin contents as text (format set via gStyle->SetPaintTextFormat)\n\
";
   const char *cdrawopt2[] =
   {"SCAT", "BOX0",   "BOX1",  "COL", 
    "CONT0", "CONT1", "CONT2", "CONT3", "CONT4", 
    "LEGO", "LEGO1", "LEGO2",
    "SURF0", "SURF1", "SURF2", "SURF3", "SURF4", 
    "ARR", "TEXT"}; 

   const char *gDrawOpt2Text[] = {
      "RadioButton_Scatter pixels",
      "RadioButton_Boxes",
      "RadioButton_Boxes (raised sunken)",
      "RadioButton_Color levels",
      "RadioButton_Contour, fillcolors",
      "RadioButton_Contour, colored lines",
      "RadioButton_Contour, diff linestyles",
      "RadioButton_Contour, b/w solidlines",
      "RadioButton_Contour, surface colors",
      "RadioButton_Lego, hidden line removal",
      "RadioButton_Lego, hidden surface removal",
      "RadioButton_Lego, colored level",
      "RadioButton_Surface, b/w,  mesh",
      "RadioButton_Surface, colored,  mesh",
      "RadioButton_Surface, colored, no mesh",
      "RadioButton_Surface + Contour",
      "RadioButton_Surface, Gouraud shading",
      "RadioButton_Arrows indicating gradient",
      "RadioButton_Numbers"
   };

   const Int_t nopt = 19;
   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   Int_t flags[nopt];
   Int_t drawzscale = 0;
   Int_t hidebackbox = 0;
   Int_t hidefrontbox = 0;

   for (Int_t i = 0; i < nopt; i++) {
      if (fDrawOpt2Dim->Contains(cdrawopt2[i], TString::kIgnoreCase))
         flags[i] = 1;
      else
         flags[i] = 0;
      if (fDrawOpt2Dim->Contains("Z"))  drawzscale = 1;
      if (fDrawOpt2Dim->Contains("BB")) hidebackbox = 1;
      if (fDrawOpt2Dim->Contains("FB")) hidefrontbox = 1;
      valp[ind++] = &flags[i];
      row_lab->Add(new TObjString(gDrawOpt2Text[i]));
   }
   row_lab->Add(new TObjString("CheckButton_Show z-scale"));
   valp[ind++] = &drawzscale;
   row_lab->Add(new TObjString("CheckButton_Dont show back box (LEGO)"));
   valp[ind++] = &hidebackbox;
   row_lab->Add(new TObjString("CheckButton_Dont show front box (LEGO)"));
   valp[ind++] = &hidefrontbox;
   row_lab->Add(new TObjString("CheckButton_Show statbox when dragging mouse"));
   valp[ind++] = &fLiveStat2dim;
   row_lab->Add(new TObjString("DoubleValue_LogScale_Minimum"));
   valp[ind++] = &fLogScaleMin;
   row_lab->Add(new TObjString("DoubleValue_LinScale_Minimum"));
   valp[ind++] = &fLinScaleMin;

   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("How to draw a 1-dim hist", NULL, itemwidth, win
                     ,NULL, NULL, row_lab, valp
                     ,NULL, NULL, &helptext[0]);
   if (!ok) return;

   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0)
         *fDrawOpt2Dim = cdrawopt2[i];
   }

   TString sopt = *fDrawOpt2Dim;
//         cout << " sopt " << sopt << endl;
   if (sopt.Contains("SURF", TString::kIgnoreCase) ||
       sopt.Contains("LEGO", TString::kIgnoreCase)) {
       cout << " sopt " << sopt << endl;

      if (hidebackbox != 0)
         *fDrawOpt2Dim += "BB";
      if (hidefrontbox != 0)
         *fDrawOpt2Dim += "FB";
   }
   if (sopt.Contains("CONT", TString::kIgnoreCase) ||
      sopt.Contains("COL", TString::kIgnoreCase)) {
      if (drawzscale != 0)
         *fDrawOpt2Dim += "Z";
   }
   if (sopt.Contains("CONT0", TString::kIgnoreCase))
      gStyle->SetHistFillStyle(1001);

   if (fh) {
      fh->SetSelectedPad();
//      cout << " fDrawOpt2Dim: " << fDrawOpt2Dim->Data() << endl;
      fh->GetSelHist()->SetOption(fDrawOpt2Dim->Data());
      fh->GetSelHist()->SetDrawOption(fDrawOpt2Dim->Data());
   }
   SaveOptions();
   if (fh) fh->UpdateDrawOptions();
}

//_______________________________________________________________________

void HistPresent::Set2DimColorOpt(TGWindow * win, FitHist * fh)
{
   const Int_t nopt = 5;
   Int_t flags[nopt];
   static void *valp[10];
   Int_t ind = 0;
   TList *row_lab = new TList();
   const char *cdrawopt2[] =
       { "TRANSRGB", "TRANSHLS", "MONO", "MINV", "REGB"};
   const char *gDrawOpt2Text[] = {
      "RadioButton_Color transition RGB",
      "RadioButton_Color transition HLS",
      "RadioButton_Grey levels, highest=white",
      "RadioButton_Grey levels, highest=black",
      "RadioButton_Rainbow colors"
   };
   for (Int_t i = 0; i < nopt; i++) {
      if (f2DimColorPalette->Contains(cdrawopt2[i]))
         flags[i] = 1;
      else
         flags[i] = 0;
      valp[ind++] = &flags[i];
      row_lab->Add(new TObjString(gDrawOpt2Text[i]));
   }
   row_lab->Add(new TObjString("PlainIntVal_Start color in RGB trans"));
   row_lab->Add(new TObjString("PlainIntVal_End   color in RGB trans"));
   valp[ind++] = &fStartColor;
   valp[ind++] = &fEndColor;

   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("2 Dim Color Mode", NULL, itemwidth, win,
                      NULL, NULL, row_lab, valp);
   if (!ok) return;
   *f2DimColorPalette = "DEFA"; 
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0)
         *f2DimColorPalette = cdrawopt2[i];
   }

   SetColorPalette();
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetDisplayOptions(TGWindow * win, FitHist * fh)
{
   const Int_t nopt = 7;
   Int_t flags[nopt];
   Int_t statopts[nopt] = {1, 10, 100, 1000, 10000, 100000, 1000000};

   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   row_lab->Add(new TObjString("CheckButton_Show histogram title"));
   row_lab->Add(new TObjString("CheckButton_Show date box"));
   row_lab->Add(new TObjString("CheckButton_Use time of display"));
   row_lab->Add(new TObjString("CheckButton_Show statistics box"));
   row_lab->Add(new TObjString("CheckButton_Show name of histogram"));
   row_lab->Add(new TObjString("CheckButton_Show number of entries"));
   row_lab->Add(new TObjString("CheckButton_Show mean"));
   row_lab->Add(new TObjString("CheckButton_Show rms"));
   row_lab->Add(new TObjString("CheckButton_Show underflow"));
   row_lab->Add(new TObjString("CheckButton_Show overflow"));
   row_lab->Add(new TObjString("CheckButton_Show integral"));
   row_lab->Add(new TObjString("CheckButton_Show fit parameters"));

   valp[ind++] = &fShowTitle;
   valp[ind++] = &fShowDateBox;
   valp[ind++] = &fUseTimeOfDisplay;
   valp[ind++] = &fShowStatBox;

   for (Int_t i = 0; i < nopt; i++) {
      if (((fOptStat / statopts[i]) % 10) != 0) flags[i] = 1; 
      else                                      flags[i] = 0; 
      valp[ind++] = &flags[i];
   }
   valp[ind++] = &fShowFitBox;
   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("What to show for a histogram", NULL, itemwidth, win
                     ,NULL, NULL, row_lab, valp);
   if (!ok) return;
   fOptStat = 0;
   for (Int_t i = 0; i < nopt; i++) {
      if (flags[i] != 0)
        fOptStat += statopts[i];
   }
   gStyle->SetOptFit(0);
   gStyle->SetOptStat(0);
   if (fh){
      fh->GetCanvas()->Modified();
      fh->GetCanvas()->Update();
   }
   if (fShowStatBox) {
      gStyle->SetOptStat(fOptStat);
      gStyle->SetOptFit(fShowFitBox);
   }    gStyle->SetOptTitle(fShowTitle);
   if (fh){
      fh->GetCanvas()->Modified();
      fh->GetCanvas()->Update();
   }
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetVariousOptions(TGWindow * win, FitHist * fh)
{
// *INDENT-OFF* 
static const char helptext[] = 
"\n\
With sockets, show lists only\n\
-----------------------------\n\
If very many histograms are used this option allows\n\
to display user defined lists of histograms only.\n\
This avoids fetching the statistics of all histograms\n\
when tables are assembled.\n\
\n\
Show PS file after creation\n\
---------------------------\n\
Automatically invoke ghostview when a PostScript file is\n\
generated.\n\
____________________________________________________________\n\
Enable calibration\n\
------------------\n\
The X - scale of of a 1-dim histogram may be recalibrated\n\
providing (more than 1) x-y-points. If this option is active\n\
the user is asked after a fit to a peak if the mean value \n\
should be added to the list of calibration points.\n\
____________________________________________________________\n\
Auto Display calibrated hist\n\
----------------------------\n\
Display automatically a calibrated histogram if calibration\n\
data are storteed in defaulsts file\n\
____________________________________________________________\n\
Display fitted curves\n\
---------------------\n\
Draw a fitted curve into the histogram.\n\
____________________________________________________________\n\
Remember hist limits if showing trees\n\
-------------------------------------\n\
When displaying trees (ntuples) limits for histograms \n\
(number of channels, lower, upper edge), these values\n\
may be remembered for each tree entry between sessions.\n\
____________________________________________________________\n\
Remember Expand settings (Marks) \n\
-----------------------------------\n\
Using marks expanded parts of hists may be shown. This\n\
option allows to pass these to later sessions.\n\
____________________________________________________________\n\
Remember Zoomings (by left mouse)\n\
---------------------------------\n\
Pressing the left mouse button in the scale of a histogram\n\
dragging to the required limit allows to zoom in the picture\n\
This option allows to pass these limits to later sessions.\n\
_____________________________________________________________\n\
Use Attribute Macro \n\
-------------------\n\
Each time a histogram is display a macro (FH_setdefaults.C)\n\
may be executed. This option can be switched on or off.\n\
____________________________________________________________\n\
Use Regular expression syntax\n\
------------------------------\n\
Normally wild card syntax (e.g. ls *.root to list all\n\
files ending with .root) is used in file/histo selection\n\
masks. One may switch to the more powerful Regular expression\n\
For details consult a book on Unix.\n\
_____________________________________________________________\n\
Max Ents in Lists\n\
-----------------\n\
Too many entries in histlist (e.g. > 1000) make build up\n\
of lists slow and may even crash X on some older systems\n\
With this option one can limit this number\n\
____________________________________________________________\n\
Auto Update Interval\n\
Histograms may be updated automatically, this number is the\n\
update interval in seconds\n\
____________________________________________________________\n\
Nof color transition levels\n\
---------------------------\n\
Number of transition levels in color palettes.\n\
\n\
";
// *INDENT-ON* 
   TList *row_lab = new TList();
   static void *valp[50];
   Int_t ind = 0;
   row_lab->Add(new TObjString("CheckButton_Force style)); i.e show histograms with current style"));
   valp[ind++] = &fForceStyle;
   row_lab->Add(new TObjString("CheckButton_Show histlists only"));
   valp[ind++] = &fShowListsOnly;
   row_lab->Add(new TObjString("CheckButton_Show PS file after creation"));
   valp[ind++] = &fShowPSFile;
   row_lab->Add(new TObjString("CheckButton_Enable calibration"));
   valp[ind++] = &fEnableCalibration;
   row_lab->Add(new TObjString("CheckButton_Auto Display calibrated hist"));
   valp[ind++] = &fDisplayCalibrated;
   row_lab->Add(new TObjString("CheckButton_Display compents of fit function"));
   valp[ind++] = &fShowFittedCurves;
   row_lab->Add(new TObjString("CheckButton_Remember hist limits when showing trees"));
   valp[ind++] = &fRememberTreeHists;
   row_lab->Add(new TObjString("CheckButton_Ask always for hist limits when showing trees"));
   valp[ind++] = &fAlwaysNewLimits;
   row_lab->Add(new TObjString("CheckButton_Keep old hists when showing trees (add version # to name)"));
   valp[ind++] = &fNtupleVersioning;
   row_lab->Add(new TObjString("CheckButton_Remember Expand settings (Marks)"));
   valp[ind++] = &fRememberLastSet;
   row_lab->Add(new TObjString("CheckButton_Remember Zoomings (by left mouse)"));
   valp[ind++] = &fRememberZoom;
   row_lab->Add(new TObjString("CheckButton_Use Attribute Macro"));
   valp[ind++] = &fUseAttributeMacro;
   row_lab->Add(new TObjString("CheckButton_In Show Selected: Show All As First"));
   valp[ind++] = &fShowAllAsFirst;
   row_lab->Add(new TObjString("CheckButton_Really stack (instead of superimpose)"));
   valp[ind++] = &fRealStack;
   row_lab->Add(new TObjString("CheckButton_Use Regular expression syntax"));
   valp[ind++] = &fUseRegexp;
   row_lab->Add(new TObjString("PlainIntVal_Max Ents in Lists"));
   valp[ind++] = &fMaxListEntries;
   row_lab->Add(new TObjString("PlainIntVal_Auto Update Interval"));
   valp[ind++] = &fAutoUpdateDelay;
   row_lab->Add(new TObjString("PlainIntVal_Nof color transition levels"));
   valp[ind++] = &fNofTransLevels;

   Bool_t ok; 
   Int_t itemwidth = 240;
   ok = GetStringExt("How to draw a 1-dim hist", NULL, itemwidth, win
                     ,NULL, NULL, row_lab, valp
                     ,NULL, NULL, helptext);
   if (!ok) return;

   if (fForceStyle > 0) gROOT->ForceStyle();
   else                 gROOT->ForceStyle(kFALSE);
   SaveOptions();
}

//_______________________________________________________________________

void HistPresent::SetWindowSizes(TGWindow * win, FitHist * fh)
{
// *INDENT-OFF* 
   static const char helptext[] = 
"\n\
This menu controls default window sizes:\n\
Window sizes and offsets are measured in pixels\n\
Top left is (0, 0)\n\
\n\
Main Window Width:\n\
This defines the width of the main control window \n\
and windows for file and hist lists, default 250.\n\
\n\
Lists Width:\n\
This gives additional control for lists:\n\
> 0: Take value as absolute width (pixels)\n\
= 0: Take default width (see above)\n\
< 0: Adjust width to fit names without scrollbars\n\
\n\
Window_X_Width_1dim etc.:\n\
Widths for 1 and 2-dim Histograms\n\
\n\
Window_Shift_X, Y:\n\
Each new window is staggered ny theses values\n\
\n\
Window_Top_X, Y\n\
Position of first window (after Close Windows)\n\
\n\
Project_Both_Ratio\n\
When projectecting 2-dim histograms on both axis\n\
the 2-dim histogram takes this amount of the space\n\
available\n\
\n\
X- Y Margin in ShowSelected\n\
This controls the space between the pictures when\n\
multiple histograms are shown in one canvas.\n\
_____________________________________________________\n\
\n\
";
// *INDENT-ON* 

   TList *row_lab = new TList();
   static void *valp[25];
   Int_t ind = 0;
   Int_t fittext = 0;
   Int_t usecustom = 0;
   if (fWinwidx_hlist == -1) {
      fittext = 1;
      fWinwidx_hlist = fMainWidth;
   }
   else if (fWinwidx_hlist > 0) {
      usecustom = 1;
   } else {
      usecustom = 0;
      fWinwidx_hlist = fMainWidth;
   }
   row_lab->Add(new TObjString("PlainIntVal_Main Window Width"));
   row_lab->Add(new TObjString("CheckButton_Expand width to fit"));
   row_lab->Add(new TObjString("CheckButton_Use fixed width"));
   row_lab->Add(new TObjString("PlainIntVal_Lists Width"));
   row_lab->Add(new TObjString("PlainIntVal_Window_X_Width_1dim"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Y_Width_1dim"));
   row_lab->Add(new TObjString("PlainIntVal_Window_X_Width_2dim"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Y_Width_2dim"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Shift_X"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Shift_Y"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Top_X"));
   row_lab->Add(new TObjString("PlainIntVal_Window_Top_Y"));
   row_lab->Add(new TObjString("DoubleValue_Project_Both_Ratio"));
   row_lab->Add(new TObjString("Float_Value_X - Margin in ShowSelected"));
   row_lab->Add(new TObjString("Float_Value_Y - Margin in ShowSelected"));

   valp[ind++] = &fMainWidth;
   valp[ind++] = &fittext;
   valp[ind++] = &usecustom;
   valp[ind++] = &fWinwidx_hlist;
   valp[ind++] = &fWinwidx_1dim;
   valp[ind++] = &fWinwidy_1dim;
   valp[ind++] = &fWinwidx_2dim;
   valp[ind++] = &fWinwidy_2dim;
   valp[ind++] = &fWinshiftx;
   valp[ind++] = &fWinshifty;
   valp[ind++] = &fWintopx;
   valp[ind++] = &fWintopy;
   valp[ind++] = &fProjectBothRatio;
   valp[ind++] = &fDivMarginX;
   valp[ind++] = &fDivMarginY;

   Bool_t ok; 
   Int_t itemwidth = 320;
   ok = GetStringExt("Default window sizes and positions", NULL, itemwidth
                     , win, NULL, NULL, row_lab, valp
                     , NULL, NULL, helptext);
   if (!ok) return;
   if (fittext != 0) fWinwidx_hlist = -1;
   else if (usecustom == 0) fWinwidx_hlist = fMainWidth;
   SaveOptions();
}
//___________________________________________________________________________________________

void HistPresent::SetGreyLevels()
{
   fGreyPalette    = new Int_t[fNofGreyLevels];
   fGreyPaletteInv = new Int_t[fNofGreyLevels];
   static Int_t mono = 0;
   TColor *color;
   if (mono == 0) {
      mono = 1;
      Float_t frac = 1 / (Float_t) fNofGreyLevels;
      for (Int_t i = 0; i < fNofGreyLevels; i++) {
         Float_t x = (Float_t) i;
         fStartColorIndex = 331;
         color =
             new TColor(fStartColorIndex + i, 1 - x * frac, 1 - x * frac, 1 - x * frac,
                        "");
         fGreyPaletteInv[i] = fStartColorIndex + i;

         fStartColorIndex = 301;
         color = new TColor(fStartColorIndex + i, x * frac, x * frac, x * frac, "");
         fGreyPalette[i] = fStartColorIndex + i;
      }
   }
}
//___________________________________________________________________________________________

void HistPresent::SetColorPalette()
{
   if (f2DimColorPalette->Contains("MONO")) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPalette;
      fStartColorIndex = 301;
   } else if (f2DimColorPalette->Contains("MINV")) {
      fNofColorLevels = fNofGreyLevels;
      fPalette = fGreyPaletteInv;
      fStartColorIndex = 331;
   } else if (f2DimColorPalette->Contains("TRANSRGB")) {
      fNofColorLevels = fNofTransLevels;
      fPalette = fTransPaletteRGB;
      fStartColorIndex = 361;
   } else if (f2DimColorPalette->Contains("TRANSHLS")) {
      fNofColorLevels = fNofTransLevels;
      fPalette = fTransPaletteHLS;
      fStartColorIndex = 391;
   } else if (f2DimColorPalette->Contains("REGB")) {
      fNofColorLevels = 50;
      fStartColorIndex = 51;
      fPalette = NULL;
   } else {
      fNofColorLevels = 50;
      fPalette = NULL;
      fStartColorIndex = 1;
   }
//   cout << "SetColorPalette: " << *f2DimColorPalette << " " 
//        << fNofColorLevels << " " <<  fPalette << " " << fStartColorIndex << endl;
   if ( f2DimColorPalette->Contains("REGB"))
      gStyle->SetPalette(1, NULL);
   else
      gStyle->SetPalette(fNofColorLevels, fPalette);
}
//___________________________________________________________________________________________

void HistPresent::SetTransLevelsRGB()
{
   if (fNofTransLevels < 2) fNofTransLevels = 2;
   if (fTransPaletteRGB) delete [] fTransPaletteRGB;
   fTransPaletteRGB    = new Int_t[fNofTransLevels];
   TColor * color;
   TColor * sc = GetColorByInd(fStartColor); 
   TColor * ec = GetColorByInd(fEndColor);
   Float_t start_r = sc->GetRed(); 
   Float_t step_r = (ec->GetRed() - start_r) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_r = 0;
   Float_t start_g = sc->GetGreen(); 
   Float_t step_g = (ec->GetGreen() - start_g) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_g = 0;
   Float_t start_b = sc->GetBlue(); 
   Float_t step_b = (ec->GetBlue() - start_b) / (Float_t)(fNofTransLevels - 1);
   Float_t frac_b = 0;
//   Float_t nof2 = fNofTransLevels / 2.;
   fStartColorIndex = 361;
   for (Int_t i = 0; i < fNofTransLevels; i++) {
      color = GetColorByInd(fStartColorIndex + i);
      if (color) delete color;
//      Float_t dc = 1 + 2. * (1. - 2. * TMath::Abs((i - nof2) / fNofTransLevels ));
      Float_t rgb[3];
      Float_t h, l, s;
      rgb[0] = start_r + frac_r;
      rgb[1] = start_g + frac_g;
      rgb[2] = start_b + frac_b;
      TColor::RGB2HLS(rgb[0], rgb[1], rgb[2], h, l, s);
//      cout << setprecision(3) << "rgb: " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " "  
//           << "hls " << h << " " << l << " " << s << endl;
//    adjust to same lightness
//      Float_t max = TMath::MaxElement(3, rgb);
//      if (max > 0) max = 1. / max;
//      for (Int_t j = 0; j < 3; j++) rgb[j] *= max;
      rgb[0] *= fEnhenceRed;           
      rgb[1] *= fEnhenceGreen;           
      rgb[2] *= fEnhenceBlue;
      for (Int_t jj = 0; jj < 1; jj++) {
         if (rgb[0] > 1.) {
            rgb[1] += 0.5 * (rgb[0] - 1.);            
            rgb[2] += 0.5 * (rgb[0] - 1.);
            rgb[0] = 1.;
         }            
         if (rgb[1] > 1.) {
            rgb[0] += 0.5 * (rgb[1] - 1.);            
            rgb[2] += 0.5 * (rgb[1] - 1.);
            rgb[1] = 1.;
         }            
         if (rgb[2] > 1.) {
            rgb[0] += 0.5 * (rgb[2] - 1.);            
            rgb[1] += 0.5 * (rgb[2] - 1.);
            rgb[2] = 1.;
         } 
      }           
      color = new TColor(fStartColorIndex + i, rgb[0], rgb[1], rgb[2],"");
//      color->Print();
      fTransPaletteRGB[i] = fStartColorIndex + i;
      frac_r += step_r;
      frac_g += step_g;
      frac_b += step_b;
   }
}
//___________________________________________________________________________________________

void HistPresent::SetTransLevelsHLS()
{
   if (fNofTransLevels < 2) fNofTransLevels = 2;
   if (fTransPaletteHLS) delete [] fTransPaletteHLS;
   fTransPaletteHLS    = new Int_t[fNofTransLevels];
   TColor * color;
  
   Float_t step_h = (fEndHue - fStartHue) / (Float_t)(fNofTransLevels - 1);
   Float_t rgb[3];
   Float_t h, l, s;

   fStartColorIndex = 391;
   h = fStartHue;
   l = fLightness;
   s = fSaturation;

   for (Int_t i = 0; i < fNofTransLevels; i++) {
      color = GetColorByInd(fStartColorIndex + i);
      if (color) delete color;
    
      TColor::HLS2RGB(h, l, s, rgb[0], rgb[1], rgb[2]);
     
      color = new TColor(fStartColorIndex + i, rgb[0], rgb[1], rgb[2],"");
//      cout << setprecision(3) << "rgb: " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " "  
//           << "hls " << h << " " << l << " " << s << endl;
      fTransPaletteHLS[i] = fStartColorIndex + i;
      h += step_h;
   }
}
