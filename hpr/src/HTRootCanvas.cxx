#include "HTRootCanvas.h"
//*KEEP,TRootApplication,T=C++.
#include "TRootApplication.h"
//*KEEP,TRootHelpDialog,T=C++.
#include "TRootHelpDialog.h"
//*KEEP,TGClient.
#include "TGClient.h"
//*KEEP,TGCanvas.
#include "TGCanvas.h"
//*KEEP,TGMenu,T=C++.
#include "TGMenu.h"
#include "TGButton.h"
//*KEEP,TGWidget.
#include "TGWidget.h"
//*KEEP,TGFileDialog.
#include "TGFileDialog.h"
//*KEEP,TGStatusBar.
#include "TGStatusBar.h"
//*KEEP,TROOT.
#include "TROOT.h"
//*KEEP,TCanvas.
#include "HTCanvas.h"
//*KEEP,TBrowser.
#include "TBrowser.h"
//*KEEP,TMarker.
#include "TMarker.h"
//*KEEP,TStyle.
#include "TStyle.h"
//*KEEP,TVirtualX.
#include "TVirtualX.h"
//*KEEP,TApplication.
#include "TApplication.h"
//*KEEP,TFile.
#include "TFile.h"
#include "TInterpreter.h"
#include "TGMsgBox.h"
//*KEND.

//*KEEP,HelpText,T=C++.
#include "support.h"
#include "HelpText.h"
#include "TGMrbHelpWindow.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "FitHist_Help.h"
#include "TMrbHelpBrowser.h" 

//#include "FhToASCII.h"
//#include "FHCommands.h"
//*KEND.

enum ERootCanvasCommands {
   kFileNewCanvas,
   kFileOpen,
   kFileSaveAs,
   kFileSaveAsRoot,
   kFileSaveAsC,
   kFileSaveAsPS,
   kFileSaveAsEPS,
   kFileSaveAsGIF,
   kFilePrint,
   kFileCloseCanvas,
   kFileQuit,

   kEditEditor,
   kEditUndo,
   kEditClearPad,
   kEditClearCanvas,

   kViewColors,
   kViewFonts,
   kViewMarkers,
   kViewIconify,
   kViewX3D,
   kViewOpenGL,
   kInterrupt,

   kOptionEventStatus,
   kOptionAutoExec,
   kOptionAutoResize,
   kOptionResizeCanvas,
   kOptionMoveOpaque,
   kOptionResizeOpaque,
   kOptionRefresh,
   kOptionStatistics,
   kOptionHistTitle,
   kOptionFitParams,
   kOptionCanEdit,

   kInspectRoot,
   kInspectBrowser,

   kClassesTree,

   kHelpAbout,
   kHelpOnCanvas,
   kHelpOnMenus,
   kHelpOnGraphicsEd,
   kHelpOnBrowser,
   kHelpOnObjects,
   kHelpOnPS,

//   Ottos stuff

   kOptionFit,
   kOptionDisp,
   kOption1Dim,
   kOption2Dim,
   kOptionHpr,
   kOptionNum,
   kOptionWin,
   kOptionCol,
   kFHMarks,
   kFHMarkon,
   kFHMarkoff,
   kFHClearMarks,
   kFHPrintMarks,
   kFHSet2Marks,
   kFHSelectInside,
   kFHKeepPar,
   kFHCallMinos,
 
   kFHDisplay,
   kFHExpand,
   kFHEntire,
   kFHMagnify,
   kFHSuperimpose,
   kFHSuperimposeScale,
   kFHGetRange,
//   kFHSetDrawMode,
//   kFHSetLabAtt,
   kFHLogY,
   kFHProjectX,
   kFHProjectY,
   kFHProjectF,
   kFHProjectB,
   kFHAllAsSel,
   kFHCalAllAsSel,
   kFHAllAsFirst,
   kFHCanvas2LP,
   kFHCanvas2LPplain,
   kFHAllAsSelRangeOnly,
   kFHActivateTimer,
   kFHRebinAll,
   kFHRebinOne,
//   kFHProjectY,
   kFHOutputStat,
   kFHHistToFile,
   kFHCanvasToFile,
   kFHHistToASCII,
   kFHPictToPSplain,
   kFHPictToPS,
   kFHPictToLP,

   kFHCut,
   kFHInitCut,
   kFHClearCut,
   kFHListCuts,
   kFHListWindows,
   kFHDrawCut,
   kFHDrawCutName,
   kFHMarksToCut,  
   kFHWriteOutCut,
   kFHTerminate,

   kFHFit,
   kFHSetLinBg,
   kFHFitGBgOnly,
   kFHFitGBgTailLow,
   kFHFitGBgTailHigh,
   kFHFitUser,
   kFHEditUser,
   kFHMacroBreitW,
   kFHMacroExpG,
   kFHMacroLandau,
   kFHMacroTwoG,
   kFHFuncsToHist,
   kFHWriteFunc,
   kFHWriteFuncList,
   kFHFitPolyHist,
   kFHFitPolyMarks,
   kFHFindPeaks,
   kFHCalibrate,
   kFHDeleteCal,
   kFHShowPeaks,
   kFHRedefineAxis,
   kFHAddAxisX,
   kFHAddAxisY,
   kFHMarksToWindow,  
   kFHDrawWindows,  
   kFHClearWindows,  
   kFHWindowsToHist,  
   kFHCutsToHist,  
   kFHWriteOutWindows,
   kFHCutsFromASCII,  
   kFH_CASCADE1_0,
   kFH_CASCADE1_1,
   kFH_CASCADE1_2,
   kFH_CASCADE1_3,
   kFH_CASCADE1_4,
   kFH_CASCADE1_5,
   kFH_CASCADE1_6,
   kFH_CASCADE1_7,
   kFH_CASCADE1_8,
   kFH_CASCADE1_U,
   kFH_CASCADE2_0,
   kFH_CASCADE2_1,
   kFH_CASCADE2_2,
   kFH_CASCADE2_3,
   kFH_CASCADE2_4,
   kFH_CASCADE2_5,
   kFH_CASCADE2_6,
   kFH_CASCADE2_7,
   kFH_CASCADE2_8,
   kFH_CASCADE2_U,
};
static const char *gOpenTypes[] = { "ROOT files",   "*.root",
                                    "All files",    "*",
                                    0,              0 };

static const char *gSaveAsTypes[] = { "PostScript",   "*.ps",
                                      "Encapsulated PostScript", "*.eps",
                                      "Gif files",    "*.gif",
                                      "Macro files",  "*.C",
                                      "ROOT files",   "*.root",
                                      "All files",    "*",
                                      0,              0 };
// TGPopupMenu *oMarkMenu, *oDisplayMenu, *oCutMenu, *oFitMenu;
// FitHist *oFitHist;   
/////////////////////////////////////////////////////////////////////////
//                                                                      //
// HTRootContainer                                                       //
//                                                                      //
// Utility class used by HTRootCanvas. The HTRootContainer is the frame   //
// embedded in the TGCanvas widget. The ROOT graphics goes into this    //
// frame. This class is used to enable input events on this graphics    //
// frame and forward the events to the HTRootCanvas handlers.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class HTRootContainer : public TGCompositeFrame {
private:
   HTRootCanvas  *fCanvas;    // pointer back to canvas imp
public:
   HTRootContainer(HTRootCanvas *c, Window_t id, const TGWindow *parent);

   Bool_t  HandleButton(Event_t *ev)
                { return fCanvas->HandleContainerButton(ev); }
   Bool_t  HandleDoubleClick(Event_t *ev)
                { return fCanvas->HandleContainerDoubleClick(ev); }
   Bool_t  HandleConfigureNotify(Event_t *ev)
                { TGFrame::HandleConfigureNotify(ev);
                  return fCanvas->HandleContainerConfigure(ev); }
   Bool_t  HandleKey(Event_t *ev)
                { return fCanvas->HandleContainerKey(ev); }
   Bool_t  HandleMotion(Event_t *ev)
                { return fCanvas->HandleContainerMotion(ev); }
   Bool_t  HandleExpose(Event_t *ev)
                { return fCanvas->HandleContainerExpose(ev); }
   Bool_t  HandleCrossing(Event_t *ev)
                { return fCanvas->HandleContainerCrossing(ev); }
};

