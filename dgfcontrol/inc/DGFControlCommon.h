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

using namespace std;

#include <iostream>
#include <iomanip>

#include "TMrbNamedX.h"

// channel ids to be used by checkbuttons
enum EDGFChannels			{
									kDGFChannel0			= BIT(12),
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
#define HEAP(x)	fHeap.AddFirst(x)

const SMrbNamedXShort kDGFChannelNumbers[] =
							{
								{kDGFChannel0,	"0" 		},
								{kDGFChannel1,	"1" 		},
								{kDGFChannel2,	"2" 		},
								{kDGFChannel3,	"3" 		},
								{0, 			NULL		}
							};

// geometry settings
enum						{	kTabWidth 				= 1100					};
enum						{	kTabHeight 				= 850					};
enum						{	kVFrameWidth			= kTabWidth / 2 		};
enum						{	kVFrameHeight			= kTabHeight / 2 		};
enum						{	kLEWidth				= kAutoWidth			};
enum						{	kEntryWidth				= 100					};
enum						{	kFileEntryWidth			= 200					};
enum						{	kLEHeight				= 20					};
enum						{	kButtonWidth			= 400					};
enum						{	kButtonHeight			= 20					};

#endif
