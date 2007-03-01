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
#include <TFile.h>
#include <TKey.h>
#include <TColor.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRegexp.h>

#include <TGResourcePool.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGColorSelect.h>
#include "TGMrbValuesAndText.h"
//#include "TGedAlignSelect.h"
#include "SetColor.h"


///////////////////////////////////////////////////////////////////////////
//                                                                       //
// General purpose dialog widget to request input of a text string       // 
// and any number of ints, floats, colors etc. arranged in a table       //
// each optionally with a checkbutton                                    //
// The string input part implements a history and text completion        //
// facility                                                              //
//                                                                       //
// The meaning of the requested values are stored in the first 11        //
// characters terminated by _, + of the row labels as follows:           //
// The termination char _, + determine if the dialog entry starts        //
// on new row: _: start new row, +: continue on same row                 //
// 																							 //
// StringValue : Text string															 //
// PlainShtVal : Short_t																 //
// PlainIntVal : Int_t  																 //
// DoubleValue : Double_t																 //
// Float_Value : Float_t																 //
// ArrowSelect : a arrow selection (Style_t) 									 //
// LineSSelect : Line Style  (Style_t) 											 //
// CfontSelect : Character Font (Font_t)											 //
// AlignSelect : Text alignment (Style_t) 										 //
// Fill_Select : Fill style															 //
// Mark_Select : Marker style 														 //
// ColorSelect : Color_t																 //
// RadioButton : radio button 														 //
// CheckButton : check button 														 //
// FileRequest : file name (invokes file dialog)								 //
// FileContReq : invoke file dialog and present listbox with content		 //
//             : arg: filename | classname | object name                 //
// CommandButt : Command button implemented as follows:						 //
//               assume: CommandButt_DrawArrow()								 //
//               when pressed will emit:											 //
//               ("Clicked()", Char_t * cname,  								 //
//                 TObject * calling_class,"DrawArrow()")					 //
// 																							 //
// and a help text button                                                //
//                                                                       //
// required arguments:                                                   //
//                                                                       //
//  const char *Prompt: the Prompt string                                //
//  TString * text:     the value of the string                          //
//  Int_t * retval    : 0 if ok pressed, -1 if cancel                    //
//  Int_t   win_width : Width (pixels) of the dialog  widget             //
//                                                                       //
//  optional arguments:                                                  //
//                                                                       //
// TGWindow *win        pointer to parent window								 //
// Char_t * FileName:   a file containing lines from which a selection   //
//                      may be made, a TGListBox is used in this case	 //
//                      This is used to store newly type text lines not  //
//                      yet in the list											 //
// TList * Complist:    list containing TObjStrings used for				 //
//                      text completion (tab)									 //
// TList * rowlabs:     List of TObjStrings of row labels					 //
// void ** valpointers  array with pointers to the requested values  	 //
// TArrayI * Flags      pointer to integer array of flags, a checkbutton //
//                      is added to each value  								 //
// Char_t * flaglabel   label of the (opotional)flags / checkbuttons 	 // 
//                      column														 //
// Char_t * helptext    helptext displayed when pressing (optional)  	 //
//                      Help button 												 //
// TObject * class      pointer to the calling class used in the emitted //
//                      signal triggered by a command button				 //
// Char_t * classname   name of the calling class  							 //
//                                                                       //
//                                                                       //
// This class is normally accessed via the following utility function:   //
//                                                                       //
//                                                                       //
// Bool_t GetStringExt(const char *Prompt, TString  *text , 				 //
// 							  Int_t win_width, TGWindow *Win,   				 //
// 							  const char *FileName, TList * Complist, 		 //
// 							  TList * rowlabs, void **val_pointers,			 //
// 							  TArrayI * Flags, const char * Flagslabel,		 //
// 							  const char *helptext, TObject * calling_class, //
// 							  const char * cname)									 //
//  																							 //
//  																							 //
///////////////////////////////////////////////////////////////////////////
//
//extern Ssiz_t IndexOfLastSpecial(TString &str);
//extern Ssiz_t IndexOfLast(TString &str, char &c);
//extern Int_t Matches(TList * list, const char * s, Int_t * matchlength);
//________________________________________________________________________________________

Ssiz_t IndexOfLastSpecial(TString &str) 
{
   // return index  of not (letter, number or _ ) in str searching backwards
   // Skip ::
   TRegexp spc("[^a-zA-Z0-9_]", kFALSE);
   if (str.Index(spc) < 0) return -1;    // no occurence
   Int_t len = str.Length();
   Int_t ind = len - 1;
   Int_t indspc;
   TString onec;
   const char *sp = str.Data();
   char c = {':'};
   while (ind >= 0) {
//      cout << ind << " " << str.Index(spc, ind) << endl;
      onec = str(ind, 1);
      indspc = onec.Index(spc);
      if (indspc == 0) {
         if (sp[ind] != c) return ind;
         if (sp[ind] == c && (sp[ind -1] != c || ind == 0)) {
            if (ind == 0) cout << setred 
                         << "Warning: Expression begins with ':' "
                         << setblack << endl;
            return ind;
         }
         ind--;
      }
      ind--;
   }
   return -1;
} 
//________________________________________________________________________________________

Ssiz_t IndexOfLast(TString &str, char &c) 
{
   // return index of detached char c in str searching backwards
   if (str.Index(c) < 0) return -1;    // no occurence
   Int_t len = str.Length();
   const char *sp = str.Data();
//   cout << sp << endl;
   Int_t ind = len - 1;
   while (ind > 0) {
      if (sp[ind] == c && sp[ind -1] != c) return ind;
      ind--;
      if (sp[ind+1] == c)ind--;
   }
   return -1;
} 
//________________________________________________________________________________________

Int_t Matches(TList * list, const char * s, Int_t * matchlength)
{
   Int_t pos = -1;
   Bool_t unique = kTRUE;
   TObjString * obs;
   Int_t lmax = 1000000;
   for (Int_t i = 0; i < list->GetSize(); i++) {
      obs = (TObjString *)list->At(i);
      TString var = obs->GetString();
      if (var.Index(s, 0) == 0) {
//        cout << "Input: " << s << " matches: " << var << endl;
         if (pos >=0) unique = kFALSE;
         pos = i;
         if (var.Length() < lmax) lmax = var.Length(); 
         *matchlength = var.Length();
      }
   }
   if (unique) {
      return pos;
   } else {
      cout << setred << "Not unique, find  longest match: lmax = " << lmax << setblack << endl;
      Int_t lm = 0;
      for (Int_t l = 0; l < lmax; l++) {
         Bool_t fm = kTRUE;
         char cm ={' '};
         for (Int_t i = 0; i < list->GetSize(); i++) {
            obs = (TObjString *)list->At(i);
            TString var = obs->GetString();
            if (var.Index(s, 0) != 0) continue;
            if (fm) {
               cm = var[l];
               fm = kFALSE;
            } else {
               if (var[l] != cm) {
                  *matchlength = lm;
//                  cout << "pso, lm: " << pos << " " << lm << endl;
                  return pos;
               } 
            }
         }
         lm++;    
      }
   }
   *matchlength = lmax;
   return pos;
}
;