//______________________________________________________________________________
HTRootContainer::HTRootContainer(HTRootCanvas *c, Window_t id, const TGWindow *p)
   : TGCompositeFrame(gClient, id, p)
{
   // Create a canvas container.

   fCanvas = c;

   gVirtualX->GrabButton(fId, kAnyButton, kAnyModifier,
                    kButtonPressMask | kButtonReleaseMask,
                    kNone, kNone);

   gVirtualX->SelectInput(fId, kKeyPressMask | kExposureMask | kPointerMotionMask |
                     kStructureNotifyMask | kLeaveWindowMask);
}

//______________________________________________________________________________

ClassImp(HTRootCanvas)

//______________________________________________________________________________
HTRootCanvas::HTRootCanvas(HTCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height)
   : TGMainFrame(gClient->GetRoot(), width, height), HTCanvasImp(c)
{
   // Create a basic ROOT canvas.
   fHCanvas = c;
   CreateCanvas(name);

   MoveResize(x, y, width, height);
   SetWMPosition(x, y);
}

//______________________________________________________________________________

void HTRootCanvas::CreateCanvas(const char *name)
{
//  start Ottos
//   cout << "HTRootCanvas::CreateCanvas: " << name << endl;
   TString myFitHistName = name;
   myFitHistName.Remove(0,2);
//   cout <<  myFitHistName  << endl;
//   char *cn = (const char *)myFitHistName; 
   HistPresent * hpr = 0;
   hpr = fHCanvas->GetHistPresent();
   TMrbHelpBrowser * hbrowser = 0;
   if (hpr) {
     hbrowser = hpr->GetHelpBrowser();
     hpr->SetMyCanvas(this);
   }
   FitHist * oFitHist = fHCanvas->GetFitHist();
//   FitHist *oFitHist = 0;
//   if(strlen(cn) > 0)oFitHist=(FitHist*)gROOT->FindObject(cn);
//   else   cout << setred << "CreateCanvas: name too short: " 
//               << name << setblack << endl; 
   Bool_t is2dim = kFALSE;

   // Create the actual canvas.

   fButton  = 0;
   fCwidth  = 0;
   fCheight = 0;
   fAutoFit = kTRUE;   // check also menu entry
   fClassesMenu  = 0;
   fEditMenu     = 0;
   fViewMenu     = 0;
   fInspectMenu  = 0;    
  // Create menus
   fFileMenu = new TGPopupMenu(fClient->GetRoot());
//   fFileMenu->AddEntry("&New Canvas",         kFileNewCanvas);
   if(oFitHist){
      fFileMenu->AddEntry("Hist_to_ROOT-File",               kFHHistToFile);
      fFileMenu->AddEntry("Hist_to_ASCII-File"              ,kFHHistToASCII);
//      fFileMenu->AddSeparator();
//      fFileMenu->AddEntry("Picture_to_PS-File",    kFHPictToPS);
//      fFileMenu->AddEntry("Pict_to_PS-File (plain)", kFHPictToPSplain);
//      fFileMenu->AddEntry("Picture_to_Printer",      kFHPictToLP);
      fFileMenu->AddEntry("Canvas_to_ROOT-File",     kFHCanvasToFile);
//   fFileMenu->AddEntry("&Open...",            kFileOpen);
      fFileMenu->AddSeparator();
//   fFileMenu->AddEntry("Save As...",          kFileSaveAs);
   }
   fFileMenu->AddEntry("Picture_to_Printer (white bg)",  kFHCanvas2LPplain);
   fFileMenu->AddEntry("Picture_to_Printer (as it is)",  kFHCanvas2LP);
   fFileMenu->AddEntry("Picture_to_PS-File (white bg)",  kFHPictToPSplain);
   fFileMenu->AddEntry("Picture_to_PS-File (as it is)",  kFHPictToPS);
//   fFileMenu->AddEntry("Save As canvas.ps",   kFileSaveAsPS);
   fFileMenu->AddEntry("Save As canvas.eps",  kFileSaveAsEPS);
   fFileMenu->AddEntry("Save As canvas.gif",  kFileSaveAsGIF);
   fFileMenu->AddEntry("Save As canvas.C",    kFileSaveAsC);
   if(!oFitHist)fFileMenu->AddEntry("Canvas_to_ROOT-File",     kFHCanvasToFile);
//   if(!oFitHist)fFileMenu->AddEntry("Save As canvas.root", kFileSaveAsRoot);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("Read Cuts (Window2D) from ASCII file", kFHCutsFromASCII);
//   fFileMenu->AddEntry("&Print...",           kFilePrint);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Close Canvas",       kFileCloseCanvas);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("Terminate program",          kFileQuit);

   fOptionMenu = new TGPopupMenu(fClient->GetRoot());
   fOptionMenu->AddEntry("&Event Status",       kOptionEventStatus);
   fOptionMenu->AddEntry("&Pad Auto Exec",        kOptionAutoExec);
   fOptionMenu->AddSeparator();
   fOptionMenu->AddEntry("&Auto Resize Canvas", kOptionAutoResize);
   fOptionMenu->AddEntry("&Resize Canvas",      kOptionResizeCanvas);
   fOptionMenu->AddEntry("R&efresh",            kOptionRefresh);
   fOptionMenu->AddEntry("Can Edit Histograms", kOptionCanEdit);
   fOptionMenu->AddSeparator();
   fOptionMenu->AddEntry("What to display for a histgram", kOptionDisp);
   if(!oFitHist || !(oFitHist->Its2dim()))
      fOptionMenu->AddEntry("How to display a 1-dim histgram", kOption1Dim);
   if(!oFitHist || oFitHist->Its2dim())
      fOptionMenu->AddEntry("How to display a 2-dim histgram", kOption2Dim);
   fOptionMenu->AddEntry("Various HistPresent Options", kOptionHpr);
   fOptionMenu->AddEntry("HistPresent Numerical Options", kOptionNum);
   fOptionMenu->AddEntry("Default window sizes", kOptionWin);
   fOptionMenu->AddEntry("Default colors and fonts", kOptionCol);
//   fOptionMenu->AddEntry("&Statistics",         kOptionStatistics);
//   fOptionMenu->AddEntry("&Histogram Title",    kOptionHistTitle);
//   fOptionMenu->AddEntry("Show &Fit Params",    kOptionFitParams);

   fOptionMenu->CheckEntry(kOptionAutoResize);
   fOptionMenu->CheckEntry(kOptionStatistics);
   fOptionMenu->CheckEntry(kOptionHistTitle);
   if (fCanvas->GetAutoExec()) {
      fOptionMenu->CheckEntry(kOptionAutoExec);
   } else {
      fOptionMenu->UnCheckEntry(kOptionAutoExec);
   }

   if (oFitHist || fHCanvas->GetHistList()) {
   	fViewMenu = new TGPopupMenu(fClient->GetRoot());
   	if (oFitHist) {
      	oFitHist->SetMyCanvas(this);
      	is2dim = oFitHist->Its2dim();

      	fViewMenu->AddEntry("Expand",      kFHExpand     );
      	fViewMenu->AddEntry("Entire",      kFHEntire     );
      	if(!is2dim)fViewMenu->AddEntry("Rebin",       kFHRebinOne);
      	fViewMenu->AddSeparator();
      	fViewMenu->AddEntry("ClearMarks",   kFHClearMarks);
      	fViewMenu->AddEntry("PrintMarks",   kFHPrintMarks);
      	fViewMenu->AddEntry("Set2Marks",    kFHSet2Marks);
	//      fViewMenu->AddEntry("Help On Marks",         kFH_Help_Mark);
      	fViewMenu->AddSeparator();
      	if(is2dim)fViewMenu->AddEntry("ProjectX",    kFHProjectX   );
      	if(is2dim)fViewMenu->AddEntry("ProjectY",    kFHProjectY   );
      	if(is2dim)fViewMenu->AddEntry("ProjectBoth", kFHProjectB   );
      	if(is2dim)fViewMenu->AddEntry("ProjectAlongFunction",    kFHProjectF   );
      	fViewMenu->AddEntry("Superimpose", kFHSuperimpose);
      	if(!is2dim)fViewMenu->AddEntry("Superimpose scaled", kFHSuperimposeScale);
      	fViewMenu->AddEntry("Show in same Range",    kFHGetRange   );
      	fViewMenu->AddEntry("Show Statistics only",  kFHOutputStat );
      	if(!is2dim)fViewMenu->AddEntry("Show Peaks",     kFHShowPeaks);
      	fViewMenu->AddSeparator();
      	fViewMenu->AddEntry("Magnify",     kFHMagnify    );
      	fViewMenu->AddEntry("Redefine Axis",     kFHRedefineAxis);
      	fViewMenu->AddEntry("Add new X axis",     kFHAddAxisX);
      	fViewMenu->AddEntry("Add new Y axis",     kFHAddAxisY);

      	if (oFitHist->GetSelHist()->GetDimension() == 1) {
         	fViewMenu->AddEntry("Log Y scale",  kFHLogY);
         	if (oFitHist->GetLogy()) fViewMenu->CheckEntry(kFHLogY);
         	else                   fViewMenu->UnCheckEntry(kFHLogY);
      	} else if (oFitHist->GetSelHist()->GetDimension() == 2) {
         	fViewMenu->AddEntry("Log Z scale",  kFHLogY);
         	if (oFitHist->GetLogz()) fViewMenu->CheckEntry(kFHLogY);
         	else                   fViewMenu->UnCheckEntry(kFHLogY);
      	}

      	fViewMenu->AddEntry("SelectInside",  kFHSelectInside);
      	if(oFitHist->InsideState()) fViewMenu->CheckEntry(kFHSelectInside);
      	else                        fViewMenu->UnCheckEntry(kFHSelectInside);

      	if(hbrowser)hbrowser->DisplayMenu(fViewMenu, "display.html");
	//      fViewMenu->AddEntry("ProjectY",    kFHProjectY, );
   	} else if (fHCanvas->GetHistList()) {
      	fViewMenu->AddEntry("Show now all as selected, Range only", kFHAllAsSelRangeOnly);
      	fViewMenu->AddEntry("Show now all as selected, Range, Min, Max ",  kFHAllAsSel);
      	fViewMenu->AddEntry("Calibrate all as selected",  kFHCalAllAsSel);
      	fViewMenu->AddEntry("Show always all as First", kFHAllAsFirst);
         if (hpr->fShowAllAsFirst) fViewMenu->CheckEntry(kFHAllAsFirst);
         else                      fViewMenu->UnCheckEntry(kFHAllAsFirst);
      	fViewMenu->AddEntry("Rebin all",  kFHRebinAll);
      	fViewMenu->AddEntry("Activate automatic update",  kFHActivateTimer);
      	fViewMenu->AddSeparator();
	//      fViewMenu->AddEntry("Help",  kFH_Help_ShowSelected);
   	}
   	fViewMenu->AddSeparator();
   	fViewMenu->AddEntry("Show Colors",             kViewColors);
   	fViewMenu->AddEntry("Show Fonts",              kViewFonts);
   	fViewMenu->AddEntry("Graph Editor",            kEditEditor);
   }
   if(oFitHist){
      fEditMenu     = new TGPopupMenu(fClient->GetRoot());
      if(is2dim){
         fEditMenu->AddEntry("InitCut",     kFHInitCut    );
         fEditMenu->AddEntry("MarksToCut",  kFHMarksToCut);
         fEditMenu->AddEntry("ClearCut",    kFHClearCut   );
         fEditMenu->AddEntry("ListCuts",    kFHListCuts    );
         fEditMenu->AddEntry("DrawCut",     kFHDrawCut    );
         fEditMenu->AddEntry("DrawCutName", kFHDrawCutName    );
         fEditMenu->AddEntry("CutsToHist",  kFHCutsToHist   );
         fEditMenu->AddEntry("WriteOutCut", kFHWriteOutCut);
      } else {
         fEditMenu->AddEntry("MarksToWindow",   kFHMarksToWindow   );
         fEditMenu->AddEntry("ListWindows",     kFHListWindows     );
         fEditMenu->AddEntry("DrawWindows",     kFHDrawWindows     );
         fEditMenu->AddEntry("ClearWindows",    kFHClearWindows    );
         fEditMenu->AddEntry("WindowsToHist",   kFHWindowsToHist   );
         fEditMenu->AddEntry("WriteOutWindows", kFHWriteOutWindows);
      }
      fEditMenu->AddSeparator();
      fEditMenu->AddEntry("ClearMarks",   kFHClearMarks);
      fEditMenu->AddEntry("PrintMarks",   kFHPrintMarks);
      fEditMenu->AddEntry("Set2Marks",    kFHSet2Marks);
      fEditMenu->AddSeparator();
      if(hbrowser)hbrowser->DisplayMenu(fEditMenu, "cuts.html");
//      fEditMenu->AddEntry("Help On Marks",         kFH_Help_Mark);
//      fEditMenu->AddEntry("Help On Cuts/Windows",  kFH_Help_Cuts);

      fClassesMenu = new TGPopupMenu(fClient->GetRoot());
      fClassesMenu->AddEntry("Pol 0", kFH_CASCADE1_0);
      fClassesMenu->AddEntry("Pol 1", kFH_CASCADE1_1);
      fClassesMenu->AddEntry("Pol 2", kFH_CASCADE1_2);
      fClassesMenu->AddEntry("Pol 3", kFH_CASCADE1_3);
      fClassesMenu->AddEntry("Pol 4", kFH_CASCADE1_4);
      fClassesMenu->AddEntry("Pol 5", kFH_CASCADE1_5);
      fClassesMenu->AddEntry("Pol 6", kFH_CASCADE1_6);
      fClassesMenu->AddEntry("Pol 7", kFH_CASCADE1_7);
      fClassesMenu->AddEntry("Pol 8", kFH_CASCADE1_8);
      fClassesMenu->AddEntry("User Formula", kFH_CASCADE1_U);

      TGPopupMenu *fCascadeMenu2 = new TGPopupMenu(fClient->GetRoot());
      fCascadeMenu2->AddEntry("Pol 0", kFH_CASCADE2_0);
      fCascadeMenu2->AddEntry("Pol 1", kFH_CASCADE2_1);
      fCascadeMenu2->AddEntry("Pol 2", kFH_CASCADE2_2);
      fCascadeMenu2->AddEntry("Pol 3", kFH_CASCADE2_3);
      fCascadeMenu2->AddEntry("Pol 4", kFH_CASCADE2_4);
      fCascadeMenu2->AddEntry("Pol 5", kFH_CASCADE2_5);
      fCascadeMenu2->AddEntry("Pol 6", kFH_CASCADE2_6);
      fCascadeMenu2->AddEntry("Pol 7", kFH_CASCADE2_7);
      fCascadeMenu2->AddEntry("Pol 8", kFH_CASCADE2_8);
      fCascadeMenu2->AddEntry("User Formula", kFH_CASCADE2_U);

      fInspectMenu     = new TGPopupMenu(fClient->GetRoot());
      fInspectMenu->AddPopup("FitPolyHist",  fClassesMenu);
      if(is2dim){
         fInspectMenu->AddPopup("FitPolyMarks", fCascadeMenu2);
      } else {
         fInspectMenu->AddEntry("FitGauss+BgOnly", kFHFitGBgOnly);
         fInspectMenu->AddEntry("FitGauss+Low Tail",   kFHFitGBgTailLow);
         fInspectMenu->AddEntry("FitGauss+High Tail",   kFHFitGBgTailHigh);
         fInspectMenu->AddEntry("Determine linear background", kFHSetLinBg);
         fInspectMenu->AddEntry("Execute User Fit Macro", kFHFitUser);
         fInspectMenu->AddEntry("Edit User Fit Macro", kFHEditUser);
//         fInspectMenu->AddEntry("SetTemplateMacro", kFHSetTemplateMacro);
//         TGPopupMenu *fCascadeMenu3 = new TGPopupMenu(fClient->GetRoot());
//         fCascadeMenu3->AddEntry("Breit Wigner", kFHMacroBreitW);
//         fCascadeMenu3->AddEntry("Exp + Gauss ", kFHMacroExpG);
//         fCascadeMenu3->AddEntry("Two Gaussians", kFHMacroTwoG);
//         fCascadeMenu3->AddEntry("Three Landaus", kFHMacroLandau);
//         fInspectMenu->AddPopup("Select Template Fit Macro", fCascadeMenu3);
         fInspectMenu->AddSeparator();
//         fInspectMenu->AddEntry("Keep Parameters",  kFHKeepPar);
//         fInspectMenu->AddEntry("Call Minos",       kFHCallMinos);
//         fInspectMenu->AddEntry("Keep Parameters",  kFHKeepPar);
         fInspectMenu->AddEntry("Set Fitting Options",       kOptionFit);
         if(oFitHist->GetKeepPar()) fInspectMenu->CheckEntry(kFHKeepPar);
         else                       fInspectMenu->UnCheckEntry(kFHKeepPar);
         if(oFitHist->GetCallMinos()) fInspectMenu->CheckEntry(kFHCallMinos);
         else                       fInspectMenu->UnCheckEntry(kFHCallMinos);
      }
      fInspectMenu->AddSeparator();
      fInspectMenu->AddEntry("Add Functions to Hist", kFHFuncsToHist);
      fInspectMenu->AddEntry("Write Functions to File",     kFHWriteFunc);
//      fInspectMenu->AddEntry("Write Funcs as List", kFHWriteFuncList);
      fInspectMenu->AddSeparator();
      if(hbrowser)hbrowser->DisplayMenu(fInspectMenu, "fitting.html");
      
      if(!is2dim){
 //        fInspectMenu->AddEntry("Help On Fitting 1-dim",       kFH_Help_Fit1);
         fInspectMenu->AddSeparator();
         fInspectMenu->AddEntry("Calibrate",         kFHCalibrate);
         fInspectMenu->AddEntry("Clear Calibration", kFHDeleteCal);
         fInspectMenu->AddSeparator();
         fInspectMenu->AddEntry("FindPeaks",         kFHFindPeaks);
         if(hbrowser)hbrowser->DisplayMenu(fInspectMenu, "calibration.html");
      }
      fEditMenu->Associate(this);
      fClassesMenu->Associate(this);
//      fCascadeMenu1->Associate(this);
      fCascadeMenu2->Associate(this);
      fInspectMenu->Associate(this);
//      fClassesMenu->Associate(this);
   }
   if (fViewMenu) fViewMenu->Associate(this);
//  end Otto

   fHelpMenu = new TGPopupMenu(fClient->GetRoot());
   fHelpMenu->AddEntry("&About ROOT...",        kHelpAbout);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("Help On Canvas...",     kHelpOnCanvas);
   fHelpMenu->AddEntry("Help On Menus...",      kHelpOnMenus);
   fHelpMenu->AddEntry("Help On Graphics Editor...", kHelpOnGraphicsEd);
   fHelpMenu->AddEntry("Help On Browser...",    kHelpOnBrowser);
   fHelpMenu->AddEntry("Help On Objects...",    kHelpOnObjects);
   fHelpMenu->AddEntry("Help On PostScript...", kHelpOnPS);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("Help On Colors...",     kViewColors);
   fHelpMenu->AddEntry("Help On Fonts...",      kViewFonts);
   fHelpMenu->AddSeparator();

   if(hbrowser)hbrowser->DisplayMenu(fHelpMenu, "*.html");
// This main frame will process the menu commands
   fFileMenu->Associate(this);
   fOptionMenu->Associate(this);
   fHelpMenu->Associate(this);

   // Create menubar layout hints
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   // Create menubar
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout);
   fMenuBar->AddPopup("&Options", fOptionMenu,  fMenuBarItemLayout);
   if (fViewMenu) fMenuBar->AddPopup("&Display", fViewMenu,  fMenuBarItemLayout);
   if(oFitHist){
//      fMenuBar->AddPopup("&In_Output",   fClassesMenu,  fMenuBarItemLayout);
      fMenuBar->AddPopup("Cuts/Windows",    fEditMenu,  fMenuBarItemLayout);
      fMenuBar->AddPopup("Fit/Functions",     fInspectMenu,  fMenuBarItemLayout);
   }
