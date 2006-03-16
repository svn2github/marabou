/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish and/or distribute copies of
 * the Software, and to permit persons who receive copies from any such 
 * party to do so, with the only requirement being that this copyright 
 * notice remain intact.
 *
 */

/********************* CURVES FOR SPLINES *****************************

 The following spline drawing routines are from

    "X-splines : A Spline Model Designed for the End User"

    by Carole BLANC and Christophe SCHLICK,
    in Proceedings of SIGGRAPH ' 95

***********************************************************************/
//
//  Code taken from xfig (see Copyright above)
//  Mods by OS:
//     Change to c++ calling conventions
//     Wrap into a ROOT class
//  
//
// #include "fig.h"
// #include "resources.h"
// #include "mode.h"

#include <math.h>
#include "TSplineX.h"
#include "iostream"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TPolyLine.h"
#include "TGMrbTableFrame.h"
#include "TGMrbSliders.h"
#include "HprEditBits.h"
//#include "RailwaySleeper.h"

namespace std {} using namespace std;

#define         HIGH_PRECISION    0.5
#define         LOW_PRECISION     1.0
#define         ZOOM_PRECISION    5.0
#define         ARROW_START       4
#define         MAX_SPLINE_STEP   0.2

/***********************************************************************/

#define Q(s)  (-(s))
#define EQN_NUMERATOR(dim) \
  (A_blend[0]*p0->dim+A_blend[1]*p1->dim+A_blend[2]*p2->dim+A_blend[3]*p3->dim)


ClassImp(TSplineX)
ClassImp(ControlGraph)
ClassImp(ParallelGraph)
ClassImp(RailwaySleeper)

//_______________________________________________________________________
//
// This class implements:
// "X-splines : A Spline Model Designed for the End User"
//   
//    by Carole BLANC and Christophe SCHLICK,
//    described in Proceedings of SIGGRAPH ' 95
//
//_______________________________________________________________________


Double_t f_blend(Double_t numerator, Double_t denominator)
{
  Double_t p = 2 * denominator * denominator;
  Double_t u = numerator / denominator;
  Double_t u2 = u * u;

  return (u * u2 * (10 - p + (2*p - 15)*u + (6 - p)*u2));
}

Double_t g_blend(Double_t u, Double_t q)             /* p equals 2 */
{
  return(u*(q + u*(2*q + u*(8 - 12*q + u*(14*q - 11 + u*(4 - 5*q))))));
}

Double_t h_blend(Double_t u, Double_t q)
{
  Double_t u2=u*u;
   return (u * (q + u * (2 * q + u2 * (-2*q - u*q))));
}

void negative_s1_influence(Double_t t, Double_t s1, Double_t *A0, Double_t *A2)
{
  *A0 = h_blend(-t, Q(s1));
  *A2 = g_blend(t, Q(s1));
}

void negative_s2_influence(Double_t t, Double_t s2, Double_t *A1, Double_t *A3)
{
  *A1 = g_blend(1-t, Q(s2));
  *A3 = h_blend(t-1, Q(s2));
}

void positive_s1_influence(Int_t k, Double_t t, Double_t s1, Double_t *A0, Double_t *A2)
{
  Double_t Tk;
  
  Tk = k+1+s1;
  *A0 = (t+k+1<Tk) ? f_blend(t+k+1-Tk, k-Tk) : 0.0;
  
  Tk = k+1-s1;
  *A2 = f_blend(t+k+1-Tk, k+2-Tk);
}

void positive_s2_influence(Int_t k, Double_t t, Double_t s2, Double_t *A1, Double_t *A3)
{
  Double_t Tk;

  Tk = k+2+s2; 
  *A1 = f_blend(t+k+1-Tk, k+1-Tk);
  
  Tk = k+2-s2;
  *A3 = (t+k+1>Tk) ? f_blend(t+k+1-Tk, k+3-Tk) : 0.0;
}


void TSplineX::point_adding(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3)
{
  Double_t weights_sum;

  weights_sum = A_blend[0] + A_blend[1] + A_blend[2] + A_blend[3];
  if (!add_point(EQN_NUMERATOR(x) / (weights_sum),
		 EQN_NUMERATOR(y) / (weights_sum)))
      too_many_points();
}

void point_computing(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t *xs, Double_t *ys)
{
  Double_t weights_sum;

  weights_sum = A_blend[0] + A_blend[1] + A_blend[2] + A_blend[3];

  *xs = (int)round(EQN_NUMERATOR(x) / (weights_sum));
  *ys = (int)round(EQN_NUMERATOR(y) / (weights_sum));
}

Float_t TSplineX::step_computing(Int_t k, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t s1, Double_t s2, Float_t precision)
{
  Double_t A_blend[4];
  Double_t    xstart, ystart, xend, yend, xmid, ymid, xlength, ylength;
  Double_t    start_to_end_dist; 
  Double_t    number_of_steps;
  Float_t  step, angle_cos, scal_prod, xv1, xv2, yv1, yv2, sides_length_prod;
  
  /* This function computes the step used to draw the segment (p1, p2)
     (xv1, yv1) : coordinates of the vector from middle to origin
     (xv2, yv2) : coordinates of the vector from middle to extremity */

  if ((s1 == 0) && (s2 == 0))
    return(1.0);              /* only one step in case of linear segment */

  /* compute coordinates of the origin */
  if (s1>0) {
      if (s2<0) {
	  positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	  negative_s2_influence(0.0, s2, &A_blend[1], &A_blend[3]); 
      } else {
	  positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	  positive_s2_influence(k, 0.0, s2, &A_blend[1], &A_blend[3]); 
      }
      point_computing(A_blend, p0, p1, p2, p3, &xstart, &ystart);
  } else {
      xstart = p1->x;
      ystart = p1->y;
  }
  
  /* compute coordinates  of the extremity */
  if (s2>0) {
      if (s1<0) {
	  negative_s1_influence(1.0, s1, &A_blend[0], &A_blend[2]);
	  positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]);
      } else {
	  positive_s1_influence(k, 1.0, s1, &A_blend[0], &A_blend[2]);
	  positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]); 
      }
      point_computing(A_blend, p0, p1, p2, p3, &xend, &yend);
  } else {
      xend = p2->x;
      yend = p2->y;
  }

  /* compute coordinates  of the middle */
  if (s2>0) {
      if (s1<0) {
	  negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
	  positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]);
      } else {
	  positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
	  positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]); 
	}
  } else if (s1<0) {
      negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  } else {
      positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  }
  point_computing(A_blend, p0, p1, p2, p3, &xmid, &ymid);

  xv1 = xstart - xmid;
  yv1 = ystart - ymid;
  xv2 = xend - xmid;
  yv2 = yend - ymid;

  scal_prod = xv1*xv2 + yv1*yv2;
  
  sides_length_prod = sqrt((xv1*xv1 + yv1*yv1)*(xv2*xv2 + yv2*yv2));

  /* compute cosinus of origin-middle-extremity angle, which approximates the
     curve of the spline segment */
  if (sides_length_prod == 0.0)
    angle_cos = 0.0;
  else
    angle_cos = scal_prod/sides_length_prod; 

  xlength = xend - xstart;
  ylength = yend - ystart;

  start_to_end_dist = sqrt(xlength*xlength + ylength*ylength);

  /* more steps if segment's origin and extremity are remote */
  number_of_steps = sqrt(start_to_end_dist)/2;

  /* more steps if the curve is high */
  number_of_steps += (int)((1 + angle_cos)*10);

  if (number_of_steps == 0)
    step = 1;
  else
    step = precision/number_of_steps;
  
  if ((step > MAX_SPLINE_STEP) || (step == 0))
    step = MAX_SPLINE_STEP;
