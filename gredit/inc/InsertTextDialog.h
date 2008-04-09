#ifndef INSERTTEXTDIALOG
#define INSERTTEXTDIALOG
#include "TObject.h"
#include "TString.h"
#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
//_____________________________________________________________________________________


class InsertTextDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas        *fCanvas;
   TObject        *fCaller;
   TString        *fEditTextPointer;
   TString        fEditTextFileName;
   Int_t          fEditTextFromFile;
   Double_t       fEditTextX0;
   Double_t       fEditTextY0;
   Double_t       fEditTextDy;
   Size_t         fEditTextSize;
   Float_t        fEditTextAngle;
   Short_t        fEditTextAlign;
   Color_t        fEditTextColor;
   Font_t         fEditTextFont;
   Int_t          fEditTextPrec;
   Int_t          fEditTextMarkCompound;
   Int_t          fEditTextLatexFilter;
   Int_t          fEditTextSeqNr;
public:
   InsertTextDialog(Bool_t from_file = kFALSE, TObject *caller = NULL);
   virtual ~InsertTextDialog();
   void InsertTextExecute();
   TString        *GetTextPointer() { return fEditTextPointer; } ;
   Size_t GetEditTextSize() { return fEditTextSize; };
   Float_t GetEditTextAngle() { return fEditTextAngle; };
   Short_t GetEditTextAlign() { return fEditTextAlign; };
   Color_t GetEditTextColor() { return fEditTextColor; };
   Font_t GetEditTextFont() { return fEditTextFont; };
   Int_t GetEditTextPrec() { return fEditTextPrec; };
   Double_t GetEditTextX0() { return fEditTextX0; };
   Double_t GetEditTextY0() { return fEditTextY0; };

   void SaveDefaults();
   void RestoreDefaults();
   void Show_Head_of_File();
   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(InsertTextDialog,0)
};
#endif
