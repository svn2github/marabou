// @(#)root/html:$Id$
// Author: OttoSchaile <mailto:OttoSchaile@lmu.de>
// * Copyright Marabou
#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TDatime.h"
#include "TH1I.h"
#include "TBenchmark.h"
#include "TStopwatch.h"
#include "TMrbSerialComm.h"
#include "TGMrbValuesAndText.h"
#include "Save2FileDialog.h"
#include "SetColor.h"
#include "mca8000.h"
#include <unistd.h>
#include <stdlib.h>

#define MAX_BAUD_RATE 115200
#define MAX_RECEIVE_BUFFER  1024
#define STATUS_SIZE 20
#define TIME_STAMP_SIZE 8
#define MAX_GAIN_CODE 6
#define MAX_GAIN (16 * 1024)
//#define MIN_GAIN (MAX_GAIN >> MAX_GAIN_CODE)
#define MIN_GAIN (256)
//#define MAX_GROUP (2 << MAX_GAIN_CODE)
#define MAX_GROUP (128)
#define TOTAL_MEMORY_SIZE (32 * 1024)

ClassImp(MCA8000)

MCA8000::MCA8000(TString device)
//_____________________________________________________________________
//                                                                      //
// This programs implements control and readout of a                    //
// Amptek MCA8000a device.                                              //
// The MCA8000a accepts input signals of 0 - +5V or                     //
// 0 - +10 selectable by a hardware switch on the device.               //
// The resolution of the spectrum can be selected by                    //
// software between 256 and 16384 channels.                             //
// The upper 1/32 channels are used for sliding scale                   //
// linearisation hence of e.g. 1024 channels only 992 contain           //
// useful data. Channel data are stored with 32 bits.                   //
// If not more than 65536 counts in any channel                         //
// (of interest) are expected readout time may be halved by             //
// reading only the lower 16 bits of the spectrum.                      //
// Input signals require no gate, the threshold is set by               //
// software.                                                            //
// The preset acquisition time is selectable, 0 indicates no preset     //
// The time can be real (wall clock) of ADC live time                   //
// Data acquisition may be stopped and restarted at any time.           //
// If preset time or spectra should be cleared on restart can           //
// be selected.                                                         //
// For more information on the device please consult:                   //
// http://www.bl.physik.uni-muenchen.de/marabou/htmldoc/mca8000a.html   //
//                                                                      //
// Data should be  stored in a histogram. Number of channels and        //
// axis ranges are determined by the selected resolution of the MCA.    //
// E.g. resolution 1024 gives  Nbins: 992, low edge: 0, upper 992.      //
// The name of the hist is the date/time stamp of the booking time      //
// e.g. mca_20130919_090332, the title may be defined by the user.      //
// Book Histogram may be issued at any time,  Fill Histogram            //
// uses the last booked histogram, if none exists yet it is booked      //
// automatically.                                                       //
// To display the histograms use List Objects in Memory from            //
// HistPresents main menu, use Hist_to_ROOT-File from the               //
//  File popup menu in the canvas to save the histogram to a file       //
//	Caveat:                                                              //
//	The serial connection with the MCA8000a seems not very stable.       //
//	Therefore sometimes several retries are needed to get status         //
//	and data (max 5 retries). In some cases the command must be          //
//	repeated.                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


{
	fSerComm = NULL;
	fStatusOk = 0;
	fSerDev = device;
	fNofBinsCode = 2;       // flag bits 0:2
	fNofBins = 4096; 
	fNofBinsString = "";
	fNofBinsString += fNofBins;   
	fThreshold = 300;
	fTimerFlag = 0;         // flag bit 3, RealTime
	fBaudRateDivisor = 24;  // 4800 Baud
	fSleepTime = 4000;
	fAcqTime = 0;          // 0 sec = inf
	fHist = NULL;
	fData = NULL;
	fBuf0 = NULL;
	fBuf1 = NULL;
	fBufLength = 0;
	fClearElTime = 1;
	fClearSpData = 1;
	fShortRead   = 0;
	SetHistTitle();
	fHistTitleMenu = fHistTitle;
	fUserTitle = 0;
	fVerbose = 0;
	fStopwatch = new TStopwatch;
	fSerComm = new TMrbSerialComm(fSerDev);
	// initial baudrate of MCA8000 = 4800
	Int_t baudrate = 4800;
	fSerComm->Setup(baudrate, 8, TMrbSerialComm::kMrbParityNone, 1, kFALSE, kFALSE);
	fSerComm->Open();
	fSerComm->SetRaw();
	fSerComm->SetSpaceParity();
	fSerComm->ResetRts();
	fSerComm->ResetDtr();
};

//_____________________________________________________________________

MCA8000::~MCA8000()
{
	fSerComm->Close();
}
//____________________________________________________________________

Int_t MCA8000::OpenDevice()