//  cout << "precision,number_of_steps,step " << 
//         precision << " " <<number_of_steps << " " <<step << endl; 
  return (step);
}

void TSplineX::spline_segment_computing(Float_t step, Int_t k, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t s1, Double_t s2)
{
  Double_t A_blend[4];
  Double_t t;
  
  if (s1<0) {  
     if (s2<0) {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     } else {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     }
  } else if (s2<0) {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      }
  } else {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      } 
  }
}

/********************* MAIN METHODS *************************************/
//
//  Code taken from xfig (see Copyright above)
//  Mods by OS
//  Change to c++ calling conventions
//  Input points and sfactors directly (was via struct F_spline)
#define COPY_CONTROL_POINT(P0, S0, P1, S1) \
      P0 = P1; \
      S0 = S1

#define NEXT_CONTROL_POINTS(P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, P1, S1); \
      COPY_CONTROL_POINT(P1, S1, P2, S2); \
      COPY_CONTROL_POINT(P2, S2, P3, S3); \
      COPY_CONTROL_POINT(P3, S3, P3->next, S3->next)

#define INIT_CONTROL_POINTS(POINTS, FACTORS, P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, POINTS, FACTORS); \
      COPY_CONTROL_POINT(P1, S1, P0->next, S0->next);               \
      COPY_CONTROL_POINT(P2, S2, P1->next, S1->next);               \
      COPY_CONTROL_POINT(P3, S3, P2->next, S2->next)

#define SPLINE_SEGMENT_LOOP(K, P0, P1, P2, P3, S1, S2, PREC) \
      step = step_computing(K, P0, P1, P2, P3, S1, S2, PREC);    \
      spline_segment_computing(step, K, P0, P1, P2, P3, S1, S2)


Int_t TSplineX::op_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision)
{
  Int_t       k;
  Float_t     step;
  ControlPoint   *p0, *p1, *p2, *p3;
  ShapeFactor *s0, *s1, *s2, *s3;

  fNpoints = 0;

  COPY_CONTROL_POINT(p0, s0, cpoints, sfactors);
  COPY_CONTROL_POINT(p1, s1, p0, s0);
  /* first control point is needed twice for the first segment */
  COPY_CONTROL_POINT(p2, s2, p1->next, s1->next);

  if (p2->next == 0) {
      COPY_CONTROL_POINT(p3, s3, p2, s2);
  } else {
      COPY_CONTROL_POINT(p3, s3, p2->next, s2->next);
  }


  for (k = 0 ;  ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
//      if (DONE)
//        break;
      if (p3->next == 0)
	break;
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
  }
  /* last control point is needed twice for the last segment */
//  if (!DONE) {
    COPY_CONTROL_POINT(p0, s0, p1, s1);
    COPY_CONTROL_POINT(p1, s1, p2, s2);
    COPY_CONTROL_POINT(p2, s2, p3, s3);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
//  }
  
  if (!add_point(p3->x, p3->y))
    too_many_points();
  
  return 1 ;
}

Int_t TSplineX::cl_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision)
{
  Int_t i, k;
//  int npoints = fNofControlPoints;
  Float_t     step;
//  Double_t    t;
  ControlPoint   *p0, *p1, *p2, *p3, *first;
  ShapeFactor *s0, *s1, *s2, *s3, *s_first;

  fNpoints = 0;

  INIT_CONTROL_POINTS(cpoints, sfactors, p0, s0, p1, s1, p2, s2, p3, s3);
  COPY_CONTROL_POINT(first, s_first, p0, s0); 

  for (k = 0 ; p3 != 0 ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
//      if (DONE)
//        break;
  }
  /* when we are at the end, join to the beginning */
    COPY_CONTROL_POINT(p3, s3, first, s_first);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);

    for (i=0; i<2; i++) {
//        if (DONE)
//          break;
        k++;
        NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
        SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
    }

  if (!add_closepoint())
    too_many_points();

  return 1 ;
}
//____________________________________________________________________________________
//
// some useful auxiliaries
//
Double_t TSplineX::Length(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   return TMath::Sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
//_______________________________________________________________________________________

Double_t TSplineX::PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   Double_t len = Length(x1, y1, x2, y2);
   Double_t phi = 0;
   if (len > 0) {
      Double_t sina = (y2 - y1) / len;
      Double_t cosa = (x2 - x1) / len;
      phi = TMath::ATan2(sina, cosa);
      if (phi < 0) phi += 2. * TMath::Pi();
   }
   return phi;
}
//_______________________________________________________________________________________

void TSplineX::Nextpoint(Double_t phi, Double_t x, Double_t y, 
              Double_t dist, Double_t* a,  Double_t* b)
{
   Double_t phip = phi; 
   if (phip < 0) phip += 2 * TMath::Pi();
   *a = x + dist * TMath::Cos(phip);
   *b = y + dist * TMath::Sin(phip);
}
//_______________________________________________________________________________________

void TSplineX::Endpoint(Double_t phi, Double_t x, Double_t y, 
              Double_t dist, Double_t* a,  Double_t* b)
{
   Double_t phip = phi - 0.5 * TMath::Pi(); 
   if (phip < 0) phip += 2 * TMath::Pi();
   *a = x + dist * TMath::Cos(phip);
   *b = y + dist * TMath::Sin(phip);
}
//_______________________________________________________________________________________

