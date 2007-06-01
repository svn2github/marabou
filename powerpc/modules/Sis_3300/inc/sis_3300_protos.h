/*_________________________________________________________________[PROTOTYPES]
//////////////////////////////////////////////////////////////////////////////
// Name:           sis_3300.h
// Purpose:        Prototypes for SIS3300 digitizing ADC
// Description:    Contains prototypes to be used with SIS 3300
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:       
////////////////////////////////////////////////////////////////////////////*/

struct s_sis_3300 * sis_3300_alloc(unsigned long vmeAddr, volatile unsigned char * base, char * name, int serial);
void sis_3300_module_info(struct s_sis_3300 * s);
int sis_3300_fill_struct(struct s_sis_3300 * s, char * file);

void sis_3300_reset(struct s_sis_3300 * s);
int sis_3300_initialize(struct s_sis_3300 * s);
int sis_3300_readout(struct s_sis_3300 * s, uint8_t bank, uint32_t * pointer);
void sis_3300_dumpRegisters(struct s_sis_3300 * s);

void sis_3300_setStartDelay(struct s_sis_3300 * s, uint16_t delay);
uint16_t sis_3300_getStartDelay(struct s_sis_3300 * s);
void sis_3300_setStopDelay(struct s_sis_3300 * s, uint16_t delay);
uint16_t sis_3300_getStopDelay(struct s_sis_3300 * s);
void sis_3300_setTimeStampPredivider(struct s_sis_3300 * s, uint16_t divider);
uint16_t sis_3300_getTimeStampPredivider(struct s_sis_3300 * s);
void sis_3300_startSampling(struct s_sis_3300 * s);
void sis_3300_stopSampling(struct s_sis_3300 * s);
void sis_3300_startAutoBankSwitch(struct s_sis_3300 * s);
void sis_3300_stopAutoBankSwitch(struct s_sis_3300 * s);
void sis_3300_clearBankFull(struct s_sis_3300 * s, uint8_t bank);
uint32_t sis_3300_getEventTimeStamp(struct s_sis_3300 * s, uint8_t bank, uint16_t event);
void sis_3300_setPageSize(struct s_sis_3300 * s, uint8_t pagesize);
uint8_t sis_3300_getPageSize(struct s_sis_3300 * s);
void sis_3300_enableWrapAroundMode(struct s_sis_3300 * s);
void sis_3300_disableWrapAroundMode(struct s_sis_3300 * s);
uint8_t sis_3300_wrapAroundModeEnabled(struct s_sis_3300 * s);
void sis_3300_enableGateChaining(struct s_sis_3300 * s);
void sis_3300_disableGateChaining(struct s_sis_3300 * s);
uint8_t sis_3300_gateChainingEnabled(struct s_sis_3300 * s);
void sis_3300_setAverage(struct s_sis_3300 * s, uint8_t average);
uint8_t sis_3300_getAverage(struct s_sis_3300 * s);
void sis_3300_setThreshold(struct s_sis_3300 * s, uint8_t channel, uint16_t value);
uint16_t sis_3300_getThreshold(struct s_sis_3300 * s, uint8_t channel);
void sis_3300_setTriggerClearCounter(struct s_sis_3300 * s, uint8_t channelGrp,
                                    uint16_t value);
