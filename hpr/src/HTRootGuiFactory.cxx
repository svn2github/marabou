
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HTRootGuiFactory                                                     //
//                                                                      //
// This class is a factory for ROOT GUI components. It overrides        //
// the member functions of the ABS TGuiFactory                          //
// make a modified version for HitsPresent (Otto S.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "HTRootGuiFactory.h"
#include "HTRootCanvas.h"

ClassImp(HTRootGuiFactory)
HTRootGuiFactory::HTRootGuiFactory(
    const char *name, const char *title) :
    TRootGuiFactory(name, title){} ;
HTCanvasImp * HTRootGuiFactory::CreateCanvasImp(
   HTCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height)
{
   // Create a ROOT native GUI version of TCanvasImp

   return new HTRootCanvas(c, title, x, y, width, height);
};
