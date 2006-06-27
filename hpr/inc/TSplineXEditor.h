// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TSplineXEditor.h,v 1.1 2006-06-27 11:18:03 Otto.Schaile Exp $
// Author: Carsten Hof 28/07/04

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TSplineXEditor
#define ROOT_TSplineXEditor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TSplineXEditor                                                        //
//                                                                      //
//  Editor for changing SplineX attributes.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGButton
#include "TGWidget.h"
#endif
#ifndef ROOT_TGedFrame
#include "TGedFrame.h"
#endif
#ifndef ROOT_TSplineX
#include "TSplineX.h"
#endif

class TGLabel;
class TGNumberEntry;
class TGTextEntry;
class TGCheckButton;
class TGRadioButton;

class TSplineXEditor : public TGedFrame {

protected:
   TSplineX            *fSplineX;      // SplineX object
   TGCheckButton       *fOpenClose;   // close / open TSplineX
   TGCheckButton       *fMarkerOnOff;  // set Controlpoints visible/unvisible
   TGCheckButton       *fArrowAtStart; // 
   TGCheckButton       *fArrowAtEnd;   // 
   TGCheckButton       *fArrowFill;    // 
   TGNumberEntry       *fArrowLength;    //
   TGNumberEntry       *fArrowAngle;    //
   TGNumberEntry       *fArrowIndentAngle;    //
   TGNumberEntry       *fRailwayGage;    //
   TGNumberEntry       *fFilledLength;    //
   TGNumberEntry       *fEmptyLength;    //
   TGTextButton        *fControlGraphMixer;    // 
   TGTextButton        *fAddParallel;    // 
   TGNumberEntry       *fDistParallelEntry;    //
   Double_t             fDistParallel;
   virtual void ConnectSignals2Slots();

public:
   TSplineXEditor(const TGWindow *p, Int_t id,
               Int_t width = 140, Int_t height = 30,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~TSplineXEditor();
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);

   // slots related to SplineX attributes
   virtual void DoOpenClose(Bool_t on);
   virtual void DoMarkerOnOff(Bool_t on);
   virtual void DoArrowAtStart(Bool_t on);
   virtual void DoArrowAtEnd(Bool_t on);
   virtual void DoArrowFill(Bool_t on);
   virtual void DoArrowLength();
   virtual void DoArrowAngle();
   virtual void DoArrowIndentAngle();
   virtual void DoRailwayGage();
   virtual void DoFilledLength();
   virtual void DoEmptyLength();
   virtual void DoControlGraphMixer();
   virtual void DoAddParallel();
   virtual void DoDistParallelEntry();

   ClassDef(TSplineXEditor,0)        // ParallelGraph editor
};

class ParallelGraphEditor : public TGedFrame {

protected:
   ParallelGraph       *fParallel;          // ParallelGraph object
   TGNumberEntry       *fDistance;          //
   TGTextButton        *fFillToParallel;    // 
   TGTextButton        *fClearToParallel;    // 
   TGNumberEntry       *fDistParallelEntry; //
   Double_t             fDistParallel;
   virtual void ConnectSignals2Slots();

public:
   ParallelGraphEditor(const TGWindow *p, Int_t id,
               Int_t width = 140, Int_t height = 30,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~ParallelGraphEditor();
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);

   // 
   virtual void DoDistance();
   virtual void DoFillToParallel();
   virtual void DoClearToParallel();
   virtual void DoDistParallelEntry();

   ClassDef(ParallelGraphEditor,0)        // SplineX editor
};
#endif

