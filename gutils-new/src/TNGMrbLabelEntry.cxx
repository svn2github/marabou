
//
//__________________________________________________[C++ CLASS IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           gutils/src/TNGMrbLabelEntry.cxx
// Purpose:        MARaBOU graphic utilities: a labelled entry
// Description:    Implements class methods to handle a labelled entry
//                 Based on TGNumberEntry object (Daniel Sigg)
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TNGMrbLabelEntry.cxx,v 1.5 2009-09-23 10:42:52 Marabou Exp $       
// Date:           
// Layout:         A labelled entry field with up/down/begin/end buttons
//Begin_Html
/*
<img src=gutils/TNGMrbLabelEntry.gif>
*/
//End_Html
//////////////////////////////////////////////////////////////////////////////

#include "TGClient.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGToolTip.h"
#include "TMrbString.h"
#include "TMrbLogger.h"
#include "TNGMrbLabelEntry.h"
#include "TGTableLayout.h"

#include "SetColor.h"

ClassImp(TNGMrbLabelEntry)

extern TMrbLogger * gMrbLog;		// access to message logging


TNGMrbLabelEntry::TNGMrbLabelEntry(	const TGWindow * Parent,
									const Char_t * Label,
									Int_t FrameId,
									TNGMrbProfile * Profile,
									Int_t Width, Int_t Height, Int_t EntryWidth,
									UInt_t EntryOptions,
									TMrbNamedX * Action) :
							TGCompositeFrame(Parent, Width, Height, Profile->GetFrameOptions() | kHorizontalFrame) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry
// Purpose:        Define a label together with an text entry field
// Arguments:      TGWindow * Parent           -- parent window
//                 Char_t * Label              -- label text
//                 TNGMrbProfile * Profile      -- graphics profile
//                 Int_t Width                 -- frame width
//                 Int_t Height                -- frame height
//                 Int_t EntryWidth            -- width of the entry field 
//                 UInt_t EntryOptions         -- options to configure the entry
//                 TMrbNamedX * Action         -- action button
// Results:        --
// Exceptions:
// Description:    Class constructor
// Keywords:
//////////////////////////////////////////////////////////////////////////////



	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();

	TNGMrbGContext * labelGC = Profile->GetGC(TNGMrbGContext::kGMrbGCLabel);
	TNGMrbGContext * entryGC = Profile->GetGC(TNGMrbGContext::kGMrbGCTextEntry);
	TNGMrbGContext * buttonGC = Profile->GetGC(TNGMrbGContext::kGMrbGCButton);

	fEntryWidth = EntryWidth;
	fFrameId=FrameId;
	fLabel = NULL;
	fNumberEntry = NULL;
	fNumberEntryField = NULL;
	fTextEntry = NULL;
	fAction = NULL;
	fButtonBegin = NULL;
	fButtonEnd = NULL;
	fIncrement=-1;
	fShowToolTip=kFALSE;
	fShowRange=kFALSE;

	this->SetForegroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->FG());
	this->SetBackgroundColor(Profile->GetGC(TNGMrbGContext::kGMrbGCGroupFrame)->BG());

	
	tabLayout = new TGTableLayout(this, 2, 6, kFALSE);
	this->SetLayoutManager(tabLayout);
											

