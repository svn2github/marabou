// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TGedAlignSelect.cxx,v 1.1 2005-03-03 08:10:59 schaileo Exp $
// Author: Marek Biskup, Ilka Antcheva   22/07/03

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGedAlignSelect, TGedAlignPopup                                    //
//                                                                      //
// The TGedAlignPopup is a popup containing buttons to                 //
// select align style.                                                 //
//                                                                      //
// The TGedAlignSelect widget is a button showing selected align      //
// and a little down arrow. When clicked on the arrow the               //
// TGedAlignPopup pops up.                                             //
//                                                                      //
// Selecting a align in this widget will generate the event:           //
// kC_ALIGNSEL, kALI_SELCHANGED, widget id, style.                     //
//                                                                      //
// and the signal:                                                      //
// AlignSelected(Style_t align)                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGedAlignSelect.h"
#include "TGResourcePool.h"
#include "TGPicture.h"
#include "TGToolTip.h"
#include "TGButton.h"
#include "Riostream.h"

ClassImp(TGedAlignSelect)
ClassImp(TGedAlignPopup)

EWidgetMessageTypes kC_ALIGNSEL = (EWidgetMessageTypes)1001, 
                    kALI_SELCHANGED = (EWidgetMessageTypes)2;


struct alignDescription_t {
   const char* filename;
   const char* name;
   int number;
};

static alignDescription_t  Aligns[] = {

   {"align13.gif","13 Top, Left",      13},
   {"align23.gif","23 Top, Middle",    23},
   {"align33.gif","33 Top, Right",     33},
   {"align12.gif","12 Middle, Left",   12},
   {"align22.gif","22 Middle, Middle", 22},
   {"align32.gif","32 Middle, Right",  32},
   {"align11.gif","11 Bottom, Left",   11},
   {"align21.gif","21 Bottom, Middle", 21},
   {"align31.gif","31 Bottom, Right",  31},
   {0, 0, 0}
};

static alignDescription_t* GetAlignByNumber(int number)
{
   for (int i = 0; Aligns[i].filename != 0; i++) {
      if (Aligns[i].number == number)
          return &Aligns[i];
   }
   return 0;
}

//______________________________________________________________________________
TGedAlignPopup::TGedAlignPopup(const TGWindow *p, const TGWindow *m, Style_t alignStyle)
   : TGedPopup(p, m, 30, 30, kDoubleBorder | kRaisedFrame | kOwnBackground,
               GetDefaultFrameBackground())
{
   TGButton *b;
   fCurrentStyle = alignStyle;

   Pixel_t white;
   gClient->GetColorByName("white", white); // white background
   SetBackgroundColor(white);

   SetLayoutManager(new TGTileLayout(this, 1));

   for (int i = 0; Aligns[i].filename != 0; i++) {
      AddFrame(b = new TGPictureButton(this, Aligns[i].filename,
               Aligns[i].number, TGButton::GetDefaultGC()(), kSunkenFrame),
               new TGLayoutHints(kLHintsLeft, 14, 14, 14, 14));
      b->SetToolTipText(Aligns[i].name);
//      cout << Aligns[i].filename << endl;
   }

   Resize(76, 76);
   MapSubwindows();
}

//______________________________________________________________________________
TGedAlignPopup::~TGedAlignPopup()
{
   Cleanup();
}

//______________________________________________________________________________
Bool_t TGedAlignPopup::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   cout << "TGedAlignPopup::ProcessMessage " << GET_MSG(msg) << endl;
   if (GET_MSG(msg) == kC_COMMAND && GET_SUBMSG(msg) == kCM_BUTTON) {
      SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED), 0, parm1);
      EndPopup();
   }

   if (parm2)
      ;              // no warning

   return kTRUE;
}

//______________________________________________________________________________
TGedAlignSelect::TGedAlignSelect(const TGWindow *p, Style_t alignStyle, Int_t id)
   : TGedSelect(p, id)
{
//   cout << "TGedAlignSelect::ctor " << alignStyle << endl;
   fPicture = 0;
   SetPopup(new TGedAlignPopup(gClient->GetDefaultRoot(), this, alignStyle));
   SetAlignStyle(alignStyle);

}

//_____________________________________________________________________________
Bool_t TGedAlignSelect::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
//   cout << "TGedAlignSelect::ProcessMessage " << GET_MSG(msg) << endl;
   if (GET_MSG(msg) == kC_ALIGNSEL && GET_SUBMSG(msg) == kALI_SELCHANGED) {
      SetAlignStyle(parm2);
      SendMessage(fMsgWindow, MK_MSG(kC_ALIGNSEL, kALI_SELCHANGED),
                  fWidgetId, parm2);
   }

   if (parm1)     // no warning
      ;
   return kTRUE;
}

//_____________________________________________________________________________
void TGedAlignSelect::DoRedraw()
{
   TGedSelect::DoRedraw();

   Int_t  x, y;
   UInt_t w, h;

   if (IsEnabled()) {
      // pattern rectangle

      x = fBorderWidth + 2;
      y = fBorderWidth + 2;  // 1;
      h = fHeight - (fBorderWidth * 2) - 4;  // -3;  // 14
      w = h;
      if (fState == kButtonDown) {
         ++x; ++y;
      }
      gVirtualX->DrawRectangle(fId, GetShadowGC()(), x, y, w - 1, h - 1);

      if (fPicture != 0) fPicture->Draw(fId, fDrawGC->GetGC(), x + 1, y + 1);
   } else { // sunken rectangle
      x = fBorderWidth + 2;
      y = fBorderWidth + 2;  // 1;
      w = 42;
      h = fHeight - (fBorderWidth * 2) - 4;  // 3;
      Draw3dRectangle(kSunkenFrame, x, y, w, h);
   }
}

//_____________________________________________________________________________
void TGedAlignSelect::SetAlignStyle(Style_t alignStyle)
{
   // Set align.

   fAlignStyle = alignStyle;
   gClient->NeedRedraw(this);

   if (fPicture) {
      gClient->FreePicture(fPicture);
      fPicture = 0;
   }

   alignDescription_t *md = GetAlignByNumber(fAlignStyle);

//   cout << "TGedAlignSelect::SetAlignStyle " << md->filename << endl;
   if (md) {
      fPicture = gClient->GetPicture(md->filename);
      SetToolTipText(md->name);
   }
   AlignSelected(fAlignStyle);
}

//______________________________________________________________________________
void TGedAlignSelect::SavePrimitive(ofstream &out, Option_t *)
{
   // Save the pattern select widget as a C++ statement(s) on output stream out

   out <<"   TGedAlignSelect *";
   out << GetName() << " = new TGedAlignSelect(" << fParent->GetName()
       << "," << fAlignStyle << "," << WidgetId() << ");" << endl;
}
