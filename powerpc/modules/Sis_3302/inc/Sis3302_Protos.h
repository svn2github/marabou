#ifndef __SIS3302_PROTOS_H__
#define __SIS3302_PROTOS_H__

/*_______________________________________________________________[HEADER FILE]
//////////////////////////////////////////////////////////////////////////////
//! \file			Sis3302_Protos.h
//! \brief			Definitions for SIS3302 ADC
//! \details		Prototypes for SIS3302 ADC
//! $Author: Rudolf.Lutter $
//! $Mail:			<a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>$
//! $Revision: 1.3 $
//! $Date: 2010-04-22 13:44:41 $
////////////////////////////////////////////////////////////////////////////*/


struct s_sis_3302 * Moduleis3302_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * moduleName, Int_t serial);

/*! Get module info */
Bool_t sis3302_moduleInfo(struct s_sis_3302 * Module, Int_t * boardId, Int_t * majorVersion, Int_t * minorVersion, Bool_t PrintFlag);
void sis3302_setPrefix(struct s_sis_3302 * Module, char * prefix);

/*! Fill SIS3302 database from file */
Bool_t sis3302_fillStruct(struct s_sis_3302 * Module, char * file);

/*! Load settings from database */
void sis3302_loadFromDb(struct s_sis_3302 * Module, UInt_t chnPattern);

/*! Debugging tools: dump settings, prInt_t database contents */
Bool_t sis3302_dumpRegisters(struct s_sis_3302 * Module, char * file);
Bool_t sis3302_dumpRaw(struct s_sis_3302 * Module, char * file);
void sis3302_printDb(struct s_sis_3302 * Module);

/*! Reset module */
void sis3302_reset(struct s_sis_3302 * Module);

/*! Initialize settings with derfault values */
void sis3302_initDefaults(struct s_sis_3302 * Module);

/*! Read dac values */
Bool_t sis3302_readDac(struct s_sis_3302 * Module, Int_t * DacValues, Int_t AdcNo);
/*! Write dac values */
Bool_t sis3302_writeDac(struct s_sis_3302 * Module, Int_t * DacValues, Int_t AdcNo);
Bool_t sis3302_writeDac_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! Switch user LED on/off */
Bool_t sis3302_setUserLED(struct s_sis_3302 * Module, Bool_t OnFlag);

/*! Exec KEY command */
Bool_t sis3302_keyAddr(struct s_sis_3302 * Module, Int_t Key);

/*! Exec KEY command: reset */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyReset(struct s_sis_3302 * Module);

/*! Exec KEY command: reset sample */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyResetSample(struct s_sis_3302 * Module);

/*! Exec KEY command: generate trigger */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyTrigger(struct s_sis_3302 * Module);

/*! Exec KEY command: clear time stamp */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyClearTimestamp(struct s_sis_3302 * Module);

/*! Exec KEY command: arm sampling for bank 1 */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyArmBank1Sampling(struct s_sis_3302 * Module);

/*! Exec KEY command: arm sampling for bank 2 */
/*! \param[in]	Module	-- module address */
Bool_t sis3302_keyArmBank2Sampling(struct s_sis_3302 * Module);

/*! control and status */
UInt_t sis3302_readControlStatus(struct s_sis_3302 * Module);
Bool_t sis3302_writeControlStatus(struct s_sis_3302 * Module, unsigned Int_t Bits);
Bool_t sis3302_writeControlStatus_db(struct s_sis_3302 * Module);

