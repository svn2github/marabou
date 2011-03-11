#ifndef SETHISTOPTHISTDIALOG
#define SETHISTOPTHISTDIALOG
#include "TH1.h"
#include "TGWindow.h"
#include "TStyle.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


class SetHistOptDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
	TCollection  *fHistList;
   TGWindow     *fWindow;
	TH1          *fHist;
   Int_t        fPdivX;
	Int_t        fSdivX;
	Int_t        fTdivX;
	Int_t        fPdivY;
	Int_t        fSdivY;
	Int_t        fTdivY;
	Int_t        fPdivZ;
	Int_t        fSdivZ;
	Int_t        fTdivZ;
	Int_t        fOptimizedivX;
	Int_t        fOptimizedivY;
	Int_t        fOptimizedivZ;
   Int_t        fNdivisionsX;
   Int_t        fNdivisionsY;
   Int_t        fNdivisionsZ;
   Color_t      fAxisColor;
   Color_t      fLabelColor;
   Style_t      fLabelFont;
   Float_t      fLabelOffsetX;
   Float_t      fLabelOffsetY;
   Float_t      fLabelOffsetZ;
   Float_t      fLabelSize;
   Int_t        fLabelMaxDigits;
   Float_t      fTickLength;
   TString      fTickSideX;
   TString      fTickSideY;
   TString      fTickSideZ;
   Float_t      fTitleOffsetX;
   Float_t      fTitleOffsetY;
   Float_t      fTitleOffsetZ;
   Float_t      fTitleSize;
   Color_t      fTitleColorA;
   Style_t      fTitleFontA;

   Color_t      fTitleLineColor;
	Color_t      fTitleFillColor;
	Color_t      fTitleTextColor;
   Width_t      fTitleBorderSize;
   Style_t      fTitleFont;
   Float_t      fTitleFontSize;
   Style_t      fTitleStyle;
   Float_t      fTitleX;
   Float_t      fTitleY;
   Float_t      fTitleW;
   Float_t      fTitleH;
   Short_t      fTitleAlign;        //Text alignment
   Int_t        fTitleCmd1;
   Int_t        fTitleCmd2;
   Color_t      fStatLineColor;
	Color_t      fStatFillColor;
	Color_t      fStatTextColor;
   Width_t      fStatBorderSize;
   Style_t      fStatFont;
   TString      fStatFormat;
   Float_t      fStatFontSize;
   Style_t      fStatStyle;
   Float_t      fStatX;
   Float_t      fStatY;
   Float_t      fStatW;
   Float_t      fStatH;
   Int_t        fStatCmd1;
   Int_t        fStatCmd2;
   TString      fCustomStyleName;
   Int_t        fNewStyle;
	Int_t			 fDummy;
	Int_t        fOk;
public:
	static Int_t fTitleCenterX;
	static Int_t fTitleCenterY;
	static Int_t fTitleCenterZ;
	
public:
		
   SetHistOptDialog(TGWindow * win = NULL, TCollection * hlist = NULL);
	SetHistOptDialog(Int_t batch);
   ~SetHistOptDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas, Int_t bid = 0);
   void SetHistAtt(TCanvas *canvas, Int_t bid = 0);
   void SetHistAttPermLocal();
   void SetHistAttPerm(TStyle * style = NULL);
   void SetStatBoxAttr(TStyle * style = NULL);
   void SetTitleBoxAttr(TStyle * style = NULL);
   void SetCustomStyle();
   void SaveDefaults();
   void SetAllToDefault();
   void RestoreDefaults(Int_t resetall = 0);
	void GetValuesFromHist();
   static void SetDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(SetHistOptDialog,0)
};
#endif
