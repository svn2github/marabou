#ifndef INSERTTEXTDIALOG
#define INSERTTEXTDIALOG
#include "TObject.h"
#include "TString.h"
//_____________________________________________________________________________________


class InsertTextDialog : public TObject {

private:
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
   InsertTextDialog(Bool_t from_file);
   virtual ~InsertTextDialog();
   void InsertTextExecute();
   void SaveDefaults();
   void RestoreDefaults();
   void Show_Head_of_File();
   void CloseDown();

ClassDef(InsertTextDialog,0)
};
#endif
