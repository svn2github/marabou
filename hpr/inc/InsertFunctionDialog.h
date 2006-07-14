#ifndef INSERTFUNCTIONDIALOG
#define INSERTFUNCTIONDIALOG
#include "TObject.h"
#include "TString.h"
//_____________________________________________________________________________________


class InsertFunctionDialog : public TObject {

private:
   TString  *fTpointer;
   Double_t fPar[10];
   Int_t 	fNpar;
   TString  fName;
   TString  fXtitle;
   TString  fYtitle;
   Double_t fFrom;
   Double_t fTo;
   Color_t  fCol;
   Int_t    fPad_opacity;
   Int_t    fSame_pad;
   Int_t    fNew_pad;
   Int_t    fNew_canvas;

public:
   InsertFunctionDialog(Int_t npar);
   virtual ~InsertFunctionDialog();
   void InsertFunctionExecute();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();
   void IncrementIndex(TString * arg);

ClassDef(InsertFunctionDialog,0)
};
#endif
