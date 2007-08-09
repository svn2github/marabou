// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TSplineXEditor.cxx,v 1.1 2007-08-09 13:53:46 Otto.Schaile Exp $
// Author: Carsten Hof   16/08/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TSplineXEditor                                                      //
//                                                                      //
//  Implements GUI for SplineX attributes.                              //
//                                                                      //
//  Check box: 'Marker On/Off' Set ControlGraphsMarker visible/invisible//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/TSplineXEditor.gif">
*/
//End_Html

#include "TGComboBox.h"
#include "TGButton.h"
//#include "TGGroupFrame.h"
#include "TSplineXEditor.h"
#include "TGedEditor.h"
#include "TGedFrame.h"
#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGToolTip.h"
#include "TGLabel.h"
#include "TGClient.h"
#include "TColor.h"
#include "TVirtualPad.h"
#include "TStyle.h"
#include "TClass.h"

ClassImp(TSplineXEditor)
ClassImp(ParallelGraphEditor)

enum ESplineXWid {
   kARROW_START,
   kARROW_END,
   kARROW_FILL,
   kARROW_LENGTH,
   kARROW_ANGLE,
   kARROW_INDENTANGLE,
   kMARKER_ONOFF,
   kOPEN_CLOSE,
   kADDPARALLEL,
   kDISTPARALLEL,
   kCONTROLGRAPHMIXER,
};

//______________________________________________________________________________

#if ROOTVERSION < 51304
TSplineXEditor::TSplineXEditor(const TGWindow *p, Int_t id, Int_t width,
                        Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, id, width, height, options | kVerticalFrame, back)
#else
TSplineXEditor::TSplineXEditor(const TGWindow *p, Int_t width,
                         Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, width, height, options | kVerticalFrame, back)
