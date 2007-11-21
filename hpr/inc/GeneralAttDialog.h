#ifndef GENERALATTDIALOG
#define GENERALATTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class GeneralAttDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;

public:
   static Int_t fForceStyle;
   static Int_t fShowPSFile;
   static Int_t fUseRegexp;
   static Int_t fShowListsOnly;
   static Int_t fRememberLastSet;
   static Int_t fRememberZoom;
   static Int_t fUseAttributeMacro;
   static Int_t fMaxListEntries;

   GeneralAttDialog(TGWindow * win = NULL);
   ~GeneralAttDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(GeneralAttDialog,0)
};
#endif