//			Number Entry

	if (EntryOptions & kGMrbEntryIsNumber) {
		if (EntryOptions & kGMrbEntryHasUpDownButtons) {

			
			TGNumberEntryField * NumberEntryFieldTemp = new TGNumberEntryField(this, 0., EntryWidth, FrameId);
			Int_t h = NumberEntryFieldTemp->GetDefaultHeight();
   			Int_t charw = NumberEntryFieldTemp->GetCharWidth("0123456789");
			Int_t wdigits = (EntryWidth-8-2*h/3)*10/charw;
   			
			delete NumberEntryFieldTemp;



			fNumberEntry = new TGNumberEntry(this, 0., wdigits, FrameId);
			TO_HEAP(fNumberEntry);
			fNumberEntryField = fNumberEntry->GetNumberEntry();
			fTextEntry = fNumberEntryField;
			
			this->AddFrame(fNumberEntry, new TGTableLayoutHints(2,3,0,2,
        	                       //     kLHintsExpandX|kLHintsExpandY 
        	                      //      |kLHintsShrinkX|kLHintsShrinkY |
					      kLHintsCenterX|kLHintsCenterY
					//    |kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));

			fNumberEntry->GetButtonUp()->SetToolTipText("StepUp", 500);
			fNumberEntry->GetButtonDown()->SetToolTipText("StepDown", 500);

//			EndButton und BeginButton

			if (EntryOptions & kGMrbEntryHasBeginEndButtons) {
				fButtonEnd = new TGPictureButton(this, fClient->GetPicture("arrow_rightright.xpm"), kGMrbEntryButtonEnd);
				TO_HEAP(fButtonEnd);
				fButtonEnd->ChangeBackground(buttonGC->BG());
				fButtonEnd->SetToolTipText("ToEnd", 500);
				fButtonEnd->SetState(kButtonDisabled);
				
				((TQObject*)fButtonEnd)->Connect("Clicked()", "TNGMrbLabelEntry", this, Form("ButtonPressed(Int_t=%d)", 														kGMrbEntryButtonEnd));

				
				fButtonEnd->SetHeight(Height/2-2);
				this->AddFrame(fButtonEnd, new TGTableLayoutHints(3,4,0,1,
        	                        //  kLHintsExpandX|kLHintsExpandY |
        	                            kLHintsShrinkX|kLHintsShrinkY |
					    kLHintsCenterX|kLHintsCenterY
					  //|kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));

				fButtonBegin = new TGPictureButton(this, fClient->GetPicture("arrow_leftleft.xpm"), kGMrbEntryButtonBegin);
				TO_HEAP(fButtonBegin);
				fButtonBegin->ChangeBackground(buttonGC->BG());
				fButtonBegin->SetToolTipText("ToBegin", 500);
				fButtonBegin->SetState(kButtonDisabled);
				
				((TQObject*)fButtonBegin)->Connect("Clicked()", "TNGMrbLabelEntry", this, Form("ButtonPressed(Int_t=%d)", 															kGMrbEntryButtonBegin));

				
				fButtonBegin->SetHeight(Height/2-2);
				this->AddFrame(fButtonBegin, new TGTableLayoutHints(3,4,1,2,
        	                       //   kLHintsExpandX|kLHintsExpandY |
        	                            kLHintsShrinkX|kLHintsShrinkY |
					    kLHintsCenterX|kLHintsCenterY
					 // |kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));

			}
			this->GetButtonUp()->ChangeBackground(buttonGC->BG());
			this->GetButtonDown()->ChangeBackground(buttonGC->BG());


			((TQObject*)(fNumberEntry))->Connect("ValueSet(Long_t)", "TNGMrbLabelEntry", this, "UpDownButtonPressed(Long_t)");
			
			
			
// 			Number Entry Field (without UpDownButtos)
		} else {
			fNumberEntry = NULL;
			fNumberEntryField = new TGNumberEntryField(this, FrameId, 0.);
			fNumberEntryField->SetWidth(EntryWidth);

			TO_HEAP(fNumberEntryField);
			fTextEntry = fNumberEntryField;

			this->AddFrame(fNumberEntryField, new TGTableLayoutHints(2,3,0,2,
        	                       //    kLHintsExpandX|kLHintsExpandY |
        	                        //   kLHintsShrinkX|kLHintsShrinkY |
					     kLHintsCenterX|kLHintsCenterY
					//   |kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));
		}
//			Text Entry
	} else {
		fNumberEntry = NULL;
		fNumberEntryField = NULL;
		fTextEntry = new TGTextEntry(this, new TGTextBuffer(), FrameId);
		fTextEntry->SetWidth(EntryWidth);

		TO_HEAP(fTextEntry);
	
		this->AddFrame(fTextEntry, new TGTableLayoutHints(2,3,0,2,
        	                          //kLHintsExpandX|kLHintsExpandY |
        	                          //kLHintsShrinkX|kLHintsShrinkY |
					    kLHintsCenterX|kLHintsCenterY
					 // |kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));		
	}
///////////////////////////////////////////////Connect NumberEntryField zur Range Abfrage/////////////////////
	if (fNumberEntryField!=NULL){
		
 //		((TGTextEntry*)fNumberEntryField)->Connect("ReturnPressed()", "TNGMrbLabelEntry", this, "FocusLeft()");
 //		((TGTextEntry*)fNumberEntryField)->Connect("ShiftTabPressed()", "TNGMrbLabelEntry", this, "FocusLeft()");
 //		((TGTextEntry*)fNumberEntryField)->Connect("TabPressed()", "TNGMrbLabelEntry", this, "FocusLeft()");
 //		((TGTextEntry*)fNumberEntryField)->Connect("CursorOutUp()", "TNGMrbLabelEntry", this, "FocusLeft()");
 //		((TGFrame*)fNumberEntryField)->Connect("ProcessedEvent(Event_t *event)", "TNGMrbLabelEntry",this,"ProcessedEvent(Event_t *event)");


	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////


//			ActionButton
	if (Action && Action->GetAssignedObject()) {
		fAction = new TGTextButton(this, Action->GetName(), Action->GetIndex());
		TO_HEAP(fAction);
		fAction->SetFont(buttonGC->Font());
		fAction->SetToolTipText(Action->GetTitle(), 500);
		fAction->SetBackgroundColor(buttonGC->BG());
		
		
		this->AddFrame(fAction, new TGTableLayoutHints(4,5,0,2,
        	                      //     kLHintsExpandX|kLHintsExpandY |
        	                      //     kLHintsShrinkX|kLHintsShrinkY |
					     kLHintsCenterX|kLHintsCenterY 
					//   |kLHintsFillX|kLHintsFillY
					     ,1,1,1,1));			
		//fAction->Associate((const TGWindow *) Action->GetAssignedObject());
		((TQObject*)fAction)->Connect("Clicked()", "TNGMrbLabelEntry", this, Form("ButtonPressed(Int_t=%d)", kGMrbEntryButtonAction));
	}

//			Label
	if (Label != NULL) {
		fLabel = new TGLabel(this, new TGString(Label));
		fLabel->SetTextFont(labelGC->Font());
		fLabel->SetForegroundColor(labelGC->FG());
		fLabel->SetBackgroundColor(labelGC->BG());
		fLabel->ChangeOptions(labelGC->GetOptions());
		fLabel->ChangeOptions(fLabel->GetOptions() | kFixedWidth); 
		TO_HEAP(fLabel);

				
		this->AddFrame(fLabel, new TGTableLayoutHints(0,1,0,2,
        	                            kLHintsExpandX|kLHintsExpandY |
        	                            kLHintsShrinkX|kLHintsShrinkY |
					    kLHintsCenterX|kLHintsCenterY
					    |kLHintsFillX|kLHintsFillY
					     ,1,10,1,1));

		fLabel->SetTextJustify(kTextLeft);
	}



	this->SetFont(entryGC->Font());
	this->SetEntryBackground(entryGC->BG());
	

	this->Layout();

	

	
	
}

void TNGMrbLabelEntry::UpDownButtonPressed(Long_t val){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::UpDownButtonPressed
// Purpose:        To decide weather de Up- or DownButton was pressed
// Arguments:      Long_t val: Argument of the TGNumberEntry::ValueSet(Long_t) Signal
//                   
// Results:        
// Exceptions:     
// Description:    To decide weather de Up- or DownButton was pressed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

RealiseIncrement(val);

if (val/10000==0) ButtonPressed(kGMrbEntryButtonUp);
else if(val/10000!=0) ButtonPressed(kGMrbEntryButtonDown);
	
}

void TNGMrbLabelEntry::ButtonPressed(Int_t Button){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::ButtonPressed
// Purpose:        Message handler for up/down/begin/end/action buttons
// Arguments:      Int_t Button    -- Button Type which was pressed
//                   
// Results:        
// Exceptions:     
// Description:    Handle messages reveived from up/down/begin/end/action buttons
// Keywords:       
//////////////////////////////////////////////////////////////////////////////


	switch (Button){
		case kGMrbEntryButtonUp:
		case kGMrbEntryButtonDown:
		
		
			switch(this->GetNumStyle()) {
				case TGNumberEntry::kNESHex:
				this->SetHexNumber((Int_t) this->GetNumber());
									
			}
			break;
		case kGMrbEntryButtonBegin:
		
			if (this->HasBeginEndButtons()) {
				UInt_t l = this->GetNumLimits();
				UInt_t s = this->GetNumStyle();
				if (l == TGNumberEntry::kNELLimitMin || l == TGNumberEntry::kNELLimitMinMax) {
					switch (s) {
						case TGNumberEntry::kNESInteger:
						case TGNumberEntry::kNESDegree:
						case TGNumberEntry::kNESMinSec:
						case TGNumberEntry::kNESHourMin:
						case TGNumberEntry::kNESHourMinSec:
						case TGNumberEntry::kNESDayMYear:
						case TGNumberEntry::kNESMDayYear:
							this->SetIntNumber((Int_t) this->GetNumMin());
							break;
						case TGNumberEntry::kNESHex:
							this->SetHexNumber((Int_t) this->GetNumMin());
							break;
						case TGNumberEntry::kNESRealOne:
						case TGNumberEntry::kNESRealTwo:
						case TGNumberEntry::kNESRealThree:
						case TGNumberEntry::kNESRealFour:
						case TGNumberEntry::kNESReal:
							this->SetNumber(this->GetNumMin());
							break;
					}
				}
			}
			break;
		case kGMrbEntryButtonEnd:
		
			if (this->HasBeginEndButtons()) {
				UInt_t l = this->GetNumLimits();
				UInt_t s = this->GetNumStyle();
				if (l == TGNumberEntry::kNELLimitMax || l == TGNumberEntry::kNELLimitMinMax) {
					switch (s) {
						case TGNumberEntry::kNESInteger:
						case TGNumberEntry::kNESDegree:
						case TGNumberEntry::kNESMinSec:
						case TGNumberEntry::kNESHourMin:
						case TGNumberEntry::kNESHourMinSec:
						case TGNumberEntry::kNESDayMYear:
						case TGNumberEntry::kNESMDayYear:
							this->SetIntNumber((Int_t) this->GetNumMax());
							break;
						case TGNumberEntry::kNESHex:
							this->SetHexNumber((Int_t) this->GetNumMax());
							break;
						case TGNumberEntry::kNESRealOne:
						case TGNumberEntry::kNESRealTwo:
						case TGNumberEntry::kNESRealThree:
						case TGNumberEntry::kNESRealFour:
						case TGNumberEntry::kNESReal:
							this->SetNumber(this->GetNumMax());
							break;
					}
				}
			}
			break;
		case kGMrbEntryButtonAction:
		
		break;
	}
	
	LabelButtonPressed(fFrameId, Button);
	
}

void TNGMrbLabelEntry::LabelButtonPressed(Int_t Id, Int_t Button){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::LabelButtonPressed
// Purpose:        Emit Signal if a Button was pressed
// Arguments:      Int_t Id	   -- Id of the LabelEntry
//		   Int_t Button    -- ButtonId of the Button which was pressed
//                   
// Results:        
// Exceptions:     
// Description:    Emit Signal if a Button was pressed
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Long_t args[2];

	args[0] = Id;
	args[1] = Button;
	
	this->Emit("LabelButtonPressed(Int_t, Int_t)", args);
	this->EntryChanged();
	
}

void TNGMrbLabelEntry::UpDownButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::UpDownButtonEnable
// Purpose:        Enable/disable up/down buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables up/down buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntry) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fNumberEntry->GetButtonUp()->SetState(btnState);
		fNumberEntry->GetButtonDown()->SetState(btnState);
	}
}

void TNGMrbLabelEntry::BeginEndButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::fNumberEntry->GetButtonUp()->SetState(btnState);
// Purpose:        Enable/disable begin/end buttons
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables begin/end buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fButtonBegin) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fButtonBegin->SetState(btnState);
		fButtonEnd->SetState(btnState);
	}
}

void TNGMrbLabelEntry::ActionButtonEnable(Bool_t Flag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::ActionButtonEnable
// Purpose:        Enable/disable action button
// Arguments:      Bool_t Flag      -- kTRUE/kFALSE
// Results:        --
// Exceptions:     
// Description:    Enables/disables action button.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fAction) {
		EButtonState btnState = Flag ? kButtonUp : kButtonDisabled;
		fAction->SetState(btnState);
	}
}

void TNGMrbLabelEntry::SetFormat(TGNumberEntry::EStyle Style, TGNumberEntry::EAttribute Attr) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetFormat
// Purpose:        Set limits
// Arguments:      EStyle Style     -- style
//                 EAttribute Attr  -- attribute
// Results:        --
// Exceptions:     
// Description:    Sets number style & attr
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) fNumberEntryField->SetFormat((TGNumberEntry::EStyle) Style, (TGNumberEntry::EAttribute) Attr);

}

void TNGMrbLabelEntry::SetLimits(TGNumberEntry::ELimit Limits, Double_t Min, Double_t Max) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetLimits
// Purpose:        Set limits
// Arguments:      ELimit Limits   -- limit specifier
//                 Double_t Min    -- min value
//                 Double_t Max    -- max value
// Results:        --
// Exceptions:     
// Description:    Sets limits & enables/disables begin/end buttons.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) {
		fNumberEntryField->SetLimits((TGNumberEntry::ELimit) Limits, Min, Max);
		fNumberEntryField->SetToolTipText(Form("%g ... %g", Min, Max), 500);
		if (fButtonBegin) {
			switch (Limits) {
				case TGNumberEntry::kNELNoLimits:
					fButtonBegin->SetState(kButtonDisabled);
					fButtonEnd->SetState(kButtonDisabled);
					break;
				case TGNumberEntry::kNELLimitMin:
					fButtonBegin->SetState(kButtonUp);
					fButtonEnd->SetState(kButtonDisabled);
					break;
				case TGNumberEntry::kNELLimitMax:
					fButtonBegin->SetState(kButtonDisabled);
					fButtonEnd->SetState(kButtonUp);
					break;
				case TGNumberEntry::kNELLimitMinMax:
					fButtonBegin->SetState(kButtonUp);
					fButtonEnd->SetState(kButtonUp);
					break;
			}
		}
	}
	if (this->GetNumStyle()==TGNumberEntry::kNESHex){this->SetHexNumber((Int_t) this->GetNumber());}
} 

