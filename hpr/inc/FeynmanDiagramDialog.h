#ifndef FEYNMANDIAGRAMDIALOG
#define FEYNMANDIAGRAMDIALOG
#include "TObject.h"
//_____________________________________________________________________________________


class FeynmanDiagramDialog : public TObject {

private:
   Double_t   fWaveLength; // wavelength of sinusoid in percent of pad height
   Double_t   fAmplitude;  // amplitude of sinusoid in percent of pad height
   Color_t    fColor ;  	// Feynman line color 
   Width_t    fWidth ;  	// Feynman line width
   Int_t      fPhi1; 		// Phi1 angle for curly arcs
   Int_t      fPhi2; 		// Phi2 angle for curly arcs
public:
   FeynmanDiagramDialog();
   virtual ~FeynmanDiagramDialog();
   void    FeynmanArrow();
   void    FeynmanWavyLine();
   void    FeynmanWavyArc();
   void    FeynmanCurlyLine();
   void    FeynmanCurlyArc();
   void    FeynmanSolidLine();
   void    FeynmanDashedLine();
   void    SaveDefaults();
   void    RestoreDefaults();
   void    CloseDown();

ClassDef(FeynmanDiagramDialog,0)
};
#endif
