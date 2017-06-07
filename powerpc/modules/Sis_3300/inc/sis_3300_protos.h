/*_________________________________________________________________[PROTOTYPES]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3300_protos.h
// Purpose:        Prototypes for SIS3300 digitizing ADC
// Description:    Contains prototypes to be used with SIS 3300
// Author:         R. Lutter
// Revision:
// Date:
// Keywords:
////////////////////////////////////////////////////////////////////////////*/

s_sis_3300 * sis_3300_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * name, int serial);
void sis_3300_module_info(s_sis_3300 * s);
int sis_3300_fill_struct(s_sis_3300 * s, char * file);

void sis_3300_reset(s_sis_3300 * s);
int sis_3300_initialize(s_sis_3300 * s);
int sis_3300_readout(s_sis_3300 * s, uint8_t bank, uint32_t * pointer);
void sis_3300_dumpRegisters(s_sis_3300 * s);

void sis_3300_setStartDelay(s_sis_3300 * s, uint16_t delay);
uint16_t sis_3300_getStartDelay(s_sis_3300 * s);
void sis_3300_setStopDelay(s_sis_3300 * s, uint16_t delay);
uint16_t sis_3300_getStopDelay(s_sis_3300 * s);
void sis_3300_setTimeStampPredivider(s_sis_3300 * s, uint16_t divider);
uint16_t sis_3300_getTimeStampPredivider(s_sis_3300 * s);
void sis_3300_startSampling(s_sis_3300 * s);
void sis_3300_stopSampling(s_sis_3300 * s);
void sis_3300_startAutoBankSwitch(s_sis_3300 * s);
void sis_3300_stopAutoBankSwitch(s_sis_3300 * s);
void sis_3300_clearBankFull(s_sis_3300 * s, uint8_t bank);
uint32_t sis_3300_getEventTimeStamp(s_sis_3300 * s, uint8_t bank, uint16_t event);
void sis_3300_setPageSize(s_sis_3300 * s, uint8_t pagesize);
uint8_t sis_3300_getPageSize(s_sis_3300 * s);
void sis_3300_enableWrapAroundMode(s_sis_3300 * s);
void sis_3300_disableWrapAroundMode(s_sis_3300 * s);
uint8_t sis_3300_wrapAroundModeEnabled(s_sis_3300 * s);
void sis_3300_enableGateChaining(s_sis_3300 * s);
void sis_3300_disableGateChaining(s_sis_3300 * s);
uint8_t sis_3300_gateChainingEnabled(s_sis_3300 * s);
void sis_3300_setAverage(s_sis_3300 * s, uint8_t average);
uint8_t sis_3300_getAverage(s_sis_3300 * s);
void sis_3300_setThreshold(s_sis_3300 * s, uint8_t channel, uint16_t value);
uint16_t sis_3300_getThreshold(s_sis_3300 * s, uint8_t channel);
void sis_3300_setTriggerClearCounter(s_sis_3300 * s, uint8_t channelGrp,
                                    uint16_t value);
