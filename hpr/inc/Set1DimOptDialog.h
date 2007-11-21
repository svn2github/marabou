#ifndef SET1DIMOPTHISTDIALOG
#define SET1DIMOPTHISTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________

class Set1DimOptDialog : public TObject {

private:
   void *fValp[100];
   Int_t fFlags[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   TString fDrawOpt;
   Int_t   fFill1Dim;      
   Color_t fHistFillColor ;
   Style_t fHistFillStyle; 
   Color_t fHistLineColor; 
   Style_t fHistLineStyle;
   Width_t fHistLineWidth;
   Float_t fEndErrorSize;
   Int_t   fErrorX;      
   Color_t fMarkerColor;           //Marker color
   Style_t fMarkerStyle;           //Marker style
   Float_t fMarkerSize;           //Marker width
   Int_t   fLiveStat1Dim;  
   Int_t   fLiveGauss;     
   Int_t   fLiveBG;        
   Int_t   fDrawAxisAtTop; 
   Int_t   fShowContour;   
   Int_t   fShowErrors;    
   
public:

   Set1DimOptDialog(TGWindow * win = NULL);
   ~Set1DimOptDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   void SetHistAttNow(TCanvas *canvas);
   void SetHistAtt(TCanvas *canvas);
   void SetAtt(TH1* hist);
   void SetHistAttPermLocal();
   void SetHistAttPerm();
   static void SetDefaults();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
ClassDef(Set1DimOptDialog,0)
};
#endif
