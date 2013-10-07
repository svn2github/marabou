//////////////////////////////////////////////////////////////////////
//
// Request.cpp: implementation of the Request class.
//
// This module is a part of the PMCA COM package. It contains bodies
// of the EXEC and MONITOR threads and functions supporting request
// processing.
//
// Amptek Inc. 2000
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <process.h>
#include <time.h>
#include "Request.h"
#include "MicroTime.h"

#define RETRY_COUNT 10
#define NORMAL_EXEC_PRIORITY   0
#define MAX_RECEIVE_BUFFER  1024

#define CMD_GET_DATA        0x00 
#define CMD_GET_DATA_GROUP  0x10   
#define CMD_SET_CONTROL     0x01   
#define CMD_SET_ACQ_TIME    0x02   
#define CMD_DELETE          0x05   
#define CMD_SET_DATE_19     0x19
#define CMD_SET_DATE_20     0x20
#define CMD_SET_TIME        0x25
#define CMD_GET_TIME_STAMP  0x30
#define CMD_SET_GROUP       0x11
#define CMD_SET_LOCK        0x75 

#define SetDtr() s_dtr.Set()
#define ResetDtr() s_dtr.Reset()
#define SetRts() EscapeCommFunction(s_comHandle, SETRTS)
#define ResetRts() EscapeCommFunction(s_comHandle, CLRRTS)
#define RemoveRequest(requestId) \
    (RequestType *) s_queue.Remove(requestId)
#define EnqueueRequest(request) s_queue.Put(&request->queueEntry)

static void ExecThreadProc(LPVOID param);
static void MonitorThreadProc(LPVOID param);
static void ExecDisconnect(void);
static int SetExecPriority(int priority);
static int ActivateThreads(void);
static int SendCommandData(WORD group);
static int SendCommandDataGroup(WORD group);
static int ReceiveStatus(PmcaStatusType *buffer);
static int ReceiveStatusWithPrompt(PmcaStatusType *status);
static int ReceiveStatusWithRetry(PmcaStatusType *status);
static int ExecWaitForRequestCompletion(RequestType *request,
        BOOL enqueu, BOOL critical);
static void ReturnResult(RequestType *request, int result);
static void RemoveAllRequests(void);

#pragma pack(push)
#pragma pack(1)

// Command Type structure (5 bytes long)
// Used in exchange with PMCA
typedef struct
{
    BYTE code;
    union
    {
        struct
        {
            BYTE b1;
            BYTE b2;
            BYTE b3;
        } b;
        struct
        {
            WORD group;
            BYTE baudRateDivisor;
        } data;
        struct
        {
            WORD lWord;
            BYTE hByte;
        } acqTime;  // 24 - bit unsigned value
        PmcaControlType control;
        struct
        {
            BYTE data;
            BYTE time;
            BYTE spare;
        } clear;
        struct
        {
            BYTE year;
            BYTE month;
            BYTE day;
        } dateStamp;
        struct
        {
            BYTE hours;
            BYTE minutes;
            BYTE seconds;
        } timeStamp;
        struct
        {
            BYTE spare1;
            BYTE number;
            BYTE spare2;
        } group;
        struct
        {
            WORD code;
            BYTE spare;
        } lock;
    } u;
    BYTE checkSum;
} CommandType;  

#pragma pack(pop)

// Module static variables
static HANDLE s_comHandle = 0;
static HANDLE s_execHandle;
static HANDLE s_monitorHandle;
static HANDLE s_waitExecTermination;
static HANDLE s_waitMonitorTermination;
static DWORD s_byteSent = 0;
static DWORD s_byteReceived = 0;
static int s_maxBaudRate;
static int s_baudRateDivisor;
static int s_receiveBufferSize;
static int s_serialNumber;
static int s_gainCode;
static int s_priority = NORMAL_EXEC_PRIORITY;
static WORD s_portNumber;
static PmcaDeviceType s_device;
static RequestType *s_currentRequest = NULL;
static BOOL s_sending;
static BOOL s_receiving;
static BOOL s_connected = FALSE;
static BOOL s_terminate = FALSE;
static BOOL s_threadsActivated = FALSE;
static CRITICAL_SECTION s_requestCritical;
static QueueType s_queue;
static MicroTimeType s_mTime;
static BYTE newCodeTable[OP_END_CODE] = {0};
static bool isUSBMCA = false;

class DtrType
{
    BOOL m_dtr;
public:
    void Set(void)
    {
        EscapeCommFunction(::s_comHandle, SETDTR);
        m_dtr = TRUE;
        return;
    }
    void Reset(void)
    {
        EscapeCommFunction(::s_comHandle, CLRDTR);
        m_dtr = FALSE;
        return;
    }
    void GetByte(void)
    {
        DWORD escapeCode;
        // Toggle DTR to signal "ready to receive" to PMCA
        m_dtr = !m_dtr;
        escapeCode = m_dtr ? SETDTR : CLRDTR;
        EscapeCommFunction(::s_comHandle, escapeCode);
        ++::s_byteReceived;
        ::s_byteReceived &= 0x7FFFFFFF;
        return;
    }
}; // End of class DtrType

static DtrType s_dtr;

static int const PriorityTable[] =
{
    THREAD_PRIORITY_IDLE,
    THREAD_PRIORITY_LOWEST,
    THREAD_PRIORITY_BELOW_NORMAL,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST,
    THREAD_PRIORITY_TIME_CRITICAL
};

// DllMain

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID load)
{
    if (reason == DLL_PROCESS_DETACH)
        TerminateCom();
    return TRUE;
}

// Exported functions

PmcaDeviceType GetDeviceType(void)
{
    return(s_device);
}

int TerminateCom(void)
{
    DWORD waitStat;
    RequestType *request;
    int stat = PMCA_ERROR_SUCCESS;
    if (!s_threadsActivated)
        goto L_Leave;
    s_waitExecTermination =
            CreateEvent(NULL, TRUE, FALSE, NULL);
    s_waitMonitorTermination =
            CreateEvent(NULL, TRUE, FALSE, NULL);
    RemoveAllRequests();
    request = AllocateRequest(OP_TERMINATE, NULL);
    request->wait = NULL;
    PutRequest(request);
    s_terminate = TRUE;
    waitStat = WaitForSingleObject(
            s_waitExecTermination, 10000);
    CloseHandle(s_waitExecTermination);
    if (waitStat == WAIT_TIMEOUT)
        stat = PMCA_ERROR_TERMINATION_FAILED;
    s_threadsActivated = FALSE;
    RemoveAllRequests();    // Clear the queue again
    DeleteCriticalSection(&s_requestCritical);
    // Requests posted before TerminateCom returns are lost
L_Leave:
    return stat;
} // End of TerminateCom

