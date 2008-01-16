#ifndef INSERTPADDIALOG
#define INSERTPADDIALOG
#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
#include "TObject.h"
//_____________________________________________________________________________________


class InsertPadDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   Double_t   fX1;         //
   Double_t   fY1;         //
   Double_t   fDx;         //
   Double_t   fDy;         //
   Color_t    fFillColor;  // fill color
   Style_t    fFillStyle;  // fill style
   Short_t    fBorderSize; // Size of Shadow
   Short_t    fBorderMode; //
public:
   InsertPadDialog();
   virtual ~InsertPadDialog();
   void    ExecuteInsert();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(InsertPadDialog,0)
};
#endif
