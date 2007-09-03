#ifndef INSERTTEXTBOXDIALOG
#define INSERTTEXTBOXDIALOG
#include "TObject.h"
//_____________________________________________________________________________________


class InsertTextBoxDialog : public TObject {

private:
   Double_t   fX1;         //
   Double_t   fY1;         // 
   Double_t   fDx;         //
   Double_t   fDy;         // 
   Color_t    fColor;      // line color 
   Width_t    fWidth;  	   // line width
   Style_t    fStyle;      // line style
   Color_t    fFillColor;  // fill color 
   Style_t    fFillStyle;  // fill style
   Double_t   fCornerRadius;
   Int_t      fRoundCorners;
   Int_t      fShowShadow;
   Int_t      fBorderSize;          // Size of Shadow
   Short_t    fShadowPosition;     // convention like in text align
   Int_t      fNoAlign;
   Int_t      fWeakAlign;
   Int_t      fStrongAlign;
public:
   InsertTextBoxDialog();
   virtual ~InsertTextBoxDialog();
   void    ExecuteInsert();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    CloseDown();

ClassDef(InsertTextBoxDialog,0)
};
#endif