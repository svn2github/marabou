#ifndef INC_HandleMenus
#define INC_HandleMenus

#include "TList.h"
#include "TCanvas.h"
#include "TGMenu.h"
#include "TGraph.h"
#include "TGStatusBar.h"
#include "TRootCanvas.h"
#include <iostream>

namespace std {} using namespace std;

class FitHist;
class HistPresent;
class HTCanvas;

class HandleMenus : public TGFrame {
private:
   HTCanvas       * fHCanvas;
   HistPresent    * fHistPresent; 
   FitHist        * fFitHist;
   TGraph         * fGraph;
//  from HTRootCanvas
   TRootCanvas        * fRootCanvas;
   TGMenuBar           *fRootsMenuBar;       // menubar
   TGPopupMenu         *fFileMenu;           // file menu
   TGPopupMenu         *fCutsMenu;           // edit menu
   TGPopupMenu         *fDisplayMenu;           // view menu
   TGPopupMenu         *fViewMenu;           // view menu
   TGPopupMenu         *fOptionMenu;         // option menu
   TGPopupMenu         *fAttrMenu;         // option menu
   TGPopupMenu         *fAttrMenuDef;         // option menu
   TGPopupMenu         *fFitMenu;            //
   TGPopupMenu         *fCascadeMenu1;        // classes menu
   TGPopupMenu         *fCascadeMenu2; 
   TGPopupMenu         *fEditMenu;            //
   TGPopupMenu         *fHelpMenu;           // help menu
   TGLayoutHints       *fMenuBarLayout;      // menubar layout hints
   TGLayoutHints       *fMenuBarItemLayout;  // layout hints for menu in menubar
   TGLayoutHints       *fMenuBarHelpLayout;  // layout hint for help menu in menubar
   TGLayoutHints       *fCanvasLayout;       // layout for canvas widget
   TGStatusBar         *fStatusBar;          // statusbar widget
   TGLayoutHints       *fStatusBarLayout;    // layout hints for statusbar
public:
   HandleMenus(HTCanvas * c, HistPresent * hpr, FitHist * fh, TGraph * graph = 0);
   ~HandleMenus();
   Bool_t   ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void BuildMenus();
   void SetLog(Int_t state);

   ClassDef(HandleMenus,0)  
}; 
 
#endif
