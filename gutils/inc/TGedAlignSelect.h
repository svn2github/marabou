// @(#)root/ged:$Name: not supported by cvs2svn $:$Id: TGedAlignSelect.h,v 1.1 2005-03-03 08:10:59 schaileo Exp $
// Author: Marek Biskup, Ilka Antcheva   24/07/03

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGedAlignSelect
#define ROOT_TGedAlignSelect

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGedAlignPopup and TGedAlignSelect.                                //
//                                                                      //
// The TGedAlignPopup is a popup containing all diferent styles of     //
// aligns.                                                             //
//                                                                      //
// The TGedAlignSelect widget is a button with align drawn inside     //
// and a little down arrow. When clicked the TGAlignPopup.             //
//                                                                      //
// Selecting a align in this widget will generate the event:           //
// kC_MARKERSEL, kMAR_SELCHANGED, widget id, pixel.                     //
//                                                                      //
// and the signal:                                                      //
// AlignSelected(Style_t align)                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGedPatternSelect
#include "TGedPatternSelect.h"
#endif
class TGedAlignPopup : public TGedPopup {

protected:
   Style_t  fCurrentStyle;

public:
   TGedAlignPopup(const TGWindow *p, const TGWindow *m, Style_t alignStyle);
   virtual ~TGedAlignPopup();

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(TGedAlignPopup,0)  //align select popup
};


class TGedAlignSelect : public TGedSelect {

protected:
   Style_t          fAlignStyle;
   const TGPicture *fPicture;

   virtual void     DoRedraw();

public:
   TGedAlignSelect(const TGWindow *p, Style_t alignStyle, Int_t id);
   virtual ~TGedAlignSelect() { if(fPicture) gClient->FreePicture(fPicture);}

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void           SetAlignStyle(Style_t pattern);
   Style_t        GetAlignStyle() const { return fAlignStyle; }
   virtual void   SavePrimitive(ofstream &out, Option_t *);

   virtual TGDimension GetDefaultSize() const { return TGDimension(38, 21); }

   virtual void AlignSelected(Style_t align = 0) 
                { Emit("AlignSelected(Style_t)", align ? align : GetAlignStyle()); }  // *SIGNAL*

   ClassDef(TGedAlignSelect,0)  // Align selection button
};

#endif