int SetPriority(int priority)
{
    int stat = SetExecPriority(priority);
    if (!stat)
        s_priority = priority;
    return stat;
}

int CancelRequest(int requestId)
{
    int stat = PMCA_ERROR_SUCCESS;
    RequestType *request = RemoveRequest(requestId);
    if (request)
    {
        if (request->result)
            ReturnResult(request, PMCA_ERROR_REQUEST_CANCELLED);
        FreeRequest(request);
    }
    else
        stat = PMCA_ERROR_REQUEST_NOT_FOUND;
    return stat;
} // End of CancelRequest
    
int WaitForRequestCompletion(int requestId)
{
    int stat;
    RequestType *request =
            (RequestType *) s_queue.LockedFind(requestId);
    if (!request)
    {
        // Check currently executed request
        EnterCriticalSection(&s_requestCritical);
        if (s_currentRequest &&
                s_currentRequest->queueEntry.id == requestId)
            stat = ExecWaitForRequestCompletion(
                    s_currentRequest,
                    FALSE,  // Don't enqueue
                    TRUE);  // In critical section
        else
        {
            LeaveCriticalSection(&s_requestCritical);
            stat = PMCA_ERROR_REQUEST_NOT_FOUND;
        }
    }
    else
        // Queue access is unlocked in the following procedure
        stat = ExecWaitForRequestCompletion(request, FALSE, FALSE);
    return stat;
} // End of WaitForRequestCompletion

// Function : PutRequest
// return   : request ID or error code
// errors   : PMCA_ERROR_PMCA_DISCONNECTED
//          : from ActivateThreads
int PutRequest(RequestType *request)
{
// This function enqueues the request and if "wait" field is set
// waits for the operation completion
    int stat;
    if (!s_connected)
    {
        if (request->code == OP_CONNECT ||
                request->code == OP_POWER_UP)
        {
            if (!s_threadsActivated)
            {
                stat = ActivateThreads();
                if (stat)
                    goto L_Error;
            }
        }
        else if (request->code != OP_TERMINATE ||
                !s_threadsActivated)
        {
            stat = PMCA_ERROR_PMCA_DISCONNECTED;
            goto L_Error;
        }
    }
    if (s_device == PMCA_8000 && newCodeTable[request->code])
    {
        stat = PMCA_ERROR_INVALID_REQUEST;
        goto L_Error;
    }
    if (request->wait)
        // Enqueue request and wait
        stat = ExecWaitForRequestCompletion(request, TRUE, FALSE);
    else
        stat = EnqueueRequest(request);
L_Leave:
    return stat;
L_Error:
    delete request;
    goto L_Leave;
} // End of PutRequest

// Function : AllocateRequest
// return   : request record
// errors   : none
RequestType *AllocateRequest(OpCodeType code, int *result)
{
    RequestType *request;
    request = new RequestType;
    request->code = code;
    request->result = result;
    request->waitCount = 0;
    // If result field is set then request should be asynchronous
    request->wait = result ? NULL : (void *) 1;
    return request;
} // End of AllocateRequest

// Local functions

// Request utility functions

static void FreeRequest(RequestType *request)
{
    BOOL success;
    EnterCriticalSection(&s_requestCritical);
    if (request->wait)
    {
        if (request->waitCount)
        {
            // Release wait if someone is waiting
            --request->waitCount;
            success = SetEvent(request->wait);
        }
        else
        {
            success = CloseHandle(request->wait);
            delete request;
        }
    }
    else
        delete request;
    LeaveCriticalSection(&s_requestCritical);
    return;
} // End of FreeRequest

static void ReturnResult(RequestType *request, int result)
{
    try // Result pointer can be bad
    {
        *request->result = result;
    }
    catch(...)
    {}  // Nothing we can do about it
    return;
} // End of ReturnResult

static void RemoveAllRequests(void)
{
    RequestType *request = RemoveRequest(-1);
    while (request)
    {
        if (request->result)
            ReturnResult(request, PMCA_ERROR_REQUEST_CANCELLED);
        FreeRequest(request);
        request = RemoveRequest(-1);
    }
    return;
}

// Control functions

static int SetExecPriority(int priority)
{
    int stat = PMCA_ERROR_SUCCESS;
    if (s_threadsActivated)
    {
        BOOL success = SetThreadPriority(s_execHandle,
                PriorityTable[priority]);
        if (!success)
            stat = PMCA_ERROR_SET_PRIORITY_FAILED;
    }
    return stat;
}

// Function : ExecWaitForRequestCompletion
// input    : request
//          : enque - request must be placed in the queue
//          : critical - current request, protected by
//          :       s_requestCritical critical section
// return   : error status
// comment  : This function is called from application threads
//          : through WaitForRequestCompletion and PutRequest
static int ExecWaitForRequestCompletion(RequestType *request,
        BOOL enqueu, BOOL critical)
{
    int stat;
    int result;
    if (!request->waitCount)
        request->wait = CreateEvent(
                NULL,   // No security
                FALSE,  // Auto-reset
                FALSE,  // Non-signaled
                NULL);  // Unnamed
    ++request->waitCount;
    if (!request->result)
        request->result = &result;
    if (critical)
        LeaveCriticalSection(&s_requestCritical);
    else if (enqueu)
        stat = EnqueueRequest(request);
    else    // Request already in the queue - unlock access
        s_queue.UnlockAccess();
    // Wait for the operation completion
    stat = WaitForSingleObject(request->wait, INFINITE);
    if (stat != WAIT_OBJECT_0)
        stat = GetLastError();
    // The operation has been completed or cancelled
    try
    {
        stat = *request->result;
    }
    catch(...)
    {
        stat = PMCA_ERROR_INVALID_DATA_POINTER;
    }
    FreeRequest(request);
    return stat;
} // End of ExecWaitForRequestCompletion

// Communication utility functions