//#include "TGResourcePool.h"
//#include "TGPicture.h"
//#include "TGToolTip.h"
//#include "TGButton.h"
//#include "Riostream.h"

ClassImp(TGedAlignSelect)
ClassImp(TGedAlignPopup)

EWidgetMessageTypes kC_ALIGNSEL = (EWidgetMessageTypes)1001, 
                    kALI_SELCHANGED = (EWidgetMessageTypes)2;


struct alignDescription_t {
//   const char* filename;
   const char* pictname;
   const char* name;
   int number;
   int seqnr;
};

static alignDescription_t  Aligns[] = {
/*
   {"align13.gif","13 Top, Left",      13},
   {"align23.gif","23 Top, Middle",    23},
   {"align33.gif","33 Top, Right",     33},
   {"align12.gif","12 Middle, Left",   12},
   {"align22.gif","22 Middle, Middle", 22},
   {"align32.gif","32 Middle, Right",  32},
   {"align11.gif","11 Bottom, Left",   11},
   {"align21.gif","21 Bottom, Middle", 21},
   {"align31.gif","31 Bottom, Right",  31},
*/
   {"align13","13 Top, Left",      13, 0},
   {"align23","23 Top, Middle",    23, 1},
   {"align33","33 Top, Right",     33, 2},
   {"align12","12 Middle, Left",   12, 3},
   {"align22","22 Middle, Middle", 22, 4},
   {"align32","32 Middle, Right",  32, 5},
   {"align11","11 Bottom, Left",   11, 6},
   {"align21","21 Bottom, Middle", 21, 7},
   {"align31","31 Bottom, Right",  31, 8},
   {0, 0, 0, 0}
};
   /* XPM */
   static char *align11[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align12[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align13[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%      %%%%%%%%%%%%%",
   "%%    %%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align21[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align22[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align23[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%      %%%%%%%",
   "%%%%%%%%    %%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align31[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align32[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char *align33[] = {
   /* columns rows colors chars-per-pixel */
   "20 20 2 1",
   "  c black",
   "% c #FDFDFE",
   /* pixels */
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%      %",
   "%%%%%%%%%%%%%%    %%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%",
   "%%%%%%%%%%%%%%%%%%%%"
   };
   static char **pixarr[9] = {
      &align13[0],
      &align23[0],
      &align33[0],
      &align12[0],
      &align22[0],
      &align32[0],
      &align11[0],
      &align21[0],
      &align31[0]
   };


static alignDescription_t* GetAlignByNumber(int number)
{
   for (int i = 0; Aligns[i].number != 0; i++) {
      if (Aligns[i].number == number)
          return &Aligns[i];
   }
   return 0;
}

static const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "Data files",    "*.dat",
                            "Text files",    "*.txt",
                            "Text files",    "*.asc",
                            "Text files",    "*.ascii",
                            0,               0 };

//______________________________________________________________________________
TGedAlignPopup::TGedAlignPopup(const TGWindow *p, const TGWindow *m, Style_t alignStyle)
   : TGedPopup(p, m, 30, 30, kDoubleBorder | kRaisedFrame | kOwnBackground,
               GetDefaultFrameBackground())
{
/*
   pixarr[0] = &align13[0];
   pixarr[1] = &align23[0];
   pixarr[2] = &align33[0];
   pixarr[3] = &align12[0];
   pixarr[4] = &align22[0];
   pixarr[5] = &align32[0];
   pixarr[6] = &align11[0];
   pixarr[7] = &align21[0];
   pixarr[8] = &align31[0];
*/
   TGButton *b;
   fCurrentStyle = alignStyle;

   Pixel_t white;
   gClient->GetColorByName("white", white); // white background
   SetBackgroundColor(white);

   SetLayoutManager(new TGTileLayout(this, 1));
   for (int i = 0; Aligns[i].number != 0; i++) {
      const TGPicture * pict = gClient->GetPicturePool()->GetPicture(Aligns[i].pictname, pixarr[i]);
      AddFrame(b = new TGPictureButton(this, pict,
               Aligns[i].number, TGButton::GetDefaultGC()(), kSunkenFrame),
               new TGLayoutHints(kLHintsLeft, 14, 14, 14, 14));
      b->SetToolTipText(Aligns[i].name);
//      cout << Aligns[i].filename << endl;
   }

   Resize(76, 76);
   MapSubwindows();
}

//______________________________________________________________________________
TGedAlignPopup::~TGedAlignPopup()
{
   Cleanup();
}

//______________________________________________________________________________
Bool_t TGedAlignPopup::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   cout << "TGedAlignPopup::ProcessMessage " << GET_MSG(msg) << endl;
   if (GET_MSG(msg) == kC_COMMAND && GET_SUBMSG(msg) == kCM_BUTTON) {
      SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED), 0, parm1);
      EndPopup();
   }

   if (parm2)
      ;              // no warning

   return kTRUE;
}

//______________________________________________________________________________
TGedAlignSelect::TGedAlignSelect(const TGWindow *p, Style_t alignStyle, Int_t id)
   : TGedSelect(p, id)
{
//   cout << "TGedAlignSelect::ctor " << alignStyle << endl;
   fPicture = 0;
   SetPopup(new TGedAlignPopup(gClient->GetDefaultRoot(), this, alignStyle));
   SetAlignStyle(alignStyle);

}

//_____________________________________________________________________________
Bool_t TGedAlignSelect::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   cout << "TGedAlignSelect::ProcessMessage " << GET_MSG(msg) << endl;
   if (GET_MSG(msg) == kC_ALIGNSEL && GET_SUBMSG(msg) == kALI_SELCHANGED) {
      SetAlignStyle(parm2);
      SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED),
                  fWidgetId, parm2);
   }

   if (parm1)     // no warning
      ;
   return kTRUE;
}

//_____________________________________________________________________________

void TGedAlignSelect::DoRedraw()
{
   TGedSelect::DoRedraw();

   Int_t  x, y;
   UInt_t w, h;

   if (IsEnabled()) {
      // pattern rectangle
//      cout << "TGedAlignSelect::DoRedraw() " << fPicture<< endl;
      x = fBorderWidth + 2;
      y = fBorderWidth + 2;  // 1;
      h = fHeight - (fBorderWidth * 2) - 4;  // -3;  // 14
      w = h;
      if (fState == kButtonDown) {
         ++x; ++y;
      }
      gVirtualX->DrawRectangle(fId, GetShadowGC()(), x, y, w - 1, h - 1);

      if (fPicture != 0) fPicture->Draw(fId, fDrawGC->GetGC(), x + 1, y + 1);
   } else { // sunken rectangle
      x = fBorderWidth + 2;
      y = fBorderWidth + 2;  // 1;
      w = 42;
      h = fHeight - (fBorderWidth * 2) - 4;  // 3;
      Draw3dRectangle(kSunkenFrame, x, y, w, h);
   }
}

