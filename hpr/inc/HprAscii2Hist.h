#ifndef HPRASCII2HIST
#define HPRASCII2HIST
#include "TArrayD.h"
#include "TGWindow.h"
#include "TString.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class HistPresent;

class HprAscii2Hist : public TObject {

private:
   HistPresent *fHistPresent;
   TArrayD  fXval;
   TArrayD  fYval;
   TArrayD  fZval;
   TArrayD  fWval;

   TString  fCommand;
   TString  fCommandHead;
   TString  fReadCommand;
   TString  fHistFileName;
   TString  fHistName;
   TString  fHistTitle;
   Int_t    fNvalues;
   Int_t    fSpectrum;        // Channel Contents only, no X values
   Int_t    f1Dim;            // X values to be filled
   Int_t    f1DimWithWeight;  // X, Weight
   Int_t    f2Dim;            // X, Y values to be filled
   Int_t    f2DimWithWeight;  // X,Y,  Weight
   Int_t    f3Dim;            // X, Y, Z values to be filled
   Int_t    f3DimWithWeight;  // X,Y,Z,  Weight
   Int_t    fNbinsX; 
   Double_t fXlow;  
   Double_t fXup;   
   Int_t    fNbinsY; 
   Double_t fYlow;  
   Double_t fYup;   
   Int_t    fNbinsZ; 
   Double_t fZlow;  
   Double_t fZup;   
 
public:
   HprAscii2Hist(HistPresent * hpr = NULL, TGWindow * win = NULL);
   ~HprAscii2Hist();
   void Draw_The_Hist();
   void Read_Input();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();

ClassDef(HprAscii2Hist,0)
};
#endif
