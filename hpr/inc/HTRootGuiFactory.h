
//+SEQ,CopyRight,T=NOINCLUDE.

#ifndef H_TRootGuiFactory
#define H_TRootGuiFactory

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRootGuiFactory                                                      //
//                                                                      //
// This class is a factory for ROOT GUI components. It overrides        //
// the member functions of the ABS TGuiFactory.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGuiFactory
//*KEEP,TGuiFactory.
#include "TRootGuiFactory.h"
//*KEND.
#endif

using namespace std;


class HTCanvasImp;
class HTCanvas;

class HTRootGuiFactory : public TRootGuiFactory {

public:
   HTRootGuiFactory(const char *name = "Root", const char *title = "ROOT GUI Factory");
   ~HTRootGuiFactory() { }
   HTCanvasImp *CreateCanvasImp(HTCanvas *c, const char *title, Int_t x, Int_t y, UInt_t width, UInt_t height);
   ClassDef(HTRootGuiFactory,0)  //Factory for ROOT GUI components
};

#endif