//_____________________________________________________________________________
void TGedAlignSelect::SetAlignStyle(Style_t alignStyle)
{
   // Set align.

   fAlignStyle = alignStyle;
//   gClient->NeedRedraw(this);

   if (fPicture) {
      gClient->FreePicture(fPicture);
      fPicture = 0;
   }

   alignDescription_t *md = GetAlignByNumber(fAlignStyle);

//   cout << "TGedAlignSelect::SetAlignStyle " << md->filename << endl;
   if (md) {
//   cout << "TGedAlignSelect::SetAlignStyle " << fAlignStyle << " " << md->pictname << endl;
//      fPicture = gClient->GetPicture(md->filename);
      fPicture = gClient->GetPicturePool()->GetPicture(md->pictname, pixarr[md->seqnr]);
//      fPicture = gClient->GetPicture(md->filename);
      SetToolTipText(md->name);
   }
   gClient->NeedRedraw(this);
   AlignSelected(fAlignStyle);
}

//______________________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void TGedAlignSelect::SavePrimitive(ostream &out, Option_t *)
#else
void TGedAlignSelect::SavePrimitive(ofstream &out, Option_t *)
#endif 
{
   // Save the pattern select widget as a C++ statement(s) on output stream out

   out <<"   TGedAlignSelect *";
   out << GetName() << " = new TGedAlignSelect(" << fParent->GetName()
       << "," << fAlignStyle << "," << WidgetId() << ");" << endl;
}
//________________________________________________________________________________________

enum buttonId {kIdOk = 101, kIdCancel = 102, kIdHelp = 103, kIdClearHist = 104,
               kIdText = 201, kIdTextValue = 301, kIdTextSelect, kIdListBoxReq = 401,
               kIdFileDialog = 4, kIdFileDialogCont = 5, kIdFontS = 6, kIdCommand = 7, 
               kIdLineS, kIdArrowS, kIdAlignS, kIdMarkS, kIdFillS};
 
ClassImp(TGMrbValuesAndText)

//________________________________________________________________________________________

TGMrbValuesAndText::TGMrbValuesAndText(const char *Prompt, TString * text, 
                  Int_t * ok, Int_t win_width, const TGWindow *Win,
                  const char * FileName, TList * complist,
                  TList * RowLabels, void **val_pointers,
                  TArrayI * Flags, const char * Flagslabel,
                  const char *helptext, TObject * calling_class, 
                  const char * cname):
							    TGTransientFrame(gClient->GetRoot(), Win, 10, 10),
                         fRedTextGC(TGButton::GetDefaultGC())
{
   // Create  input dialog.
   ULong_t brown;
   gClient->GetColorByName("firebrick", brown);
   Pixel_t red, blue;
   fClient->GetColorByName("red", red);
   fRedTextGC.SetForeground(red);
   fClient->GetColorByName("blue", blue);
   fBlueTextGC.SetForeground(blue);

   TGGC myGC = *fClient->GetResourcePool()->GetFrameGC();
   TGFont *myfont = fClient->GetFont("-adobe-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   if (myfont) myGC.SetFont(myfont->GetFontHandle());

   fWidgets = new TList;
   fFlagButtons = new TList;
//   fFinis = 0;
   fEmitClose = kTRUE;
   fListBox = NULL;
   fLabels = RowLabels;
   fValPointers = val_pointers;
   fFlags = Flags;
   fText = text;
   fPrompt = Prompt;
   fHelpText = helptext;
   const TGWindow * main = gClient->GetRoot();
   if(Win != 0)fMyWindow = Win;
   else        fMyWindow = main;
   fCompList = complist;
   fReturn = ok;
   *ok = -1;    // if closed by cancel
   TString CancelCmd;
   Bool_t has_commands = kFALSE;
   fCallClose = kTRUE;
   
   if (calling_class != NULL && win_width > 0) {
      this->Connect("CloseWindow()",cname, calling_class, "CloseDown()");
   }
   if (win_width < 0) {
      fCallClose = kFALSE;
      win_width *= -1;
   } 

//   cout << "win_width " <<win_width << endl;

   TGLayoutHints * lo1 = new TGLayoutHints(kLHintsExpandX|kLHintsCenterY , 2, 2, 2, 2);
   TGLayoutHints * lor = new TGLayoutHints( kLHintsRight , 2, 2, 2, 2);
   TGLayoutHints * locy = new TGLayoutHints( kLHintsCenterY , 2, 2, 2, 2);
   TGLayoutHints * l1 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 3, 3, 0, 0);
   TGLayoutHints * l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
   TGLayoutHints * l3 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY | kLHintsExpandX, 3, 3, 0, 0);
   fWidgets->AddFirst(l3);
   fWidgets->AddFirst(lor);
   fWidgets->AddFirst(l1);
   fWidgets->AddFirst(l2);
   fWidgets->AddFirst(lo1);

