#include "TG3DLine.h"
#include "TGToolBar.h"
#include "TGDockableFrame.h"

#include "GEdit.h"

#include "TDialogCanvas.h"
#include "TGMrbInputDialog.h"
#include "InsertArcDialog.h"
#include "InsertAxisDialog.h"
#include "InsertTextBoxDialog.h"
#include "InsertPadDialog.h"
#include "InsertTextDialog.h"
#include "ChangeTextDialog.h"
#include "TSplineXDialog.h"
#include "InsertFunctionDialog.h"
#include "InsertImageDialog.h"
#include "InsertHistGraphDialog.h"
#include "FeynmanDiagramDialog.h"
#include "CurlyLineWithArrowDialog.h"
#include "MarkerLineDialog.h"

enum ERootCanvasCommands {
   kOptionRefresh,
   kToolLine,
   kToolArrow,
   kToolDiamond,
   kToolEllipse,
   kToolPolyLine,
   kToolAxis,
   kToolTSplineX,
   kToolPad,
   kToolPave,
   kToolGraph,
   kToolHist,
   kToolCurlyLine,
   kToolCurlyLineArrow,
   kToolLatex,
   kToolLatexFile,
   kToolFunction,
   kToolPicture,
   kToolMarker,
   kToolPregion,
   kToolRregion,
   kToolUseGrid,
   kToolRemoveGrid,
   kToolUnZoom,
   kToolZoomIn,
   kToolZoomOut
};
//______________________________________________________________________________
static ToolBarData_t gHprToolBarData[] = {
   // { filename,      tooltip,            staydown,  id,              button}
   { "refresh2.xpm",  "Refresh",          kFALSE,    kOptionRefresh,  0 },
   { "pregion.xpm",    "Define Polygon Region",     kFALSE,    kToolPregion,  0 },
   { "rregion.xpm",   "Define Rectangular Region", kFALSE,    kToolRregion,  0 },
   { "usegrid.xpm",   "Use / Draw Grid",           kFALSE,    kToolUseGrid,  0 },
   { "removegrid.xpm","Remove Grid",               kFALSE,    kToolRemoveGrid, 0 },
   { "zoom_in.xpm",   "Zoom in",           kFALSE,    kToolZoomIn,  0 },
   { "zoom_out.xpm",  "Zoom out",          kFALSE,    kToolZoomOut,  0 },
   { "unzoom.xpm",    "UnZoom",            kFALSE,    kToolUnZoom,  0 },
   { "",               "",                 kFALSE,    -1,              0 },
   { 0,                0,                  kFALSE,    0,               0 }
};
//______________________________________________________________________________

static ToolBarData_t gHprToolBarData1[] = {
   { "line.xpm",       "Line",             kFALSE,    kToolLine,       0 },
   { "arrow.xpm",      "Arrow",            kFALSE,    kToolArrow,      0 },
   { "marker.xpm",     "Marker",           kFALSE,    kToolMarker,     0 },
   { "ellipse.xpm",    "Circle / Arc / Ellipse",    kFALSE,    kToolEllipse,    0 },
   { "graph.xpm",      "Polyline",         kFALSE,    kToolPolyLine,   0 },
   { "gaxis.xpm",      "Axis",             kFALSE,    kToolAxis,   0 },
   { "curlyline.xpm",  "Feynman Diagram",  kFALSE,    kToolCurlyLine,  0 },
   { "curlyline_arrow.xpm","Curlyline with Arrow",  kFALSE,    kToolCurlyLineArrow,  0 },
   { "tsplinex.xpm",   "TSplineX",         kFALSE,    kToolTSplineX,  0 },
   { "pad.xpm",        "Graphics Pad",     kFALSE,    kToolPad,        0 },
   { "pave.xpm",       "TextBox",          kFALSE,    kToolPave,       0 },
   { "latex_keyboard.xpm", "Text/Latex form keyboard", kFALSE,  kToolLatex, 0 },
   { "latex_file.xpm",     "Text/Latex from file",    kFALSE,  kToolLatexFile,0 },
   { "h1_t.xpm",     "Histogram from file",           kFALSE,  kToolHist,      0 },
   { "graph_file.xpm",     "Graph from file",         kFALSE,  kToolGraph,      0 },
   { "function.xpm",       "Function",                kFALSE,  kToolFunction,0 },
   { "picture.xpm",        "Picture, JPEG, GIF, PNG", kFALSE,  kToolPicture,0 },
   { 0,                0,                  kFALSE,    0,               0 }
};

