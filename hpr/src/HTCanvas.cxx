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
#include "TList.h"
#include "TMarker.h"
#include "TArrow.h"
#include "TEllipse.h"
#include "TMath.h"
#include "HTCanvas.h"
#include "HandleMenus.h"
#include "TContextMenu.h"
#include "TVirtualPadEditor.h"

#include "FitHist.h"
#include "HistPresent.h"
#include "support.h"
#include "HTimer.h"
#include "SetColor.h"
#include "TMrbHelpBrowser.h" 
#include "TGMrbInputDialog.h"
#include "EditMarker.h"
#include "GroupOfGObjects.h"

const Size_t kDefaultCanvasSize   = 20;

ClassImp(HTCanvas)

//____________________________________________________________________________
HTCanvas::HTCanvas():TCanvas()
{
   fHistPresent = NULL; 
   fFitHist = NULL;     
   fHistList = NULL;    
   fGraph = NULL;       
   fTimer = NULL;       
   fRootCanvas = NULL;  
   fHandleMenus = NULL;  
   fGObjectGroups = NULL; 
};

HTCanvas::HTCanvas(const Text_t *name, const Text_t *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh, HistPresent * hpr, FitHist * fh,
           TList * hlist, TGraph * graph, Int_t flag)
           :  TCanvas(kFALSE), fHistPresent(hpr), fFitHist(fh), 
              fHistList(hlist),fGraph(graph)
            {

//*-*-*-*-*-*-*-*-*-*-*-*Canvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ==================
//  Create a new canvas.
//
//  wtopx,wtopy are the pixel coordinates of the top left corner of the canvas
//     if wtopx < 0) the menubar is not shown.
//  ww is the canvas size in pixels along X
//  wh is the canvas size in pixels along Y
//   if(!gHTGuiFactory) gHTGuiFactory = new HTRootGuiFactory();

   if (gThreadXAR) {
      void *arr[8];
      arr[1] = this;   arr[2] = (void*)name;   arr[3] = (void*)title;
      arr[4] = &wtopx; arr[5] = &wtopy; arr[6] = &ww; arr[7] = &wh;
      if ((*gThreadXAR)("CANV", 8, arr, NULL)) return;
   }

   Init();
   fMenuBar = kTRUE;
//   cout << wtopx << " " << wtopy << " " << ww << " " << wh << endl;
   if (wtopx < 0) {
      wtopx    = -wtopx;
      fMenuBar = kFALSE;
   }
   fCw           = ww;
   fCh           = wh;
   fCanvasID = -1;

//   fShowEditor = kFALSE;
//   fShowToolBar = kFALSE;
   HTCanvas *old = (HTCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
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
//      if (fMenuBar) cout << "HTCanvas: ctor fMenuBar TRUE" << endl;
//      else          cout << "HTCanvas: ctor fMenuBar FALSE" << endl;

      fCanvasImp->ShowMenuBar(fMenuBar);
//      fCanvasImp->Show();
      fBatch = kFALSE;
   }
   SetName(name);
   SetTitle(title); // requires fCanvasImp set
   fTimer = NULL;
   fEditGridX = 0;
   fEditGridY = 0;
   fVisibleGridX = 0;
   fVisibleGridY = 0;
   fGObjectGroups = NULL; 
   fUseEditGrid = kFALSE;
   fCommonRotate= kFALSE;
   fRootCanvas = (TRootCanvas*)fCanvasImp;
   if(fHistPresent && !fFitHist)fHistPresent->SetMyCanvas(fRootCanvas);
   Build();
   TEnv env(".rootrc");
   Int_t temp = env.GetValue("HistPresent.InsertMacrosAsGroup", 0);
   if (temp == 0) fInsertMacrosAsGroup = kFALSE;
   else  fInsertMacrosAsGroup = kTRUE;
   temp = env.GetValue("HistPresent.UseEditGrid", 1);
   if (temp == 0) fUseEditGrid = kFALSE;
   else  fUseEditGrid = kTRUE;
   
   if (flag & HTCanvas::kIsAEditorPage) SetBit(HTCanvas::kIsAEditorPage);
   fHandleMenus = new HandleMenus(this, fHistPresent, fFitHist, fGraph);         
//   cout << "fHandleMenus->GetId() " << fHandleMenus->GetId() << endl;
  fHandleMenus->BuildMenus(); 
//   ToggleEventStatus();
//   ToggleEventStatus();
   fCanvasImp->ShowEditor(kFALSE);
   fCanvasImp->ShowToolBar(kFALSE);
   fCanvasImp->ShowStatusBar(kFALSE);
   fCanvasImp->ShowMenuBar(fMenuBar);

   // Popup canvas
   fCanvasImp->Show();
   SetWindowSize(ww , wh );

//   if(fHistPresent && fFitHist)fHistPresent->GetCanvasList()->Add(this);
//   cout << "ctor HTCanvas: " << this << " " << name << endl;
};