uint16_t sis_3300_getTriggerClearCounter(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setClockPredivider(s_sis_3300 * s, uint8_t predivider);
uint8_t sis_3300_getClockPredivider(s_sis_3300 * s);
void sis_3300_setNumberOfSamples(s_sis_3300 * s, uint8_t value);
uint8_t sis_3300_getNumberOfSamples(s_sis_3300 * s);
void sis_3300_setTriggerGapTime(s_sis_3300 * s, uint8_t channelGrp,
                               uint8_t value);
uint8_t sis_3300_getTriggerGapTime(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setTriggerPeakingTime(s_sis_3300 * s, uint8_t channelGrp,
                                   uint8_t value);
uint8_t sis_3300_getTriggerPeakingTime(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setTriggerPulseLength(s_sis_3300 * s, uint8_t channelGrp,
                                  uint8_t value);
uint8_t sis_3300_getTriggerPulseLength(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_enableFIRTestEven(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTestEven(s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTestEvenEnabled(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_enableFIRTestMode(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTestMode(s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTestModeEnabled(s_sis_3300 * s, uint8_t channelGrp);
void SIS330_setTriggerMode(s_sis_3300 * s, uint8_t channel, uint8_t mode);
uint8_t SIS330_getTriggerMode(s_sis_3300 * s, uint8_t channel);
void sis_3300_enablePulseMode(s_sis_3300 * s, uint8_t channel);
void sis_3300_disablePulseMode(s_sis_3300 * s, uint8_t channel);
uint8_t sis_3300_pulseModeEnabled(s_sis_3300 * s, uint8_t channel);
void sis_3300_enableFIRTrigger(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTrigger(s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTriggerEnabled(s_sis_3300 * s, uint8_t channel);
uint32_t sis_3300_getTriggerSetupRegister(s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setMaxNumberOfEvents(s_sis_3300 * s, uint16_t maxno);
uint16_t sis_3300_getMaxNumberOfEvents(s_sis_3300 * s);
void sis_3300_armBank(s_sis_3300 * s, uint8_t bank);
void sis_3300_disarmBank(s_sis_3300 * s, uint8_t bank);
uint8_t sis_3300_bankArmed(s_sis_3300 * s, uint8_t bank);
void sis_3300_enableAutoBankSwitch(s_sis_3300 * s);
void sis_3300_disableAutoBankSwitch(s_sis_3300 * s);
uint8_t sis_3300_autoBankSwitchEnabled(s_sis_3300 * s);
void sis_3300_enableAutostart(s_sis_3300 * s);
void sis_3300_disableAutostart(s_sis_3300 * s);
uint8_t sis_3300_autostartEnabled(s_sis_3300 * s);
void sis_3300_enableMultiEventMode(s_sis_3300 * s);
void sis_3300_disableMultiEventMode(s_sis_3300 * s);
uint8_t sis_3300_multiEventModeEnabled(s_sis_3300 * s);
void sis_3300_enableStartDelay(s_sis_3300 * s);
void sis_3300_disableStartDelay(s_sis_3300 * s);
uint8_t sis_3300_startDelayEnabled(s_sis_3300 * s);
void sis_3300_enableStopDelay(s_sis_3300 * s);
void sis_3300_disableStopDelay(s_sis_3300 * s);
uint8_t sis_3300_stopDelayEnabled(s_sis_3300 * s);
void sis_3300_enableFrontStartStop(s_sis_3300 * s);
void sis_3300_disableFrontStartStop(s_sis_3300 * s);
uint8_t sis_3300_frontStartStopEnabled(s_sis_3300 * s);
void sis_3300_enableP2StartStop(s_sis_3300 * s);
void sis_3300_disableP2StartStop(s_sis_3300 * s);
uint8_t sis_3300_P2StartStopEnabled(s_sis_3300 * s);
void sis_3300_enableFrontPanelGateMode(s_sis_3300 * s);
void sis_3300_disableFrontPanelGateMode(s_sis_3300 * s);
uint8_t sis_3300_gateModeFrontPanelEnabled(s_sis_3300 * s);
void sis_3300_enableExtClockRandomMode(s_sis_3300 * s);
void sis_3300_disableExtClockRandomMode(s_sis_3300 * s);
uint8_t sis_3300_extClockRandomModeEnabled(s_sis_3300 * s);
void sis_3300_setClockSource(s_sis_3300 * s, uint8_t mode);
uint8_t sis_3300_getClockSource(s_sis_3300 * s);
uint8_t sis_3300_getADC_BUSY(s_sis_3300 * s);
uint8_t sis_3300_getBankSwitchBusy(s_sis_3300 * s);
uint8_t sis_3300_getBankBusy(s_sis_3300 * s, uint8_t bank);
uint8_t sis_3300_getBankFull(s_sis_3300 * s, uint8_t bank);
void sis_3300_setUserLED(s_sis_3300 * s, uint8_t on);
uint8_t sis_3300_getUserLED(s_sis_3300 * s);
void sis_3300_setUserOutput(s_sis_3300 * s, uint8_t on);
uint8_t sis_3300_getUserOutput(s_sis_3300 * s);
void sis_3300_enableTriggerOut(s_sis_3300 * s);
void sis_3300_disableTriggerOut(s_sis_3300 * s);
uint8_t sis_3300_triggerOutEnabled(s_sis_3300 * s);
void sis_3300_enableInverseTriggerOut(s_sis_3300 * s);
void sis_3300_disableInverseTriggerOut(s_sis_3300 * s);
uint8_t sis_3300_inverseTriggerOutEnabled(s_sis_3300 * s);
void sis_3300_enableTriggerArmedStart(s_sis_3300 * s);
void sis_3300_disableTriggerArmedStart(s_sis_3300 * s);
uint8_t sis_3300_triggerArmedStartEnabled(s_sis_3300 * s);
void sis_3300_enableTriggerRouting(s_sis_3300 * s);
void sis_3300_disableTriggerRouting(s_sis_3300 * s);
uint8_t sis_3300_triggerRoutingEnabled(s_sis_3300 * s);
void sis_3300_enableBankFullLemo(s_sis_3300 * s, uint8_t lemo);
void sis_3300_disableBankFullLemo(s_sis_3300 * s);
uint8_t sis_3300_bankFullLemoEnabled(s_sis_3300 * s);
uint8_t sis_3300_getP2_TEST_IN(s_sis_3300 * s);
uint8_t sis_3300_getP2_RESET_IN(s_sis_3300 * s);
uint8_t sis_3300_getP2_SAMPLE_IN(s_sis_3300 * s);
void  sis_3300_enableVMEIRQ(s_sis_3300 * s);
void  sis_3300_enableROAKIRQ(s_sis_3300 * s);
void sis_3300_enableIRQ(s_sis_3300 * s, uint8_t source);
void sis_3300_disableIRQ(s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_IRQEnabled(s_sis_3300 * s, uint8_t source);
void sis_3300_clearIRQ(s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQStatusFlag(s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQStatus(s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQInternalStatus(s_sis_3300 * s);
uint8_t sis_3300_getIRQVMEStatus(s_sis_3300 * s);
uint32_t sis_3300_getTriggerEventDirectory(s_sis_3300 * s, uint8_t bank,
                                          uint16_t event);
uint8_t sis_3300_getEventWrap(s_sis_3300 * s, uint8_t bank, uint16_t event);
uint32_t sis_3300_getEventEnd(s_sis_3300 * s, uint8_t bank, uint8_t channelGrp,
                             uint16_t event);
uint32_t sis_3300_getAddressCounter(s_sis_3300 * s, uint8_t bank);
uint16_t sis_3300_getEventCounter(s_sis_3300 * s, uint8_t bank);
uint32_t sis_3300_getActualSampleValue(s_sis_3300 * s, uint8_t channelGrp);
uint32_t sis_3300_getPageSizeVal(s_sis_3300 * s);
float sis_3300_getFrequency(s_sis_3300 * s);

void sis_3300_enable_bma(s_sis_3300 * s);
void sis_3300_dumpRegisters(s_sis_3300 * s);