//  table part

   TGCompositeFrame * hframe = NULL , * hframe1 = NULL;
   if (RowLabels) {
      TGLabel * label;
      TGTextEntry * tentry;
      TGNumberEntry * tnentry;
      TGButton * cbutton;
      TGTextBuffer * tbuf;
      TString s;
      TString l;
      TString nl;
      fNrows = fLabels->GetSize();
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
         hframe1->AddFrame(label, lor);
         hframe->AddFrame(hframe1, lor);

         this->AddFrame(hframe, lor);
      }
      fEntries = new TList();
      hframe = NULL;
      for (Int_t i= 0; i < fNrows; i++) {
         l = ((TObjString *)RowLabels->At(i))->String();
//       in multi column row save space by avoiding expandx
         TGLayoutHints * loc = lo1;
         if (l[11] == '-') loc = locy;
         if (i < fNrows-1) {
            nl =((TObjString *)RowLabels->At(i+1))->String();
            if (nl[11] == '-') loc = locy;
         }
//       continue in same row if +
         if (l[11] == '_' && hframe != NULL) {
             this->AddFrame(hframe, lo1);
             hframe = NULL;
         }
         if (hframe == NULL) {
            hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);  
            fWidgets->Add(hframe);
         }
         if (!l.BeginsWith("CommandButt")) {
// label 
            TString lab(l);
            if (lab.Length() > 12) {
               lab.Remove(0,12);
               label = new TGLabel(hframe, new TGString((const char *)lab));
			      fWidgets->Add(label);
               if (l.BeginsWith("CommentRigh")) loc = lor;
               hframe->AddFrame(label, loc);
               if (l.BeginsWith("Comment")) fEntries->Add(label);
            }

//            if (l.BeginsWith("CheckButton") || l.BeginsWith("RadioButton"))
//               hframe->AddFrame(label, l3);
 //           else 
//             save space but allow for non vertical align
//               if (l[11] == '+' || l[11] == '-')
//                  hframe->AddFrame(label, l1);
//               else 
//                  hframe->AddFrame(label, l3);
         }

         if (l.BeginsWith("Comment")) {

         } else if (l.BeginsWith("CheckButton")) {
            cbutton = new TGCheckButton(hframe, new TGHotString(""), i);
            Int_t state = *(Int_t*)fValPointers[i];
            if (state != 0) cbutton->SetState(kButtonDown);
            else                    cbutton->SetState(kButtonUp);
//            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
            cbutton->Resize(win_width / 6, cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            cbutton->Associate(this);
            hframe->AddFrame(cbutton, locy);
         } else if (l.BeginsWith("RadioButton")) {
            cbutton = new TGRadioButton(hframe, new TGHotString(""), i);
            Int_t state = *(Int_t*)fValPointers[i];
            if (state != 0) cbutton->SetState(kButtonDown);
            else                    cbutton->SetState(kButtonUp);
//            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
            cbutton->Resize(win_width / 6, cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            cbutton->Associate(this);
            hframe->AddFrame(cbutton, locy);
         } else if (l.BeginsWith("ColorSelect")) {
            Int_t col = GetColorPixelByInd(*(Color_t*)fValPointers[i]);
            cbutton = new TGColorSelect(hframe, col, i);
//            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
//            cbutton->Resize(win_width / 4, cbutton->GetDefaultHeight());
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            cbutton->Associate(this);
            hframe->AddFrame(cbutton, loc);

         } else if (l.BeginsWith("CommandButt")) {
            TString *sr = (TString*)fValPointers[i];
            TString lab(l);
            lab.Remove(0,12);
            cbutton = new TGTextButton(hframe, new TGHotString(lab.Data()), i + 100* kIdCommand,
                          fBlueTextGC());
            cbutton->Connect("Clicked()", cname, calling_class,sr->Data());
            cbutton->Resize(cbutton->GetDefaultWidth(), cbutton->GetDefaultHeight());
//            cbutton->Resize(cbutton->GetDefaultWidth(), 20);
            fWidgets->Add(cbutton);
            fEntries->Add(cbutton);
            hframe->AddFrame(cbutton, lo1);
            has_commands = kTRUE;
            cbutton->Associate(this);
         } else if (l.BeginsWith("Mark_Select")) {
            Style_t fMarker = *(Style_t*)fValPointers[i];
            fMarkerSelect = new TGedMarkerSelect(hframe, fMarker, kIdMarkS);
            fMarkerSelect->Associate(this);
            fWidgets->Add(fMarkerSelect);
            fEntries->Add(fMarkerSelect);
//            fMarkerSelect->Resize(win_width / 4, fMarkerSelect->GetDefaultHeight());
            hframe->AddFrame(fMarkerSelect, loc);

         } else if (l.BeginsWith("Fill_Select")) {
            fPattern = *(Style_t*)fValPointers[i];
            fPatternSelect = new TGedPatternSelect(hframe, fPattern, kIdFillS);
            fPatternSelect->Associate(this);
            fWidgets->Add(fPatternSelect);
            fEntries->Add(fPatternSelect);
//            fPatternSelect->Resize(win_width / 4, fPatternSelect->GetDefaultHeight());
            hframe->AddFrame(fPatternSelect, loc);

         } else if (l.BeginsWith("AlignSelect")) {
            fAlign = *(Short_t*)fValPointers[i];
            fAlignSelect = new TGedAlignSelect(hframe, fAlign, kIdAlignS);
            fAlignSelect->Associate(this);
            fWidgets->Add(fAlignSelect);
            fEntries->Add(fAlignSelect);
//            fAlignSelect->Resize(win_width / 4, fAlignSelect->GetDefaultHeight());
            hframe->AddFrame(fAlignSelect, loc);

         } else if (l.BeginsWith("CfontSelect")) {
            fFont = *(Font_t*)fValPointers[i];
            fFontComboBox = new TGFontTypeComboBox(hframe, kIdFontS);
            fFontComboBox->Resize(fFontComboBox->GetDefaultWidth(), 20);
            fFontComboBox->Select(fFont);
            fWidgets->Add(fFontComboBox);
            fEntries->Add(fFontComboBox);
            fFontComboBox->Associate(this);
            hframe->AddFrame(fFontComboBox, lo1);

         } else if (l.BeginsWith("LineSSelect")) {
            fLineStyle = *(Style_t*)fValPointers[i];
            fLineStyleComboBox = new TGLineStyleComboBox(hframe, kIdLineS);
//            fLineStyleComboBox->Resize(fLineStyleComboBox->GetDefaultWidth(), 20);
            fLineStyleComboBox->Select(fLineStyle);
            fWidgets->Add(fLineStyleComboBox);
            fEntries->Add(fLineStyleComboBox);
            fLineStyleComboBox->Associate(this);
            fLineStyleComboBox->Resize(win_width / 4, 20);
            hframe->AddFrame(fLineStyleComboBox, loc);

         } else if (l.BeginsWith("ArrowSelect")) {
            fArrowShape = *(Int_t*)fValPointers[i];
            fArrowComboBox = new TGComboBox(hframe, kIdArrowS);
            fArrowComboBox->AddEntry(" ----|>",1);
            fArrowComboBox->AddEntry(" <|----",2);
            fArrowComboBox->AddEntry(" ----->",3);
            fArrowComboBox->AddEntry(" <-----",4);
            fArrowComboBox->AddEntry(" --->--",5);
            fArrowComboBox->AddEntry(" ---<--",6);
            fArrowComboBox->AddEntry(" --|>--",7);
            fArrowComboBox->AddEntry(" --<|--",8);
            fArrowComboBox->AddEntry(" <--->", 9);
            fArrowComboBox->AddEntry(" <|--|>",10);
//            fArrowComboBox->Resize(fArrowComboBox->GetDefaultWidth(), 20);
//            (fArrowComboBox->GetListBox())->Resize(, 136);
            fArrowComboBox->Select(fArrowShape);
            fWidgets->Add(fArrowComboBox);
            fEntries->Add(fArrowComboBox);
            fArrowComboBox->Associate(this);
            fArrowComboBox->Resize(win_width / 5, 20);
            hframe->AddFrame(fArrowComboBox, l3);

         } else if (l.BeginsWith("Float_Value")) {
//               scol = Form("%f", *(Float_t*)fValPointers[i]);
            Double_t neval = *(Float_t*)fValPointers[i];
            tnentry = new TGNumberEntry(hframe,neval, 5, i, TGNumberFormat::kNESReal); 
//            tnentry->Resize(win_width/2, tnentry->GetDefaultHeight());
            hframe->AddFrame(tnentry, l2);
			   fWidgets->Add(tnentry);
            tnentry->Associate(this);
            fEntries->Add(tnentry);
         } else if (l.BeginsWith("DoubleValue")) {
//               scol = Form("%g", *(Double_t*)fValPointers[i]);
            Double_t neval = *(Double_t*)fValPointers[i];
            tnentry = new TGNumberEntry(hframe,neval, 5,  i, TGNumberFormat::kNESReal); 
//            tnentry->Resize(win_width/2, tnentry->GetDefaultHeight());
            hframe->AddFrame(tnentry, l2);
			   fWidgets->Add(tnentry);
            tnentry->Associate(this);
            fEntries->Add(tnentry);

         } else if (l.BeginsWith("PlainIntVal")) {
//               scol = Form("%d", *(Int_t*)fValPointers[i]);
            Double_t neval = *(Int_t*)fValPointers[i];
            tnentry = new TGNumberEntry(hframe,neval, 5, i, TGNumberFormat::kNESInteger ); 
//            tnentry->Resize(win_width/2, tnentry->GetDefaultHeight());
            hframe->AddFrame(tnentry, l2);
			   fWidgets->Add(tnentry);
            tnentry->Associate(this);
            fEntries->Add(tnentry);
         } else if (l.BeginsWith("PlainShtVal")) {
//               scol = Form("%d", *(Short_t*)fValPointers[i]);
            Double_t neval = *(Short_t*)fValPointers[i];
            tnentry = new TGNumberEntry(hframe,neval, 5, i, TGNumberFormat::kNESInteger ); 
//            tnentry->Resize(win_width/2, tnentry->GetDefaultHeight());
            hframe->AddFrame(tnentry, l2);
			   fWidgets->Add(tnentry);
            tnentry->Associate(this);
            fEntries->Add(tnentry);

         } else if (l.BeginsWith("StringValue")) {
            TString scol;
            scol = *(TString*)fValPointers[i];
            tentry = new TGTextEntry(hframe, tbuf = new TGTextBuffer(100), kIdText);
            tentry->GetBuffer()->AddText(0, (const char *)scol);
//            tentry->Resize(win_width/2, tentry->GetDefaultHeight());
            hframe->AddFrame(tentry, l3);
			   fWidgets->Add(tentry);
            tentry->Associate(this);
            fEntries->Add(tentry);
         } else if (l.BeginsWith("FileRequest")) {
            TString scol;
            scol = *(TString*)fValPointers[i];
            tentry = new TGTextEntry(hframe, tbuf = new TGTextBuffer(100), kIdText);
            tentry->GetBuffer()->AddText(0, (const char *)scol);
//            tentry->Resize(win_width/2, tentry->GetDefaultHeight());
            hframe->AddFrame(tentry, l3);
			   fWidgets->Add(tentry);
            tentry->Associate(this);
            fEntries->Add(tentry);
            TGPictureButton * tb = new TGPictureButton(hframe, 
                         fClient->GetPicture("arrow_down.xpm"), i + 100 * kIdFileDialog);
            tb->Resize(20, 20);
            tb->SetToolTipText("Browse");

            hframe->AddFrame(tb, lor);
			   fWidgets->Add(tb);
            tb->Associate(this);

         } else if (l.BeginsWith("FileContReq")) {
            TString scol;
            TString fname("none");
            fClassName = "TF1";
            TString ename("none");
 
            scol = *(TString*)fValPointers[i];
            TObjArray * oa = scol.Tokenize("|");
            Int_t nent = oa->GetEntries();
            if (nent > 0) fname =((TObjString*)oa->At(0))->String();
            if (nent > 1) fClassName =((TObjString*)oa->At(1))->String();
            if (nent > 2) ename =((TObjString*)oa->At(2))->String();
            tentry = new TGTextEntry(hframe, tbuf = new TGTextBuffer(100), kIdText);
            tentry->GetBuffer()->AddText(0, (const char *)fname);
//            tentry->Resize(win_width/2, tentry->GetDefaultHeight());
            hframe->AddFrame(tentry, l3);
			   fWidgets->Add(tentry);
            tentry->Associate(this);
            fEntries->Add(tentry);
            TGPictureButton * tb = new TGPictureButton(hframe, 
                         fClient->GetPicture("arrow_down.xpm"), i + 100 * kIdFileDialogCont);
            tb->Resize(20, 20);
            tb->SetToolTipText("Browse");

            hframe->AddFrame(tb, lor);
            this->AddFrame(hframe, lo1);
            hframe = NULL;
			   fWidgets->Add(tb);
            tb->Associate(this);
//
            fListBoxReq = new TGListBox(this, kIdListBoxReq);  
            fListBoxReq->AddEntry(ename.Data(), 0);
            fListBoxReq->Resize(win_width , 20);
            fListBoxReq->Layout();
            fListBoxReq->Associate(this);
            fWidgets->AddFirst(fListBoxReq);
            this->AddFrame(fListBoxReq, l2);
            fEntries->Add(fListBoxReq);
            UpdateRequestBox(tentry->GetBuffer()->GetString());
         }
         if (fFlags) {
            hframe1 = new TGCompositeFrame(hframe, win_width*1/6, 20, kFixedWidth);
            fWidgets->Add(hframe1);
            cbutton = new TGCheckButton(hframe1, new TGHotString(""), i + 300);
            fFlagButtons->Add(cbutton);
            if (fFlags->At(i) == 1) cbutton->SetState(kButtonDown);
            else                cbutton->SetState(kButtonUp);
			   fWidgets->Add(cbutton);
            hframe1->AddFrame(cbutton, lor);
            hframe->AddFrame(hframe1, lor);
         }
      }
   }
   if (hframe != NULL) {
      this->AddFrame(hframe, lo1);
   }
   TGTextButton *b;
