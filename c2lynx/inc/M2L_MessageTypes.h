#ifndef __M2L_MessageTypes_h__
#define __M2L_MessageTypes_h__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MessageTypes                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
// Special 'non-ROOT' edition for LynxOs                                //
// R. Lutter, Apr 2007                                                  //
//////////////////////////////////////////////////////////////////////////

enum EM2L_MessageType {
	kM2L_MESS_BYE				=	0x1,					// [c] terminate, [s] died
	kM2L_MESS_ACK				=	0x100,					// acknowledge a message
		kM2L_MESS_ACK_OK,									//		[s] ok, no message text
		kM2L_MESS_ACK_MESSAGE,								//		[s] ok + text
		kM2L_MESS_ACK_WARNING,								//		[s] warning
		kM2L_MESS_ACK_ERROR,								//		[s] error
		kM2L_MESS_ACK_END,									// end of acknowledge flags
	kM2L_MESS_VME_REQ			=	0x1000,					// vme request
		kM2L_MESS_VME_CONNECT,								//		[c] connect to module, [s] return module handle
		kM2L_MESS_VME_EXEC_FUNCTION,						//		[c] exec function, [s] return function results
	kM2L_MESS_EXEC_PROG 		=	0x4000 					// [c] execute program
};

const SMrbNamedXShort kMrbLofMessageTypes[] =
	{
		{kM2L_MESS_BYE, 					"terminate|died"},
		{kM2L_MESS_ACK,						"acknowledge"},
		{kM2L_MESS_ACK_OK,					"ok, no message text"},
		{kM2L_MESS_ACK_MESSAGE,				"ok + text"},
		{kM2L_MESS_ACK_WARNING,				"warning"},
		{kM2L_MESS_ACK_ERROR,				"error"},
		{kM2L_MESS_VME_REQ,					"vme request"},
		{kM2L_MESS_VME_CONNECT,				"connect to module|return module handle"},
		{kM2L_MESS_VME_EXEC_FUNCTION,		"exec function|return function results"},
		{kM2L_MESS_EXEC_PROG,				"execute program"},
		{0, 								NULL}
	};