void TSplineX::Midpoint(Double_t phi1, Double_t phi2, Double_t x, Double_t y, 
              Double_t dist ,Double_t* a, Double_t* b)
{
   Double_t r = dist / TMath::Cos(0.5 * (phi1 - phi2));
   Double_t phi3 = 0.5 * (phi1 + phi2 - TMath::Pi()); 
   if (phi3 < 0) phi3 += 2 * TMath::Pi();
   *a = x + r * TMath::Cos(phi3);
   *b = y + r * TMath::Sin(phi3);
}
//____________________________________________________________________________________

TSplineX::TSplineX()
{
   fControlPointList = 0;
   fShapeFactorList = 0;
   fReady = kFALSE;
   fComputeDone = kFALSE;
   cout << "TSplineX: def ctor npoints " << fNpoints << " " << endl;
}
//____________________________________________________________________________________

TSplineX::TSplineX(Int_t npoints, Double_t *x, Double_t *y,
         Float_t *sf, Float_t prec, Bool_t closed)
{
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*TSplineX constructor-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                          ===================
// The controlpoints of TSplineX are stored in its TGraph parent.
//   
   fControlPointList = 0;
   fShapeFactorList = 0;
   fReady = kFALSE;
   fPrec = prec;
   fClosed = closed;
   fNofControlPoints = 0;
   fNpoints = 0;
   fReady = kFALSE;
   fComputeDone = kFALSE;
//   fNP = 0;
   fRailwaylike = 0;
   fFilledLength = 0;
   fEmptyLength = 0;
   fMType = 24;
   fMSize = 2;
   fArrowAtStart = NULL;
   fArrowAtEnd   = NULL;
   fPaintArrowAtStart = kFALSE;
   fPaintArrowAtEnd   = kFALSE;
   fArrowFill    = 0;
   fArrowLength = 10;
   fArrowAngle  = 30;
   fArrowIndentAngle = 15;
   SetName("TSplineX");
   fCPGraph.SetName("ControlGraph");
   fCPGraph.SetParent(this);
   if (npoints > 0) {
     if (x != NULL && y != NULL) SetControlPoints(npoints, x, y);
     if (sf != NULL) SetShapeFactors(npoints, sf);
   }
   gROOT->GetListOfCleanups()->Add(&fPGraphs);
   fPGraphs.Clear();
   fDPolyLines.Clear();
   cout << "TSplineX: ctor npoints, fNP" << npoints << " "<< endl;
}
//____________________________________________________________________________________

TSplineX::~TSplineX()
{
   cout << "~TSplineX(): "<< this  << endl << flush;
   gROOT->GetListOfCleanups()->Remove(&fPGraphs);
   if (fArrowAtStart) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtStart);
      delete fArrowAtStart;
   }
   if (fArrowAtEnd) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtEnd);
      delete fArrowAtEnd;
   }
   RemovePolyLines();
   fPGraphs.Delete();
   Delete_ControlPoints();
   Delete_ShapeFactors();
   cout << "exit ~TSplineX(): "<< this  << endl << flush;

}
//_____________________________________________________________________________________

void TSplineX::SetShapeFactors(Int_t npoints, Float_t* s)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Set the ShapeFactors-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// 
// ShapeFactor  > -1 < 0: Force line through point. 
//                      : -1  influence of point is wide range
//                      :     leads to big curvature 
//                      : near 0, short range, small curvature
//                 ==  0: Treat like an end point, no smooth
//           < 0 && <= 1: Line not forced through point
//                      : 1 smallest influence
//
   if (npoints <= 0 ||  s == 0) {
      cout << "Incomplete arguments" << endl;
      return;
   }
   if (fNofControlPoints != 0 && npoints != fNofControlPoints) {
      cout << "Number of ShapeFactors: " << npoints << " != " 
           << "Number of ControlPoints " << endl;
      fReady = kFALSE;
      return;
   }
   fCPGraph.SetAllShapeFactors(npoints, s);
   fReady = kTRUE;
}
//_____________________________________________________________________________________

void TSplineX::SetControlPoints(Int_t npoints, Double_t* x, Double_t* y)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Set ControlPoints-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// 
   if (npoints <= 0 || x == 0 && y == 0) {
      cout << "Incomplete arguments" << endl;
      return;
   }

   fNofControlPoints = npoints;
//   if (npoints == 2) fNofControlPoints = 3;
   fCPGraph.Set(fNofControlPoints);
   for (Int_t i = 0; i < npoints; i++) {
      fCPGraph.SetPoint(i, x[i], y[i]);
   }
//  force a point in between
//   if (npoints == 2) {
//      fCPGraph.SetPoint(2, x[1], y[1]);
//      fCPGraph.SetPoint(1, 0.5 * (x[0] + x[1]), 0.5 * (y[0] +y[1]));
//   }
}
//_____________________________________________________________________________________

Int_t TSplineX::ComputeSpline(Float_t prec, Bool_t closed)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Compute the spline-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
// prec: Controls the number of intermediate points
// open: Open spline if TRUE otherwise closed
//
   if (!fReady) {
      cout << "ShapeFactors not set" << endl;
      return -1;
   }
   if (prec == 0) {
      cout << "Precision must be != 0" << endl;
      return -1;
   }
   if (prec > 0) {
      fPrec = prec;
      fClosed = closed;
   }
//   Dump();
   CopyControlPoints();
   if (fClosed) 
      cl_spline(fControlPointList, fShapeFactorList, fPrec);
   else
      op_spline(fControlPointList, fShapeFactorList, fPrec);
   SetGraph();
//   cout << "ComputeSpline  fNpoints: " << fNpoints << " fNP: " << fNP<< endl;
   if (fPGraphs.GetEntries() > 0) {
      for (Int_t i = 0; i <= fPGraphs.GetLast(); i++) {
         ParallelGraph * gr = (ParallelGraph*)fPGraphs[i];
         if (gr) {
            gr->Compute();
            if ((fPaintArrowAtStart || fPaintArrowAtEnd) 
                && (fFilledLength <= 0 || fEmptyLength  <= 0))
              gr->CorrectForArrows( fArrowLength, fArrowAngle,fArrowIndentAngle,
                                    fPaintArrowAtStart, fPaintArrowAtEnd);
         }
      }
   }
   fComputeDone = kFALSE;
//   cout << "ComputeSpline  fNpoints: " << fNpoints << " fNP: " << fNP<< endl;
   gPad->Modified();
   gPad->Update();
   return fNpoints;
}
//_____________________________________________________________________________________

Int_t TSplineX::GetResult(Double_t*& x, Double_t*& y)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Return values of the resulting spline-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
    x = fX.GetArray();
    y = fY.GetArray();
    return fNpoints;
}
//_____________________________________________________________________________________