static int GetModemStatus(DWORD *modemStatus)
{
    int stat = 0;
    BOOL success = GetCommModemStatus(s_comHandle, modemStatus);
    if (!success)
        stat = GetLastError();
    return stat;
}

static void CloseComPort(void)
{
    if (s_comHandle)
        CloseHandle(s_comHandle);
    s_comHandle = NULL;
    return;
}

static void ExecDisconnect(void)
{
    RemoveAllRequests();
    CloseComPort();
    s_connected = FALSE;
    return;
}

// Function : ActivateThreads
// return   : error code
// errors   : PMCA_ERROR_STARTING_PMCA_COM_THREADS;
// comment  : starts exec and monitor threads; the monitor thread runs
//          : at THREAD_PRIORITY_TIME_CRITICAL and the exec thread
//          : initially runs at THREAD_PRIORITY_IDLE; the application
//          : can change the exec priority using the PmcaSetPriority
//          : API function
static int ActivateThreads(void)
{
    int stat;
    s_terminate = FALSE;
    s_execHandle = (HANDLE) _beginthread(ExecThreadProc, 0, NULL);
    if (s_execHandle == INVALID_HANDLE_VALUE)
        stat = PMCA_ERROR_STARTING_COM_THREADS;
    else
    {
        s_threadsActivated = TRUE;
        InitializeCriticalSection(&s_requestCritical);
        stat = SetExecPriority(s_priority);
        s_monitorHandle = (HANDLE) _beginthread(MonitorThreadProc, 0, NULL);
        if (s_monitorHandle == INVALID_HANDLE_VALUE)
        {
            stat = TerminateCom();
            stat = PMCA_ERROR_STARTING_COM_THREADS;
        }
        else
        {
            BOOL success = SetThreadPriority(
                    s_monitorHandle,
                    THREAD_PRIORITY_TIME_CRITICAL);
            stat = PMCA_ERROR_SUCCESS;
            newCodeTable[OP_GET_SERIAL_NUMBER] = 1;
            newCodeTable[OP_GET_GROUP] = 1;
            newCodeTable[OP_GET_THRESHOLD] = 1;
            newCodeTable[OP_GET_DATE_TIME] = 1;
            newCodeTable[OP_SET_TIME] = 1;
            newCodeTable[OP_SET_DATE] = 1;
            newCodeTable[OP_SET_LOCK] = 1;
        }
    }
    return stat;
} // End of ActivateThreads

// Function : OpenComPort
// input    : port number (1 .. MAX_COM_PORT)
// return   : error code
// errors   : system
//          : PMCA_ERROR_BAD_SERIAL_PORT
//          : PMCA_ERROR_SERIAL_PORT_CAPABILITIES
static int OpenComPort(WORD port)
{
    HANDLE hCom;
    BOOL success;
    int stat = PMCA_ERROR_SUCCESS;
    int transmitBufferSize;
    DCB dcb;
    COMMPROP prop;
    COMMTIMEOUTS timeouts;
    char portName[] = "com0";

    if (s_comHandle)
        CloseComPort();
    portName[3] = '0' + port;
    hCom = CreateFile(
        portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
//        FILE_FLAG_NO_BUFFERING,
        NULL);
    if (hCom == INVALID_HANDLE_VALUE)
    {
        stat = GetLastError();
        goto L_Leave;
    }
    success = GetCommProperties(hCom, &prop);
    if (!success)
        goto L_Error;
    if (prop.dwServiceMask != SP_SERIALCOMM ||
            prop.dwProvSubType != PST_RS232)
    {
        stat = PMCA_ERROR_BAD_SERIAL_PORT;
        goto L_Close;
    }
    if (!(prop.dwProvCapabilities & PCF_RTSCTS) ||
            !(prop.dwProvCapabilities & PCF_DTRDSR))
    {
        stat = PMCA_ERROR_SERIAL_PORT_CAPABILITIES;
        goto L_Close;
    }
    s_maxBaudRate = prop.dwMaxBaud > MAX_BAUD_RATE ?
            MAX_BAUD_RATE : prop.dwMaxBaud;
    s_receiveBufferSize = !prop.dwMaxRxQueue ||
            prop.dwMaxRxQueue > MAX_RECEIVE_BUFFER ?
            MAX_RECEIVE_BUFFER : prop.dwMaxRxQueue;
    transmitBufferSize = !prop.dwMaxTxQueue ||
            prop.dwMaxRxQueue > 32 ?
            32 : prop.dwMaxRxQueue;
    success = SetupComm(hCom,
            s_receiveBufferSize, transmitBufferSize);
    if (!success)
        goto L_Error;

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
// Read operation returns immediately if there is anything
// in the read queue otherwise timeouts in 10 milliseconds
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    success = SetCommTimeouts(hCom, &timeouts);
    if (!success)
        goto L_Error;
    success = GetCommState(hCom, &dcb);
    if (!success)
        goto L_Error;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fNull = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError = FALSE;
    dcb.ByteSize = 8;
    dcb.Parity = SPACEPARITY;
    dcb.StopBits = ONESTOPBIT;

    success = SetCommState(hCom, &dcb);
    if (!success)
        goto L_Error;
    s_comHandle = hCom;
    s_portNumber = port;
L_Leave:
    return stat;
L_Error:
    stat = GetLastError();
    goto L_Leave;
L_Close:
    CloseHandle(hCom);
    goto L_Leave;
} // End of OpenComPort

