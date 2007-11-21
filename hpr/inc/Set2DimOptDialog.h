#ifndef SET2DIMOPTHISTDIALOG
#define SET2DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

const Int_t kNdrawopt = 20;

class Set2DimOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   TString fDrawOpt;
   Int_t fOptRadio[kNdrawopt];
   char *fDrawOpt2DimArray[kNdrawopt];
   TString fDrawOpt2Dim;
   Int_t fHideFrontBox;
   Int_t fHideBackBox;
   Int_t fLiveStat2Dim;
   Double_t fLogScaleMin;
   Double_t fLogScaleMax;
   
public:

   Set2DimOptDialog(TGWindow * win = NULL);
   ~Set2DimOptDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas);
   void SetHistAtt(TCanvas *canvas);
   void SetHistAttPermLocal();
   void SetHistAttPerm();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(Set2DimOptDialog,0)
};
#endif
