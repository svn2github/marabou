using namespace std;

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
#include <TGSlider.h>
#include <TGFileDialog.h>
#include <TList.h>
#include "TGMrbInputDialog.h"
#include "TGMrbHelpWindow.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget to request input of a string                            //    
// optionally a Checkbutton is displayed to provide a yes-no selction    //
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const char *Prompt: the Prompt string                                //
//  const char *DefVal: the default value of the string                  //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //                  
//  char *retstr      : the returned string                              //
//                                                                       //
// optional arguments:                                                   //
//                                                                       //
// TGWindow *win       : pointer to parent window                        //  
// const char *YNPrompt: Prompt for Checkbutton                          //
// Bool_t * YesNo:       truth value of Checkbutton                      //
// const char *HelpText: help text                                       //   
//                                                                       //
// This class is normally accessed via the following utility functions:  //
//                                                                       //
// const char * GetString(const char *Prompt, const char * DefVal,        //
// 							 Bool_t * Ok = 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0, Bool_t * yn=0,			 //
// 							 const char *helptext=0);								 //
// 																							 //
// Int_t  	  GetInteger(const char *Prompt, Int_t *DefVal, 				 //
// 							 Bool_t * Ok= 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0,Bool_t * yn=0, 			 //
// 							 const char *helptext=0);								 //
// Float_t		 GetFloat(const char *Prompt, Float_t *DefVal,				 //
// 							 Bool_t * Ok = 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0, Bool_t * yn=0,			 //
// 							 const char *helptext=0);								 //
//  Note: most arguments have defaults, ok is true if OK buttom pressed  //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


ClassImp(TGMrbInputDialog)

TGMrbInputDialog::TGMrbInputDialog(const char *Prompt, const char *DefVal, 
                         Int_t * Return, char *retstr,const TGWindow *Win,
                         const char *YNPrompt, Bool_t * YesNo,
                         const char *HelpText,
                         const char *YNPrompt1, Bool_t * YesNo1):
							    TGTransientFrame(gClient->GetRoot(), Win, 10, 10)
{
   // Create  input dialog.

   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);

   fWidgets = new TList;
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;
 
   // create Prompt label and textentry widget
   TGLabel *label = new TGLabel(this, Prompt);
   fWidgets->AddFirst(label);

   TGTextBuffer *tbuf = new TGTextBuffer(256);  //will be deleted by TGtextEntry
   tbuf->AddText(0, DefVal);

   fTE = new TGTextEntry(this, tbuf);
   Int_t wid = 50+10*strlen(DefVal);
   Int_t tewidth = TMath::Max(wid,320);
   fTE->Resize(tewidth, fTE->GetDefaultHeight());
   fTE->Associate(this);
   fWidgets->AddFirst(fTE);
//   fTE->Associate(this);
   TGLayoutHints *l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
   TGLayoutHints *l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fWidgets->AddFirst(l1);
   fWidgets->AddFirst(l2);

   this->AddFrame(label, l1);
   this->AddFrame(fTE, l2);

   // create frame and layout hints for Ok and Cancel buttons
   TGHorizontalFrame *hf = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
   fWidgets->AddFirst(hf);
   TGLayoutHints     *l3 = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);

   // put hf as last in list to be deleted
   fWidgets->AddFirst(l3);

   if(YNPrompt && YesNo){
      fCheckYesNo = new TGCheckButton(this, YNPrompt, 0);
      if(*YesNo)fCheckYesNo->SetState(kButtonDown);
      fWidgets->AddFirst(fCheckYesNo);
      this->AddFrame(fCheckYesNo, l2);
   }
   if(YNPrompt1 && YesNo1){
      fCheckYesNo1 = new TGCheckButton(this, YNPrompt1, 0);
      if(*YesNo1)fCheckYesNo1->SetState(kButtonDown);
      fWidgets->AddFirst(fCheckYesNo1);
      this->AddFrame(fCheckYesNo1, l2);
   }
   // create OK and Cancel buttons in their own frame (hf)
   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", 1);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   b = new TGTextButton(hf, "&Cancel", 2);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   if(HelpText){
      b = new TGTextButton(hf, "Help", 3);
      fWidgets->AddFirst(b);
      b->Associate(this);
      hf->AddFrame(b, l3);
      height = b->GetDefaultHeight();
      width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   }
   // place button frame (hf) at the bottom
   TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
   fWidgets->AddFirst(l4);

   this->AddFrame(hf, l4);

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * nb, height);

   // set dialog title
   this->SetWindowName("Get Input");

   // map all widgets and calculate size of dialog
   this->MapSubwindows();

   width  = this->GetDefaultWidth();
   height = this->GetDefaultHeight();

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
//  make sure its inside physical sceen
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
   hf->ChangeBackground(brown);

   fRetStr = retstr;
   fDefVal = DefVal;
   fReturn = Return;
   fRetYesNo = YesNo;
   fRetYesNo1 = YesNo1;
   fHelpText = HelpText;
   gClient->WaitFor(this);
};

