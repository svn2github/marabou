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
	Int_t        fGlobalStyleButton;

public:
	static Int_t fPrependFilenameName;
	static Int_t fPrependFilename;
   static Int_t fForceStyle;
   static Int_t fShowPSFile;
   static Int_t fSuppressWarnings;
   static Int_t fUseRegexp;
   static Int_t fShowListsOnly;
   static Int_t fRememberLastSet;
   static Int_t fRememberZoom;
   static Int_t fUseAttributeMacro;
   static Int_t fMaxListEntries;
   static Int_t fContentLowLimit;
   static Int_t fVertAdjustLimit;
   static Int_t fStackedReally;
   static Int_t fStackedNostack;
   static Int_t fStackedPads;
	static TString fGlobalStyle;

   GeneralAttDialog(TGWindow * win = NULL);
   ~GeneralAttDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   static
	void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(GeneralAttDialog,0)
};
#endif
