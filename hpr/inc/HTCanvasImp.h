
//+SEQ,CopyRight,T=NOINCLUDE.

#ifndef H_HTCanvasImp
#define H_HTCanvasImp

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// HTCanvasImp                                                           //
//                                                                      //
// ABC describing GUI independent main window (with menubar, scrollbars //
// and a drawing area).                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
//*KEEP,Rtypes.
#include "Rtypes.h"
//*KEND.
#endif

using namespace std;


class HTCanvas;


class HTCanvasImp {

protected:
   HTCanvas  *fCanvas;   //HTCanvas associated with this implementation

public:
   HTCanvasImp(HTCanvas *c=0) : fCanvas(c) { }
   HTCanvasImp(HTCanvas *c, const char *name, UInt_t width, UInt_t height);
   HTCanvasImp(HTCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
   virtual ~HTCanvasImp() { }

   HTCanvas       *Canvas() const { return fCanvas; }
   virtual void   Close() { }
   virtual void   ForceUpdate() { }
   virtual void   GetWindowGeometry(Int_t &x, Int_t &y, UInt_t &w, UInt_t &h);
   virtual void   Iconify() { }
   virtual Int_t  InitWindow() { return 0; }
   virtual void   SetStatusText(Text_t *text = 0, Int_t partidx = 0);
   virtual void   SetWindowPosition(Int_t x, Int_t y);
   virtual void   SetWindowSize(UInt_t w, UInt_t h);
   virtual void   SetWindowTitle(const Text_t *newTitle);
   virtual void   SetCanvasSize(UInt_t w, UInt_t h);
   virtual void   ShowMenuBar(Bool_t show = kTRUE);
   virtual void   ShowStatusBar(Bool_t show = kTRUE);
   virtual void   Show() { }

   ClassDef(HTCanvasImp,0)  //ABC describing main window protocol
};

inline HTCanvasImp::HTCanvasImp(HTCanvas *, const char *, UInt_t, UInt_t) { }
inline HTCanvasImp::HTCanvasImp(HTCanvas *, const char *, Int_t, Int_t, UInt_t, UInt_t) { }
inline void HTCanvasImp::GetWindowGeometry(Int_t &x, Int_t &y, UInt_t &w, UInt_t &h)
               { x = y = 0; w = h = 0; }
inline void HTCanvasImp::SetStatusText(Text_t *, Int_t) { }
inline void HTCanvasImp::SetWindowPosition(Int_t, Int_t) { }
inline void HTCanvasImp::SetWindowSize(UInt_t, UInt_t) { }
inline void HTCanvasImp::SetWindowTitle(const Text_t *) { }
inline void HTCanvasImp::SetCanvasSize(UInt_t, UInt_t) { }
inline void HTCanvasImp::ShowMenuBar(Bool_t) { }
inline void HTCanvasImp::ShowStatusBar(Bool_t) { }

#endif
