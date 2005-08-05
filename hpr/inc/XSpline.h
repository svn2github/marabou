#ifndef X_SPLINE
#define X_SPLINE
#include "TObject.h"
#include "TGraph.h"
#include "TArrayD.h"
#include "TMarker.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "Buttons.h"
#include "ControlGraph.h"

class ShapeFactor
{
// Shapefactor for use in a simply linked list
// only used internally
// no need to make persistent, since remade anyway

public:
   Float_t s;
   ShapeFactor* next;
   ShapeFactor (Float_t f, ShapeFactor* nf) {
      s = f;
      next = nf;
   };
   
   ~ShapeFactor() {};
   void SetNext(ShapeFactor * nf) { 
      next = nf;
   }
   ShapeFactor* GetNext() {return next;};
};
//__________________________________________________________________

class ControlPoint
{
// ControlPoint for use in a simply linked list
// only used internally
// no need to make persistent, since remade anyway

public:
   Double_t x, y;
   ControlPoint* next;
   ControlPoint (Double_t  xv, Double_t yv, ControlPoint* np) {
      x = xv;
      y = yv;
      next = np;
   };
   
   ~ControlPoint() {};
   void SetNext(ControlPoint * np) { 
      next = np;
   }
   ControlPoint* GetNext() {return next;};
};
//____________________________________________________________________________________

class XSpline : public TGraph
{
protected:
   ShapeFactor*  fShapeFactorList;     //! pointer to linked list
   ControlPoint* fControlPointList;    //! 
   Float_t       fPrec;
   Bool_t        fClosed;
   Int_t         fNofControlPoints;
   Int_t         fNpoints;           // number of used points
   Bool_t        fReady; 
   Bool_t        fNeedReCompute;     //
   TArrayD       fX;                 // result X
   TArrayD       fY;                 // result Y
   ControlGraph  fCPGraph;           // Controlpoints
   Int_t         fNP;                // number of parallel graphs
   Int_t         fMType;             // markertype
   Size_t        fMSize;             // markersize
   TObjArray     fPGraphs;           //! pointer list to parallel lines
   TArrayD       fPDists;
   Double_t      fFilledLength;      // draw like railway in maps
   Double_t      fEmptyLength;       // space in between
   TList         fDPolyLines;        // !
private:
   void point_adding(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, 
                                      ControlPoint *p2, ControlPoint *p3);
   Float_t step_computing(int k, ControlPoint *p0, ControlPoint *p1,
                               ControlPoint *p2, ControlPoint *p3, 
                               Double_t s1, Double_t s2, Float_t precision);
   void spline_segment_computing(Float_t step, Int_t k, 
                                 ControlPoint *p0, ControlPoint *p1, 
                                 ControlPoint *p2, ControlPoint *p3, 
                                 Double_t s1, Double_t s2);
   Int_t op_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision);
   Int_t cl_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision);
   Int_t add_point(Double_t x, Double_t y);
   Int_t add_closepoint();
   void too_many_points();
   void Delete_ShapeFactors();
   void Delete_ControlPoints();
   void SetGraph();
   void CopyControlPoints();

public:
   XSpline(Int_t npoints = 0, Double_t* x = 0, Double_t* y = 0);
   virtual ~XSpline();
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void RecursiveRemove(TObject * obj);
   virtual void EditControlGraph();    //  *MENU*
   void Print(Option_t * opt = " ") const{TGraph::Print();};    //  *MENU*
   void DrawControlPoints(Int_t marker = 20, Size_t size = 2); // *MENU* *ARGS={marker=>fMType, size=>fMSize}
   Int_t GetMType() {return fMType;};
   Size_t GetMSize() {return fMSize;};
   void SetMType(Int_t type) {fMType = type;};
   void SetMSize(Size_t size) {fMSize = size;};
   void SetControlPoints(Int_t npoints, Double_t* x, Double_t* y);
   void SetShapeFactors(Int_t npoints, Float_t* sf);
//   Int_t GetShapeFactors(Double_t* x) {;
//   Int_t GetControlPoints(Double_t* x, Double_t* y);
   Int_t GetNofControlPoints() {return fNofControlPoints;};
   Int_t GetResult(Double_t*& x, Double_t*& y);
   Int_t ComputeSpline(Float_t prec = -1, Bool_t open = kTRUE);

   TGraph* AddParallelGraphExt(TGraph* ng = NULL, Double_t dist = 2, Color_t color=0, 
                            Width_t width=0, Style_t style=0);
   TGraph* AddParallelGraph(Double_t dist = 2, Color_t color=0, 
                            Width_t width=0, Style_t style=0);   // *MENU*
   TObjArray* GetParallelGraphs() {return &fPGraphs;};
   void  DrawParallelGraphs();
   Double_t* GetParallelDists() {return fPDists.GetArray();};
   ControlGraph* GetControlGraph() {return &fCPGraph;};

   void RemovePolyLines();
   static Bool_t Parallel_Graph(TGraph* ograph, TGraph* pgraph, 
                                Double_t dist, Bool_t closed);
   static Double_t Length(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   static Double_t PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   static void Nextpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   static void Endpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   static void Midpoint(Double_t phi1, Double_t phi2, Double_t x, Double_t y, 
                        Double_t dist ,Double_t* a, Double_t* b);
   void  SetFilledLength(Double_t flen) {fFilledLength = flen;};
   void  SetEmptyLength(Double_t elen) {fEmptyLength = elen;};
   Double_t SetFilledLength() {return fFilledLength;};
   Double_t SetEmptyLength() {return fEmptyLength;};
   void Paint(Option_t * option = " ");
   void SetNeedReCompute() {fNeedReCompute = kTRUE;};
   ClassDef(XSpline, 1)
};
#endif
