#ifndef X_SPLINE
#define X_SPLINE
#include "TObject.h"
#include "TGraph.h"
#include "TArrayD.h"
#include "TArrayF.h"
#include "TArrow.h"
#include "TList.h"
#include "TMarker.h"
#include "TObjArray.h"
#include "TPolyLine.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "Buttons.h"
#include <iostream>

class TSplineX;

//_________________________________________________________________

class PolyLineNoEdit: public  TPolyLine {
public:
   PolyLineNoEdit(){};
   PolyLineNoEdit(Int_t np, Double_t * x = NULL, Double_t * y = NULL);
   virtual ~PolyLineNoEdit() {};
   Int_t DistancetoPrimitive(Int_t, Int_t){return 9999;};
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(ofstream &, Option_t *){};     // dont write to .C file
 #endif 

ClassDef(PolyLineNoEdit,0)
};
//__________________________________________________________________

class RailwaySleeper : public  TPolyLine {
private:
   TSplineX * fParent; //!
//   Color_t fColor;
public:
   RailwaySleeper(){};
   RailwaySleeper(Double_t * x, Double_t * y, TSplineX * parent = NULL,
                  Color_t color = 1);
   ~RailwaySleeper(){ };
   Int_t DistancetoPrimitive(Int_t, Int_t){return 9999;};
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void Draw(Option_t * opt = "F");
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(ofstream &, Option_t *){};     // dont write to .C file
 #endif 
ClassDef(RailwaySleeper,0)
};
//__________________________________________________________________

class ControlGraph : public TGraph
{
private:
   TSplineX *fParent;                       //
   TArrayF  fShapeFactors;
   Int_t    fSelectedPoint;                 //!
   Double_t fSelectedX;                     //!
   Double_t fSelectedY;                     //!
   Float_t  fSelectedShapeFactor;           //!
   TArrayI fMixerValues;                    //!
   TArrayI fMixerMinval;                    //!
   TArrayI fMixerMaxval;                    //!
   TArrayI fMixerFlags;                     //!
 

public:
   ControlGraph (Int_t npoints = 0, Double_t*  x = NULL, Double_t* y = NULL);
   virtual ~ControlGraph() {std::cout << "dtor ControlGraph(): " << this << std::endl;};
   Int_t DistancetoPrimitive(Int_t px, Int_t py);
   void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void SetParent(TSplineX* parent); 
   TSplineX  *GetParent(){return fParent;}; 
   void SetLength(Int_t npoints) {Set(npoints);fShapeFactors.Set(npoints);};
   void SetAllShapeFactors(Int_t npoints, Float_t* sf);
   Float_t   GetShapeFactor(Int_t ipoint) {return fShapeFactors[ipoint];};
   Int_t     GetSelectedPoint() { return fSelectedPoint; };
   Double_t  GetSelectedX()     { return fSelectedX; };
   Double_t  GetSelectedY()     { return fSelectedY; };
   Float_t   GetSelectedShapeFactor(){ return fSelectedShapeFactor;};

   void SetShapeFactor(Int_t ipoint, Int_t f100); 
   void SetOneShapeFactor(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor); // *MENU* *ARGS={ipoint=>fSelectedPoint,x=>fSelectedX,y=>fSelectedY,sfactor=>fSelectedShapeFactor}
   void SetControlPoint(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor); // *MENU* *ARGS={ipoint=>fSelectedPoint,x=>fSelectedX,y=>fSelectedY,sfactor=>fSelectedShapeFactor}
   void GetControlPoint(Int_t ipoint, Double_t *x, Double_t *y, Float_t *sfactor); 
   Int_t InsertPoint();                  // *MENU*
   Int_t RemovePoint();                  // *MENU*
   Int_t RemovePoint(Int_t ip) {return ip;};  //
//   virtual void EditControlGraph();    // *MENU*
   virtual void ControlGraphMixer();     // *MENU*
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(ofstream &, Option_t *){};     // dont write to .C file
 #endif 
   ClassDef(ControlGraph, 1)
};
//__________________________________________________________________

