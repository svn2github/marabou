#include "TROOT.h"
#include "TApplication.h"
#include "TWbox.h"
#include "TButton.h"
#include "TGClient.h"
#include "TGuiFactory.h"
#include "TRootHelpDialog.h"
#include "TGMenu.h"
#include "TGWindow.h"
#include "TStyle.h"
#include "TString.h"
#include "TH1.h"
#include "THStack.h"
#include "TList.h"
#include "TMarker.h"
#include "TMath.h"
#include "TImage.h"
#include "TError.h"
#include "TContextMenu.h"

#include "HTCanvas.h"
#include "HandleMenus.h"
#include "FitHist.h"
#include "HistPresent.h"
#include "support.h"
#include "HTimer.h"
#include "SetColor.h"
#include "TMrbHelpBrowser.h" 
#include "TGMrbTableFrame.h" 

void EditFitMacroG(TGWindow * win);
void ExecFitMacroG(TGraph * graph, TGWindow * win);


enum ERootCanvasCommands {
   kFileNewCanvas,
   kFileOpen,
   kFileSaveAs,
   kFileSaveAsRoot,
   kFileSaveAsC,
   kFileSaveAsPS,
   kFileSaveAsEPS,
   kFileSaveAsPDF,
   kFileSaveAsGIF,
   kFileSaveAsJPG,
   kFilePrint,
   kFileCloseCanvas,
   kFileQuit,

   kEditEditor,
   kEditUndo,
   kEditClearPad,
   kEditClearCanvas,

   kViewEventStatus,
   kViewColors,
   kViewFonts,
   kViewMarkers,
   kViewFillStyles,
   kViewLineStyles,
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
   kOption2DimCol,
   kOptionGraph,
   kOptionHpr,
   kOptionNum,
   kOptionWin,
   kOptionCol,
   kOptionBright,
   kOptionHLS,
   kFHMarks,
   kFHMarkon,
   kFHMarkoff,
   kFHClearMarks,
   kFHPrintMarks,
   kFHSet2Marks,
   kFHColorMarked,
   kFHSelectInside,
   kFHKeepPar,
   kFHCallMinos,
 
   kFHDisplay,
   kFHExpand,
   kFHEntire,
   kFHMagnify,
   kFHSuperimpose,
   kFHKolmogorov,
   kFHSuperimposeScale,
   kFHGetRange,
//   kFHSetDrawMode,
//   kFHSetLabAtt,
   kFHLogY,
   kFHRotateClock,
   kFHRotateCClock,
   kFHTranspose,
   kFHProfileX,
   kFHProfileY,
   kFHProjectX,
   kFHProjectY,
   kFHProjectF,
   kFHProjectB,
   kFHAllAsSel,
   kFHCalAllAsSel,
   kFHCommonRotate,
   kFHAllAsFirst,
   kFHStack,
   kFHCanvas2LP,
   kFHCanvas2LPplain,
   kFHAllAsSelRangeOnly,
   kFHActivateTimer,
   kFHRebinAll,
   kFHRebinOne,
   kFHUserCont,
   kFHUserContClear,
   kFHUserContSave,
   kFHUserContUse,
//   kFHProjectY,
   kFHOutputStat,
   kFHHistToFile,
   kFHGraphToFile,
   kFHGraphToASCII,
   kFHCanvasToFile,
   kFHHistToASCII,
   kFHPictToPSplain,
   kFHPictToPS,
   kFHPictToLP,