//   fMenuBar->AddPopup("&Inspect", fInspectMenu, fMenuBarItemLayout);
//   fMenuBar->AddPopup("&Classes", fClassesMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help",    fHelpMenu,    fMenuBarHelpLayout);
//   fMenuBar->AddPopup("&Help Hpr",    fHelpMenu,    fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);
/*
   // Create canvas and canvas container that will host the ROOT graphics
   fCanvasWindow = new TGCanvas(this, GetWidth()+4, GetHeight()+4,
                                kSunkenFrame | kDoubleBorder);
   fCanvasContainer = new HTRootContainer(fCanvasWindow->GetViewPort(), this, 10, 10);
   fCanvasWindow->SetContainer(fCanvasContainer);
   fCanvasLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   AddFrame(fCanvasWindow, fCanvasLayout);
*/
 // Create canvas and canvas container that will host the ROOT graphics
   fCanvasWindow = new TGCanvas(this, GetWidth()+4, GetHeight()+4,
                                kSunkenFrame | kDoubleBorder);
   fCanvasID = gVirtualX->InitWindow((ULong_t)fCanvasWindow->GetViewPort()->GetId());
   Window_t win = gVirtualX->GetWindowID(fCanvasID);
   fCanvasContainer = new HTRootContainer(this, win, fCanvasWindow->GetViewPort());
   fCanvasWindow->SetContainer(fCanvasContainer);
   fCanvasLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   AddFrame(fCanvasWindow, fCanvasLayout);
   // Create status bar
   int parts[] = { 33, 10, 10, 47 };
   fStatusBar = new TGStatusBar(this, 10, 10);
   fStatusBar->SetParts(parts, 4);

   fStatusBarLayout = new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 2, 2, 1, 1);

   AddFrame(fStatusBar, fStatusBarLayout);

   // Misc

   SetWindowName(name);
   SetIconName(name);
   SetClassHints("Canvas", "Canvas");

   SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);

   MapSubwindows();

   // by default status bar is hidden
   HideFrame(fStatusBar);

   // we need to use GetDefaultSize() to initialize the layout algorithm...
   Resize(GetDefaultSize());
}