void TNGMrbLabelEntry::SetHexNumber(UInt_t Value) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetHexNumber
// Purpose:        Display in hex format
// Arguments:      UInt_t Value    -- integer value to be displayed
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in hex format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntryField) {
		fNumberEntryField->SetHexNumber((ULong_t) Value);
		TString x = "0x";
		x += fNumberEntryField->GetText();
		fNumberEntryField->TGTextEntry::SetText(x.Data());
	}
}

void TNGMrbLabelEntry::SetOctNumber() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetOctNumber
// Purpose:        Display in octal format
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in octal format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

void TNGMrbLabelEntry::SetBinNumber() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetOctNumber
// Purpose:        Display in binary format
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Displays an integer value in binary format.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

}

void TNGMrbLabelEntry::SetTimeLimit(TGNumberEntry::ELimit Limit, Int_t Hour, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetTimeLimit
// Purpose:        Set time limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Hour      -- hour
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets time limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	Double_t t;
	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryTime)) {
		switch (s) {
			case TGNumberEntry::kNESHourMinSec:
				if (!this->CheckRange(Hour, 0, 24) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 60)) return;
				t = 3600 * Hour + 60 * Min + Sec;
				break;
			case TGNumberEntry::kNESHourMin:
				if (!this->CheckRange(Hour, 0, 24) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 0)) return;
				t = 60 * Hour + Min;
				break;
			case TGNumberEntry::kNESMinSec:
				if (!this->CheckRange(Hour, 0, 0) || !this->CheckRange(Min, 0, 60) || !this->CheckRange(Sec, 0, 60)) return;
				t = 60 * Min + Sec;
				break;
		}
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), t, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), t);
		}
	}
}

