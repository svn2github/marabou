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

void sis_print(struct s_sis_3302 * Module, char * Message);

/*! Alloc database for SIS3302 module */
struct s_sis_3302 * sis3302_alloc(Char_t * ModuleName, struct mapDescr * MD, Int_t Serial);

/*! Map VME address */
volatile char * sis3302_mapAddress_sized(struct s_sis_3302 * Module, Int_t Offset, Int_t SegSize);
volatile char * sis3302_MapAddress(struct s_sis_3302 * Module, Int_t Offset);

/*! Get module info */
void sis3302_moduleInfo(struct s_sis_3302 * Module);

/*! Fill SIS3302 database from file */
Bool_t sis3302_fillStruct(struct s_sis_3302 * Module, Char_t * SettingsFile);

/*! Load settings from database */
void sis3302_loadFromDb(struct s_sis_3302 * Module);

/*! Set pattern of active channels */
/*! ...FromDb: generate pattern from trigger mode bits */

void sis3302_setActiveChannels(struct s_sis_3302 * Module, UInt_t ChnPattern);
void sis3302_setActiveChannelsFromDb(struct s_sis_3302 * Module);

/*! Set max buffer size */
/*! >>> MBS can't handle 8 MB buffers <<< */

void sis3302_setBufferSize(struct s_sis_3302 * Module, Int_t BufferSize);

/*! Debugging tools: dump settings, print_t database contents */
Bool_t sis3302_dumpRegisters(struct s_sis_3302 * Module, Char_t * File);

/*! Read dac values */
Bool_t sis3302_readDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo);
/*! Write dac values */
Bool_t sis3302_writeDac(struct s_sis_3302 * Module, Int_t DacValues[], Int_t ChanNo);
Bool_t sis3302_writeDac_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! Switch user LED on/off */
Bool_t sis3302_setUserLED(struct s_sis_3302 * Module, Bool_t OnFlag);

/*! Exec KEY command */
Bool_t sis3302_keyAddr(struct s_sis_3302 * Module, Int_t Key);

/*! Exec KEY command: reset */
Bool_t sis3302_reset(struct s_sis_3302 * Module);

/*! Exec KEY command: reset sample */
Bool_t sis3302_resetSampling(struct s_sis_3302 * Module);

/*! Exec KEY command: generate trigger */
Bool_t sis3302_fireTrigger(struct s_sis_3302 * Module);

/*! Exec KEY command: clear time stamp */
Bool_t sis3302_clearTimestamp(struct s_sis_3302 * Module);

/*! Exec KEY command: arm sampling */
Bool_t sis3302_armSampling(struct s_sis_3302 * Module, Int_t Sampling);

/*! Exec KEY command: disarm sampling */
Bool_t sis3302_disarmSampling(struct s_sis_3302 * Module);

/*! set/restore trace length */
void sis3302_setTracingMode(struct s_sis_3302 * Module, Bool_t OnFlag);
void sis3302_adjustTraceLength(struct s_sis_3302 * Module);
void sis3302_restoreTraceLength(struct s_sis_3302 * Module);

/*! set fifo limit */
void sis3302_setEndAddress(struct s_sis_3302 * Module, Int_t NofEvents);

/*! start/stop acquisition */
void sis3302_startAcquisition(struct s_sis_3302 * Module, Int_t NofEvents);
void sis3302_stopAcquisition(struct s_sis_3302 * Module);

/*! control and status */
UInt_t sis3302_readControlStatus(struct s_sis_3302 * Module);
Bool_t sis3302_writeControlStatus(struct s_sis_3302 * Module, UInt_t Bits);
Bool_t sis3302_writeControlStatus_db(struct s_sis_3302 * Module);

