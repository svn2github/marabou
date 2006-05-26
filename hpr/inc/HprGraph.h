#ifndef HPRGRAPH
#define HPRGRAPH
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class HistPresent;

class HprGraph : public TObject {

private:
   HistPresent *fHistPresent;
   TString fCommand;
   TString fCommandHead;
   TString fGraphFileName;
   TString fGraphName;
   TString fGraphXtitle;
   TString fGraphYtitle;
   Int_t   fGraphColSelect; 
   Int_t   fGraphColSel1; 
   Int_t   fGraphColSel2; 
   Int_t   fGraphColSel3; 
   Int_t   fGraphXsize; 
   Int_t   fGraphYsize;  
   Int_t   fGraphXdiv;   
   Int_t   fGraphYdiv;   
   Int_t   fGraph_Simple;
   Int_t   fGraph_Error;
   Int_t   fGraph_AsymError;
   Int_t   fGraphSerialNr;
   Int_t   fGraphSelPad;  
   Int_t   fGraphNewPad;  
   Int_t   fGraphDrawMark;  
   Int_t   fGraphDrawLine;  
   Size_t  fGraphMarkerSize;  
   Style_t fGraphMarkerStyle;  
   Color_t fGraphMarkerColor;  
   Style_t fGraphLineStyle;  
   Color_t fGraphLineColor;  
   Short_t fGraphLineWidth;  
   Style_t fGraphFillStyle;  
   Color_t fGraphFillColor;  
 
public:
   HprGraph(HistPresent * hpr = NULL, TGWindow * win = NULL);
   ~HprGraph();
   void Draw_The_Graph();
   void Show_Head_of_File();
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown();

ClassDef(HprGraph,0)
};
#endif