////////////////////////////////////////////////////////////////////////
//
//  Change baudrate to 115200 and try to connect to MCA8000a device
//
////////////////////////////////////////////////////////////////////////
{
	if (fStatusOk == 1 ) {
		printf("WARNING: MCA alredy connected\n");
//		return 20;
	}
	// set baudrate to 115200
//	Int_t baudrate = 4800;
	Int_t baudrate = 115200;
	fBaudRateDivisor = MAX_BAUD_RATE / baudrate;
	SetBaudRate(fBaudRateDivisor);
	// fBaudRateDivisor = 1 for 115200, 24 for 4800
	// time / byte[micro seconds]
	fSleepTime = fBaudRateDivisor * (15000000.0 / MAX_BAUD_RATE);
	UChar_t cdat[20];

	Int_t nread = PrintStatusRaw(cdat);
	if ( nread <= 0 ) { 
		printf("Cannot reach MCA, please check cables and power");
	} 
	if ( nread == 20 ) { 
		UShort_t SerialNr = cdat[0] *256 + cdat[1];
//		UShort_t gain_code = cdat[18] & 0x3; // 3 bits of flags
		printf("SerialNr: %d \n", SerialNr);
		fStatusOk = 1;
	} else {
		printf("Illegal number of status bytes read: %d \n", nread);
	}
	return nread;
}	
//____________________________________________________________________

void MCA8000::PowerOn()

////////////////////////////////////////////////////////////////////////
//
//  Turn power on  
//
////////////////////////////////////////////////////////////////////////
{
	fSerComm->SetRts();
	for (Int_t i=0; i <200; i++) {
		fSerComm->ToggleDtr();
		usleep(250);
	}
	fSerComm->ResetDtr();
	fSerComm->ResetRts();
}
//____________________________________________________________________

void MCA8000::SetBaudRate(Int_t baudRateDivisor)

////////////////////////////////////////////////////////////////////////
//
//  Set baudrate of MCA8000a 
//
////////////////////////////////////////////////////////////////////////
{
	Int_t written = 0;
	UChar_t cmd[5] = {0, 0, 0, baudRateDivisor, 0};
	UChar_t csum =0;
	usleep(100000);	

	for (Int_t i =0; i < 4; i++)
		csum += cmd[i];
	cmd[4] = csum % 256;

	usleep(1000);	
	fSerComm->SetRtsAndDtr();
	for (Int_t i=0; i <5; i++) {
		written += fSerComm->WriteDataRaw(&cmd[i], 1);
//		printf("%d \n", (UInt_t)cmd[i]);
		usleep(5000);	
	}
	fSerComm->SetModemBaudRate(115200 / baudRateDivisor);
	usleep(10000);	
	fSerComm->ResetRts();
	fSerComm->SetDtr();
	usleep(10000);
	if (fVerbose > 0)	
		printf("\nSetBaudRate: %d \n", 115200 / baudRateDivisor);
}
//____________________________________________________________________

Int_t MCA8000::SendCommand(UChar_t * cmd)

////////////////////////////////////////////////////////////////////////
//
//  Send a 5 bytes command
//
////////////////////////////////////////////////////////////////////////
{
	Int_t written = 0;
	usleep(100000);	
	fSerComm->ResetDtr();
	fSerComm->SetRts();
	UChar_t csum =0;
	usleep(10000);	

	for (Int_t i =0; i < 4; i++)
		csum += cmd[i];
	cmd[4] = csum % 256;
	usleep(fSleepTime);	
	for (Int_t i = 0; i < 5; i++ ) {
		if ( fVerbose > 0 )
			printf("%d ", (UInt_t)cmd[i]);
		written += fSerComm->WriteDataRaw(&cmd[i], 1);
		usleep(fSleepTime);
	}
	if ( fVerbose > 0 )
		printf("\n");
	// sleeptime depends on number of bins (resolution)
	usleep(40 * fNofBins);	
//	UChar_t buf[2];
//	fSerComm->ReadDataRaw(buf,2);
	fSerComm->ResetRts();
	usleep(10000);	
	return written;
}
//____________________________________________________________________

Int_t MCA8000::SendControl(UChar_t * data)

////////////////////////////////////////////////////////////////////////
//
//  Send control command 
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {1,0,0,0,0};
	for (Int_t i = 0; i < 3; i++)
			cmd[i+1] = data[i];
	fSerComm->ResetDtr();
	return SendCommand(cmd);
}
//____________________________________________________________________

Int_t MCA8000::SendDataAndChecksum(Int_t group)

////////////////////////////////////////////////////////////////////////
//
//  Request data, first 4 byte are checksum of previous transfer
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {0,0,0,0,0};
	cmd[1] = group & 0xFF;
//	cmd[2] = (group >> 8 ) &0xFF;
//	cmd[3] = fBaudRateDivisor;
	fSerComm->ResetDtr();
	return SendCommand(cmd);
	fSerComm->SetDtr();
}
//____________________________________________________________________

Int_t MCA8000::SendDataGroupAndSN(Int_t group)

////////////////////////////////////////////////////////////////////////
//
//  Request data, first 4 byte are group and serial number of device
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {16,0,0,0,0};
	
//			printf(" %d % d\n", i, (UInt_t)data[i]);
	cmd[1] = group & 0xFF;
//	cmd[2] = (group >> 8 ) &0xFF;
	cmd[3] = fBaudRateDivisor;
	fSerComm->ResetDtr();
	return SendCommand(cmd);
	fSerComm->SetDtr();
}

//____________________________________________________________________

Int_t MCA8000::ReadPacket(UChar_t * data, Int_t nbytes)

