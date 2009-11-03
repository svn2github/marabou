#include "TROOT.h"
#include "TWbox.h"
#include "TGuiFactory.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TMath.h"
#include "GrCanvas.h"
#include "TContextMenu.h"
#include "GEdit.h"
#include "SetColor.h"
#include "HprElement.h"
#include "HprEditBits.h"

const Size_t kDefaultCanvasSize   = 20;

ClassImp(GrCanvas)

//____________________________________________________________________________
GrCanvas::GrCanvas():TCanvas()
{
   fRootCanvas  = NULL;
   fGEdit       = NULL;
   fHasConnection = kFALSE;
   fHiddenPrimitives  = NULL;
   fEditGridX = 0;
   fEditGridY = 0;
   fUseEditGrid = 0;
   fEditorIsShown = kFALSE;
   fHasConnection = kFALSE;
   fCurrentPlane      = 50;
//   fOrigWw = 0;
//  fOrigWh = 0;
   fButtonsEnabled = kTRUE;
   fOrigWw = GetWw();
   fOrigWh = GetWh();
};

GrCanvas::GrCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh) :  TCanvas(kFALSE)
{

//*-*-*-*-*-*-*-*-*-*-*-*Canvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ==================
//  Create a new canvas.
//
//  wtopx,wtopy are the pixel coordinates of the top left corner of the canvas
//     if wtopx < 0) the menubar is not shown.
//  ww is the canvas size in pixels along X
//  wh is the canvas size in pixels along Y
//
// GrCanvas has the following extra facilities:
//
// Edit grid: if activated forces coordinates of mouse clicks on the grid
//--------------------------------------------------------------------------


   if (gThreadXAR) {
      void *arr[8];
      arr[1] = this;   arr[2] = (void*)name;   arr[3] = (void*)title;
      arr[4] = &wtopx; arr[5] = &wtopy; arr[6] = &ww; arr[7] = &wh;
      if ((*gThreadXAR)("CANV", 8, arr, NULL)) return;
   }
#if ROOTVERSION > 50000
   SetBit(kShowEventStatus,0);
   SetBit(kAutoExec  	  ,0);
   SetBit(kMenuBar		  ,0);
   SetBit(kShowToolBar    ,0);
   SetBit(kShowEditor	  ,0);
   SetBit(kMoveOpaque	  ,0);
   SetBit(kResizeOpaque   ,0);
#endif
   Init();
#if ROOTVERSION > 50000
   SetBit(kMenuBar,1);
   if (ww < 0) {
      ww       = -ww;
      SetBit(kMenuBar,0);
   }
   if (wtopx < 0) {
      wtopx  = -wtopx;
      SetBit(kMenuBar,0);
   }
#else
   fMenuBar = kTRUE;
   if (wtopx < 0) {
      wtopx    = -wtopx;
      fMenuBar = kFALSE;
   }
#endif

   fCw           = ww;
   fCh           = wh;
   fCanvasID = -1;

//   fShowEditor = kFALSE;
//   fShowToolBar = kFALSE;
   GrCanvas *old = (GrCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
   if (old && old->IsOnHeap()) delete old;
   if (strlen(name) == 0 || gROOT->IsBatch()) {   //We are in Batch mode
      fWindowTopX   = fWindowTopY = 0;
      fWindowWidth  = ww;
      fWindowHeight = wh;
      fCw           = ww;
      fCh           = wh;
      fCanvasImp    = gBatchGuiFactory->CreateCanvasImp(this, name, fCw, fCh);
      fBatch        = kTRUE;
   } else {                   //normal mode with a screen window
      Float_t cx = gStyle->GetScreenFactor();
      fCanvasImp = gGuiFactory->CreateCanvasImp(this, name, Int_t(cx*wtopx), Int_t(cx*wtopy), UInt_t(cx*ww), UInt_t(cx*wh));
//      if (fMenuBar) cout << "GrCanvas: ctor fMenuBar TRUE" << endl;
//      else          cout << "GrCanvas: ctor fMenuBar FALSE" << endl;

      fCanvasImp->ShowMenuBar(HasMenuBar());
//      fCanvasImp->ShowMenuBar(HasMenuBar());
//      fCanvasImp->Show();
      fBatch = kFALSE;
   }

   SetName(name);
   SetTitle(title); // requires fCanvasImp set
   fEditGridX = 0;
   fEditGridY = 0;
   fGEdit       = NULL;
   fHiddenPrimitives = new TList();
   fUseEditGrid = 0;
   fHasConnection = kFALSE;
   fCurrentPlane      = 50;
   fButtonsEnabled = kTRUE;
   fRootCanvas = (TRootCanvas*)fCanvasImp;
   Build();
   fCanvasImp->ShowEditor(kFALSE);
   fEditorIsShown = kFALSE;
   fCanvasImp->ShowToolBar(kFALSE);
   fCanvasImp->ShowStatusBar(kFALSE);
   fCanvasImp->ShowMenuBar(HasMenuBar());

   // Popup canvas
   fCanvasImp->Show();
   SetWindowSize(ww , wh );
   if (TestBit(kIsAEditorPage)) {
//      InitEditCommands();
      fRootCanvas->DontCallClose();
      TQObject::Connect((TGMainFrame*)fRootCanvas, "CloseWindow()",
                        this->ClassName(), this, "MyClose()");
   }
   fOrigWw = GetWw();
   fOrigWh = GetWh();

//   cout << "ctor GrCanvas: " << this << " " << name
//        << " Id " << fRootCanvas->GetId() << endl;
};
//______________________________________________________________________________________

void GrCanvas::MyClose()
{
   cout << "GrCanvas::MyClose() set ShowEditor(kFALSE) " << endl;
   fRootCanvas->ShowEditor(kFALSE);
   fRootCanvas->CloseWindow();
}
//______________________________________________________________________________________

GrCanvas::~GrCanvas()
{
//  cout << "dtor GrCanvas: " << this << " " << GetName()<< endl;
};
//____________________________________________________________________________

void GrCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//
//  if (event > 0) return;
//OS start
   static Int_t pxB1down, pyB1down;
   static Bool_t in_edit = kFALSE;
   static Bool_t obj_moved = kFALSE;

   Double_t x = 0, y = 0;
   Int_t n;
   static TObject * pad_of_image = 0;
   static Bool_t in_image = kFALSE;
//OS end

   TPad         *pad;
   TObject      *prevSelObj = 0;
   TPad         *prevSelPad = 0;

   if ( !fButtonsEnabled ) return;

   if (gROOT->GetEditorMode() != 0) {
      in_edit = kTRUE;
   }
   if (fSelected && fSelected->TestBit(kNotDeleted))
      prevSelObj = fSelected;
   if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
      prevSelPad = (TPad*) fSelectedPad;

   fPadSave = (TPad*)gPad;
   if (event == kButton1Down) {
//      cout << "kButton1Down fSelected " << fSelected->ClassName() << " gPad " << gPad
//        << " fSelectedPad " << fSelectedPad <<  endl;
      if (gROOT->GetEditorMode() != 0 && fSelectedPad != gPad) {
         cout << "Please use selected pad" << endl;
         gROOT->SetEditorMode();
         gROOT->SetSelectedPad(NULL);
         return;
      }
   }
   cd();        // make sure this canvas is the current canvas

//   if (event == kButton1Down)  cout << "gPad " << gPad << endl;

   fEvent  = event;
   fEventX = px;
   fEventY = py;
   obj_moved = kFALSE;

   if (fSelected) {
      if (event == kButton1Down && !strncmp(fSelected->ClassName(), "TASI", 4)) {
//         cout << "GrCanvas: " << fSelected->GetName()<< " gPad " <<
//              gPad << endl;
//         gPad->Dump();
         in_image = kTRUE;
      }
   }

//   if (gROOT->GetEditorMode() && fEvent == kMouseMotion) fEvent = kButton1Motion;

   switch (fEvent) {

   case kMouseMotion:
      // highlight object tracked over
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      fSelected->ExecuteEvent(event, px, py);

      if (GetAutoExec()) RunAutoExec();

      break;

   case kMouseEnter:
      // mouse enters canvas
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
      break;
   case kMouseLeave:
      // mouse leaves canvas
      {
//         cout << " kMouseLeave " << endl;
         // force popdown of tooltips
         TObject *sobj = fSelected;
         TPad    *spad = fSelectedPad;
         fSelected    = 0;
         fSelectedPad = 0;
         EnterLeave(prevSelPad, prevSelObj);
         fSelected    = sobj;
         fSelectedPad = spad;
         if (!fDoubleBuffer) FeedbackMode(kFALSE);
      }
      break;

   case kButton1Double:
      // triggered on the second button down within 350ms and within
      // 3x3 pixels of the first button down, button up finishes action
//      break;

   case kButton1Down:

//      cout << "kButton1Down "  << px << " " << py << " " << fSelected << endl;
     // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if (in_image) {
         pad_of_image = pad;
         fSelected = pad_of_image;
      }

//OS start
      pxB1down = px;
      pyB1down = py;

      if (fSelected->TestBit(kIsBound)) break;
      x = gPad->AbsPixeltoX(px);
      y = gPad->AbsPixeltoY(py);
      if(fUseEditGrid && fSelectedPad == this &&
       !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() )
        ){
//         cout << "x y  " << gPad->AbsPixeltoX(px) << " " << gPad->AbsPixeltoY(py) << endl;
         if(fEditGridX !=0){
            n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
            x = n * fEditGridX;
         }
         if(fEditGridY !=0){
            n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
            y = n * fEditGridY;
         }
            if(fEditGridX !=0){
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw()) px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
//         cout << "x y grid " << x << " " << y << endl;
      }