void TNGMrbLabelEntry::SetDateLimit(TGNumberEntry::ELimit Limit, Int_t Year, Int_t Month, Int_t Day) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDateLimit
// Purpose:        Set date limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Year      -- year
//                 Int_t Month     -- month
//                 Int_t Day       -- day
// Results:        --
// Exceptions:     
// Description:    Sets time limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDate) && this->CheckRange(Month, 1, 12) && this->CheckRange(Day, 1, 31)) {
		Double_t d = 10000 * Year + 100 * Month + Day;
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), d, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), d);
		}
	}
}

void TNGMrbLabelEntry::SetDegree(Int_t Deg, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDegree
// Purpose:        Set degree value
// Arguments:      Int_t Deg       -- degree
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets degree value.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDegree) && this->CheckRange(Deg, -360, 360) && this->CheckRange(Min, 0, 60) && this->CheckRange(Sec, 0, 60)) {
		Int_t d = 3600 * Deg + 60 * Min + Sec;
		this->SetIntNumber(d);
	}
}

void TNGMrbLabelEntry::SetDegreeLimit(TGNumberEntry::ELimit Limit, Int_t Deg, Int_t Min, Int_t Sec) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetDegreeLimit
// Purpose:        Set degree limit
// Arguments:      ELimit Limit    -- limit specifier (min or max)
//                 Int_t Deg       -- degree
//                 Int_t Min       -- min
//                 Int_t Sec       -- sec
// Results:        --
// Exceptions:     
// Description:    Sets degree limit.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	TGNumberEntry::EStyle s = (TGNumberEntry::EStyle) this->GetNumStyle();
	if (this->CheckStyle(s, kGMrbEntryDegree) && this->CheckRange(Deg, -360, 360) && this->CheckRange(Min, 0, 60) && this->CheckRange(Sec, 0, 60)) {
		Double_t d = 3600 * Deg + 60 * Min + Sec;
		if (Limit == TGNumberEntry::kNELLimitMin) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), d, this->GetNumMax());
		} else if (Limit == TGNumberEntry::kNELLimitMax) {
			this->SetLimits((TGNumberEntry::ELimit) this->GetNumLimits(), this->GetNumMin(), d);
		}
	}
}

