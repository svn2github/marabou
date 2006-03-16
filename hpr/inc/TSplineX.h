#ifndef X_SPLINE
#define X_SPLINE
#include "TObject.h"
#include "TGraph.h"
#include "TArrayD.h"
#include "TArrow.h"
#include "TMarker.h"
#include "TPolyLine.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "Buttons.h"
#include <iostream>
//#include "ControlGraph.h"
//#include "ParallelGraph.h"

class TSplineX;

class RailwaySleeper : public  TPolyLine{
private:
   TSplineX * fParent; //!
   Color_t fColor;
public:
   RailwaySleeper(){};
   RailwaySleeper(Double_t * x, Double_t * y, TSplineX * parent, Color_t color = 1);
//   ~RailwaySleeper(){std::cout << "dtor RailwaySleeper: " << this << std::endl; };
   ~RailwaySleeper(){ };
//   Int_t DistancetoPrimitive(Int_t px, Int_t py){return 9999;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   Color_t GetSleeperColor() {return fColor;};
   void SetColor(Color_t color); 
   void SetSleeperColor(Color_t color);     // *MENU*
   void Draw(Option_t * opt = "");
ClassDef(RailwaySleeper,0)
};
//__________________________________________________________________

class ControlGraph : public TGraph
{
private:
   TSplineX *fParent;
   TArrayF  fShapeFactors;
   Int_t    fSelectedPoint;                 //!
   Double_t fSelectedX;                     //!
   Double_t fSelectedY;                     //!
   Float_t  fSelectedShapeFactor;           //!
   Int_t   fChangeShapeFactorsConjointly;  //!

public:
   ControlGraph (Int_t npoints = 0, Double_t*  x = NULL, Double_t* y = NULL);
   virtual ~ControlGraph() {std::cout << "dtor ControlGraph(): " << this << std::endl;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void SetParent(TSplineX* parent); 
   TSplineX  *GetParent(){return fParent;}; 
   void SetAllShapeFactors(Int_t npoints, Float_t* sf);
   Float_t   GetShapeFactorByPointNumber(Int_t ipoint) {return fShapeFactors[ipoint];};
   Int_t     GetSelectedPoint() { return fSelectedPoint; };
   Double_t  GetSelectedX()     { return fSelectedX; };
   Double_t  GetSelectedY()     { return fSelectedY; };
   Float_t   GetSelectedShapeFactor(){ return fSelectedShapeFactor; };

   virtual void SetOneShapeFactor(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor); // *MENU* *ARGS={ipoint=>fSelectedPoint,x=>fSelectedX,y=>fSelectedY,sfactor=>fSelectedShapeFactor}
   Int_t InsertPoint();                                          // *MENU*
   Int_t RemovePoint();                                          // *MENU*
   virtual void EditControlGraph();    //  *MENU*
   virtual void ControlGraphMixer();    //  *MENU*
   virtual void Delete(Option_t *opt = " "); // *MENU*
   void SetShapeFactors(Int_t id, Int_t ip, Int_t val);
   void SetChangeShapeFactorsConjointly(Int_t val = 1);  // *MENU* *ARGS={val=>fChangeShapeFactorsConjointly}
   Int_t  GetChangeShapeFactorsConjointly(); 

   TGraph* AddPGraph(Double_t dist = 2, Color_t color=0, 
                            Width_t width=0, Style_t style=0);   // *MENU*
   ClassDef(ControlGraph, 1)
};
//__________________________________________________________________

class ParallelGraph : public TGraph
{
private:
   TGraph* fParent;
   Double_t fDist;       // distance to parent
   Bool_t fClosed;
public:
   ParallelGraph () {};
   ParallelGraph (TGraph *parent, Double_t dist, Bool_t closed);
   virtual ~ParallelGraph() {if (fParent) fParent->RecursiveRemove(this);};
   void Compute ();
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   TGraph *GetParent(){return fParent;}; 
   Double_t GetDist(){return fDist;}; 
   virtual void Delete(Option_t *opt = " "); // *MENU*
   void CorrectForArrows(Double_t alength, Double_t aangle,Double_t aindent_angle,
                                     Bool_t at_start, Bool_t at_end);
   ClassDef(ParallelGraph, 0)
};
//__________________________________________________________________

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

class TSplineX : public TGraph
{
protected:
   ShapeFactor*  fShapeFactorList;     //! pointer to linked list
   ControlPoint* fControlPointList;    //! 
   Float_t       fPrec;
   Bool_t        fClosed;
   Int_t         fNofControlPoints;
   Int_t         fNpoints;           // number of used points
   Bool_t        fReady; 
   Bool_t        fComputeDone;     // !
   TArrayD       fX;                 // result X
   TArrayD       fY;                 // result Y
   ControlGraph  fCPGraph;           // Controlpoints
   Int_t         fMType;             // markertype for ControlGraph
   Size_t        fMSize;             // markersize for ControlGraph
   TObjArray     fPGraphs;           // pointer list to parallel lines
 //  TArrayD       fPDists;
   Int_t         fRailwaylike;
   Double_t      fFilledLength;      // draw like railway in maps
   Double_t      fEmptyLength;       // space in between
   TGraph*       fArrowAtStart;
   TGraph*       fArrowAtEnd;
   Bool_t        fPaintArrowAtStart;
   Bool_t        fPaintArrowAtEnd;
   Int_t         fArrowFill;
   Double_t      fArrowLength;
   Double_t      fArrowAngle;
   Double_t      fArrowIndentAngle;
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
   TSplineX();
   TSplineX(Int_t npoints, Double_t *x, Double_t *y, 
           Float_t *sf = 0, Float_t prec = 0.01, Bool_t closed = kFALSE);
   virtual ~TSplineX();
   void Remove() { delete this; } // *MENU*
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void RecursiveRemove(TObject * obj) {fPGraphs.Remove(obj);};
   virtual void EditControlGraph();    //  *MENU*
   void Print(Option_t * opt = " ") const{TGraph::Print();};    //  *MENU*
   void DrawControlPoints(Int_t marker = 24, Size_t size = 1); // *MENU* *ARGS={marker=>fMType, size=>fMSize}
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

//   ParallelGraph* AddParallelGraphExt(ParallelGraph* ng = NULL, Double_t dist = 2, Color_t color=0, 
 //                           Width_t width=0, Style_t style=0);
   ParallelGraph* AddParallelGraph(Double_t dist = 2, Color_t color=0, 
                            Width_t width=0, Style_t style=0);   // *MENU*
   TObjArray* GetParallelGraphs() {return &fPGraphs;};
   void  DrawParallelGraphs();
//   Double_t* GetParallelDists() {return fPDists.GetArray();};
   ControlGraph* GetControlGraph() {return &fCPGraph;};