Bool_t TGMrbInputDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON:
                switch (parm1) {
                   case 1:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fTE->GetBuffer()->GetString());
                      if(fRetYesNo){
                         if(fCheckYesNo->GetState() == kButtonUp) *fRetYesNo = kFALSE;
                         else                                     *fRetYesNo = kTRUE;
                      }
                      if(fRetYesNo1){
                         if(fCheckYesNo1->GetState() == kButtonUp) *fRetYesNo1 = kFALSE;
                         else                                      *fRetYesNo1 = kTRUE;
                      }
                      *fReturn = 0;
                      delete this;
                      break;

                    case 2:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fDefVal);
                      *fReturn = -1;
                      delete this;
                      break;
                  case 3:
                      new TGMrbHelpWindow(this, "HistPresent Help", fHelpText, 550, 500);
                      break;
                 }
              default:
                 break;
          }
          break;

       case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
             case kTE_ENTER:
                // here copy the string from text buffer to return variable
                strcpy(fRetStr, fTE->GetBuffer()->GetString());
                if(fRetYesNo){
                   if(fCheckYesNo->GetState() == kButtonUp) *fRetYesNo = kFALSE;
                   else                                     *fRetYesNo = kTRUE;
                }
                if(fRetYesNo1){
                   if(fCheckYesNo1->GetState() == kButtonUp) *fRetYesNo1 = kFALSE;
                   else                                      *fRetYesNo1 = kTRUE;
                }
                *fReturn = 0;
                delete this;
                break;
             default:
                break;
          }
          break;

       default:
          break;
   }
   return kTRUE;
}

//_______________________________________________________________________________________

const char *GetString(const char *Prompt, const char *DefVal, Bool_t * Ok,
                      TGWindow *Win,  const char *YNPrompt, Bool_t * YesNo,
                      const char *HelpText, const char *YNPrompt1, Bool_t * YesNo1)
{
   // Prompt for string. The typed in string is returned.

   static char answer[128];
   Int_t ret;
   new TGMrbInputDialog(Prompt, DefVal, &ret, answer,Win, YNPrompt, YesNo, HelpText, YNPrompt1, YesNo1);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }
   return answer;
}
//_______________________________________________________________________________________

Int_t GetInteger(const char *Prompt, Int_t DefVal, Bool_t * Ok,
                 TGWindow *Win,  const char *YNPrompt, Bool_t * YesNo,
                 const char *HelpText, const char *YNPrompt1, Bool_t * YesNo1)
{
   // Prompt for integer. The typed in integer is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%d", DefVal);

   Int_t ret;
   new TGMrbInputDialog(Prompt, defv, &ret, answer, Win, YNPrompt, YesNo, HelpText, YNPrompt1, YesNo1);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }

   return atoi(answer);
}
//_______________________________________________________________________________________

Float_t GetFloat(const char *Prompt, Float_t DefVal, Bool_t * Ok,
                 TGWindow *Win,  const char *YNPrompt, Bool_t * YesNo,
                 const char *HelpText, const char *YNPrompt1, Bool_t * YesNo1)
{
   // Prompt for float. The typed in float is returned.

   static char answer[32];

   char defv[32];
   sprintf(defv, "%f", DefVal);

   Int_t ret;
   new TGMrbInputDialog(Prompt, defv, &ret, answer, Win, YNPrompt, YesNo, HelpText, YNPrompt1, YesNo1);
   if(Ok){
      if(ret == 0) *Ok = kTRUE;
      else         *Ok = kFALSE;
   }
   return atof(answer);
}