void TNGMrbLabelEntry::SetButtonBackground(Pixel_t Color) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::SetButtonBackground
// Purpose:        Change background color for buttons
// Arguments:      Pixel_t Color     -- color
// Results:        --
// Exceptions:     
// Description:    Changes background color.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (fNumberEntry) {
		fNumberEntry->GetButtonUp()->SetBackgroundColor(Color);
		fNumberEntry->GetButtonDown()->SetBackgroundColor(Color);
	}
	if (fButtonBegin) {
		fButtonBegin->SetBackgroundColor(Color);
		fButtonEnd->SetBackgroundColor(Color);
	}

	if (fAction) {
		fAction->SetBackgroundColor(Color);
	}
}

Bool_t TNGMrbLabelEntry::CheckRange(Int_t Value, Int_t LowerLim, Int_t UpperLim) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::CheckRange
// Purpose:        Check range
// Arguments:      --
// Results:        --
// Exceptions:     
// Description:    Checks if value within range.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Value < LowerLim || Value > UpperLim) {
		gMrbLog->Err()	<< "Illegal range - " << Value
						<< " (should be in [" << LowerLim << "," << UpperLim << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckRange");
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TNGMrbLabelEntry::CheckStyle(TGNumberEntry::EStyle Style, EGMrbEntryStyle StyleGroup) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::CheckStyle
// Purpose:        Check style
// Arguments:      EStyle Style                -- style
//                 EGMrbEntryStyle StyleGroup  -- style group
// Results:        --
// Exceptions:     
// Description:    Checks for appropriate style.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	EGMrbEntryStyle sg;
	TString s1, s2;

	switch (Style) {
		case TGNumberEntry::kNESInteger:
		case TGNumberEntry::kNESHex:
			sg = kGMrbEntryInt;
			s1 = "integer";
			break;
		case TGNumberEntry::kNESRealOne:
		case TGNumberEntry::kNESRealTwo:
		case TGNumberEntry::kNESRealThree:
		case TGNumberEntry::kNESRealFour:
		case TGNumberEntry::kNESReal:
			sg = kGMrbEntryReal;
			s1 = "real";
			break;
		case TGNumberEntry::kNESDegree:
			sg = kGMrbEntryDegree;
			s1 = "degree";
			break;
		case TGNumberEntry::kNESHourMinSec:
		case TGNumberEntry::kNESMinSec:
		case TGNumberEntry::kNESHourMin:
			sg = kGMrbEntryTime;
			s1 = "time";
			break;
		case TGNumberEntry::kNESDayMYear:
		case TGNumberEntry::kNESMDayYear:
			sg = kGMrbEntryDate;
			s1 = "date";
			break;
	}
	if (sg != StyleGroup) {
		switch (StyleGroup) {
			case kGMrbEntryInt: 	s2 = "integer"; break;
			case kGMrbEntryReal:	s2 = "real"; break;
			case kGMrbEntryDegree:	s2 = "degree"; break;
			case kGMrbEntryTime:	s2 = "time"; break;
			case kGMrbEntryDate:	s2 = "date"; break;
		}
		gMrbLog->Err()	<< "Illegal number style - " << s2
						<< " (should be \"" << s1 << "\")" << endl;
		gMrbLog->Flush(this->ClassName(), "CheckStyle");
		return(kFALSE);
	}
	return(kTRUE);
}

