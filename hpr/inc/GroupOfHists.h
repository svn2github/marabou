#ifndef GROUPOFHISTS
#define GROUPOFHISTS
#include <TVirtualPad.h>
#include <TGWindow.h>
#include <HTCanvas.h>
#include <TRootCanvas.h>
#include <TList.h>
#include <TGMenu.h>
#include <TTimer.h>
#include <iostream>
//_____________________________________________________________________________________


namespace std {} using namespace std;

class GroupOfHists;

class GoHTimer : public TTimer 
{
private:
   GroupOfHists *fGoH;
public:  
   GoHTimer(Long_t ms, Bool_t synch, GroupOfHists *goh);
   virtual ~GoHTimer(){};
   Bool_t Notify();

ClassDef(GoHTimer,0)
};
//________________________________________________________________________

class HistPresent;

class GroupOfHists : public TNamed {

private:
   HistPresent *fHistPresent;
   HTCanvas    *fCanvas;
   TList       fHistList;
   GoHTimer    *fTimer;
   TGPopupMenu *fMenu;
   TRootCanvas *fRootCanvas;   
   Int_t       fAutoUpdateDelay;
   Bool_t      fCommonRotate;
   Bool_t      fAnyFromSocket;
   Bool_t      fShowAllAsFirst;
  
public:
   GroupOfHists(TList * hlist, HistPresent * hpr, const Char_t *title = NULL);
   ~GroupOfHists();
   void RecursiveRemove(TObject *obj);
   void   ActivateTimer(Int_t delay);    // start timer, delay(milli seconds)
   void   UpdateHists();
   void   BuildMenu();
   void   HandleMenu(Int_t id);
   void   auto_exec();
   void   ShowAllAsSelected(TVirtualPad * pad, TCanvas * canvas, 
          Int_t mode = 0, TGWindow * win = NULL);
   void   RebinAll(TCanvas * canvas);
//   void   WarnBox(const char *message);
   TH1    *GetTheHist(TVirtualPad * pad);
ClassDef(GroupOfHists,0)
};
#endif