class ParallelGraph : public TPolyLine
{
private:
   TSplineX* fParent;
   ParallelGraph* fSlave;
   Double_t fDistToSlave;    // distance to a partner 
   ParallelGraph* fMaster;
   Double_t fDist;           // distance to parent
   Bool_t fClosed;
   Bool_t fIsRail;           // is part of a railway like track
public:
   ParallelGraph () {};
   ParallelGraph (TSplineX *parent, Double_t dist, Bool_t closed);
   virtual ~ParallelGraph();
   void    Compute();
   void    Paint(Option_t * option = " ");
   Int_t   DistancetoPrimitive(Int_t px, Int_t py);
   void    Pop();
   void    ExecuteEvent(Int_t event, Int_t px, Int_t py);
   TSplineX *GetParent(){return fParent;}; 
   Double_t  GetDist(){return fDist;}; 
   void      SetDist(Double_t d){fDist = d;};           // *MENU*
   void   Remove(Option_t * opt);                                     // *MENU*
   void   Delete(Option_t * opt) {this->Remove(opt);};              
   void   FillToSlave(Double_t dist = 0);               // *MENU*
   void   ClearFillToSlave();                           // *MENU*
   void   SetSlave(ParallelGraph* slave)   {fSlave = slave;};
   void   SetMaster(ParallelGraph* master) {fMaster = master;};
   Double_t  GetDistToSlave()              {return fDistToSlave;};
   void      SetDistToSlave(Double_t d)    {fDistToSlave = d;};
   ParallelGraph* GetSlave()               {return fSlave;};
   ParallelGraph* GetMaster()              {return fMaster;};
   Bool_t GetIsRail()                      {return fIsRail;};
   void   SetIsRail(Bool_t israil = kTRUE) {fIsRail = israil;};
   void   CorrectForArrows(Double_t rxy, Double_t alen,Double_t aangle, Double_t aind_angle,
                           Bool_t at_start, Bool_t at_end);
 #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *){};     // dont write to .C file
 #else
   void SavePrimitive(ofstream &, Option_t *){};     // dont write to .C file
 #endif 
   ClassDef(ParallelGraph, 1)
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
   ShapeFactor (Float_t f, ShapeFactor* nf) {      s = f;
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

class TSplineX : public TPolyLine
{

friend class ControlGraph;
friend class ParallelGraph;
friend class HandleMenus;

private:
   ShapeFactor*  fShapeFactorList;   //! pointer to linked list
   ControlPoint* fControlPointList;  //! 
   Int_t         fNofControlPoints;
   Int_t         fNpoints;           //! number of used points
   Bool_t        fComputeDone;       //!
   TArrayD       fX;                 //! result X
   TArrayD       fY;                 //! result Y
   ControlGraph  fCPGraph;           // Controlpoints
   Bool_t        fCPDrawn;           //!
   Style_t       fMStyle;            // markertype for ControlGraph
   Size_t        fMSize;             // markersize for ControlGraph
   ParallelGraph *fRailL;            // pointer to left rail, if railwaylike
   ParallelGraph *fRailR;            // pointer to right rail
   TObjArray     fPGraphs;           // pointer list to parallel lines
   PolyLineNoEdit *fArrowAtStart;    //!
   PolyLineNoEdit *fArrowAtEnd;      //!
   TList         fDPolyLines;        //!

   Float_t       fPrec;
   Bool_t        fClosed;
   Int_t         fRailwaylike;
   Double_t      fRailwayGage;
   Double_t      fFilledLength;      // draw like railway in maps
   Double_t      fEmptyLength;       // space in between
   Style_t       fLineStyle;
   Width_t       fLineWidth;
   Color_t       fLineColor;
   Int_t         fParallelFill;
   Bool_t        fPaintArrowAtStart;
   Bool_t        fPaintArrowAtEnd;
   Int_t         fArrowFill;
   Double_t      fArrowLength;
   Double_t      fArrowAngle;
   Double_t      fArrowIndentAngle;
   Double_t      fRatioXY;

	Double_t f_blend(Double_t numerator, Double_t denominator);
	Double_t g_blend(Double_t u, Double_t q);
	Double_t h_blend(Double_t u, Double_t q);
	void negative_s1_influence(Double_t t, Double_t s1, Double_t *A0, Double_t *A2);
	void negative_s2_influence(Double_t t, Double_t s2, Double_t *A1, Double_t *A3);
	void positive_s1_influence(Int_t k, Double_t t, Double_t s1, Double_t *A0, Double_t *A2);
	void positive_s2_influence(Int_t k, Double_t t, Double_t s2, Double_t *A1, Double_t *A3);
	void point_computing(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, 
             ControlPoint *p2, ControlPoint *p3, Double_t *xs, Double_t *ys);
   void    point_adding(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, 
                                      ControlPoint *p2, ControlPoint *p3);
   Float_t step_computing(int k, ControlPoint *p0, ControlPoint *p1,
                               ControlPoint *p2, ControlPoint *p3, 
                               Double_t s1, Double_t s2, Float_t precision);
   void    spline_segment_computing(Float_t step, Int_t k, 
                                 ControlPoint *p0, ControlPoint *p1, 
                                 ControlPoint *p2, ControlPoint *p3, 
                                 Double_t s1, Double_t s2);
   Int_t   op_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision);
   Int_t   cl_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision);
   Int_t   add_point(Double_t x, Double_t y);
   Int_t   add_closepoint();
   void    too_many_points();
   void    Delete_ShapeFactors();
   void    Delete_ControlPoints();
   void    SetGraph();
   void    CopyControlPoints();

protected:

   Int_t ComputeSpline();

