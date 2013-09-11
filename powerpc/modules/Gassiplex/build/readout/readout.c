#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <hrace.h>
#include <hdtu.h>
#define BUFFER_SIZE (8 * 6 * 65)

/*static void swapInt(uint32_t *buffer, size_t size) {
  size_t i;
  uint32_t val = 0; 
  uint8_t* old = (uint8_t*)(&val);
  uint8_t* buf = NULL;
  
  for (i = 0; i < size; i++) {
    val = buffer[i];
    buf = (uint8_t*)(&buffer[i]);
    buf[0] = old[3];
    buf[1] = old[2];
    buf[2] = old[1];
    buf[3] = old[0];
  }
}*/
static HDTU* hdtu = NULL;
static HRace* hrace = NULL;

int main(int argc, char *argv[]){ 
  FILE *outfile;
  char datafdisk[100];
  char filename2[20];
  int fileindex=0;
  uint32_t* buffer = NULL;
  uint32_t evtCtr = 0;
  int status;
  int runstodo;
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

  if(argc!=2){
    fprintf(stderr, "ERROR: Usage is: readout #events_to_do\n");
    exit(EXIT_FAILURE);
  }
  runstodo=atoi(argv[1]);
    
   /*init dtu and race*/
  if ((hdtu = newHDTU("dtu", 0x01000000)) == NULL) abort();
  fprintf(stderr, "HDtu p is 0x%p\n", (void*)hdtu->lvme->virtBase);
  
  if ((hrace = newHRace(0x09000000, 0)) == NULL) abort();
  fprintf(stderr, "HRace p is 0x%p\n", (void*)hrace->regRegion->virtBase);
  
  fprintf(stderr, "Init DTU: \n");
  HDTU_initDefaults(hdtu);
  
  if ((buffer = (uint32_t*)malloc(sizeof(uint32_t) * BUFFER_SIZE)) == NULL){
    fprintf(stderr, "failed to allocate buffer\n");
    abort();
  }
  /*build outputfile*/
  do{
    strcpy(datafdisk,"../data/data_gassi_");            /*Dateinamen zusammensetzen*/
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
  

  /* arm DTU */
  fprintf(stderr, "Start DAQ\n");
  HDTU_reset(hdtu);
  HDTU_arm(hdtu);
  HDTU_status(hdtu);
  
  printf("runstodo = %i\n",runstodo); 
  printf("writing to %s\n",datafdisk);
  
  /*open appropriate outputfile*/
  outfile = fopen(datafdisk, "w");
  if (outfile == NULL){
    printf("ERROR:Opening outfile");
    abort();
  }
  printf("opening file %s\n",datafdisk);
  /*data aquisition*/
  evtCtr = 0;
  while (1){
    /*waiting for the trigger ... */
    HDTU_waitData(hdtu);
    /* HDTU_status(hdtu);*/

    /* ... trigger arrived, data on race, arm again ...*/
    HDTU_arm(hdtu);/*two banks ... */
    /*read event from race to rio*/
    status = HRace_readSubEvt(hrace, buffer);
    HDTU_release(hdtu);

    evtCtr++;

    /*display data*/
    /*  p = buffer;
    for(i=0;i<4;i++){
      printf("0x%08x\n",*p);
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
   
    /*write data to file*/
    fwrite(buffer,  sizeof(uint32_t),  status, outfile);
    if((evtCtr%1000)==0){
      fprintf(stderr, "status: %d,  event: %d \n", status, evtCtr);
    }    

    if (evtCtr == runstodo) break;
  }  
  fclose(outfile);
  printf("Run %i, %i events written to file %s\n",atoi(filename2),evtCtr,datafdisk);
  exit(EXIT_SUCCESS);
}