HTCanvas::~HTCanvas()
{
//   cout << "dtor HTCanvas: " << this << " " << GetName()<< endl;
   if ((TVirtualPadEditor::GetPadEditor(kFALSE) != 0))
      TVirtualPadEditor::Terminate();

   if (fHandleMenus) {
      delete fHandleMenus;
      fHandleMenus = 0;
   } else return;
   
   if (fHistPresent && fFitHist) {
      TH1 * hist =  fFitHist->GetSelHist();
      if(!hist || !hist->TestBit(TObject::kNotDeleted) ||
             hist->TestBit(0xf0000000)){
         cout << "~HTCanvas:fFitHist is deleted" << endl;
         return;
      }
      const char * hname =  fFitHist->GetSelHist()->GetName();
      TList * hl = fHistPresent->GetHistList();
      if(hl->GetSize() > 0){
         TIter next(hl);
//         cout << "hl->GetSize() " << hl->GetSize() << endl;
         TCanvas *ca;
         while ( (ca = (TCanvas*)next()) ) {
            TIter nextobj(ca->GetListOfPrimitives());
            TButton *b;
            while(TObject * obj = nextobj()){
               if(!strcmp(obj->ClassName(),"TButton")){
                  b = (TButton*)obj;
                  if(b->TestBit(kSelected) && b->TestBit(kCommand)){
                     TString cname(b->GetMethod());
//                     cout << "~HTCanvas():hname " << hname << endl;
//                     cout << "~HTCanvas():cname.Data() " << cname.Data() << endl;
                     Int_t i = cname.First('"'); cname.Remove(0,i+1);
                     if(cname.Contains("Memory")
                        || cname.Contains("Socket")
                        || cname.Contains(".map")){
                        i = cname.First('"'); cname.Remove(0,i+1);
                        i = cname.First('"'); cname.Remove(0,i+1);
                     } else { 
                        i = cname.First('.');
                        cname.Remove(i,8);
                        cname.Insert(i,"_");                    
                     }
                     i = cname.First('"');
                     Int_t len = cname.Length() - i; 
//                     cout << cname.Data() << " len " << len << endl;
                     cname.Remove(i,len);
//                     cout << cname.Data() << endl;
                     if(!strcmp(cname.Data(),hname)){
                        b->ResetBit(kSelected);
                        b->SetFillColor(42);
                        b->Modified(kTRUE);
                        b->Update();
                     }
                  }
               }
            }
         }
      }
//   }
   }
   if(!fHistPresent && !fFitHist){
      TString cn(GetName());
      if(cn.Contains(".root") || cn.Contains(".map")
         ||cn.Contains(".histlist")){
         TCanvas *ca = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("Filelist");
         if(ca){
            TIter nextobj(ca->GetListOfPrimitives());
            TButton *b;
             cout << "dtor HTCanvas:  (!fHistPresent && !fFitHist)" << GetName()<< endl;
            while(TObject * obj = nextobj()){
               if(!strcmp(obj->ClassName(),"TButton")){
                  b = (TButton*)obj;
                  TString cname(b->GetMethod());
                  if(b->TestBit(kSelected) && b->TestBit(kCommand)){
                     TString cname(b->GetMethod());
                     Int_t i = cname.First('"');
                     cname.Remove(0,i+1);
                     i = cname.First('"');
                     Int_t len = cname.Length() - i;
                     cname.Remove(i,len);
//                     cout << cname.Data() << " len " << len << endl;
                     if(!cname.CompareTo(cn)){
                        b->ResetBit(kSelected);
                        b->SetFillColor(19);
                        b->Modified(kTRUE);
                        b->Update();  
                     }                        
                  }
               }
            }
         }
      }
   }
   if(fHistPresent){
      fHistPresent->GetHistList()->Remove(this);
      fHistPresent->SetMyCanvas(NULL);
//     cout << "remove " << this->GetName() << endl;
   }
   if(fHistList){
      fHistPresent->fHistListList->Remove(fHistList);
      fHistList->Delete();
      delete fHistList;
   }
   if(fTimer) delete fTimer;
   if(fHistPresent){
//     cout << "HTCanvas: Remove(this) " << this << endl;
      fHistPresent->GetCanvasList()->Remove(this);
      fHistPresent->GetHistList()->Remove(this);
   }
   if(fFitHist) {
      fFitHist->UpdateCut();
      fFitHist->SetCanvasIsDeleted();
//      cout << "HTCanvas: delete fFitHist " << this << endl;
      delete fFitHist;
   }
};
//____________________________________________________________________________

void HTCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Handle Input Events*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                ===================
//  Handle input events, like button up/down in current canvas.
//

   TPad         *pad;
   TPad         *padsave;
//   TObjLink     *pickobj;
   TObject      *prevSelObj = 0;
   TPad         *prevSelPad = 0;
//OS start
   static Int_t pxB1down, pyB1down;
   static Double_t xEnclosingCut = 0, yEnclosingCut = 0;
   Double_t x = 0, y = 0;
   Int_t n;
//OS end

   if (fSelected && fSelected->TestBit(kNotDeleted))
      prevSelObj = fSelected;
   if (fSelectedPad && fSelectedPad->TestBit(kNotDeleted))
      prevSelPad = (TPad*) fSelectedPad;
   fPadSave = (TPad*)gPad;
   padsave  = fPadSave;
   cd();        // make sure this canvas is the current canvas

   fEvent  = event;
   fEventX = px;
   fEventY = py;
   static TObject * pad_of_image = 0;
   static Bool_t in_image = kFALSE;
   if (fSelected) {
      if (event == kButton1Down && !strncmp(fSelected->ClassName(), "TASI", 4)) {
//         cout << "HTCanvas: " << fSelected->ClassName()<< " gPad " <<
//              gPad << endl;
//         gPad->Dump();
         in_image = kTRUE;
      }
   }
   
   switch (event) {

   case kMouseMotion:
      // highlight object tracked over
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      EnterLeave(prevSelPad, prevSelObj);

      gPad = pad;   // don't use cd() we will use the current
                    // canvas via the GetCanvas member and not via
                    // gPad->GetCanvas

      fSelected->ExecuteEvent(event, px, py);

      if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);
      if (fAutoExec)        RunAutoExec();

      break;

   case kMouseLeave:
      // mouse leaves canvas
      {
//         cout << " kMouseLeave " << endl;
         // force popdown of tooltips
         TObject     *sobj = fSelected;
         TPad *spad = fSelectedPad;
         fSelected    = 0;
         fSelectedPad = 0;
         EnterLeave(prevSelPad, prevSelObj);
         fSelected    = sobj;
         fSelectedPad = spad;
      }
      break;

   case kButton1Double:
      // triggered on the second button down within 350ms and within
      // 3x3 pixels of the first button down, button up finishes action
      break;

   case kButton1Down:

     // find pad in which input occured
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;
      
      gPad = pad;   // don't use cd() because we won't draw in pad
                    // we will only use its coordinate system
      if (in_image) {
         pad_of_image = pad;
         fSelected = pad_of_image;
      }
//      cout << fSelected  << endl;
   
      FeedbackMode(kTRUE);   // to draw in rubberband mode