// Function : ExecConnect
// return   : error code
// errors   : system
//          : PMCA_ERROR_CONNECTION_FAILED
static int ExecConnect(RequestType *request)
{
    int stat = 0;
    BOOL success;
    PmcaDeviceType device = request->p.connect.device;
    int baudRate = request->p.connect.baudRate;
    DCB dcb;
    PmcaStatusType status;

    if (s_connected)
        ExecDisconnect();
    stat = OpenComPort(request->p.connect.port);
    if (stat)
        goto L_Leave;
    if (baudRate > s_maxBaudRate)
        baudRate = s_maxBaudRate;
    success = GetCommState(s_comHandle, &dcb);
    if (!success)
        goto L_Error;
    s_baudRateDivisor = MAX_BAUD_RATE / baudRate;
    s_connected = TRUE;
    // PMCA expects initial connection at 4800 baud
    dcb.BaudRate = 4800;
    success = SetCommState(s_comHandle, &dcb);
    if (!success)
        goto L_Error;
    // DTR must be high for autoconnect
    SetDtr();
    stat = SendCommandData(0);
    ResetDtr();
    if (!stat)
    {
        success = GetCommState(s_comHandle, &dcb);
        dcb.BaudRate = baudRate;
        success = SetCommState(s_comHandle, &dcb);
        if (!success)
            goto L_Error;
    }
    else
        goto L_ClosePort;
    
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        // Repeat to clear CheckSum
        stat = ReceiveStatusWithPrompt(&status);
    if (stat)
        goto L_ClosePort;
    s_serialNumber = status.u.gsn.serialNumber;
    if (!s_serialNumber)
        s_device = PMCA_8000;
    else
        s_device = PMCA_8000A;
    if (device != PMCA_AUTO_DETECT && device != s_device)
    {
        stat = PMCA_ERROR_INVALID_DEVICE;
        goto L_ClosePort;
    }
    
    s_gainCode = status.control.flags.bits.gain;
L_Leave:
    return (stat);
L_Error:
    stat = GetLastError();
L_ClosePort:
    s_connected = FALSE;
    CloseComPort();
    goto L_Leave;
} // End of ExecConnect

// Function : ReceivePacket
// return   : error code
// errors   : system
//          : PMCA_ERROR_READING_COM_PORT
// comment : count must be less than s_receiveBufferSize
static int ReceivePacket(BYTE *buffer, int count)
{
    BOOL success;
    int stat;
    DWORD bytes;
    int i;
    int remCount = count;
    BYTE *buffPtr = buffer;
    BOOL dtr = FALSE;
    double timeOut;
    double t;
    double byteTime = s_baudRateDivisor * (15000000.0 / MAX_BAUD_RATE);
    // in microseconds
    double baudRateRCV = (double)(MAX_BAUD_RATE) / (double)(s_baudRateDivisor);
    double byteTime_uS = (1.0 + 11.0) * (1.0 / baudRateRCV) * 1000000.0;

    for (i = count; i--;)
    {
        if (isUSBMCA) {
            s_mTime.Wait(byteTime_uS);
        } else {
        s_dtr.GetByte();    // Toggle DTR
        s_mTime.Wait(byteTime);
        }
    }

    if (isUSBMCA) {
        timeOut = s_mTime.Get() + 750000.0;     // time out in 0.75 second
    } else {
    s_mTime.Wait(200);
    timeOut = s_mTime.Get() + 500000.0;     // time out in 0.5 second
    }
    for (;;)
    {
        success = ReadFile(s_comHandle,
                buffPtr, remCount, &bytes, NULL);
        if (!success)
        {
            stat = GetLastError();
            break;
        }
        else
        {
            remCount -= bytes;
            if (!remCount)
            {
                stat = PMCA_ERROR_SUCCESS;
                break;
            }
            else
            {
                t = s_mTime.Get();
                if (t > timeOut)
                {
                    stat = PMCA_ERROR_READING_COM_PORT;
                    break;
                }
            }
            buffPtr += bytes;
        }
    }
    return (stat);
} // End of ReceivePacket

// Function : Receive
// return   : error code
// errors   : from ReceivePacket
// comment  : PMCA must be ready to send
//          : dtr must be reset in the beginning of receive sequence
static int Receive(BYTE *buffer, int count)
{
    BYTE *buff = buffer;
    int remCount = count;   // Remaining bytes
    int packSize;           // Packet size
    int stat = PMCA_ERROR_SUCCESS;
    if (! isUSBMCA) {
    BOOL success = PurgeComm(s_comHandle, PURGE_RXCLEAR); 
    }
    s_receiving = TRUE;

    while (remCount)
    {
        // make sure that packSize < receive buffer size
        packSize = remCount > s_receiveBufferSize ?
                s_receiveBufferSize : remCount;
        stat = ReceivePacket(buff, packSize);
        if (stat)
            break;
        remCount -= packSize;
        buff += packSize;
    }
    s_receiving = FALSE;
    return stat;
} // End of Receive

// Function : WaitForDsrToggle
// output   : modemStatus (new modem status)
// return   : error code
// errors   : system
//          : PMCA_ERROR_LOST_CONNECTION
static int WaitForDsrToggle(DWORD *modemStatus)
{
    int stat = 0;
    DWORD newModemStatus;
    double endTime = s_mTime.Get() + 200000.0; // 200 milliseconds
    while (s_mTime.Get() < endTime)
    {
        if (!s_connected)
        {
            stat = PMCA_ERROR_LOST_CONNECTION;
            break;
        }
        stat = GetModemStatus(&newModemStatus);
        if (stat)
            break;
        if ((newModemStatus ^ *modemStatus) & MS_DSR_ON)
        {
            *modemStatus = newModemStatus;
            stat = PMCA_ERROR_SUCCESS;
            break;
        }
    }
    return stat;
} // End of WaitForDsrToggle

// Function : WaitForCtsToggle
// output   : modemStatus (new modem status)
// return   : error code
// errors   : system
//          : PMCA_ERROR_LOST_CONNECTION
static int WaitForCtsToggle(DWORD *modemStatus)
{
    int stat = 0;
    DWORD newModemStatus;
    double endTime = s_mTime.Get() + 200000.0; // 200 milliseconds
    while (s_mTime.Get() < endTime)
    {
        if (!s_connected)
        {
            stat = PMCA_ERROR_LOST_CONNECTION;
            break;
        }
        stat = GetModemStatus(&newModemStatus);
        if (stat)
            break;
        if ((newModemStatus ^ *modemStatus) & MS_CTS_ON)
        {
            *modemStatus = newModemStatus;
            stat = PMCA_ERROR_SUCCESS;
            break;
        }
    }
    return stat;
} // End of WaitForCtsToggle

