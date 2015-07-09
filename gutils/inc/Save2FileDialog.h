#ifndef SAVE2FILEDIALOG
#define SAVE2FILEDIALOG
#include "TString.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class Save2FileDialog : public TObject {

private:
   TObject * fObject;
   TObject * fList;
   TString fCommand;
   TString fObjName;
   TString fFileName;
   TString fDir;
   Int_t   fHistInRange;
   Int_t   fKeepDialog;
   Int_t   fAsList;
   Int_t   fCanceled;
   TGMrbValuesAndText * fWidget;

public:
   Save2FileDialog(TObject *obj, const char *lname = NULL, TRootCanvas *win = NULL);
   ~Save2FileDialog();
   void ExecuteSave();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid = 0);
   void CloseDialog();
   void CRButtonPressed(Int_t, Int_t, TObject *) {};
   const char * GetObjName(){return fObjName.Data();};
	Int_t Canceled() { return fCanceled; };
ClassDef(Save2FileDialog,0)
};
#endif
