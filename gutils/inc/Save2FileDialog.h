#ifndef SAVE2FILEDIALOG
#define SAVE2FILEDIALOG
#include "TString.h"
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
   Int_t   fKeepDialog;
   Int_t   fAsList;
   TGMrbValuesAndText * fWidget;

public:
   Save2FileDialog(TObject *obj, const char *lname = NULL);
   ~Save2FileDialog();
   void ExecuteSave();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(Save2FileDialog,0)
};
#endif
