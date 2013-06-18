#ifndef __BMA_ERRORS__
#define __BMA_ERRORS__

char * bmaErrlist[] = {
        "[BMA_OK] success",
        "[BMA_E_Error] unspecific err",
        "[BMA_E_NotImp] not (yet) implemented",
        "[BMA_E_NoUio] no 'User I/O' driver",
        "[BMA_E_NoMem] cannot allocate memory",
        "[BMA_E_NoMod] cannot allocate mode structure",
        "[BMA_E_IllMod] illegal mode ID",
        "[BMA_E_UnsPar] unsupported mode parameter",
        "[BMA_E_IllPar] illegal mode parameter",
        "[BMA_E_IllVal] illegal mode parameter value",
        "[BMA_E_PciTa] PCI Target Abort",
        "[BMA_E_VmeBerr] VME Bus Error",
        "[BMA_E_VmeRtry] VME Retry",
        "[BMA_E_VmeBto] VME Bus Timeout",
        "[BMA_E_Wcnt] WordCount not zero",
        "[BMA_E_ChOvf] Chain Overflow",
        "[BMA_E_NoStat] No status found in chain",
        "[BMA_E_InvPtr] Invalid (chain) pointer"
};
#endif