enum EM2L_FunctionType {
	kM2L_FCT_GET_MODULE_INFO	=	0x1, 					// get module info
//-------------------------------------------------------------------------------
	kM2L_FCT_CAEN_875			=	0x100,					// caen adc v875
//-------------------------------------------------------------------------------
	kM2L_FCT_SIS_3302			=	0x200,					// sis flash adc 3302
	kM2L_FCT_SIS_3302_GET_MODULE_ADDR,
	kM2L_FCT_SIS_3302_SET_USER_LED,
	kM2L_FCT_SIS_3302_READ_DAC,
	kM2L_FCT_SIS_3302_WRITE_DAC,
	kM2L_FCT_SIS_3302_KEY_ADDR,
	kM2L_FCT_SIS_3302_READ_EVENT_CONFIG,
	kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG,
	kM2L_FCT_SIS_3302_READ_EVENT_EXTENDED_CONFIG,
	kM2L_FCT_SIS_3302_WRITE_EVENT_EXTENDED_CONFIG,
	kM2L_FCT_SIS_3302_SET_HEADER_BITS,
	kM2L_FCT_SIS_3302_GET_HEADER_BITS,
	kM2L_FCT_SIS_3302_GET_GROUP_ID,
	kM2L_FCT_SIS_3302_SET_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_GET_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_SET_GATE_MODE,
	kM2L_FCT_SIS_3302_GET_GATE_MODE,
	kM2L_FCT_SIS_3302_SET_NNB_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_GET_NNB_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_SET_NNB_GATE_MODE,
	kM2L_FCT_SIS_3302_GET_NNB_GATE_MODE,
	kM2L_FCT_SIS_3302_SET_POLARITY,
	kM2L_FCT_SIS_3302_GET_POLARITY,
	kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH,
	kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH,
	kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY,
	kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY,
	kM2L_FCT_SIS_3302_READ_TRIGGER_GATE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_GATE_LENGTH,
	kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX,
	kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX,
	kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR,
	kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR,
	kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE,
	kM2L_FCT_SIS_3302_READ_TRIGGER_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_READ_TRIGGER_PULSE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_PULSE_LENGTH,
	kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_GATE,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_GATE,
	kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_DELAY,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_DELAY,
	kM2L_FCT_SIS_3302_GET_TRIGGER_DECIMATION,
	kM2L_FCT_SIS_3302_SET_TRIGGER_DECIMATION,
	kM2L_FCT_SIS_3302_READ_TRIGGER_THRESH,
	kM2L_FCT_SIS_3302_WRITE_TRIGGER_THRESH,
	kM2L_FCT_SIS_3302_GET_TRIGGER_GT,
	kM2L_FCT_SIS_3302_SET_TRIGGER_GT,
	kM2L_FCT_SIS_3302_GET_TRIGGER_OUT,
	kM2L_FCT_SIS_3302_SET_TRIGGER_OUT,
	kM2L_FCT_SIS_3302_READ_ENERGY_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_WRITE_ENERGY_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_READ_ENERGY_GATE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_ENERGY_GATE_LENGTH,
	kM2L_FCT_SIS_3302_READ_ENERGY_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_ENERGY_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_GET_TEST_BITS,
	kM2L_FCT_SIS_3302_SET_TEST_BITS,
	kM2L_FCT_SIS_3302_GET_ENERGY_DECIMATION,
	kM2L_FCT_SIS_3302_SET_ENERGY_DECIMATION,
	kM2L_FCT_SIS_3302_READ_START_INDEX,
	kM2L_FCT_SIS_3302_WRITE_START_INDEX,
	kM2L_FCT_SIS_3302_READ_TAU_FACTOR,
	kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR,
	kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE,
	kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE,
	kM2L_FCT_SIS_3302_GET_LEMO_IN_MODE,
	kM2L_FCT_SIS_3302_SET_LEMO_IN_MODE,
	kM2L_FCT_SIS_3302_GET_LEMO_OUT_MODE,
	kM2L_FCT_SIS_3302_SET_LEMO_OUT_MODE,
	kM2L_FCT_SIS_3302_GET_LEMO_IN_ENABLE_MASK,
	kM2L_FCT_SIS_3302_SET_LEMO_IN_ENABLE_MASK,
	kM2L_FCT_SIS_3302_GET_TRIGGER_FEEDBACK,
	kM2L_FCT_SIS_3302_SET_TRIGGER_FEEDBACK,
	kM2L_FCT_SIS_3302_START_TRACE_COLLECTION,
	kM2L_FCT_SIS_3302_CONT_TRACE_COLLECTION,
	kM2L_FCT_SIS_3302_STOP_TRACE_COLLECTION,
	kM2L_FCT_SIS_3302_GET_TRACE_DATA,
	kM2L_FCT_SIS_3302_GET_TRACE_LENGTH,
	kM2L_FCT_SIS_3302_DUMP_TRACE,
	kM2L_FCT_SIS_3302_ACCU_HISTOGRAM,
	kM2L_FCT_SIS_3302_RAMP_DAC,
	kM2L_FCT_SIS_3302_DUMP_REGISTERS,
	kM2L_FCT_SIS_3302_SET_VERBOSE_MODE,
//--------------------------------------------------------------------------------
	kM2L_FCT_VULOM_TB			=	0x300,					// vulom3 / triggerbox
	kM2L_FCT_VULOM_TB_READ_SCALER,
	kM2L_FCT_VULOM_TB_READ_CHANNEL,
	kM2L_FCT_VULOM_TB_ENABLE_CHANNEL,
	kM2L_FCT_VULOM_TB_DISABLE_CHANNEL,
	kM2L_FCT_VULOM_TB_SET_ENABLE_MASK,
	kM2L_FCT_VULOM_TB_GET_ENABLE_MASK,
	kM2L_FCT_VULOM_TB_SET_SCALE_DOWN,
	kM2L_FCT_VULOM_TB_GET_SCALE_DOWN,
	kM2L_FCT_VULOM_TB_CLEAR_SCALER
//-------------------------------------------------------------------------------
};