void TSplineX::RemovePolyLines()
{
   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      TObject* obj;
      TList * lop = gPad->GetListOfPrimitives();
      lop->ls();
      while ( (obj=next()) ) {
         if (lop->FindObject(obj)) cout << "lop->Remove(obj) " << obj<< endl;
         lop->Remove(obj);
      }
      fDPolyLines.Delete();
      lop->ls();
   }
}
//_____________________________________________________________________________________

void TSplineX::Delete_ControlPoints()
{
   if (fControlPointList) {
      while (fControlPointList) {
         ControlPoint * next = fControlPointList->GetNext();
         delete  fControlPointList;
         fControlPointList = next;
      }
   }
   fControlPointList = NULL;
}
//_____________________________________________________________________________________

void TSplineX::Delete_ShapeFactors()
{
   if (fShapeFactorList) {
      while (fShapeFactorList) {
         ShapeFactor * next = fShapeFactorList->GetNext();
         delete  fShapeFactorList;
         fShapeFactorList = next;
      }
   }
   fShapeFactorList = NULL;
}
//_____________________________________________________________________________________

void TSplineX::CopyControlPoints()
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Copy ControlPoints and ShapeFactors into a linked list
// as expected by the internally used routines  doing the
// real compututations
//

   Double_t xp, yp;
   if (fControlPointList) Delete_ControlPoints();
   if (fShapeFactorList)  Delete_ShapeFactors() ;
   ControlPoint* previous_cp = 0;     
   ShapeFactor* previous_sf = 0;
   fNofControlPoints = fCPGraph.GetN();
   for (Int_t i = 0; i < fNofControlPoints; i++) {
      fCPGraph.GetPoint(i, xp, yp);
//      cout << i << " " << xp << " " << yp << endl;

      ControlPoint * p_cp = new ControlPoint(xp, yp, (ControlPoint*)0);
      if (fControlPointList == 0) fControlPointList = p_cp;
      if (previous_cp != 0) previous_cp->SetNext(p_cp);
      previous_cp = p_cp;
      ShapeFactor* p_sf = new ShapeFactor(fCPGraph.GetShapeFactorByPointNumber(i), (ShapeFactor*)0);
      if (fShapeFactorList == 0) fShapeFactorList = p_sf;
      if (previous_sf != 0) previous_sf->SetNext(p_sf);
      previous_sf = p_sf;
   }
//   cout << fNofControlPoints << endl;
   fX.Set(fNofControlPoints * 50);
   fY.Set(fNofControlPoints * 50);
}
//_____________________________________________________________________________________

Int_t TSplineX::add_point(Double_t x, Double_t y)
{
   if (fNpoints >= fX.GetSize()) {
      fX.Set(fNpoints + 50);
      fY.Set(fNpoints + 50);
      cout << "TSplineX::add_point, expand result vector to: " << fX.GetSize()<< endl;
   }
   fX[fNpoints] = x;
   fY[fNpoints] = y;
//   cout << x << " " << y << endl;
   fNpoints ++;
   return 1;
}
//_____________________________________________________________________________________

Int_t TSplineX::add_closepoint()
{
   if (fNpoints >= fX.GetSize()) {
      fX.Set(fNpoints + 1);
      fY.Set(fNpoints + 1);
   }
   fX[fNpoints] = fX[0];
   fY[fNpoints] = fY[0];
   fNpoints ++;
   return 1;
}
//_____________________________________________________________________________________

void TSplineX::too_many_points()
{
   cout << "too_many_points()" << endl;
}
//_____________________________________________________________________________________
 
void TSplineX::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//
//  The spline is shifted by shifting its ControlPoints
//
    fCPGraph.ExecuteEvent(event, px, py);
}
//_____________________________________________________________________________________
 
void TSplineX::SetGraph()
{
   Set(fNpoints);
   for (Int_t i = 0; i < fNpoints; i++) {
      SetPoint(i, fX[i], fY[i]);
   }
}
//_____________________________________________________________________________________
 
void TSplineX::DrawControlPoints(Int_t marker,  Size_t size)
{
   if (fCPGraph.GetN() <= 0) return;
   
   if (marker != 0) fMType =  marker;
   fCPGraph.SetMarkerStyle(fMType);
   if (size > 0)  fMSize = size;
   fCPGraph.SetMarkerSize(fMSize);
    cout << "DrawControlPoints marker " << marker<< " fMType " << fMType<<endl; 
   fCPGraph.Draw("P");
   gPad->Update();
}
//_____________________________________________________________________________________

void TSplineX::EditControlGraph() 
{
    fCPGraph.EditControlGraph();
}
//_____________________________________________________________________________________

ParallelGraph* TSplineX::AddParallelGraph(Double_t dist, Color_t color, Width_t width, Style_t style)
{
   ParallelGraph *pgraph = new ParallelGraph (this, dist, fClosed);
   Int_t idx = fPGraphs.GetLast() + 1;
   fPGraphs.AddAtAndExpand(pgraph, idx);
   if (color <= 0) pgraph->SetLineColor(this->GetLineColor());
   else            pgraph->SetLineColor(color);
   if (width <= 0) pgraph->SetLineWidth(this->GetLineWidth());
   else            pgraph->SetLineWidth(width);
   if (style <= 0) pgraph->SetLineStyle(this->GetLineStyle());
   else            pgraph->SetLineStyle(style);
   pgraph->Draw("L");
   return pgraph;
}
//_____________________________________________________________________________________

void  TSplineX::DrawParallelGraphs()
{
   for (Int_t i = 0; i < fPGraphs.GetEntries(); i++) {
      TGraph* gr = (TGraph*)fPGraphs[i];
      if (gr) gr->Draw("L");
   }
}
//_____________________________________________________________________________________
 
