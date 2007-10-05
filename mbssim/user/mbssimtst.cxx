#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Rtypes.h"

#include "TMrbLogger.h"

#include "MbsSimRdo.h"

#include "SetColor.h"

extern TMrbLogger * gMrbLog;
extern MbsSimRdo * gMbsSimRdo;

Int_t main(Int_t argc, Char_t * argv[]) {
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger("mbssimtst", "mbssim.log");
	gMbsSimRdo = new MbsSimRdo();
}
