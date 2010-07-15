#ifndef ASCII2GRAPHDIALOG
#define ASCII2GRAPHDIALOG
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class Ascii2GraphDialog : public TObject {

private:
   Int_t   fWinx;
   Int_t   fWiny;
   TString fCommand;
   TString fCommandHead;
   TString fCommandTail;
   TString fGraphFileName;
   TString fGraphName;
   TString fGraphXtitle;
   TString fGraphYtitle;
   Double_t fXaxisMin;
   Double_t fYaxisMin;
   Double_t fXaxisMax;
   Double_t fYaxisMax;
   Int_t   fGraphColSelect;
   Int_t   fGraphColSel1;
   Int_t   fGraphColSel2;
   Int_t   fGraphColSel3;
   Int_t   fGraphXsize;
   Int_t   fGraphYsize;
   Int_t   fGraphXdiv;
   Int_t   fGraphYdiv;
   Int_t   fEmptyPad;
   Int_t   fGraph_Simple;
   Int_t   fGraph_Error;
   Int_t   fGraph_AsymError;
   Int_t   fGraphSerialNr;
   Int_t   fGraphSelPad;
   Int_t   fGraphNewPad;
	Int_t   fGraphShowTitle;
	Int_t   fGraphPolyMarker;
   Int_t   fGraphSimpleLine;
	Int_t   fGraphSmoothLine;
	Int_t   fGraphBarChart;
	Int_t   fGraphFill;
	Size_t  fGraphMarkerSize;
   Style_t fGraphMarkerStyle;
   Color_t fGraphMarkerColor;
   Style_t fGraphLineStyle;
   Color_t fGraphLineColor;
   Short_t fGraphLineWidth;
   Style_t fGraphFillStyle;
   Color_t fGraphFillColor;
	Int_t   fGraphLogX;
	Int_t   fGraphLogY;
	Int_t   fGraphLogZ;
	
public:
   Ascii2GraphDialog(TGWindow * win = NULL, Int_t winx = 100, Int_t winy = 100);
   ~Ascii2GraphDialog();
   void Draw_The_Graph();
   void Show_Head_of_File();
   void Show_Tail_of_File();
   Int_t FindGraphs(TVirtualPad * ca, TList * logr, TList * pads);
   void SaveDefaults();
   void RestoreDefaults();
   void CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};
ClassDef(Ascii2GraphDialog,0)
};
#endif