void TSplineX::Paint(Option_t * option)
{
//   cout << " TSplineX::Paint fComputeDone = ";
//   if (fComputeDone) cout << " true" ;
//  else              cout << " false" ;
//  cout << endl;

   if (fRailwaylike <= 0)  {
      TGraph::Paint(option);
      if (fArrowAtStart) { 
         fArrowAtStart->SetLineColor(GetLineColor());
         if (fArrowAtStart->GetFillStyle() != 0) 
            fArrowAtStart->SetFillColor(GetLineColor());
      } 
      if (fArrowAtEnd) { 
         fArrowAtEnd->SetLineColor(GetLineColor());
         if (fArrowAtEnd->GetFillStyle() != 0) 
            fArrowAtEnd->SetFillColor(GetLineColor());
      } 
      gPad->Modified();
      gPad->Update();

   } 

   if (fComputeDone) return;
   fComputeDone = kTRUE;

   if (fArrowAtStart) {
//      fArrowSize  = fArrowAtStart->GetArrowSize();
//      fArrowAngle = fArrowAtStart->GetAngle();
 //     if (fArrowAtStart->GetFillStyle() != 0) fArrowFill = kTRUE;
//      else                                    fArrowFill = kFALSE;
      gPad->GetListOfPrimitives()->Remove(fArrowAtStart);
      delete fArrowAtStart; 
      fArrowAtStart = NULL;
   }
   if (fArrowAtEnd) {
//      fArrowSize  = fArrowAtEnd->GetArrowSize();
//      fArrowAngle = fArrowAtEnd->GetAngle();
 //     if (fArrowAtEnd->GetFillStyle() != 0) fArrowFill = kTRUE;
 //     else                                  fArrowFill = kFALSE;
      gPad->GetListOfPrimitives()->Remove(fArrowAtEnd);
      delete fArrowAtEnd; 
      fArrowAtEnd = NULL;
   }

   if (fFilledLength <= 0 || fEmptyLength <= 0 || fRailwaylike <= 0) {
//  currently no arrays with railway sleepers allowed
      if (fPaintArrowAtStart) PaintArrow(0);
      if (fPaintArrowAtEnd)   PaintArrow(1);
      return;
   }
   
//  draw railway sleepers
   ParallelGraph* lg = (ParallelGraph*)fPGraphs[0];
   ParallelGraph* rg = (ParallelGraph*)fPGraphs[1];
//   cout << "TSplineX::Paint " << lg << " " << rg  << endl;
   if (lg->GetN() != rg->GetN() || lg->GetN() < 4) {
      cout << "Not enough points on spline" << endl;
      return;
   }

   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      TObject* obj;
      TList * lop = gPad->GetListOfPrimitives();
      while ( (obj=next()) ) lop->Remove(obj); 
      fDPolyLines.Delete();
   }
//   TPolyLine * pl;
   RailwaySleeper * pl;
   Double_t xp[5], yp[5];
   Int_t ip = 0;
   Double_t * xc = this->GetX();
   Double_t * yc = this->GetY();
   Double_t * xl = lg->GetX();
   Double_t * yl = lg->GetY();
   Double_t * xr = rg->GetX();
   Double_t * yr = rg->GetY();
   Double_t needed = fFilledLength;

