#ifndef __TMrbDGFCommon_h__
#define __TMrbDGFCommon_h__

#include "TMrbDGF.h"
#include "TMrbDGFData.h"
#include "TMrbNamedX.h"

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           xiadgf/inc/TMrbDGFCommon.h
// Purpose:        Common defs for XIA DGF-4C
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

// file types for DSP and FPGA code files, resp.
const SMrbNamedXShort kMrbDGFFileTypes[] =
			{
				{	TMrbDGFData::kFileUndef,				"Undefined" 							},
				{	TMrbDGFData::kFileAscii,				"Ascii" 								},
				{	TMrbDGFData::kFileBinary,				"Binary"								},
				{	0,										NULL									}
			};

// bit definitions in the control/status reg of the CAMAC System FPGA
const SMrbNamedXShort kMrbCamacICSRBits[] =
			{
				{	TMrbDGFData::kSystemFPGAReset,			"System FPGA Reset/Config"						},
				{	TMrbDGFData::kFippiFPGA0Reset,			"Fippi FPGA Chn0 Reset/Config"					},
				{	TMrbDGFData::kFippiFPGA1Reset,			"Fippi FPGA Chn1 Reset/Config"					},
				{	TMrbDGFData::kFippiFPGA2Reset,			"Fippi FPGA Chn2 Reset/Config"					},
				{	TMrbDGFData::kFippiFPGA3Reset,			"Fippi FPGA Chn3 Reset/Config"					},
				{	TMrbDGFData::kConnectDSPFromRight,		"Connect to DSP Trigger from RIGHT neighbor"	},
				{	TMrbDGFData::kConnectDSPFromLeft,		"Connect to DSP Trigger from LEFT neighbor" 	},
				{	TMrbDGFData::kTerminateDSP,				"Terminate DSP Trigger Bus (100 Ohm)"			},
				{	TMrbDGFData::kConnectFastFromRight,		"Connect to Fast Trigger from RIGHT neighbor"	},
				{	TMrbDGFData::kConnectFastFromLeft,		"Connect to Fast Trigger from LEFT neighbor"	},
				{	TMrbDGFData::kTerminateFast,			"Terminate Fast Trigger Bus (100 Ohm)"			},
				{	0,										NULL											}
			};

// bit definitions in the control/status reg of the CAMAC Fippi FPGA
const SMrbNamedXShort kMrbCamacCSRBits[] =
			{
				{	TMrbDGFData::kRunEna,					"Run Enable"							},
				{	TMrbDGFData::kNewRun,					"New Run"								},
				{	TMrbDGFData::kEnaLAM,					"LAM Enable"							},
				{	TMrbDGFData::kDSPReset, 				"Reset DSP" 							},
				{	TMrbDGFData::kFPGAReset,				"Reset FPGA"							},
				{	TMrbDGFData::kDSPError, 				"DSP Error" 							},
				{	TMrbDGFData::kActive,					"Run Active"							},
				{	TMrbDGFData::kLAMActive,				"LAM Active"							},
				{	0,										NULL									}
			};

// bit definitions in parameter MODCSRA: module control/status reg A
const SMrbNamedXShort kMrbModCSRABits[] =
			{
				{	TMrbDGFData::kWriteLevel1,				"Write Level1 Buffer"					},
				{	0,										NULL									}
			};

// value definitions for parameter RUNTASK: defines kind of run task to be started
const SMrbNamedXShort kMrbRunTasks[] =
			{
				{	TMrbDGFData::kRunControl,				"Slow Control Run"						},
				{	TMrbDGFData::kRunLinear,				"DAQ linear"							},
				{	TMrbDGFData::kRunCircular,				"DAQ circular"							},
				{	TMrbDGFData::kRunPSA	,				"DAQ PSA"								},
				{	TMrbDGFData::kRunShort,					"DAQ short" 							},
				{	TMrbDGFData::kRunMCA,					"MCA Mode"								},
				{	0,										NULL									}
			};

// value definitions for parameter CONTROLTASK: defines control tasks
const SMrbNamedXShort kMrbControlTasks[] =
			{
				{	TMrbDGFData::kProgramDACs,				"Program DACs"							},
				{	TMrbDGFData::kUpdateFPGA,				"Update FGPAs"							},
				{	TMrbDGFData::kCalibrate ,				"Calibrate ADCs"						},
				{	TMrbDGFData::kSampleADCs,				"Sample ADCs"							},
				{	TMrbDGFData::kReadHistoFirstPage,		"Read Histogram Memory (1st page)"		},
				{	TMrbDGFData::kReadHistoNextPage,		"Read Histogram Memory (next page)"		},
				{	0,										NULL									}
			};

// bit definitions in parameter CHANCSRA: control/status reg A for channel X
const SMrbNamedXShort kMrbChanCSRABits[] =
			{
				{	TMrbDGFData::kGroupTriggerOnly, 		"Respond to group triggers only"		},
				{	TMrbDGFData::kIndivLiveTime,			"Measure individual live time"			},
				{	TMrbDGFData::kGoodChannel,				"Good channel"							},
				{	TMrbDGFData::kReadAlways,				"Read always"							},
				{	TMrbDGFData::kEnableTrigger,			"Enable trigger"						},
				{	TMrbDGFData::kTriggerPositive,			"Trigger positive"						},
				{	TMrbDGFData::kGFLTValidate, 			"Validate events using GFLT"			},
				{	TMrbDGFData::kHistoEnergies,			"Histogram energies"					},
				{	TMrbDGFData::kHistoBaselines,			"Histogram baselines"					},
				{	TMrbDGFData::kCorrBallDeficit,			"Correct for ballistic deficit" 		},
				{	TMrbDGFData::kComputeCFT,				"Compute CFT"				},
				{	TMrbDGFData::kEnaMultiplicity,			"Enable contribution to multiplicity"	},
				{	TMrbDGFData::kBipolarSignals ,			"Bipolar signals"						},
				{	0,										NULL									}
			};

// bit definitions in soft status word (data)
const SMrbNamedXShort kMrbDGFStatusDBits[] =
			{
				{	TMrbDGFData::kSystemFPGACodeRead, 		"FPGA Code Read [System]"				},
				{	TMrbDGFData::kFippiFPGARevDCodeRead, 	"FPGA Code Read [Fippi, RevD]"			},
				{	TMrbDGFData::kFippiFPGARevECodeRead, 	"FPGA Code Read [Fippi, RevE]"			},
				{	TMrbDGFData::kDSPCodeRead,				"DSP Code Read"							},
				{	TMrbDGFData::kParamNamesRead,			"Param Names Read"						},
				{	0,										NULL									}
			};

// bit definitions in soft status word (module)
const SMrbNamedXShort kMrbDGFStatusMBits[] =
			{
				{	TMrbDGF::kLocalData,					"Local Data Base"						},
				{	TMrbDGF::kSystemFPGACodeLoaded,			"FPGA Code Loaded [System]"				},
				{	TMrbDGF::kFippiFPGACodeLoaded,			"FPGA Code Loaded [Fippi]"				},
				{	TMrbDGF::kDSPCodeLoaded,				"DSP Code Loaded"						},
				{	TMrbDGF::kParamValuesRead,				"Param Values Read" 					},
				{	TMrbDGF::kDSPRunning,					"DSP Running"							},
				{	0,										NULL									}
			};

// system or fippi fpga?
const SMrbNamedXShort kMrbFPGATypes[] =
			{
				{	TMrbDGFData::kSystemFPGA,				"SystemFPGA"							},
				{	TMrbDGFData::kFippiFPGA,				"FippiFPGA" 							},
				{	0,										NULL									}
			};

#endif