//OS start
      pxB1down = px;
      pyB1down = py;
      
      if (fSelected->TestBit(GroupOfGObjects::kIsBound)) break;
      if (fSelected->TestBit(GroupOfGObjects::kIsEnclosingCut)) {
          TCutG * cut = (TCutG*)fSelected;
          xEnclosingCut = (cut->GetX())[0];
          yEnclosingCut = (cut->GetY())[0];
      }
      if(fUseEditGrid && fSelectedPad == this &&
       !(fSelected->IsA() == TPad::Class() ||fSelected->IsA() == TLatex::Class() )
        ){
//         cout << "x y  " << gPad->AbsPixeltoX(px) << " " << gPad->AbsPixeltoY(py) << endl;
         if(fEditGridX !=0){
            x = gPad->AbsPixeltoX(px);
            n = (Int_t)((x + TMath::Sign(0.5*fEditGridX, x)) / fEditGridX);
            x = n * fEditGridX;
         }
         if(fEditGridY !=0){
            y = gPad->AbsPixeltoY(py);
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
   
      if (fGetMouse) {
         fMouseX = gPad->AbsPixeltoX(px);
         fMouseY = gPad->AbsPixeltoY(py);
         fGetMouse = kFALSE;
      }
//OS end
      fSelected->ExecuteEvent(event, px, py);
      if (fAutoExec)        RunAutoExec();

      break;

   case kButton1Motion:

      if (fSelected) {
         gPad = fSelectedPad;
//OS start
      if (fSelected->TestBit(GroupOfGObjects::kIsBound)) break;
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
//         cout << ", " << px << " " << py ;
         fSelected->ExecuteEvent(event, px, py);

         {
            Bool_t resize = kFALSE;
            if (fSelected->InheritsFrom(TBox::Class()))
               resize = ((TBox*)fSelected)->IsBeingResized();
            if (fSelected->InheritsFrom(TVirtualPad::Class()))
               resize = ((TVirtualPad*)fSelected)->IsBeingResized();

            if ((!resize && fMoveOpaque) || (resize && fResizeOpaque)) {
               gPad = fPadSave;
               Update();
               FeedbackMode(kTRUE);
            }
         }
         if (fAutoExec) RunAutoExec();
//         if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);
      }

      break;

   case kButton1Up:
      if (fSelected) {
         gPad = fSelectedPad;
 
         if (fSelected->TestBit(GroupOfGObjects::kIsBound)) break;
         if (in_image) {
//            cout << "setting: " << fSelected << endl;
            fSelected = pad_of_image;
            in_image = kFALSE;
         }
//OS start
//         cout << "name, px, py, bef " << px << " " << py << endl;
//         fSelected->Print();
         
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

         fSelected->ExecuteEvent(event, px, py);

         if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);
         if (fAutoExec)        RunAutoExec();
      	if (fSelected->TestBit(GroupOfGObjects::kIsEnclosingCut)) {
         	 TCutG * cut = (TCutG*)fSelected;
         	 Double_t xshift = (cut->GetX())[0] -  xEnclosingCut;
         	 Double_t yshift = (cut->GetY())[0] -  yEnclosingCut;
             ShiftObjects(((GroupOfGObjects*)fSelected)->GetMemberList(), xshift, yshift);
      	}
//         if (fSelected->TestBit(GroupOfGObjects::kIsEnclosingCut)) {
//            Double_t xshift = gPad->AbsPixeltoX(px) - gPad->AbsPixeltoX(pxB1down);
//            Double_t yshift = - (gPad->AbsPixeltoX(py) - gPad->AbsPixeltoX(pyB1down));
 //           ShiftObjects(((EnclosingCut*)fSelected)->GetLofGObjects(), xshift, yshift);
//         }
//         if (fPadSave->TestBit(kNotDeleted))
//            gPad = fPadSave;
//         else {
            gPad     = this;
            fPadSave = this;
            padsave  = fPadSave;
//        }

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
      if (gDebug)
         printf("Current Pad: %s / %s\n", pad->GetName(), pad->GetTitle());

      // loop over all canvases to make sure that only one pad is highlighted
      {
         TIter next(gROOT->GetListOfCanvases());
         TCanvas *tc;
         while ((tc = (TCanvas *)next()))
            tc->Update();
      }
//     Otto 
      {
         if(fFitHist) fFitHist->AddMark((TPad*)gPad,px,py);
         else {
         }
      }
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
      // popup context menu
      pad = Pick(px, py, prevSelObj);
      if (!pad) return;

      if (fContextMenu && !fSelected->TestBit(kNoContextMenu) &&
          !pad->TestBit(kNoContextMenu) && !TestBit(kNoContextMenu))
          fContextMenu->Popup(px, py, fSelected, this, pad);


      break;
   }
   case kButton3Motion:
      break;

   case kButton3Up:
      if (fAutoExec)        RunAutoExec();
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

//      if (fShowEventStatus) DrawEventStatus(event, px, py, fSelected);
      if (fAutoExec)        RunAutoExec();

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
}
//______________________________________________________________________________
void HTCanvas::DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//*-*-*-*-*-*-*Report name and title of primitive below the cursor*-*-*-*-*-*
//*-*          ===================================================
//
//    This function is called when the option "Event Status"
//    in the canvas menu "Options" is selected.
//

   const Int_t kTMAX=256;
   static char atext[kTMAX];

   if (!fShowEventStatus || !selected) return;

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
void HTCanvas::Build()
{
   // Build a canvas. Called by all constructors.

   // Get window identifier
   if (fCanvasID == -1 && fCanvasImp)
      fCanvasID = fCanvasImp->InitWindow();
   if (fCanvasID == -1) return;

   if (fCw < fCh) fXsizeReal = fYsizeReal*Float_t(fCw)/Float_t(fCh);
   else           fYsizeReal = fXsizeReal*Float_t(fCh)/Float_t(fCw);

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
   }
   gROOT->GetListOfCanvases()->Add(this);

   // Set Pad parameters
   gPad            = this;
   fCanvas         = this;
   fMother         = (TPad*)gPad;
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


   if (fShowEventStatus && fMenuBar && fCanvasImp)
      fCanvasImp->ShowStatusBar(fShowEventStatus);
   // ... and toolbar + editor
   if (fShowToolBar && fMenuBar && fCanvasImp)
      fCanvasImp->ShowToolBar(fShowToolBar);
   if (fShowEditor && fMenuBar && fCanvasImp)
      fCanvasImp->ShowEditor(fShowEditor);