// create Prompt label and textentry widget
    
   if (fText) {
      hframe = new TGCompositeFrame(this, win_width, 20, kHorizontalFrame);
      TGLabel *label = new TGLabel(hframe, Prompt);
      fWidgets->AddFirst(label);
      hframe->AddFrame(label, lo1);
      if (FileName != NULL) {
         b = new TGTextButton(hframe, "Clear history", kIdClearHist);
         fWidgets->AddFirst(b);
         b->Associate(this);
         hframe->AddFrame(b, lor);
      }
      this->AddFrame(hframe, l2);

      TGTextBuffer *tbuf = new TGTextBuffer(256);  // will be deleted by TGtextEntry
      tbuf->AddText(0, text->Data());

      fTE = new TGTextEntry(this, tbuf, kIdTextValue);
      fTE->Resize(win_width, fTE->GetDefaultHeight());
      fTE->Associate(this);
      fWidgets->AddFirst(fTE);

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
                  if (fText) *fText = fTE->GetBuffer()->GetString();
                  gClient->NeedRedraw(fTE);
               }
               id++;
            }
            selections.close();
 //           cout << "Entries: " << id << endl;
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
   if (!has_commands) {
      b = new TGTextButton(hf, "&Apply", kIdOk);
      fWidgets->AddFirst(b);
      b->Associate(this);
      hf->AddFrame(b, l3);
      height = b->GetDefaultHeight();
      width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;
      b = new TGTextButton(hf, "Cancel", kIdCancel);
   } else {
      b = new TGTextButton(hf, "Close Dialog", kIdCancel);
      if (fCallClose) b->Connect("Clicked()", cname, calling_class, "CloseDown()");
   } 
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
   TString wn("Get Input:");
   wn += Prompt;
   this->SetWindowName(wn.Data());

   // map all widgets and calculate size of dialog
   this->MapSubwindows();

   width  = this->GetDefaultWidth();
   height = this->GetDefaultHeight();