// Function : SendCommand
// input    : pointer to command structure
// return   : error code
// errors   : system
//          : PMCA_ERROR_LOST_CONNECTION
static int SendCommand(CommandType *command)
{
    int stat;
    BOOL success;
    DWORD modemStatus;
    int i, j;
    BYTE *buff = (BYTE *) command;
    int cmdSize = sizeof(CommandType);
    int checkSum = (int) command->code + command->u.b.b1 +
            command->u.b.b2 + command->u.b.b3;
    command->checkSum = (BYTE) (checkSum & 0xFF);
    stat = GetModemStatus(&modemStatus);
    if (stat)
        goto L_Leave;

    s_sending = TRUE;
    for (j = RETRY_COUNT; ;)
    {
        SetRts();   // Prepare PMCA to receive command
        if (isUSBMCA) {
            stat = WaitForCtsToggle(&modemStatus);
            if (stat)
                break;
            else
            {
                for (i = -1; ++i < sizeof(CommandType);)
                {
                    DWORD bytes;
                    success = WriteFile(s_comHandle,
                            &buff[i], 1, &bytes, NULL);
                    if (!success)
                    {
                        stat = GetLastError();
                        break;
                    }
                }
                s_byteSent++;
                s_byteSent &= 0x7FFFFFFF;
            }
            success = PurgeComm(s_comHandle, PURGE_RXCLEAR);
            stat = WaitForCtsToggle(&modemStatus);
        } else {
        for (i = -1; ++i < sizeof(CommandType);)
        {
            stat = WaitForDsrToggle(&modemStatus);
            if (stat)
                break;
            else
            {
                DWORD bytes;
                success = WriteFile(s_comHandle,
                        &buff[i], 1, &bytes, NULL);
                if (!success)
                {
                    stat = GetLastError();
                    break;
                }
            }
            s_byteSent++;
            s_byteSent &= 0x7FFFFFFF;
        }
        stat = WaitForDsrToggle(&modemStatus);
        }
        ResetRts(); // Tell PMCA to exit the receive state
        if (!stat || !(--j))
            break;
        // Retry after 20 milliseconds
        if (! isUSBMCA)
            Sleep(20);
    }
    s_sending = FALSE;
    s_mTime.Wait(200.0);
    // Now PMCA should be ready to send status
L_Leave:
    return stat;
} // End of SendCommand

static int SendCommandData(WORD group)
{
    CommandType command;
    int stat;
    command.code = CMD_GET_DATA;
    command.u.data.group = group;
    command.u.data.baudRateDivisor = s_baudRateDivisor;
    stat = SendCommand(&command);
    return stat;
} // End of SendCommandData

static int SendCommandDataEx(WORD group, WORD count)
{
    CommandType command;
    int stat;
    BYTE isUpper;
    BYTE groupH;
    BYTE groupL;
    BYTE countH;
    BYTE countL;
    isUpper = group & 0x02;
    countH = (count & 0x300) >> 8;
    countL = count & 0xFF;
    groupH = (group & 0xFF00) >> 8;
    groupL = (group & 0xFC) + countH;
    command.code = CMD_GET_DATA + 0x40 + isUpper;
    //command.u.data.group = group;
    command.u.b.b1 = groupL;
    command.u.b.b2 = groupH;
    command.u.b.b3 = countL;
    //command.u.data.baudRateDivisor = s_baudRateDivisor;
    stat = SendCommand(&command);
    return stat;
} // End of SendCommandData

static int SendCommandDataGroup(WORD group)
{
    CommandType command;
    int stat;
    command.code = CMD_GET_DATA_GROUP;
    command.u.data.group = group;
    command.u.data.baudRateDivisor = s_baudRateDivisor;
    stat = SendCommand(&command);
    return stat;
} // End of SendCommandDataGroup

static int SendByteCommand(BYTE code, BYTE b1, BYTE b2, BYTE b3)
{
    CommandType command;
    int stat;
    command.code = code;
    command.u.b.b1 = b1;
    command.u.b.b2 = b2;
    command.u.b.b3 = b3;
    stat = SendCommand(&command);
    return stat;
} // End of SendByteCommand

static int SendControl(PmcaStatusType *status)
{
    CommandType command;
    int stat;
    command.code = CMD_SET_CONTROL;
    command.u.control = status->control;
    stat = SendCommand(&command);
    return stat;
} // End of SendControl

static int ReceiveDateTime(DateTimeType *stamp)
{
    int stat;
    int i;
    BYTE temp[TIME_STAMP_SIZE];
    for (i = RETRY_COUNT; i--;)
    {
        if (isUSBMCA) {
            BOOL success = PurgeComm(s_comHandle, PURGE_RXCLEAR);
        }
        stat = SendByteCommand(CMD_GET_TIME_STAMP, 1, 1, 1);
        if (stat)
            continue;
        stat = Receive(temp, TIME_STAMP_SIZE);
        if (!stat)  // If success exit
        {
            BYTE *ptr = (BYTE *) stamp;
            int j;
            // Reverse order
            for (j = TIME_STAMP_SIZE; j--;)
            {
                *ptr = temp[j];
                ++ptr;
            }
            break;
        }
    }
    return stat;
} // End of ReceiveTimeStamp
    
static int PromptPmcaForStatus(void)
{
    int stat;
    BOOL success;
    DWORD modemStatus;
    // First clear RTS to make sure that PMCA is in the send state
    ResetRts();
    s_mTime.Wait(200.0);
    stat = GetModemStatus(&modemStatus);
    if (!stat)
    {
        // Switch PMCA to the receive state
        SetRts();
        if (isUSBMCA) {
            // Wait until PMCA goes into the receive state (CTS is toggled)
            stat = WaitForCtsToggle(&modemStatus);
            success = PurgeComm(s_comHandle, PURGE_RXCLEAR);
        } else {
        // Wait until PMCA goes into the receive state (DSR is toggled)
        stat = WaitForDsrToggle(&modemStatus);
        }
        // Switch PMCA to the send status state
        ResetRts();
        s_mTime.Wait(200.0);    // Wait few hundred microseconds
        // Now PMCA is going to send status when requested to send
    }
    return stat;
} // End of PromptPmcaForStatus

static int ReceiveStatus(PmcaStatusType *buffer)
{
    // PMCA must be in the send status state when this function
    // is called
    BYTE tempBuff[STATUS_SIZE];    // Local buffer
    int stat;
    stat = Receive(tempBuff, STATUS_SIZE);
    if (!stat)
    {
        BYTE *ptr = (BYTE *) buffer;
        BYTE oneByte;
        BYTE byteCheckSum = tempBuff[STATUS_SIZE - 1];
        int checkSum = -byteCheckSum;
        int i;
        // Information has been received in reversed order
        *ptr = byteCheckSum;
        ++ptr;
        for (i = STATUS_SIZE - 1; i--;)
        {
            oneByte = tempBuff[i];
            checkSum += oneByte;
            *ptr = oneByte;
            ++ptr;
        }
        if ((checkSum & 0xFF))
            stat = PMCA_ERROR_INVALID_STATUS_CHECK_SUM;
    }
    return stat;
} // End of ReceiveStatus