////////////////////////////////////////////////////////////////////////
//
//  Read a chunk of nbytes
//
////////////////////////////////////////////////////////////////////////
{
	if ( fVerbose > 0)  
		fStopwatch->Start(kTRUE);
	for (Int_t i = 0; i < nbytes; i++) {
		fSerComm->ToggleDtr();
		if (i < nbytes)
			data[i] = 0;	
		usleep(fSleepTime);
	}
	if ( fVerbose > 0)  {
		printf("ReadPacket after toggle,  nbytes; %d realtime: %f\n",
		  nbytes, fStopwatch->RealTime());
		  fStopwatch->Start(kTRUE);
	}
	Int_t nread = fSerComm->ReadDataRaw(&data[0], nbytes);
	if ( nread  != nbytes ){
		if ( fVerbose > 0 )
			printf("ReadPacket expected: %d got: %d\n", nbytes, nread);
		Int_t nprint = 20;
		if (nprint > nread)
			nprint = nread;
		if ( fVerbose > 0 && nprint > 0) {
			for (Int_t i =0; i <nprint; i++) {
				printf("%x\n", data[i]);
			}
		}
	} else {
		if ( fVerbose > 0)  {
			printf("ReadPacket after Read, realtime: %f\n",
		  fStopwatch->RealTime());
		}
	}
	return nread;
}
//____________________________________________________________________

Int_t MCA8000::ReadData(UChar_t * data, Int_t nbytes, Int_t group )

////////////////////////////////////////////////////////////////////////
//
//  Read nbytes of data for group,
//  group + 0 lower 2 bytes of spectrum data 
//  group + 2 upper 2 bytes
//  print Presettime Realtime and Livetime
//
////////////////////////////////////////////////////////////////////////
{
	usleep(100);
	UChar_t stat[20];
	if (group == 0) {
	GetStatus(stat, group);
	fPresetTime = stat[6] + stat[5] * TMath::Power(2,8) + 
				stat[4] * TMath::Power(2,16);
	fRealTime = stat[10] + stat[9] * TMath::Power(2,8) + 
				stat[8] * TMath::Power(2,16) 
				+ (1. -((Double_t)stat[11])/75.);
	fLiveTime = stat[14] + stat[13] * TMath::Power(2,8) + 
				stat[12] * TMath::Power(2,16)
				+ (1. -((Double_t)stat[15])/75.);
	TDatime da;
	printf("ReadData at: %d PresetT: %f RealT: %f LiveT: %f\n",
			da.GetTime(), fPresetTime, fRealTime, fLiveTime);
	}
//	PrintStatus();
	SendDataAndChecksum(group);
	Int_t remaining = nbytes + 20;  // // Remaining bytes, 20 control words
	UChar_t *buff = data;
	Int_t packSize;           // Packet size
	Int_t nread, total = 0;
	while (remaining) {
		// make sure that packSize < receive buffer size
		packSize = remaining  > MAX_RECEIVE_BUFFER ?
                MAX_RECEIVE_BUFFER : remaining;
		nread = ReadPacket(buff, packSize);
		if (nread != packSize)
			break;
		remaining -= packSize;
		buff += packSize;
		total += nread;
		gSystem->Sleep(1);
	}
	gSystem->Sleep(10);
	return total -20;
}
//____________________________________________________________________

Int_t MCA8000::GetData(UInt_t *data)

