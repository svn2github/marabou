#ifndef EMPTYHISTDIALOG
#define EMPTYHISTDIALOG
#include "TGWindow.h"
#include "TString.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class EmptyHistDialog : public TObject {

private:
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
   Int_t   fHistXsize;
   Int_t   fHistYsize;  
   Int_t   fHistXdiv;   
   Int_t   fHistYdiv;   
   Int_t   fHistSelPad;   
   Int_t   fHistNewPad;   

public:
   EmptyHistDialog(TGWindow * win = NULL, Int_t winx = 100, Int_t winy = 100);
   ~EmptyHistDialog();
   void Draw_The_Hist();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();

ClassDef(EmptyHistDialog,0)
};
#endif