static int ReceiveStatusWithRetry(PmcaStatusType *status)
{
    int i;
    int stat;
    for (i = RETRY_COUNT; i--;)
    {
        stat = ReceiveStatus(status);
        if (!stat)
            break;
        stat = PromptPmcaForStatus();
        if (stat)
            break;
    }
    return stat;
} // End of ReceiveStatusWithRetry

static int ReceiveStatusWithPrompt(PmcaStatusType *status)
{
    int stat = PromptPmcaForStatus();
    if (!stat)
        stat = ReceiveStatusWithRetry(status);
    return stat;
} // End of ReceiveStatusWithPrompt

static int GetStatusWithGroup(PmcaStatusType *status)
{
    int stat;
    if (s_device == PMCA_8000A)
    {
        int i;
        for (i = RETRY_COUNT; i--;)
        {
            stat = SendCommandDataGroup(0);
            if (!stat)
            {
                Sleep(1);
                stat = ReceiveStatus(status);
            }
            if (!stat)
                break;
        }
    }
    else
        stat = PMCA_ERROR_INVALID_REQUEST;
    return stat;
} // End of ReceiveStatusWithGroup

// Function : ReceiveDataWords
// input    : group (group number * 4 + odd(2)/even(0) indicator)
//          : wordCount (number of words)
// output   : buffer
// return   : error code
// errors   : PMCA_ERROR_INVALID_DATA_CHECK_SUM
static int ReceiveDataWords(BYTE *buffer, int group, int wordCount)
{
    int remCount;   // Remaining bytes
    int blockSize;
    int blockSizeEx;
    int i, j;
    int stat;
    BYTE tempBuff[MAX_RECEIVE_BUFFER];
    PmcaStatusType *status = (PmcaStatusType *)tempBuff;
    for (j = RETRY_COUNT; j--;)
    {
        DWORD checkSum = 0;
        BYTE *buffPtr = buffer;
        if (!isUSBMCA) {
            stat = SendCommandData(group);
        } else {
            blockSizeEx = wordCount;
            if (blockSizeEx > MAX_RECEIVE_BUFFER) 
                blockSizeEx = MAX_RECEIVE_BUFFER;
            if (blockSizeEx <= 0)
                blockSizeEx = 1;
            blockSizeEx--;
            stat = SendCommandDataEx(group,blockSizeEx);
        }
        if (stat)
            continue;
		Sleep(5);								//added for battery power up problem
        stat = ReceiveStatusWithRetry(status);  // Skip status

        if (stat)
            continue;
        remCount = 2 * wordCount;
        while (remCount)
        {
            blockSize = remCount > MAX_RECEIVE_BUFFER ?
                    MAX_RECEIVE_BUFFER : remCount;
            stat = Receive(tempBuff, blockSize);
            if (!stat)
            {
                BYTE *tempPtr = tempBuff;
                remCount -= blockSize;
                for (i = blockSize / 2; i--;)
                {
                    BYTE oneByte = *tempPtr;
                    checkSum += oneByte;
                    *buffPtr = oneByte;
                    buffPtr++;
                    tempPtr++;
                    oneByte = *tempPtr;
                    checkSum += oneByte;
                    *buffPtr = oneByte;
                    buffPtr += 3;
                    tempPtr++;
                }
            }
            else
                break;
        }
        if (remCount)
            continue;
        // Verify check sum
        if (!isUSBMCA) {
            stat = SendCommandData(group);
        } else {
            stat = SendCommandDataEx(group,0);
        }
        if (stat)
        {
            continue;
        //} else if (isUSBMCA) {
        //    stat = ReceiveStatusWithPrompt(status);
        //    break;
        }


        // The previous command sets SendChkSum bit in p52_162 program.
        // This is the only opportunity to get status with checksum.
        // After status is sent PMCA clears the checksum 
        stat = ReceiveStatus(status);
        if (stat)
            continue;

         //   char str[200];
         //   ULONG ucheckSum;
         //   ucheckSum = status->u.checkSum;
         //   sprintf(str,"MCA APP cksum, oneByte : %ld : %ld : %ld : %ld", ucheckSum,checkSum,wordCount,group);
	        //::MessageBox(NULL,str,"CheckSum Test",MB_OK);

        Sleep(20);
        if (status->u.checkSum == checkSum)
        {
            if (isUSBMCA) stat = ReceiveStatusWithPrompt(status);
            break;  // Success
        } else {
            stat = PMCA_ERROR_INVALID_DATA_CHECK_SUM;
        }
        //if (isUSBMCA) stat = ReceiveStatusWithPrompt(status);



    }
    return stat;
} // End of ReceiveDataWords

static int ClearDataAndTime(HANDLE wait, BYTE data, BYTE time)
{
    int gain;
    PmcaStatusType status;
    int stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        int clearTime;
        gain = MAX_GAIN >> status.control.flags.bits.gain;
        clearTime = s_device == PMCA_8000 ?
                gain / 2 + 1536 : gain / 8 + 256;
        stat = SendByteCommand(CMD_DELETE, data, time, 1);
        if (!stat)
            Sleep(clearTime);
    }
    return stat;
} // End of ClearDataAndTime

static int GetBatteryCapacity(BYTE value, int kind)
{
#define BATTERY_SCALE 9
    static const BYTE batteryValue [2][BATTERY_SCALE + 1] = {
        {0, 50, 63, 68, 71, 74, 77, 80, 88, 255},
        {0, 68, 73, 74, 75, 76, 77, 78, 80, 255}};

    static const int batteryCapacity [BATTERY_SCALE + 1] =
        {0, 50, 125, 250, 375, 500, 625, 750, 875, 1000};
    int capacity;
    int i, j;
    for (i = 1; ;)  // Value can't be 0
    {
        if (value <= batteryValue[kind][i])
            break;
        ++i;
    }
    j = i--;    // Low range
    capacity =
            (batteryCapacity[j] +
            (batteryCapacity[i] - batteryCapacity[j]) *
            (value - batteryValue[kind][j]) /
            (batteryValue[kind][i] - batteryValue[kind][j])) / 10;
    return capacity;
} // End of GetBatteryCapacity

