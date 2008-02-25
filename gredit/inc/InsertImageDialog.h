#ifndef INSERTIMAGEDIALOG
#define INSERTIMAGEDIALOG
#include "TString.h"
#include "TList.h"
#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________

class TCanvas;
class TRootCanvas;
class TObject;

class InsertImageDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   TRootCanvas *fWindow;
   Int_t fNewPad;
   Int_t fFix_h;
   Int_t fFix_w;
   Int_t fFix_wh;
   Int_t fOffset_x;
   Int_t fOffset_y;
   TString fPname;

public:
   InsertImageDialog();
   virtual ~InsertImageDialog();
   void    ExecuteInsert();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(InsertImageDialog,0)
};
#endif
