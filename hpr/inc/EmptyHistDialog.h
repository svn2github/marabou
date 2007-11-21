#ifndef EMPTYHISTDIALOG
#define EMPTYHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMenu.h"
#include "TString.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class EmptyHistDialog : public TObject {

private:
   TCanvas *fCanvas;
   TH1D    *fHist;
   Int_t   fWinx;
   Int_t   fWiny;
   TString fCommand;
   TString fHistName;
   TString fHistXtitle;
   TString fHistYtitle;
   Double_t fXaxisMin;
   Double_t fYaxisMin;
   Double_t fXaxisMax;
   Double_t fYaxisMax;
   Int_t   fNbins;
   Int_t   fHistXsize;
   Int_t   fHistYsize;  
   Int_t   fHistXdiv;   
   Int_t   fHistYdiv;   
   Int_t   fHistSelPad;   
   Int_t   fHistNewPad;   
   TRootCanvas *fRootCanvas;
   TGPopupMenu *fMenu;
   
public:
   EmptyHistDialog(TGWindow * win = NULL, Int_t winx = 100, Int_t winy = 100);
   ~EmptyHistDialog();
   void RecursiveRemove(TObject*);
   void Draw_The_Hist();
   void BuildMenu();
   void HandleMenu(Int_t id);
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);

ClassDef(EmptyHistDialog,0)
};
#endif