//OS end
      if (fSelected) {
         FeedbackMode(kTRUE);   // to draw in rubberband mode
         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0) {
            fSelected->ExecuteEvent(event, px, py);
         }
         if (GetAutoExec()) RunAutoExec();
      }

      break;

//   case kMouseMotion:
   case kButton1Motion:

      if (fSelected) {
         gPad = fSelectedPad;
//OS start
      if (fSelected->TestBit(kIsBound)) break;
         if (in_image) {
            fSelected = pad_of_image;
//            fSelected = gPad;
         }
//         if(fUseEditGrid && !(fSelected->IsA() == TPad::Class())){
         if(fUseEditGrid && fSelectedPad == this &&
          !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() )
           ){
            if(fEditGridX !=0){
               x = gPad->AbsPixeltoX(px);
               n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
               x = n * fEditGridX;
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw())px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               y = gPad->AbsPixeltoY(py);
               n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
               y = n * fEditGridY;
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
         }

//OS end
         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0)
            fSelected->ExecuteEvent(event, px, py);
            gVirtualX->Update();

         {
            Bool_t resize = kFALSE;
            if (fSelected->InheritsFrom(TBox::Class()))
               resize = ((TBox*)fSelected)->IsBeingResized();
            if (fSelected->InheritsFrom(TVirtualPad::Class()))
               resize = ((TVirtualPad*)fSelected)->IsBeingResized();

            if ((!resize && OpaqueMoving()) || (resize && OpaqueResizing())) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         }
          if (GetAutoExec()) RunAutoExec();