   kFHSpectrum, 
   kFHSpectrumError, 
   kFH1dimHist, 
   kFH1dimHistWeight,
   kFH2dimHist, 
   kFH2dimHistWeight, 
   kFHGraph, 
   kFHGraphError,
   kFHGraphAsymmError,

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
   kFHFitGOnly,
   kFHFitGBg,
   kFHFitGBgTailLow,
   kFHFitGBgTailHigh,
   kFHFitGTailLow,
   kFHFitGTailHigh,
   kFHFitUserG,
   kFHEditUserG,
   kFHFitUser,
   kFHEditUser,
   kFHEditSlicesYUser,
   kFHFitSlicesYUser,
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
   kFHCalibrateNew,
   kFHDeleteCal,
   kFHShowPeaks,
   kFHRedefineAxis,
   kFHAddAxisX,
   kFHAddAxisY,
   kFHMarksToWindow,  
   kFHDrawWindows,  
   kFHDrawFunctions,  
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
   kFH_Portrait,
   kFH_UserEdit,
   kFH_Landscape,
   kFH_WritePrim,
   kFH_GetPrim,
   kFH_DrawHist,
   kFH_DrawGraph,
   kFH_InsertImage,
   kFH_InsertAxis,
   kFH_InsertLatex,
   kFH_InsertLatexF,
   kFH_SetGrid,
   kFH_UseGrid,
   kFH_DrawGrid,  
   kFH_DrawGridVis,  
   kFH_RemoveGrid,
   kFH_SetVisEnclosingCut,
   kFH_PutObjectsOnGrid,
   kFH_ExtractGObjects,
   kFH_MarkGObjects,
   kFH_InsertGObjectsG,
   kFH_InsertGObjects,
   kFH_WriteGObjects,
   kFH_DeleteObjects,
   kFH_ReadGObjects,
   kFH_ShowGallery,
   kOptionPad,
   kOptionGeneral,
   kOptionFeynman,
   kOptionTitle,
   kOptionHist,
   kOptionStat,
   kOptionStatDef,
   kOptionXaxis,
   kOptionYaxis,
   kOptionZaxis
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

ClassImp (HandleMenus)

HandleMenus::HandleMenus(HTCanvas * c, HistPresent * hpr, FitHist * fh, TGraph * graph) 
              : TGFrame(gClient->GetRoot(), 10 ,10),
                fHCanvas(c),fHistPresent(hpr), fFitHist(fh), fGraph(graph)
{
   fRootCanvas = (TRootCanvas*)fHCanvas->GetCanvasImp();

//   cout << "fRootCanvas: " << fRootCanvas << endl;
//   cout << "fRootCanvas->Canvas(): " << fRootCanvas->Canvas() << endl;
   fRootsMenuBar = fRootCanvas->GetMenuBar();
   fMenuBarItemLayout = fRootCanvas->GetMenuBarItemLayout();
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
//   MapWindow(); 
//   UnMapWindow(); 
};
HandleMenus::~HandleMenus(){};

//______________________________________________________________________________
Bool_t HandleMenus::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.

//  find the corresponding FitHist Object 
//   cout << " enter HandleMenus::ProcessMessage" << endl;
   Bool_t autops = kFALSE;
   TMrbHelpBrowser * hbrowser = NULL;
   if(fHistPresent){
      autops = fHistPresent->GetShowPSFile();
      hbrowser=fHistPresent->GetHelpBrowser();
   }
   Int_t np;
//   fHCanvas->cd();
//   cout << fCanvasWindow->GetTitle() << endl;
//   TRootHelpDialog *hd;

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
                        new TGFileDialog(fRootCanvas->GetParent(), fRootCanvas, kFDOpen,&fi);
                        if (!fi.fFilename) return kTRUE;
                        new TFile(fi.fFilename, "update");
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAs:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (const char **) gSaveAsTypes;
                        new TGFileDialog(fRootCanvas->GetParent(), fRootCanvas, kFDSave,&fi);
                        if (!fi.fFilename) return kTRUE;
                        if (strstr(fi.fFilename, ".root") ||
                            strstr(fi.fFilename, ".ps")   ||
                            strstr(fi.fFilename, ".eps")  ||
                            strstr(fi.fFilename, ".gif"))
                           fHCanvas->SaveAs(fi.fFilename);
                        else if (strstr(fi.fFilename, ".C"))
                           fHCanvas->SaveSource(fi.fFilename);
                        else
                           Warning("ProcessMessage", "file cannot be save with this extension (%s)", fi.fFilename);
                        delete [] fi.fFilename;
                     }
                     break;
                  case kFileSaveAsRoot:
                     fHCanvas->SaveAs(".root");
                     break;
                  case kFileSaveAsC:
                     {
//                     cout << "GetName() " << GetName()<< endl;
                     TString csave = GetName();
                     TString nname = GetName();
                     nname += "_save";
                     fHCanvas->SetName(nname.Data());
                     fHCanvas->SaveSource();
                     fHCanvas->SetName(csave.Data());
                     }
                     break;
                  case kFileSaveAsPS:
                    fHCanvas->SetFillStyle(0);
                    fHCanvas->SaveAs();
                     break;
                  case kFileSaveAsEPS:
                     fHCanvas->SaveAs(".eps");
                     break;
                  case kFileSaveAsPDF:
                     fHCanvas->SaveAs(".pdf");
                     break;
                  case kFileSaveAsGIF:
                    fHCanvas->SetFillStyle(0);
                     fHCanvas->SaveAs(".gif");
                     break;
                  case kFileSaveAsJPG:
                     fHCanvas->SaveAs(".jpg");
                     break;
                  case kFilePrint:
                    fHCanvas->Print();
                    fHCanvas->SetFillStyle(0);
                     break;
                  case kFileCloseCanvas:
                     delete fHCanvas;  // this in turn will delete this object
                     break;
                  case kFHCutsFromASCII:
                     {
//                     HistPresent *fHistPresent = (HistPresent*)gROOT->FindObject("mypres");
                     if(fHistPresent)fHistPresent->CutsFromASCII(fRootCanvas);
                     }
                     break;
                  case kFileQuit:
                     {
                     Int_t buttons= kMBYes | kMBNo, retval=0;
                     EMsgBoxIcon icontype = kMBIconStop;
                     new TGMsgBox(gClient->GetRoot(), fRootCanvas,
                      "Do you really want to quit?", 
                       "Do you really want to quit?",
               //       gClient->GetPicture("/home/rg/schaileo/myroot/xpm/rquit.xpm"),
                      icontype, buttons, &retval);
                     if(retval == kMBYes){
                        if(fHistPresent)fHistPresent->CloseAllCanvases();
                        gApplication->Terminate(0);
                     }
                     }
                     break;
                  case kFHActivateTimer:
                     if (fHCanvas->GetHTimer()) {
                        fHCanvas->ActivateTimer(-1);    // deactivate
                        fDisplayMenu->UnCheckEntry(kFHActivateTimer);
                     } else { 
                     	Int_t tms = 0;
                     	if(fHistPresent)tms = (Int_t)(1000 * fHistPresent->GetAutoUpdateDelay());
                     	if(tms <= 0) tms = 2000;
                     	cout << "Setting AutoUpdateDelay to " << tms << endl;
                     	fHCanvas->ActivateTimer(tms);    // in milli second
                     	fDisplayMenu->CheckEntry(kFHActivateTimer);
                     }
                     break;            
                  case kFHRebinAll:
                     RebinAll(gPad, fHCanvas, 0);
                     break;
                  case kFHAllAsSelRangeOnly:
                     ShowAllAsSelected(gPad, fHCanvas, 0, 
                               (TGWindow*)fRootCanvas);
                     break;
                  case kFHCalAllAsSel:
                     CalibrateAllAsSelected(gPad, fHCanvas, 1);
                     break;

                  case kFHAllAsSel:
                     ShowAllAsSelected(gPad, fHCanvas, 1);
                     break;
                  case kFHCommonRotate:
                     if (fHCanvas->GetCommonRotate()) {
                        fHCanvas->SetCommonRotate(kFALSE);
                        fDisplayMenu->UnCheckEntry(kFHCommonRotate);
                     } else {
                        fHCanvas->SetCommonRotate(kTRUE);
                        fDisplayMenu->CheckEntry(kFHCommonRotate);
                     }
                     break;
                  case kFHAllAsFirst:
                     if (fHistPresent->fShowAllAsFirst) {
                        fHistPresent->fShowAllAsFirst = 0;
                        fDisplayMenu->UnCheckEntry(kFHAllAsFirst);
                     } else {
                        fHistPresent->fShowAllAsFirst = 1;
                        fDisplayMenu->CheckEntry(kFHAllAsFirst);
                     }
                     break;

                  case kFHStack:
                     {
                     THStack * st = (THStack *)fHCanvas->GetListOfPrimitives()
                                               ->FindObject("hstack");
                     fHCanvas->cd();
                     if (fHistPresent->fRealStack) {
                        fHistPresent->fRealStack = 0;
                        fDisplayMenu->UnCheckEntry(kFHStack);
                        if (st) {
                           st->SetDrawOption("nostack");
//                           cout << "nostack" << endl;
                        }
                     } else {
                        fHistPresent->fRealStack = 1;
                        fDisplayMenu->CheckEntry(kFHStack);
                        if (st) { 
                          st->SetDrawOption("");
 //                         cout << "stack" << endl;
                        }
                     }
                     fHCanvas->Modified();
                     fHCanvas->Update();
                     }
                     break;

                  case kFHCanvas2LP:
                     Canvas2LP((TCanvas*)fHCanvas, "lp", fRootCanvas, autops);
                     break;
                  case kFHCanvas2LPplain:
                     Canvas2LP((TCanvas*)fHCanvas, "plainlp", fRootCanvas, autops);
                     break;
                  case kFHPictToPSplain:
                     Canvas2LP((TCanvas*)fHCanvas, "plainps", fRootCanvas, autops);
                     break;
                  case kFHPictToPS:
                     Canvas2LP((TCanvas*)fHCanvas, "ps", fRootCanvas, autops);
                     break;

                  // Handle Edit menu items...
                  case kEditEditor:
//                     fHCanvas->EditorBar();
//                     fHCanvas->ToggleEditor();
                       fRootCanvas->ShowEditor();
                       fRootCanvas->ShowToolBar();