////////////////////////////////////////////////////////////////////////
//
//  Get 32 bit spectrum data, check checksum
//  number of bins is determined by choosen  resolution (256, 512 etc)
//
////////////////////////////////////////////////////////////////////////
{
	Int_t runstat = IsRunning();
	if (runstat < 0 ){
		printf("Error getting status, try again\n");
		return -1;
	}
	Int_t ip = 20;   // first 20 bytes are status
	Int_t nchannels = -1;
	UInt_t checksum = 0;
	Int_t save_fClearElTime = fClearElTime;
	Int_t save_fClearSpData = fClearSpData;
	if ( runstat == 1 ){
		fClearElTime = 0;
		fClearSpData = 0;
		StopAcq();
	}
	Int_t nbytes = fNofBins * 2;
	if (fBufLength != nbytes ) {
		if ( !fBuf0 ) { 
			delete [] fBuf0;
			fBuf0 = NULL;
		}
		if ( !fBuf1 ) {
			delete [] fBuf1;
			fBuf1 = NULL;
		}
	}
			
	if ( fBuf0 == NULL ) {
		fBufLength = nbytes + 20;
		fBuf0 = new UChar_t[fBufLength]; // incl 20 control words 
	}
	Int_t nd = ReadData(fBuf0, nbytes, 0);
	if ( nd != nbytes ) {
		if ( fVerbose > 0 ) 
			printf("ReadData failed: %d\n", nd);
		goto finis;
	}
	checksum = 0;
	for (Int_t i=20; i < nd+20; i+=2) {
		checksum += fBuf0[i];
		checksum += fBuf0[i+1];
		if ( fVerbose > 1  && (fBuf0[i] != 0 || fBuf0[i+1] != 0) ) 
			printf("%4d: %4d %4d\n", i, fBuf0[i], fBuf0[i+1] );
	}
	checksum = checksum & 0xFFFF;
	UChar_t stat[20];
	GetStatus(stat, 0, 1);
	UInt_t datachksum;
	datachksum = stat[0] * TMath::Power(2,24) + stat[1] * TMath::Power(2,16)
					+ stat[2] * TMath::Power(2,8) + stat[3];
	if (checksum != datachksum) 
		printf("GetData: checksum error, sum of bytes: %d checksum: % d\n",
				checksum, datachksum);
	if ( fShortRead == 0) {
		if ( fBuf1 == NULL ) {
			fBuf1 = new UChar_t[nbytes + 20]; // 20 control words 
		}
		nd = ReadData(fBuf1, nbytes, 2);
		if ( nd != nbytes ) {
		if ( fVerbose > 0 ) 
			printf("ReadData failed: %d\n", nd);
			goto finis;
		}
		checksum = 0;
		// checksum: all bytes of spectrum data incl last 1/32 (scale lin)
		for (Int_t i=20; i < nd + 20; i+=2) {
			checksum += fBuf1[i];
			checksum += fBuf1[i+1];
			if ( fVerbose > 1  && (fBuf1[i] != 0 || fBuf1[i+1] != 0) ) 
			printf("%4d: %4d %4d\n", i, fBuf1[i], fBuf1[i+1] );
		}
		checksum = checksum & 0xFFFF;
		GetStatus(stat, 2, 1);
		datachksum = stat[0] * TMath::Power(2,24) + stat[1] * TMath::Power(2,16)
					+ stat[2] * TMath::Power(2,8) + stat[3];
	if (checksum != datachksum) 
		printf("GetData: checksum error, sum of bytes: %d checksum: % d\n",
				checksum, datachksum);
	}
	
	nchannels = fNofBins - fNofBins /32; // upper 1/32 channels are used for sliding cal
	for (Int_t ic = 0; ic < nchannels; ic++) {
		if ( fVerbose > 2  && (fBuf0[ip] != 0 || fBuf0[ip+1] != 0) )  { 
			printf("%4d: %4d %4d\n", ic+1, fBuf0[ip], fBuf0[ip+1]);
		}
		data[ic] = fBuf0[ip] + fBuf0[ip+1] * TMath::Power(2,8);
		if  ( fShortRead == 0) {
			if ( fVerbose > 2  && (fBuf1[ip] != 0 || fBuf1[ip+1] != 0) ) 
				printf("%4d %4d: %4d",ic+1, fBuf1[ip],fBuf1[ip+1]);
			data[ic] += (fBuf1[ip] *TMath::Power(2,16) + fBuf1[ip+1] * TMath::Power(2,24));
		}
		ip += 2;
	}
	finis:
	if ( runstat == 1 ){
		StartAcq();
		fClearElTime = save_fClearElTime;
		fClearSpData = save_fClearSpData;
	}
	
	return nchannels;
}
//____________________________________________________________________

Int_t MCA8000::GetStatus(UChar_t * stat, Int_t group, Int_t what)

////////////////////////////////////////////////////////////////////////
//
//  Get 20 bytes of status for group, 
//  what == 0: group and serial number
//          1: data checksum
//
////////////////////////////////////////////////////////////////////////
{
	Int_t retry = 5;
	tryagain:
	if (retry <= 0 ){
		return -1;
	}
	retry--;
	
	if (what == 0) 
		SendDataGroupAndSN(group);
	else
		SendDataAndChecksum(group);
		
	usleep(10000);
	fSerComm->ToggleDtr();
	fSerComm->ResetRts();
	usleep(10000);
	
	for (Int_t i = 0; i < 20; i++) {
		fSerComm->ToggleDtr();
		stat[i] = 0;	
//		gSystem->Sleep(2);
		usleep(fSleepTime);
	}
	Int_t nread = fSerComm->ReadDataRaw(stat, 20);
	
	if (nread != 20) {
		if (fVerbose > 0)
			printf("GetStatus, nread != 20: %d\n", nread);
		goto tryagain;
	}
	UInt_t checkSum = 0;
	for (Int_t i = 0; i < 19; i++) {
		checkSum += stat[i];
	}
	if ( fVerbose > 1) {
		printf("GetStatus nread: %d  checkSum: %d  cdat[19] %d\n\n", 
		nread, checkSum%256, (Int_t)stat[19]);
		printf("DataChkSum_3 = %x\n", stat[0]);
		printf("DataChkSum_2 = %x\n", stat[1]);
		printf("DataChkSum_1 = %x\n", stat[2]);
		printf("DataChkSum_0 = %x\n", stat[3]);
		printf("PresetTime_2 = %x\n", stat[4]);
		printf("PresetTime_1 = %x\n", stat[5]);
		printf("PresetTime_0 = %x\n", stat[6]);
		printf("Battery      = %x\n", stat[7]);
		printf("RealTime_2   = %x\n", stat[8]);
		printf("RealTime_1   = %x\n", stat[9]);
		printf("RealTime_0   = %x\n", stat[10]);
		printf("RealTime_75  = %x\n", stat[11]);
		printf("LiveTime_2   = %x\n", stat[12]);
		printf("LiveTime_1   = %x\n", stat[13]);
		printf("LiveTime_0   = %x\n", stat[14]);
		printf("LiveTime_75  = %x\n", stat[15]);
		printf("Threshold_1  = %x\n", stat[16]);
		printf("Threshold_0  = %x\n", stat[17]);
		printf("Flags        = %x\n", stat[18]);
		printf("CheckSum     = %x\n\n", stat[19]);
	}

	if ( (UChar_t)checkSum%256 != stat[19] ) {
		printf("GetStatus nread: %d  CheckSum error\n", nread);
		goto tryagain;
	}
	return nread;
}
//____________________________________________________________________