   Double_t Length(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   Double_t PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
   void Nextpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   void Endpoint(Double_t phi, Double_t x, Double_t y, 
                        Double_t dist, Double_t* a,  Double_t* b);
   void Midpoint(Double_t phi1, Double_t phi2, Double_t x, Double_t y, 
                        Double_t dist ,Double_t* a, Double_t* b);

public:
   TSplineX();
   TSplineX(Int_t npoints, Double_t *x = NULL, Double_t *y = NULL, 
           Float_t *sf = NULL, Float_t prec = 0.2, Bool_t closed = kFALSE);
   virtual ~TSplineX();
   Int_t   DistancetoPrimitive(Int_t px, Int_t py);
   void    ExecuteEvent(Int_t event, Int_t px, Int_t py);
   void    Pop();
//   void  EditControlGraph();                                        // *MENU*
   Int_t   InsertPoint() {return fCPGraph.InsertPoint();};            // *MENU*
   Int_t   RemovePoint() {return fCPGraph.RemovePoint();};            // *MENU*
   void    RemovePolyLines();
   void    ControlGraphMixer(){fCPGraph.ControlGraphMixer();};        // *MENU* 
   void    Print(Option_t *) const;                                   // *MENU*
   void    DrawControlPoints(Style_t marker = 24, Size_t size = 1);   // *MENU* *ARGS={marker=>fMStyle, size=>fMSize}
   void    RemoveControlPoints();                                     // *MENU* 
   Style_t GetMStyle() {return fMStyle;};
   Size_t  GetMSize()  {return fMSize;};
   void    SetMStyle(Style_t type) {fMStyle = type;};
   void    SetMSize(Size_t size)   {fMSize = size;};
   void    SetAllControlPoints(Int_t npoints, Double_t* x, Double_t* y);
   void    SetControlPoint(Int_t ip, Double_t x, Double_t y, Float_t sf)
             {fCPGraph.SetControlPoint(ip, x, y, sf);}; 
   void    GetControlPoint(Int_t ip, Double_t *x, Double_t *y, Float_t *sf)
             {fCPGraph.GetControlPoint(ip, x, y, sf);};  
   void    SetShapeFactors(Int_t npoints, Float_t* sf);
   Int_t   GetNofControlPoints() {return fCPGraph.GetN();};
   Bool_t  GetCPDrawn()         {return fCPDrawn;};
   Int_t   GetResult(Double_t*& x, Double_t*& y);
   void    SetIsClosed(Bool_t isclosed = kTRUE);                      // *MENU*
   Bool_t  GetIsClosed()        {return fClosed;};

   ControlGraph* GetControlGraph() {return &fCPGraph;};
   TList*     GetDPolyLines() {return &fDPolyLines;};  

   void Paint(Option_t * option = " ");
   void PaintArrow(Int_t where);

   void     DrawParallelGraphs();
   ParallelGraph* AddParallelGraph(Double_t dist = 2, Color_t color=0,  
                            Width_t width=0, Style_t style=0);        // *MENU*
   TObjArray* GetParallelGraphs() {return &fPGraphs;};  

   void     SetRailwaylike (Double_t gage = 4);                       // *MENU*
   Int_t    IsRailwaylike()                {return fRailwaylike;};
   void     SetRailwayGage (Double_t gage) {SetRailwaylike(gage);};   // *MENU*
   Double_t GetRailwayGage ()              {return fRailwayGage;};    // *MENU*
   void     SetFilledLength(Double_t flen) {fFilledLength = flen;};   // *MENU*
   void     SetEmptyLength(Double_t elen)  {fEmptyLength = elen;};    // *MENU*
   Double_t GetFilledLength()              {return fFilledLength;};
   Double_t GetEmptyLength()               {return fEmptyLength;};
   void     NeedReCompute()                {fComputeDone = kFALSE;};
   void     SetSleeperColor(Color_t color);

   Int_t    GetParallelFill()    {return fParallelFill;};
   void     SetParallelFill(Int_t sf) {fParallelFill = sf;};          // *MENU*

   void     AddArrow(Int_t where = 1, Double_t size = 10, Double_t angle = 30,
                     Double_t indent_angle = 0, Int_t filled = 0);    // *MENU*
   void     SetArrowLength(Double_t length) {fArrowLength = length;}; // *MENU*
   void     SetArrowAngle(Double_t angle)   {fArrowAngle = angle;};   // *MENU*
   void     SetArrowIndentAngle(Double_t a) {fArrowIndentAngle = a;}; // *MENU*
   Double_t GetArrowLength()                {return fArrowLength;};
   Double_t GetArrowAngle()                 {return fArrowAngle;}; 
   Double_t GetArrowIndentAngle()           {return fArrowIndentAngle;}; 

   void     SetPaintArrowAtStart(Bool_t ok) {fPaintArrowAtStart = ok;};
   void     SetPaintArrowAtEnd(Bool_t ok)   {fPaintArrowAtEnd = ok;};
   Bool_t   GetPaintArrowAtStart()          {return fPaintArrowAtStart;};
   Bool_t   GetPaintArrowAtEnd()            {return fPaintArrowAtEnd;};
   Int_t    GetArrowFill()                  {return fArrowFill;};
   void     SetArrowFill(Bool_t filled);                              // *MENU*
   #if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void SavePrimitive(ostream &, Option_t *);
   #else
   void SavePrimitive(ofstream &, Option_t *); 
   #endif 

   ClassDef(TSplineX, 1)
};
#endif
