// Author: Otto Schaile <http://www.physik.uni-muenchen.de/~Otto.Schaile>
// * Copyright OS

#include "Rtypes.h"
#include "TSystem.h"
#include <TGTextEntry.h>

class TMrbSerialComm;
class TH1I;
class TGMrbValuesAndText;
class TStopwatch;
//______________________________________
//
// MCA8000, 
// This programs implements control and readout of a                    //
// Amptek MCA8000a device.                                              //
//
class MCA8000 : public TObject {
private:
	TMrbSerialComm * fSerComm;   // pointer to class serving the device
	TString fSerDev;         // serial dev name: /dev/ttyS0
	Int_t fStatusOk;         // flag for successful initialisation
	Int_t fNofBinsCode;      // flag bits 0:2 code 
	Int_t fNofBins;          // number of bins in spectrum
	Int_t fBufLength;        // length of buffer 2 * fNofBins + 20
	TString fNofBinsString;  // needed for combo box 
	Int_t fThreshold;        // input signal threshold
	Int_t fTimerFlag;        // flag bit 3, 0: set RealTime 1: livetime
	Int_t fBaudRateDivisor;  // = 1 for 115200; 24 for 4800
	Int_t fSleepTime;        // times needed for 1 byte
	Int_t fAcqTime;          // preset acquistion time
	Int_t fShortRead;        // read only lower 16 bit of spectrum data
	Double_t fPresetTime;    // Preset acquisition time read from MCA
//	Double_t fRealTime;      // Elepased real time read from MCA
	Double_t fLiveTime;      // Elapsed live time read from MCA
	
	TH1I * fHist;            // pointer to latest booked histogram
	UInt_t * fData;          // to keep 32 bit bin values on readout
	UChar_t * fBuf0;         // byte buffer for readout lower 16 bits
	UChar_t * fBuf1;         // byte buffer for readout upper 16 bits
//	TString fHistTitle;      // histogram title
	TString fHistTitleMenu;  // histogram title, variable used in menu
	Int_t fUserTitle;        // > 0 indicates if user changed title
	TStopwatch * fStopwatch; //  measure readout time
	TStopwatch * fStopwatchRun; //  measure run time
	TTimer * fTimer;          // to update run time
	Int_t fRunStatus;
	TString fRunStatusText;
	Double_t fRunTime;        // Accumulated approx runtime, measured by wall clock 
	TGMrbValuesAndText * fDialogCmd;
	TList * fRowLab;
	void * fValp[100];
	TList * fButtonList;
	TGTextEntry *fStatusButton;
	
	Int_t fClearElTime;
	Int_t fClearSpData;
	Int_t fBidNofBins;
	Int_t fBidThreshold;
	Int_t fBidAcqTime;
	Int_t fBidTimerFlag;
	Int_t fBidBookHist;
	Int_t fBidFillHist;
	Int_t fBidTitle;
	Int_t fBidRunStatusText;
	Int_t fBidRunTime;
	Int_t fVerbose;
private:	
	void SetBaudRate(Int_t baudRateDivisor);
	Int_t SendCommand(UChar_t * cmd);
	Int_t SendControl(UChar_t * data);
	Int_t SendDataAndChecksum(Int_t group);
	Int_t SendDataGroupAndSN(Int_t group);
	Int_t ReadPacket(UChar_t * data, Int_t nbytes);
	Int_t ReadData(UChar_t * data, Int_t nbytes, Int_t group );
	Int_t SetAcqTime(UChar_t * data);
public:	
	enum ERunStatus { kUnknown, kConnected, kRunning, kPausing, kStopped, kError };
	Double_t fRealTime;      // Elapsed real time read from MCA
	TString fHistTitle;      // histogram title
public:	
	MCA8000(TString serdev = "/dev/ttyS0");
	~MCA8000();
	
	void PowerOn();
	Int_t OpenDevice();
	TMrbSerialComm * GetSerialComm() { return fSerComm; };
	Int_t GetData(UInt_t *data);
	Int_t GetStatus(UChar_t * cdat, Int_t group = 0, Int_t what = 0);
	Int_t SetPresetTime(Int_t ptime = 60);
	void SetThreshold(Int_t thresh = 300) {fThreshold = thresh;};
	Int_t SetNofBins(Int_t nofbins = 2048); 
	Int_t SetGroup(Int_t group = 0); 
	Int_t DeleteTime(); 
	Int_t DeleteData();
	Int_t StartAcq(Int_t acqtime = 0, Int_t threshold = 0); 
	Int_t StopAcq(); 
	Int_t PrintStatusRaw(UChar_t * stat = NULL);
	Int_t PrintStatus(Int_t what = 0);
	Int_t IsRunning();
	TH1I * BookHistogram(const char * hname = NULL , const char * htitle = NULL);
	Int_t FillHistogram(); 
	Int_t SaveHistogram(); 
	void SetHistTitle(const char * title = NULL);
	void StartGui();
	void HandleTimerEvents();
	void CRButtonPressed(Int_t wid, Int_t bid, TObject *obj);
	void CloseDown(Int_t id);
	void SetVerbose(Int_t verbose) {fVerbose = verbose;};

	ClassDef(MCA8000, 0)
};