   void RemovePolyLines();
//   Bool_t ComputeParallelGraph(TGraph* ograph, ParallelGraph* pgraph, 
//                                Double_t dist, Bool_t closed);
   static Double_t Length(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   static Double_t PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   static void Nextpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   static void Endpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   static void Midpoint(Double_t phi1, Double_t phi2, Double_t x, Double_t y, 
                        Double_t dist ,Double_t* a, Double_t* b);
   void  SetRailwaylike(Int_t railway) {fRailwaylike = railway;};
   Int_t GetRailwaylike() {return fRailwaylike;};
   void  SetFilledLength(Double_t flen) {fFilledLength = flen;};
   void  SetEmptyLength(Double_t elen) {fEmptyLength = elen;};
   Double_t GetFilledLength() {return fFilledLength;};
   Double_t GetEmptyLength() {return fEmptyLength;};
   void Paint(Option_t * option = " ");
   void SetComputeDone(Bool_t done) {fComputeDone = done;};
   void SetColor(Color_t color);
   void AddArrow(Int_t where, Double_t size, Double_t angle, Double_t indent_angle, Int_t filled);
   void PaintArrow(Int_t where);
   Bool_t   IsClosed()    {return fClosed;};
   Int_t    GetArrowFill()   {return fArrowFill;};
   Double_t GetArrowLength() {return fArrowLength;};
   Double_t GetArrowAngle()  {return fArrowAngle;}; 
   Double_t GetArrowIndentAngle() {return fArrowIndentAngle;}; 

   void SetArrowFill(Int_t filled) {
      if (filled > 0) {
         if (fArrowAtStart)fArrowAtStart->SetFillStyle(1001);
         if (fArrowAtEnd)fArrowAtEnd->SetFillStyle(1001);
      } else {
         if (fArrowAtStart)fArrowAtStart->SetFillStyle(0);
         if (fArrowAtEnd)fArrowAtEnd->SetFillStyle(0);
      }
      fArrowFill = filled;
   }; //  *MENU*
   void SetArrowLength(Double_t length) {fArrowLength = length;}; //  *MENU*
   void SetArrowAngle(Double_t angle) {fArrowAngle = angle;}; //  *MENU*
   void SetArrowIndentAngle(Double_t angle) {fArrowIndentAngle = angle;}; //  *MENU*
   ClassDef(TSplineX, 1)
};
#endif