// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TSplineXEditor.h,v 1.3 2006-10-23 15:46:11 Otto.Schaile Exp $
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   TSplineXEditor(const TGWindow *p = 0,
#else
   TSplineXEditor(const TGWindow *p, Int_t id,
#endif
               Int_t width = 140, Int_t height = 30,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~TSplineXEditor();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   void TSplineXEditor::ActivateBaseClassEditors(TClass* cl);
   virtual void SetModel(TObject *obj);
#else
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
#endif

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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   ParallelGraphEditor(const TGWindow *p = 0,
#else
   ParallelGraphEditor(const TGWindow *p, Int_t id,
#endif
               Int_t width = 140, Int_t height = 30,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~ParallelGraphEditor();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,4)
   void ParallelGraphEditor::ActivateBaseClassEditors(TClass* cl);
   virtual void SetModel(TObject *obj);
#else
   virtual void SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
#endif

   // 
   virtual void DoDistance();
   virtual void DoFillToParallel();
   virtual void DoClearToParallel();
   virtual void DoDistParallelEntry();

   ClassDef(ParallelGraphEditor,0)        // SplineX editor
};
#endif