/*
      if (fMenuBar) cout << "HTCanvas: ctor fMenuBar TRUE" << endl;
      else          cout << "HTCanvas: ctor fMenuBar FALSE" << endl;
      if (fMenuBar) cout << "HTCanvas: ctor fMenuBar TRUE" << endl;
      else          cout << "HTCanvas: ctor fMenuBar FALSE" << endl;
      if (fShowToolBar) cout << "HTCanvas: ctor fShowToolBar TRUE" << endl;
      else          cout << "HTCanvas: ctor fShowToolBar FALSE" << endl;
      if (fShowEditor) cout << "HTCanvas: ctor fShowEditor TRUE" << endl;
      else          cout << "HTCanvas: ctor fShowEditor FALSE" << endl;
*/
#if defined(WIN32) && !defined(GDK_WIN32)
   if (!strcmp(gVirtualX->GetName(), "Win32"))
      gVirtualX->UpdateWindow(1);
#endif
}
//______________________________________________________________________________
/*
 void HTCanvas::Build()
{
   // Build a canvas. Called by all constructors.

   // Make sure the application environment exists. It is need for graphics
   // (colors are initialized in the TApplication ctor).
   if (!gApplication)
      TApplication::CreateApplication();

   // Get some default from .rootrc. Used in fCanvasImp->InitWindow().
   fMoveOpaque      = gEnv->GetValue("Canvas.MoveOpaque", 0);
   fResizeOpaque    = gEnv->GetValue("Canvas.ResizeOpaque", 0);
   fHighLightColor  = gEnv->GetValue("Canvas.HighLightColor", kRed);
   fShowEventStatus = gEnv->GetValue("Canvas.ShowEventStatus", kFALSE);
   fAutoExec        = gEnv->GetValue("Canvas.AutoExec", kTRUE);

   // Get window identifier
   if (fCanvasID == -1)
      fCanvasID = fCanvasImp->InitWindow();
#ifndef WIN32
   if (fCanvasID < 0) return;
#else
   // fCanvasID is in fact a pointer to the TGWin32 class
   if (fCanvasID  == -1) return;
#endif

   fContextMenu = 0;
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

      fContextMenu = new TContextMenu( "ContextMenu" );
   }
   // Fill canvas ROOT data structure
   fXsizeUser = 0;
   fYsizeUser = 0;
   if ( fCw < fCh ) {
      fYsizeReal = kDefaultCanvasSize;
      fXsizeReal = fYsizeReal*Float_t(fCw)/Float_t(fCh);
   }
   else {
      fXsizeReal = kDefaultCanvasSize;
      fYsizeReal = fXsizeReal*Float_t(fCh)/Float_t(fCw);
   }

   fDISPLAY         = "$DISPLAY";
   fRetained        = 1;

   // transient canvases have typically no menubar and should not get
   // by default the event status bar (if set by default)
   if (fShowEventStatus && fMenuBar && fCanvasImp)
      fCanvasImp->ShowStatusBar(fShowEventStatus);

   fSelected        = 0;
   fSelectedPad     = 0;
   fPadSave         = 0;
//   fEditorBar       = 0;
   fEvent           = -1;
   fEventX          = -1;
   fEventY          = -1;
   gROOT->GetListOfCanvases()->Add(this);

   // Set Pad parameters
   gPad            = this;
   fCanvas         = this;
   fMother         = (TPad*)gPad;
   if (!fPrimitives) {
      fPrimitives     = new TList(this);
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
      Range(0, 0, 1, 1);   //Pad range is set by default to [0,1] in x and y
      PaintBorder(GetFillColor(), kTRUE);    //Paint background
//      TWbox::Paint("");    //Paint background
   }
   SetBit(kObjInCanvas);
#ifdef WIN32
   gVirtualX->UpdateWindow(1);
#endif
}
*/
//______________________________________________________________________________
void HTCanvas::RunAutoExec()
{
   // Execute the list of TExecs in the current pad.

   if (!gPad) return;
   ((TPad*)gPad)->AutoExec();
}