//   Double_t dist = TMath::Abs(fPDists[0]);
   Double_t dist = TMath::Abs(lg->GetDist());
   Double_t available = 0;
   Double_t phi, ax, ay, xcprev, ycprev, xccur, yccur;
   Bool_t   box_done = kFALSE;
   phi = PhiOfLine( xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
   Endpoint(phi, xc[0], yc[0], -dist, &ax, &ay);
   xp[0] = ax;
   yp[0] = ay;
   Endpoint(phi, xc[0], yc[0],  dist, &ax, &ay);
   xp[1] = ax;
   yp[1] = ay;
   available = Length(xc[0], yc[0], xc[1], yc[1]);
   xcprev = xc[0];
   ycprev = yc[0];
   Bool_t within_gap = kFALSE;

//      draw a filled box with length fFilledLength
   while (ip < GetN()- 2) {
      if (needed <= available) {
         within_gap = kFALSE;
         phi = PhiOfLine( xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
//             extrapolate previous
         Nextpoint(phi, xcprev, ycprev, needed, &xccur, &yccur);
         Endpoint(phi, xccur, yccur, +dist, &ax, &ay);
         xp[2] = ax;
         yp[2] = ay;
         Endpoint(phi, xccur, yccur, -dist, &ax, &ay);
         xp[3] = ax;
         yp[3] = ay;
         xp[4] = xp[0];
         yp[4] = yp[0];
//         pl = new TPolyLine(5, xp, yp);
//         pl->SetBit(kCantEdit);
//         fDPolyLines.Add(pl);
//         pl->SetFillStyle(1003);
//         pl->SetFillColor(this->GetLineColor());
//         pl->Draw("F");
         pl = new RailwaySleeper(xp, yp, this, this->GetLineColor());
         fDPolyLines.Add(pl);
         pl->Draw();

         available -= needed;
         box_done = kTRUE;
         xcprev = xccur;
         ycprev = yccur;
//       filled box complete, enter gap 
         needed = fEmptyLength;
         while (1) {
            within_gap = kTRUE;
            if (needed <= available) {
               phi = PhiOfLine( xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
//                extrapolate previous
               Nextpoint(phi, xcprev, ycprev, needed, &xccur, &yccur);
               xcprev = xccur;
               ycprev = yccur;
               available -= needed;
               needed = fFilledLength;
               Endpoint(phi, xccur, yccur, -dist, &ax, &ay);
               xp[0] = ax;
               yp[0] = ay;
               Endpoint(phi, xccur, yccur, +dist, &ax, &ay);
               xp[1] = ax;
               yp[1] = ay;
               break;
            } else {
               ip += 1;
               if (ip >= GetN()- 2) break; 
               needed -=   Length(xcprev, ycprev, xc[ip], yc[ip]);
               available = Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
               xcprev = xc[ip];
               ycprev = yc[ip];
            }
         }
      } else {
         within_gap = kFALSE;
         xp[2] = xl[ip+1];
         yp[2] = yl[ip+1];
         xp[3] = xr[ip+1];
         yp[3] = yr[ip+1];
         xp[4] = xp[0];
         yp[4] = yp[0];
//         pl = new TPolyLine(5, xp, yp);
//         pl->SetBit(kCantEdit);
//         fDPolyLines.Add(pl);
//         pl->SetFillStyle(1003);
//         pl->SetFillColor(this->GetLineColor());
//         pl->Draw("F");
         pl = new RailwaySleeper(xp, yp, this, this->GetLineColor());
         fDPolyLines.Add(pl);
         pl->Draw();
         ip += 1;
         needed -=   Length(xcprev, ycprev, xc[ip], yc[ip]);
         available = Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
         xcprev = xc[ip];
         ycprev = yc[ip];
         xp[0] = xr[ip];
         yp[0] = yr[ip];
         xp[1] = xl[ip];
         yp[1] = yl[ip];
      }
   }
  
   ip = GetN()- 2;
   if ( !within_gap ) {
      xp[0] = xr[ip];
      yp[0] = yr[ip];
      xp[1] = xl[ip];
      yp[1] = yl[ip];
      xp[2] = xl[ip+1];
      yp[2] = yl[ip+1];
      xp[3] = xr[ip+1];
      yp[3] = yr[ip+1];
      xp[4] = xp[0];
      yp[4] = yp[0];
   
//      pl = new TPolyLine(5, xp, yp);
//      pl->SetBit(kCantEdit);
//      fDPolyLines.Add(pl);
//      pl->SetFillStyle(1003);
//      pl->SetFillColor(this->GetLineColor());
//      pl->Draw("F");
      pl = new RailwaySleeper(xp, yp, this, this->GetLineColor());
      fDPolyLines.Add(pl);
      pl->Draw();
//      cout << "pl->GetFillColor() " << pl->GetFillColor() << endl;
   }
   gPad->Modified();
   gPad->Update();
}
//_____________________________________________________________________________________

void TSplineX::SetColor(Color_t color)
{
   SetLineColor(color);
//   std::cout << "TSplineX::SetColor " << color << std::endl;
   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      RailwaySleeper * rs;
      while ( (rs = (RailwaySleeper*)next()) ) {
         rs->SetColor(color);
      }
      gPad->Update();
   }
}
//_____________________________________________________________________________________

void TSplineX::AddArrow(Int_t where, Double_t length, 
                       Double_t angle, Double_t indent_angle,Int_t filled)
{
   if (where == 0) fPaintArrowAtStart = kTRUE;
   if (where == 1) fPaintArrowAtEnd = kTRUE;
   fArrowLength = length;
   fArrowAngle = angle;
   fArrowIndentAngle = indent_angle;
   fArrowFill = filled;
}
//_____________________________________________________________________________________

void TSplineX::PaintArrow(Int_t where)
{
   Double_t* xp;
   Double_t* yp;
   Int_t np = this->GetResult(xp, yp);
   Double_t px1;
   Double_t py1;
   Double_t px2;
   Double_t py2;
   if (where == 1) {
   	px1 = xp[np-2];
   	py1 = yp[np-2];
   	px2 = xp[np-1];
   	py2 = yp[np-1];
   } else {
      px1 = xp[1];
   	py1 = yp[1];
		px2 = xp[0];
		py2 = yp[0];
   }
   Double_t deg2rad = TMath::Pi() / 180;
   Double_t p2 = 0.5 * fArrowAngle * deg2rad;
   Double_t a2 = fArrowIndentAngle *deg2rad;

   TGraph * arrow = new TGraph(5);
   Double_t *x = arrow->GetX();
   Double_t *y = arrow->GetY();
   x[0] = x[4] = 0;
   y[0] = y[4] = 0;
   x[1] = - fArrowLength;
   y[1] = fArrowLength * TMath::Tan(p2);
   x[2] = - fArrowLength + y[1] * TMath::Tan(a2);
   y[2] = 0;
   x[3] = - fArrowLength;
   y[3] = -y[1];

   Double_t angle = PhiOfLine(px1, py1, px2, py2);
   Double_t cosang = TMath::Cos(angle);
   Double_t sinang = TMath::Sin(angle);
   Double_t xx, yy;
   for (Int_t i = 0; i < 5; i++) {
      xx = x[i] * cosang - y[i] * sinang;
      yy = x[i] * sinang + y[i] * cosang;
      x[i] = xx + px2;
      y[i] = yy + py2;
   }

   arrow->SetLineColor(GetLineColor());
   if (fArrowFill != 0) {
      arrow->SetName("FilledArrow");
      arrow->SetFillStyle(1001);
      arrow->SetFillColor(GetLineColor());
      arrow->Draw("F");
   } else {
      arrow->SetName("OpenArrow");
      arrow->SetFillStyle(0);
      arrow->SetLineWidth(GetLineWidth());
      arrow->Draw();
   }
   if (where == 1) fArrowAtEnd = arrow;
   else            fArrowAtStart = arrow;
}
//____py2 = yp[0];_______________________________________________________________________
//___________________________________________________________________________
//
// ControlGraph, a helper class for TSplineXs,
// To profit from TGraphs methods the Controlpoints are stored
// as a TGraph
//
ControlGraph::ControlGraph (Int_t npoints, Double_t*  x, Double_t* y) :
              TGraph(npoints, x, y) {
   fParent = NULL;
   fSelectedX = 0;
   fSelectedY = 0;
   fSelectedPoint = -1;
   fSelectedShapeFactor = -1;
   fChangeShapeFactorsConjointly = 1;
};
//_____________________________________________________________________________________
   
void ControlGraph::Delete(Option_t *opt)
{
   cout << "Cant delete ControlGraph, delete its TSplineX" << endl; 
}
//_____________________________________________________________________________________
   
void ControlGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
//
// This ExecuteEvent expects to be called b e f o r e the ContextMenu
// is invoked. Here we set the default values. Note in this
// way it is possible to set also values contained in arrays
// since the index can be computed from the position px, py
// After return the ContextMenu will pop up
//
//   static Color_t color = fParent->GetLineColor();
//   static Int_t  lwidth = fParent->GetLineWidth();
//   static Int_t  lstyle = fParent->GetLineStyle();
//   cout << event << " "<< px << " " << py << endl;

   if (event == kButton3Down) {
//      cout << "kButton3Down " << endl;
       for (Int_t i=0; i < fNpoints; i++) {
          Double_t d = TMath::Sqrt(TMath::Power(px -  gPad->XtoAbsPixel(fX[i]), 2) 
                     + TMath::Power(py - gPad->YtoAbsPixel(fY[i]), 2));
//         cout << d << endl;
         if (d < 10) {
            fSelectedX = fX[i];
            fSelectedY = fY[i];
            fSelectedPoint = i; 
            fSelectedShapeFactor = fShapeFactors[i];
            break;
         }
      }
   } else if (event == kButton3Up) {
      cout << "kButton3Up " << endl;
 
//      if (GetLineColor() != color) fParent->SetLineColor(color);
//      if (GetLineWidth() != lwidth) fParent->SetLineWidth(lwidth);
//      if (GetLineStyle() != lstyle) fParent->SetLineStyle(lstyle);
   } else {
      TGraph::ExecuteEvent(event, px, py);
   }
   if (event == kButton1Up && fParent) fParent->ComputeSpline();
}
//_____________________________________________________________________________________
   
void ControlGraph::SetParent(TSplineX* parent) 
{
  fParent = parent;
} 
//_____________________________________________________________________________________

void ControlGraph::SetAllShapeFactors(Int_t npoints, Float_t* sfactors)
{
   fShapeFactors.Set(npoints, sfactors);
}
//_____________________________________________________________________________________

