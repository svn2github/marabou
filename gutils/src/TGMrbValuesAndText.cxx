namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TROOT.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include "TRootHelpDialog.h"
#include <TList.h>
#include <TObjString.h>
#include <TRegexp.h>

#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGColorSelect.h>
#include "TGMrbValuesAndText.h"
#include "TGMrbGetTextAlignment.h"
#include "TGMrbHelpWindow.h"
#include "SetColor.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Dialog Widget to request input of a string                            // 
// optionally 1 col table of values can be filled out                    //   
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const char *Prompt: the Prompt string                                //
//  TString * text:     the value of the string                          //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //                  
//                                                                       //
// optional arguments:                                                   //
//                                                                       //
// TGWindow *win       : pointer to parent window                        //  
// const char *FileName: a file containing lines from which a selection  //
//                       may be made, a TGListBox is used in this case   //
// TList * Complist:     list containing TObjStrings used for            //
//                       command completion                              //
//                                                                       //
// This class is normally accessed via the following utility functions:  //
//                                                                       //
// void GetStringExt(const char *Prompt, const char * DefVal,            //
// 							 Bool_t * Ok = 0, TGWindow *win=0,					 //
// 							 const char *ynprompt=0, Bool_t * yn=0,			 //
// 							 const char *helptext=0);								 //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

extern Ssiz_t IndexOfLastSpecial(TString &str);
extern Ssiz_t IndexOfLast(TString &str, char &c);
extern Int_t Matches(TList * list, const char * s, Int_t * matchlength);
//________________________________________________________________________________________
enum buttonId {kIdOk = 101, kIdCancel = 102, kIdHelp = 103,
               kIdText = 201, kIdTextSelect, kIdFontS, 
               kIdLineS, kIdArrowS, kIdAlignS, kIdMarkS, kIdFillS};
 
ClassImp(TGMrbValuesAndText)

TGMrbValuesAndText::TGMrbValuesAndText(const char *Prompt, TString * text, 
                  Int_t * ok, Int_t win_width, const TGWindow *Win,
                  const char * FileName, TList * complist,
                  TList * RowLabels, TList * Values,
                  TArrayI * Flags, const char * Flagslabel,
                  const char *helptext):
							    TGTransientFrame(gClient->GetRoot(), Win, 10, 10)
{
   // Create  input dialog.
   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);

   fWidgets = new TList;
   fFlagButtons = new TList;
   fListBox = NULL;
   fValues = Values;
   fFlags = Flags;
   fText = text;
   fHelpText = helptext;
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;
   fCompList = complist;
   fReturn = ok;
//   cout << "win_width " <<win_width << endl;

   TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX , 2, 2, 2, 2);
   TGLayoutHints * l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
   TGLayoutHints * l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   TGLayoutHints * l3 = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);
   fWidgets->AddFirst(l3);
   fWidgets->AddFirst(l1);
   fWidgets->AddFirst(l2);
   fWidgets->AddFirst(lo1);

//  table part

   if (RowLabels) {
      TGCompositeFrame * hframe, * hframe1;
      TGLabel * label;
      TGTextEntry * tentry;
      TGButton * cbutton;
      TGTextBuffer * tbuf;
      TString s;
      fNrows = Values->GetSize();
      if(fNrows <= 0 || fNrows != RowLabels->GetSize()) {
         cout << "TGMrbValuesAndText:: Value array has illegal size: <= 0 || !RowLabes size" << endl;
         return;
      }
      if (fFlags && Flagslabel) {
         hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);
         fWidgets->Add(hframe);
         hframe1 = new TGCompositeFrame(hframe, win_width*5/6, 20, kFixedWidth);
         fWidgets->Add(hframe1);
         hframe->AddFrame(hframe1, l2);
         hframe1 = new TGCompositeFrame(hframe, win_width*1/6, 20, kFixedWidth);
         fWidgets->Add(hframe1);
         label = new TGLabel(hframe1, new TGString(Flagslabel));
			fWidgets->Add(label);
         hframe1->AddFrame(label, l3);
         hframe->AddFrame(hframe1, l2);

         this->AddFrame(hframe, lo1);
      }
      fEntries = new TList();
      for (Int_t i= 0; i < fNrows; i++) {
         hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);  
         fWidgets->Add(hframe);
         s = ((TObjString *)RowLabels->At(i))->String();
         label = new TGLabel(hframe, new TGString((const char *)s));
			fWidgets->Add(label);
         hframe->AddFrame(label, l3);
