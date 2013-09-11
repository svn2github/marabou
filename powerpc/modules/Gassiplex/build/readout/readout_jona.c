#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <hrace.h>
#include <hdtu.h>

#include "caen_v977.c"

#include "caen_v775.c"

#define BUFFER_SIZE (8 * 6 * 65)
#define NOCHAN (1*6*64)

#define v775_mask_DATAHEADER    0x2000000
#define v775_mask_DATATRAILER   0x4000000
#define v775_mask_DATAUNVAL     0x6000000
#define v775_mask_NODATAWORDS   0x3f00
#define v775_mask_WORDTYPE      0x7000000


static HDTU* hdtu = NULL;
static HRace* hrace = NULL;
static CAEN_V977* v977 = NULL;
static CAEN_V775* tdc1 = NULL;
static CAEN_V775* tdc2 = NULL;

int main(int argc, char *argv[]){ 
  FILE *outfile;
  char datafdisk[100];
  char copycom[150];
  uint32_t writeperiod = 30000;
  uint32_t totallen = 0;
  uint32_t maxlen = 4000000;
  char datafile[100];
  char filename2[20];
  int fileindex=0;
  uint32_t* buffer = NULL;
  uint32_t evtCtr = 0;
  int status;
  int runstodo;
  FILE *logfile;
  time_t now;
  int filethere;

  uint8_t channel;
  uint8_t module;
  uint8_t port;
  uint16_t adcVal;
  uint8_t race;
  uint8_t sector;
  uint8_t dprRest;
  uint32_t* p = NULL;
  int i;

  int is_trigbox=0;
  u_int32_t trigtag;
  u_int32_t tdc1header, tdc1trailer;
  u_int32_t tdc1data[18];
  int tdcchctr, notdc1words;

  int is_hodo=1;
  u_int32_t tdc2header, tdc2trailer;
  u_int32_t tdc2data[18];
  int notdc2words;

  int time_tdcwords;

  runstodo=atoi(argv[1]);
    
  writeperiod=atoi(argv[2]);

  if((writeperiod!=0)&&((runstodo%writeperiod)!=0)){
    printf("ERROR: usage: 'readout runstodo writeperiod' where writeperiod must be a multiple of runstodo!\n");
    abort();
  }
 
  /*init dtu and race*/
  if ((hdtu = newHDTU("dtu", 0x01000000)) == NULL) abort();
  fprintf(stderr, "HDtu p is 0x%p\n", (void*)hdtu->lvme->virtBase);
  
  if ((hrace = newHRace(0x00000000, 0)) == NULL) abort();
  fprintf(stderr, "HRace p is 0x%p\n", hrace->regRegion->virtBase);
  
  fprintf(stderr, "Init DTU: \n");
  HDTU_initDefaults(hdtu);
  
  if ((buffer = (uint32_t*)malloc(sizeof(uint32_t) * BUFFER_SIZE)) == NULL) 
    abort();

  /*init io-register*/
  if ((v977 = newV977("ioreg", 0x90000000)) == NULL) abort();
  fprintf(stderr, "IO-Reg p is 0x%p\n", (void*)v977->lvme->virtBase);
  V977_reset(v977);
  V977_setmode(v977,0x0);/*set it to io-register mode*/
  /*generate busy*/
  V977_setoutput(v977,0x2000);
  

  /*init triggertag-tdc*/
  if(is_trigbox){
    if((tdc1 = newV775("tdc1",0xA0000000)) == NULL) abort();
    fprintf(stderr, "TDC1 p is 0x%p\n", (void*)tdc1->lvme->virtBase);
    
    V775_reset(tdc1);

    v775_setemptyprog(tdc1,1);

    v775_setfullrange(tdc1,0x32);/*0x32: 1 TDC channel = 178 ps*/
    
    v775_enablecomstop(tdc1);
    
    v775_goonline(tdc1);
  }

  /*init hodoscope-tdc*/
  if(is_hodo){
    if((tdc2 = newV775("tdc2",0x80000000)) == NULL) abort();
    fprintf(stderr, "TDC2 p is 0x%p\n", (void*)tdc2->lvme->virtBase);
    
    V775_reset(tdc2);

    v775_setemptyprog(tdc2,1);

    v775_setfullrange(tdc2,0x32);
    
    v775_enablecomstop(tdc2);
    
    v775_goonline(tdc2);
  }

  /*build outputfile*/
  do{
    /* strcpy(datafile,"/data/etpdaq/gassi/control/data_gas_");*/            /*Dateinamen zusammensetzen*/
    strcpy(datafdisk,"/data/etpdaq/gassi/ana/data_Juni12_");            /*Dateinamen zusammensetzen*/
    sprintf(filename2,"%.4i",fileindex);
    strcat(datafdisk,filename2);
    strcat(datafdisk,".dat");
    outfile = fopen(datafdisk, "r");   /*Datei vorhanden?*/
    filethere=0;
    if(outfile != NULL){
      fclose(outfile);
      filethere=1;
    }
    fileindex++;                /*Naechste Dateinummer*/
  } while(filethere);
  
  strcpy(datafile,"/ramdisk/data_inter.dat");

  strcpy(copycom,"cat ");
  strcat(copycom,datafile);
  strcat(copycom," >> ");
  strcat(copycom,datafdisk);
  
  /* arm DTU */
  fprintf(stderr, "Start DAQ\n");
  HDTU_reset(hdtu);
  HDTU_arm(hdtu);
  HDTU_status(hdtu);
  
  printf("runstodo = %i\n",runstodo); 
  printf("writeperiod = %i\n",writeperiod);
  printf("writing to %s\n",datafdisk);
  
  /*open appropriate outputfile*/
  if(writeperiod > 0){
    outfile = fopen(datafile, "w");
    if (outfile == NULL){
      printf("ERROR:Opening outfile");
      abort();
    }
    printf("opening file %s\n",datafile);
  }else if(writeperiod==0){
    outfile = fopen(datafdisk, "w");
    if (outfile == NULL){
      printf("ERROR:Opening outfile");
      abort();
    }
    printf("opening file %s\n",datafdisk);
  }else{
    abort();
  }
 
  /*write entry in logfile*/
  now = time((time_t *)NULL);
  if((logfile = fopen("/data/etpdaq/gassi/control/log_Juni12.dat", "a"))==NULL){
    printf("ERROR: Opening logfile");
  }
  
  fprintf(logfile,"Beginning of run with %s\n",datafdisk);
  fprintf(logfile,"Acquiring %i events with write period %i\n",runstodo,writeperiod);
  /* fprintf(logfile,"%i\n",evtCtr);*/
  fprintf(logfile,"%s",ctime(&now));
  fprintf(logfile,"%i\n",now);
  fclose(logfile);

  /*send reset pulse via the io-register to veto-timer, triggerbox, tdc1 and tdc2, this also kills the busy2*/
  V977_setoutput(v977,0x1007);
  V977_setoutput(v977,0x00);
 

  /*data aquisition*/
  evtCtr = 0;
  while (1){
    /*waiting for the trigger ... */
    HDTU_waitData(hdtu);
   
    /*  V977_setoutput(v977,0x4000);
    V977_setoutput(v977,0x00);
    V977_setoutput(v977,0x4000);
    V977_setoutput(v977,0x00);
    
    */
    trigtag=0;
    notdc1words=0;
    if(is_trigbox){
      /*read tdc1 header*/
      tdc1header=v775_getdata(tdc1);
      /* fprintf(stderr,"%lu\n",(unsigned long)tdcheader);
	 fprintf(stderr,"%lu\n",(unsigned long)tdcheader&v775_mask_DATAHEADER);
	 fprintf(stderr,"%lu\n",(unsigned long)tdcheader&v775_mask_NODATAWORDS);
      */
      
      if((tdc1header&v775_mask_WORDTYPE)==v775_mask_DATAUNVAL){
	printf("no data in tdc1 buffer\n");   
      }else if((tdc1header&v775_mask_WORDTYPE)==v775_mask_DATAHEADER){/*valid trailer...*/
	/* printf("found header\n");*/
	/*determine number of data words to be read*/
	notdc1words=(tdc1header&v775_mask_NODATAWORDS)/0x100;
	/* printf("notdcwords %i\n",notdcwords);*/
	/*read data*/
	tdc1data[0]=tdc1header;
	for(tdcchctr=1;tdcchctr<=notdc1words;tdcchctr++){
	  tdc1data[tdcchctr]=v775_getdata(tdc1);
	  /* fprintf(stderr,"data %lu\n",(unsigned long)tdcdata[tdcchctr]);*/
	  /*   fprintf(stderr,"channel %lu\n",(unsigned long)(tdc1data[tdcchctr]&0x1e0000)>>17);
	     fprintf(stderr,"time %lu\n",(unsigned long)(tdc1data[tdcchctr]&0xfff));
	  */
	  trigtag+=(1<<((tdc1data[tdcchctr]&0x1e0000)>>17));
	}
	/*read trailer*/
	tdc1trailer=v775_getdata(tdc1);
	if((tdc1trailer&v775_mask_WORDTYPE)!=v775_mask_DATATRAILER){
	  printf("ERROR: tdc1 data is not followed by trailer\n");
	  exit(EXIT_FAILURE);
	}
	tdc1data[notdc1words+1]=tdc1trailer;
      }else{
	printf("ERROR: tdc1 word neither header nor unvalid datum. This should not happen\n");
	exit(EXIT_FAILURE);
      }
    }
  
    notdc2words=0;
    if(is_hodo){
      /*read tdc2 header*/
      tdc2header=v775_getdata(tdc2);
      /* fprintf(stderr,"%lu\n",(unsigned long)tdcheader);
	 fprintf(stderr,"%lu\n",(unsigned long)tdcheader&v775_mask_DATAHEADER);
	 fprintf(stderr,"%lu\n",(unsigned long)tdcheader&v775_mask_NODATAWORDS);
      */
      
      if((tdc2header&v775_mask_WORDTYPE)==v775_mask_DATAUNVAL){
	printf("no data in tdc2 buffer\n");   
      }else if((tdc2header&v775_mask_WORDTYPE)==v775_mask_DATAHEADER){/*valid trailer...*/
	/* printf("found header\n");*/
	/*determine number of data words to be read*/
	notdc2words=(tdc2header&v775_mask_NODATAWORDS)/0x100;
	/* printf("notdcwords %i\n",notdcwords);*/
	tdc2data[0]=tdc2header;
	/*read data*/
	for(tdcchctr=1;tdcchctr<=notdc2words;tdcchctr++){
	  tdc2data[tdcchctr]=v775_getdata(tdc2);
	  /* fprintf(stderr,"data %lu\n",(unsigned long)tdcdata[tdcchctr]);*/
	  /*  fprintf(stderr,"channel %lu\n",(unsigned long)(tdc2data[tdcchctr]&0x1e0000)>>17);
	  fprintf(stderr,"time %lu\n",(unsigned long)(tdc2data[tdcchctr]&0xfff));
	  */  
	}
	/*read trailer*/
	tdc2trailer=v775_getdata(tdc2);
	if((tdc2trailer&v775_mask_WORDTYPE)!=v775_mask_DATATRAILER){
	  printf("ERROR: tdc2 data is not followed by trailer\n");
	  exit(EXIT_FAILURE);
	}
	tdc2data[notdc2words+1]=tdc2trailer;
      }else{
	printf("ERROR: tdc2 word neither header nor unvalid datum. This should not happen\n");
	exit(EXIT_FAILURE);
      }
    }
  
    /* ... trigger arrived, data on race, arm again ...*/
    HDTU_arm(hdtu);

    /*kill busy1 (generated by CAEN TIMER)*/
    V977_setoutput(v977,0x1000);
    V977_setoutput(v977,0x00);

    /*read event from race to rio*/
    status = HRace_readSubEvt(hrace, buffer);

    /* printf("status %i",status);*/
    
    /*data transfer to rio done, enable read data from fes to race again*/
    HDTU_release(hdtu);

  
    evtCtr++;
    
    /* write all data to file */
    time_tdcwords=0;

    /*generate busy*/
    V977_setoutput(v977,0x2000);

    /*write timestamp to file*/
    now = time((time_t *)NULL);
    fwrite((u_int32_t*)&now,  sizeof(uint32_t),  1, outfile);
    time_tdcwords=1;
    
    /*write triggertag information to file*/
    if(is_trigbox){
      fwrite((u_int32_t*)&trigtag,  sizeof(uint32_t),  1, outfile);
      fwrite((u_int32_t*)&tdc1data[0],  sizeof(uint32_t), notdc1words+2, outfile);
      printf("event: %i, tag: %i, time: %i\n",evtCtr,trigtag,now);
      time_tdcwords+= ( 1 + notdc1words+2);
    }

    /*write hodoscope information to file*/
    if(is_hodo){
      fwrite((u_int32_t*)&tdc2data[0],  sizeof(uint32_t), notdc2words+2, outfile);
      time_tdcwords+= (notdc2words+2);

    }
    /*write buffer to file*/
    fwrite(buffer, sizeof(uint32_t), status, outfile);
    
    fflush(outfile);

    /*optionally print gassiplex-data*/
    /*   p = buffer;
    
    for(i=0;i<4;i++){
      printf("0x%08x\n", *p);
      p++;
    }
  
    for( i=0;i<status-4;i++){
      adcVal  = (*p     ) & 0x03ff;
      channel = (*p >> 10) & 0x003f;
      module  = (*p >> 16) & 0x0007;
      port    = (*p >> 19) & 0x0007;
      race    = (*p >> 22) & 0x0001;
      sector  = (*p >> 23) & 0x0007;
      dprRest = (*p >> 26) & 0x00ff;

      printf("0x%08x  "
	     "dpr: 0x%02x  "
	     "s: 0x%02x  "
	     "r: 0x%02x  "
	     "p: 0x%02x  "
	     "m: 0x%02x  "
	     "c: %i "
	     "adc: %i"
	     "\n",
	     *p, dprRest, sector, race, port, module, channel, adcVal);
      p++;
      }*/
   
    /*cancel busy*/
    V977_setoutput(v977,0x00);
   
    if ((evtCtr==1)||((evtCtr%1000) == 0)) {
      /*generate busy*/
      V977_setoutput(v977,0x2000);

      fprintf(stderr, "status: %d,  %d Events received\n", status, evtCtr);
      /*fclose(outfile);
      outfile = fopen(datafile, "a");
      if (outfile == NULL){
	printf("ERROR:Opening outfile");
	abort();
      }*/
     
      if(evtCtr>1){
	now = time((time_t *)NULL);
	if((logfile = fopen("/data/etpdaq/gassi/control/log_Juni12.dat", "a"))==NULL){
	  printf("ERROR: Opening logfile");
	}
	
	fprintf(logfile,"\n%s\n",datafile);
	fprintf(logfile,"%i\n",evtCtr);
	fprintf(logfile,"%s",ctime(&now));
	fprintf(logfile,"%i\n",now);
	fclose(logfile);
      }
      /*cancel busy*/
      V977_setoutput(v977,0x00);
    }

    /*do only when using the ramdisk as temporary storage*/
    if(writeperiod>0){
      /*generate busy*/
      V977_setoutput(v977,0x2000);
      totallen+=(status+time_tdcwords);/*count number of 32-bit words*/

      if(totallen>maxlen){
	printf("ERROR:to many data written to ramdiskfile, aborting\n");
	printf("evtCtr= %i, totallen= %i, maxlen= %i, writeperiod= %i\n",evtCtr,totallen, maxlen, writeperiod);
	abort();
      }
    
      /*read data from ramdisk and write to harddisk*/
      if ((evtCtr>0)&&((evtCtr%writeperiod) == 0)) {
    	fclose(outfile); /*close ramdiskfile in write mode*/
	printf("closing ramdiskfile\n");
	
	printf("copying ramdiskfile to harddisk, event %i\n",evtCtr);
	system(copycom);
	printf("done with copying to harddisk\n");

	totallen=0;
	outfile = fopen(datafile, "w"); /*open ramdiskfile in write mode*/
        if (outfile == NULL){
	  printf("ERROR:Opening outfile");
	  abort();
        }
	/*}*/
      }
      /*cancel busy*/
      V977_setoutput(v977,0x00);
    }
   
    if (evtCtr == runstodo) break;
  }  
  fclose(outfile);
  printf("Run %i, %i events written to file %s\n",atoi(filename2),evtCtr,datafdisk);
  
  now = time((time_t *)NULL);
  if((logfile = fopen("/data/etpdaq/gassi/control/log_Juni12.dat", "a"))==NULL){
     printf("ERROR: Opening logfile");
  }
	
  fprintf(logfile,"\nEnd of run with %s\n",datafdisk);
  fprintf(logfile,"%i\n",evtCtr);
  fprintf(logfile,"%s",ctime(&now));
  fprintf(logfile,"%i\n\n",(int)now);
  fclose(logfile);
  exit(EXIT_SUCCESS);
}
