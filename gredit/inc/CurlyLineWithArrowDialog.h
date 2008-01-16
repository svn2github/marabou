#ifndef CURLYLINEWITHARROWDIALOG
#define CURLYLINEWITHARROWDIALOG
#include "TObject.h"
#include "Gtypes.h"
#include "TGMrbValuesAndText.h"
#include "TCanvas.h"
//_____________________________________________________________________________________


class CurlyLineWithArrowDialog : public TObject {

private:
   void *fValp[100];
   TList *fRow_lab;
   TGMrbValuesAndText *fDialog;
   TCanvas      *fCanvas;
   Double_t   fWaveLength;  // wavelength of sinusoid in percent of pad height
   Double_t   fAmplitude;   // amplitude of sinusoid in percent of pad height
   Color_t    fColor;   	 // Feynman line color
   Width_t    fWidth;  	    // Feynman line width
   Style_t    fStyle;
   Int_t      fCurly;
   Float_t    fArrowAngle;   //
   Float_t    fArrowSize;   //
   Style_t    fArrowStyle;  //
   Int_t      fArrowAtStart;
   Int_t      fArrowAtEnd;
public:
   CurlyLineWithArrowDialog();
   virtual ~CurlyLineWithArrowDialog();
   void    ExecuteInsert();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    RecursiveRemove(TObject * obj);
   void    CloseDialog();
   void    CloseDown(Int_t wid);
   void    CRButtonPressed(Int_t wid, Int_t bid, TObject *obj) {};

ClassDef(CurlyLineWithArrowDialog,0)
};
#endif
