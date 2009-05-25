#ifndef HPR_VALUESANDTEXT
#define HPR_VALUESANDTEXT
#include <TGWindow.h>
#include <TCanvas.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include "TGedMarkerSelect.h"
#include "TGedPatternSelect.h"
#include <TObjString.h>
#include "TOrdCollection.h"
#include "TArrayI.h"
#include <iostream>

//________________________________________________________________________

class TGedAlignPopup : public TGedPopup {

protected:
   Style_t  fCurrentStyle;

public:
   TGedAlignPopup(const TGWindow *p, const TGWindow *m, Style_t alignStyle);
   virtual ~TGedAlignPopup();

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(TGedAlignPopup,0)  //align select popup
};
//_________________________________________________________________________________________

class TGedAlignSelect : public TGedSelect {

protected:
   Style_t          fAlignStyle;
   const TGPicture *fPicture;

   virtual void     DoRedraw();

public:
   TGedAlignSelect(const TGWindow *p, Style_t alignStyle, Int_t id);
   virtual ~TGedAlignSelect() { if(fPicture) gClient->FreePicture(fPicture);}

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void           SetAlignStyle(Style_t pattern);
   Style_t        GetAlignStyle() const { return fAlignStyle; }
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
void SavePrimitive(ostream &out, Option_t *);
#else
void SavePrimitive(ofstream &out, Option_t *);
#endif

   virtual TGDimension GetDefaultSize() const { return TGDimension(38, 21); }

   virtual void AlignSelected(Style_t align = 0)
                { //std::cout << "  Emit(\"AlignSelected(Style_t)\"" << std::endl;
                   Emit("AlignSelected(Style_t)", align ? align : GetAlignStyle()); }  // *SIGNAL*

   ClassDef(TGedAlignSelect,0)  // Align selection button
};

//_________________________________________________________________________________________

class TGMrbValuesAndText : public TGTransientFrame {

private:
   const TGWindow   *fMyWindow;
   TCanvas          *fCallingCanvas;
   TGTextEntry      *fTE;        // text entry widget
   TGTextEntry      *fFileNameEntry;     // for file content
//   TList            *fWidgets;   // keep track of widgets to be deleted in dtor
   TString          *fText;      // address to store return string
   Int_t            *fReturn;    // address to store cancel = -1 / ok =0
   TList            *fLabels;
   void            **fValPointers;
   TArrayI          *fFlags;
   TList            *fFlagButtons;
   TGListBox        *fListBox;
   TGListBox        *fListBoxReq;
   TGTextEntry      *fFileDialogContTextEntry;
   TString          fClassName;
   TString          fFileName;
   Int_t            fWidgetId;
   TGTextButton     *fCancelButton;
   const char       *fPrompt;
   const char       *fHelpText;
   TList            *fCompList;
   Int_t            fWindowWidth;

   Int_t            fNrows;
   Int_t            fAlign;
   Int_t            fArrowShape;
   TGComboBox       *fArrowComboBox;
   TGComboBox       *fComboBox;
   TString          *fComboSelect;
   Int_t            fFont;
   TGFontTypeComboBox *fFontComboBox;
   Style_t           fLineStyle;
   TGLineStyleComboBox *fLineStyleComboBox;
   TGPictureButton  *fAlignButton;
   TGedMarkerSelect *fMarkerSelect;
   TGedAlignSelect  *fAlignSelect;
   TGedPatternSelect *fPatternSelect;
   Style_t            fMarker;
   Style_t            fPattern;
   TList				  *fEntries;
   Int_t             fButtonId;
   Bool_t            fEmitClose;
   Bool_t            fCallClose;
   Int_t             fLastColorSelect;
//   Int_t             fFinis;
public:
   TGMrbValuesAndText(const char *prompt, TString * text,
                             Int_t * ok, Int_t win_width, const TGWindow *win=0,
                             const char * FileName = 0, TList * complist=0,
                             TList * rowlabs = 0, void **val_pointers = 0,
                             TArrayI * Flags = 0, const char * Flagslabel = 0,
                             const char *helptext=0, TObject * calling_class = 0,
                             const char * cname = 0, const Int_t id = 0);
   virtual ~TGMrbValuesAndText();

   virtual Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void UpdateRequestBox(const char *fname, Bool_t store = kTRUE);
   void StoreValues();
   void ReloadValues();
   void ReloadValue(TObject *obj, TObjString *objs, Int_t ipos);
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t, Int_t, TObject*);
   void CloseWindowExt();
   void CloseWindow();
   void SaveList();
   Int_t GetColorPixelByInd(Int_t index);
   void EnableButton(Int_t id) ;
   void DisableButton(Int_t id);
   void EnableCancelButton()    { fCancelButton->SetEnabled(kTRUE); };
   void DisableCancelButton()   { fCancelButton->SetEnabled(kFALSE); };

ClassDef(TGMrbValuesAndText,0)		// [GraphUtils] A dialog box
};
//_________________________________________________________________________________________

Bool_t  GetStringExt(const char *prompt, TString * text,
                             Int_t win_width, TGWindow *win = 0,
                             const char * FileName = 0, TList * complist=0,
                             TList * rowlabs =0 , void ** val_pointers=0,
                             TArrayI * Flags = 0, const char * Flagslabel = 0,
                             const char *helptext = 0, TObject * calling_class = 0,
                             const char * cname = 0);
#endif