/*! event configuration */
UInt_t sis3302_readEventConfig(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEventConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
UInt_t sis3302_readEventExtendedConfig(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEventExtendedConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

/*! event header & group id */
Bool_t sis3302_setHeaderBits(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setHeaderBits_db(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getHeaderBits(struct s_sis_3302 * Module, Int_t ChanNo);

UInt_t sis3302_getGroupId(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger mode */
Bool_t sis3302_setTriggerMode(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setTriggerMode_db(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getTriggerMode(struct s_sis_3302 * Module, Int_t ChanNo);

/*! gate mode */
Bool_t sis3302_setGateMode(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setGateMode_db(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getGateMode(struct s_sis_3302 * Module, Int_t ChanNo);

/*! next neighbors, trigger & gate */
Bool_t sis3302_setNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t Bits, Int_t ChanNo);
Bool_t sis3302_setNextNeighborTriggerMode_db(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setNextNeighborGateMode(struct s_sis_3302 * Module, Int_t Bits, Int_t ChanNo);
Bool_t sis3302_setNextNeighborGateMode_db(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_getNextNeighborGateMode(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger polarity */
Bool_t sis3302_setPolarity(struct s_sis_3302 * Module, Bool_t InvertFlag, Int_t ChanNo);
Bool_t sis3302_setPolarity_db(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_polarityIsInverted(struct s_sis_3302 * Module, Int_t ChanNo);

/*! end address threshold */
Int_t sis3302_readEndAddrThresh(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEndAddrThresh(struct s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo);
Bool_t sis3302_writeEndAddrThresh_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! pre-trigger delay and gate length */
Int_t sis3302_readPreTrigDelay(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelay(struct s_sis_3302 * Module, Int_t Delay, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelay_db(struct s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readTrigGateLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTrigGateLength(struct s_sis_3302 * Module, Int_t Gate, Int_t ChanNo);
Bool_t sis3302_writeTrigGateLength_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! raw data sample length & start */
Int_t sis3302_readRawDataSampleLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataSampleLength(struct s_sis_3302 * Module, Int_t Sample, Int_t ChanNo);
Bool_t sis3302_writeRawDataSampleLength_db(struct s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readRawDataStartIndex(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataStartIndex(struct s_sis_3302 * Module, Int_t Index, Int_t ChanNo);
Bool_t sis3302_writeRawDataStartIndex_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! sample address */
Int_t sis3302_readNextSampleAddr(struct s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readPrevBankSampleAddr(struct s_sis_3302 * Module, Int_t ChanNo);
UInt_t sis3302_readActualSample(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger peak & gap */
Bool_t sis3302_readTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo);
Bool_t sis3302_writeTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t ChanNo);
Bool_t sis3302_writeTriggerPeakAndGap_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger out pulse length */
Int_t sis3302_readTriggerPulseLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerPulseLength(struct s_sis_3302 * Module, Int_t PulseLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerPulseLength_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger internal gate & delay */
Int_t sis3302_readTriggerInternalGate(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalGate(struct s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalGate_db(struct s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readTriggerInternalDelay(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalDelay(struct s_sis_3302 * Module, Int_t DelayLength, Int_t ChanNo);
Bool_t sis3302_writeTriggerInternalDelay_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger decimation */
Int_t sis3302_getTriggerDecimation(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo);
Bool_t sis3302_setTriggerDecimation_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! trigger threshold */
Int_t sis3302_readTriggerThreshold(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshold(struct s_sis_3302 * Module, Int_t Thresh, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshold_db(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_getTriggerGT(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerGT(struct s_sis_3302 * Module, Bool_t GTFlag, Int_t ChanNo);
Bool_t sis3302_setTriggerGT_db(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_getTriggerOut(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTriggerOut(struct s_sis_3302 * Module, Bool_t TrigOutFlag, Int_t ChanNo);
Bool_t sis3302_setTriggerOut_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! energy peak & gap */
Bool_t sis3302_readEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t PeakGap[], Int_t ChanNo);
Bool_t sis3302_writeEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t ChanNo);
Bool_t sis3302_writeEnergyPeakAndGap_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! energy decimation */
Int_t sis3302_getEnergyDecimation(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setEnergyDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t ChanNo);
Bool_t sis3302_setEnergyDecimation_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! energy gate length */
Int_t sis3302_readEnergyGateLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateLength(struct s_sis_3302 * Module, Int_t GateLength, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateLength_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! energy sample length & start */
Int_t sis3302_readEnergySampleLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergySampleLength(struct s_sis_3302 * Module, Int_t SampleLength, Int_t ChanNo);
Bool_t sis3302_writeEnergySampleLength_db(struct s_sis_3302 * Module, Int_t ChanNo);
Int_t sis3302_readStartIndex(struct s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo);
Bool_t sis3302_writeStartIndex(struct s_sis_3302 * Module, Int_t IdxVal, Int_t IdxNo, Int_t ChanNo);
Bool_t sis3302_writeStartIndex_db(struct s_sis_3302 * Module, Int_t IdxNo, Int_t ChanNo);

/*! test bits */
UInt_t sis3302_getTestBits(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_setTestBits(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);
Bool_t sis3302_setTestBits_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! energy tau factor */
Int_t sis3302_readTauFactor(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTauFactor(struct s_sis_3302 * Module, Int_t Tau, Int_t ChanNo);
Bool_t sis3302_writeTauFactor_db(struct s_sis_3302 * Module, Int_t ChanNo);

/*! clock source */
Int_t sis3302_getClockSource(struct s_sis_3302 * Module);
Bool_t sis3302_setClockSource(struct s_sis_3302 * Module, Int_t ClockSource);
Bool_t sis3302_setClockSource_db(struct s_sis_3302 * Module);

/*! lemo in & out */
Int_t sis3302_getLemoInMode(struct s_sis_3302 * Module);
Bool_t sis3302_setLemoInMode(struct s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoInMode_db(struct s_sis_3302 * Module);
Int_t sis3302_getLemoOutMode(struct s_sis_3302 * Module);
Bool_t sis3302_setLemoOutMode(struct s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoOutMode_db(struct s_sis_3302 * Module);
Int_t sis3302_getLemoInEnableMask(struct s_sis_3302 * Module);
Bool_t sis3302_setLemoInEnableMask(struct s_sis_3302 * Module, Int_t Bits);
Bool_t sis3302_setLemoInEnableMask_db(struct s_sis_3302 * Module);

/*! trigger feedback */
Bool_t sis3302_getTriggerFeedback(struct s_sis_3302 * Module);
Bool_t sis3302_setTriggerFeedback(struct s_sis_3302 * Module, Bool_t Feedback);
Bool_t sis3302_setTriggerFeedback_db(struct s_sis_3302 * Module);

/*! read traces and/or energy data */
Int_t sis3302_readout(struct s_sis_3302 * Module, UInt_t * Pointer);

/*! internal functions */
UInt_t sis3302_readPreTrigDelayAndGateLength(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writePreTrigDelayAndGateLength(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

UInt_t sis3302_readRawDataBufConfig(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeRawDataBufConfig(struct s_sis_3302 * Module, UInt_t Bits, Int_t ChanNo);

UInt_t sis3302_readTriggerSetup(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readTriggerExtendedSetup(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerExtendedSetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readTriggerThreshReg(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeTriggerThreshReg(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readEnergySetup(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergySetup(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readEnergyGateReg(struct s_sis_3302 * Module, Int_t ChanNo);
Bool_t sis3302_writeEnergyGateReg(struct s_sis_3302 * Module, UInt_t Data, Int_t ChanNo);

UInt_t sis3302_readAcquisitionControl(struct s_sis_3302 * Module);
Bool_t sis3302_writeAcquisitionControl(struct s_sis_3302 * Module, UInt_t Data);

Bool_t sis3302_dataReady(struct s_sis_3302 * Module);

void sis3302_switchSampling(struct s_sis_3302 * Module);
Bool_t sis3302_setPageReg(struct s_sis_3302 * Module, Int_t PageNo);
Int_t sis3302_getPageReg(struct s_sis_3302 * Module);

Bool_t sis3302_checkChannelNo(struct s_sis_3302 * Module, Char_t * Caller, Int_t ChanNo);

void sis3302_setStatus(struct s_sis_3302 * Module, UInt_t Bits);
void sis3302_clearStatus(struct s_sis_3302 * Module, UInt_t Bits);
UInt_t sis3302_getStatus(struct s_sis_3302 * Module);
Bool_t sis3302_ssStatus(struct s_sis_3302 * Module, UInt_t Bits);

/*! check if reduced address space */
Bool_t sis3302_checkAddressSpace(struct s_sis_3302 * Module);
ULong_t ca(struct s_sis_3302 * Module, ULong_t Address);

/*! check if block xfer mode */
Bool_t sis3302_blockXferIsOn(struct s_sis_3302 * Module);

#endif