void ControlGraph::SetOneShapeFactor(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor)
{
// This method is invoked from the ContextMenu
//
   if (!fParent) return;
   if (ipoint >=0 && ipoint < fShapeFactors.GetSize() ) {
      fShapeFactors[ipoint] = sfactor;
      SetPoint(ipoint, x, y);
      cout << "Set ShapeFactor[" << ipoint << "] = " << sfactor << endl;
      fParent->ComputeSpline();
   }
   fSelectedShapeFactor = sfactor;
   fSelectedPoint  = ipoint;
}
//_____________________________________________________________________________________

Int_t ControlGraph::InsertPoint() 
{
// This method is invoked from the ContextMenu
// When inserting a point also a ShapeFactor must be inserted 
// for this point

   Int_t npoints_before =  GetN();
   Int_t ipoint = TGraph::InsertPoint();

   if (GetN() > npoints_before) {
      TArrayF temp(fShapeFactors);
      fShapeFactors.Set(GetN());
      for (Int_t i = 0; i < ipoint; i++) fShapeFactors[i] = temp[i];
      fShapeFactors[ipoint] = 0.5;
      for (Int_t i = ipoint +1 ; i < fShapeFactors.GetSize(); i++) { 
         fShapeFactors[i] = temp[i - 1];
      }
      fParent->ComputeSpline();
   }
   return ipoint;
}
//_____________________________________________________________________________________

Int_t ControlGraph::RemovePoint() 
{
// This method is invoked from the ContextMenu
// When removing a point also its ShapeFactor must be removed 

   Int_t npoints_before =  GetN();
   cout << "ControlGraph::RemovePoint() " << gPad->GetEventX() 
        << " " << gPad->GetEventY() << endl;

   Int_t ipoint = TGraph::RemovePoint();
   cout << "ipoint: " << ipoint << endl;

   if (GetN() < npoints_before) {
      TArrayF temp(fShapeFactors);
      fShapeFactors.Set(GetN());
      for (Int_t i = 0; i < ipoint; i++) fShapeFactors[i] = temp[i];
//      fShapeFactors[ipoint] = 0.5;
      for (Int_t i = ipoint; i < fShapeFactors.GetSize(); i++) { 
         fShapeFactors[i] = temp[i + 1];
      }
      fParent->ComputeSpline();
   }
   return ipoint;
}
//_____________________________________________________________________________________

void ControlGraph::EditControlGraph() 
{
// This method is invoked from the ContextMenu
   if (!fParent) return;
   TOrdCollection * col_lab = new TOrdCollection;
   col_lab->Add(new TObjString("X"));
   col_lab->Add(new TObjString("Y"));
   col_lab->Add(new TObjString("Shape F"));
//   col_lab->Add(new TObjString("cc"));
   Int_t nrows = GetN();
   Int_t ncols = 3;

   TArrayD values(ncols * nrows);
//   TArrayI flags(nrows);
   Double_t * x = GetX();
   Double_t * y = GetY();
   for(Int_t i = 0; i < nrows; i++) {
      values[i] = x[i];
      values[i + nrows] = y[i];
      values[i + 2 * nrows] = fShapeFactors[i];
   }
   Int_t ret = 0;
   Int_t itemwidth = 100;
   Int_t prec = 3;
   new TGMrbTableOfDoubles(0, &ret, "Edit Spline", itemwidth,
                          ncols, nrows, values, prec, 
                          col_lab);
   if (ret < 0) return;
   for(Int_t i = 0; i < nrows; i++) {
      x[i] = values[i];
      y[i] = values[i + nrows];
      fShapeFactors[i] = values[i + 2 * nrows];
   }
   fParent->ComputeSpline();
}
//_____________________________________________________________________________________

void ControlGraph::ControlGraphMixer() 
{
// This method is invoked from the ContextMenu
   if (!fParent) return;
   TOrdCollection * row_lab = new TOrdCollection;
//   col_lab->Add(new TObjString("cc"));
   Int_t nrows = GetN();
   if (fChangeShapeFactorsConjointly > 0) 
      nrows = 1;

   TArrayI values(nrows);
   TArrayI minval(nrows);
   TArrayI maxval(nrows);
   Double_t * x = GetX();
   Double_t * y = GetY();
//   TArrayI flags(nrows);
   for(Int_t i = 0; i < nrows; i++) {
      values[i] = (Int_t) (100 * fShapeFactors[i]);
      minval[i] = -100;
      maxval[i] =  100;
      Int_t ix = (Int_t)x[i];
      Int_t iy = (Int_t)y[i];
      row_lab->Add(new TObjString(Form("%d, %d", ix, iy)));
   }
   if (fChangeShapeFactorsConjointly > 0) 
      values[0] = (Int_t) (100 * fShapeFactors[1]);
   TGMrbSliders * sliders = new TGMrbSliders("Set Shapefactors (*100)", nrows, 
                       minval.GetArray(), maxval.GetArray(), values.GetArray(),
                       row_lab, NULL, NULL);
   sliders->Connect("SliderEvent(Int_t, Int_t, Int_t)",this->ClassName() , this, 
               "SetShapeFactors(Int_t, Int_t, Int_t)");
}
//_____________________________________________________________________________________

void ControlGraph::SetShapeFactors(Int_t id, Int_t ip, Int_t val) 
{
//   cout << "SetShapeFactors: " << ip << " " << val << endl;
   if (fChangeShapeFactorsConjointly > 0) {
      Int_t from, to;
      if (fParent->IsClosed()) {
         from = 0;
         to = GetN() - 1;
      } else {
         from = 1;
         to = GetN() - 2;
      }
      for (Int_t i = from; i <= to; i++) {
         fShapeFactors[i] = (Float_t)val / 100.;
      }
   } else {
      fShapeFactors[ip] = (Float_t)val / 100.;
   }
   fParent->ComputeSpline();
}
//_____________________________________________________________________________________

TGraph* ControlGraph::AddPGraph(Double_t dist, Color_t color, 
                      Width_t width, Style_t style)
{
   if (fParent) return fParent->AddParallelGraph(dist, color, width, style);
   else         return NULL;
}
//_____________________________________________________________________________________

void ControlGraph::SetChangeShapeFactorsConjointly(Int_t val) {
        fChangeShapeFactorsConjointly = val;}; 

Int_t  ControlGraph::GetChangeShapeFactorsConjointly() {
        return fChangeShapeFactorsConjointly;}; 
//_____________________________________________________________________________________

