#ifndef GROUPOFGOBJECTS
#define GROUPOFGOBJECTS

#include "TNamed.h"
#include "TList.h"

class GroupOfGObjects : public TNamed {
friend class HTCanvas;
protected:
   Double_t fXorig;
   Double_t fYorig;
   Double_t fXLowEdge;     // Bounding Box X Low Edge
   Double_t fYLowEdge;     // Bounding Box Y Low Edge
   Double_t fXUpEdge;      // Bounding Box X Upper Edge
   Double_t fYUpEdge;      // Bounding Box Y Upper Edge
   
   TList    fMembers;     // Members 
public:
   enum {
      kIsFree = BIT(14)
   };
   GroupOfGObjects(){};
   GroupOfGObjects(const char * name, const char * title, Double_t x, Double_t y);
   virtual ~GroupOfGObjects();
   void SetOrigin(Double_t x, Double_t y){fXorig = x; fYorig = y;}
   Double_t GetXorig(){return fXorig;}
   Double_t GetYorig(){return fYorig;}
   TList * GetMemberList() {return & fMembers;};
   void    AddMember(TObject * obj) {fMembers.Add(obj);};
   void    RemoveMember(TObject * obj) {fMembers.Remove(obj);};
   Bool_t  BindMember(TObject * obj);
   Bool_t  FreeMember(TObject * obj);
   Int_t   GetNMembers() {return fMembers.GetSize();};
   Int_t   AddMembersToList(TList * list, Double_t xoff = 0, Double_t yoff = 0,
                            Double_t scale = 1, Double_t angle = 0, Bool_t all = kTRUE);
   
ClassDef(GroupOfGObjects,0)
};
#endif