//______________________________________________________________________________

HTRootCanvas::~HTRootCanvas()
{
   // Delete ROOT basic canvas. Order is significant. Delete in reverse
   // order of creation.

   delete fStatusBarLayout;
   delete fStatusBar;
   delete fCanvasLayout;
   delete fCanvasContainer;
   delete fCanvasWindow;
   delete fMenuBar;
   delete fMenuBarHelpLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarLayout;
   delete fHelpMenu;
   if (fInspectMenu) delete fInspectMenu;
   if (fClassesMenu) delete fClassesMenu;
   if (fEditMenu)    delete fEditMenu;
   if (fViewMenu)    delete fViewMenu;
   delete fOptionMenu;
   delete fFileMenu;
}
//______________________________________________________________________________
void HTRootCanvas::Close()
{
   // Called via TCanvasImp interface by TCanvas.

   gVirtualX->CloseWindow();
}

//______________________________________________________________________________
void HTRootCanvas::CloseWindow()
{
   // In case window is closed via WM we get here.

   TVirtualPad *savepad = gPad;
   gPad = 0;        // hide gPad from CINT
   gInterpreter->DeleteGlobal(fCanvas);
   gPad = savepad;  // restore gPad for ROOT
   delete fCanvas;  // this in turn will delete this object
}
/*
//______________________________________________________________________________
void HTRootCanvas::CloseWindow()
{
   // In case window is closed via WM we get here.
   // Forward message to central message handler as button event.

   SendMessage(this, MK_MSG(kC_COMMAND, kCM_BUTTON), kFileCloseCanvas, 0);
}
*/
//______________________________________________________________________________
UInt_t HTRootCanvas::GetCwidth() const
{
   // Return width of canvas container.

   return fCanvasContainer->GetWidth();
}