Int_t MCA8000::SetGroup(Int_t group) 

////////////////////////////////////////////////////////////////////////
//
//  Set group  (not user callable)
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {17,0,0,1,0};
	if (TOTAL_MEMORY_SIZE <= fNofBins * group) {
		return -1;
	} else {
		cmd[2] = group;
		return SendCommand(cmd);
	}
}
//____________________________________________________________________

Int_t MCA8000::SetAcqTime(UChar_t * data)

////////////////////////////////////////////////////////////////////////
//
//  Set aquisition time  (not user callable)
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {2,0,0,0,0};
	for (Int_t i = 0; i < 3; i++)
			cmd[i+1] = data[i];
//	fSerComm->ResetDtr();
	return SendCommand(cmd);
}
//____________________________________________________________________

Int_t MCA8000::SetPresetTime(Int_t ptime) 

////////////////////////////////////////////////////////////////////////
//
//  Set preset aquisition time, user callable 
//
////////////////////////////////////////////////////////////////////////
{
	fAcqTime = ptime;
	UChar_t dat[3];
	dat[0] = (UChar_t)ptime & 0xff;  
	dat[1] = (UChar_t)(ptime >> 8) & 0xff;
	dat[2] = (UChar_t)(ptime >> 16) & 0xff;; 
	return SetAcqTime(dat);
}
//____________________________________________________________________

Int_t MCA8000::SetNofBins(Int_t nofBins) 

////////////////////////////////////////////////////////////////////////
//
// Set resolution i.e. number of bins in spectrum
// 256 <= nofBins <= 16384
//
////////////////////////////////////////////////////////////////////////
{
	if (nofBins < 256 || nofBins > 16384) 
		return -1;
	if ( nofBins != fNofBins ) {
		if ( fData != NULL ) {
			delete [] fData;
			fData = NULL;
		}
		if ( fBuf0 != NULL ) {
			delete [] fBuf0;
			fBuf0 = NULL;
		}
		if ( fBuf1 != NULL ) {
			delete [] fBuf1;
			fBuf1 = NULL;
		}
	}
	UChar_t dat[3];
	dat[0] = (UChar_t)0;     // 
	dat[1] = (UChar_t)fThreshold & 0xff;    // threshold 0-7
	dat[2] = (UChar_t)(fThreshold >> 8 )& 0xff;     // threshold  8-15
	Int_t gainCode = MAX_GAIN_CODE;
	Int_t gain = (nofBins - 1) / MIN_GAIN;
	while(gain)
	{
		gain >>= 1;
		gainCode--;
	}
	dat[0] = dat[0] | gainCode;
	fNofBins = nofBins;
	fNofBinsCode = gainCode;
	SendControl(dat);
	return fNofBinsCode;
}
//____________________________________________________________________

Int_t MCA8000::DeleteTime() 

////////////////////////////////////////////////////////////////////////
//
//  Reset elapsed times 
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {5,0,1,1,0};
	if (fVerbose > 0) {
		printf("DeleteTime()\n");
	}
	return SendCommand(cmd);
}
//____________________________________________________________________

Int_t MCA8000::DeleteData() 

////////////////////////////////////////////////////////////////////////
//
//  Reset spectrum data 
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t cmd[5] = {5,1,0,1,0};
	if (fVerbose > 0) {
		printf("DeleteData()\n");
	}
	Int_t stat =  SendCommand(cmd);
	usleep(80 * fNofBins);
	return stat;
}
//_____________________________________________________________________

Int_t MCA8000::StartAcq(Int_t acqt, Int_t thresh) 

////////////////////////////////////////////////////////////////////////
//
//  Start acquisition with preset time and threshold
//
////////////////////////////////////////////////////////////////////////
{
	if ( fStatusOk != 1) {
		printf("MCA not correctly connected\n");
		return -1;
	}
	SetNofBins(fNofBins);
	Int_t threshold = fThreshold;
	if ( thresh != 0 ) 
		threshold = thresh;
	Int_t acqtime = fAcqTime;
	if ( acqt != 0 ) {
		acqtime = acqt;
	}
	if ( fClearElTime )
		DeleteTime();           // Reset accumulated time?
	if ( fClearSpData ) 
		DeleteData();
	SetPresetTime(acqtime);
	UChar_t dat[3];
	dat[0] = fNofBinsCode + (fTimerFlag << 3);
	dat[0] |= 0x10;     // flag = start acq
	dat[1] = (UChar_t)threshold & 0xff;    // threshold 0-7
	dat[2] = (UChar_t)(threshold >> 8 )& 0xff;     // threshold  8-15
	TDatime da;
	printf("StartAcq: %d AcqTime: %d Threshold: %d Flag: %x\n", 
	da.GetTime(), acqtime, threshold, dat[0]);
	return SendControl(dat);
}
//____________________________________________________________________

