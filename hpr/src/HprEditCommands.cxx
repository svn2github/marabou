namespace std {} using namespace std;

#include "TObjString.h"
#include <iostream>
#include <iomanip>

#include "HprEditCommands.h"

ClassImp(HprEditCommands)

HprEditCommands::HprEditCommands(const TGWindow *Win, Int_t win_width, 
                 TObject * calling_class, const char * cname,
                 TList * labels, TList * methods):
							    TGTransientFrame(gClient->GetRoot(), Win, 10, 10)
{
   // Create  input dialog.
   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);

   fWidgets = new TList;
   
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;
//   cout << "win_width " <<win_width << endl;

   TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX , 2, 2, 2, 2);
     this->SetWindowName("Hpr Edit Commands");
//
   Int_t nrows = labels->GetSize();
   if (nrows != methods->GetSize()) {
      cout << "Nof Labels != Nof Methods" << endl;
      return;
   }
//   TGCompositeFrame * hframe; 
   TGTextButton * cmd;
   TString s;
   for (Int_t i = 0; i < nrows; i++) {
//      hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame); 
     s = ((TObjString *)labels->At(i))->String();   
     cmd = new TGTextButton(this, new TGHotString((const char *)s), i);
     this->AddFrame(cmd, lo1);
     s = ((TObjString *)methods->At(i))->String();   
     cmd->Connect("Clicked()", cname, calling_class,(const char *)s);
   }  
    
   UInt_t  width = 0, height = 0;
   this->MapSubwindows();

   width  = this->GetDefaultWidth();
   height = this->GetDefaultHeight();
   this->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
         0,
         ((TGFrame *) fMyWindow)->GetHeight() - ( height),
                          ax, ay, wdum);
//         ((TGFrame *) fMyWindow)->GetWidth() - width >> 1,
   cout << "ax, ay, width, height," << ax << " " << ay << " " << width << " " << height <<  endl;
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
//  make sure its inside physical screen
   if      (ax < 0)    ax = 10;
   else if (ax + wi > wwi)  ax = wwi - wi - 5;
   if      (ay + hi/2 < 0) ay =  hi/2 + 5;
   else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);

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
