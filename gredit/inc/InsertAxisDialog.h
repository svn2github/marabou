#ifndef INSERTAxisDIALOG
#define INSERTTAMPLATEDIALOG
#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TObject.h"
//_____________________________________________________________________________________


class InsertAxisDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   TRootCanvas  *fWindow;
   Double_t fX0;
   Double_t fY0;
   Double_t fX1;
   Double_t fY1;
   Double_t fWmin;
   Double_t fWmax;
   Int_t    fNdiv;
   Int_t    fLogScale;
   Int_t    fUseTimeFormat;
   Int_t    fTimeZero;
   TString  fTformat;
public:
   InsertAxisDialog();
   virtual ~InsertAxisDialog();
   void    ExecuteInsert();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(InsertAxisDialog,0)
};
#endif