void TNGMrbLabelEntry::RealiseIncrement(Long_t val){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::RealiseIncrement
// Purpose:        Realise other Increments then the standard Increment
// Arguments:      Long_t val                -- Argument emitted for example from TGNumberEntry::SetValue(Long_t val)
//                 
// Results:        --
// Exceptions:     
// Description:    Realises other Increments then the standard Increment (if fIncrement is negative, the standard Increment is used)
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

  if (fIncrement<0) return;					
  Int_t stepsize = val%100;
  Bool_t down =(val / 10000 != 0);
  

  Int_t sign = down ? 1 : -1;


// Wenn das Limit erreicht wurde, die Zahl beibehalten 

  if (fNumberEntryField->GetNumber()==fNumberEntryField->GetNumMax() || fNumberEntryField->GetNumber()==fNumberEntryField->GetNumMin()) return;




// Addition / Subtraktion des Buttons Rueckgaengig machen

fNumberEntryField->IncreaseNumber((TGNumberFormat::EStepSize)stepsize,sign,(val % 10000 / 100 != 0));

//  if (fNumberEntryField!=NULL){
//    fNumberEntryField->SetNumber(fNumberEntryField->GetNumber()-change);
//  }

// Zahl um fIncrement aendern

if (down) fNumberEntryField->SetNumber(fNumberEntryField->GetNumber()-fIncrement);
else fNumberEntryField->SetNumber(fNumberEntryField->GetNumber()+fIncrement);

return;
}