const SMrbNamedXShort kMrbLofFunctionTypes[] =
	{
		{kM2L_FCT_GET_MODULE_INFO,						"get module info"},
//--------------------------------------------------------------------------------
		{kM2L_FCT_CAEN_875, 							"exec function (CAEN V785)"},
//--------------------------------------------------------------------------------
		{kM2L_FCT_SIS_3302, 							"exec function (SIS 3302)"},
		{kM2L_FCT_SIS_3302_GET_MODULE_ADDR,				"get module address"},
		{kM2L_FCT_SIS_3302_SET_USER_LED,				"turn on/off user led"},
		{kM2L_FCT_SIS_3302_READ_DAC,					"read dac offsets"},
		{kM2L_FCT_SIS_3302_WRITE_DAC,					"write dac offsets"},
		{kM2L_FCT_SIS_3302_KEY_ADDR,					"exec key addr command"},
		{kM2L_FCT_SIS_3302_READ_EVENT_CONFIG,			"read event configuration"},
		{kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG,			"write event configuration"},
		{kM2L_FCT_SIS_3302_READ_EVENT_EXTENDED_CONFIG,	"read extended event configuration"},
		{kM2L_FCT_SIS_3302_WRITE_EVENT_EXTENDED_CONFIG,	"write extended event configuration"},
		{kM2L_FCT_SIS_3302_SET_HEADER_BITS,				"set header bits"},
		{kM2L_FCT_SIS_3302_GET_HEADER_BITS,				"get header bits"},
		{kM2L_FCT_SIS_3302_GET_GROUP_ID,				"get group id"},
		{kM2L_FCT_SIS_3302_SET_TRIGGER_MODE,			"set trigger mode"},
		{kM2L_FCT_SIS_3302_GET_TRIGGER_MODE,			"get trigger mode"},
		{kM2L_FCT_SIS_3302_SET_GATE_MODE,				"set gate mode"},
		{kM2L_FCT_SIS_3302_GET_GATE_MODE,				"get gate mode"},
		{kM2L_FCT_SIS_3302_SET_NNB_TRIGGER_MODE,		"set next-neighbor trigger mode"},
		{kM2L_FCT_SIS_3302_GET_NNB_TRIGGER_MODE,		"get next-neighbor trigger mode"},
		{kM2L_FCT_SIS_3302_SET_NNB_GATE_MODE,			"set next-neighbor gate mode"},
		{kM2L_FCT_SIS_3302_GET_NNB_GATE_MODE,			"get next-neighbor gate mode"},
		{kM2L_FCT_SIS_3302_SET_POLARITY,				"set polarity"},
		{kM2L_FCT_SIS_3302_GET_POLARITY,				"get polarity"},
		{kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH,		"read end addr threshold"},
		{kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH,		"write end addr threshold"},
		{kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY,			"read pretrigger delay"},
		{kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY, 		"write pretrigger delay"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_GATE_LENGTH,	"read trigger gate length"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_GATE_LENGTH,	"write trigger gate length"},
		{kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH,	"read raw data sample length"},
		{kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH,	"write raw data sample length"},
		{kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX,	"read raw data start index"},
		{kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX,	"write raw data start index"},
		{kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR,		"read next sample address"},
		{kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR,	"read previous bank sample address"},
		{kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE,			"read actual sample"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_PEAK_AND_GAP,	"read trigger peak and gap times"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_PEAK_AND_GAP, 	"write trigger peak and gap times"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_PULSE_LENGTH,	"read trigger pulse length"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_PULSE_LENGTH, 	"write trigger pulse length"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_GATE, 	"read trigger internal gate"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_GATE, "write trigger internal gate"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_INTERNAL_DELAY, "read trigger internal delay"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_INTERNAL_DELAY,	"write trigger internal delay"},
		{kM2L_FCT_SIS_3302_GET_TRIGGER_DECIMATION, 		"write trigger decimation"},
		{kM2L_FCT_SIS_3302_SET_TRIGGER_DECIMATION, 		"write trigger decimation"},
		{kM2L_FCT_SIS_3302_READ_TRIGGER_THRESH,			"read trigger threshold"},
		{kM2L_FCT_SIS_3302_WRITE_TRIGGER_THRESH,		"write trigger threshold"},
		{kM2L_FCT_SIS_3302_GET_TRIGGER_GT,				"get trigger GT bit"},
		{kM2L_FCT_SIS_3302_SET_TRIGGER_GT,				"set trigger GT bit"},
		{kM2L_FCT_SIS_3302_GET_TRIGGER_OUT, 			"get trigger OUT bit"},
		{kM2L_FCT_SIS_3302_SET_TRIGGER_OUT, 			"set trigger OUT bit"},
		{kM2L_FCT_SIS_3302_READ_ENERGY_PEAK_AND_GAP,	"read energy peak and gap times"},
		{kM2L_FCT_SIS_3302_WRITE_ENERGY_PEAK_AND_GAP,	"write energy peak and gap times"},
		{kM2L_FCT_SIS_3302_READ_ENERGY_GATE_LENGTH, 	"read energy gate length"},
		{kM2L_FCT_SIS_3302_WRITE_ENERGY_GATE_LENGTH,	"write energy gate length"},
		{kM2L_FCT_SIS_3302_READ_ENERGY_SAMPLE_LENGTH, 	"read energy sample length"},
		{kM2L_FCT_SIS_3302_WRITE_ENERGY_SAMPLE_LENGTH,	"write energy sample length"},
		{kM2L_FCT_SIS_3302_GET_TEST_BITS,				"get test bits"},
		{kM2L_FCT_SIS_3302_SET_TEST_BITS,				"set test bits"},
		{kM2L_FCT_SIS_3302_GET_ENERGY_DECIMATION,		"get energy decimation"},
		{kM2L_FCT_SIS_3302_SET_ENERGY_DECIMATION,		"set energy decimation"},
		{kM2L_FCT_SIS_3302_READ_START_INDEX,			"read sample start index"},
		{kM2L_FCT_SIS_3302_WRITE_START_INDEX,			"write sample start index"},
		{kM2L_FCT_SIS_3302_READ_TAU_FACTOR, 			"read tau factor"},
		{kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR,			"write tau factor"},
		{kM2L_FCT_SIS_3302_GET_LEMO_IN_MODE,			"get lemo-in mode"},
		{kM2L_FCT_SIS_3302_SET_LEMO_IN_MODE,			"set lemo-in mode"},
		{kM2L_FCT_SIS_3302_GET_LEMO_OUT_MODE,			"get lemo-out mode"},
		{kM2L_FCT_SIS_3302_SET_LEMO_OUT_MODE,			"set lemo-out mode"},
		{kM2L_FCT_SIS_3302_GET_LEMO_IN_ENABLE_MASK,		"get lemo-in enable mask"},
		{kM2L_FCT_SIS_3302_SET_LEMO_IN_ENABLE_MASK,		"set lemo-in enable mask"},
		{kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE,			"get clock source"},
		{kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE,			"set clock source"},
		{kM2L_FCT_SIS_3302_START_TRACE_COLLECTION,		"start trace collection"},
		{kM2L_FCT_SIS_3302_CONT_TRACE_COLLECTION,		"continue trace collection"},
		{kM2L_FCT_SIS_3302_STOP_TRACE_COLLECTION,		"stop trace collection"},
		{kM2L_FCT_SIS_3302_GET_TRACE_DATA,				"get trace data"},
		{kM2L_FCT_SIS_3302_GET_TRACE_LENGTH,			"get trace length"},
		{kM2L_FCT_SIS_3302_DUMP_TRACE,					"dump trace data"},
		{kM2L_FCT_SIS_3302_ACCU_HISTOGRAM,				"accu histogram"},
		{kM2L_FCT_SIS_3302_RAMP_DAC,					"ramp dac"},
		{kM2L_FCT_SIS_3302_DUMP_REGISTERS,				"dump register contents"},
		{kM2L_FCT_SIS_3302_SET_VERBOSE_MODE,			"set verbose mode"},
//--------------------------------------------------------------------------------
		{kM2L_FCT_VULOM_TB, 							"exec function (VULOM / TRIGBOX)"},
		{kM2L_FCT_VULOM_TB_READ_SCALER, 				"read scaler"},
		{kM2L_FCT_VULOM_TB_READ_CHANNEL, 				"read channel"},
		{kM2L_FCT_VULOM_TB_ENABLE_CHANNEL,				"enable channel"},
		{kM2L_FCT_VULOM_TB_DISABLE_CHANNEL, 			"disable channel"},
		{kM2L_FCT_VULOM_TB_SET_ENABLE_MASK, 			"set enable mask"},
		{kM2L_FCT_VULOM_TB_GET_ENABLE_MASK, 			"get enable mask"},
		{kM2L_FCT_VULOM_TB_SET_SCALE_DOWN,				"set scale down"},
		{kM2L_FCT_VULOM_TB_GET_SCALE_DOWN,				"get scale down"},
		{kM2L_FCT_VULOM_TB_CLEAR_SCALER,				"clear scaler"},
//--------------------------------------------------------------------------------
		{0, 											NULL}
	};
#endif