//______________________________________________________________________________
void GEdit::ShowToolBar(Bool_t show)
{
   // Show or hide toolbar.
   fToolDock = fRootCanvas->GetToolDock();
   if ( !fToolDock ) {
      cout << "Cant GetToolDock()" << endl;
      return;
   }
   if (show && !fToolBar) {
      TGLayoutHints *fHorizontal1Layout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
      fToolBarSep = new TGHorizontal3DLine(fRootCanvas);
      fToolBar = new TGToolBar(fToolDock, 60, 20, kHorizontalFrame);
      fToolDock->AddFrame(fToolBar, fHorizontal1Layout);

      Int_t spacing = 6, i;
      for (i = 0; gHprToolBarData[i].fPixmap; i++) {
         if (strlen(gHprToolBarData[i].fPixmap) == 0) {
            spacing = 6;
            continue;
         }
         TGButton * button =
         fToolBar->AddButton(fRootCanvas, &gHprToolBarData[i], spacing);
         button->Associate(this);
         spacing = 0;
      }
      fVertical1 = new TGVertical3DLine(fToolBar);
      fVertical2 = new TGVertical3DLine(fToolBar);
      fVertical1Layout = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 4,2,0,0);
      fVertical2Layout = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
      fToolBar->AddFrame(fVertical1, fVertical1Layout);
      fToolBar->AddFrame(fVertical2, fVertical2Layout);

      spacing = 6;
      for (i = 0; gHprToolBarData1[i].fPixmap; i++) {
         if (strlen(gHprToolBarData1[i].fPixmap) == 0) {
            spacing = 6;
            continue;
         }
         TGButton * button =
         fToolBar->AddButton(fRootCanvas, &gHprToolBarData1[i], spacing);
         button->Associate(this);
         spacing = 0;
      }
      fToolDock->MapSubwindows();
      fToolDock->Layout();
      fToolDock->SetWindowName(Form("ToolBar: %s", fRootCanvas->GetWindowName()));
      fToolDock->Connect("Docked()", "GEdit", this, "AdjustSize()");
      fToolDock->Connect("Undocked()", "GEdit", this, "AdjustSize()");
   }

   if (!fToolBar) return;

   UInt_t h = fRootCanvas->GetHeight();
   UInt_t sh = fToolBarSep->GetHeight();
   UInt_t dh = fToolBar->GetHeight();

   if (show) {
      fRootCanvas->ShowFrame(fToolDock);
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         fRootCanvas->ShowFrame(fHorizontal1);
//         h = h + sh;
//      }
      fRootCanvas->ShowFrame(fToolBarSep);
//      fViewMenu->CheckEntry(kViewToolbar);
      h = h + dh + sh;
   } else {
      if (fToolDock->IsUndocked()) {
         fToolDock->DockContainer();
         h = h + 2*sh;
      } else h = h - dh;

      fRootCanvas->HideFrame(fToolDock);
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         HideFrame(fHorizontal1);
//         h = h - sh;
//      }
      fRootCanvas->HideFrame(fToolBarSep);
      h = h - sh;
//      fViewMenu->UnCheckEntry(kViewToolbar);
   }
   fRootCanvas->Resize(fRootCanvas->GetWidth(), h);
}

//______________________________________________________________________________
void GEdit::AdjustSize()
{
   // Keep the same canvas size while docking/undocking toolbar.
   if ( !fHorizontal1 ) return;
   UInt_t h = fRootCanvas->GetHeight();
   UInt_t dh = fToolBar->GetHeight();
   UInt_t sh = fHorizontal1->GetHeight();

   if (fToolDock->IsUndocked()) {
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         fRootCanvas->HideFrame(fHorizontal1);
//         h = h - sh;
//      }
      fRootCanvas->HideFrame(fToolBarSep);
      h = h - dh - sh;
   } else {
//      if (!fViewMenu->IsEntryChecked(kViewEditor)) {
//         fRootCanvas->ShowFrame(fHorizontal1);
//         h = h + sh;
//      }
      fRootCanvas->ShowFrame(fToolBarSep);
      h = h + dh + sh;
   }
   fRootCanvas->Resize(fRootCanvas->GetWidth(), h);
}
//______________________________________________________________________________
Bool_t GEdit::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.
   cout << "GEdit::ProcessMessage: " << parm1 << " gPad " << gPad->GetName()<< endl;
//   fParent->cd();
   switch (GET_MSG(msg)) {

      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:

               switch (parm1) {
                  // Handle toolbar items...
                  case kOptionRefresh:
                     fParent->Paint();
                     fParent->Update();
                     break;

                  case kToolPregion:
                     DefinePolygone();
                     break;
                  case kToolRregion:
                     DefineBox();
                     break;
                  case kToolUseGrid:
                     SetUseEditGrid(1);
                     DrawEditGrid(kTRUE);
                     break;
                  case kToolRemoveGrid:
                     SetUseEditGrid(0);
                     RemoveEditGrid();
                     break;

                   case kToolZoomIn:
                     ZoomIn();
                     break;

                 case kToolZoomOut:
                     ZoomOut();
                     break;

                  case kToolUnZoom:
                     UnZoom();
                     break;

                  case kToolLine:
                    new MarkerLineDialog();
                     break;
                  case kToolAxis:
                    new InsertAxisDialog();
                     break;
                  case kToolPolyLine:
                    new MarkerLineDialog();
                     break;
                  case kToolArrow:
                     new MarkerLineDialog();
                     break;
//                  case kToolDiamond:
//                     gROOT->SetEditorMode("Diamond");
//                     break;

                  case kToolEllipse:
                     new InsertArcDialog();
                     break;
                  case kToolPad:
                     new InsertPadDialog();
                     break;
                  case kToolPave:
                     new InsertTextBoxDialog();
                     break;

//                  case kToolPsText:
//                     gROOT->SetEditorMode("PavesText");
                     break;
                  case kToolHist:
                     new InsertHistGraphDialog(0);
                     break;
                   case kToolGraph:
                     new InsertHistGraphDialog(1);
//                     InsertGraph();
                     break;
                 case kToolTSplineX:
                     new TSplineXDialog();
                     break;
                  case kToolCurlyLine:
                     new FeynmanDiagramDialog();
                     break;
                  case kToolCurlyLineArrow:
                     new CurlyLineWithArrowDialog();
                     break;
                  case kToolLatex:
                     new InsertTextDialog();
                     break;
                   case kToolLatexFile:
                     new InsertTextDialog(kTRUE);
                     break;
                 case kToolFunction:
                     new InsertFunctionDialog();
                     break;
                  case kToolPicture:
                     new InsertImageDialog();
//                     InsertImage();
                    break;
//                  case kToolMarker:
//                     gROOT->SetEditorMode("Marker");
//                     break;
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}