//_____________________________________________________________________________

HistPresent * HTCanvas::GetHistPresent(){return fHistPresent;};
//_____________________________________________________________________________

FitHist * HTCanvas::GetFitHist(){return fFitHist;};
//_____________________________________________________________________________

void HTCanvas::ActivateTimer(Int_t delay){
   if(fTimer){
      cout << "Deleting existing timer" << endl;
      delete fTimer;
      fTimer = 0;
   }
   if(delay > 0) fTimer = new HTimer(delay, kTRUE, this);      
}
//_____________________________________________________________________________

void HTCanvas::UpdateHists() 
{
   if (!fHistPresent) return;
   if (!fHistList)    return; 
   if (fHistPresent->fAnyFromSocket && !(fHistPresent->fComSocket)) return;
//      TList * hlist = fHistPresent->GetSelectedHist();
   Int_t nhist = fHistList->GetSize();
   if(nhist <= 0)return;
   Int_t fx, lx, fy, ly;
   for(Int_t i=0; i<nhist; i++){
      this->cd(i+1);
//         TPad * p = gPad;
      TH1 * hold  = GetTheHist(gPad);
      fx =  hold->GetXaxis()->GetFirst(); 
      lx =  hold->GetXaxis()->GetLast(); 
      TH1 * hist = fHistPresent->GetSelHistAt(i,fHistList);
     if (!hist) {
         cout << setred << "Cant get histogram, M_analyze stopped? " << endl;
         if(fTimer){
            cout << "Deleting existing timer" << endl;
            delete fTimer;
            fTimer = 0;
//            fHistPresent->fSocketIsOpen = kFALSE;
         }
          cout << setblack << endl;

         return;
      }
     if(is2dim(hist)){
         fy =  hold->GetYaxis()->GetFirst(); 
         ly =  hold->GetYaxis()->GetLast();        
         hist->Draw(fHistPresent->fDrawOpt2Dim->Data());
         hist->GetXaxis()->SetRange(fx, lx);
         hist->GetYaxis()->SetRange(fy, ly);
      } else { 
         TString drawopt;
         if(fHistPresent->fShowContour)drawopt = "hist";
         if(fHistPresent->fShowErrors)drawopt += "e1";
         if(fHistPresent->fFill1Dim){
            hist->SetFillStyle(1001);
			hist->SetFillColor(44);
         } else hist->SetFillStyle(0);
         hist->Draw(drawopt.Data());
         hist->GetXaxis()->SetRange(fx, lx);
      }
   }
   this->Modified(kTRUE);
   this->Update();
}
//______________________________________________________________________________
void HTCanvas::SetLog(Int_t state)
{
    fHandleMenus->SetLog(state);

}