//   cout << "width " << width<< endl;
   this->Resize(win_width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

//   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
//         ((TGFrame *) fMyWindow)->GetWidth() - width >> 1,
//         ((TGFrame *) fMyWindow)->GetHeight() - height >> 1,
//                          ax, ay, wdum);
   gVirtualX->TranslateCoordinates(fMyWindow->GetId(), this->GetParent()->GetId(),
         0, ((TGFrame *) fMyWindow)->GetHeight(),
                          ax, ay, wdum);

   ax = ax - (int)( win_width >> 1);
   ay = ay - (int)( height) - 30;

   UInt_t ww, wh;
   Int_t  wwi,whi;
   Int_t screen_x, screen_y;
//  screen size in pixels
   gVirtualX->GetWindowSize(gClient->GetRoot()->GetId(),
              screen_x, screen_y, ww, wh);
   wwi = ww;
   whi = wh;
   Int_t wi = win_width;
   Int_t hi = height;
//  make sure its inside physical screen
   if      (ax < 0)    ax = 10;
   else if (ax + wi > wwi)  ax = wwi - wi - 5;
   if      (ay + hi/2 < 0) ay =  hi/2 + 5;
   else if (ay + hi/2 > whi) ay = whi - hi/2 -5;

   this->Move(ax, ay);
   this->SetWMPosition(ax, ay);
/*
   // make the message box non-resizable
   this->SetWMSize(width, height);
   this->SetWMSizeHints(width, height, width, height, 0, 0);

   this->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);
*/
   // popup dialog and wait till user replies
   this->MapWindow();
   this->ChangeBackground(brown);
   hf->ChangeBackground(brown);
   if (!has_commands)
      gClient->WaitFor(this);
};

