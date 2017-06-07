#ifndef __SIS3302_PROTOS_H__
#define __SIS3302_PROTOS_H__

#include "LwrTypes.h"
#include "sis_3302_database.h"
#include "mapping_database.h"

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Protos.h
//! \brief			Definitions for SIS3302 ADC
//! \details		Prototypes for SIS3302 ADC
//! $Author: Marabou $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.10 $
//! $Date: 2011-12-08 10:04:49 $
////////////////////////////////////////////////////////////////////////////*/

/*! Alloc database for SIS3302 module */
s_sis_3302 * sis3302_alloc(Char_t * ModuleName, s_mapDescr * MD, Int_t Serial);

/*! Map VME address */
volatile char * sis3302_mapAddress_sized(s_sis_3302 * Module, Int_t Offset, Int_t SegSize);
volatile char * sis3302_MapAddress(s_sis_3302 * Module, Int_t Offset);

/*! Get module info */
void sis3302_moduleInfo(s_sis_3302 * Module);

/*! Fill SIS3302 database from file */
Bool_t sis3302_fillStruct(s_sis_3302 * Module, Char_t * SettingsFile);

/*! Load settings from database */
void sis3302_loadFromDb(s_sis_3302 * Module);

/*! Set pattern of active channels */
/*! ...FromDb: generate pattern from trigger mode bits */

void sis3302_setActiveChannels(s_sis_3302 * Module, UInt_t ChnPattern);
void sis3302_setActiveChannelsFromDb(s_sis_3302 * Module);

/*! Set max buffer size */
/*! >>> MBS can't handle 8 MB buffers <<< */

void sis3302_setBufferSize(s_sis_3302 * Module, Int_t BufferSize);

/*! Debugging tools: dump settings, print_t database contents */
Bool_t sis3302_dumpRegisters(s_sis_3302 * Module, Char_t * File);

/*! Read dac values */
Bool_t sis3302_readDac(s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo);
/*! Write dac values */
Bool_t sis3302_writeDac(s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo);
Bool_t sis3302_writeDac_db(s_sis_3302 * Module, Int_t ChanNo);

/*! Switch user LED on/off */
Bool_t sis3302_setUserLED(s_sis_3302 * Module, Bool_t OnFlag);

/*! Exec KEY command */
Bool_t sis3302_keyAddr(s_sis_3302 * Module, Int_t Key);

/*! Exec KEY command: reset */
Bool_t sis3302_reset(s_sis_3302 * Module);

/*! Exec KEY command: reset sample */
Bool_t sis3302_resetSampling(s_sis_3302 * Module);

/*! Exec KEY command: generate trigger */
Bool_t sis3302_fireTrigger(s_sis_3302 * Module);

/*! Exec KEY command: clear time stamp */
Bool_t sis3302_clearTimestamp(s_sis_3302 * Module);

/*! Exec KEY command: arm sampling */
Bool_t sis3302_armSampling(s_sis_3302 * Module, Int_t Sampling);

/*! Exec KEY command: disarm sampling */
Bool_t sis3302_disarmSampling(s_sis_3302 * Module);

/*! set/restore trace length */
void sis3302_setTracingMode(s_sis_3302 * Module, Bool_t OnFlag);
void sis3302_adjustTraceLength(s_sis_3302 * Module);
void sis3302_restoreTraceLength(s_sis_3302 * Module);

/*! set fifo limit */
void sis3302_setEndAddress(s_sis_3302 * Module, Int_t NofEvents);

/*! start/stop acquisition */
void sis3302_startAcquisition(s_sis_3302 * Module, Int_t NofEvents);
void sis3302_stopAcquisition(s_sis_3302 * Module);

/*! control and status */
UInt_t sis3302_readControlStatus(s_sis_3302 * Module);
Bool_t sis3302_writeControlStatus(s_sis_3302 * Module, UInt_t Bits);
Bool_t sis3302_writeControlStatus_db(s_sis_3302 * Module);