//______________________________________________________________________________
UInt_t HTRootCanvas::GetCheight() const
{
   // Return height of canvas container.

   return fCanvasContainer->GetHeight();
}

//______________________________________________________________________________
void HTRootCanvas::GetWindowGeometry(Int_t &x, Int_t &y, UInt_t &w, UInt_t &h)
{
   // Gets the size and position of the window containing the canvas. This
   // size includes the menubar and borders.

   gVirtualX->GetWindowSize(fId, x, y, w, h);

   // Get position of window on the screen. For this we need to get the parent
   // of the ROOT canvas, i.e. the window managed by the window manager and get
   // its position
   UInt_t wdum, hdum;
   Window_t id = fId;
   do {
      gVirtualX->GetWindowSize(id, x, y, wdum, hdum);
      id = gVirtualX->GetParent(id);
   } while (id != gClient->GetRoot()->GetId());
}

//______________________________________________________________________________
void HTRootCanvas::SetStatusText(Text_t *txt, Int_t partidx)
{
   // Set text in status bar.

   fStatusBar->SetText(txt, partidx);
}
//______________________________________________________________________________
Bool_t HTRootCanvas::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.

//  find the corresponding FitHist Object 
   Bool_t autops = kFALSE;
   TMrbHelpBrowser * hbrowser = NULL;
   HistPresent *hpr = fHCanvas->GetHistPresent();
   if(hpr){
      autops = hpr->GetShowPSFile();
      hbrowser=hpr->GetHelpBrowser();
   }
   FitHist *oFitHist = NULL; 
   TList *tl=gDirectory->GetList();
   TIter next(tl);
   while(TObject* obj=next()){
      if(!(strncmp(obj->ClassName(),"FitHist",7))){
         oFitHist=(FitHist*)obj;
         if(oFitHist->GetMyCanvas() == this) break;
      }
   }
//   if(!oFitHist) return;
/*
   if(oFitHist->MarkState()){
      fClassesMenu->CheckEntry(kFHMarkon);
   } else {
      fClassesMenu->UnCheckEntry(kFHMarkon);
   }
   if(oFitHist->InsideState()){
      fClassesMenu->CheckEntry(kFHSelectInside);
   } else {
      fClassesMenu->UnCheckEntry(kFHSelectInside);
   }
*/
   Int_t np;