Int_t MCA8000::StopAcq() 

////////////////////////////////////////////////////////////////////////
//
//  Stop acquisition
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t dat[3];
	dat[0] = fNofBinsCode + (fTimerFlag << 3);
	// flag bit 4 =  0 stop acq
	dat[1] = (UChar_t)fThreshold & 0xff;    // threshold 0-7
	dat[2] = (UChar_t)(fThreshold >> 8 )& 0xff;     // threshold  8-15
	if (fVerbose > 0) {
		printf("StopAcq()\n");
	}
	return SendControl(dat);
}
//____________________________________________________________________

Int_t MCA8000::IsRunning()

////////////////////////////////////////////////////////////////////////
//
//  Get running state: 0: stopped, 1 running, -1 error
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t stat[20];
	Int_t nread = GetStatus(stat);
	if (nread == 20) {
		return (stat[18] >> 4) & 0x1; // bit 3 of flag
	} else {
		return -1;
	}
} 
//____________________________________________________________________

TH1I * MCA8000::BookHistogram(const char * hname, const char * htitle) 

////////////////////////////////////////////////////////////////////////
//
// Book histogram with hname and htitle
// hname and htitle are normally NULL, in this case:
// hname is timestamp e.g. mca_20120930_131532
// htitle: MCA, Thr: 30 AqT: 60 Time: 131532
//
////////////////////////////////////////////////////////////////////////
{
	Int_t nbins = fNofBins - fNofBins / 32;    // upper 1/32 channels are useless
	TString name;
	TDatime dt;
	TString ti(Form("%06d",dt.GetTime()));
	TString da(Form("%8d",dt.GetDate()));
	if ( hname ) {
		name = hname;
	} else {
		name = "mca_";
		name += da;
		name += "_";
		name += ti;
	}
	if ( htitle ) {
		fHistTitle = htitle;
	} else if ( fUserTitle == 0 ) {
		SetHistTitle(ti.Data());
	}	
	fHist = new TH1I(name, fHistTitle, nbins, 0, (Double_t)nbins);
	fHist->SetUniqueID(dt.Convert());
	printf("Book histogram name: %s Title: %s\n", fHist->GetName(), fHist->GetTitle());
	return fHist;
}
//____________________________________________________________________

Int_t MCA8000::SaveHistogram() 

////////////////////////////////////////////////////////////////////////
//
// Open dialog to save histogram to file
//
////////////////////////////////////////////////////////////////////////
{
	if ( !fHist ) {
		printf("No hist booked\n");
		return -1;
	}
	Int_t sum = (Int_t)fHist->GetSumOfWeights();
	if ( sum == 0 ) {
		printf("Histogram is empty\n");
		return 0;
	}
	new Save2FileDialog(fHist, NULL, NULL);
	return sum;
}
//____________________________________________________________________

Int_t MCA8000::FillHistogram() 

////////////////////////////////////////////////////////////////////////
//
// Fill last booked histogram, if none exists book default
//
////////////////////////////////////////////////////////////////////////
{
	if ( fHist ) {
		TH1 *hh = (TH1*)gROOT->FindObject(fHist);
		if (hh == NULL) {
			printf("Histogram was deleted, book new one\n");
			fHist = NULL;
		}
	}
	if (fHist && fHist->GetNbinsX() != fNofBins - fNofBins / 32) {
		printf("Histogram has wrong number of bins: %d should be: %d\n",
			fHist->GetNbinsX(), fNofBins - fNofBins / 32);
		printf("Please book new histogram and fill again\n");
		return -1;
	} 
	if ( !fHist )
		fHist = BookHistogram();
	if ( !fData ) {
		fData = new UInt_t[fNofBins];
	}
	if (fVerbose > 0) {
		gBenchmark->Reset();
		gBenchmark->Start("fill_hist");
	}
	Int_t retry = 5;
	Int_t sum = -1;
	while ( retry-- ) {
		Int_t nd = GetData(fData);
		if ( nd == fHist->GetNbinsX() ) {
			UInt_t * ip = fData;
			sum = 0;
			Int_t max_count = -1;
			for (Int_t i = 0; i < nd; i++) {
				Int_t cont = (Int_t)(*ip++);
				fHist->SetBinContent(i+1, cont);
				sum += cont;
				if (cont > max_count)
					max_count = cont;
			}
			SetHistTitle();
			printf("Fill hist:  Name: %s Sum of cont: %d Max count: %d\n", 
			fHist->GetName(), sum, max_count);
			if (fVerbose > 0) {
				gBenchmark->Show("fill_hist");
			}
			break;
		}
	}
	return sum;
}
//____________________________________________________________________

void MCA8000::SetHistTitle(const char * ti) 

////////////////////////////////////////////////////////////////////////
//
// Construct title from current time
//
////////////////////////////////////////////////////////////////////////
{
	fHistTitle = "MCA, Thr: ";
	fHistTitle += fThreshold;
	fHistTitle += " ElapsedT: ";
	fHistTitle += Form("%6.1f", fRealTime);
	fHistTitle += " FillT: ";
	if ( ti ) {
		fHistTitle += ti;
	} else {
		TDatime da;
		fHistTitle += da.GetTime();
	}
}
//____________________________________________________________________