void TNGMrbLabelEntry::ShowToolTip(Bool_t ShowFlag, Bool_t ShowRange) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::ShowToolTip
// Purpose:        Turn tooltips on/off
// Arguments:      Bool_t ShowFlag    -- kTRUE if tooltips have to be shown
//                 Bool_t ShowRange   -- kTRUE if range has to be shown
// Results:        --
// Exceptions:     
// Description:    Shows number as binary, octal, dec, and hex in tooltip.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	fShowToolTip = ShowFlag;
	fShowRange = ShowRange;
	
	if (fShowToolTip) {
		Bool_t showRange = fShowRange;
		if (fIncrement <= 0) showRange = kFALSE;

		if (fNumberEntryField->GetNumStyle() == TGNumberFormat::kNESInteger) {
//			TMrbString v = fNumberEntry->GetIntNumber();
			Int_t value = fNumberEntryField->GetIntNumber();
//			v.ToInteger(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendInteger((Int_t) fNumberEntryField->GetNumMin(), 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fIncrement, 0, 10);
				ttStr += "...";
				ttStr.AppendInteger((Int_t) fNumberEntryField->GetNumMax(), 0, 10);
				ttStr += "] ";
			}
			TMrbString m;
			if (value < 0) {
				value = -value;
				m = "-";
			} else {
				m = "";
			}
			ttStr += m;
			ttStr.AppendInteger(value, 0, 2);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 8);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 10);
			ttStr += " | " + m;
			ttStr.AppendInteger(value, 0, 16);
			fNumberEntryField->SetToolTipText(ttStr.Data());
		} else if (fNumberEntryField->GetNumStyle() >= TGNumberFormat::kNESRealOne || fNumberEntryField->GetNumStyle()<=TGNumberFormat::kNESReal) {
//			TMrbString v = fNumberEntry->GetText();
			Double_t value = fNumberEntryField->GetNumber();
//			v.ToDouble(value);
			TMrbString ttStr = "";
			if (showRange) {
				ttStr += "[";
				ttStr.AppendDouble(fNumberEntryField->GetNumMin());
				ttStr += "...";
				ttStr.AppendDouble(fIncrement);
				ttStr += "...";
				ttStr.AppendDouble(fNumberEntryField->GetNumMax());
				ttStr += "] ";
			}
			ttStr.AppendDouble(value);
			fNumberEntryField->SetToolTipText(ttStr.Data());
		}
	}
}