//                     if (!fEditor) CreateEditor();
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
                     fHCanvas->Clear();
                     fHCanvas->Modified();
                     fHCanvas->Update();
                     break;
                  case kOptionRefresh:
                     fHCanvas->Paint();
                     fHCanvas->Update();
                     break;
                  case kFH_SetGrid:
                     {
                    TArrayD values(4);
                    TOrdCollection *row_lab = new TOrdCollection();
                    row_lab->Add(new TObjString("Real EditGrid X"));
                    row_lab->Add(new TObjString("Real EditGrid Y"));
                    row_lab->Add(new TObjString("Visible Grid X"));
                    row_lab->Add(new TObjString("Visible Grid Y"));
                    values[0] = fHCanvas->GetEditGridX();
                    values[1] = fHCanvas->GetEditGridY();
                    values[2] = fHCanvas->GetVisibleGridX();
                    values[3] = fHCanvas->GetVisibleGridY();
   					  Int_t ret, itemwidth = 240, precission = 5;
   					  TGMrbTableOfDoubles(fRootCanvas, &ret,  "Edit Grid", itemwidth, 
                                       1, 4, values, precission, 0, row_lab);
   					  if (ret >= 0) {
                       fHCanvas->SetEditGridX(values[0]);
                       fHCanvas->SetEditGridY(values[1]);
                       fHCanvas->SetVisibleGridX(values[2]);
                       fHCanvas->SetVisibleGridY(values[3]);
                       fHCanvas->SetUseEditGrid(kTRUE);
                       fHCanvas->DrawEditGrid(kTRUE);
                    }
                    }
                    break;
                  case kFH_UseGrid:
     					if (fHCanvas->GetUseEditGrid()) {
                        fEditMenu->UnCheckEntry(kFH_UseGrid);
                        fHCanvas->SetUseEditGrid(kFALSE);
      					} else {                    
                        fEditMenu->CheckEntry(kFH_UseGrid);
                        fHCanvas->SetUseEditGrid(kTRUE);
                     }
                     break;

                  case kFH_DrawGrid:  
                        fHCanvas->DrawEditGrid(kFALSE);
                     break;

                  case kFH_DrawGridVis:  
                        fHCanvas->DrawEditGrid(kTRUE);
                     break;

                  case kFH_RemoveGrid:
                        fHCanvas->RemoveEditGrid();
                     break;
                  case kFH_SetVisEnclosingCut:
                        if (fEditMenu->IsEntryChecked(kFH_SetVisEnclosingCut)) {
                           fEditMenu->UnCheckEntry(kFH_SetVisEnclosingCut);
                           fHCanvas->SetVisibilityOfEnclosingCuts(kFALSE);
                        } else {
                           fEditMenu->CheckEntry(kFH_SetVisEnclosingCut);
                           fHCanvas->SetVisibilityOfEnclosingCuts(kTRUE);
                        }
                     break;
                  case kFH_PutObjectsOnGrid:
                        fHCanvas->PutObjectsOnGrid();
                     break;

                  case kFH_DrawHist:
                        fHCanvas->DrawHist();
                     break;
                  case kFH_DrawGraph:
                        fHCanvas->DrawGraph();
                     break;

                  case kFH_InsertLatex:
                        fHCanvas->Latex2Root(kFALSE);
                     break;
                  case kFH_InsertLatexF:
                        fHCanvas->Latex2Root(kTRUE);
                     break;
                  case kFH_InsertImage:
                        fHCanvas->InsertImage();
                     break;
                  case kFH_InsertAxis:
                        fHCanvas->InsertAxis();
                     break;
                  case kFH_WritePrim:
                        fHCanvas->WritePrimitives();
                     break;

                  case kFH_GetPrim:
                        fHCanvas->GetPrimitives();
                     break;
                  case kFH_InsertGObjects:
                       fHCanvas->InsertGObjects(NULL);
                     break;
                  case kFH_InsertGObjectsG:
                     {
                     Int_t temp =0;
                     if (fHCanvas->fInsertMacrosAsGroup) {
                        fEditMenu->UnCheckEntry(kFH_InsertGObjectsG);
                        fHCanvas->fInsertMacrosAsGroup = kFALSE;

                     } else {
                        fEditMenu->CheckEntry(kFH_InsertGObjectsG);
                        fHCanvas->fInsertMacrosAsGroup = kTRUE;
                        temp = 1;
                     } 
                     TEnv env(".rootrc");
                     env.SetValue("HistPresent.InsertMacrosAsGroup", temp);
                     env.SaveLevel(kEnvUser);
                     }                   
                     break;
                  case kFH_MarkGObjects:
                       fHCanvas->ExtractGObjects(kTRUE);
                     break;
                  case kFH_ExtractGObjects:
                       fHCanvas->ExtractGObjects(kFALSE);
                     break;
                  case kFH_WriteGObjects:
                       fHCanvas->WriteGObjects();
                     break;
                  case kFH_DeleteObjects:
                       fHCanvas->DeleteObjects();
                     break;
                  case kFH_ReadGObjects:
                       fHCanvas->ReadGObjects();
                     break;
                  case kFH_ShowGallery:
                       fHCanvas->ShowGallery();
                     break;
                  case    kFH_Portrait:
                     if(fHistPresent)
                        fHistPresent->DinA4Page(0);                    
                     break;
                  case    kFH_Landscape:
                     if(fHistPresent)
                        fHistPresent->DinA4Page(1);                       
                     break;
                  case    kFH_UserEdit:
                     if(fHistPresent)
                        fHistPresent->DinA4Page(2);                    
                     break;

                  // Handle View menu items...
                  case kViewEventStatus:
                     fHCanvas->ToggleEventStatus();
                     fRootCanvas->ShowStatusBar(fHCanvas->GetShowEventStatus());
                     break;
                  case kViewColors:
                     DrawColors(); 
                     break;
                   case kViewFillStyles:
                     DrawFillStyles(); 
                     break;
                   case kViewLineStyles:
                     DrawLineStyles(); 
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
                  case kOptionFit:
                     {
                     if(fHistPresent){
                        fHistPresent->SetFittingOptions(fRootCanvas, fFitHist);
                     }
                     }
                     break;
                  case kOptionDisp:
                     {
                     if(fHistPresent){
                        fHistPresent->SetDisplayOptions(fRootCanvas, fFitHist);
                        if(fFitHist){
                           fFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOption1Dim:
                     {
                     if(fHistPresent){
                        fHistPresent->Set1DimOptions(fRootCanvas, fFitHist);
                        if(fFitHist){
                           fFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOption2Dim:
                     {
                     if(fHistPresent){
                        fHistPresent->Set2DimOptions(fRootCanvas, fFitHist);
                        if(fFitHist){
                           fFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOption2DimCol:
                     {
                     if(fHistPresent){
                        fHistPresent->Set2DimColorOpt(fRootCanvas, fFitHist);
                        if(fFitHist){
                           fFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOptionGraph:
                     {
                     if(fHistPresent){
                        fHistPresent->SetGraphOptions(fRootCanvas, fHCanvas);
                     }
                     }
                     break;
                  case kOptionHpr:
                     {
                     if(fHistPresent){
                        fHistPresent->SetVariousOptions(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionNum:
                     {
                     if(fHistPresent){
                        fHistPresent->SetNumericalOptions(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionWin:
                     {
                     if(fHistPresent){
                        fHistPresent->SetWindowSizes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionCol:
                     {
                     if(fHistPresent){
                        fHistPresent->SetFontsAndColors(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionBright:
                     {
                     if(fFitHist && fHistPresent){
                        fFitHist->SetBrightness(fRootCanvas);
                     }
                     }
                     break;
                  case kOptionHLS:
                     {
                     if(fFitHist && fHistPresent){
                        fFitHist->SetHLS(fRootCanvas);
                     }
                     }
                     break;
                  case kOptionGeneral:
                     {
                     if(fHistPresent){
                        fHistPresent->SetGeneralAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionFeynman:
                     {
                     if(fHistPresent){
                        fHistPresent->SetCurlyAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionPad:
                     {
                     if(fHistPresent){
                        fHistPresent->SetPadAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionTitle:
                     {
                     if(fHistPresent){
                        fHistPresent->SetTitleAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionHist:
                     {
                     if(fHistPresent){
                        fHistPresent->SetHistAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionStatDef:
                     {
                     if(fHistPresent){
                        fHistPresent->SetStatDefaults(fHCanvas);
                     }
                     }
                     break;
                  case kOptionStat:
                     {
                     if(fHistPresent){
                        fHistPresent->SetStatAttributes(fRootCanvas, fFitHist);
                        if (fFitHist) {
                           if (gStyle->GetOptStat() != 0) {
                              fFitHist->GetSelHist()->SetStats(0);
                              fFitHist->GetSelHist()->SetStats(1);
                           }
                           fFitHist->UpdateCanvas();
                        }
                     }
                     }
                     break;
                  case kOptionXaxis:
                     {
                     if(fHistPresent){
                        fHistPresent->SetXaxisAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionYaxis:
                     {
                     if(fHistPresent){
                        fHistPresent->SetYaxisAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kOptionZaxis:
                     {
                     if(fHistPresent){
                        fHistPresent->SetZaxisAttributes(fRootCanvas, fFitHist);
                        if(fFitHist)fFitHist->UpdateCanvas();
                     }
                     }
                     break;
                  case kFHClearMarks:
                     fFitHist->ClearMarks(); 
                     break;
                  case kFHPrintMarks:
                     fFitHist->PrintMarks(); 
                     break;
                  case kFHSet2Marks:
                     fFitHist->Set2Marks(); 
                     break;
                  case kFHColorMarked:
                     fFitHist->ColorMarked(); 
                     break;
                  case kFHSelectInside:
                     if(fFitHist->InsideState()) {
                         fFitHist->SetInside(kFALSE);
                         fDisplayMenu->UnCheckEntry(kFHSelectInside);
                     } else {
                         fFitHist->SetInside(kTRUE);
                         fDisplayMenu->CheckEntry(kFHSelectInside);
                     }
                     break;
                  case  kFHMarksToWindow:
                     fFitHist->MarksToWindow();
                     break;
                  case  kFHDrawWindows:
                     fFitHist->DrawWindows();
                     break;
                  case  kFHDrawFunctions:
                     fFitHist->DrawSelectedFunctions();
                     break;
                  case  kFHWindowsToHist:
                     fFitHist->AddWindowsToHist();
                     break;
                  case  kFHClearWindows:
                     fFitHist->ClearWindows();
                     break;
                  case  kFHWriteOutWindows:
                     fFitHist->WriteOutWindows();
                     break;
                  case kFHExpand:
                     fFitHist->Expand(); 
                     break;
                  case kFHEntire:
                     fFitHist->Entire(); 
                     break;
                  case kFHRebinOne:
                     fFitHist->RebinOne(); 
                     break;
                  case kFHUserContUse:
                     fFitHist->UseSelectedContour(); 
                     break;
                  case kFHUserCont:
                     fFitHist->SetUserContours(); 
                     break;
                  case kFHUserContClear:
                     fFitHist->ClearUserContours(); 
                     break;
                  case kFHUserContSave:
                     fFitHist->SaveUserContours(); 
                     break;
                  case kFHMagnify:
                     fFitHist->Magnify(); 
                     break;
                  case kFHSuperimpose:
                     fFitHist->Superimpose(0); 
                     break;
                  case kFHKolmogorov:
                     fFitHist->KolmogorovTest(); 
                     break;
                  case kFHSuperimposeScale:
                     fFitHist->Superimpose(1); 
                     break;
                  case kFHGetRange:
                     fFitHist->GetRange(); 
                     break;
                  case kFHLogY:
                     if (is2dim(fFitHist->GetSelHist())) {
                     	if (fFitHist->GetLogz()) {
                        	 fFitHist->SetLogz(0);
                        	 fDisplayMenu->UnCheckEntry(kFHLogY);
                     	} else {
                        	 fFitHist->SetLogz(1);
                        	 fDisplayMenu->CheckEntry(kFHLogY);
                     	}
                     } else {
                     	if (fFitHist->GetLogy()) {
                        	 fFitHist->SetLogy(0);
                        	 fDisplayMenu->UnCheckEntry(kFHLogY);
                     	} else {
                        	 fFitHist->SetLogy(1);
                        	 fDisplayMenu->CheckEntry(kFHLogY);
                     	}
                     }
                     fFitHist->SaveDefaults();
                     fFitHist->GetCanvas()->Modified(kTRUE);
                     fFitHist->GetCanvas()->Update();
                     break;
                  case kFHRotateClock:
                     fFitHist->Rotate(0); 
                     break;
                  case kFHRotateCClock:
                     fFitHist->Rotate(1); 
                     break;
                  case kFHTranspose:
                     fFitHist->Transpose(); 
                     break;
                  case kFHProfileX:
                     fFitHist->ProfileX(); 
                     break;
                  case kFHProfileY:
                     fFitHist->ProfileY(); 
                     break;
                  case kFHProjectX:
                     fFitHist->ProjectX(); 
                     break;
                  case kFHProjectY:

                     fFitHist->ProjectY(); 
                     break;
                  case kFHProjectF:
                     fFitHist->ProjectF(); 
                     break;
                  case kFHProjectB:
                     fFitHist->ProjectBoth(); 
                     break;
                  case kFHOutputStat:
                     fFitHist->OutputStat(); 
                     break;
                  case kFHHistToFile:
                      fFitHist->WriteOutHist(); 
                     break;
                  case kFHGraphToFile:
                     WriteOutGraph(fGraph, fRootCanvas); 
                     break;
                  case kFHGraphToASCII:
                     WriteGraphasASCII(fGraph, fRootCanvas); 
                     break;
                  case kFHCanvasToFile:
//                      fFitHist->WriteOutCanvas(); 
                     Canvas2RootFile((TCanvas*)fHCanvas, fRootCanvas); 
                     break;
                  case kFHHistToASCII:
                     fFitHist->WriteHistasASCII(0); 
                     break;

                  case kFHSpectrum:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::kSpectrum); 
                     break;
                  case kFHSpectrumError:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::kSpectrumError); 
                     break;
                  case kFH1dimHist:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::k1dimHist); 
                     break;
                  case kFH1dimHistWeight:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::k1dimHistWeight); 
                     break;
                 case kFH2dimHist:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::k2dimHist); 
                     break;
                  case kFH2dimHistWeight:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::k2dimHistWeight); 
                     break;
                  case kFHGraph:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::kGraph); 
                     break;
                  case kFHGraphError:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::kGraphError); 
                     break;
                  case kFHGraphAsymmError:
                     fHistPresent->HistFromASCII(fRootCanvas, HistPresent::kGraphAsymmError); 
                     break;

                  case  kFHMarksToCut:
                     fFitHist->MarksToCut();
                     break;
                  case kFHInitCut:
                     fFitHist->InitCut(); 
                     break;
                  case kFHClearCut:
                     fFitHist->ClearCut(); 
                     break;
                  case kFHListCuts:
                     fFitHist->ListCuts(); 
                     break;
                  case kFHListWindows:
                     fFitHist->ListWindows(); 
                     break;
                  case kFHDrawCut:
                     fFitHist->DrawCut();
                     break;
                  case kFHDrawCutName:
                     fFitHist->DrawCutName(); 
                     break;
                  case kFHWriteOutCut:
                     fFitHist->WriteOutCut(); 
                     break;
                  case  kFHCutsToHist:
                     fFitHist->AddCutsToHist();
                     break;

                  case kFHTerminate:
                     gApplication->Terminate(0);
                     break;

                  case kFHFitGOnly:
                     fFitHist->FitGBg(0, 1); 
                     break;
                   case kFHFitGBg:
                     fFitHist->FitGBg(0, 0); 
                     break;
                 case kFHFitGBgTailLow:
                     fFitHist->FitGBg(1, 0); 
                     break;
                  case kFHFitGBgTailHigh:
                     fFitHist->FitGBg(2, 0); 
                     break;
                 case kFHFitGTailLow:
                     fFitHist->FitGBg(1, 1); 
                     break;
                  case kFHFitGTailHigh:
                     fFitHist->FitGBg(2, 1); 
                     break;
                  case kFHFitUserG:
                     ExecFitMacroG(fGraph, fRootCanvas);   // global function !! 
                     break;
                  case kFHFitUser:
                     fFitHist->ExecFitMacro(); 
                     break;
                  case kFHSetLinBg:
                     fFitHist->SetLinBg(); 
                     break;
                  case kFHFitSlicesYUser:
                     fFitHist->ExecFitSliceYMacro(); 
                     break;
                  case kFHEditSlicesYUser:
                     fFitHist->EditFitSliceYMacro(); 
                     break;
                  case kFHEditUserG:
                     EditFitMacroG(fRootCanvas);    // global function !!
                     break;
                  case kFHEditUser:
                     fFitHist->EditFitMacro(); 
                     break;
                  case kFHMacroBreitW:
                     fFitHist->SetTemplateMacro("BreitWigner"); 
                     break;
                  case kFHMacroTwoG:
                     fFitHist->SetTemplateMacro("TwoGauss"); 
                     break;
                  case kFHMacroExpG:
                     fFitHist->SetTemplateMacro("ExpGauss"); 
                     break;
                  case kFHMacroLandau:
                     fFitHist->SetTemplateMacro("Landau"); 
                     break;
                  case kFHKeepPar:
                     if(fFitHist->GetKeepPar()){
                        fFitHist->SetKeepPar(kFALSE);
                        fFitMenu->UnCheckEntry(kFHKeepPar);
                     } else  {
                        fFitHist->SetKeepPar(kTRUE);
                        fFitMenu->CheckEntry(kFHKeepPar);
                     }
                     break;
                  case kFHCallMinos:
                     if(fFitHist->GetCallMinos()){
                        fFitHist->SetCallMinos(kFALSE);
                        fFitMenu->UnCheckEntry(kFHCallMinos);
                     } else  {
                        fFitHist->SetCallMinos(kTRUE);
                        fFitMenu->CheckEntry(kFHCallMinos);
                     }
                     break;
                  case kFHFuncsToHist:
                     fFitHist->AddFunctionsToHist(); 
                     break;
                  case kFHWriteFunc:
                     fFitHist->WriteFunctions(); 
                     break;
                  case kFHWriteFuncList:
                     fFitHist->WriteFunctionList(); 
                     break;
                  case kFH_CASCADE1_0:
                     fFitHist->FitPolyHist(0); 
                     break;
                  case kFH_CASCADE1_1:
                     fFitHist->FitPolyHist(1); 
                     break;
                  case kFH_CASCADE1_2:
                     fFitHist->FitPolyHist(2); 
                     break;
                  case kFH_CASCADE1_3:
                     fFitHist->FitPolyHist(3); 
                     break;
                  case kFH_CASCADE1_4:
                     fFitHist->FitPolyHist(4); 
                     break;
                  case kFH_CASCADE1_5:
                     fFitHist->FitPolyHist(5); 
                     break;
                  case kFH_CASCADE1_6:
                     fFitHist->FitPolyHist(6); 
                     break;
                  case kFH_CASCADE1_7:
                     fFitHist->FitPolyHist(7); 
                     break;
                  case kFH_CASCADE1_8:
                     fFitHist->FitPolyHist(8); 
                     break;
                  case kFH_CASCADE1_U:
                     fFitHist->FitPolyHist(-1); 
                     break;

                  case kFH_CASCADE2_0:
                     fFitHist->FitPolyMarks(0); 
                     break;
                  case kFH_CASCADE2_1:
                     fFitHist->FitPolyMarks(1); 
                     break;
                  case kFH_CASCADE2_2:
                     fFitHist->FitPolyMarks(2); 
                     break;
                  case kFH_CASCADE2_3:
                     fFitHist->FitPolyMarks(3); 
                     break;
                  case kFH_CASCADE2_4:
                     fFitHist->FitPolyMarks(4); 
                     break;
                  case kFH_CASCADE2_5:
                     fFitHist->FitPolyMarks(5); 
                     break;
                  case kFH_CASCADE2_6:
                     fFitHist->FitPolyMarks(6); 
                     break;
                  case kFH_CASCADE2_7:
                     fFitHist->FitPolyMarks(7); 
                     break;
                  case kFH_CASCADE2_8:
                     fFitHist->FitPolyMarks(8); 
                     break;
                  case kFH_CASCADE2_U:
                     fFitHist->FitPolyMarks(-1); 
                     break;

                  case kFHCalibrate:
                     fFitHist->Calibrate(0); 
                     break;
                  case kFHCalibrateNew:
                     fFitHist->Calibrate(1); 
                     break;
                  case kFHDeleteCal:
                     fFitHist->ClearCalibration(); 
//                     fFitHist->SetDeleteCalFlag(); 
                     break;
                  case kFHFindPeaks:
                     np=fFitHist->FindPeaks(); 
                     break;
                  case kFHShowPeaks:
                     np=fFitHist->ShowPeaks(); 
                     break;
                  case kFHRedefineAxis:
                     fFitHist->RedefineAxis(); 
                     break;
                  case kFHAddAxisX:
                     fFitHist->AddAxis(1); 
                     break;
                  case kFHAddAxisY:
                     fFitHist->AddAxis(2); 
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

void HandleMenus::BuildMenus() 
{
   fRootsMenuBar = ((TRootCanvas*)fHCanvas->GetCanvasImp())->GetMenuBar();
/*
   cout << "fRootsMenuBar: " << fRootsMenuBar<< endl;
   cout << "fMenuBarItemLayout: " << fMenuBarItemLayout<< endl;
   cout << "fRootCanvas: " << fRootCanvas<< endl;
   cout << "fHistPresent: " << fHistPresent<< endl;
   cout << "fFitHist: " << fFitHist<< endl;
*/
   TMrbHelpBrowser * hbrowser = NULL;
   Bool_t fh_menus = kFALSE;
   Bool_t edit_menus = kFALSE;
   Bool_t autops = kFALSE;
   if(fHistPresent){
      autops = fHistPresent->GetShowPSFile();
      hbrowser=fHistPresent->GetHelpBrowser();
      if (fHCanvas->TestBit(HTCanvas::kIsAEditorPage)) edit_menus = kTRUE;
   }
   if (fFitHist != 0 && fFitHist->GetSelHist()->GetDimension() != 3) { 
     fh_menus = kTRUE;
//     edit_menus = kTRUE;
   }
   TGPopupMenu * pm; 
   const TList * l;
   TGMenuEntry * e;

   pm = fRootsMenuBar->GetPopup("Help");
   if (pm) {
      l = pm->GetListOfEntries();
      if (l) {
         TIter next(l);
         while ( (e = (TGMenuEntry *)next()) ) {
            pm->DeleteEntry(e);
         }
         fRootsMenuBar->RemovePopup("Help");
      }
   }
   pm = fRootsMenuBar->GetPopup("File");
   if (pm) {
      l = pm->GetListOfEntries();
      if (l) {
         TIter next(l);
         while ( (e = (TGMenuEntry *)next()) ) {
            pm->DeleteEntry(e);
         }
         fRootsMenuBar->RemovePopup("File");
      }
   }
   pm = fRootsMenuBar->GetPopup("Edit");
   if (pm) {
      l = pm->GetListOfEntries();
      if (l) {
         TIter next(l);
         while ( (e = (TGMenuEntry *)next()) ) {
            pm->DeleteEntry(e);
         }
         fRootsMenuBar->RemovePopup("Edit");
      }
   }
   pm = fRootsMenuBar->GetPopup("Classes");
   if (pm) {
      l = pm->GetListOfEntries();
      if (l) {
         TIter next(l);
         while ( (e = (TGMenuEntry *)next()) ) {
            pm->DeleteEntry(e);
         }
         fRootsMenuBar->RemovePopup("Classes");
      }
   }
   pm = fRootsMenuBar->GetPopup("View");
   if (pm) {
      l = pm->GetListOfEntries();
      if (l) {
         TIter next(l);
         while ( (e = (TGMenuEntry *)next()) ) {
            pm->DeleteEntry(e);
         }
         fRootsMenuBar->RemovePopup("View");
      }
   }
   if (!fFitHist) {

      pm = fRootsMenuBar->GetPopup("Options");
      if (pm) {
         l = pm->GetListOfEntries();
         if (l) {
            TIter next(l);
            while ( (e = (TGMenuEntry *)next()) ) {
               pm->DeleteEntry(e);
            }
            fRootsMenuBar->RemovePopup("Options");
         }
      }

   }
   if (fFitHist) {
           pm = fRootsMenuBar->GetPopup("Inspect");
      if (pm) {
         l = pm->GetListOfEntries();
         if (l) {
            TIter next(l);
            while ( (e = (TGMenuEntry *)next()) ) {
               pm->DeleteEntry(e);
            }
            fRootsMenuBar->RemovePopup("Inspect");
         }
      }
   }
   fCascadeMenu1 = NULL;
   fCutsMenu     = NULL;
   fViewMenu     = NULL;
   fDisplayMenu  = NULL;
   fFitMenu      = NULL; 
   fOptionMenu   = NULL; 
   fAttrMenu     = NULL; 
  
   Bool_t is2dim = kFALSE;   
   TGPopupMenu * pmo = fRootsMenuBar->GetPopup("Options");
   if (pmo) {
      pmo = fRootsMenuBar->RemovePopup("Options");
      fRootsMenuBar->AddPopup("ROOT-Options", pmo, fMenuBarItemLayout);
      e = pmo->GetEntry("Pad Auto Exec");
      if (e && fHCanvas->GetAutoExec())pmo->CheckEntry(e->GetEntryId());
   }
   TGPopupMenu * pmi = fRootsMenuBar->GetPopup("Inspect");
   if (pmi == 0) pmi = pmo;
  // Create menus
   fFileMenu = new TGPopupMenu(fRootCanvas->GetParent());
//   fFileMenu->AddEntry("&New Canvas",         kFileNewCanvas);
   if(fHistPresent){
      if (fFitHist) {
         fFileMenu->AddEntry("Hist_to_ROOT-File",             kFHHistToFile);
         fFileMenu->AddEntry("Hist_to_ASCII-File", kFHHistToASCII);
      }
//      fFileMenu->AddSeparator();
//      fGraph = FindGraph(fHCanvas);
      if (fGraph) {
         fFileMenu->AddEntry("Graph_to_ROOT-File",      kFHGraphToFile);
         fFileMenu->AddEntry("Graph_to_ASCII-File",     kFHGraphToASCII);
      }
      if (!edit_menus) {
         TGPopupMenu * HfromAMenu = new TGPopupMenu(fRootCanvas->GetParent());
         HfromAMenu->AddEntry("Channel contents, (Spectrum)",   kFHSpectrum); 
         HfromAMenu->AddEntry("Channel contents, error",   kFHSpectrumError); 
         HfromAMenu->AddEntry("X-values of histogram",   kFH1dimHist); 
         HfromAMenu->AddEntry("X-values, weights of histogram",	kFH1dimHistWeight);
         HfromAMenu->AddEntry("X-, Y-values of 2dim histogram",	kFH2dimHist); 
         HfromAMenu->AddEntry("X-, Y-values, weights values of 2dim histogram",	kFH2dimHistWeight); 

         TGPopupMenu * GfromAMenu = new TGPopupMenu(fRootCanvas->GetParent());
         GfromAMenu->AddEntry("X, Y of TGraph (without errors)",	kFHGraph); 
         GfromAMenu->AddEntry("X, Y, ErrorX, ErrorY of TGraphErrors",	kFHGraphError);
         GfromAMenu->AddEntry("X, Y, EXlow, EXhigh, EYlow, EYhigh of TGraphAsymmErrors",	kFHGraphAsymmError);
         fFileMenu->AddPopup("ASCII data from file to histogram ",  HfromAMenu);
         fFileMenu->AddPopup("ASCII data from file to graph",  GfromAMenu);

         HfromAMenu->Associate((TGWindow*)this);
         GfromAMenu->Associate((TGWindow*)this);
      }
      fFileMenu->AddSeparator();
   }
   fFileMenu->AddEntry("Picture_to_Printer (white bg)",  kFHCanvas2LPplain);
   fFileMenu->AddEntry("Picture_to_Printer (as it is)",  kFHCanvas2LP);
   fFileMenu->AddEntry("Picture_to_PS-File (white bg)",  kFHPictToPSplain);
   fFileMenu->AddEntry("Picture_to_PS-File (as it is)",  kFHPictToPS);
//   fFileMenu->AddEntry("Save As canvas.ps",   kFileSaveAsPS);
   fFileMenu->AddEntry("Save As .eps",  kFileSaveAsEPS);
   fFileMenu->AddEntry("Save As .pdf",  kFileSaveAsPDF);
   fFileMenu->AddEntry("Save As .gif",  kFileSaveAsGIF);
   static Int_t img = 0;

   if (!img) {
      Int_t sav = gErrorIgnoreLevel;
      gErrorIgnoreLevel = kFatal;
      img = TImage::Create() ? 1 : -1;
      gErrorIgnoreLevel = sav;
   }
   if (img > 0) {
      fFileMenu->AddEntry("Save As .jpg",  kFileSaveAsJPG);
   }
   fFileMenu->AddEntry("Save As .C",    kFileSaveAsC);
   if (!edit_menus) {
//   if(!fFitHist)fFileMenu->AddEntry("Canvas_to_ROOT-File",     kFHCanvasToFile);
//   if(!fFitHist)fFileMenu->AddEntry("Save As canvas.root", kFileSaveAsRoot);
      fFileMenu->AddSeparator();
      fFileMenu->AddEntry("Read Cuts (Window2D) from ASCII file", kFHCutsFromASCII);
//   fFileMenu->AddEntry("&Print...",           kFilePrint);
      fFileMenu->AddSeparator();
      fFileMenu->AddEntry("&Close Canvas",       kFileCloseCanvas);
      fFileMenu->AddEntry("Open Edit canvas (A4 portrait)",    kFH_Portrait);
      fFileMenu->AddEntry("Open Edit canvas (A4 landscape)",   kFH_Landscape);
      fFileMenu->AddEntry("Open Edit canvas (user defined)",   kFH_UserEdit);
   } else {
      fFileMenu->AddSeparator();
    	fFileMenu->AddEntry("Write objects to file",  kFH_WritePrim);
   }
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("Terminate program",          kFileQuit);

   fOptionMenu = new TGPopupMenu(fRootCanvas->GetParent());
   fOptionMenu->AddEntry("What to display for a histgram", kOptionDisp);
   if(!fFitHist || !(fFitHist->Its2dim()))
      fOptionMenu->AddEntry("How to display a 1-dim histgram", kOption1Dim);
   if(!fFitHist || fFitHist->Its2dim()){
      fOptionMenu->AddEntry("Display mode of 2-dim histgram ", kOption2Dim);
      fOptionMenu->AddEntry("Color mode of 2-dim histgram", kOption2DimCol);
   }
   fOptionMenu->AddEntry("How to display a graph", kOptionGraph);
   fOptionMenu->AddEntry("Various HistPresent Options", kOptionHpr);
   fOptionMenu->AddEntry("HistPresent Numerical Options", kOptionNum);
   fOptionMenu->AddEntry("Default window sizes", kOptionWin);
   fOptionMenu->AddEntry("Brightness of rgb", kOptionBright);
   fOptionMenu->AddEntry("Lightness+Saturation (HLS)", kOptionHLS);
   fOptionMenu->AddEntry("Default colors and fonts", kOptionCol);
   fAttrMenu = new TGPopupMenu(fRootCanvas->GetParent());
   fAttrMenu->AddEntry("Histogram / function", kOptionHist);
   fAttrMenu->AddEntry("Statistics box", kOptionStat);
   fAttrMenu->AddEntry("Title attributes", kOptionTitle);
   fAttrMenu->AddEntry("X axis attributes", kOptionXaxis);
   fAttrMenu->AddEntry("Y axis attributes", kOptionYaxis);
   fAttrMenu->AddEntry("Z axis attributes", kOptionZaxis);
   fAttrMenu->AddEntry("Line - Text - Markers - Fill", kOptionGeneral);
   fAttrMenu->AddEntry("Feynman diagrams", kOptionFeynman);
   fAttrMenu->AddEntry("Canvas, Pad, Frame", kOptionPad);
   fOptionMenu->AddPopup("Graphics Attr (Canvas, Pads, Hist, Axis, etc)",  fAttrMenu);

   fAttrMenuDef = new TGPopupMenu(fRootCanvas->GetParent());
   fAttrMenuDef->AddEntry("Take and set Stat box defaults", kOptionStatDef);
   fOptionMenu->AddPopup(" Take and set defaults",  fAttrMenuDef);

   fOptionMenu->CheckEntry(kOptionAutoResize);
   fOptionMenu->CheckEntry(kOptionStatistics);
   fOptionMenu->CheckEntry(kOptionHistTitle);
   if (fHCanvas->GetAutoExec()) {
      fOptionMenu->CheckEntry(kOptionAutoExec);
   } else {
      fOptionMenu->UnCheckEntry(kOptionAutoExec);
   }

   fViewMenu = new TGPopupMenu(fRootCanvas->GetParent());
   if (!edit_menus)fViewMenu->AddEntry("Launch Graphics Editor",        kEditEditor);
   fViewMenu->AddEntry("Event &Status", kViewEventStatus);
   fViewMenu->AddEntry("Draw selected Functions",     kFHDrawFunctions);
   fViewMenu->AddSeparator();
   fViewMenu->AddEntry("Show Colors",             kViewColors);
   fViewMenu->AddEntry("Show Fonts",              kViewFonts);
   fViewMenu->AddEntry("Show Markers",            kViewMarkers);
   fViewMenu->AddEntry("Show Fillstyles",         kViewFillStyles);
   fViewMenu->AddEntry("Show Line Attr",          kViewLineStyles);
   
   if (fh_menus || fHCanvas->GetHistList()) {
   	fDisplayMenu = new TGPopupMenu(fRootCanvas->GetParent());
   	if (fh_menus) {
      	fFitHist->SetMyCanvas(fRootCanvas);
      	is2dim = fFitHist->Its2dim();

      	fDisplayMenu->AddEntry("Expand / Apply cuts",      kFHExpand     );
      	fDisplayMenu->AddEntry("Entire / Ignore cuts",      kFHEntire     );
      	if(!is2dim)fDisplayMenu->AddEntry("Rebin",       kFHRebinOne);
      	if (is2dim) {
      	   fDisplayMenu->AddSeparator();
            fDisplayMenu->AddEntry("Set User Contours",   kFHUserCont);
            fDisplayMenu->AddEntry("Use Selected Contours",   kFHUserContUse);
            fDisplayMenu->AddEntry("Clear User Contours",   kFHUserContClear);
            fDisplayMenu->AddEntry("Save User Contours",   kFHUserContSave);
         }
      	fDisplayMenu->AddSeparator();
      	fDisplayMenu->AddEntry("ClearMarks",   kFHClearMarks);
      	fDisplayMenu->AddEntry("PrintMarks",   kFHPrintMarks);
      	fDisplayMenu->AddEntry("Set2Marks",    kFHSet2Marks);
      	fDisplayMenu->AddEntry("Highlight marked area",    kFHColorMarked);
	//      fDisplayMenu->AddEntry("Help On Marks",         kFH_Help_Mark);
      	fDisplayMenu->AddSeparator();
        
      	if(is2dim) {
            fDisplayMenu->AddEntry("ProjectX",    kFHProjectX   );
      	   fDisplayMenu->AddEntry("ProjectY",    kFHProjectY   );
      	   fDisplayMenu->AddEntry("ProjectBoth", kFHProjectB   );
      	   fDisplayMenu->AddEntry("ProjectAlongFunction",    kFHProjectF   );
      	   fDisplayMenu->AddEntry("ProfileX",    kFHProfileX   );
      	   fDisplayMenu->AddEntry("ProfileY",    kFHProfileY   );
      	   fDisplayMenu->AddEntry("Transpose",   kFHTranspose  );
      	   fDisplayMenu->AddEntry("Rotate Clockwise", kFHRotateClock);
      	   fDisplayMenu->AddEntry("Rotate Counter Clockwise", kFHRotateCClock);
         } else {
      	   fDisplayMenu->AddEntry("Superimpose scaled", kFHSuperimposeScale);
         }
      	fDisplayMenu->AddEntry("Superimpose", kFHSuperimpose);
      	fDisplayMenu->AddEntry("Show in same Range",    kFHGetRange   );

      	fDisplayMenu->AddSeparator();
      	fDisplayMenu->AddEntry("Show Statistics only",  kFHOutputStat );
      	fDisplayMenu->AddEntry("SelectInside",  kFHSelectInside);
      	if(fFitHist->InsideState()) fDisplayMenu->CheckEntry(kFHSelectInside);
      	else                        fDisplayMenu->UnCheckEntry(kFHSelectInside);
      	fDisplayMenu->AddSeparator();
      	if(!is2dim)fDisplayMenu->AddEntry("Show Peaks",     kFHShowPeaks);
      	fDisplayMenu->AddSeparator();
      	fDisplayMenu->AddEntry("Magnify",     kFHMagnify    );
      	fDisplayMenu->AddEntry("Redefine Axis",     kFHRedefineAxis);
      	fDisplayMenu->AddEntry("Add new X axis",     kFHAddAxisX);
      	fDisplayMenu->AddEntry("Add new Y axis",     kFHAddAxisY);

      	if (fFitHist->GetSelHist()->GetDimension() == 1) {
         	fDisplayMenu->AddEntry("Log Y scale",  kFHLogY);
         	if (fFitHist->GetLogy()) fDisplayMenu->CheckEntry(kFHLogY);
         	else                   fDisplayMenu->UnCheckEntry(kFHLogY);
      	} else if (fFitHist->GetSelHist()->GetDimension() == 2) {
         	fDisplayMenu->AddEntry("Log Z scale",  kFHLogY);
         	if (fFitHist->GetLogz()) fDisplayMenu->CheckEntry(kFHLogY);
         	else                   fDisplayMenu->UnCheckEntry(kFHLogY);
      	}


      	if(hbrowser)hbrowser->DisplayMenu(fDisplayMenu, "display.html");
	//      fDisplayMenu->AddEntry("ProjectY",    kFHProjectY, );
   	} else if (fHCanvas->GetHistList()) {
         if (!strncmp(fHCanvas->GetName(), "cmany", 5)) {
      	   fDisplayMenu->AddEntry("Show now all as selected, Range only", kFHAllAsSelRangeOnly);
      	   fDisplayMenu->AddEntry("Show now all as selected, Range, Min, Max ",  kFHAllAsSel);
      	   fDisplayMenu->AddEntry("Calibrate all as selected",  kFHCalAllAsSel);
      	   fDisplayMenu->AddEntry("Show always all as First", kFHAllAsFirst);
            if (fHistPresent->fShowAllAsFirst) fDisplayMenu->CheckEntry(kFHAllAsFirst);
            else                      fDisplayMenu->UnCheckEntry(kFHAllAsFirst);
      	   fDisplayMenu->AddEntry("Rebin all",  kFHRebinAll);
      	   fDisplayMenu->AddSeparator();
         	fDisplayMenu->AddEntry("Activate automatic update",  kFHActivateTimer);
         	fDisplayMenu->AddEntry("Activate simultanous rotation", kFHCommonRotate);
         } else if (!strncmp(fHCanvas->GetName(), "cstack", 5)){
      	   fDisplayMenu->AddEntry("Real stack", kFHStack);
            if (fHistPresent->fRealStack) fDisplayMenu->CheckEntry(kFHStack);
            else                          fDisplayMenu->UnCheckEntry(kFHStack);
            
         }
      	fDisplayMenu->AddSeparator();
	//      fDisplayMenu->AddEntry("Help",  kFH_Help_ShowSelected);
   	}
   }
   if(fh_menus){
      fCutsMenu     = new TGPopupMenu(fRootCanvas->GetParent());
      if(is2dim){
         fCutsMenu->AddEntry("InitCut",     kFHInitCut    );
         fCutsMenu->AddEntry("MarksToCut",  kFHMarksToCut);
         fCutsMenu->AddEntry("ClearCut",    kFHClearCut   );
         fCutsMenu->AddEntry("ListCuts",    kFHListCuts    );
         fCutsMenu->AddEntry("DrawCut",     kFHDrawCut    );
         fCutsMenu->AddEntry("DrawCutName", kFHDrawCutName    );
         fCutsMenu->AddEntry("CutsToHist",  kFHCutsToHist   );
         fCutsMenu->AddEntry("WriteOutCuts", kFHWriteOutCut);
      } else {
         fCutsMenu->AddEntry("MarksToWindow",   kFHMarksToWindow   );
         fCutsMenu->AddEntry("ListWindows",     kFHListWindows     );
         fCutsMenu->AddEntry("DrawWindows",     kFHDrawWindows     );
         fCutsMenu->AddEntry("ClearWindows",    kFHClearWindows    );
         fCutsMenu->AddEntry("WindowsToHist",   kFHWindowsToHist   );
         fCutsMenu->AddEntry("WriteOutWindows", kFHWriteOutWindows);
      }
      fCutsMenu->AddSeparator();
      fCutsMenu->AddEntry("ClearMarks",   kFHClearMarks);
      fCutsMenu->AddEntry("PrintMarks",   kFHPrintMarks);
      fCutsMenu->AddEntry("Set2Marks",    kFHSet2Marks);
      fCutsMenu->AddSeparator();
      if(hbrowser)hbrowser->DisplayMenu(fCutsMenu, "cuts.html");
//      fCutsMenu->AddEntry("Help On Marks",         kFH_Help_Mark);
//      fCutsMenu->AddEntry("Help On Cuts/Windows",  kFH_Help_Cuts);

      fCascadeMenu1 = new TGPopupMenu(fRootCanvas->GetParent());
      fCascadeMenu1->AddEntry("Pol 0", kFH_CASCADE1_0);
      fCascadeMenu1->AddEntry("Pol 1", kFH_CASCADE1_1);
      fCascadeMenu1->AddEntry("Pol 2", kFH_CASCADE1_2);
      fCascadeMenu1->AddEntry("Pol 3", kFH_CASCADE1_3);
      fCascadeMenu1->AddEntry("Pol 4", kFH_CASCADE1_4);
      fCascadeMenu1->AddEntry("Pol 5", kFH_CASCADE1_5);
      fCascadeMenu1->AddEntry("Pol 6", kFH_CASCADE1_6);
      fCascadeMenu1->AddEntry("Pol 7", kFH_CASCADE1_7);
      fCascadeMenu1->AddEntry("Pol 8", kFH_CASCADE1_8);
      fCascadeMenu1->AddEntry("User Formula", kFH_CASCADE1_U);

      fCascadeMenu2 = new TGPopupMenu(fRootCanvas->GetParent());
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

      fFitMenu     = new TGPopupMenu(fRootCanvas->GetParent());
      fFitMenu->AddPopup("FitPolyHist",  fCascadeMenu1);
      if(is2dim){
         fFitMenu->AddPopup("FitPolyMarks", fCascadeMenu2);
      	fFitMenu->AddSeparator();
      	fFitMenu->AddEntry("Edit User Fit Slices Y Macro", kFHEditSlicesYUser);
      	fFitMenu->AddEntry("Execute User FitSlices Y Macro", kFHFitSlicesYUser);
      	fFitMenu->AddSeparator();
      } else {
         fFitMenu->AddEntry("Fit Gauss Only",             kFHFitGOnly);
         fFitMenu->AddEntry("Fit Gauss + Bg",             kFHFitGBg);
         fFitMenu->AddEntry("Fit Gauss + Low Tail",       kFHFitGTailLow);
         fFitMenu->AddEntry("Fit Gauss + High Tail",      kFHFitGTailHigh);
         fFitMenu->AddEntry("Fit Gauss + Bg + Low Tail",  kFHFitGBgTailLow);
         fFitMenu->AddEntry("Fit Gauss + Bg + High Tail", kFHFitGBgTailHigh);
         fFitMenu->AddEntry("Determine linear background", kFHSetLinBg);
         fFitMenu->AddSeparator();
         fFitMenu->AddEntry("Set Fitting Options",       kOptionFit);
         if(fFitHist->GetKeepPar()) fFitMenu->CheckEntry(kFHKeepPar);
         else                       fFitMenu->UnCheckEntry(kFHKeepPar);
         if(fFitHist->GetCallMinos()) fFitMenu->CheckEntry(kFHCallMinos);
         else                       fFitMenu->UnCheckEntry(kFHCallMinos);
      }
      fFitMenu->AddEntry("Edit User Fit Macro", kFHEditUser);
      fFitMenu->AddEntry("Execute User Fit Macro", kFHFitUser);
      fFitMenu->AddSeparator();

      fFitMenu->AddEntry("Add Functions to Hist", kFHFuncsToHist);
      fFitMenu->AddEntry("Write Functions to File",     kFHWriteFunc);
      fFitMenu->AddEntry("Draw selected Functions",     kFHDrawFunctions);
      fFitMenu->AddSeparator();

  
      if(hbrowser)hbrowser->DisplayMenu(fFitMenu, "fitting.html");
      
      if(!is2dim){
 //        fFitMenu->AddEntry("Help On Fitting 1-dim",       kFH_Help_Fit1);
         fFitMenu->AddSeparator();
         fFitMenu->AddEntry("Calibrate, Generate new hist",         kFHCalibrateNew);
         fFitMenu->AddEntry("Calibrate, change scale only",         kFHCalibrate);
         fFitMenu->AddEntry("Clear Calibration", kFHDeleteCal);
         fFitMenu->AddSeparator();
         fFitMenu->AddEntry("FindPeaks",         kFHFindPeaks);
         if(hbrowser)hbrowser->DisplayMenu(fFitMenu, "calibration.html");
      }
      fFitMenu->AddSeparator();
      fFitMenu->AddEntry("Kolmogorov Test",         kFHKolmogorov);

      fCutsMenu->Associate((TGWindow*)this);
      fCascadeMenu1->Associate((TGWindow*)this);
      fCascadeMenu2->Associate((TGWindow*)this);
//      fCascadeMenu1->Associate(this);
   }

   if (fGraph) {
      fFitMenu     = new TGPopupMenu(fRootCanvas->GetParent());
      fFitMenu->AddEntry("Edit User Fit Macro", kFHEditUserG);
      fFitMenu->AddEntry("Execute User Fit Macro", kFHFitUserG);
      fFitMenu->AddSeparator();
   }
   if(edit_menus){
      fEditMenu     = new TGPopupMenu(fRootCanvas->GetParent());
//   	fEditMenu->AddEntry("Launch Graphics Editor",        kEditEditor);
   	fEditMenu->AddEntry("Write this picture to root file",  kFH_WritePrim);
      fEditMenu->AddSeparator();

   	fEditMenu->AddEntry("Insert  image (gif, jpg) into selected pad", kFH_InsertImage);
   	fEditMenu->AddEntry("Insert selected histogram into selected pad",  kFH_DrawHist);
   	fEditMenu->AddEntry("Insert graph into seperate pad",  kFH_DrawGraph);
      fEditMenu->AddSeparator();
   	fEditMenu->AddEntry("Insert text (Latex) from file", kFH_InsertLatexF);
   	fEditMenu->AddEntry("Insert text (Latex) from keyboard", kFH_InsertLatex);
   	fEditMenu->AddEntry("Draw an axis", kFH_InsertAxis);
   	fEditMenu->AddEntry("Insert macro object",  kFH_InsertGObjects);
      fEditMenu->AddSeparator();
//   	fEditMenu->AddEntry("Keep connection when inserting macro objects", kFH_InsertGObjectsG);
//      if (fHCanvas->fInsertMacrosAsGroup) fEditMenu->CheckEntry(kFH_InsertGObjectsG);
//      else                                fEditMenu->UnCheckEntry(kFH_InsertGObjectsG);
   	fEditMenu->AddEntry("Write macro objects to file",  kFH_WriteGObjects);
   	fEditMenu->AddEntry("Read macro objects from file",  kFH_ReadGObjects);
   	fEditMenu->AddEntry("Display list of macro objects",  kFH_ShowGallery);
      fEditMenu->AddSeparator();
   	fEditMenu->AddEntry("Mark selected objects as compound",  kFH_MarkGObjects);
   	fEditMenu->AddEntry("Extract selected objects as compound",  kFH_ExtractGObjects);
   	fEditMenu->AddEntry("Delete selected objects",  kFH_DeleteObjects);
//   	fEditMenu->AddEntry("Get objects from file",  kFH_GetPrim);
     
//   	fEditMenu->AddEntry("Clear Pad",              kEditClearPad);
//   	fEditMenu->AddEntry("Clear Canvas",           kEditClearCanvas);
//      fEditMenu->AddEntry("Refresh",                kOptionRefresh);

      fEditMenu->AddSeparator();
   	fEditMenu->AddEntry("Set Edit Grid",           kFH_SetGrid);
   	fEditMenu->AddEntry("Use Edit Grid",           kFH_UseGrid);
   	fEditMenu->AddEntry("Align objects at grid",     kFH_PutObjectsOnGrid);
      if (fHCanvas->GetUseEditGrid()) fEditMenu->CheckEntry(kFH_UseGrid);
      else                      fEditMenu->UnCheckEntry(kFH_UseGrid);
//      fEditMenu->AddEntry("Edit User Fit Macro",     kFHEditUser);
   	fEditMenu->AddEntry("Draw visible grid",        kFH_DrawGridVis);
   	fEditMenu->AddEntry("Draw real grid",           kFH_DrawGrid  );
   	fEditMenu->AddEntry("Remove Edit Grid",         kFH_RemoveGrid);
   	fEditMenu->AddEntry("Make EnclosingCut visible", kFH_SetVisEnclosingCut);
      fEditMenu->CheckEntry(kFH_SetVisEnclosingCut);
      fEditMenu->AddSeparator();
      fEditMenu->AddEntry("Line- Text- Fill attributes", kOptionGeneral);
      fEditMenu->AddEntry("Feynman diagram attributes", kOptionFeynman);
      fEditMenu->AddSeparator();
//   	fEditMenu->AddEntry("Show Colors",  			  kViewColors);
//   	fEditMenu->AddEntry("Show Fonts",				  kViewFonts);
//   	fEditMenu->AddEntry("Show Markers", 			  kViewMarkers);
//      fEditMenu->AddEntry("Show Fillstyles",         kViewFillStyles);
//      fEditMenu->AddEntry("Show Line Attr",          kViewLineStyles);
      fEditMenu->Associate((TGWindow*)this);
   }
   if (fFitMenu)   fFitMenu->Associate((TGWindow*)this);
   if (fDisplayMenu) fDisplayMenu->Associate((TGWindow*)this);
   if (fViewMenu) fViewMenu->Associate((TGWindow*)this);
// this main frame will process the menu commands
   fFileMenu->Associate((TGWindow*)this);
   fOptionMenu->Associate((TGWindow*)this);
   fAttrMenu->Associate((TGWindow*)this);
   fAttrMenu->Associate((TGWindow*)this);

   fRootsMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout, pmi);
   fRootsMenuBar->AddPopup("&Hpr-Options", fOptionMenu,  fMenuBarItemLayout, pmi);
   if (fViewMenu) fRootsMenuBar->AddPopup("&View", fViewMenu,  fMenuBarItemLayout, pmi);
   if (fDisplayMenu) fRootsMenuBar->AddPopup("&Display", fDisplayMenu,  fMenuBarItemLayout, pmi);
   if(fh_menus){
      fRootsMenuBar->AddPopup("Cuts/Windows",    fCutsMenu,  fMenuBarItemLayout, pmi);
      fRootsMenuBar->AddPopup("Fit / Calibrate", fFitMenu,   fMenuBarItemLayout, pmi);
   }
   if(edit_menus){
         fRootsMenuBar->AddPopup("Edit",            fEditMenu,  fMenuBarItemLayout, pmi);
   }
   if (fGraph) {
      fRootsMenuBar->AddPopup("Fit", fFitMenu,   fMenuBarItemLayout, pmi);
   } 
   if(hbrowser) {
      fHelpMenu     = new TGPopupMenu(fRootCanvas->GetParent());
      hbrowser->DisplayMenu(fHelpMenu, "*.html");
      fHelpMenu->Associate((TGWindow*)this);
      fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
      fRootsMenuBar->AddPopup("&Help",    fHelpMenu,    fMenuBarHelpLayout);
   }
   fRootCanvas->MapSubwindows();
//   fRootCanvas->Resize(fRootCanvas->GetDefaultSize());
//   fRootCanvas->ShowEditor(kFALSE);
//   fRootCanvas->ShowToolBar(kFALSE);
   fRootCanvas->MapWindow();
   fRootCanvas->ForceUpdate();
   return;
}
//______________________________________________________________________________
void HandleMenus::SetLog(Int_t state)
{
    if(state > 0) fDisplayMenu->CheckEntry(kFHLogY);
    else          fDisplayMenu->UnCheckEntry(kFHLogY);
}
//______________________________________________________________________________