Int_t MCA8000::PrintStatus()

////////////////////////////////////////////////////////////////////////
//
// Print status: Serial number, threshold, times, running state
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t stat[20];		
	Int_t nread = GetStatus(stat);
	
	if (nread < 20) {
		printf("PrintStatus failed nread: %d expected 20\n", nread);
		return nread;
	}
	UShort_t SerialNr = stat[0] *256 + stat[1];
//	UShort_t gain_code = stat[18] & 0x3; // 3 bits of flags
	UShort_t timer_flag = (stat[18] >> 3) & 0x1; // bit 3
	UShort_t startstop_flag = (stat[18] >> 4) & 0x1; // bit 3
	UShort_t thresh = stat[16] *256 + stat[17];
	printf("SerialNr: %d Threshold: %d Use Livetime: %d\n", 
			SerialNr, thresh, timer_flag);
	Double_t ptime = stat[6] + stat[5] * TMath::Power(2,8) + 
				stat[4] * TMath::Power(2,16);
	Double_t rtime = stat[10] + stat[9] * TMath::Power(2,8) + 
				stat[8] * TMath::Power(2,16) 
				+ (1. -((Double_t)stat[11])/75.);
	Double_t ltime = stat[14] + stat[13] * TMath::Power(2,8) + 
				stat[12] * TMath::Power(2,16)
				+ (1. -((Double_t)stat[15])/75.);
	TDatime da;
	printf("WallClock: %d PresetT: %f RealT: %f LiveT: %f\n",
			da.GetTime(), ptime, rtime, ltime);
	Double_t time_left = -1;
	if (ptime > 0 ) {
		if( timer_flag ) 
			time_left = ptime - ltime;
		else
			time_left = ptime - rtime;
	}
	if ( startstop_flag == 0 ) {
		printf("Acquisition stopped");
		if ( ptime > 0 && time_left > 0)
			printf(" time left: %f", time_left);
		printf("\n");
	} else {
		if ( timer_flag &&  time_left == 0) {
			printf("Elapsed livetime reached preset time\n");
		} else if (time_left == 0) {
			printf("Elapsed real time reached preset time\n");
		}  else {
			printf("Acquisition running, time left: %f\n", time_left);
		}
	}
	return (Int_t)time_left;
}
//____________________________________________________________________

Int_t MCA8000::PrintStatusRaw(UChar_t * stat0)

////////////////////////////////////////////////////////////////////////
//
// Print status raw, 20 bytes 
//
////////////////////////////////////////////////////////////////////////
{
	UChar_t stat1[20];
	UChar_t * stat;
	if (stat0 != NULL) 
		stat = stat0;
	else 
		stat = stat1;
	for (Int_t i=0; i < 20; i++)
		stat[i] = 0;
		
	Int_t nread = GetStatus(stat);
	
	printf("\nPrintStatus nread: %d\n", nread);
	if (nread <= 0)
		return nread;
	if ( fVerbose > 0 ) {
		printf("DataChkSum_3 = %x\n", stat[0]);
		printf("DataChkSum_2 = %x\n", stat[1]);
		printf("DataChkSum_1 = %x\n", stat[2]);
		printf("DataChkSum_0 = %x\n", stat[3]);
		printf("PresetTime_2 = %x\n", stat[4]);
		printf("PresetTime_1 = %x\n", stat[5]);
		printf("PresetTime_0 = %x\n", stat[6]);
		printf("Battery      = %x\n", stat[7]);
		printf("RealTime_2   = %x\n", stat[8]);
		printf("RealTime_1   = %x\n", stat[9]);
		printf("RealTime_0   = %x\n", stat[10]);
		printf("RealTime_75  = %x\n", stat[11]);
		printf("LiveTime_2   = %x\n", stat[12]);
		printf("LiveTime_1   = %x\n", stat[13]);
		printf("LiveTime_0   = %x\n", stat[14]);
		printf("LiveTime_75  = %x\n", stat[15]);
		printf("Threshold_1  = %x\n", stat[16]);
		printf("Threshold_0  = %x\n", stat[17]);
		printf("Flags        = %x\n", stat[18]);
		printf("CheckSum     = %x\n", stat[19]);
	}
	return nread;
}
//____________________________________________________________________

void MCA8000::StartGui()