/*! event configuration */
UInt_t sis3302_readEventConfig(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEventConfig(struct s_sis_3302 * Module, unsigned Int_t Bits, Int_t AdcNo);
Bool_t sis3302_writeEventConfig_db(struct s_sis_3302 * Module, Int_t AdcNo);
UInt_t sis3302_readEventExtendedConfig(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEventExtendedConfig(struct s_sis_3302 * Module, unsigned Int_t Bits, Int_t AdcNo);
Bool_t sis3302_writeEventExtendedConfig_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! event header & group id */
Bool_t sis3302_setHeaderBits(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setHeaderBits_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getHeaderBits(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getGroupId(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger mode */
Bool_t sis3302_setTriggerMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setTriggerMode_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getTriggerMode(struct s_sis_3302 * Module, Int_t AdcNo);

/*! gate mode */
Bool_t sis3302_setGateMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setGateMode_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getGateMode(struct s_sis_3302 * Module, Int_t AdcNo);

/*! next neighbors, trigger & gate */
Bool_t sis3302_setNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setNextNeighborTriggerMode_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getNextNeighborTriggerMode(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setNextNeighborGateMode(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setNextNeighborGateMode_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_getNextNeighborGateMode(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger polarity */
Bool_t sis3302_setPolarity(struct s_sis_3302 * Module, Bool_t InvertFlag, Int_t AdcNo);
Bool_t sis3302_setPolarity_db(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_getPolarity(struct s_sis_3302 * Module, Int_t AdcNo);

/*! end address threshold */
Int_t sis3302_readEndAddrThresh(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEndAddrThresh(struct s_sis_3302 * Module, Int_t Thresh, Int_t AdcNo);
Bool_t sis3302_writeEndAddrThresh_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! pre-trigger delay and gate length */
Int_t sis3302_readPreTrigDelay(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writePreTrigDelay(struct s_sis_3302 * Module, Int_t Delay, Int_t AdcNo);
Bool_t sis3302_writePreTrigDelay_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_readTrigGateLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTrigGateLength(struct s_sis_3302 * Module, Int_t Gate, Int_t AdcNo);
Bool_t sis3302_writeTrigGateLength_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! raw data sample length & start */
Int_t sis3302_readRawDataSampleLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeRawDataSampleLength(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_writeRawDataSampleLength_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_readRawDataStartIndex(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeRawDataStartIndex(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_writeRawDataStartIndex(struct s_sis_3302 * Module, Int_t AdcNo);

Int_t sis3302_readNextSampleAddr(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_readPrevBankSampleAddr(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_readActualSample(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger peak & gap */
Bool_t sis3302_readTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t * Peak, Int_t * Gap, Int_t AdcNo);
Bool_t sis3302_writeTriggerPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t AdcNo);
Bool_t sis3302_writeTriggerPeakAndGap_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger out pulse length */
Int_t sis3302_readTriggerPulseLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerPulseLength(struct s_sis_3302 * Module, Int_t PulseLength, Int_t AdcNo);
Bool_t sis3302_writeTriggerPulseLength_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger internal gate & delay */
Int_t sis3302_readTriggerInternalGate(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerInternalGate(struct s_sis_3302 * Module, Int_t GateLength, Int_t AdcNo);
Bool_t sis3302_writeTriggerInternalGate_db(struct s_sis_3302 * Module, Int_t AdcNo);
Int_t sis3302_readTriggerInternalDelay(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerInternalDelay(struct s_sis_3302 * Module, Int_t DelayLength, Int_t AdcNo);
Bool_t sis3302_writeTriggerInternalDelay_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger decimation */
Int_t sis3302_getTriggerDecimation(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setTriggerDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t AdcNo);
Bool_t sis3302_setTriggerDecimation_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! trigger threshold */
Int_t sis3302_readTriggerThreshold(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerThreshold(struct s_sis_3302 * Module, Int_t Thresh, Int_t AdcNo);
Bool_t sis3302_writeTriggerThreshold_db(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_getTriggerGT(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setTriggerGT(struct s_sis_3302 * Module, Bool_t GTFlag, Int_t AdcNo);
Bool_t sis3302_setTriggerGT_db(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_getTriggerOut(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setTriggerOut(struct s_sis_3302 * Module, Bool_t TrigOutFlag, Int_t AdcNo);
Bool_t sis3302_setTriggerOut_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! energy peak & gap */
Int_t sis3302_readEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t * Peak, Int_t * Gap, Int_t AdcNo);
Bool_t sis3302_writeEnergyPeakAndGap(struct s_sis_3302 * Module, Int_t Peak, Int_t Gap, Int_t AdcNo);
Bool_t sis3302_writeEnergyPeakAndGap_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! energy decimation */
Int_t sis3302_getEnergyDecimation(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setEnergyDecimation(struct s_sis_3302 * Module, Int_t Decimation, Int_t AdcNo);
Bool_t sis3302_setEnergyDecimation_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! energy gate length */
Int_t sis3302_readEnergyGateLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEnergyGateLength(struct s_sis_3302 * Module, Int_t GateLength, Int_t AdcNo);
Bool_t sis3302_writeEnergyGateLength_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! energy sample length & start */
Int_t sis3302_readEnergySampleLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEnergySampleLength(struct s_sis_3302 * Module, Int_t SampleLength, Int_t AdcNo);
Bool_t sis3302_writeEnergySampleLength_db(struct s_sis_3302 * Module, Int_t SampleLength, Int_t AdcNo);
Int_t sis3302_readStartIndex(struct s_sis_3302 * Module, Int_t IdxNo, Int_t AdcNo);
Bool_t sis3302_writeStartIndex(struct s_sis_3302 * Module, Int_t IdxVal, Int_t IdxNo, Int_t AdcNo);
Bool_t sis3302_writeStartIndex_db(struct s_sis_3302 * Module, Int_t IdxNo, Int_t AdcNo);

/*! test bits */
Int_t sis3302_getTestBits(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_setTestBits(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Bool_t sis3302_setTestBits_db(struct s_sis_3302 * Module, Int_t AdcNo);

/*! energy tau factor */
Int_t sis3302_readTauFactor(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTauFactor(struct s_sis_3302 * Module, Int_t Tau, Int_t AdcNo);
Bool_t sis3302_writeTauFactor_db(struct s_sis_3302 * Module, Int_t AdcNo);

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

/*! accu single event */
Int_t sis3302_getSingleEvent(struct s_sis_3302 * Module, struct sis_3302_data * Data, Int_t AdcNo);

/*! internal functions */
Int_t sis3302_readPreTrigDelayAndGateLength(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writePreTrigDelayAndGateLength(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readRawDataBufConfig(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeRawDataBufConfig(struct s_sis_3302 * Module, Int_t Bits, Int_t AdcNo);
Int_t sis3302_readTriggerSetup(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerSetup(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readTriggerExtendedSetup(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerExtendedSetup(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readTriggerThreshReg(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeTriggerThreshReg(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readEnergySetup(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEnergySetup(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readEnergyGateReg(struct s_sis_3302 * Module, Int_t AdcNo);
Bool_t sis3302_writeEnergyGateReg(struct s_sis_3302 * Module, Int_t Data, Int_t AdcNo);
Int_t sis3302_readAcquisitionControl(struct s_sis_3302 * Module);
Bool_t sis3302_writeAcquisitionControl(struct s_sis_3302 * Module, Int_t Data);

Bool_t DataReady(struct s_sis_3302 * Module);
Int_t sis3302_readData(struct s_sis_3302 * Module, struct sis_3302_data * Data, Int_t NofWords, Int_t AdcNo);

#endif