Bool_t TGMrbValuesAndText::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events
//   cout << "TGMrbValuesAndText::ProcessMessage " 
//       << GET_MSG(msg) << " "
//        << GET_SUBMSG(msg) << " " << parm1 << " " << parm2 << endl;

   if (!fEmitClose) return kTRUE;

   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) { 
             case kCM_BUTTON:
                {
                if (parm1 == kIdOk) {
                   // here copy the string from text buffer to return variable
                   if (fText) *fText = fTE->GetBuffer()->GetString();
                   if (fFileName && fFileName.Length() > 0) this->SaveList();
                   StoreValues();
                   *fReturn = 0;
                   CloseWindow();
//                 break;

                } else if (parm1 == kIdClearHist) {
#if ROOTVERSION > 51000
                   if (fFileName && fFileName.Length() > 0) {
                      TString cmd(fFileName);
                      cmd.Prepend("rm ");
                      gSystem->Exec(cmd);
                      fListBox->RemoveAll();
                      gClient->NeedRedraw(fListBox);
 
                   }
#else
                    cout << "Not yet implemented, please do it handish" << endl;
#endif
                } else if (parm1 == kIdCancel) {
                    CloseWindow();
                    return kTRUE;
//                    break;
                } else if (parm1 == kIdHelp) {
                    TString temp(fHelpText);
                    Int_t nl = temp.CountChar('\n');
                    nl *= 15;
                    TRootHelpDialog * hd = new TRootHelpDialog(this, fPrompt, 450, nl+20);
                    hd->SetText(fHelpText);
                    hd->Popup();
                } else {
                   Int_t id = parm1 / 100;
                   if (id == kIdFileDialog || id == kIdFileDialogCont) {
                      Int_t entryNr = parm1%100;
                      TString fn;
                      TGFileInfo* fi = new TGFileInfo();
 //                     const char * filter[] = {"data files", "*", 0, 0};
                      fi->fFileTypes = filetypes;
   						 new  TGFileDialog(gClient->GetRoot(), this, kFDOpen, fi);
  						    if (fi->fFilename) { 
                         fn = fi->fFilename;
                         TString pwd(gSystem->Getenv("PWD"));
                         if (fn.BeginsWith(pwd.Data()))fn.Remove(0,pwd.Length()+1);
                         TGTextEntry *te = (TGTextEntry*)fEntries->At(entryNr);
                
                         te->SetText(fn.Data());
                         gClient->NeedRedraw(te);
                         gClient->NeedRedraw(this);
                      }
   						 delete fi;
                      if (id == kIdFileDialogCont) {
                         UpdateRequestBox(fn.Data());
                      }

                   }
                }
                break;
                }
             case kCM_RADIOBUTTON:
                {   
 //                  cout << "toggle TGRadioButtons " << parm1 << endl;
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
                }

             case kCM_LISTBOX:
                {
                switch (parm1) {
                   case kIdTextSelect:
                      TGTextLBEntry * tge = (TGTextLBEntry *)fListBox->GetEntry(parm2);
                      TString txt = tge->GetText()->GetString();
                      fTE->SetText(txt.Data());
                      gClient->NeedRedraw(fTE);
                      if (fText) *fText = txt.Data();
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
//               cout << "Tab " << parm1 << " " << fCompList<< endl;
//                if (parm1 != kIdText) break;
                if (fCompList) {
                   TString temp = fTE->GetBuffer()->GetString();
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
//                cout << "kTE_ENTERparm1 " << parm1 << " kIdText" << kIdText << endl;
                if (parm1 != kIdText) break;
                // here copy the string from text buffer to return variable
                if (fText) {
                   *fText = fTE->GetBuffer()->GetString();
                   if (fFileName.Length() > 0) this->SaveList();
                   *fReturn = 0;
                   CloseWindow();
                } else if (fValPointers != NULL) {
//                   StoreValues();
                }
//                delete this;
                break;
             default:
                if (fText && parm1 == kIdTextValue) { 
                   *fText = fTE->GetBuffer()->GetString();
//                    cout << "kC_TEXTENTRY: " << GET_SUBMSG(msg) << " " 
//                    << parm1 << " " << parm2 << endl;
                }
                break;
          }
          break;

       default:
//      	 if (fValPointers != NULL) 
//         	 StoreValues();
          break;
   }
//   cout << "fValPointers " << fValPointers  << endl;
//  only if a command is executed 
   if (  GET_MSG(msg) == kC_COMMAND 
      && GET_SUBMSG(msg) == kCM_BUTTON
      && parm1 / 100 == kIdCommand) {
//
      if (fText) *fText = fTE->GetBuffer()->GetString();
	   if (fFileName && fFileName.Length() > 0) this->SaveList();
      ReloadValues();
   } else if (fValPointers != NULL) {
      StoreValues();
   }
   return kTRUE;
}

//_____________________________________________________________________________

void TGMrbValuesAndText::UpdateRequestBox(const char *fname)
{
   if (gSystem->AccessPathName(fname)) {
      cout << "Cant find: " << fname << endl;
      return;
   }
   fListBoxReq->RemoveAll();
   TFile f(fname);
   TIter next(f.GetListOfKeys());
   TKey* key;
   TObject* obj;
   Int_t id = 0;
   while( (key = (TKey*)next()) ){
      obj = (TObject*)key->ReadObj(); 
//        cout << "AddEntry " << fClassName<< " " <<  obj->ClassName()<< endl;
        if (obj->InheritsFrom(fClassName)) {
         TString s(obj->GetName());
         s += " | ";
         s += obj->GetTitle();
         fListBoxReq->AddEntry(s.Data(), id);
//         cout << "AddEntry " << obj->GetName()<< endl;
         id++;
      }
   }      
   fListBoxReq->Resize(fListBoxReq->GetDefaultWidth(), TMath::Min(200, (id+1)*20));
   fListBoxReq->Layout();
   gClient->NeedRedraw(fListBoxReq);
   gClient->NeedRedraw(this);
}
//________________________________________________________________[C++ METHOD]
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
  
//   cout << "StoreValues()" << endl;
   
//   if (fFinis != 0) return;
   if (!fEntries) return;

   TIter nextent(fEntries);
   TIter next(fLabels);
   TObjString *objs;

   TObject * obj;
   TGCheckButton * cbutton;
   TGColorSelect * colsel;
   Int_t i=0;

   while((objs = (TObjString*)next())){
       obj=nextent();
//       cout << "fValue: " << objs->String() << endl;
       if (obj->InheritsFrom("TGColorSelect")){ 
          colsel = (TGColorSelect*)obj;
          Int_t col = colsel->GetColor();
          Color_t val =  TColor::GetColor((Int_t)col);
/*
//           check if it is an existing color index
          Int_t cind = 0;
          cout << i << " col "<< col << " val " << val << endl;
          TIter next(gROOT->GetListOfColors());
          while (TColor * c = (TColor *)next()) {
             cout << "c->GetNumber() " << c->GetNumber() << endl;
             if (c->GetNumber() == val) {
                cind = 1;
                break;
             }
         	 if (cind == 0) val = 1;
          }
*/
//             cout << "col  " << col <<" val "  << val << endl;
          *(Color_t*)fValPointers[i] = val;

       } else if (obj->InheritsFrom("TGedAlignSelect")) {
          Short_t align =((TGedAlignSelect*)obj)->GetAlignStyle(); 
          *(Short_t*)fValPointers[i] = align;

       } else if (obj->InheritsFrom("TGedMarkerSelect")) {
          fMarker = ((TGedMarkerSelect*)obj)->GetMarkerStyle(); 
          *(Style_t*)fValPointers[i] = fMarker;

       } else if (obj->InheritsFrom("TGedPatternSelect")) {
          Style_t sel = ((TGedPatternSelect*)obj)->GetPattern(); 
          *(Style_t*)fValPointers[i] = sel;

       } else if (obj->InheritsFrom("TGComboBox")) {
          Short_t sel = -1;
          if (((TGComboBox*)obj)->WidgetId() == kIdArrowS) {
             sel = ((TGComboBox*)obj)->GetSelected();

          } else if (((TGComboBox*)obj)->WidgetId() == kIdFontS) {
             sel = ((TGComboBox*)obj)->GetSelected();

          } else if (((TGComboBox*)obj)->WidgetId() == kIdLineS) {
             sel = ((TGComboBox*)obj)->GetSelected();
          }
          *(Short_t*)fValPointers[i] = sel;

       } else if (obj->InheritsFrom("TGCheckButton")) {
          cbutton = (TGCheckButton*)obj;
          Int_t state = 0;
          if (cbutton->GetState() == kButtonDown) state = 1;
          *(Int_t*)fValPointers[i] = state;

       } else if (obj->InheritsFrom("TGRadioButton")) {
          cbutton = (TGCheckButton*)obj;
          Int_t state = 0;
          if (cbutton->GetState() == kButtonDown) state = 1;
          *(Int_t*)fValPointers[i] = state;
          
       } else if (obj->InheritsFrom("TGNumberEntry")) {
         
          TGNumberEntry * tnentry = (TGNumberEntry*)obj;
          TString tmp(objs->String());
//           cout << "TGTextEntry " << tmp << endl;
          if (tmp.BeginsWith("DoubleValue")) {
              *(Double_t*)fValPointers[i] = tnentry->GetNumber();

          } else if (tmp.BeginsWith("Float_Value")) {
              *(Float_t*)fValPointers[i] = (Float_t)tnentry->GetNumber();

          } else if (tmp.BeginsWith("PlainIntVal")) {
             *(Int_t*)fValPointers[i] = tnentry->GetIntNumber();
          } else if (tmp.BeginsWith("PlainShtVal")) {
             *(Short_t*)fValPointers[i] = (Short_t)tnentry->GetIntNumber();
          }
       } else if (obj->InheritsFrom("TGTextEntry")) {
          TGTextEntry   * tentry  = (TGTextEntry*)obj;
          TString tmp(objs->String());
          if (tmp.BeginsWith("StringValue") || tmp.BeginsWith("FileRequest")
             || tmp.BeginsWith("FileContReq") ) {
             TString * sr = (TString*)fValPointers[i];
//             cout << tmp << " " << *sr << endl;
             if (tmp.BeginsWith("FileContReq")) {
                TString retstr(tentry->GetBuffer()->GetString());
                retstr += "|";
                retstr += fClassName.Data();
                retstr += "|";
                TGLBEntry * tle = fListBoxReq->GetSelectedEntry();
                if (tle == NULL) {
                   retstr += "noselection";
                } else {
                   TGTextLBEntry * tge = (TGTextLBEntry *)tle;
                   TString temp(tge->GetText()->GetString());
                   Int_t ip = temp.Index("|");
                   if (ip > 0) temp.Resize(ip);
                   retstr += temp.Data();
                   retstr = retstr.Strip(TString::kBoth);
                }
                *sr = retstr.Data();
             } else {
                *sr = tentry->GetBuffer()->GetString();
             }
          }
       } 

       if (fFlags) {
          cbutton = (TGCheckButton*)fFlagButtons->At(i);
          if (cbutton->GetState() == kButtonDown) (*fFlags)[i]= 1;
          else                                    (*fFlags)[i]= 0;
       }
       i++; 
   }
}
//_____________________________________________________________________________

