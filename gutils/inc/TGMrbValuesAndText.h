#ifndef HPR_VALUESANDTEXT
#define HPR_VALUESANDTEXT
#include <TGWindow.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include "TGedMarkerSelect.h"
#include "TGedPatternSelect.h"
#include "TOrdCollection.h"
#include "TArrayI.h"
//_________________________________________________________________________________________

Bool_t  GetStringExt(const char *prompt, TString * text, 
                             Int_t win_width, TGWindow *win = 0, 
                             const char * FileName = 0, TList * complist=0,
                             TList * rowlabs =0 , TList * values = 0,
                             TArrayI * Flags = 0, const char * Flagslabel = 0,
                             const char *helptext = 0);
//_________________________________________________________________________________________

class TGMrbValuesAndText : public TGTransientFrame {

private:
   const TGWindow   *fMyWindow;
   TGTextEntry      *fTE;        // text entry widget
   TList            *fWidgets;   // keep track of widgets to be deleted in dtor
   TString          *fText;      // address to store return string
   Int_t            *fReturn;    // address to store cancel = -1 / ok =0 
   TList            *fValues;
   TArrayI          *fFlags;
   TList            *fFlagButtons;
   TGListBox        *fListBox;
   TString          fFileName;
   const char       *fHelpText;
   TList            *fCompList;
   Int_t            fNrows;
   Int_t            fAlign;
   Int_t            fArrowShape;
   TGComboBox       *fArrowComboBox;
   Int_t            fFont;
   TGFontTypeComboBox *fFontComboBox;
   Style_t           fLineStyle;
   TGLineStyleComboBox *fLineStyleComboBox;
   TGPictureButton  *fAlignButton;
   TGedMarkerSelect *fMarkerSelect;
   TGedPatternSelect *fPatternSelect;
   Style_t            fMarker;
   Style_t            fPattern;
   TList				  *fEntries;

public:
   TGMrbValuesAndText(const char *prompt, TString * text, 
                             Int_t * ok, Int_t win_width, const TGWindow *win=0, 
                             const char * FileName = 0, TList * complist=0,
                             TList * rowlabs = 0, TList * values = 0,
                             TArrayI * Flags = 0, const char * Flagslabel = 0,
                             const char *helptext=0);
   virtual ~TGMrbValuesAndText(){
// Cleanup dialog.
      if (fFlagButtons) delete fFlagButtons;
      fWidgets->Delete();
      delete fWidgets;
   }

   virtual Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void StoreValues();
//   void DoMarkerStyle(Style_t marker) {fMarker = marker;};
   void SaveList();

ClassDef(TGMrbValuesAndText,0)		// [GraphUtils] A dialog box
};   
#endif
