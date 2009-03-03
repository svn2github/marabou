#ifndef HPRSTACK
#define HPRSTACK
#include <TVirtualPad.h>
#include <TGWindow.h>
#include <HTCanvas.h>
#include <TRootCanvas.h>
#include <TList.h>
#include <TGMenu.h>
#include <THStack.h>
#include <iostream>
//#include "TGMrbValuesAndText.h"
//_____________________________________________________________________________________


namespace std {} using namespace std;

class TGMrbValuesAndText;

class HprStack : public TNamed {

private:
   void *fValp[100];
   TList *fRow_lab;
   TList *fHList;
   Int_t fNhists;
   Int_t fNDrawn;
   Int_t fDim;
   THStack *fStack;
   Option_t *fOpt;
   TGMrbValuesAndText *fDialog;
   TCanvas    *fCanvas;
   TList       fHistList;
   TGPopupMenu *fMenu;
   TGPopupMenu *fOptionMenu;
   TRootCanvas *fRootCanvas;
   Int_t       fWindowXWidth;
   Int_t       fWindowYWidth;
   Double_t    fLegendX1;
   Double_t    fLegendX2;
   Double_t    fLegendY1;
   Double_t    fLegendY2;
   TString fDrawOpt;
   Int_t   *fFill_1Dim;
   Color_t *fFillColor;
   Style_t *fFillStyle;
   Color_t *fLineColor;
   Style_t *fLineStyle;
   Width_t *fLineWidth;
   Color_t *fMarkerColor;
   Style_t *fMarkerStyle;
   Size_t *fMarkerSize;
   Int_t   fShowContour;
   Int_t   fShowErrors;
   Int_t   fShowMarkers;
   Float_t fEndErrorSize;
   Int_t   fErrorX;

public:
   HprStack(TList * hlist, Option_t * opt = 0);
   HprStack(){};
   ~HprStack();
   void   BuildCanvas();
   void   RecursiveRemove(TObject *obj);
   void   BuildMenu();
   void   HandleMenu(Int_t id);
   void   SetOptions();
   void   SetAttributes();
   void   SaveDefaults();
   void   RestoreDefaults();
   void   CloseDown(Int_t wid);
   void   CRButtonPressed(Int_t wid = 0, Int_t bid = 0, TObject* obj = 0);

ClassDef(HprStack,0)
};
#endif
