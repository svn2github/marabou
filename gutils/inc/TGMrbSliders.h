#ifndef __TGMrbSliders_h__
#define __TGMrbSliders_h__

#include "Rtypes.h"
#include "TSystem.h"
#include <TQObject.h>
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGSlider.h"
#include "TOrdCollection.h"
#include "TStopwatch.h"
#include "TColor.h"
class TGMrbSliders : public TGTransientFrame {

private:
   Int_t fNValues;
   Int_t * fVal;
   Int_t fIdentifier;
   const TGWindow * fMyWindow;
   TGTextEntry  ** fTePointers;
   TGTextBuffer ** fTbPointers;
   TGHSlider    ** fSlPointers;
   TGVerticalFrame *fLabelFrame, *fSliderFrame, *fValueFrame;   
   TGLayoutHints *fBly, *fBfly1, *fLO4; 
   TStopwatch * fStopwatch;
   TColor * fTeColor;

public:
   TGMrbSliders(const char *Title, Int_t * min, Int_t * max, Int_t * val,
                TOrdCollection * RowLabels,  const Int_t NValues,
                         const Int_t Identifier,
                         Int_t * Return, const TGWindow *Win = 0,
                         const char *HelpText = 0);
   virtual ~TGMrbSliders();

   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void SliderEvent(Int_t, Int_t, Int_t);
ClassDef(TGMrbSliders,0)		// [GraphUtils] A box with sliders
};
#endif