static void MonitorThreadProc(LPVOID param)
{
#define MONITOR_TICK 200    // milliseconds
#define STAY_ALIVE_COUNT 5
    LPVOID par;
    DWORD byteReceived;
    DWORD byteSent;
    BOOL priorityBoost = FALSE;
    int stayAlive = STAY_ALIVE_COUNT;
    par = param;
    s_byteReceived = 0;
    byteReceived = 0;
    s_byteSent = 0;
    byteSent = 0;
    while (!s_terminate)
    {
        Sleep(MONITOR_TICK);

        if (!s_connected)
            continue;
        if ((s_receiving &&
                byteReceived == s_byteReceived) ||
                (s_sending && byteSent == s_byteSent))
        {
            if (!priorityBoost)
            {
                int stat = SetExecPriority(MAX_EXEC_PRIORITY - 1);
                priorityBoost = TRUE;
            }
            else
            {
                int stat = SetExecPriority(s_priority);
                priorityBoost = FALSE;
            }
        }
        else if (priorityBoost)
        {
            int stat = SetExecPriority(s_priority);
            priorityBoost = FALSE;
        }
        if ((!(s_sending || s_receiving)) && !isUSBMCA)
        {
            if (!(--stayAlive))
            {
                RequestType *request =
                        AllocateRequest(OP_STAY_ALIVE, NULL);
                int stat;
                request->wait = NULL;
                stat = PutRequest(request);
                stayAlive = STAY_ALIVE_COUNT;
            }
        }
        byteReceived = s_byteReceived;
        byteSent = s_byteSent;

    }
    SetEvent(s_waitMonitorTermination);
    return;
} // End of MonitorThreadProc

// Function : GetCts
// output   : CTS Status
// return   : bool CTS
static bool GetCts()
{
    int stat = 0;
    DWORD ModemStatus;
    stat = GetModemStatus(&ModemStatus);
    if (ModemStatus & MS_CTS_ON) {
        return true;
    } else {
        return false;
    }
}

// Function : GetDsr
// output   : DSR Status
// return   : bool DSR
static bool GetDsr()
{
    int stat = 0;
    DWORD ModemStatus;
    stat = GetModemStatus(&ModemStatus);
    if (ModemStatus & MS_DSR_ON) {
        return true;
    } else {
        return false;
    }
}

static void ExecThreadProc(LPVOID param)
{
    LPVOID par;
    RequestType *request;
    PmcaStatusType status;
    CommandType command;
    int stat;

    par = param;
    for (;;)
    {
        request = (RequestType *) s_queue.Get();
        s_currentRequest = request;
        try
        {
// Request processing

switch (request->code)
{   // Begin operation switch

case OP_DUMMY:
    stat = PMCA_ERROR_SUCCESS;
    break;

case OP_POWER_UP:
{
    #define POWERUP_TICK 500.0  // 0.5 milliseconds x2 (1kHz)
    int i;
    bool initCTS;
    bool newCTS;
    bool isUSBMCACTS;
    bool initDSR;
    bool newDSR;
    bool isUSBMCADSR;
    if (s_connected)
        ExecDisconnect();
    stat = OpenComPort(request->p.powerUpPort);
    if (!stat)
    {
        int statP = SetExecPriority(MAX_EXEC_PRIORITY);
		initCTS = GetCts();
		initDSR = GetDsr();
        SetRts();
        for (i = 200; --i;)		// 200 milliseconds
        {
            SetDtr();
            s_mTime.Wait(POWERUP_TICK);
            ResetDtr();
            s_mTime.Wait(POWERUP_TICK);
        }
        Sleep(3800);
        newCTS = GetCts();
        newDSR = GetDsr();
        if (initCTS != newCTS) {
            isUSBMCACTS = true;
        } else {
            isUSBMCACTS = false;
        }
        if (initDSR != newDSR) {
            isUSBMCADSR = false;
        } else {
            isUSBMCADSR = true;
        }
        if (isUSBMCACTS == isUSBMCADSR) {
            isUSBMCA = isUSBMCACTS;
        } else {
            isUSBMCA = false;   // error can't identify
        }
        ResetRts();
        statP = SetExecPriority(s_priority);
        CloseComPort();
    }
    break;
}

case OP_CONNECT:
    stat = ExecConnect(request);
    break;

case OP_DISCONNECT:
    ExecDisconnect();
    stat = PMCA_ERROR_SUCCESS;
    break;

case OP_GET_SERIAL_NUMBER:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = (int) status.u.gsn.serialNumber;
    break;

case OP_GET_GROUP:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = (int) status.u.gsn.group;
    break;

case OP_GET_THRESHOLD:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = (int) status.control.threshold;
    break;

case OP_GET_GAIN:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = MAX_GAIN >> status.control.flags.bits.gain;
    break;

case OP_GET_BATTERY_TYPE:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = (int) status.control.flags.bits.batteryType;
    break;

case OP_GET_BATTERY_CONDITION:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = (int) status.control.flags.bits.batteryCondition;
    break;

case OP_GET_BATTERY_CAPACITY:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        BYTE value = (BYTE) status.battery;
        if (!value)
            stat = 101;
        else
            stat = GetBatteryCapacity(value,
                    status.control.flags.bits.batteryType);
    }
    break;

#define EvaluateTime(sec, f75) \
    (((sec) + 1) * 1000 - (int) ((float)(f75) / 0.075f))

case OP_GET_REAL_TIME:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = EvaluateTime(status.realTime, status.realTime75);
    break;

case OP_GET_LIVE_TIME:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        stat = EvaluateTime(status.liveTime, status.liveTime75);
    break;

case OP_GET_FLAGS:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
        *request->p.flagsPtr = status.control.flags;
    break;

case OP_GET_STATUS:
    stat = ReceiveStatusWithPrompt(request->p.statusPtr);
    break;

case OP_GET_DATA:
{
    int gain;
    BYTE *buffer = request->p.data.buffer;
    int count = request->p.data.dwordCount;
    int channel = request->p.data.channel;
    stat = ReceiveStatusWithPrompt(&status);
    gain = MAX_GAIN >> status.control.flags.bits.gain;
    if (channel >= gain)
        stat = PMCA_ERROR_INVALID_CHANNEL;
    else
    { 
        stat = ReceiveDataWords(buffer, channel * 4, count);
        if (!stat)
        {
            stat = ReceiveDataWords(buffer + 2,
                    channel * 4 + 2, count);
            if (isUSBMCA) Sleep(100);
        }
    }
    if (isUSBMCA) stat = ReceiveStatusWithPrompt(&status);
    break;
}

case OP_GET_DATE_TIME:
{
#define MakeDigits(bcd, bf) \
    *((bf)++) = ((bcd) >> 4) + '0'; \
    *((bf)++) = ((bcd) & 0xF) + '0'

    DateTimeType stamp;
    stat = ReceiveDateTime(&stamp);
    if (!stat)
    {
        char *buff = request->p.strDateTime;
        char *bf = buff;
        MakeDigits(stamp.date.month, buff);
        *(buff++) = '/';
        MakeDigits(stamp.date.day, buff);
        *(buff++) = '/';
        MakeDigits(stamp.date.century, buff);
        MakeDigits(stamp.date.year, buff);
        *(buff++) = ' ';
        MakeDigits(stamp.time.hours, buff);
        *(buff++) = ':';
        MakeDigits(stamp.time.minutes, buff);
        *(buff++) = ':';
        MakeDigits(stamp.time.seconds, buff);
        *(buff++) = '\0';
    }
    break;
}

case OP_SET_ABSOLUTE_PEAK_MODE:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.absolutePeak = 1;
        stat = SendControl(&status);
    }
    break;

case OP_SET_FIRST_PEAK_MODE:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.absolutePeak = 0;
        stat = SendControl(&status);
    }
    break;

case OP_SELECT_REAL_TIMER:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.flags.bits.live = 0;
        stat = SendControl(&status);
    }
    break;

