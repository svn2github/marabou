#ifndef GROUPOFGOBJECTS
#define GROUPOFGOBJECTS

#include "TNamed.h"
#include "TList.h"
#include "TPad.h"
#include "TCutG.h"

//___________________________________________________________________

class GroupOfGObjects : public TCutG {
friend class HTCanvas;
protected:
   Double_t fXorigin;
   Double_t fYorigin;
   Double_t fXLowEdge;      // Bounding Box X Low Edge
   Double_t fYLowEdge;      // Bounding Box Y Low Edge
   Double_t fXUpEdge;       // Bounding Box X Upper Edge
   Double_t fYUpEdge;       // Bounding Box Y Upper Edge
   Bool_t   fIsVisible;
   
   TList    fMembers;       // Members 
public:
   enum {
      kIsBound = BIT(21),
      kIsEnclosingCut = BIT(22)
   };
   GroupOfGObjects(){};

   GroupOfGObjects(const char * name, 
                   Double_t x, Double_t y,  TCutG* cut);
   virtual ~GroupOfGObjects();
   void    RecursiveRemove(TObject * obj);
   void    SetOrigin(Double_t x, Double_t y){fXorigin = x; fYorigin = y;}
   Double_t GetXorigin(){return fXorigin;}
   Double_t GetYorigin(){return fYorigin;}
   TList * GetMemberList() {return & fMembers;};
   void    SetEnclosingCut(TCutG *cut);
   void    AddMember(TObject * obj, Option_t *option);
   void    RemoveMember(TObject * obj) {fMembers.Remove(obj);};
   Bool_t  BindMember(TObject * obj);
   Bool_t  FreeMember(TObject * obj);
   Int_t   GetNMembers() {return fMembers.GetSize();};
   Int_t   AddMembersToList(TPad * pad, Double_t xoff = 0, Double_t yoff = 0,
                            Double_t scaleNDC = 1,Double_t scaleG = 1,
                            Double_t angle = 0, Int_t align = 11, Int_t draw_cut=0);
   void Transform(Double_t x, Double_t y, Double_t xoff,Double_t yoff,
                    Double_t scale, Double_t angle,Int_t  align, Double_t* xt, Double_t* yt);  
   void Print();                                                // *MENU*
   void Paint(Option_t* opt) {if (fIsVisible) TGraph::Paint(opt);};
   void    SetVisible(Bool_t vis) {fIsVisible = vis;};          // *MENU*
   Bool_t  GetVisible() {return fIsVisible;};
   void BindObjects() {BindReleaseObjects(kTRUE);};            // *MENU*
   void ReleaseObjects() {BindReleaseObjects(kFALSE);};        // *MENU*
   void DeleteObjects();                                       // *MENU*
   void BindReleaseObjects(Bool_t bind) ; 
        
ClassDef(GroupOfGObjects,2)
};
#endif
