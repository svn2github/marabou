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
		{kM2L_MESS_BYE, 				"terminate|died"},
		{kM2L_MESS_ACK,					"acknowledge"},
		{kM2L_MESS_ACK_OK,					"ok, no message text"},
		{kM2L_MESS_ACK_MESSAGE,				"ok + text"},
		{kM2L_MESS_ACK_WARNING,				"warning"},
		{kM2L_MESS_ACK_ERROR,				"error"},
		{kM2L_MESS_VME_REQ,				"vme request"},
		{kM2L_MESS_VME_CONNECT,				"connect to module|return module handle"},
		{kM2L_MESS_VME_EXEC_FUNCTION,		"exec function|return function results"},
		{kM2L_MESS_EXEC_PROG,			"execute program"},
		{0, 							NULL}
	};

enum EM2L_FunctionType {
	kM2L_FCT_GET_MODULE_INFO	=	0x1, 					// get module info
	kM2L_FCT_CAEN_875			=	0x100,					// caen adc v875
	kM2L_FCT_SIS_3302			=	0x200,					// sis flash adc 3302
	kM2L_FCT_SIS_3302_SET_TIMEOUT,
	kM2L_FCT_SIS_3302_READ_DAC,
	kM2L_FCT_SIS_3302_WRITE_DAC,
	kM2L_FCT_SIS_3302_KEY_ADDR,
	kM2L_FCT_SIS_3302_READ_EVENT_CONFIG,
	kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG,
	kM2L_FCT_SIS_3302_SET_HEADER_BITS,
	kM2L_FCT_SIS_3302_GET_HEADER_BITS,
	kM2L_FCT_SIS_3302_GET_GROUP_ID,
	kM2L_FCT_SIS_3302_SET_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_GET_TRIGGER_MODE,
	kM2L_FCT_SIS_3302_SET_POLARITY,
	kM2L_FCT_SIS_3302_GET_POLARITY,
	kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH,
	kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH,
	kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY,
	kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY,
	kM2L_FCT_SIS_3302_READ_TRIG_GATE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_TRIG_GATE_LENGTH,
	kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH,
	kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX,
	kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX,
	kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR,
	kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR,
	kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE,
	kM2L_FCT_SIS_3302_READ_TRIG_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_WRITE_TRIG_PEAK_AND_GAP,
	kM2L_FCT_SIS_3302_READ_TRIG_PULSE_LENGTH,
	kM2L_FCT_SIS_3302_WRITE_TRIG_PULSE_LENGTH,
	kM2L_FCT_SIS_3302_READ_TRIG_THRESH,
	kM2L_FCT_SIS_3302_WRITE_TRIG_THRESH,
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
	kM2L_FCT_SIS_3302_GET_DECIMATION,
	kM2L_FCT_SIS_3302_SET_DECIMATION,
	kM2L_FCT_SIS_3302_READ_START_INDEX,
	kM2L_FCT_SIS_3302_WRITE_START_INDEX,
	kM2L_FCT_SIS_3302_READ_TAU_FACTOR,
	kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR,
	kM2L_FCT_SIS_3302_GET_EXTERNAL_TRIGGER,
	kM2L_FCT_SIS_3302_SET_EXTERNAL_TRIGGER,
	kM2L_FCT_SIS_3302_GET_EXTERNAL_TIMESTAMP_CLEAR,
	kM2L_FCT_SIS_3302_SET_EXTERNAL_TIMESTAMP_CLEAR,
	kM2L_FCT_SIS_3302_GET_INTERNAL_TRIGGER,
	kM2L_FCT_SIS_3302_SET_INTERNAL_TRIGGER,
	kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE,
	kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE,
	kM2L_FCT_SIS_3302_GET_SINGLE_EVENT,
	kM2L_FCT_SIS_3302_ACCU_HISTOGRAM,
	kM2L_FCT_SIS_3302_START_RUN,
};