//         if (GetShowEventStatus()) DrawEventStatus(event, px, py, fSelected);
      }

      break;

   case kButton1Up:
      if (fSelected) {
         gPad = fSelectedPad;
//         cout << "kButton1Up: this " << this
 //             << " fSelected " << fSelected->ClassName() << " gPad " << gPad << endl;
         if (fSelected->TestBit(kIsBound)) break;
         if (in_image) {
//            cout << "setting: " << fSelected << endl;
            fSelected = pad_of_image;
            in_image = kFALSE;
         }
//OS start
//         cout << "name, px, py, bef " << fSelected->ClassName() << " " << pxB1down << " " << pyB1down
//                 << " "  << px << " " << py << endl;

         if (px != pxB1down || py != pyB1down) obj_moved = kTRUE;
         if (fUseEditGrid && fSelectedPad == this &&
          !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() )
           ){
            if(fEditGridX !=0){
               x = gPad->AbsPixeltoX(px);
               n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
               x = n * fEditGridX;
               px =  gPad->XtoAbsPixel(x);
               if(px < 1)px = 1;
               if(px > (Int_t)gPad->GetWw())px = gPad->GetWw()-1;
            }
            if(fEditGridY !=0){
               y = gPad->AbsPixeltoY(py);
               n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
               y = n * fEditGridY;
               py =  gPad->YtoAbsPixel(y);
               if(py < 1)py = 1;
               if(py > (Int_t)gPad->GetWh())py = gPad->GetWh()-1;
            }
         }
