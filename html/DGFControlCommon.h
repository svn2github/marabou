#ifndef __DGFControlCommon_h__
#define __DGFControlCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           DGFControlCommon.h
// Purpose:        Common defs for the DGF-4C GUI
// Description:    Defines common values to be used for DGFControl
// Author:         R. Lutter
// Revision:       
// Date:           
// URL:            
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <iomanip.h>

#include "TMrbNamedX.h"

// channel ids to be used by checkbuttons
enum EDGFChannels			{	kDGFChannel0			= BIT(12),
								kDGFChannel1			= BIT(13),
								kDGFChannel2			= BIT(14),
								kDGFChannel3			= BIT(15)
								};

enum						{	kDGFChannelMask			= kDGFChannel0 | kDGFChannel1 | kDGFChannel2 | kDGFChannel3 };

enum						{	kAutoWidth			= 1 		};
enum						{	kAutoHeight			= 1 		};


enum						{	kNofModulesPerCrystal	= 2 	};
enum						{	kNofModulesPerCluster	= 6 	};
enum						{	kNofClusters			= 20	};
enum						{	kNofCrates				= 8 	};

// lifo to store objects located on heap
#define H(x)	fHeap.AddFirst(x)

const SMrbNamedXShort kDGFChannelNumbers[] =
							{
								{kDGFChannel0,	"0" 		},
								{kDGFChannel1,	"1" 		},
								{kDGFChannel2,	"2" 		},
								{kDGFChannel3,	"3" 		},
								{0, 			NULL		}
							};
#endif
