 



















































































































































.MODULE/RAM/SEG=USERPROGRAM USER;

  .ENTRY USERBEGIN;
  .ENTRY USERRUNINIT;
  .ENTRY USERCHANNEL;
  .ENTRY USEREVENT;
  .ENTRY USERRUNFINISH;

  .EXTERNAL USERBEGINRETURN;                        
  .EXTERNAL USERRUNINITRETURN;                       
  .EXTERNAL USERCHANNELRETURN;                       
  .EXTERNAL USEREVENTRETURN;                         
  .EXTERNAL USERRUNFINISHRETURN;                     
 
#INCLUDE 1 <INTRFACE.INC>
 






  

.EXTERNAL  USERIN;          
.EXTERNAL  USEROUT;         

.EXTERNAL  ATSTART;         
.EXTERNAL  TLEN;            
.EXTERNAL  ENERGY;          
.EXTERNAL  CHANNUM;         
.EXTERNAL  URETVAL;         
.EXTERNAL  PSARESULT;	    
.EXTERNAL  URETNUM; 		   
.EXTERNAL  GSLTTIMEA;       
.EXTERNAL  GSLTTIMEB;       
.EXTERNAL  GSLTTIMEC;       
.EXTERNAL  RUNTASK;		   
.EXTERNAL  E0L;			   
.EXTERNAL  E0H;			   
.EXTERNAL  E1L;			   
.EXTERNAL  E1H;			   

#ENDINCLUDE 29 "INTRFACE.INC"
  
  .VAR/DM/SEG=USERDATA SAVEI0, SAVEI1, SAVEI3, SAVEI4; 
  .VAR/DM/SEG=USERDATA MYBUFFER[16];


 







USERBEGIN:
   
 



           
                                   
                                   

  JUMP USERBEGINRETURN;     

 





USERRUNINIT:
     
  NOP;                             
        
  JUMP USERRUNINITRETURN;    

 








USERCHANNEL:

   

  DM(SAVEI0)   = I0;  DM(SAVEI1)   = I1;      
  DM(SAVEI3)   = I3;  DM(SAVEI4)   = I4;      

   

  AR=-1;
  DM(URETVAL) = AR;
  DM(URETVAL+1)=AR;
  DM(URETVAL+2)=AR;
  DM(URETVAL+3)=AR;
  DM(URETVAL+4)=AR;
  DM(URETVAL+5)=AR;
  
   
   
   

  I0 = DM(SAVEI0); I1 = DM(SAVEI1);           
  I3 = DM(SAVEI3); I4 = DM(SAVEI4);           

   
   
  JUMP USERCHANNELRETURN;    

 





 
USEREVENT:
      
  NOP;                             
 
  JUMP USEREVENTRETURN;        

 






 
USERRUNFINISH:

  NOP;                             
 
  JUMP USERRUNFINISHRETURN;         

.ENDMOD;    
