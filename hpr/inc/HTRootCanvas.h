
//+SEQ,CopyRight,T=NOINCLUDE.

#ifndef H_TRootCanvas
#define H_TRootCanvas

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TRootCanvas                                                          //
//                                                                      //
// This class creates a main window with menubar, scrollbars and a      //
// drawing area. The widgets used are the new native ROOT GUI widgets.  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//*KEEP,TCanvasImp.
#include "HTCanvasImp.h"
//*KEND.
#ifndef ROOT_TGFrame
//*KEEP,TGFrame.
#include "TGFrame.h"
//*KEND.
#endif

using namespace std;


class HTCanvas;
class TGCanvas;
class TGMenuBar;
class TGPopupMenu;
class TGLayoutHints;
class TGStatusBar;


class HTRootCanvas : public TGMainFrame, public HTCanvasImp {

friend class HTRootContainer;

private:
   HTCanvas            *fHCanvas;            // FitHists Canvas
   TGCanvas            *fCanvasWindow;       // canvas widget
   HTRootContainer      *fCanvasContainer;    // container in canvas widget
   TGMenuBar           *fMenuBar;            // menubar
   TGPopupMenu         *fFileMenu;           // file menu
   TGPopupMenu         *fEditMenu;           // edit menu
   TGPopupMenu         *fViewMenu;           // view menu
   TGPopupMenu         *fOptionMenu;         // option menu
   TGPopupMenu         *fInspectMenu;        // inspect menu
   TGPopupMenu         *fClassesMenu;        // classes menu
   TGPopupMenu         *fHelpMenu;           // help menu
   TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
   TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
   TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
   TGLayoutHints       *fCanvasLayout;       // layout for canvas widget
   TGStatusBar         *fStatusBar;          // statusbar widget
   TGLayoutHints       *fStatusBarLayout;    // layout hints for statusbar
   //TGToolBar           *fToolBar;            // icon button toolbar

   Int_t                fCanvasID;   // index in fWindows array of TGX11
   Bool_t               fAutoFit;    // when true canvas container keeps same size as canvas
   UInt_t               fCwidth;     // width of canvas container
   UInt_t               fCheight;    // height of canvas container
   Int_t                fButton;     // currently pressed button

   void     CreateCanvas(const char *name);

   Bool_t   HandleContainerButton(Event_t *ev);
   Bool_t   HandleContainerDoubleClick(Event_t *ev);
   Bool_t   HandleContainerConfigure(Event_t *ev);
   Bool_t   HandleContainerKey(Event_t *ev);
   Bool_t   HandleContainerMotion(Event_t *ev);
   Bool_t   HandleContainerExpose(Event_t *ev);
   Bool_t   HandleContainerCrossing(Event_t *ev);

public:
//   HTRootCanvas(HTCanvas *c, const char *name, UInt_t width, UInt_t height);
   HTRootCanvas(HTCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height);
   virtual ~HTRootCanvas();

   void     ForceUpdate() { Layout(); }
   void     FitCanvas();
   void     GetWindowGeometry(Int_t &x, Int_t &y, UInt_t &w, UInt_t &h);
   UInt_t   GetCwidth() const;
   UInt_t   GetCheight() const;
   void     Iconify() { }
   Int_t    InitWindow();
   void     SetWindowPosition(Int_t x, Int_t y);
   void     SetWindowSize(UInt_t w, UInt_t h);
   void     SetWindowTitle(const Text_t *newTitle);
   void     SetCanvasSize(UInt_t w, UInt_t h);
   void     SetStatusText(Text_t *txt = 0, Int_t partidx = 0);
   void     ShowMenuBar(Bool_t show = kTRUE);
   void     ShowStatusBar(Bool_t show = kTRUE);
   void     Show() { MapRaised(); }

   // overridden from TGMainFrame
   void     Close();
   void     CloseWindow();
   Bool_t   ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void     SetLog(Int_t);
   ClassDef(HTRootCanvas,0)  //ROOT native GUI version of main window with menubar and drawing area
};

#endif
