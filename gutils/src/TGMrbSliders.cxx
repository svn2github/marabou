namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>

#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include "TRootHelpDialog.h"

#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TList.h>
#include <TObjString.h>

#include "TGMrbSliders.h"
#include "TGMrbHelpWindow.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget providing n sliders                                     //    
// optionally a Checkbutton is displayed to provide a yes-no selction    //
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  Double_t * Min, Max, Val : 3 dim array: min, max, value              //
//  TOrdCollection * RowLabels,                                          //
//  Int_t NValues     : number of entries                                //
//  Int_t identikfier : unique number passed in emit signal              //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //                  
//                                                                       //
// optional arguments:                                                   //
//                                                                       //
// TGWindow *win       : pointer to parent window                        //  
// const char *HelpText: help text                                       //   
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


ClassImp(TGMrbSliders)

TGMrbSliders::TGMrbSliders(const char *Title, Int_t * min, Int_t * max, Int_t * val, 
                         TOrdCollection * Row_Labels, const Int_t NValues,
                         const Int_t Identifier,
                         Int_t * Return, const TGWindow *Win,
                         const char *HelpText)
							    : TGTransientFrame(gClient->GetRoot(), Win, 100, 100)
{
   ChangeOptions(kHorizontalFrame);
   fIdentifier = Identifier;
   fNValues = NValues; 
   
   fVal = new Int_t[NValues];
   for (Int_t i = 0; i < NValues; i++ ) fVal[i] = val[i];
   fStopwatch = new TStopwatch();
   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);
   fTeColor = new TColor(999, 1, 0, 0);
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;

   //--- layout for buttons: top align, equally expand horizontally

   fBly = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 3, 0);

   //--- layout for the frame: place at bottom, right aligned
   fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsRight, 20, 10, 15, 0);
   fLO4 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY |kLHintsCenterY| kLHintsCenterX, 1, 1, 1, 1);

   fTePointers  = new TGTextEntry * [NValues];
   fTbPointers  = new TGTextBuffer * [NValues];
   fSlPointers  = new TGHSlider * [NValues];

   fLabelFrame = new TGVerticalFrame(this, 0, 0, 0);
   fSliderFrame = new TGVerticalFrame(this, 0, 0, 0);
   fValueFrame  = new TGVerticalFrame(this, 0, 0, 0);
   for (Int_t i = 0; i < NValues; i++) {
      fSlPointers[i] = new TGHSlider(fSliderFrame, 100, kSlider1 | kScaleBoth, i);
      fTePointers[i] = new TGTextEntry(fValueFrame, 
                      fTbPointers[i] = new TGTextBuffer(10), i + 1000);
      fTePointers[i]->Resize(80, fTePointers[i]->GetDefaultHeight());
      fTbPointers[i]->AddText(0, Form("%ld", val[i])); 
      fSlPointers[i]->Associate(this);
      fTePointers[i]->Associate(this);
      fSlPointers[i]->SetRange(min[i], max[i]);
      fSlPointers[i]->SetPosition(val[i]);
      fSliderFrame->AddFrame(fSlPointers[i], fBly);
      fValueFrame->AddFrame(fTePointers[i], fBly);
      TObjString *objs = (TObjString *)Row_Labels->At(i);
      TString s = objs->String();
      TGCompositeFrame * lfr= new TGCompositeFrame(fLabelFrame,100,20,kVerticalFrame | kFixedWidth |kRaisedFrame);
      TGLabel * label = new TGLabel(lfr, new TGString((const char *)s));
      lfr->AddFrame(label, fLO4);
      fLabelFrame->AddFrame(lfr, fBly);
   }
   AddFrame(fValueFrame, fBfly1);
   AddFrame(fSliderFrame, fBfly1);
   AddFrame(fLabelFrame, fBfly1);

   // set dialog title
   this->SetWindowName(Title);
   // map all widgets and calculate size of dialog
   this->MapSubwindows();
   Int_t  width  = this->GetDefaultWidth();
   Int_t  height = this->GetDefaultHeight();

   this->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
         ((TGFrame *) fMyWindow)->GetWidth() - width >> 1,
         ((TGFrame *) fMyWindow)->GetHeight() - height >> 1,
                          ax, ay, wdum);
   UInt_t ww, wh;
   Int_t  wwi,whi;
   Int_t screen_x, screen_y;