#endif
{
   // Constructor of SplineX editor.

   fSplineX = 0;

   // TextEntry to change the title
//   MakeTitle("Title");

   // Buttons to change the draw options of the SplineX
   TGGroupFrame *fg0 = new TGGroupFrame(this, "Arrow options");

   TGCompositeFrame *f1 = new TGCompositeFrame(fg0, 60, 20, kHorizontalFrame);
   TGLabel * label = new TGLabel(f1, "<|");
   f1->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 0));
   fArrowAtStart = new TGCheckButton(f1, "",kARROW_START);
   fArrowAtStart->SetToolTipText("Draw an Arrow at Start"); 
   f1->AddFrame(fArrowAtStart, new TGLayoutHints(kLHintsLeft, 1, 1, 0, 3));
   label = new TGLabel(f1, "|>");
   f1->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 0));
   fArrowAtEnd = new TGCheckButton(f1, "",kARROW_END);
   fArrowAtEnd->SetToolTipText("Draw an Arrow At End"); 
   f1->AddFrame(fArrowAtEnd, new TGLayoutHints(kLHintsLeft, 1, 1, 0, 3));

   label = new TGLabel(f1, "Fill");
   f1->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 0));
   fArrowFill = new TGCheckButton(f1, "",kARROW_FILL);
   fArrowFill->SetToolTipText("Fill Arrow"); 
   f1->AddFrame(fArrowFill, new TGLayoutHints(kLHintsLeft, 1, 1, 0, 3));

   fg0->AddFrame(f1, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f2 = new TGCompositeFrame(fg0, 80, 20, kHorizontalFrame);
   label = new TGLabel(f2, "Length");
   f2->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 30, 1, 0, 3));
   fArrowLength = new TGNumberEntry(f2, 10, 4, kARROW_LENGTH);
   fArrowLength->GetNumberEntry()->SetToolTipText("Arrow Length");
   f2->AddFrame(fArrowLength, new TGLayoutHints(kLHintsExpandX, 7, 1, 1, 1));
   fg0->AddFrame(f2, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f3 = new TGCompositeFrame(fg0, 80, 20, kHorizontalFrame);
   label = new TGLabel(f3, "Angle");
   f3->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 35, 1, 0, 3));
   fArrowAngle = new TGNumberEntry(f3, 30, 4, kARROW_ANGLE);
   fArrowAngle->GetNumberEntry()->SetToolTipText("Arrow Opening Angle");
   f3->AddFrame(fArrowAngle, new TGLayoutHints(kLHintsRight, 7, 1, 1, 1));
   fg0->AddFrame(f3, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f4 = new TGCompositeFrame(fg0, 80, 20, kHorizontalFrame);
   label = new TGLabel(f4, "IndentAngle");
   f4->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 3));
   fArrowIndentAngle = new TGNumberEntry(f4, 0., 4, kARROW_INDENTANGLE);
   fArrowIndentAngle->GetNumberEntry()->SetToolTipText("Arrow Indent Angle");
   f4->AddFrame(fArrowIndentAngle, new TGLayoutHints(kLHintsRight, 7, 1, 1, 1));
   fg0->AddFrame(f4, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));
   AddFrame(fg0, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

   TGGroupFrame *fg1 = new TGGroupFrame(this, "Railway options");

   TGCompositeFrame *f6 = new TGCompositeFrame(fg1, 80, 20, kHorizontalFrame);
   label = new TGLabel(f6, "Gage");
   f6->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 38, 1, 0, 3));
   fRailwayGage = new TGNumberEntry(f6, 10, 4, kARROW_LENGTH);
   fRailwayGage->GetNumberEntry()->SetToolTipText("Railway gage");
   f6->AddFrame(fRailwayGage, new TGLayoutHints(kLHintsExpandX, 7, 1, 1, 1));
   fg1->AddFrame(f6, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f7 = new TGCompositeFrame(fg1, 80, 20, kHorizontalFrame);
   label = new TGLabel(f7, "Filled Length");
   f7->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 3));
   fFilledLength = new TGNumberEntry(f7, 10, 4, kARROW_LENGTH);
   fFilledLength->GetNumberEntry()->SetToolTipText("Railway filled Length");
   f7->AddFrame(fFilledLength, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
   fg1->AddFrame(f7, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));


   TGCompositeFrame *f8 = new TGCompositeFrame(fg1, 80, 20, kHorizontalFrame);
   label = new TGLabel(f8, "Blank Length");
   f8->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 1, 0, 3));
   fEmptyLength = new TGNumberEntry(f8, 10, 4, kARROW_LENGTH);
   fEmptyLength->GetNumberEntry()->SetToolTipText("Railway empty Length");
   f8->AddFrame(fEmptyLength, new TGLayoutHints(kLHintsExpandX, 1, 1, 0, 1));
   fg1->AddFrame(f8, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));
   AddFrame(fg1, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
   
   TGCompositeFrame *f9 = new TGCompositeFrame(this, 80, 20, kVerticalFrame);
   fControlGraphMixer = new TGTextButton(f9,"ControlGraph Mixer",kCONTROLGRAPHMIXER);
   fControlGraphMixer->SetToolTipText("Modify Shapefactors of ControlPoints");
   f9->AddFrame(fControlGraphMixer, new TGLayoutHints(kLHintsTop, 5, 1, 0, 3));
   AddFrame(f9, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));
   
   TGCompositeFrame *f10 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   fAddParallel = new TGTextButton(f10,"Add Parallel",kADDPARALLEL);
   fAddParallel->SetToolTipText("Add a parallel line ");
