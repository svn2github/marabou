{**********************************************************************
 *
 *   INTRFCE.INC
 *
 *   External declarations that give user code access to main code
 *   variables.  This file is to be provided by XIA.
 *        
 ***********************************************************************}  

.EXTERNAL  UserIn;        { 16 input data words } 
.EXTERNAL  UserOut;       { 16 output data words } 

.EXTERNAL  ATstart;       {  Address of current trace } 
.EXTERNAL  TLen;          {  Length of current trace } 
.EXTERNAL  Energy;        {  energy of current event } 
.EXTERNAL  ChanNum;       {  Number of current channel } 
.EXTERNAL  URetVal;       {  Return value from user routine [6] } 
.EXTERNAL  PSAresult;	  {  Result of XIA PSA } 
.EXTERNAL  URetNum; 		  {  Number of User retun words available }
.EXTERNAL  GSLTtimeA;     {  high word, middle, low word  } 
.EXTERNAL  GSLTtimeB;     {  40MHz timer  } 
.EXTERNAL  GSLTtimeC;     {  GSLT arrival time  } 
.EXTERNAL  RUNTASK;		  {  RUNTASK of the current run }
.EXTERNAL  E0L;			  { lagging Energy filter, low word }
.EXTERNAL  E0H;			  { lagging Energy filter, high word }
.EXTERNAL  E1L;			  { leading Energy filter, low word }
.EXTERNAL  E1H;			  { leading Energy filter, high word }

