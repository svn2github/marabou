#ifndef __TNGMrbProfile_h__
#define __TNGMrbProfile_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/inc/TNGMrbProfile.h
// Purpose:        Define utilities to be used with the MARaBOU GUI
// Description:    Graphic utilities for the MARaBOU GUI.
// Author:         R. Lutter
// Revision:       $Id: TNGMrbProfile.h,v 1.4 2009-09-23 10:42:51 Marabou Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "GuiTypes.h"

#include "TGLayout.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TEnv.h"
#include "TGGC.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile
// Purpose:        A utility to ease use of ROOT's GC mechanisms
// Description:    Handles graphic contexts.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbGContext : public TObject {

	public:
		enum EGMrbButtonType		{	kGMrbTextButton 			= BIT(0),
										kGMrbRadioButton			= BIT(1),
										kGMrbCheckButton			= BIT(2),
										kGMrbPictureButton			= BIT(3)
									};

		enum EGMrbFrameType 		{	kGMrbList	 				= BIT(4),
										kGMrbGroup					= BIT(5)
									};

		enum EGMrbGCType			{	kGMrbGCFrame				= BIT(7),
										kGMrbGCLabel				= BIT(8),
										kGMrbGCButton				= BIT(9),
										kGMrbGCCombo				= BIT(10),
										kGMrbGCEntry				= BIT(11),
										kGMrbGCGroupFrame			= kGMrbGCFrame | kGMrbGroup,
										kGMrbGCTextButton			= kGMrbGCButton | kGMrbTextButton,
										kGMrbGCRadioButton			= kGMrbGCButton | kGMrbRadioButton,
										kGMrbGCCheckButton			= kGMrbGCButton | kGMrbCheckButton,
										kGMrbGCPictureButton		= kGMrbGCButton | kGMrbPictureButton,
										kGMrbGCTextEntry			= kGMrbGCEntry | (kGMrbPictureButton << 1),
										kGMrbGCLBEntry				= kGMrbGCEntry | (kGMrbPictureButton << 2)
							};

	public:
		TNGMrbGContext(const Char_t * Font = "Gui.NormalFont",
					const Char_t * Foreground = "Gui.Foreground",
					const Char_t * Background = "Gui.Background",
					UInt_t Options = 0);

		TNGMrbGContext(const Char_t * Font, Pixel_t Foreground, Pixel_t Background, UInt_t Options = 0);

		TNGMrbGContext(TNGMrbGContext::EGMrbGCType Type, UInt_t Options = 0);

		~TNGMrbGContext() {};						// dtor

		inline TGGC * GCAddr() const { return(fGC); };
		inline GContext_t GC() const { return(fGC->GetGC()); };

		Bool_t SetFont(const Char_t * Font);
		inline void SetFont(FontStruct_t Font) { fFont = Font; };
		inline FontStruct_t Font() const { return(fFont); };
		inline const Char_t * GetFontName() { return(fFontName.Data()); };

		Bool_t SetFG(const Char_t * Foreground);
		inline void SetFG(Pixel_t Foreground) { fForeground = Foreground; };
		inline Pixel_t FG() const { return(fForeground); };
		inline const Char_t * GetForegroundName() { return(fForegroundName.Data()); };

		Bool_t SetBG(const Char_t * Background);
		inline void SetBG(Pixel_t Background) { fBackground = Background; };
		inline Pixel_t BG() const { return(fBackground); };
		inline const Char_t * GetBackgroundName() { return(fBackgroundName.Data()); };

		inline void SetOptions(UInt_t Options) { fOptions = Options; };
		inline UInt_t GetOptions() { return(fOptions); };

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out);
		inline void Print() { Print(cout); };

		inline void Help() const { gSystem->Exec("kdehelp /usr/local/Marabou/doc/html/TNGMrbGContext.html&"); };

	protected:
		TGGC * fGC;
		TString fFontName;
		FontStruct_t fFont;
		TGFont	* fTGFont;
		TString fForegroundName;
		Pixel_t fForeground;
		TString fBackgroundName;
		Pixel_t fBackground;
		UInt_t fOptions;

	ClassDef(TNGMrbGContext, 0)		// [GraphUtils] Describe a graphical layout (colors, fonts, layout hints)
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbProfile
// Purpose:        Graphics profile
// Description:    Handles a list of TNGMrbGContext objects
//                 together with global graphics options.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbProfile : public TNamed {

	public:
		TNGMrbProfile(const Char_t * Name, const Char_t * Title, TEnv * Env = NULL);

		~TNGMrbProfile() {};						// dtor

		Bool_t SetGC(TNGMrbGContext::EGMrbGCType Type, TNGMrbGContext * GC);
		Bool_t SetGC(TNGMrbGContext::EGMrbGCType Type,	const Char_t * Font = "Gui.NormalFont",
										const Char_t * Foreground = "Gui.Foreground",
										const Char_t * Background = "Gui.Background");
		Bool_t SetGC(TNGMrbGContext::EGMrbGCType Type, const Char_t * Font, Pixel_t Foreground, Pixel_t Background);

		TNGMrbGContext * GetGC(TNGMrbGContext::EGMrbGCType Type);

		void SetOptions(TNGMrbGContext::EGMrbGCType Type, UInt_t Options);
		void SetOptions(const Char_t * Type, UInt_t Options);
		UInt_t GetOptions(TNGMrbGContext::EGMrbGCType Type);
		UInt_t GetOptions(const Char_t * Type);

		UInt_t GetFrameOptions();

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, const Char_t * Type = NULL) const;
		inline void Print() const { Print(cout); };

	protected:
		TNGMrbGContext * AddGC(TMrbNamedX * GCSpecs, TEnv * Env = NULL, TNGMrbGContext * DefaultGC = NULL);

	protected:
		TMrbLofNamedX fLofGCs;			// graphics contexts

	ClassDef(TNGMrbProfile, 0)		// [GraphUtils] A graphics profile: a list of graph contexts and options
};

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLofProfiles
// Purpose:        A list of graphics profiles
// Description:    Handles a list of graphics profiles.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TNGMrbLofProfiles : public TMrbLofNamedX {

	public:
		TNGMrbLofProfiles(const Char_t * Name = "LofGraphProfiles", const Char_t * Title = "");

		~TNGMrbLofProfiles() {};						// dtor

		TNGMrbProfile * AddProfile(const Char_t * Name, const Char_t * Title);
		TNGMrbProfile * FindProfile(const Char_t * Name, Bool_t FallBack = kFALSE);

		TNGMrbProfile * SetDefault(const Char_t * DefaultProfile = "standard");
		inline TNGMrbProfile * GetDefault() { return(fDefault); };

		void Print(Option_t * Option) const { TObject::Print(Option); };
		void Print(ostream & Out, const Char_t * ProfileName = NULL) const;
		inline void Print() const { Print(cout); };

	protected:

		TString fRcFile;			// where graph envs are defined
		TEnv * fEnv;				// ptr to environment data
		TNGMrbProfile * fDefault;	// default profile

	ClassDef(TNGMrbLofProfiles, 0)		// [GraphUtils] A list of graphics profiles
};
#endif