const SMrbNamedXShort kMrbLofFunctionTypes[] =
	{
		{kM2L_FCT_GET_MODULE_INFO,		"get module info"},
		{kM2L_FCT_CAEN_875, 			"exec function (CAEN V785)"},
		{kM2L_FCT_SIS_3302, 			"exec function (SIS 3302)"},
		{kM2L_FCT_SIS_3302_SET_TIMEOUT,					"set timeout (cycles)"},
		{kM2L_FCT_SIS_3302_READ_DAC,					"read dac offsets"},
		{kM2L_FCT_SIS_3302_WRITE_DAC,					"write dac offsets"},
		{kM2L_FCT_SIS_3302_KEY_ADDR,					"exec key addr command"},
		{kM2L_FCT_SIS_3302_READ_EVENT_CONFIG,			"read event configuration"},
		{kM2L_FCT_SIS_3302_WRITE_EVENT_CONFIG,			"write event configuration"},
		{kM2L_FCT_SIS_3302_SET_HEADER_BITS,				"set header bits"},
		{kM2L_FCT_SIS_3302_GET_HEADER_BITS,				"get header bits"},
		{kM2L_FCT_SIS_3302_GET_GROUP_ID,				"get group id"},
		{kM2L_FCT_SIS_3302_SET_TRIGGER_MODE,			"set trigger mode"},
		{kM2L_FCT_SIS_3302_GET_TRIGGER_MODE,			"get trigger mode"},
		{kM2L_FCT_SIS_3302_SET_POLARITY,				"set polarity"},
		{kM2L_FCT_SIS_3302_GET_POLARITY,				"get polarity"},
		{kM2L_FCT_SIS_3302_READ_END_ADDR_THRESH,		"read end addr threshold"},
		{kM2L_FCT_SIS_3302_WRITE_END_ADDR_THRESH,		"write end addr threshold"},
		{kM2L_FCT_SIS_3302_READ_PRETRIG_DELAY,			"read pretrigger delay"},
		{kM2L_FCT_SIS_3302_WRITE_PRETRIG_DELAY, 		"write pretrigger delay"},
		{kM2L_FCT_SIS_3302_READ_TRIG_GATE_LENGTH,		"read trigger gate length"},
		{kM2L_FCT_SIS_3302_WRITE_TRIG_GATE_LENGTH,		"write trigger gate length"},
		{kM2L_FCT_SIS_3302_READ_RAW_DATA_SAMPLE_LENGTH,	"read raw data sample length"},
		{kM2L_FCT_SIS_3302_WRITE_RAW_DATA_SAMPLE_LENGTH,"write raw data sample length"},
		{kM2L_FCT_SIS_3302_READ_RAW_DATA_START_INDEX,	"read raw data start index"},
		{kM2L_FCT_SIS_3302_WRITE_RAW_DATA_START_INDEX,	"write raw data start index"},
		{kM2L_FCT_SIS_3302_READ_NEXT_SAMPLE_ADDR,		"read next sample address"},
		{kM2L_FCT_SIS_3302_READ_PREV_BANK_SAMPLE_ADDR,	"read previous bank sample address"},
		{kM2L_FCT_SIS_3302_READ_ACTUAL_SAMPLE,			"read actual sample"},
		{kM2L_FCT_SIS_3302_READ_TRIG_PEAK_AND_GAP,		"read trigger peak and gap times"},
		{kM2L_FCT_SIS_3302_WRITE_TRIG_PEAK_AND_GAP, 	"write trigger peak and gap times"},
		{kM2L_FCT_SIS_3302_READ_TRIG_PULSE_LENGTH,		"read trigger pulse length"},
		{kM2L_FCT_SIS_3302_WRITE_TRIG_PULSE_LENGTH, 	"write trigger pulse length"},
		{kM2L_FCT_SIS_3302_READ_TRIG_THRESH,			"read trigger threshold"},
		{kM2L_FCT_SIS_3302_WRITE_TRIG_THRESH,			"write trigger threshold"},
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
		{kM2L_FCT_SIS_3302_GET_DECIMATION,				"get decimation"},
		{kM2L_FCT_SIS_3302_SET_DECIMATION,				"set decimation"},
		{kM2L_FCT_SIS_3302_READ_START_INDEX,			"read sample start index"},
		{kM2L_FCT_SIS_3302_WRITE_START_INDEX,			"write sample start index"},
		{kM2L_FCT_SIS_3302_READ_TAU_FACTOR, 			"read tau factor"},
		{kM2L_FCT_SIS_3302_WRITE_TAU_FACTOR,			"write tau factor"},
		{kM2L_FCT_SIS_3302_GET_EXTERNAL_TRIGGER,		"get external trigger"},
		{kM2L_FCT_SIS_3302_SET_EXTERNAL_TRIGGER,		"set external trigger"},
		{kM2L_FCT_SIS_3302_GET_EXTERNAL_TIMESTAMP_CLEAR,	"get external timestamp clear"},
		{kM2L_FCT_SIS_3302_SET_EXTERNAL_TIMESTAMP_CLEAR,	"set external timestamp clear"},
		{kM2L_FCT_SIS_3302_GET_INTERNAL_TRIGGER,		"get internal trigger"},
		{kM2L_FCT_SIS_3302_SET_INTERNAL_TRIGGER,		"set internal trigger"},
		{kM2L_FCT_SIS_3302_GET_CLOCK_SOURCE,			"get clock source"},
		{kM2L_FCT_SIS_3302_SET_CLOCK_SOURCE,			"set clock source"},
		{kM2L_FCT_SIS_3302_GET_SINGLE_EVENT,			"get single event"},
		{kM2L_FCT_SIS_3302_ACCU_HISTOGRAM,				"accu histogram"},
		{kM2L_FCT_SIS_3302_START_RUN,					"start run"},
		{0, 											NULL}
	};
#endif
