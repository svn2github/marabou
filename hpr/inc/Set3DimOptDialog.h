#ifndef SET3DIMOPTHISTDIALOG
#define SET3DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class TButton;

class Set3DimOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TH1          *fHist;
	TButton      *fCmdButton;
   TString fDrawOpt;
   TString fDrawOptPrev;
   Int_t fOptRadio[4];
   const char *fDrawOpt3DimArray[4];
   TString fDrawOpt3Dim;
   Color_t fHistFillColor3Dim;
   Color_t fHistLineColor3Dim;
   Color_t fMarkerColor3Dim; 
	Color_t f3DimBackgroundColor;
   Style_t fMarkerStyle3Dim;  
   Size_t  fMarkerSize3Dim; 
	Int_t   fBidSCAT;
	Int_t   fBidBOX;
	Int_t   fBidGL;
	Int_t   fBidPolyM;
	Int_t   fBidView3D;
	Int_t   fBidARR;
	Int_t   fBidTEXT;
	Int_t   fBidFillColor;
	Int_t   fBidLineColor;
	Int_t   fBidMarkerColor;
	Int_t   fBidMarkerStyle;
	Int_t   fBidMarkerSize;
	Int_t   fBidApplyTransp;
	Int_t   fBidTranspBelow;
	Int_t   fBidTranspAbove;
	Int_t   fApplyTranspCut;
	Int_t   fRotDx;
	Int_t   fRotDy;
	Int_t   fRotSleep;
	Int_t   fRotCycles;
	Int_t    fBidPhi;
   Double_t fPhi3Dim;
	Int_t    fBidTheta;
   Double_t fTheta3Dim;

public:

   Set3DimOptDialog(TGWindow * win = NULL, TButton *b = NULL);
	Set3DimOptDialog(Int_t batch);
   ~Set3DimOptDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas);
   void SetHistAtt(TCanvas *canvas);
	void Rotate();
//   void SetHistAttPermLocal();
//   void SetHistAttPerm();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(Set3DimOptDialog,0)
};
#endif
