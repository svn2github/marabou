#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include <allParam.h>

#include "hdtu.h"

static HDTU* hdtu = NULL;

static void atexit0()
{
  if (hdtu != NULL) delHDTU(hdtu);
  free(hdtu);

  closelog();
}

void usage(const char* progName)
{
  fprintf(stderr, "Usage: %s command [boardname]\n", progName); 
  fprintf(stderr, "boardname is optional, the default is \"dtu\".\n");
  fprintf(stderr, "command may be one of the following:\n");
  fprintf(stderr, " init\n"); 
  fprintf(stderr, " reset\n"); 
  fprintf(stderr, " status\n"); 
  fprintf(stderr, " arm\n"); 
  fprintf(stderr, " trigger\n"); 
  fprintf(stderr, " release\n");
}

int main(int argc, char **argv)
{

  char progName[128];
  int rows;
  Param param;
  uint32_t vmeBase = 0;
  char dtuName[64] = "dtu";

  strncpy(progName, basename(argv[0]), 128);
  if (argc > 3) {
    usage(progName);
    exit(EXIT_FAILURE);
  }
  atexit(atexit0);

  /* Get boardname */
  if (argc == 3) {
    strncpy(dtuName, argv[2], 64);
  }

  /* Open LogFile */
  openlog(progName, LOG_PERROR | LOG_PID, LOG_LOCAL0);
  setlogmask(LOG_UPTO(LOG_DEBUG));
  
  /* Open ParamFile */ 
  if (conSetupParam(&param, getenv("DAQ_SETUP")) == -1) {
    syslog(LOG_ERR, "Opening parameter source: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
 
  if ((Param_getInt(&param, dtuName, "cardbase", &rows, 
                    (unsigned long int*)(&vmeBase)) != 0) || 
      (rows != 1)) {
    syslog(LOG_ERR, "%s: Parameter cardbase not found", dtuName);
    exit(EXIT_FAILURE);
  }
  
  if ((hdtu = newHDTU(dtuName, vmeBase)) == NULL){
    syslog(LOG_ERR, "%s: could not create newHDTU", dtuName);
    abort() ;
  }
  
  if (strcmp("init", argv[1]) == 0) {
    char fileName[PARAM_MAX_NAME_LEN];
    
    if ((Param_getString(&param, dtuName, "filename", &rows, fileName) != 0)
        || (rows != 1)) {
      syslog(LOG_ERR, "%s: Parameter filename not found", dtuName);
      exit(EXIT_FAILURE);
    }
    
    if (HDTU_init(hdtu, fileName) == -1) exit(EXIT_FAILURE);
    exit(EXIT_SUCCESS);
  } else if (strcmp("reset", argv[1]) == 0){
    HDTU_reset(hdtu);
  } else if (strcmp("status", argv[1]) == 0) {
    HDTU_status(hdtu);
  } else if (strcmp("arm", argv[1]) == 0) {
    HDTU_arm(hdtu);
  } else if (strcmp("trigger", argv[1]) == 0) {
    HDTU_trigger(hdtu);
  } else if (strcmp("release", argv[1]) == 0) {
    HDTU_release(hdtu);
  } else {
    usage(progName);
    exit(EXIT_FAILURE);
  }
  
  exit(EXIT_SUCCESS);
}
