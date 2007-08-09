#ifndef CURLYLINEWITHARROWDIALOG
#define CURLYLINEWITHARROWDIALOG
#include "TObject.h"
//_____________________________________________________________________________________


class CurlyLineWithArrowDialog : public TObject {

private:
   Double_t   fWaveLength;  // wavelength of sinusoid in percent of pad height
   Double_t   fAmplitude;   // amplitude of sinusoid in percent of pad height
   Color_t    fColor;   	 // Feynman line color 
   Width_t    fWidth;  	    // Feynman line width
   Style_t    fStyle; 
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
   void    CloseDown();

ClassDef(CurlyLineWithArrowDialog,0)
};
#endif