//   cout << fCanvasWindow->GetTitle() << endl;
   TRootHelpDialog *hd;

   switch (GET_MSG(msg)) {

      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:

               if(parm1 >= 100000){
                  Int_t ind = parm1 - 100000;
                  if( hbrowser && ind >= 0 && ind < hbrowser->GetNofItems())
                     hbrowser->DrawText(ind);
                  break;
               }
               switch (parm1) {
                  // Handle File menu items...
                  case kFileNewCanvas:
                     gROOT->GetMakeDefCanvas()();
                     break;
                  case kFileOpen:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (const char **) gOpenTypes;
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen,&fi);
                        if (!fi.fFilename) return kTRUE;
                        new TFile(fi.fFilename, "update");
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAs:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (const char **) gSaveAsTypes;
                        new TGFileDialog(fClient->GetRoot(), this, kFDSave,&fi);
                        if (!fi.fFilename) return kTRUE;
                        if (strstr(fi.fFilename, ".root") ||
                            strstr(fi.fFilename, ".ps")   ||
                            strstr(fi.fFilename, ".eps")  ||
                            strstr(fi.fFilename, ".gif"))
                           fCanvas->SaveAs(fi.fFilename);
                        else if (strstr(fi.fFilename, ".C"))
                           fCanvas->SaveSource(fi.fFilename);
                        else
                           Warning("ProcessMessage", "file cannot be save with this extension (%s)", fi.fFilename);
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAsRoot:
                     fCanvas->SaveAs(".root");
                     break;
                  case kFileSaveAsC:
                     {
//                     cout << "fCanvas->GetName() " << fCanvas->GetName()<< endl;
                     TString csave = fCanvas->GetName();
                     TString nname = fCanvas->GetName();
                     nname += "_save";
                     fCanvas->SetName(nname.Data());
                     fCanvas->SaveSource();
                     fCanvas->SetName(csave.Data());
                     }
                     break;
                  case kFileSaveAsPS:
                    fCanvas->SetFillStyle(0);
                    fCanvas->SaveAs();
                     break;
                  case kFileSaveAsEPS:
                     fCanvas->SaveAs(".eps");
                     break;
                  case kFileSaveAsGIF:
                    fCanvas->SetFillStyle(0);
                     fCanvas->SaveAs(".gif");
                     break;
                  case kFilePrint:
                    fCanvas->Print();
                    fCanvas->SetFillStyle(0);
                     break;
                  case kFileCloseCanvas:
                     delete fCanvas;  // this in turn will delete this object
                     break;
                  case kFHCutsFromASCII:
                     {
//                     HistPresent *hpr = (HistPresent*)gROOT->FindObject("mypres");
                     if(hpr)hpr->CutsFromASCII(this);
                     }
                     break;
                  case kFileQuit:
                     {
                     Int_t buttons= kMBYes | kMBNo, retval=0;
                     EMsgBoxIcon icontype = kMBIconStop;
                     new TGMsgBox(gClient->GetRoot(), this,
                      "Do you really want to quit?", 
                       "Do you really want to quit?",
               //       gClient->GetPicture("/home/rg/schaileo/myroot/xpm/rquit.xpm"),
                      icontype, buttons, &retval);
                     if(retval == kMBYes){
//                        HistPresent *hpr = fHCanvas->GetHistPresent();
//                     HistPresent *hpr = (HistPresent*)gROOT->FindObject("mypres");
                        if(hpr)hpr->CloseAllCanvases();
                        gApplication->Terminate(0);
                     }
                     }
                     break;
                  case kFHActivateTimer:
                     if (fCanvas->GetHTimer()) {
                        fCanvas->ActivateTimer(-1);    // deactivate
                        fViewMenu->UnCheckEntry(kFHActivateTimer);
                     } else { 
                     	Int_t tms = 0;
                     	if(hpr)tms = (Int_t)(1000 * hpr->GetAutoUpdateDelay());
                     	if(tms <= 0) tms = 2000;
                     	cout << "Setting AutoUpdateDelay to " << tms << endl;
                     	fCanvas->ActivateTimer(tms);    // in milli second
                     	fViewMenu->CheckEntry(kFHActivateTimer);
                     }
                     break;            
                  case kFHRebinAll:
                     RebinAll(gPad, fCanvas, 0);
                     break;
                  case kFHAllAsSelRangeOnly:
                     ShowAllAsSelected(gPad, fCanvas, 0, 
                               (TGWindow*)this);
                     break;
                  case kFHCalAllAsSel:
                     CalibrateAllAsSelected(gPad, fCanvas, 1);
                     break;

                  case kFHAllAsSel:
                     ShowAllAsSelected(gPad, fCanvas, 1);
                     break;
                  case kFHAllAsFirst:
                     if (hpr->fShowAllAsFirst) {
                        hpr->fShowAllAsFirst = 0;
                        fViewMenu->UnCheckEntry(kFHAllAsFirst);
                     } else {
                        hpr->fShowAllAsFirst = 1;
                        fViewMenu->CheckEntry(kFHAllAsFirst);
                     }
                     break;

                  case kFHCanvas2LP:
                     Canvas2LP((TCanvas*)fCanvas, "lp", this, autops);
                     break;
                  case kFHCanvas2LPplain:
                     Canvas2LP((TCanvas*)fCanvas, "plainlp", this, autops);
                     break;
                  case kFHPictToPSplain:
                     Canvas2LP((TCanvas*)fCanvas, "plainps", this, autops);
                     break;
                  case kFHPictToPS:
                     Canvas2LP((TCanvas*)fCanvas, "ps", this, autops);
                     break;

                  // Handle Edit menu items...
                  case kEditEditor:
                     fCanvas->EditorBar();
                     break;
                  case kEditUndo:
                     // noop
                     break;
                  case kEditClearPad:
                     gPad->Clear();
                     gPad->Modified();
                     gPad->Update();
                     break;
                  case kEditClearCanvas:
                     fCanvas->Clear();
                     fCanvas->Modified();
                     fCanvas->Update();
                     break;

                  // Handle View menu items...
                  case kViewColors:
                     {
                        TVirtualPad *padsav = gPad->GetCanvas();
                        TCanvas *m = new TCanvas("colors","Color Table");
                        TPad::DrawColorTable();
                        m->Update();
                        padsav->cd();                     }
                     break;
                  case kViewFonts:
                     Show_Fonts();
                     break;
                  case kViewMarkers:
                     {
                        TVirtualPad *padsav = gPad->GetCanvas();
                        TCanvas *m = new TCanvas("markers","Marker Types",600,200);
                        TMarker::DisplayMarkerTypes();
                        m->Update();
                        padsav->cd();
                     }
                     break;
                  case kViewIconify:
                     Iconify();
                     break;
                  case kViewX3D:
                     gPad->x3d();
                     break;
                  case kViewOpenGL:
                     gPad->x3d("OPENGL");
                     break;

                  // Handle Option menu items...
                  case kOptionEventStatus:
                     fCanvas->ToggleEventStatus();
                     if (fCanvas->GetShowEventStatus()) {
                        ShowFrame(fStatusBar);
                        fOptionMenu->CheckEntry(kOptionEventStatus);
                     } else {
                        HideFrame(fStatusBar);
                        fOptionMenu->UnCheckEntry(kOptionEventStatus);
                     }
                     break;
                  case kOptionAutoExec:
                     fCanvas->ToggleAutoExec();
                     if (fCanvas->GetAutoExec()) {
                        fOptionMenu->CheckEntry(kOptionAutoExec);
                     } else {
                        fOptionMenu->UnCheckEntry(kOptionAutoExec);
                     }
                     break;
                  case kOptionAutoResize:
                     {
                        fAutoFit = fAutoFit ? kFALSE : kTRUE;
                        int opt = fCanvasContainer->GetOptions();
                        if (fAutoFit) {
                           opt &= ~kFixedSize;
                           fOptionMenu->CheckEntry(kOptionAutoResize);
                        } else {
                           opt |= kFixedSize;
                           fOptionMenu->UnCheckEntry(kOptionAutoResize);
                        }
                        fCanvasContainer->ChangeOptions(opt);
                        // in case of autofit this will generate a configure
                        // event for the container and this will force the
                        // update of the TCanvas
                        Layout();
                     }
                     break;
                  case kOptionResizeCanvas:
                     FitCanvas();
                     break;
                  case kOptionRefresh:
                     fCanvas->Paint();
                     fCanvas->Update();
                     break;
                  case kOptionStatistics:
                     if (gStyle->GetOptStat()) {
                        gStyle->SetOptStat(0);
                        fOptionMenu->UnCheckEntry(kOptionStatistics);
                     } else {
                        gStyle->SetOptStat(1);
                        fOptionMenu->CheckEntry(kOptionStatistics);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionHistTitle:
                     if (gStyle->GetOptTitle()) {
                        gStyle->SetOptTitle(0);
                        fOptionMenu->UnCheckEntry(kOptionHistTitle);
                     } else {
                        gStyle->SetOptTitle(1);
                        fOptionMenu->CheckEntry(kOptionHistTitle);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionFitParams:
                     if (gStyle->GetOptFit()) {
                        gStyle->SetOptFit(0);
                        fOptionMenu->UnCheckEntry(kOptionFitParams);
                     } else {
                        gStyle->SetOptFit(1);
                        fOptionMenu->CheckEntry(kOptionFitParams);
                     }
                     gPad->Modified();
                     fCanvas->Update();
                     break;
                  case kOptionCanEdit:
                     if (gROOT->GetEditHistograms()) {
                        gROOT->SetEditHistograms(kFALSE);
                        fOptionMenu->UnCheckEntry(kOptionCanEdit);
                     } else {
                        gROOT->SetEditHistograms(kTRUE);
                        fOptionMenu->CheckEntry(kOptionCanEdit);
                     }
                     break;
                  case kOptionFit:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetFittingOptions(this, oFitHist);
                     }
                     }
                     break;
                  case kOptionDisp:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetDisplayOptions(this, oFitHist);
                        if(oFitHist){
                           oFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOption1Dim:
                     {
 //                    HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->Set1DimOptions(this, oFitHist);
                        if(oFitHist){
                           oFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOption2Dim:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->Set2DimOptions(this, oFitHist);
                        if(oFitHist){
                           oFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOptionHpr:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetVariousOptions(this, oFitHist);
                        if(oFitHist)oFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionNum:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetNumericalOptions(this, oFitHist);
                        if(oFitHist)oFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionWin:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetWindowSizes(this, oFitHist);
                        if(oFitHist)oFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionCol:
                     {
//                     HistPresent *hpr = fHCanvas->GetHistPresent();
                     if(hpr){
                        hpr->SetFontsAndColors(this, oFitHist);
                        if(oFitHist)oFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  // Handle Inspect menu items...
                  case kInspectRoot:
                     fCanvas->cd();
                     gROOT->Inspect();
                     fCanvas->Update();
                     break;
                  case kInspectBrowser:
                     new TBrowser("browser");
                     break;

                  // Handle Inspect menu items...
//                  case kClassesFullTree:
                     // noop
//                     break;
//                  case kClassesStartAt:
                     // noop
//                     break;

                  // Handle Help menu items...
                  case kHelpAbout:
                     // coming soon
                     {
                        char str[32];
                        sprintf(str, "About ROOT %s...", gROOT->GetVersion());
                        hd = new TRootHelpDialog(this, str, 600, 400);
                        hd->SetText(gHelpAbout);
                        hd->Popup();
                     }
                     break;
                  case kHelpOnCanvas:
                     hd = new TRootHelpDialog(this, "Help on Canvas...", 600, 400);
                     hd->SetText(gHelpCanvas);
                     hd->Popup();
                     break;
                  case kHelpOnMenus:
                     hd = new TRootHelpDialog(this, "Help on Menus...", 600, 400);
                     hd->SetText(gHelpPullDownMenus);
                     hd->Popup();
                     break;
                  case kHelpOnGraphicsEd:
                     hd = new TRootHelpDialog(this, "Help on Graphics Editor...", 600, 400);
                     hd->SetText(gHelpGraphicsEditor);
                     hd->Popup();
                     break;
                  case kHelpOnBrowser:
                     hd = new TRootHelpDialog(this, "Help on Browser...", 600, 400);
                     hd->SetText(gHelpBrowser);
                     hd->Popup();
                     break;
                  case kHelpOnObjects:
                     hd = new TRootHelpDialog(this, "Help on Objects...", 600, 400);
                     hd->SetText(gHelpObjects);
                     hd->Popup();
                     break;
                  case kHelpOnPS:
                     hd = new TRootHelpDialog(this, "Help on PostScript...", 600, 400);
                     hd->SetText(gHelpPostscript);
                     hd->Popup();
                     break;

                  case kFHClearMarks:
                     oFitHist->ClearMarks(); 
                     break;
                  case kFHPrintMarks:
                     oFitHist->PrintMarks(); 
                     break;
                  case kFHSet2Marks:
                     oFitHist->Set2Marks(); 
                     break;
                  case kFHSelectInside:
                     if(oFitHist->InsideState()) {
                         oFitHist->SetInside(kFALSE);
                         fViewMenu->UnCheckEntry(kFHSelectInside);
                     } else {
                         oFitHist->SetInside(kTRUE);
                         fViewMenu->CheckEntry(kFHSelectInside);
                     }
                     break;
                  case  kFHMarksToWindow:
                     oFitHist->MarksToWindow();
                     break;
                  case  kFHDrawWindows:
                     oFitHist->DrawWindows();
                     break;
                  case  kFHWindowsToHist:
                     oFitHist->AddWindowsToHist();
                     break;
                  case  kFHClearWindows:
                     oFitHist->ClearWindows();
                     break;
                  case  kFHWriteOutWindows:
                     oFitHist->WriteOutWindows();
                     break;
                  case kFHExpand:
                     oFitHist->Expand(); 
                     break;
                  case kFHEntire:
                     oFitHist->Entire(); 
                     break;
                  case kFHRebinOne:
                     oFitHist->RebinOne(); 
                     break;
                  case kFHMagnify:
                     oFitHist->Magnify(); 
                     break;
                  case kFHSuperimpose:
                     oFitHist->Superimpose(0); 
                     break;
                  case kFHSuperimposeScale:
                     oFitHist->Superimpose(1); 
                     break;
                  case kFHGetRange:
                     oFitHist->GetRange(); 
                     break;
/*
                  case kFHSetDrawMode:
                     oFitHist->SetDrawMode(); 
                     break;
                  case kFHSetLabAtt:
                     oFitHist->SetLabAtt(); 
                     break;
*/
                  case kFHLogY:
                     if (is2dim(oFitHist->GetSelHist())) {
                     	if (oFitHist->GetLogz()) {
                        	 oFitHist->SetLogz(0);
                        	 fViewMenu->UnCheckEntry(kFHLogY);
                     	} else {
                        	 oFitHist->SetLogz(1);
                        	 fViewMenu->CheckEntry(kFHLogY);
                     	}
                     } else {
                     	if (oFitHist->GetLogy()) {
                        	 oFitHist->SetLogy(0);
                        	 fViewMenu->UnCheckEntry(kFHLogY);
                     	} else {
                        	 oFitHist->SetLogy(1);
                        	 fViewMenu->CheckEntry(kFHLogY);
                     	}
                     }
                     oFitHist->SaveDefaults();
                     oFitHist->GetCanvas()->Modified(kTRUE);
                     oFitHist->GetCanvas()->Update();
                     break;
                  case kFHProjectX:
                     oFitHist->ProjectX(); 
                     break;
                  case kFHProjectY:
                     oFitHist->ProjectY(); 
                     break;
                  case kFHProjectF:
                     oFitHist->ProjectF(); 
                     break;
                  case kFHProjectB:
                     oFitHist->ProjectBoth(); 
                     break;
                  case kFHOutputStat:
                     oFitHist->OutputStat(); 
                     break;
                  case kFHHistToFile:
                      oFitHist->WriteOutHist(); 
                     break;
                  case kFHCanvasToFile:
//                      oFitHist->WriteOutCanvas(); 
                     Canvas2RootFile((TCanvas*)fCanvas, this); 
                     break;
//                  case kFHPictToPSplain:
//                      oFitHist->PictToPSFile(1); 
//                     break;
//                  case kFHPictToPS:
//                      oFitHist->PictToPSFile(0); 
//                     break;
//                  case kFHPictToLP:
//                      oFitHist->PictToLP(); 
                     break;
                  case kFHHistToASCII:
                     oFitHist->WriteHistasASCII(); 
                     break;

                  case  kFHMarksToCut:
                     oFitHist->MarksToCut();
                     break;
                  case kFHInitCut:
                     oFitHist->InitCut(); 
                     break;
                  case kFHClearCut:
                     oFitHist->ClearCut(); 
                     break;
                  case kFHListCuts:
                     oFitHist->ListCuts(); 
                     break;
                  case kFHListWindows:
                     oFitHist->ListWindows(); 
                     break;
                  case kFHDrawCut:
                     oFitHist->DrawCut();
                     break;
                  case kFHDrawCutName:
                     oFitHist->DrawCutName(); 
                     break;
                  case kFHWriteOutCut:
                     oFitHist->WriteOutCut(); 
                     break;
                  case  kFHCutsToHist:
                     oFitHist->AddCutsToHist();
                     break;

                  case kFHTerminate:
                     gApplication->Terminate(0);
                     break;

                  case kFHFitGBgOnly:
                     oFitHist->FitGBgOnly(); 
                     break;
                  case kFHFitGBgTailLow:
                     oFitHist->FitGBgTailLow(); 
                     break;
                  case kFHFitGBgTailHigh:
                     oFitHist->FitGBgTailHigh(); 
                     break;
                  case kFHFitUser:
                     oFitHist->ExecFitMacro(); 
                     break;
                  case kFHSetLinBg:
                     oFitHist->SetLinBg(); 
                     break;
                  case kFHEditUser:
                     oFitHist->EditFitMacro(); 
                     break;
                  case kFHMacroBreitW:
                     oFitHist->SetTemplateMacro("BreitWigner"); 
                     break;
                  case kFHMacroTwoG:
                     oFitHist->SetTemplateMacro("TwoGauss"); 
                     break;
                  case kFHMacroExpG:
                     oFitHist->SetTemplateMacro("ExpGauss"); 
                     break;
                  case kFHMacroLandau:
                     oFitHist->SetTemplateMacro("Landau"); 
                     break;
                  case kFHKeepPar:
                     if(oFitHist->GetKeepPar()){
                        oFitHist->SetKeepPar(kFALSE);
                        fInspectMenu->UnCheckEntry(kFHKeepPar);
                     } else  {
                        oFitHist->SetKeepPar(kTRUE);
                        fInspectMenu->CheckEntry(kFHKeepPar);
                     }
                     break;
                  case kFHCallMinos:
                     if(oFitHist->GetCallMinos()){
                        oFitHist->SetCallMinos(kFALSE);
                        fInspectMenu->UnCheckEntry(kFHCallMinos);
                     } else  {
                        oFitHist->SetCallMinos(kTRUE);
                        fInspectMenu->CheckEntry(kFHCallMinos);
                     }
                     break;
                  case kFHFuncsToHist:
                     oFitHist->AddFunctionsToHist(); 
                     break;
                  case kFHWriteFunc:
                     oFitHist->WriteFunctions(); 
                     break;
                  case kFHWriteFuncList:
                     oFitHist->WriteFunctionList(); 
                     break;
                  case kFH_CASCADE1_0:
                     oFitHist->FitPolyHist(0); 
                     break;
                  case kFH_CASCADE1_1:
                     oFitHist->FitPolyHist(1); 
                     break;
                  case kFH_CASCADE1_2:
                     oFitHist->FitPolyHist(2); 
                     break;
                  case kFH_CASCADE1_3:
                     oFitHist->FitPolyHist(3); 
                     break;
                  case kFH_CASCADE1_4:
                     oFitHist->FitPolyHist(4); 
                     break;
                  case kFH_CASCADE1_5:
                     oFitHist->FitPolyHist(5); 
                     break;
                  case kFH_CASCADE1_6:
                     oFitHist->FitPolyHist(6); 
                     break;
                  case kFH_CASCADE1_7:
                     oFitHist->FitPolyHist(7); 
                     break;
                  case kFH_CASCADE1_8:
                     oFitHist->FitPolyHist(8); 
                     break;
                  case kFH_CASCADE1_U:
                     oFitHist->FitPolyHist(-1); 
                     break;

                  case kFH_CASCADE2_0:
                     oFitHist->FitPolyMarks(0); 
                     break;
                  case kFH_CASCADE2_1:
                     oFitHist->FitPolyMarks(1); 
                     break;
                  case kFH_CASCADE2_2:
                     oFitHist->FitPolyMarks(2); 
                     break;
                  case kFH_CASCADE2_3:
                     oFitHist->FitPolyMarks(3); 
                     break;
                  case kFH_CASCADE2_4:
                     oFitHist->FitPolyMarks(4); 
                     break;
                  case kFH_CASCADE2_5:
                     oFitHist->FitPolyMarks(5); 
                     break;
                  case kFH_CASCADE2_6:
                     oFitHist->FitPolyMarks(6); 
                     break;
                  case kFH_CASCADE2_7:
                     oFitHist->FitPolyMarks(7); 
                     break;
                  case kFH_CASCADE2_8:
                     oFitHist->FitPolyMarks(8); 
                     break;
                  case kFH_CASCADE2_U:
                     oFitHist->FitPolyMarks(-1); 
                     break;

                  case kFHCalibrate:
                     oFitHist->Calibrate(); 
                     break;
                  case kFHDeleteCal:
                     oFitHist->SetDeleteCalFlag(); 
                     break;
                  case kFHFindPeaks:
                     np=oFitHist->FindPeaks(); 
                     break;
                  case kFHShowPeaks:
                     np=oFitHist->ShowPeaks(); 
                     break;
                  case kFHRedefineAxis:
                     oFitHist->RedefineAxis(); 
                     break;
                  case kFHAddAxisX:
                     oFitHist->AddAxis(1); 
                     break;
                  case kFHAddAxisY:
                     oFitHist->AddAxis(2); 
                     break;
// Otto end 
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}

//______________________________________________________________________________
Int_t HTRootCanvas::InitWindow()
{
   // Called by TCanvas ctor to get window indetifier.

   if (fCanvas->OpaqueMoving())
      fOptionMenu->CheckEntry(kOptionMoveOpaque);
   if (fCanvas->OpaqueResizing())
      fOptionMenu->CheckEntry(kOptionResizeOpaque);

   return fCanvasID;
}

//______________________________________________________________________________
void HTRootCanvas::SetCanvasSize(UInt_t w, UInt_t h)
{
   // Set size of canvas container. Unix in pixels.

   // turn off autofit, we want to stay at the given size
   fAutoFit = kFALSE;
   fOptionMenu->UnCheckEntry(kOptionAutoResize);
   int opt = fCanvasContainer->GetOptions();
   opt |= kFixedSize;    // turn on fixed size mode
   fCanvasContainer->ChangeOptions(opt);
   fCanvasContainer->SetWidth(w);
   fCanvasContainer->SetHeight(h);
   Layout();  // force layout (will update container to given size)
   fCanvas->Resize();
   fCanvas->Update();
}

//______________________________________________________________________________
void HTRootCanvas::SetLog(Int_t state)
{
    if(state > 0) fViewMenu->CheckEntry(kFHLogY);
    else          fViewMenu->UnCheckEntry(kFHLogY);
}
//______________________________________________________________________________
void HTRootCanvas::SetWindowPosition(Int_t x, Int_t y)
{
   // Set canvas position (units in pixels).

   Move(x, y);
}

//______________________________________________________________________________
void HTRootCanvas::SetWindowSize(UInt_t w, UInt_t h)
{
   // Set size of canvas( units in pixels).

   Resize(w, h);
}

//______________________________________________________________________________
void HTRootCanvas::SetWindowTitle(const char *title)
{
   // Change title on window.

   SetWindowName(title);
   SetIconName(title);
}

//______________________________________________________________________________
void HTRootCanvas::FitCanvas()
{
   // Fit canvas container to current window size.

   if (!fAutoFit) {
      int opt = fCanvasContainer->GetOptions();
      int oopt = opt;
      opt &= ~kFixedSize;   // turn off fixed size mode
      fCanvasContainer->ChangeOptions(opt);
      Layout();  // force layout
      fCanvas->Resize();
      fCanvas->Update();
      fCanvasContainer->ChangeOptions(oopt);
   }
}

//______________________________________________________________________________
void HTRootCanvas::ShowMenuBar(Bool_t show)
{
   // Show or hide menubar.

   if (show) {
      ShowFrame(fMenuBar);
   } else {
      HideFrame(fMenuBar);
   }
}

//______________________________________________________________________________
void HTRootCanvas::ShowStatusBar(Bool_t show)
{
   // Show or hide statusbar.

   if (show) {
      ShowFrame(fStatusBar);
      fOptionMenu->CheckEntry(kOptionEventStatus);
   } else {
      HideFrame(fStatusBar);
      fOptionMenu->UnCheckEntry(kOptionEventStatus);
   }
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerButton(Event_t *event)
{
   // Handle mouse button events in the canvas container.

   Int_t button = event->fCode;
   Int_t x = event->fX;
   Int_t y = event->fY;

   if (event->fType == kButtonPress) {
      fButton = button;
      if (button == kButton1)
         fCanvas->HandleInput(kButton1Down, x, y);
      if (button == kButton2)
         fHCanvas->HandleInput(kButton2Down, x, y);
      if (button == kButton3) {
         fHCanvas->HandleInput(kButton3Down, x, y);
         fButton = 0;  // button up is consumed by TContextMenu
      }

   } else if (event->fType == kButtonRelease) {
      if (button == kButton1)
         fHCanvas->HandleInput(kButton1Up, x, y);
      if (button == kButton2)
         fHCanvas->HandleInput(kButton2Up, x, y);
      if (button == kButton3)
         fHCanvas->HandleInput(kButton3Up, x, y);

      fButton = 0;
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerDoubleClick(Event_t *event)
{
   // Handle mouse button double click events in the canvas container.

   Int_t button = event->fCode;
   Int_t x = event->fX;
   Int_t y = event->fY;

   if (button == kButton1)
      fHCanvas->HandleInput(kButton1Double, x, y);
   if (button == kButton2)
      fHCanvas->HandleInput(kButton2Double, x, y);
   if (button == kButton3)
      fHCanvas->HandleInput(kButton3Double, x, y);

   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerConfigure(Event_t *)
{
   // Handle configure (i.e. resize) event.

   if (fAutoFit) {
      fHCanvas->Resize();
      fHCanvas->Update();
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerKey(Event_t *event)
{
   // Handle keyboard events in the canvas container.

  if (event->fType == kGKeyPress) {
      fButton = event->fCode;
      UInt_t keysym;
      char str[2];
      gVirtualX->LookupString(event, str, sizeof(str), keysym);
      if (str[0] == 3)   // ctrl-c sets the interrupt flag
         gROOT->SetInterrupt();
      fHCanvas->HandleInput(kKeyPress, str[0], 0);
   } else if (event->fType == kKeyRelease)
      fButton = 0;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerMotion(Event_t *event)
{
   // Handle mouse motion event in the canvas container.

   Int_t x = event->fX;
   Int_t y = event->fY;

   if (fButton == 0)
      fHCanvas->HandleInput(kMouseMotion, x, y);
   if (fButton == kButton1)
      fHCanvas->HandleInput(kButton1Motion, x, y);

   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerExpose(Event_t *event)
{
   // Handle expose events.

   if (event->fCount == 0)
      fHCanvas->Flush();

   return kTRUE;
}

//______________________________________________________________________________
Bool_t HTRootCanvas::HandleContainerCrossing(Event_t *event)
{
   // Handle enter/leave events. Only leave is activated at the moment.
   Int_t x = event->fX;
   Int_t y = event->fY;

   if (event->fType == kLeaveNotify)
      fHCanvas->HandleInput(kMouseLeave, x, y);

   return kTRUE;
}
