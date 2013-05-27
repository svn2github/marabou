#ifndef SHIFTSCALE
#define SHIFTSCALE
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMenu.h"
#include "TString.h"
#include "FitOneDimDialog.h"
#include "Save2FileDialog.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class TGMrbValuesAndText;
class HTCanvas;
class ShiftScale : public TObject {

private:
   TGMrbValuesAndText *fDialog;
   void *fValp[100];
   TList *fRow_lab;
	Int_t fOk;
   HTCanvas *fCanvasOrig;
   HTCanvas *fCanvas;
//   FitOneDimDialog * fDialog;
 //  Save2FileDialog * fSave2FileDialog;
   TH1    *fHistOrig;
   TH1    *fHist;
   TString fCommand;
   TString fHistName;
   TString fHistTitle;
	TString fHistXtitle;
	TString fHistYtitle;
   Int_t   fNbinsX;
   Int_t   fNbinsY;
   Double_t fXaxisMin;
   Double_t fYaxisMin;
   Double_t fXaxisMax;
   Double_t fYaxisMax;
	Double_t fShiftX;
	Double_t fScaleX;
	Int_t    fRandomX;
	Double_t fShiftY;
	Double_t fScaleY;
	Int_t fRandomY;
	TRootCanvas *fRootCanvas;
   TGPopupMenu *fMenu;
	TGPopupMenu *fAttrMenu;
	
public:
   ShiftScale(TH1 * hist, HTCanvas * canvas);
   ~ShiftScale();
   void CloseDialog();
   void RecursiveRemove(TObject*);
   void Draw_The_Hist();
//   void BuildMenu();
//   void HandleMenu(Int_t id);
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t, Int_t, TObject*) {};

ClassDef(ShiftScale,0)
};
#endif