//OS end

         if (fSelected == fPadSave || gROOT->GetEditorMode() == 0)
            fSelected->ExecuteEvent(event, px, py);

          if (GetAutoExec()) RunAutoExec();
         if (fPadSave->TestBit(kNotDeleted))
            gPad = fPadSave;
         else {
            gPad     = this;
            fPadSave = this;
         }

         Update();    // before calling update make sure gPad is reset
      }

      break;

//*-*----------------------------------------------------------------------

   case kButton2Down:
      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      FeedbackMode(kTRUE);

      fSelected->Pop();           // pop object to foreground
      pad->cd();                  // and make its pad the current pad
      if (gDebug) {
         printf("Current Pad: %s / %s \n", pad->GetName(), pad->GetTitle());
         cout << "fSelected: " << fSelected << " pad " << pad << endl;
      }
      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }
//     Otto
//      {
//         if(fFitHist) fFitHist->AddMark((TPad*)gPad,px,py);
//         else {
//         }
//      }
//      if (gDebug)
//        printf("Current Pad: %x: %s / %s\n", pad, pad->GetName(), pad->GetTitle());

      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }

      return;   // don't want fPadSave->cd() to be executed at the end

   case kButton2Motion:
      break;

   case kButton2Up:
      break;

   case kButton2Double:
      break;

//*-*----------------------------------------------------------------------

   case kButton3Down:
   {
 //     cout << "kButton3Down "  << px << " " << py << " " << fSelected << endl;
      // popup context menu
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;
      if (!fDoubleBuffer) FeedbackMode(kFALSE);

      if (fContextMenu && !fSelected->TestBit(kNoContextMenu) &&
          !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu)) {
 //         fSelected->ExecuteEvent(event, px, py);
          fContextMenu->Popup(px, py, fSelected, this, pad);
      }

      break;
   }
   case kButton3Motion:
      break;

   case kButton3Up:
      if (!fDoubleBuffer) FeedbackMode(kTRUE);
//      if (fSelected) fSelected->ExecuteEvent(event, px, py);
 //     if (GetAutoExec())        RunAutoExec();
      break;

   case kButton3Double:
      break;

   case kKeyPress:

      // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system

      fSelected->ExecuteEvent(event, px, py);

//      if (GetShowEventStatus()) DrawEventStatus(event, px, py, fSelected);
      if (GetAutoExec())        RunAutoExec();

      break;

   default:
      break;
   }

//   if (fPadSave) fPadSave->cd();
   if (fPadSave && event != kButton2Down)
      fPadSave->cd();

   if (event != kMouseLeave) { // signal was already emitted for this event
      ProcessedEvent(event, px, py, fSelected);  // emit signal
      DrawEventStatus(event, px, py, fSelected);
   }
   if (in_edit && gROOT->GetEditorMode() == 0) {
      ObjectCreated(px, py, fSelected);
      in_edit = kFALSE;
   }
   if (obj_moved && fSelected && fSelected != this) {
      ObjectMoved(px, py, fSelected);
   }
}