//
         s = ((TObjString *)Values->At(i))->String();

         if (s.BeginsWith("CheckButton")) {
            cbutton = new TGCheckButton(hframe, new TGHotString(""), i);
            if (s.EndsWith("Down")) cbutton->SetState(kButtonDown);
            else                    cbutton->SetState(kButtonUp);
            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            hframe->AddFrame(cbutton, lo1);
         } else if (s.BeginsWith("RadioButton")) {
            cbutton = new TGRadioButton(hframe, new TGHotString(""), i);
            if (s.EndsWith("Down")) cbutton->SetState(kButtonDown);
            else                    cbutton->SetState(kButtonUp);
            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            cbutton->Associate(this);
            hframe->AddFrame(cbutton, lo1);
         } else if (s.BeginsWith("ColorSelect_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            Int_t col = atoi(scol.Data());
            cbutton = new TGColorSelect(hframe, col, i);
            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            hframe->AddFrame(cbutton, lo1);

         } else if (s.BeginsWith("Mark_Select_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fMarker = atoi(scol.Data());
            fMarkerSelect = new TGedMarkerSelect(hframe, fMarker, kIdMarkS);
            fMarkerSelect->Associate(this);
//            fMarkerSelect->Connect("MarkerSelected(Style_t)", 
//            "TGMrbValuesAndText", this, "DoMarkerStyle(Style_t)");
            fWidgets->Add(fMarkerSelect);
            fEntries->Add(fMarkerSelect);
            hframe->AddFrame(fMarkerSelect, lo1);

         } else if (s.BeginsWith("Fill_Select_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fPattern = atoi(scol.Data());
            fPatternSelect = new TGedPatternSelect(hframe, fPattern, kIdFillS);
            fPatternSelect->Associate(this);
//            fPatternSelect->Connect("PatternSelected(Style_t)", 
//            "TGMrbValuesAndText", this, "DoPatternStyle(Style_t)");
            fWidgets->Add(fPatternSelect);
            fEntries->Add(fPatternSelect);
            hframe->AddFrame(fPatternSelect, lo1);

         } else if (s.BeginsWith("AlignSelect_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fAlign = atoi(scol.Data());
            TString icon_name("align");
            icon_name += fAlign;
            icon_name += ".gif";
            fAlignButton = new TGPictureButton(
                           hframe, fClient->GetPicture(icon_name.Data()), kIdAlignS);
            fWidgets->Add(fAlignButton);
            fEntries->Add(fAlignButton);
            fAlignButton->Associate(this);
            hframe->AddFrame(fAlignButton, lo1);

         } else if (s.BeginsWith("CfontSelect_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fFont = atoi(scol.Data());
            fFontComboBox = new TGFontTypeComboBox(hframe, kIdFontS);
            fFontComboBox->Resize(fFontComboBox->GetDefaultWidth(), 20);
            fFontComboBox->Select(fFont);
            fWidgets->Add(fFontComboBox);
            fEntries->Add(fFontComboBox);
            fFontComboBox->Associate(this);
            hframe->AddFrame(fFontComboBox, lo1);

         } else if (s.BeginsWith("LineSSelect_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fLineStyle = atoi(scol.Data());
            fLineStyleComboBox = new TGLineStyleComboBox(hframe, kIdLineS);
            fLineStyleComboBox->Resize(fLineStyleComboBox->GetDefaultWidth(), 20);
            fLineStyleComboBox->Select(fLineStyle);
            fWidgets->Add(fLineStyleComboBox);
            fEntries->Add(fLineStyleComboBox);
            fLineStyleComboBox->Associate(this);
            hframe->AddFrame(fLineStyleComboBox, lo1);

         } else if (s.BeginsWith("ArrowSelect_")) {
            TString scol(s.Data());
            scol.Remove(0,12);
            fArrowShape = atoi(scol.Data());
            fArrowComboBox = new TGComboBox(hframe, kIdArrowS);
            fArrowComboBox->AddEntry(" -------|>",1);
            fArrowComboBox->AddEntry(" <|-------",2);
            fArrowComboBox->AddEntry(" -------->",3);
            fArrowComboBox->AddEntry(" <--------",4);
            fArrowComboBox->AddEntry(" ---->----",5);
            fArrowComboBox->AddEntry(" ----<----",6);
            fArrowComboBox->AddEntry(" ----|>---",7);
            fArrowComboBox->AddEntry(" ---<|----",8);
            fArrowComboBox->AddEntry(" <------>", 9);
            fArrowComboBox->AddEntry(" <|-----|>",10);
            fArrowComboBox->Resize(fArrowComboBox->GetDefaultWidth(), 20);
//            (fArrowComboBox->GetListBox())->Resize(, 136);
            fArrowComboBox->Select(fArrowShape);
            fWidgets->Add(fArrowComboBox);
            fEntries->Add(fArrowComboBox);
            fArrowComboBox->Associate(this);
            hframe->AddFrame(fArrowComboBox, lo1);

         } else {
            tentry = new TGTextEntry(hframe, tbuf = new TGTextBuffer(100), kIdText);
            tentry->GetBuffer()->AddText(0, (const char *)s);
            tentry->Resize(win_width/2, tentry->GetDefaultHeight());
            hframe->AddFrame(tentry, l3);
			   fWidgets->Add(tentry);
            fEntries->Add(tentry);
         }
         if (fFlags) {
            hframe1 = new TGCompositeFrame(hframe, win_width*1/6, 20, kFixedWidth);
            fWidgets->Add(hframe1);
            cbutton = new TGCheckButton(hframe1, new TGHotString(""), i + 300);
            fFlagButtons->Add(cbutton);
            if (fFlags->At(i) == 1) cbutton->SetState(kButtonDown);
            else                cbutton->SetState(kButtonUp);
			   fWidgets->Add(cbutton);
            hframe1->AddFrame(cbutton, l3);
            hframe->AddFrame(hframe1, l2);
         }
         this->AddFrame(hframe, lo1);
      }
   }
// create Prompt label and textentry widget
    
   if (fText) {
      TGLabel *label = new TGLabel(this, Prompt);
      fWidgets->AddFirst(label);

      TGTextBuffer *tbuf = new TGTextBuffer(256);  // will be deleted by TGtextEntry
      tbuf->AddText(0, text->Data());

      fTE = new TGTextEntry(this, tbuf);
      fTE->Resize(win_width, fTE->GetDefaultHeight());
      fTE->Associate(this);
      fWidgets->AddFirst(fTE);


      this->AddFrame(label, l1);
      this->AddFrame(fTE, l2);

      // optionally create a ListBox from which items can be selected
      Int_t wid;
      fFileName = FileName;         // remember
      if (FileName != NULL) {
         if (gSystem->AccessPathName(FileName)) {
            cout << "Warning: File with selections not found: " << FileName << endl;
         } else {
            ifstream selections(FileName);
            TString line;
            Int_t id = 0;
            fListBox = new TGListBox(this, kIdTextSelect);  
            while (1) {
               line.ReadLine(selections);
               if (selections.eof()) break;
               fListBox->AddEntry(line.Data(), id);
               if (id == 00 && (Int_t)fTE->GetBuffer()->GetTextLength() == 0) {
                  fTE->GetBuffer()->AddText(0, line.Data());
                  gClient->NeedRedraw(fTE);
               }
               id++;
            }
            selections.close();
            cout << "Entries: " << id << endl;
            wid = 8 * fTE->GetBuffer()->GetTextLength();
            fListBox->Resize(TMath::Max(wid,320), TMath::Min(200, id*20));
            fListBox->Layout();
            fListBox->Associate(this);
            fWidgets->AddFirst(fListBox);
            this->AddFrame(fListBox, l2);
         }
      }
      wid = 8 * fTE->GetBuffer()->GetTextLength();
      fTE->Resize(TMath::Max(wid,320), fTE->GetDefaultHeight());
   }
   // create frame and layout hints for Ok and Cancel buttons

   TGHorizontalFrame *hf = new TGHorizontalFrame(this, 60, 20, kFixedWidth);
   fWidgets->AddFirst(hf);
   // put hf as last in list to be deleted

   // create OK and Cancel buttons in their own frame (hf)

   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", kIdOk);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   b = new TGTextButton(hf, "&Cancel", kIdCancel);
   fWidgets->AddFirst(b);
   b->Associate(this);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
   if(helptext){
      b = new TGTextButton(hf, "Help", kIdHelp);
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
   hf->ChangeBackground(brown);

   gClient->WaitFor(this);
};

Bool_t TGMrbValuesAndText::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON:
                switch (parm1) {
                   case kIdOk:
                      // here copy the string from text buffer to return variable
                      if (fText) *fText = fTE->GetBuffer()->GetString();
                      if (fFileName && fFileName.Length() > 0) this->SaveList();
                      StoreValues();
                      *fReturn = 0;
                      delete this;
                      break;

                    case kIdCancel:
                      *fReturn = -1;
                      delete this;
                      break;
                  case kIdHelp:
                      new TGMrbHelpWindow(this, "HistPresent Help", fHelpText, 550, 500);
                      break;
                  case kIdAlignS:
                      fAlign = GetTextAlign(fAlign, this);
                      TString icon_name("align");
                      icon_name += fAlign;
                      icon_name += ".gif";
                      fAlignButton->SetPicture(fClient->GetPicture(icon_name.Data()));
                      gClient->NeedRedraw(fAlignButton);
                      break;
                 }
             case kCM_RADIOBUTTON:
                {   
                   TIter nextent(fEntries);
                   TObject * obj;
                   Int_t i = 0;
                   while ( (obj = nextent()) ) {
                      if (obj->InheritsFrom("TGRadioButton"))  { 
                         if (i == parm1) 
                           ((TGRadioButton*)obj)->SetState(kButtonDown);
                         else 
                           ((TGRadioButton*)obj)->SetState(kButtonUp);
                      }
                      i++;
                   }
                   break;
                }
             case kCM_COMBOBOX:
                {
                  break;
/*
                  switch (parm1) {
                   case kIdArrowShape:
                      fArrowShape = parm2;
                      break;
                   case kIdFontSelect:
                      fFont = parm2;
                      break;
                   case kIdLineStyle:
                      fLineStyle = parm2;
                      break;
                   }
*/
                }

             case kCM_LISTBOX:
                {
                switch (parm1) {
                   case kIdTextSelect:
                      TGTextLBEntry * tge = (TGTextLBEntry *)fListBox->GetEntry(parm2);
                      TString txt = tge->GetText()->GetString();
                      fTE->SetText(txt.Data());
                      gClient->NeedRedraw(fTE);
                      break;
                   }
                }
             default:
                 break;
          }
          break;

       case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
             case kTE_TAB:
 //              cout << "Tab " << parm1 << " " << fCompList<< endl;
//                if (parm1 != kIdText) break;
                if (fCompList) {
                   TString temp = fTE->GetBuffer()->GetString();
//                   cout << "temp: " << temp << endl;
//                   char colon = ':';
                   Int_t indcol = IndexOfLastSpecial(temp);
                   TString var = temp(indcol+1, temp.Length() - indcol - 1);
//                   cout << "var: " << var << endl;
                   Int_t matchlength;
                   Int_t pos = Matches(fCompList, var.Data(), &matchlength);
                   if (pos >= 0) {
                      TObjString * obs = (TObjString *)fCompList->At(pos);
                      TString result = (obs->GetString()).Data();
                      if (matchlength != result.Length())result = result(0,matchlength);
                      temp.Replace(indcol + 1, temp.Length() - indcol - 1, result.Data());
                      fTE->SetText(temp.Data());
                   }
                }
                break;
             case kTE_ENTER:
                if (parm1 != kIdText) break;
                // here copy the string from text buffer to return variable
                *fText = fTE->GetBuffer()->GetString();
                if (fFileName.Length() > 0) this->SaveList();
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

//_____________________________________________________________________________

void TGMrbValuesAndText::StoreValues(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbValuesAndText::StoreValues()
// Purpose:        Store return values
// Arguments:  
//             
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
  
   if (!fEntries) return;

   TIter nextent(fEntries);
   TIter next(fValues);
   TObjString *objs;

   TObject * obj;
   TGCheckButton * cbutton;
   TGTextEntry *tentry;
   TGColorSelect * colsel;
   Int_t i=0;
   while((objs = (TObjString*)next())){
       obj=nextent();
       if (obj->InheritsFrom("TGColorSelect")){ 
          colsel = (TGColorSelect*)obj;
          Int_t col = colsel->GetColor();
          TString scol("ColorSelect_");
          scol += col;
          objs->SetString(scol.Data());

       } else if (obj->InheritsFrom("TGPictureButton")) {
          TString salign("AlignSelect_");
          TString pn = ((TGPictureButton*)obj)->GetPicture()->GetName();
//          cout << pn << endl;
          pn.Remove(0,5);    // e,g. align22.gif
          pn.Resize(2);
          salign += pn;
          objs->SetString(salign.Data());

       } else if (obj->InheritsFrom("TGedMarkerSelect")) {
          fMarker = ((TGedMarkerSelect*)obj)->GetMarkerStyle(); 
          TString salign("Mark_Select_");
          salign += fMarker;          // only 1 marker select allowed
          objs->SetString(salign.Data());

       } else if (obj->InheritsFrom("TGedPatternSelect")) {
          Int_t sel = ((TGedPatternSelect*)obj)->GetPattern(); 
          TString salign("Fill_Select_");
          salign += sel;          // only 1 marker select allowed
          objs->SetString(salign.Data());

       } else if (obj->InheritsFrom("TGComboBox")) {
          if (((TGComboBox*)obj)->WidgetId() == kIdArrowS) {
             TString salign("ArrowSelect_");
             Int_t sel = ((TGComboBox*)obj)->GetSelected();
             salign += sel;
             objs->SetString(salign.Data());

          } else if (((TGComboBox*)obj)->WidgetId() == kIdFontS) {
             TString salign("CfontSelect_");
             Int_t sel = ((TGComboBox*)obj)->GetSelected();
             salign += sel;
             objs->SetString(salign.Data());

          } else if (((TGComboBox*)obj)->WidgetId() == kIdLineS) {
             TString salign("LineSSelect_");
             Int_t sel = ((TGComboBox*)obj)->GetSelected();
             salign += sel;
             objs->SetString(salign.Data());
          }
       } else if (obj->InheritsFrom("TGCheckButton")) {
          cbutton = (TGCheckButton*)obj;
          if (cbutton->GetState() == kButtonDown)
             objs->SetString("CheckButton_Down"); 
          else
             objs->SetString("CheckButton_Up");

       } else if (obj->InheritsFrom("TGRadioButton")) {
          cbutton = (TGCheckButton*)obj;
          if (cbutton->GetState() == kButtonDown)
             objs->SetString("RadioButton_Down"); 
          else
             objs->SetString("RadioButton_Up");

       } else { 
          tentry = (TGTextEntry*)obj;
          const char * te = tentry->GetBuffer()->GetString();
          objs->SetString((char *)te);
       } 

       if (fFlags) {
          cbutton = (TGCheckButton*)fFlagButtons->At(i);
          if (cbutton->GetState() == kButtonDown) (*fFlags)[i]= 1;
          else                                    (*fFlags)[i]= 0;
       }
       i++; 
   }
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::SaveList()
{
   if (fListBox == NULL) {
      fListBox = new TGListBox(this, kIdTextSelect);           
      fListBox->AddEntry(fTE->GetBuffer()->GetString(), 0);
      fWidgets->AddFirst(fListBox);
   }
   if (!(gSystem->AccessPathName(fFileName.Data()))) {
      TString bak(fFileName);
      bak += ".bak";
      gSystem->Rename(fFileName.Data(), bak.Data());
   }
   ofstream outfile(fFileName.Data());
   TString sel(fTE->GetBuffer()->GetString());
   Int_t ne = fListBox->GetNumberOfEntries();
//   cout << "Selected: " << sel << endl;
   outfile << sel << endl;
   if (ne > 0) {
      for (Int_t i =0; i < ne; i++) {
         TGLBEntry * tle = fListBox->GetEntry(i);
         TGTextLBEntry * tge = (TGTextLBEntry *)tle;
         TString text = tge->GetText()->GetString();
//         cout << "Ent: " << text.Data() << endl;
         if (text != sel) outfile <<text.Data() << endl;
      }
   }
   outfile.close();
}

//_______________________________________________________________________________________

Bool_t GetStringExt(const char *Prompt, TString  *text , 
                        Int_t win_width, TGWindow *Win,  
                        const char *FileName, TList * Complist,
                        TList * rowlabs, TList * values,
                        TArrayI * Flags, const char * Flagslabel,
                        const char *helptext)
{ 
   // Prompt for string. The typed in string is returned.

//   static char answer[128];
   Int_t ret;
   new TGMrbValuesAndText(Prompt, text, &ret, win_width,  Win,  
       FileName, Complist, rowlabs, values,Flags, Flagslabel, helptext);
   if(ret == 0) return kTRUE;
   else         return kFALSE;
}
