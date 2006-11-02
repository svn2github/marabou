#ifndef __TMrbIOSpec_h__
#define __TMrbIOSpec_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec.h
// Purpose:        Class to specify the i/o in an experiment
// Description:
// Author:         R. Lutter
// Revision:       $Id: TMrbIOSpec.h,v 1.2 2006-11-02 12:09:30 Rudolf.Lutter Exp $       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <iostream>

#include "TObject.h"
#include "TString.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbIOSpec
// Purpose:        Specify i/o data
// Description:    Describes how to process input data.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

class TMrbIOSpec : public TObject {

	public:
		enum EMrbInputMode		{	kInputNone			=	0,
											kInputMBS			=	0x1,
											kInputFile			=	0x2,
											kInputTCP			=	kInputMBS | 0x4,
											kInputRoot			=	kInputFile | 0x8,
											kInputLMD			=	kInputMBS | kInputFile | 0x10,
											kInputMED			=	kInputMBS | kInputFile | 0x20,
											kInputList			=	kInputRoot | 0x40,
											kInputSync			=	kInputTCP | 0x80,
											kInputAsync 		=	kInputTCP | 0x100
										};

		enum EMrbOutputMode 	{	kOutputNone 				=	0,
											kOutputMBS					=	0x1,
											kOutputOpen 				=	0x2,
											kOutputWriteRootTree		=	0x4,
											kOutputClose				=	0x8,
											kOutputWriteLMDFormat	=	kOutputMBS | 0x10,
											kOutputWriteMEDFormat	=	kOutputMBS | 0x20
										};

		enum EMrbHistoMode	 	{	kHistoNone			=	0,
											kHistoAdd			=	0x1,
											kHistoSave			=	0x2,
											kHistoClear			=	0x4,
										};

		enum EMrbParamMode		{	kParamNone			=	0,
											kParamReload		=	0x1,
											kParamReloadAscii	=	kParamReload | 0x2,
											kParamLoaded		=	0x4
										};

	public:
		TMrbIOSpec() {								// default ctor
			fTimeStampFlag = kFALSE;
			fInputMode = TMrbIOSpec::kInputNone;
			fOutputMode = TMrbIOSpec::kOutputNone;
			fParamMode = TMrbIOSpec::kParamNone;
			fHistoMode = TMrbIOSpec::kHistoNone;
			fStartEvent = 0;
			fStopEvent = 0;
		};

		~TMrbIOSpec() {};							// default dtor

		void SetInputFile(const Char_t * InputFile, EMrbInputMode InputMode = TMrbIOSpec::kInputNone) {
			fInputFile = InputFile;
			fInputMode = InputMode;
		};
		inline const Char_t * GetInputFile() const { return(fInputFile.Data()); };

		void SetOutputFile(const Char_t * OutputFile, EMrbOutputMode OutputMode = TMrbIOSpec::kOutputNone) {
			fOutputFile = OutputFile;
			fOutputMode = OutputMode;
		};
		inline const Char_t * GetOutputFile() const { return(fOutputFile.Data()); };

		void SetHistoFile(const Char_t * HistoFile, EMrbHistoMode HistoMode = TMrbIOSpec::kHistoNone) {
			fHistoFile = HistoFile;
			fHistoMode = HistoMode;
		};
		inline const Char_t * GetHistoFile() const { return(fHistoFile.Data()); };

		void SetParamFile(const Char_t * ParamFile, EMrbParamMode ParamMode = TMrbIOSpec::kParamNone) {
			fParamFile = ParamFile;
			fParamMode = ParamMode;
		};
		inline const Char_t * GetParamFile() const { return(fParamFile.Data()); };

		Bool_t SetStartStop(TString &, TString &);
		Bool_t SetStartStop(Bool_t TstampFlag, Int_t Start, Int_t Stop) {
			fTimeStampFlag = TstampFlag;
			fStartEvent = Start;
			fStopEvent = Stop;
			return(kTRUE);
		};

		inline Int_t GetStartEvent() const { return(fStartEvent); };
		inline Int_t GetStopEvent() const { return(fStopEvent); };

		Bool_t CheckStartStop(TString &, Int_t &, Bool_t &);
		void ConvertToTimeStamp(TString &, Int_t) const;

		inline Bool_t IsTimeStampMode() const { return(fTimeStampFlag); };
		inline EMrbInputMode GetInputMode() const { return(fInputMode); };
		inline void SetInputMode(TMrbIOSpec::EMrbInputMode Mode) { fInputMode = Mode; };
		inline EMrbOutputMode GetOutputMode() const { return(fOutputMode); };
		inline void SetOutputMode(TMrbIOSpec::EMrbOutputMode Mode) { fOutputMode = Mode; };
		inline EMrbParamMode GetParamMode() const { return(fParamMode); };
		inline void SetParamMode(TMrbIOSpec::EMrbParamMode Mode) { fParamMode = Mode; };
		inline EMrbHistoMode GetHistoMode() const { return(fHistoMode); };
		inline void SetHistoMode(TMrbIOSpec::EMrbHistoMode Mode) { fHistoMode = Mode; };

		void Print(Option_t * Option) const { TObject::Print(Option); }
		void Print(ostream & out = cout) const;				// output current settings

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t fTimeStampFlag;		// kTRUE if start/stop refers to time stamp rather than event count
		Int_t fStartEvent;  		// event number / time stamp to start with
		Int_t fStopEvent;			// event number / time stamp to end with

		EMrbInputMode fInputMode;	// MBS? TCP? File? List?
		TString fInputFile;			// name of input file

		EMrbOutputMode fOutputMode; // open? close? write tree?
		TString fOutputFile;		// name of output file

		EMrbHistoMode fHistoMode; 	// add? save?
		TString fHistoFile; 		// name of histogram file

		EMrbParamMode fParamMode;	// reload?
		TString fParamFile; 		// name of param file

	ClassDef(TMrbIOSpec, 1) 	// [Analyze] I/O specifications
};

#endif