uint16_t sis_3300_getTriggerClearCounter(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setClockPredivider(struct s_sis_3300 * s, uint8_t predivider);
uint8_t sis_3300_getClockPredivider(struct s_sis_3300 * s);
void sis_3300_setNumberOfSamples(struct s_sis_3300 * s, uint8_t value);
uint8_t sis_3300_getNumberOfSamples(struct s_sis_3300 * s);
void sis_3300_setTriggerGapTime(struct s_sis_3300 * s, uint8_t channelGrp, 
                               uint8_t value);
uint8_t sis_3300_getTriggerGapTime(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setTriggerPeakingTime(struct s_sis_3300 * s, uint8_t channelGrp, 
                                   uint8_t value);
uint8_t sis_3300_getTriggerPeakingTime(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setTriggerPulseLength(struct s_sis_3300 * s, uint8_t channelGrp,
                                  uint8_t value);
uint8_t sis_3300_getTriggerPulseLength(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_enableFIRTestEven(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTestEven(struct s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTestEvenEnabled(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_enableFIRTestMode(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTestMode(struct s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTestModeEnabled(struct s_sis_3300 * s, uint8_t channelGrp);
void SIS330_setTriggerMode(struct s_sis_3300 * s, uint8_t channel, uint8_t mode);
uint8_t SIS330_getTriggerMode(struct s_sis_3300 * s, uint8_t channel);
void sis_3300_enablePulseMode(struct s_sis_3300 * s, uint8_t channel);
void sis_3300_disablePulseMode(struct s_sis_3300 * s, uint8_t channel);
uint8_t sis_3300_pulseModeEnabled(struct s_sis_3300 * s, uint8_t channel);
void sis_3300_enableFIRTrigger(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_disableFIRTrigger(struct s_sis_3300 * s, uint8_t channelGrp);
uint8_t sis_3300_FIRTriggerEnabled(struct s_sis_3300 * s, uint8_t channel);
uint32_t sis_3300_getTriggerSetupRegister(struct s_sis_3300 * s, uint8_t channelGrp);
void sis_3300_setMaxNumberOfEvents(struct s_sis_3300 * s, uint16_t maxno);
uint16_t sis_3300_getMaxNumberOfEvents(struct s_sis_3300 * s);
void sis_3300_armBank(struct s_sis_3300 * s, uint8_t bank);
void sis_3300_disarmBank(struct s_sis_3300 * s, uint8_t bank);
uint8_t sis_3300_bankArmed(struct s_sis_3300 * s, uint8_t bank);
void sis_3300_enableAutoBankSwitch(struct s_sis_3300 * s);
void sis_3300_disableAutoBankSwitch(struct s_sis_3300 * s);
uint8_t sis_3300_autoBankSwitchEnabled(struct s_sis_3300 * s);
void sis_3300_enableAutostart(struct s_sis_3300 * s);
void sis_3300_disableAutostart(struct s_sis_3300 * s);
uint8_t sis_3300_autostartEnabled(struct s_sis_3300 * s);
void sis_3300_enableMultiEventMode(struct s_sis_3300 * s);
void sis_3300_disableMultiEventMode(struct s_sis_3300 * s);
uint8_t sis_3300_multiEventModeEnabled(struct s_sis_3300 * s);
void sis_3300_enableStartDelay(struct s_sis_3300 * s);
void sis_3300_disableStartDelay(struct s_sis_3300 * s);
uint8_t sis_3300_startDelayEnabled(struct s_sis_3300 * s);
void sis_3300_enableStopDelay(struct s_sis_3300 * s);
void sis_3300_disableStopDelay(struct s_sis_3300 * s);
uint8_t sis_3300_stopDelayEnabled(struct s_sis_3300 * s);
void sis_3300_enableFrontStartStop(struct s_sis_3300 * s);
void sis_3300_disableFrontStartStop(struct s_sis_3300 * s);
uint8_t sis_3300_frontStartStopEnabled(struct s_sis_3300 * s);
void sis_3300_enableP2StartStop(struct s_sis_3300 * s);
void sis_3300_disableP2StartStop(struct s_sis_3300 * s);
uint8_t sis_3300_P2StartStopEnabled(struct s_sis_3300 * s);
void sis_3300_enableFrontPanelGateMode(struct s_sis_3300 * s);
void sis_3300_disableFrontPanelGateMode(struct s_sis_3300 * s);
uint8_t sis_3300_gateModeFrontPanelEnabled(struct s_sis_3300 * s);
void sis_3300_enableExtClockRandomMode(struct s_sis_3300 * s);
void sis_3300_disableExtClockRandomMode(struct s_sis_3300 * s);
uint8_t sis_3300_extClockRandomModeEnabled(struct s_sis_3300 * s);
void sis_3300_setClockSource(struct s_sis_3300 * s, uint8_t mode);
uint8_t sis_3300_getClockSource(struct s_sis_3300 * s);
uint8_t sis_3300_getADC_BUSY(struct s_sis_3300 * s);
uint8_t sis_3300_getBankSwitchBusy(struct s_sis_3300 * s);
uint8_t sis_3300_getBankBusy(struct s_sis_3300 * s, uint8_t bank);
uint8_t sis_3300_getBankFull(struct s_sis_3300 * s, uint8_t bank);
void sis_3300_setUserLED(struct s_sis_3300 * s, uint8_t on);
uint8_t sis_3300_getUserLED(struct s_sis_3300 * s);
void sis_3300_setUserOutput(struct s_sis_3300 * s, uint8_t on);
uint8_t sis_3300_getUserOutput(struct s_sis_3300 * s);
void sis_3300_enableTriggerOut(struct s_sis_3300 * s);
void sis_3300_disableTriggerOut(struct s_sis_3300 * s);
uint8_t sis_3300_triggerOutEnabled(struct s_sis_3300 * s);
void sis_3300_enableInverseTriggerOut(struct s_sis_3300 * s);
void sis_3300_disableInverseTriggerOut(struct s_sis_3300 * s);
uint8_t sis_3300_inverseTriggerOutEnabled(struct s_sis_3300 * s);
void sis_3300_enableTriggerArmedStart(struct s_sis_3300 * s);
void sis_3300_disableTriggerArmedStart(struct s_sis_3300 * s);
uint8_t sis_3300_triggerArmedStartEnabled(struct s_sis_3300 * s);
void sis_3300_enableTriggerRouting(struct s_sis_3300 * s);
void sis_3300_disableTriggerRouting(struct s_sis_3300 * s);
uint8_t sis_3300_triggerRoutingEnabled(struct s_sis_3300 * s);
void sis_3300_enableBankFullLemo(struct s_sis_3300 * s, uint8_t lemo);
void sis_3300_disableBankFullLemo(struct s_sis_3300 * s);
uint8_t sis_3300_bankFullLemoEnabled(struct s_sis_3300 * s);
uint8_t sis_3300_getP2_TEST_IN(struct s_sis_3300 * s);
uint8_t sis_3300_getP2_RESET_IN(struct s_sis_3300 * s);
uint8_t sis_3300_getP2_SAMPLE_IN(struct s_sis_3300 * s);
void  sis_3300_enableVMEIRQ(struct s_sis_3300 * s);
void  sis_3300_enableROAKIRQ(struct s_sis_3300 * s);
void sis_3300_enableIRQ(struct s_sis_3300 * s, uint8_t source);
void sis_3300_disableIRQ(struct s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_IRQEnabled(struct s_sis_3300 * s, uint8_t source);
void sis_3300_clearIRQ(struct s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQStatusFlag(struct s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQStatus(struct s_sis_3300 * s, uint8_t source);
uint8_t sis_3300_getIRQInternalStatus(struct s_sis_3300 * s);
uint8_t sis_3300_getIRQVMEStatus(struct s_sis_3300 * s);
uint32_t sis_3300_getTriggerEventDirectory(struct s_sis_3300 * s, uint8_t bank,
                                          uint16_t event);
uint8_t sis_3300_getEventWrap(struct s_sis_3300 * s, uint8_t bank, uint16_t event);
uint32_t sis_3300_getEventEnd(struct s_sis_3300 * s, uint8_t bank, uint8_t channelGrp,
                             uint16_t event);
uint32_t sis_3300_getAddressCounter(struct s_sis_3300 * s, uint8_t bank);
uint16_t sis_3300_getEventCounter(struct s_sis_3300 * s, uint8_t bank);
uint32_t sis_3300_getActualSampleValue(struct s_sis_3300 * s, uint8_t channelGrp);
uint32_t sis_3300_getPageSizeVal(struct s_sis_3300 * s);
float sis_3300_getFrequency(struct s_sis_3300 * s);

void sis_3300_dumpRegisters(struct s_sis_3300 * s);