//______________________________________________________________________________
void GrCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//*-*-*-*-*-*-*Report name and title of primitive below the cursor*-*-*-*-*-*
//*-*          ===================================================
//
//    This function is called when the option "Event Status"
//    in the canvas menu "Options" is selected.
//

   const Int_t kTMAX=256;
   static char atext[kTMAX];

   if (!GetShowEventStatus() || !selected) return;

   if (!fCanvasImp) return; //this may happen when closing a TAttCanvas

   TVirtualPad* savepad;
   savepad = gPad;
   gPad = GetSelectedPad();

   fCanvasImp->SetStatusText(selected->GetTitle(),0);
   fCanvasImp->SetStatusText(selected->GetName(),1);
   if (event == kKeyPress)
      sprintf(atext, "%c", (char) px);
   else
      sprintf(atext, "%d,%d", px, py);
   fCanvasImp->SetStatusText(atext,2);
   fCanvasImp->SetStatusText(selected->GetObjectInfo(px,py),3);
   gPad = savepad;
}
//_____________________________________________________________________________
void GrCanvas::Build()
{
   // Build a canvas. Called by all constructors.

   // Get window identifier
   if (fCanvasID == -1 && fCanvasImp)
      fCanvasID = fCanvasImp->InitWindow();
   if (fCanvasID == -1) return;

   if (fCw < fCh) fXsizeReal = fYsizeReal*Float_t(fCw)/Float_t(fCh);
   else           fYsizeReal = fXsizeReal*Float_t(fCh)/Float_t(fCw);

   // Set Pad parameters
   gPad            = this;
   fCanvas         = this;
   fMother         = (TPad*)gPad;

   if (!IsBatch()) {    //normal mode with a screen window
      // Set default physical canvas attributes
      gVirtualX->SelectWindow(fCanvasID);
      gVirtualX->SetFillColor(1);         //Set color index for fill area
      gVirtualX->SetLineColor(1);         //Set color index for lines
      gVirtualX->SetMarkerColor(1);       //Set color index for markers
      gVirtualX->SetTextColor(1);         //Set color index for text

      // Clear workstation
      gVirtualX->ClearWindow();

      // Set Double Buffer on by default
      SetDoubleBuffer(1);

      // Get effective window parameters (with borders and menubar)
      fCanvasImp->GetWindowGeometry(fWindowTopX, fWindowTopY,
                                    fWindowWidth, fWindowHeight);

      // Get effective canvas parameters without borders
      Int_t dum1, dum2;
      gVirtualX->GetGeometry(fCanvasID, dum1, dum2, fCw, fCh);

      fContextMenu = new TContextMenu("ContextMenu");
   } else {
      // Make sure that batch interactive canvas sizes are the same
      fCw -= 4;
      fCh -= 28;
   }

   gROOT->GetListOfCanvases()->Add(this);

   if (!fPrimitives) {
      fPrimitives     = new TList;
      SetFillColor(gStyle->GetCanvasColor());
      SetFillStyle(1001);
      SetGrid(gStyle->GetPadGridX(),gStyle->GetPadGridY());
      SetTicks(gStyle->GetPadTickX(),gStyle->GetPadTickY());
      SetLogx(gStyle->GetOptLogx());
      SetLogy(gStyle->GetOptLogy());
      SetLogz(gStyle->GetOptLogz());
      SetBottomMargin(gStyle->GetPadBottomMargin());
      SetTopMargin(gStyle->GetPadTopMargin());
      SetLeftMargin(gStyle->GetPadLeftMargin());
      SetRightMargin(gStyle->GetPadRightMargin());
      SetBorderSize(gStyle->GetCanvasBorderSize());
      SetBorderMode(gStyle->GetCanvasBorderMode());
      fBorderMode=gStyle->GetCanvasBorderMode(); // do not call SetBorderMode (function redefined in TCanvas)
      SetPad(0, 0, 1, 1);
      Range(0, 0, 1, 1);   //pad range is set by default to [0,1] in x and y
      gVirtualX->SelectPixmap(fPixmapID);    //pixmap must be selected
      PaintBorder(GetFillColor(), kTRUE);    //paint background
   }

   // transient canvases have typically no menubar and should not get
   // by default the event status bar (if set by default)

#if ROOTVERSION > 50000
   if (TestBit(kMenuBar) && fCanvasImp) {
      if (TestBit(kShowEventStatus)) fCanvasImp->ShowStatusBar(kTRUE);
      // ... and toolbar + editor
      if (TestBit(kShowToolBar))     fCanvasImp->ShowToolBar(kTRUE);
      if (TestBit(kShowEditor))      fCanvasImp->ShowEditor(kTRUE);
   }
#else
   if (fShowEventStatus && fMenuBar && fCanvasImp)
      fCanvasImp->ShowStatusBar(fShowEventStatus);
   // ... and toolbar + editor
   if (fShowToolBar && fMenuBar && fCanvasImp)
      fCanvasImp->ShowToolBar(fShowToolBar);
   if (fShowEditor && fMenuBar && fCanvasImp)
      fCanvasImp->ShowEditor(fShowEditor);
#endif
#if defined(WIN32) && !defined(GDK_WIN32)
   if (!strcmp(gVirtualX->GetName(), "Win32"))
      gVirtualX->UpdateWindow(1);
#endif
}
//______________________________________________________________________________
void GrCanvas::RunAutoExec()
{
   // Execute the list of TExecs in the current pad.

   if (!gPad) return;
   ((TPad*)gPad)->AutoExec();
}
//______________________________________________________________________________
void GrCanvas::SetLog(Int_t state)
{
   // fHandleMenus->SetLog(state);

}
//______________________________________________________________________________

