#ifndef HPRELEMENT
#define HPRELEMENT
#include "Rtypes.h"
#include "TPad.h"
#include "TList.h"
#include <iostream>

class HTPad;
class HTCanvas;

class HprElement
{
private:
   Int_t fPlane;
   Int_t fVisibilityFlag;
   Int_t fMustAlign;
   TList fMemberships;
   HTPad * fHTPad;
   HTCanvas * fHTCanvas;
   TObject *  fDaughter;

public:
   HprElement(TObject * daughter = NULL);
   virtual ~HprElement();
   void  SetPlane(Int_t plane) { fPlane = plane; };         // *MENU*
   Int_t GetPlane() const      { return fPlane; };          // *MENU*
   void  SetMustAlign(Int_t al){ fMustAlign = al; };        // *MENU*
   Int_t GetMustAlign()        { return fMustAlign; };      // *MENU*
   void  SetVisibilityFlag(Int_t vis){ fVisibilityFlag = vis; };
   Int_t GetVisibilityFlag()   { return fVisibilityFlag; }; // *MENU*
   Int_t GetVisibility()       { return fVisibilityFlag; }; // *MENU*
   void  SetVisibility(Int_t vis);                          // *MENU*
   void  SetDaughter(TObject *d) { fDaughter = d;};
   TObject * GetDaughter() {return fDaughter;};
   static void MoveObject(TObject *obj, TList *from, TList *to);
   static void MoveAllObjects(TList *from, TList *to, Int_t plane, Int_t vis);
   static Int_t GetNofElementsInPlane(TList *list, Int_t plane);


 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *);
 #else
   void SavePrimitive(ofstream &, Option_t *);
 #endif
ClassDef(HprElement,2)
};
#endif