//   fAddParallel->Resize(60, fAddParallel->GetDefaultHeight());
   f10->AddFrame(fAddParallel, new TGLayoutHints(kLHintsLeft, 2, 1, 0, 3));
   label = new TGLabel(f10, "Dist");
   f10->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 0, 3));
   fDistParallelEntry = new TGNumberEntry(f10, 10, 4, kDISTPARALLEL);
   fDistParallelEntry ->GetNumberEntry()->SetToolTipText("Distance to TSplineX");
   f10->AddFrame(fDistParallelEntry , new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));

   AddFrame(f10, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   // CheckBox to activate/deactivate the drawing of the Marker
   TGCompositeFrame *f11 = new TGCompositeFrame(this, 80, 20, kVerticalFrame);
   fOpenClose = new TGCheckButton(f11,"Closed TSplineX",kOPEN_CLOSE);
   fOpenClose->SetToolTipText("Toggle open / closed TSplineX");
   f11->AddFrame(fOpenClose, new TGLayoutHints(kLHintsTop, 5, 1, 0, 3));
   AddFrame(f11, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));


   // CheckBox to activate/deactivate the drawing of the Marker
   TGCompositeFrame *f12 = new TGCompositeFrame(this, 80, 20, kVerticalFrame);
   fMarkerOnOff = new TGCheckButton(f12,"Show Marker",kMARKER_ONOFF);
   fMarkerOnOff->SetToolTipText("Make ControlGraphs Markers visible/invisible");
   f12->AddFrame(fMarkerOnOff, new TGLayoutHints(kLHintsTop, 5, 1, 0, 3));
   AddFrame(f12, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   // initialises the window layout
#if ROOTVERSION < 51304
   MapSubwindows();
   Layout();
   MapWindow();
   TClass *cl = TSplineX::Class();
   TGedElement *ge = new TGedElement;
   ge->fGedFrame = this;
   ge->fCanvas = 0;
   cl->GetEditorList()->Add(ge);
#endif

}

//______________________________________________________________________________

TSplineXEditor::~TSplineXEditor()
{
   // Destructor of graph editor.
#if ROOTVERSION < 51304
   TGFrameElement *el;
   TIter next(GetList());

   while ((el = (TGFrameElement *)next())) {
      if (!strcmp(el->fFrame->ClassName(), "TGCompositeFrame"))
         ((TGCompositeFrame *)el->fFrame)->Cleanup();
   }
   Cleanup();
#endif
}

//______________________________________________________________________________

void TSplineXEditor::ConnectSignals2Slots()
{
   // Connect signals to slots.

   fControlGraphMixer->Connect("Clicked()","TSplineXEditor",this,"DoControlGraphMixer()");
   fOpenClose->Connect("Toggled(Bool_t)","TSplineXEditor",this,"DoOpenClose(Bool_t)");
   fMarkerOnOff->Connect("Toggled(Bool_t)","TSplineXEditor",this,"DoMarkerOnOff(Bool_t)");
   fArrowAtStart->Connect("Toggled(Bool_t)","TSplineXEditor",this,"DoArrowAtStart(Bool_t)");
   fArrowAtEnd->Connect("Toggled(Bool_t)","TSplineXEditor",this,"DoArrowAtEnd(Bool_t)");
   fArrowFill->Connect("Toggled(Bool_t)","TSplineXEditor",this,"DoArrowFill(Bool_t)");
   (fArrowLength->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoArrowLength())");
   (fArrowAngle->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoArrowAngle())");
   (fArrowIndentAngle->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoArrowIndentAngle())");
   (fRailwayGage->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoRailwayGage())");
   (fFilledLength->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoFilledLength())");
   (fEmptyLength->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoEmptyLength())");
   (fArrowLength)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoArrowLength())");
   (fArrowAngle)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoArrowAngle())");
   (fArrowIndentAngle)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoArrowIndentAngle())");
   (fRailwayGage)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoRailwayGage())");
   (fFilledLength)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoFilledLength())");
   (fEmptyLength)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoEmptyLength())");
   (fDistParallelEntry->GetNumberEntry())->Connect("ReturnPressed()","TSplineXEditor",this,"DoDistParallelEntry())");
   (fDistParallelEntry)->Connect("ValueSet(Long_t)","TSplineXEditor",this,"DoDistParallelEntry())");
   (fDistParallelEntry)->Connect("ValueChanged(Long_t)","TSplineXEditor",this,"DoDistParallelEntry())");
   fAddParallel->Connect("Clicked()","TSplineXEditor",this,"DoAddParallel()");

   fInit = kFALSE;  // connect the slots to the signals only once
}