void GrCanvas::Add2ConnectedClasses(TObject *obj)
{
   this->Connect("ObjectCreated(Int_t, Int_t, TObject*)", obj->ClassName(), obj,
                 "ObjCreated(Int_t, Int_t, TObject*)");
   this->Connect("ObjectMoved(Int_t, Int_t, TObject*)", obj->ClassName(), obj,
                 "ObjMoved(Int_t, Int_t, TObject*)");
   fHasConnection = kTRUE;
}
//______________________________________________________________________________

void GrCanvas::ObjectCreated(Int_t px, Int_t py, TObject *obj)
{
   Long_t args[3];
   if (fHasConnection) {
      args[0] = (Long_t)px;
      args[1] = (Long_t)py;
      args[2] = (Long_t)obj;
      cout << "GrCanvas::Emit(ObjectCreated" << endl;
      Emit("ObjectCreated(Int_t, Int_t, TObject*)", args );
   }
}
//______________________________________________________________________________

void GrCanvas::ObjectMoved(Int_t px, Int_t py, TObject *obj)
{
   Long_t args[3];
   if (fHasConnection) {
      args[0] = (Long_t)px;
      args[1] = (Long_t)py;
      args[2] = (Long_t)obj;
      cout << "GrCanvas::Emit(ObjectMoved: " << obj << endl;
      Emit("ObjectMoved(Int_t, Int_t, TObject*)", args );
   }
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridX(Double_t x)
{
   if (fEditGridX ==  0) return x;
   Int_t n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
   return (Double_t)n * fEditGridX;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridY(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Int_t n = (Int_t)((y + TMath::Sign(0.5*fEditGridY, y)) / fEditGridY);
   return (Double_t)n * fEditGridY;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridX_NDC(Double_t x)
{
   if (fEditGridX <=  0) return x;
   Double_t px1 = gPad->GetX1();
   Double_t px2 = gPad->GetX2();
	Double_t gNDC = fEditGridX /(px2 - px1);

   Int_t n = (Int_t)((x + TMath::Sign(0.5*gNDC, x)) / gNDC);
   return (Double_t)n * gNDC;
}
//______________________________________________________________________________

Double_t GrCanvas::PutOnGridY_NDC(Double_t y)
{
   if (fEditGridY ==  0) return y;
   Double_t py1 = gPad->GetY1();
   Double_t py2 = gPad->GetY2();
	Double_t gNDC = fEditGridY /(py2 - py1);

   Int_t n = (Int_t)((y + TMath::Sign(0.5*gNDC, y)) / gNDC);
   return (Double_t)n * gNDC;
}
//______________________________________________________________________________

void GrCanvas::HideObject(TObject *obj)
{
   if (!obj || obj == this || GetListOfPrimitives()->GetEntries()  <= 0 ) return;
   std::cout << "HideObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, GetListOfPrimitives(), fHiddenPrimitives);
}
//______________________________________________________________________________

void GrCanvas::ViewObject(TObject *obj)
{
   std::cout << "ViewObject obj, gPad " << obj << " " << gPad << endl;
   HprElement::MoveObject(obj, fHiddenPrimitives, GetListOfPrimitives());
}

//______________________________________________________________________________

void GrCanvas::ViewAllObjects()
{
   if (!fHiddenPrimitives || fHiddenPrimitives->GetEntries() <=0 ) return;
   HprElement::MoveAllObjects(fHiddenPrimitives, GetListOfPrimitives(), 0, 1);
}
//____________________________________________________________________________

void GrCanvas::RemovePicture()
{
	TList * lop = GetListOfPrimitives();
	TIter next(lop );
	TObject * obj;
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("HprImage")) {
			lop->Remove(obj);
			break;
		}
	}
	Modified();
	Update();
}
//______________________________________________________________________________

void GrCanvas::PushPictureToBg()
{
	TList * lop = GetListOfPrimitives();
   TIter next(lop );
   TObject * obj;
   TObject * objsav = NULL;
   while ( (obj = next()) ) {
      if (obj->InheritsFrom("HprImage")) {
			objsav = obj;
			lop->Remove(obj);
			break;
		}
   }
	if (objsav != NULL) {
		lop->AddFirst(objsav);
	}
   Modified();
   Update();
}

