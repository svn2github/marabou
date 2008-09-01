#if (CPU_TYPE == RIO2)
static char * cpuType = "@#@CPU=RIO2";
#if (LYNX_VERSION == 25)
	static char * lynxVersion = "@#@LynxOs=2.5";
#elif (LYNX_VERSION == 31)
	static char * lynxVersion = "@#@LynxOs=3.1";
#endif
#elif (CPU_TYPE == RIO3)
static char * cpuType = "@#@CPU=RIO3";
#if (LYNX_VERSION == 25)
	static char * lynxVersion = "@#@LynxOs=2.5";
#elif (LYNX_VERSION == 31)
	static char * lynxVersion = "@#@LynxOs=3.1";
#endif
#endif