/*! event configuration */
UInt_t sis3302_readEventConfig(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEventConfig(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
UInt_t sis3302_readEventExtendedConfig(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEventExtendedConfig(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

/*! event header & group id */
Bool_t sis3302_setHeaderBits(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setHeaderBits_db(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getHeaderBits(s_sis_3302 * Module, Int_t ChanNo);

UInt_t sis3302_getGroupId(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger mode */
Bool_t sis3302_setTriggerMode(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setTriggerMode_db(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getTriggerMode(s_sis_3302 * Module, Int_t ChanNo);

/*! gate mode */
Bool_t sis3302_setGateMode(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setGateMode_db(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getGateMode(s_sis_3302 * Module, Int_t ChanNo);

/*! next neighbors, trigger & gate */
Bool_t sis3302_setNextNeighborTriggerMode(s_sis_3302 * Module, Int_t Bits, Int_t ChanNo);
Bool_t sis3302_setNextNeighborTriggerMode_db(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getNextNeighborTriggerMode(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setNextNeighborGateMode(s_sis_3302 * Module, Int_t Bits, Int_t ChanNo);
Bool_t sis3302_setNextNeighborGateMode_db(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getNextNeighborGateMode(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger polarity */
Bool_t sis3302_setPolarity(s_sis_3302 * Module, Bool_t InvertFlag, Int_t ChanNo);
Bool_t sis3302_setPolarity_db(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_polarityIsInverted(s_sis_3302 * Module, Int_t ChanNo);

/*! end address threshold */
Int_t sis3302_readEndAddrThresh(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEndAddrThresh(s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo);
Bool_t sis3302_writeEndAddrThresh_db(s_sis_3302 * Module, Int_t ChanNo);

/*! pre-trigger delay and gate length */
Int_t sis3302_readPreTrigDelay(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelay(s_sis_3302 * Module, Int_t Delay, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelay_db(s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readTrigGateLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTrigGateLength(s_sis_3302 * Module, Int_t Gate, Int_t ChanNo);
Bool_t sis3302_writeTrigGateLength_db(s_sis_3302 * Module, Int_t ChanNo);

/*! raw data sample length & start */
Int_t sis3302_readRawDataSampleLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataSampleLength(s_sis_3302 * Module, Int_t Sample, Int_t ChanNo);
Bool_t sis3302_writeRawDataSampleLength_db(s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readRawDataStartIndex(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataStartIndex(s_sis_3302 * Module, Int_t Index, Int_t ChanNo);
Bool_t sis3302_writeRawDataStartIndex_db(s_sis_3302 * Module, Int_t ChanNo);

/*! sample address */
Int_t sis3302_readNextSampleAddr(s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readPrevBankSampleAddr(s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_readActualSample(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger peak & gap */
Bool_t sis3302_readTriggerPeakAndGap(s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo);
Bool_t sis3302_writeTriggerPeakAndGap(s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t ChanNo);
Bool_t sis3302_writeTriggerPeakAndGap_db(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger out pulse length */
Int_t sis3302_readTriggerPulseLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerPulseLength(s_sis_3302 * Module, Int_t PulseLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerPulseLength_db(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger internal gate & delay */
Int_t sis3302_readTriggerInternalGate(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalGate(s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalGate_db(s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readTriggerInternalDelay(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalDelay(s_sis_3302 * Module, Int_t DelayLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalDelay_db(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger decimation */
Int_t sis3302_getTriggerDecimation(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerDecimation(s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo);
Bool_t sis3302_setTriggerDecimation_db(s_sis_3302 * Module, Int_t ChanNo);

/*! trigger threshold */
Int_t sis3302_readTriggerThreshold(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshold(s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshold_db(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_getTriggerGT(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerGT(s_sis_3302 * Module, Bool_t GTFlag, Int_t ChanNo);
Bool_t sis3302_setTriggerGT_db(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_getTriggerOut(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerOut(s_sis_3302 * Module, Bool_t TrigOutFlag, Int_t ChanNo);
Bool_t sis3302_setTriggerOut_db(s_sis_3302 * Module, Int_t ChanNo);

/*! energy peak & gap */
Bool_t sis3302_readEnergyPeakAndGap(s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo);
Bool_t sis3302_writeEnergyPeakAndGap(s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t ChanNo);
Bool_t sis3302_writeEnergyPeakAndGap_db(s_sis_3302 * Module, Int_t ChanNo);

/*! energy decimation */
Int_t sis3302_getEnergyDecimation(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setEnergyDecimation(s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo);
Bool_t sis3302_setEnergyDecimation_db(s_sis_3302 * Module, Int_t ChanNo);

/*! energy gate length */
Int_t sis3302_readEnergyGateLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateLength(s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateLength_db(s_sis_3302 * Module, Int_t ChanNo);

/*! energy sample length & start */
Int_t sis3302_readEnergySampleLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergySampleLength(s_sis_3302 * Module, Int_t SampleLength, Int_t ChanNo);
Bool_t sis3302_writeEnergySampleLength_db(s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readStartIndex(s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo);
Bool_t sis3302_writeStartIndex(s_sis_3302 * Module, Int_t IdxVal, Int_t IdxNo, Int_t ChanNo);
Bool_t sis3302_writeStartIndex_db(s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo);

/*! test bits */
UInt_t sis3302_getTestBits(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTestBits(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setTestBits_db(s_sis_3302 * Module, Int_t ChanNo);

/*! energy tau factor */
Int_t sis3302_readTauFactor(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTauFactor(s_sis_3302 * Module, Int_t Tau, Int_t ChanNo);
Bool_t sis3302_writeTauFactor_db(s_sis_3302 * Module, Int_t ChanNo);

/*! clock source */
Int_t sis3302_getClockSource(s_sis_3302 * Module);
Bool_t sis3302_setClockSource(s_sis_3302 * Module, Int_t ClockSource);
Bool_t sis3302_setClockSource_db(s_sis_3302 * Module);

/*! lemo in & out */
Int_t sis3302_getLemoInMode(s_sis_3302 * Module);
Bool_t sis3302_setLemoInMode(s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoInMode_db(s_sis_3302 * Module);
Int_t sis3302_getLemoOutMode(s_sis_3302 * Module);
Bool_t sis3302_setLemoOutMode(s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoOutMode_db(s_sis_3302 * Module);
Int_t sis3302_getLemoInEnableMask(s_sis_3302 * Module);
Bool_t sis3302_setLemoInEnableMask(s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoInEnableMask_db(s_sis_3302 * Module);

/*! trigger feedback */
Bool_t sis3302_getTriggerFeedback(s_sis_3302 * Module);
Bool_t sis3302_setTriggerFeedback(s_sis_3302 * Module, Bool_t Feedback);
Bool_t sis3302_setTriggerFeedback_db(s_sis_3302 * Module);

/*! read traces and/or energy data */
Int_t sis3302_readout(s_sis_3302 * Module, UInt_t * Pointer);

/*! internal functions */
UInt_t sis3302_readPreTrigDelayAndGateLength(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelayAndGateLength(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

UInt_t sis3302_readRawDataBufConfig(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataBufConfig(s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

UInt_t sis3302_readTriggerSetup(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerSetup(s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readTriggerExtendedSetup(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerExtendedSetup(s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readTriggerThreshReg(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshReg(s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readEnergySetup(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergySetup(s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readEnergyGateReg(s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateReg(s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readAcquisitionControl(s_sis_3302 * Module);
Bool_t sis3302_writeAcquisitionControl(s_sis_3302 * Module, UInt_t Data);

Bool_t sis3302_dataReady(s_sis_3302 * Module);

void sis3302_switchSampling(s_sis_3302 * Module);
Bool_t sis3302_setPageReg(s_sis_3302 * Module, Int_t PageNo);
Int_t sis3302_getPageReg(s_sis_3302 * Module);

Bool_t sis3302_checkChannelNo(s_sis_3302 * Module, Char_t * Caller, Int_t ChanNo);

void sis3302_setStatus(s_sis_3302 * Module, UInt_t Bits);
void sis3302_clearStatus(s_sis_3302 * Module, UInt_t Bits);
UInt_t sis3302_getStatus(s_sis_3302 * Module);
Bool_t sis3302_ssStatus(s_sis_3302 * Module, UInt_t Bits);

/*! check if reduced address space */
Bool_t sis3302_checkAddressSpace(s_sis_3302 * Module);
ULong_t ca(s_sis_3302 * Module, ULong_t Address);

/*! check if block xfer mode */
Bool_t sis3302_blockXferIsOn(s_sis_3302 * Module);

void sis_print(s_sis_3302 * Module, char * Message);

Bool_t sis3302_checkBusTrap(s_sis_3302 * Module, ULong_t Address, Char_t * Method);
void sis3302_catchBusTrap();

#endif
