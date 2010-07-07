#ifndef GRAPHATTDIALOG
#define GRAPHATTDIALOG
#include "TGWindow.h"
#include "TRootCanvas.h"
#include "TGMrbValuesAndText.h"
#include <iostream>
//_____________________________________________________________________________________


class GraphAttDialog : public TObject {

private:

   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;

public:
   static TString fDrawOptGraph;
   static Int_t   fGraphSimpleLine;
   static Int_t   fGraphSmoothLine;
   static Int_t   fGraphFill;
   static Int_t   fGraphPolyMarker;
   static Int_t   fGraphBarChart;
   static Int_t   fGraphShowAxis;
   static Style_t fGraphFStyle;
   static Color_t fGraphFColor;
	static Style_t fGraphLStyle;
	static Width_t fGraphLWidth;
	static Color_t fGraphLColor;
	static Style_t fGraphMStyle;
   static Size_t  fGraphMSize;
   static Color_t fGraphMColor;

   GraphAttDialog(TGWindow * win = NULL);
   ~GraphAttDialog() {};
   void RecursiveRemove(TObject * obj);
   void CloseDialog();
   static void SetGraphAtt(TCanvas *ca, Int_t bid = 0);
   void SetAsDefault();
   void SaveDefaults();
   static void RestoreDefaults();
   void CloseDown(Int_t wid);
   void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);

ClassDef(GraphAttDialog,0)
};
#endif
