#ifndef __TGMrbLayout_h__
#define __TGMrbLayout_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TGMrbLayout.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Class:          TGMrbLayout                  -- layout & graphic context
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TGMrbLayout.h,v 1.3 2004-09-28 13:47:33 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"

#include "TGLayout.h"
#include "TSystem.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TGMrbLayout
// Purpose:        A utility to ease use of ROOT's GC mechanisms
// Description:    Handles graphic contexts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TGMrbLayout : public TObject {

	public:
		TGMrbLayout(const Char_t * Font = "Gui.NormalFont",
					const Char_t * Foreground = "black",
					const Char_t * Background = "white",
					TGLayoutHints * Hints = NULL);

		TGMrbLayout(const Char_t * Font, ULong_t Foreground, ULong_t Background, TGLayoutHints * Hints = NULL);

		~TGMrbLayout() {};						// dtor

		inline GContext_t GC() const { return(fGC); };

		Bool_t SetFont(const Char_t * Font);
		inline void SetFont(FontStruct_t Font) { fFont = Font; };
		inline FontStruct_t Font() const { return(fFont); };

		Bool_t SetFG(const Char_t * Foreground);
		inline void SetFG(ULong_t Foreground) { fForeground = Foreground; };
		inline ULong_t FG() const { return(fForeground); };

		Bool_t SetBG(const Char_t * Background);
		inline void SetBG(ULong_t Background) { fBackground = Background; };
		inline ULong_t BG() const { return(fBackground); };

		inline void SetLH(TGLayoutHints * Hints) { fLayoutHints = Hints; };
		inline TGLayoutHints * LH() const { return(fLayoutHints); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TGMrbLayout.html&"); };

	protected:
		GContext_t CreateGC(FontStruct_t Font, ULong_t Foreground, ULong_t Background);

	protected:
		GContext_t fGC;
		FontStruct_t fFont;
		ULong_t fForeground;
		ULong_t fBackground;
		TGLayoutHints * fLayoutHints;

	ClassDef(TGMrbLayout, 0)		// [GraphUtils] Describe a graphical layout (colors, fonts, layout hints)
};

#endif
