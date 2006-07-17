#ifndef INSERTFUNCTIONDIALOG
#define INSERTFUNCTIONDIALOG
#include "TObject.h"
#include "TPad.h"
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
   Int_t    fPadOpacity;
   Int_t    fNew_canvas;

public:
   InsertFunctionDialog();
   virtual ~InsertFunctionDialog();
   void InsertFunctionExecute();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();
   void IncrementIndex(TString * arg);
   Int_t GetFunctionPad(TPad *ipad = NULL);

ClassDef(InsertFunctionDialog,0)
};
#endif