////////////////////////////////////////////////////////////////////////
//
// Build and start a simple graphical user interface
//
////////////////////////////////////////////////////////////////////////
{
	static const Char_t helptext[] =
	"This programs implements control and readout of a\n\
	Amptek MCA8000a device.\n\
	The MCA8000a accepts input signals of 0 - +5V or\n\
	0 - +10 selectable by a hardware switch on the device.\n\
	The resolution of the spectrum can be selected by \n\
	software between 256 and 16384 channels.\n\
	The upper 1/32 channels are used for sliding scale\n\
	linearisation hence of e.g. 1024 channels only 992 contain\n\
	useful data. Channel data are stored with 32 bits.\n\
	If not more than 65536 counts in any channel\n\
	(of interest) are expected readout time may be halved by\n\
	reading only the lower 16 bits of the spectrum.\n\
	Input signals require no gate, the threshold is set by\n\
	software.\n\
	The preset acquisition time is selectable, 0 indicates no preset\n\
	The time can be real (wall clock) of ADC live time\n\
	Data acquisition may be stopped and restarted at any time.\n\
	If preset time or spectra should be cleared on restart can\n\
	be selected.\n\
	For more information on the device please consult:\n\
	http://www.bl.physik.uni-muenchen.de/marabou/htmldoc/mca8000a.html\n\
	\n\
	Data should be  stored in a histogram. Number of channels and\n\
	axis ranges are determined by the selected resolution of the MCA.\n\
	E.g. resolution 1024 gives  Nbins: 992, low edge: 0, upper 992.\n\
	The name of the hist is the date/time stamp of the booking time\n\
	e.g. \"mca_20130919_090332\", the title may be defined by the user.\n\
	\"Book Histogram\" may be issued at any time,  \"Fill Histogram\"\n\
	uses the last booked histogram, if none exists yet it is booked\n\
	automatically.\n\
	To display the histograms use \"List Objects in Memory\" from\n\
	HistPresents main menu, use \"Hist_to_ROOT-File\" from the \n\
	\"File\" popup menu in the canvas to save the histogram to a file\n\
	\n\
	Caveat:\n\
	The serial connection with the MCA8000a seems not very stable.\n\
	Therefore sometimes several retries are needed to get status\n\
	and data (max 5 retries). In some cases the command must be\n\
	repeated.\n\
	"
	;
	fRowLab= new TList();
	Int_t ind = 0;
	
	TString opendev("OpenDevice()");
	TString poweron("PowerOn()");
	TString startacq("StartAcq()");
	TString stopacq("StopAcq()");
	TString bookhist("BookHistogram()");
	TString fillhist("FillHistogram()");
	TString savehist("SaveHistogram()");
	TString printstat("PrintStatus()");
	
	fRowLab->Add(new TObjString("ComboSelect_Nof Bins;256;512;1024;2048;4096;8192;16384"));
	fBidNofBins = ind;
	fValp[ind++] = &fNofBinsString;
	fRowLab->Add(new TObjString("CheckButton+Only 16 bits"));
	fValp[ind++] = &fShortRead;
	fRowLab->Add(new TObjString("PlainIntVal_Acq Time"));
	fBidAcqTime = ind;
	fValp[ind++] = &fAcqTime;
	fRowLab->Add(new TObjString("PlainIntVal+Threshld"));
	fBidThreshold= ind;
	fValp[ind++] = &fThreshold;
	fRowLab->Add(new TObjString("CheckButton_ Use livetime for preset time"));
	fBidTimerFlag = ind;
	fValp[ind++] = &fTimerFlag;
	fRowLab->Add(new TObjString("CheckButton_Clear elapsd time on StartAcq"));
	fValp[ind++] = &fClearElTime;
	fRowLab->Add(new TObjString("CheckButton_Clear spectr data on StartAcq"));
	fValp[ind++] = &fClearSpData;
	fRowLab->Add(new TObjString("StringValue_Hist_tit"));
	fBidTitle = ind;
	fValp[ind++] = &fHistTitleMenu;
	fRowLab->Add(new TObjString("CommandButt_Power On"));
	fValp[ind++] = &poweron;
	fRowLab->Add(new TObjString("CommandButt+Connect MCA"));
	fValp[ind++] = &opendev;
	fRowLab->Add(new TObjString("CommandButt_Start Acqu"));
	fValp[ind++] = &startacq;
	fRowLab->Add(new TObjString("CommandButt+Stop Acqu"));
	fValp[ind++] = &stopacq;
	fRowLab->Add(new TObjString("CommandButt_Book Histogram"));
	fBidBookHist = ind;
	fValp[ind++] = &bookhist;
	fRowLab->Add(new TObjString("CommandButt+Fill Histogram"));
	fBidFillHist = ind;
	fValp[ind++] = &fillhist;
	fRowLab->Add(new TObjString("CommandButt_Save Histogram"));
	fValp[ind++] = &savehist;
	fRowLab->Add(new TObjString("CommandButt+Print Status"));
	fValp[ind++] = &printstat;
	
		Int_t Ok = 0;
	Int_t itemwidth = 250;
	fDialogCmd =
	new TGMrbValuesAndText("MCA8000a Control", NULL, &Ok,itemwidth, NULL,
								  NULL, NULL, fRowLab, fValp,
								  NULL, NULL, helptext, this, this->ClassName());
	fDialogCmd->Move(5,455);

}
//____________________________________________________________________

void MCA8000::CRButtonPressed(Int_t /*wid*/, Int_t bid, TObject */*obj*/)
////////////////////////////////////////////////////////////////////////
//
// Handle button press events 
//
////////////////////////////////////////////////////////////////////////
{
	if ( bid == fBidNofBins ) {
		fNofBins = fNofBinsString.Atoi();
	}
	if ( bid == fBidTitle ) {
		printf("CRButtonPressed bid %d\n", bid);
		fHistTitle = fHistTitleMenu;
		fUserTitle = 1;
	}
}
//____________________________________________________________________

void MCA8000::CloseDown(Int_t /*id*/)

////////////////////////////////////////////////////////////////////////
//
// Handle end of program
//
////////////////////////////////////////////////////////////////////////
{
	
//	printf("CloseDown id %d\n", id);
}