void TGMrbValuesAndText::ReloadValues(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbValuesAndText::StoreValues()
// Purpose:        Reload values which might been modified 
//                 during execution of a command
// Arguments:  
//             
// Results:        
// Exceptions:     
// Description:    
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
  
   if (!fEntries) return;

   TIter nextent(fEntries);
   TIter next(fLabels);
   TObjString *objs;

   TObject * obj;
   Int_t i=0;

   while((objs = (TObjString*)next())){
       obj=nextent();
//       cout << "fValue: " << objs->String() << endl;
       if (obj->InheritsFrom("TGColorSelect")){
          TGColorSelect *colsel = (TGColorSelect*)obj;
          Int_t col = GetColorPixelByInd(*(Color_t*)fValPointers[i]);
          colsel->SetColor(col);

       } else if (obj->InheritsFrom("TGedAlignSelect")) {
          TGedAlignSelect * as = (TGedAlignSelect*)obj;
          as->SetAlignStyle(*(Short_t*)fValPointers[i]);

       } else if (obj->InheritsFrom("TGedMarkerSelect")) {
          TGedMarkerSelect * ms = (TGedMarkerSelect*)obj;
          ms->SetMarkerStyle(*(Short_t*)fValPointers[i]);

       } else if (obj->InheritsFrom("TGedPatternSelect")) {
          TGedPatternSelect * ms = (TGedPatternSelect*)obj;
          ms->SetPattern(*(Short_t*)fValPointers[i]);
/*
       } else if (obj->InheritsFrom("TGComboBox")) {
          Short_t sel = -1;
          if (((TGComboBox*)obj)->WidgetId() == kIdArrowS) {
             sel = ((TGComboBox*)obj)->GetSelected();

          } else if (((TGComboBox*)obj)->WidgetId() == kIdFontS) {
             sel = ((TGComboBox*)obj)->GetSelected();

          } else if (((TGComboBox*)obj)->WidgetId() == kIdLineS) {
             sel = ((TGComboBox*)obj)->GetSelected();
          }
          *(Short_t*)fValPointers[i] = sel;
*/
       } else if (obj->InheritsFrom("TGCheckButton")) {
          TGCheckButton *cbutton = (TGCheckButton*)obj;
          Int_t state = *(Int_t*)fValPointers[i];
          if (state != 0) cbutton->SetState(kButtonDown);
          else                    cbutton->SetState(kButtonUp);

       } else if (obj->InheritsFrom("TGRadioButton")) {
          TGRadioButton *cbutton = (TGRadioButton*)obj;
          Int_t state = *(Int_t*)fValPointers[i];
          if (state != 0) cbutton->SetState(kButtonDown);
          else                    cbutton->SetState(kButtonUp);
          
       } else if (obj->InheritsFrom("TGNumberEntry")) {
         
          TGNumberEntry * tnentry = (TGNumberEntry*)obj;
          TString tmp(objs->String());
//           cout << "TGTextEntry " << tmp << endl;
          if (tmp.BeginsWith("DoubleValue")) {
              tnentry->SetNumber(*(Double_t*)fValPointers[i]);
//              cout << "Double " <<*(Double_t*)fValPointers[i] << endl;
//              gClient->NeedRedraw(tnentry);
//              gClient->NeedRedraw(this);
          } else if (tmp.BeginsWith("Float_Value")) {
              *(Float_t*)fValPointers[i] = (Float_t)tnentry->GetNumber();
              Double_t va = *(Float_t*)fValPointers[i];
              tnentry->SetNumber(va);

          } else if (tmp.BeginsWith("PlainIntVal")) {
             Int_t va = *(Int_t*)fValPointers[i];
             tnentry->SetIntNumber(va);

          } else if (tmp.BeginsWith("PlainShtVal")) {
             Int_t va = *(Short_t*)fValPointers[i];
             tnentry->SetIntNumber(va);
          }
       } else if (obj->InheritsFrom("TGTextEntry")) {
          TGTextEntry   * tentry  = (TGTextEntry*)obj;
          TString tmp(objs->String());
          if (tmp.BeginsWith("StringValue") || tmp.BeginsWith("FileRequest")) {
             TString * sr = (TString*)fValPointers[i];
             tentry->SetText(sr->Data());
          }
       } 

//       if (fFlags) {
//          cbutton = (TGCheckButton*)fFlagButtons->At(i);
//          if (cbutton->GetState() == kButtonDown) (*fFlags)[i]= 1;
//          else                                    (*fFlags)[i]= 0;
//       }
       i++; 
   }
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::SaveList()
{
//   cout << "enter SaveList() " << endl;
   if (fListBox == NULL) {
      fListBox = new TGListBox(this, kIdTextSelect);           
      fListBox->AddEntry(fTE->GetBuffer()->GetString(), 0);
      fWidgets->AddFirst(fListBox);
      gClient->NeedRedraw(this);
   }
   if (!(gSystem->AccessPathName(fFileName.Data()))) {
      TString bak(fFileName);
      bak += ".bak";
      gSystem->Rename(fFileName.Data(), bak.Data());
   }
   ofstream outfile(fFileName.Data());
   TString sel(fTE->GetBuffer()->GetString());
   Int_t ne = fListBox->GetNumberOfEntries();
//   cout << "SaveList() Selected: |" << sel <<"|" << endl;
   outfile << sel << endl;
   Bool_t in = kFALSE;
   if (ne > 0) {
      for (Int_t i = 0; i < ne; i++) {
         TGLBEntry * tle = fListBox->GetEntry(i);
         if (tle == NULL) continue;
         TGTextLBEntry * tge = (TGTextLBEntry *)tle;
         TString text = tge->GetText()->GetString();

 //        cout << "Ent: |" <<  text << "|"  << endl;
         if (text != sel) {
            outfile <<text.Data() << endl;
         } else {
            in = kTRUE;
         }
      }
   }
   if (!in) {
      cout << "Put: " << sel.Data() << endl;
      fListBox->AddEntry(sel.Data(), ne);
      Int_t wid = 8 * fTE->GetBuffer()->GetTextLength();
      fListBox->Resize(TMath::Max(wid,320), TMath::Min(200, (ne+1)*20));
      fListBox->Layout();
      this->Resize(this->GetDefaultWidth(),this->GetDefaultHeight());
      gClient->NeedRedraw(fListBox);
      gClient->NeedRedraw(this);
   }
   outfile.close();
}

//_______________________________________________________________________________________

TGMrbValuesAndText::~TGMrbValuesAndText()
{
// Cleanup dialog.
//      cout << "enter dtor: TGMrbValuesAndText fFlagButtons " << fFlagButtons<< endl;
      if (fFlagButtons) delete fFlagButtons;
      fWidgets->Delete();
      delete fWidgets;
//      cout << "exit dtor: TGMrbValuesAndText " << endl;
   }
//_______________________________________________________________________________________

void TGMrbValuesAndText::CloseWindowExt()
{
   fEmitClose = kFALSE;
   CloseWindow();
}
//_______________________________________________________________________________________

void TGMrbValuesAndText::CloseWindow()
{
   cout << "TGMrbValuesAndText::CloseWindow() ";
   cout  << endl << flush;
   if (fEmitClose) Emit("CloseDown()");
   DeleteWindow();
}
//_______________________________________________________________________________________

Int_t TGMrbValuesAndText::GetColorPixelByInd(Int_t index)
{
   Int_t pixels = 0;
   TIter next(gROOT->GetListOfColors());
   while (TColor * c = (TColor *)next()) {
      if (c->GetNumber() == index) {
         pixels = c->GetPixel();;
      }
   }
   return pixels;
}
//_______________________________________________________________________________________

Bool_t GetStringExt(const char *Prompt, TString  *text , 
                        Int_t win_width, TGWindow *Win,  
                        const char *FileName, TList * Complist,
                        TList * rowlabs, void **val_pointers,
                        TArrayI * Flags, const char * Flagslabel,
                        const char *helptext, TObject * calling_class, 
                        const char * cname)
{ 
   Int_t ret = 0;
 
   new TGMrbValuesAndText(Prompt, text, &ret, win_width,  Win,  
       FileName, Complist, rowlabs, val_pointers,Flags, Flagslabel, helptext,
       calling_class, cname);
   if(ret == 0) return kTRUE;
   else         return kFALSE;
}