//  screen size in pixels
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
              screen_x, screen_y, ww, wh);
   wwi = ww;
   whi = wh;
   Int_t wi = width;
   Int_t hi = height;
// left of parent window
   Int_t parents_width = ((TGFrame *) fMyWindow)->GetWidth();
   ax = ax -  parents_width / 2 -  width / 3;

//  make sure its inside physical sceen
   if      (ax < 0)    ax = 10;
   else if (ax + wi > wwi)  ax = wwi - wi - 5;
   if      (ay + hi/2 < 0) ay =  hi/2 + 5;
   else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);
   // map all widgets and calculate size of dialog
   this->MapSubwindows();


   // make the message box non-resizable
   this->SetWMSize(width, height);
   this->SetWMSizeHints(width, height, width, height, 0, 0);

   this->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);

   // popup dialog and wait till user replies
   this->MapWindow();
   this->ChangeBackground(brown);

//   gClient->WaitFor(this);
};

TGMrbSliders::~TGMrbSliders()
{
   // Delete dialog.

   for (Int_t i = 0; i < fNValues; i++) {
      delete fSlPointers[i];
      delete fTePointers[i];                    
//      delete fTbPointers[i];
   }

   delete [] fSlPointers;
   delete [] fTePointers;                    
   delete [] fTbPointers;
   delete [] fVal;

   delete fTeColor;
   delete fStopwatch;
   delete fSliderFrame;
   delete fValueFrame;
   delete fBfly1; delete fBly;
}

void TGMrbSliders::CloseWindow()
{
   // Called when window is closed via the window manager.

   DeleteWindow();
}

Bool_t TGMrbSliders::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Process slider messages.
   Float_t etime, r, g, b;   
   Int_t row;
   switch (GET_MSG(msg)) {
      case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
            case kTE_TEXTCHANGED:
               row = parm1%1000;
//               cout << parm1 << " " << row << endl;
//               cout << fSlPointers[row] << endl;
               fSlPointers[row]->SetPosition(atoi(fTbPointers[row]->GetString()));
//               fVal[row] = fSlPointers[row]->GetPosition();
//               SliderEvent(fIdentifier, row, fSlPointers[row]->GetPosition());
               break;
         }
         break;
      case kC_VSLIDER:
      case kC_HSLIDER:
         switch (GET_SUBMSG(msg)) {
            case kSL_RELEASE:
               row = parm1%1000;
               SliderEvent(fIdentifier, row, fVal[row]);
               break;
            case kSL_POS:
               row = parm1%1000;
               fVal[row] = parm2;
//               cout <<  row << " " << parm2 << endl;
//               cout << fSlPointers[row] << endl;
               if (row == 0 || row == 1){
                  TColor::HLS2RGB((Float_t)parm2, 0.5, 1, r, g, b);
                  fTeColor->SetRGB(r, g, b);
                  Pixel_t p = fTeColor->GetPixel();
                  fTePointers[row]->SetBackgroundColor(p); 
               }
               fTbPointers[row]->Clear();
               fTbPointers[row]->AddText(0, Form("%ld", parm2));
               fClient->NeedRedraw(fTePointers[row]);
               etime= fStopwatch->RealTime();
               if (etime < 0.2) {
                  fStopwatch->Continue();
               } else {
                  fStopwatch->Reset();
                  SliderEvent(fIdentifier, row, fVal[row]);
               }
               break;
         }
         break;
      default:
         break;
   }
   return kTRUE;
}

void  TGMrbSliders::SliderEvent(Int_t id, Int_t row, Int_t val)
{
   Long_t args[3];   // ident, row, value
   args[0] = (Long_t)id;
   args[1] = (Long_t)row;
   args[2] = (Long_t)val;
//   cout << "SliderEvent: Emit " << id << " " << row << " " << val << endl;
 
   Emit("SliderEvent(Int_t, Int_t, Int_t)", args);
};