RailwaySleeper::RailwaySleeper(Double_t * x, Double_t * y, 
                TSplineX * parent, Color_t color)
                : TPolyLine(5, x, y), fParent(parent)
{
   SetLineColor(color);
   SetFillColor(color);
   SetFillStyle(1003);
//   cout << "color: " << color << endl;
}
//_____________________________________________________________________________________
void RailwaySleeper::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
    fParent->ExecuteEvent(event, px,py);
}
//_____________________________________________________________________________________

void RailwaySleeper::Draw(Option_t * opt)
{
   TPolyLine::Draw("F");
}
//_____________________________________________________________________________________

void RailwaySleeper::SetColor(Color_t color)
{
   fColor = color;
   SetFillColor(color);
}
//_____________________________________________________________________________________

void RailwaySleeper::SetSleeperColor(Color_t color)
{
   fColor = color;
   fParent->SetColor(color);
}
//_____________________________________________________________________________________
//_____________________________________________________________________________________
//
//
ParallelGraph::ParallelGraph (TGraph *ograph, Double_t dist, Bool_t closed)
               : fParent(ograph), fDist(dist),fClosed(closed)  
{
   Compute();
}
//_____________________________________________________________________________________

void ParallelGraph::Compute()
{
   Int_t n = fParent->GetN();
   if (fClosed && fParent->GetN() <= 2) {
      cout << "Closed graph must have more then 2 points" << endl;
      return;
   }
   Set(n);
   Double_t phi1, phi2;
   Double_t* xo = fParent->GetX();
   Double_t* yo = fParent->GetY();
   Double_t* xp = this->GetX();
   Double_t* yp = this->GetY();

   if (fClosed) {
      Double_t d = TMath::Sqrt((xo[0]- xo[n-1])*(xo[0]- xo[n-1]) 
                             + (yo[0]- yo[n-1])*(yo[0]- yo[n-1]));
      if (d > 0.001 * TMath::Abs(fDist)) {
         cout << "Closed graph: 1. and last point dont coincide" << endl;
         return;
      }
   }
   if (fClosed) {
      phi1 = TSplineX::PhiOfLine(xo[n-2], yo[n-2], xo[n-1], yo[n-1]);
      phi2 = TSplineX::PhiOfLine(xo[0], yo[0], xo[1], yo[1]);
      TSplineX::Midpoint(phi1, phi2, xo[0], yo[0], fDist, &xp[0], &yp[0]);
      xp[n-1] = xp[0];
      yp[n-1] = yp[0];
   } else {
      phi1 = TSplineX::PhiOfLine(xo[0], yo[0], xo[1], yo[1]);
      TSplineX::Endpoint(phi1, xo[0], yo[0], fDist, &xp[0], &yp[0]);
      phi1 = TSplineX::PhiOfLine(xo[n-2], yo[n-2], xo[n-1], yo[n-1]);
      TSplineX::Endpoint(phi1, xo[n-1], yo[n-1], fDist, &xp[n-1], &yp[n-1]);
   } 
   for (Int_t i = 1; i <= n - 2; i++) {
      phi1 = TSplineX::PhiOfLine(xo[i-1], yo[i-1], xo[i], yo[i]);
      phi2 = TSplineX::PhiOfLine(xo[i], yo[i], xo[i+1], yo[i+1]);
      TSplineX::Midpoint(phi1, phi2, xo[i], yo[i], fDist, &xp[i], &yp[i]);
   }
};
//_____________________________________________________________________________________

void ParallelGraph::CorrectForArrows(Double_t alength, Double_t aangle,Double_t aindent_angle,
                                     Bool_t at_start, Bool_t at_end)
{
//  no arrows with railway sleepers

//   if (fFilledLength > 0 && fEmptyLength > 0) return kTRUE;

   Double_t chop, seglen, xm, ym;
   Int_t ip = 0;
   Double_t deg2rad = TMath::Pi() / 180;
   Double_t p2 = 0.5 * aangle * deg2rad;
   Double_t a2 = aindent_angle *deg2rad;
   Double_t ay = alength * TMath::Tan(p2);
   Double_t dabs = TMath::Abs(fDist);  
//   cout << ay << " " << dabs << endl;
   Double_t reallength = alength  - ay * TMath::Tan(a2) 
           * (1 - dabs / ay);
   Double_t* xp = this->GetX();
   Double_t* yp = this->GetY();
   Int_t n = fParent->GetN();

   if (at_end) {
      chop = reallength ;
      ip = n - 1;
      while (1) {
         seglen = TSplineX::Length(xp[ip], yp[ip], xp[ip-1], yp[ip-1] );
//         cout << ip << " " << yp[ip] << " " << chop << " " << seglen << " " << endl;
         if (chop <= seglen) {
            xm = xp[ip-1] + (xp[ip] - xp[ip-1]) * (1 - chop/seglen);
            ym = yp[ip-1] + (yp[ip] - yp[ip-1]) * (1 - chop/seglen);
//            cout << "yp[ip], ym  " << yp[ip] << " " << ym << endl;
            xp[ip] = xm;
            yp[ip] = ym;
            this->Set(ip+1);
            break;
         } else {
            chop -= seglen;
            ip -= 1;
            if (ip < 1) break;
         }
      }
   }
   xp = this->GetX();
   yp = this->GetY();
   n = this->GetN();
//   cout << "n " << n << endl;
   if (at_start) {
      chop = reallength;
      ip = 0;
      while (1) {
         seglen = TSplineX::Length(xp[ip], yp[ip], xp[ip+1], yp[ip+1] );
//         cout << ip << " " << yp[ip] << " " << chop << " " << seglen << " " << endl;
         if (chop <= seglen) {
            xm = xp[ip] + (xp[ip+1] - xp[ip]) * (chop/seglen);
            ym = yp[ip] + (yp[ip+1] - yp[ip]) * (chop/seglen);
//            cout << "yp[ip], ym  " << yp[ip] << " " << ym << endl;
            xp[ip] = xm;
            yp[ip] = ym;
            break;
         } else {
            chop -= seglen;
            ip += 1;
            if (ip >= n-1) break;
         }
      }
      if (ip > 0) {
         Int_t nn = n - ip;
         for (Int_t i = 0; i < nn ; i++) {
            xp[i] = xp[ip]; 
            yp[i] = yp[ip]; 
            ip++;
         }
//         cout << "nn " << nn << endl;
         this->Set(nn);
      }
   }
   return;
}

//_____________________________________________________________________________________
   
void ParallelGraph::Delete(Option_t *opt)
{
   delete this; 
}
//_____________________________________________________________________________________
   
void ParallelGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
//  execute 
   if (fParent) fParent->ExecuteEvent(event, px, py);
}