/*void TNGMrbLabelEntry::FocusLeft(){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::FocusLeft
// Purpose:        Check Range when Focus leaves the NumberEntryField
// Arguments:      
//                 
// Results:        --
// Exceptions:     
// Description:    Check Range when Focus leaves the NumberEntryField.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////
	TGNumberFormat::ELimit limit=fNumberEntryField->GetNumLimits();
	Double_t max, min;
	max=fNumberEntryField->GetNumMax();
	min=fNumberEntryField->GetNumMin();


	switch(limit){
		case TGNumberFormat::kNELNoLimits: break;
		case TGNumberFormat::kNELLimitMin:
			if (fNumberEntryField->GetNumber()<min) {fNumberEntryField->SetNumber(min);};
			break;
		case TGNumberFormat::kNELLimitMax:
			if (fNumberEntryField->GetNumber()>max) {fNumberEntryField->SetNumber(max);};
			break;
		case TGNumberFormat::kNELLimitMinMax:
			if (fNumberEntryField->GetNumber()<min) {fNumberEntryField->SetNumber(min);};
			if (fNumberEntryField->GetNumber()>max) {fNumberEntryField->SetNumber(max);};
			break;
	}

	if (this->GetNumStyle()==TGNumberEntry::kNESHex){this->SetHexNumber((Int_t) this->GetNumber());}

	this->EntryChanged();
}
*/
void TNGMrbLabelEntry::EntryChanged(Int_t FrameId){
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TNGMrbLabelEntry::EntryChanged
// Purpose:        Signal handler
// Arguments:      Int_t FrameId     -- frame id

// Results:       
// Exceptions:     
// Description:    Emits signal on "Entry Field Changed"
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

   Long_t args[1];

   args[0] = FrameId;


   this->Emit("EntryChanged(Int_t)", args);
}

/*void TNGMrbLabelEntry::ProcessedEvent(Event_t *event){

	if (event->fType== kFocusOut) {this->FocusLeft();cout<<"@@@kFocusOut"<<endl;}
	if (event->fType == kGKeyPress) {this->FocusLeft();cout<<"@@@kGKeyPress"<<endl;}
	if (event->fType == kFocusIn) {this->FocusLeft();cout<<"@@@kFocusIn"<<endl;}
	if (event->fType== kKeyRelease) {this->FocusLeft();cout<<"@@@kKeyRelease"<<endl;}


}*/