//______________________________________________________________________________

#if ROOTVERSION >= 51304
void TSplineXEditor::SetModel(TObject* obj)
{
   fSplineX = (TSplineX *)obj;
#else
void TSplineXEditor::SetModel(TVirtualPad *pad, TObject *obj, Int_t event)
{
   fSplineX = 0;
   fModel = 0;
   fPad = 0;

   if (obj == 0 || !obj->InheritsFrom(TSplineX::Class())) {
      SetActive(kFALSE);
      return;
   }

   fSplineX = (TSplineX *)obj;
   fModel = obj;
   fPad = pad;
#endif
   // Pick up the used values of graph attributes.
   if (fSplineX->GetIsClosed())         fOpenClose->SetState(kButtonDown);
   else                                 fOpenClose->SetState(kButtonUp);
   if (fSplineX->GetCPDrawn())          fMarkerOnOff->SetState(kButtonDown);
   else                                 fMarkerOnOff->SetState(kButtonUp);
   if (fSplineX->GetArrowFill())        fArrowFill->SetState(kButtonDown);
   else                                 fArrowFill->SetState(kButtonUp);
   if (fSplineX->GetPaintArrowAtStart())fArrowAtStart->SetState(kButtonDown);
   else                                 fArrowAtStart->SetState(kButtonUp);
   if (fSplineX->GetPaintArrowAtEnd())   fArrowAtEnd->SetState(kButtonDown);
   else                                 fArrowAtEnd->SetState(kButtonUp);
   Double_t val;
   val = fSplineX->GetArrowLength();
   fArrowLength->SetNumber(val);
   val = fSplineX->GetArrowAngle();
   fArrowAngle->SetNumber(val);
   val = fSplineX->GetArrowIndentAngle();
   fArrowIndentAngle->SetNumber(val);

   val = fSplineX->GetRailwayGage();
   fRailwayGage->SetNumber(val);
   val = fSplineX->GetFilledLength();
   fFilledLength->SetNumber(val);
   val = fSplineX->GetEmptyLength();
   fEmptyLength->SetNumber(val);

   fDistParallel = 10;
   fDistParallelEntry->SetNumber(fDistParallel);

   if (fInit) ConnectSignals2Slots();
//   SetActive();  // activates this Editor
}
#if ROOTVERSION >= 51304
//______________________________________________________________________________

void TSplineXEditor::ActivateBaseClassEditors(TClass* cl)
{
   // Add editors to fGedFrame and exclude TLineEditor.

//   fGedEditor->ExcludeClassEditor(TAttFill::Class());
   TGedFrame::ActivateBaseClassEditors(cl);
}
#endif
//______________________________________________________________________________

void TSplineXEditor::DoControlGraphMixer()
{
   fSplineX->ControlGraphMixer();
}
//______________________________________________________________________________

void TSplineXEditor::DoAddParallel()
{
   fDistParallel = fDistParallelEntry->GetNumber();
   fSplineX->AddParallelGraph(fDistParallel);
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoMarkerOnOff(Bool_t on)
{
   // Slot for setting markers as visible/invisible.
   if (on) fSplineX->DrawControlPoints();
   else    fSplineX->RemoveControlPoints();
   gPad->Update();
}

//______________________________________________________________________________

void TSplineXEditor::DoOpenClose(Bool_t on)
{
   // Toggle open / closed TSplineX.
   fSplineX->SetIsClosed(on);
   gPad->Update();
}

//______________________________________________________________________________

void TSplineXEditor::DoArrowAtStart(Bool_t on)
{
   fSplineX->SetPaintArrowAtStart(on);
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoArrowAtEnd(Bool_t on)
{
   fSplineX->SetPaintArrowAtEnd(on);
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}

//______________________________________________________________________________

void TSplineXEditor::DoArrowFill(Bool_t on)
{
   fSplineX->SetArrowFill(on);
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoDistParallelEntry()
{
//    std::cout << "DoDistParallelEntry() " << fDistParallelEntry->GetNumber() << std::endl;
//   fSplineX->SetDistParallelEntry(fDistParallelEntry->GetNumber());
    fDistParallel = fDistParallelEntry->GetNumber();
}
//______________________________________________________________________________

void TSplineXEditor::DoArrowLength()
{
//    std::cout << "DoArrowLength() " << fArrowLength->GetNumber() << std::endl;
   fSplineX->SetArrowLength(fArrowLength->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}

//______________________________________________________________________________

void TSplineXEditor::DoArrowAngle()
{
   fSplineX->SetArrowAngle(fArrowAngle->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoArrowIndentAngle()
{
   fSplineX->SetArrowIndentAngle(fArrowIndentAngle->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoRailwayGage()
{
//    std::cout << "DoRailwayGage() " << fRailwayGage->GetNumber() << std::endl;
   fSplineX->SetRailwayGage(fRailwayGage->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoFilledLength()
{
//    std::cout << "DoFilledLength() " << fFilledLength->GetNumber() << std::endl;
   fSplineX->SetFilledLength(fFilledLength->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}
//______________________________________________________________________________

void TSplineXEditor::DoEmptyLength()
{
//    std::cout << "DoEmptyLength() " << fEmptyLength->GetNumber() << std::endl;
   fSplineX->SetEmptyLength(fEmptyLength->GetNumber());
   fSplineX->NeedReCompute();
   fSplineX->Paint();
}

//______________________________________________________________________________

#if ROOTVERSION >= 51304
ParallelGraphEditor::ParallelGraphEditor(const TGWindow *p, Int_t width,
#else
ParallelGraphEditor::ParallelGraphEditor(const TGWindow *p, Int_t id, Int_t width,
#endif
                         Int_t height, UInt_t options, Pixel_t back)
   : TGedFrame(p, width, height, options | kVerticalFrame, back)
{
   // Constructor of SplineX editor.

   fParallel = 0;

   // TextEntry to change the title
//   MakeTitle("Title");

   // Buttons to change the draw options of the SplineX
   TGCompositeFrame *f2 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   TGLabel *label = new TGLabel(f2, "Distance to Parent");
   f2->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 0, 3));
   fDistance = new TGNumberEntry(f2, 10, 4, 1);
   fDistance->GetNumberEntry()->SetToolTipText("Distance to parent");
   f2->AddFrame(fDistance, new TGLayoutHints(kLHintsExpandX, 7, 1, 1, 1));
   this->AddFrame(f2, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f10 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   fFillToParallel = new TGTextButton(f10,"Fill to Parallel", 1);
   fFillToParallel->SetToolTipText("Fill to another Parallel");
//   fFillToParallel->Resize(60, fFillToParallel->GetDefaultHeight());
   f10->AddFrame(fFillToParallel, new TGLayoutHints(kLHintsLeft, 2, 1, 0, 3));
   label = new TGLabel(f10, "Dist");
   f10->AddFrame(label,new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 2, 1, 0, 3));
   fDistParallelEntry = new TGNumberEntry(f10, 10, 4, 1);
   fDistParallelEntry ->GetNumberEntry()->SetToolTipText("Distance to Parallel");
   f10->AddFrame(fDistParallelEntry , new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
   AddFrame(f10, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));

   TGCompositeFrame *f11 = new TGCompositeFrame(this, 80, 20, kHorizontalFrame);
   fClearToParallel = new TGTextButton(f11,"Remove Filling", 1);
   fClearToParallel->SetToolTipText("Remove Filling");
//   fClearToParallel->Resize(60, fClearToParallel->GetDefaultHeight());
   f11->AddFrame(fClearToParallel, new TGLayoutHints(kLHintsLeft, 2, 1, 0, 3));
   AddFrame(f11, new TGLayoutHints(kLHintsTop, 1, 1, 0, 0));
   // initialises the window layout
/*
   MapSubwindows();
   Layout();
   MapWindow();

   TClass *cl = ParallelGraph::Class();
   TGedElement *ge = new TGedElement;
   ge->fGedFrame = this;
   ge->fCanvas = 0;
   cl->GetEditorList()->Add(ge);
*/
}

//______________________________________________________________________________

ParallelGraphEditor::~ParallelGraphEditor()
{
   // Destructor of graph editor.

   TGFrameElement *el;
   TIter next(GetList());

   while ((el = (TGFrameElement *)next())) {
      if (!strcmp(el->fFrame->ClassName(), "TGCompositeFrame"))
         ((TGCompositeFrame *)el->fFrame)->Cleanup();
   }
   Cleanup();
}

//______________________________________________________________________________

void ParallelGraphEditor::ConnectSignals2Slots()
{
   // Connect signals to slots.

   (fDistance->GetNumberEntry())->Connect("ReturnPressed()","ParallelGraphEditor",this,"DoDistance())");
   (fDistance)->Connect("ValueSet(Long_t)","ParallelGraphEditor",this,"DoDistance())");
   (fDistParallelEntry->GetNumberEntry())->Connect("ReturnPressed()","ParallelGraphEditor",this,"DoDistParallelEntry())");
   (fDistParallelEntry)->Connect("ValueSet(Long_t)","ParallelGraphEditor",this,"DoDistParallelEntry())");
   (fDistParallelEntry)->Connect("ValueChanged(Long_t)","ParallelGraphEditor",this,"DoDistParallelEntry())");
   fFillToParallel->Connect("Clicked()","ParallelGraphEditor",this,"DoFillToParallel()");
   fClearToParallel->Connect("Clicked()","ParallelGraphEditor",this,"DoClearToParallel()");

   fInit = kFALSE;  // connect the slots to the signals only once
}

//______________________________________________________________________________

#if ROOTVERSION >= 51304
void ParallelGraphEditor::SetModel(TObject* obj)
{
   fParallel = (ParallelGraph *)obj;
#else
void ParallelGraphEditor::SetModel(TVirtualPad *pad, TObject *obj, Int_t event)
{
   fModel = 0;
   fPad = 0;

   if (obj == 0 || !obj->InheritsFrom(ParallelGraph::Class())) {
      SetActive(kFALSE);
      return;
   }

   fModel = obj;
   fPad = pad;
#endif
   // Pick up the used values of graph attributes.
   Double_t val;
   val = fParallel->GetDist();
   fDistance->SetNumber(val);

   fDistParallel = 0;
   fDistParallelEntry->SetNumber(fDistParallel);

   if (fInit) ConnectSignals2Slots();
//   SetActive();  // activates this Editor
}
#if ROOTVERSION >= 51304
//______________________________________________________________________________

void ParallelGraphEditor::ActivateBaseClassEditors(TClass* cl)
{
   // Add editors to fGedFrame and exclude TLineEditor.

//   fGedEditor->ExcludeClassEditor(TAttFill::Class());
   TGedFrame::ActivateBaseClassEditors(cl);
}
#endif

//______________________________________________________________________________

void ParallelGraphEditor::DoFillToParallel()
{
   fDistParallel = fDistParallelEntry->GetNumber();
   fParallel->FillToSlave(fDistParallel);
}
//______________________________________________________________________________

void ParallelGraphEditor::DoClearToParallel()
{
   fParallel->ClearFillToSlave();
}
//______________________________________________________________________________

void ParallelGraphEditor::DoDistParallelEntry()
{
//    std::cout << "DoDistParallelEntry() " << fDistParallelEntry->GetNumber() << std::endl;
//   fSplineX->SetDistParallelEntry(fDistParallelEntry->GetNumber());
    fDistParallel = fDistParallelEntry->GetNumber();
}
//______________________________________________________________________________

void ParallelGraphEditor::DoDistance()
{
//    std::cout << "DoDistance() " << fDistance->GetNumber() << std::endl;
   fParallel->SetDist(fDistance->GetNumber());
   fParallel->GetParent()->NeedReCompute();
   fParallel->GetParent()->Paint();
}