case OP_SELECT_LIVE_TIMER:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.flags.bits.live = 1;
        stat = SendControl(&status);
    }
    break;

case OP_CLEAR_DATA:
    stat = ClearDataAndTime(request->wait, 1, 0);
    break;

case OP_CLEAR_TIME:
    stat = ClearDataAndTime(FALSE, 0, 1);
    break;

case OP_CLEAR_DATA_AND_TIME:
    stat = ClearDataAndTime(request->wait, 1, 1);
    break;

case OP_START_ACQUISITION:
#define MakeBcd(num) (BYTE) (((num) % 10) + (((num) / 10) << 4))
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        if (request->p.setAcquisitionStartTime &&
                s_device == PMCA_8000A)
        {
            BYTE b1, b2, b3, code;
            time_t timet;
            struct tm *ltm;
            time(&timet);
            ltm = localtime(&timet);
            b1 = MakeBcd(ltm->tm_hour);
            b2 = MakeBcd(ltm->tm_min);
            b3 = MakeBcd(ltm->tm_sec);
            stat = SendByteCommand(CMD_SET_TIME, b1, b2, b3);
            Sleep(1);
            code = MakeBcd(ltm->tm_year / 100 + 19);
            b1 = MakeBcd(ltm->tm_year % 100);
            b2 = MakeBcd(ltm->tm_mon + 1);
            b3 = MakeBcd(ltm->tm_mday);
            stat = SendByteCommand(code, b1, b2, b3);
            Sleep(1);
        }
        status.control.flags.bits.acquire = 1;
        stat = SendControl(&status);
    }
    break;

case OP_STOP_ACQUISITION:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.flags.bits.acquire = 0;
        stat = SendControl(&status);
    }
    break;

case OP_SET_GROUP:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        int group = request->p.group;
        int gainCode = status.control.flags.bits.gain;
        int gain = MAX_GAIN >> gainCode;
        if (TOTAL_MEMORY_SIZE <= gain * group)
            stat = PMCA_ERROR_INVALID_GROUP;
        else
            stat = SendByteCommand(CMD_SET_GROUP, 0, group, 1);
    }
    break;

case OP_SET_GAIN:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        int gainCode = MAX_GAIN_CODE;
        int gain = (request->p.gain - 1) / MIN_GAIN;
        while(gain)
        {
            gain >>= 1;
            gainCode--;
        }
        status.control.flags.bits.gain = gainCode;
        stat = SendControl(&status);
    }
    break;

case OP_SET_TIME:
    stat = SendByteCommand(
            CMD_SET_TIME,
            request->p.timeStamp.hours,
            request->p.timeStamp.minutes,
            request->p.timeStamp.seconds);
    break;

case OP_SET_DATE:
    stat = SendByteCommand(
            request->p.dateStamp.century,   // CMD_SET_DATE_19/20 
            request->p.dateStamp.year,
            request->p.dateStamp.month,
            request->p.dateStamp.day);
    break;

case OP_SET_LOCK:
    command.code = CMD_SET_LOCK;
    command.u.lock.code = request->p.lock;
    command.u.lock.spare = 1;
    stat = SendCommand(&command);
    break;

case OP_SET_BATTERY_TYPE:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.flags.bits.batteryType =
                request->p.batteryType;
        stat = SendControl(&status);
    }
    break;

case OP_SET_ACQUISITION_TIME:
{
    struct timeStructure
    {
        DWORD time:24;
        DWORD spare:8;
    } *tPtr = (struct timeStructure *)&command.u.acqTime;
    command.code = CMD_SET_ACQ_TIME;
    tPtr->time = request->p.time;
    stat = SendCommand(&command);
    break;
}

case OP_SET_THRESHOLD:
    stat = ReceiveStatusWithPrompt(&status);
    if (!stat)
    {
        status.control.threshold = request->p.threshold;
        stat = SendControl(&status);
    }
    break;

case OP_STAY_ALIVE:
    stat = SendCommandData(0);
    break;

case OP_TERMINATE:
    s_currentRequest = NULL;
    FreeRequest(request);
    goto L_Terminate;

default:
    stat = PMCA_ERROR_SYSTEM_ERROR;
} // End operation switch

        }   // Exception block
        catch(...)
        {
            stat = PMCA_ERROR_REQUEST_EXCEPTION;
        }
        if (request->result)
            ReturnResult(request, stat);
        s_currentRequest = NULL;
        FreeRequest(request);
// Go to get a new request    
    } // End while(!s_terminate)
L_Terminate:
    ExecDisconnect();    // Clears the queue
    stat = WaitForSingleObject(
        s_waitMonitorTermination, 10000);
    CloseHandle(s_waitMonitorTermination);
    SetEvent(s_waitExecTermination);
    return;
} // End ExecThreadProc
