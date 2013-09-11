static const char rcsId[] =
  "$Header: /misc/hadaq/cvsroot/runctrl/daq_cas/vme/support/blubctrl.c,v 1.13 2005/03/02 13:44:33 hades Exp $";
#define _XOPEN_SOURCE
#define _XOPEN_VERSION 4

#if HAVE_CONFIG_H
#include <config.h>
#endif                                                  /* HAVE_CONFIG_H */

#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SYSLOG_NAMES
#include <syslog.h>

#include <allParam.h>
#include <lvme.h>

#include "blubctrl.h"

jmp_buf terminateJmp;

static char *ourBoardName;

static void sighandler(int sig)
{
  switch (sig) {
  case SIGBUS:
    syslog(LOG_ERR, "%s: Bus Error", ourBoardName);
    break;
  default:
    break;
  }
  longjmp(terminateJmp, sig);
}

void usage(char *commandName)
{
  fprintf(stderr, "Usage: %s command boardName [register_name]\n", commandName);
  fprintf(stderr, "where command may be one of the following:\n");
  fprintf(stderr, "init\n");
  fprintf(stderr, "loadthr\n");
  fprintf(stderr, "reset\n");
  fprintf(stderr, "start\n");
  fprintf(stderr, "stop\n");
  fprintf(stderr, "initstatus\n");
  fprintf(stderr, "runstatus\n");
  fprintf(stderr, "lvl1busy\n");
  fprintf(stderr, "lvl2busy\n");
  fprintf(stderr, "error\n");
  fprintf(stderr, "statusregister\n");
  fprintf(stderr, "status\n");
}

int blubctrl(int argc, char *argv[], const char *buildDate, int (*funtab[])())
{
  int retVal;
  Param paramS, *param = &paramS;
  char *command;
  char verbosity[PARAM_MAX_VALUE_LEN];
  int rows;

  openlog(basename(argv[0]), LOG_PERROR | LOG_PID, LOG_LOCAL0);

#if 0
  Param_registerLogMsg(syslog);
#endif
  if (-1 == conSetupParam(param, getenv("DAQ_SETUP"))) {
    syslog(LOG_ERR, "Opening parameter source: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  syslog(LOG_INFO, "Build date: %s", buildDate);

  if (argc < 3) {
    syslog(LOG_ERR, "Usage error");
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  command = argv[1];

#if 0
  LVme_registerLogMsg(syslog);
#endif
  if (0 == setjmp(terminateJmp)) {
    int board;
    int nBoards;
    char *name;

    signal(SIGHUP, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGBUS, sighandler);

    if (strcmp(command, "statusregister") == 0) {
      nBoards = argc - 1;     /* statusregister has one extra argument */
    } else {
      nBoards = argc;
    }

    for (board = 2; board < nBoards; board++) {
      int i;

      name = argv[board];
      ourBoardName = name;

      if (Param_getString(param, name, "verb", &rows, verbosity)
          || (rows != 1)) {
        syslog(LOG_NOTICE, "%s(verb) not set. Defaulting to info.", name);
        strcpy(verbosity, "info");
      }
      /* Figure out the verbosity level from the string */
      for (i = 0; prioritynames[i].c_name != NULL && 0 != strcmp(prioritynames[i].c_name, verbosity); i++) {
      }
      if (prioritynames[i].c_name == NULL) {
        syslog(LOG_ERR, "Unknown verbosity %s. Defaulting to info.", verbosity);
        i = 6;
      }
      
      setlogmask(LOG_UPTO(prioritynames[i].c_val));

      if (strcmp(command, "init") == 0) {
        if (0 == funtab[init] (name, param)) {
          syslog(LOG_INFO, "%s: init done", name);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "%s: init fail", name);
          retVal = -1;
        }
      } else if (strcmp(command, "loadthr") == 0) {
        if (0 == funtab[loadThresholds] (name, param)) {
          syslog(LOG_INFO, "%s: loadthr done", name);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "%s: loadthr fail", name);
          retVal = -1;
        }
      } else if (strcmp(command, "reset") == 0) {
        if (0 == funtab[reset] (name, param)) {
          syslog(LOG_INFO, "%s: reset done", name);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "%s: reset fail", name);
          retVal = -1;
        }
      } else if (strcmp(command, "start") == 0) {
        if (0 == funtab[start] (name, param)) {
          syslog(LOG_INFO, "%s: start done", name);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "%s: start fail", name);
          retVal = -1;
        }
      } else if (strcmp(command, "stop") == 0) {
        if (0 == funtab[stop] (name, param)) {
          syslog(LOG_INFO, "%s: stop done", name);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "%s: stop fail", name);
          retVal = -1;
        }
      } else if (strcmp(command, "initstatus") == 0) {
        int init;

        if ((init = funtab[initialized] (name, param)) == 1) {
          printf("%s is initialized.\n", name);
          retVal = 0;
        } else if (init == 0) {
          printf("%s is not initialized.\n", name);
          retVal = 1;
        } else {
          printf("Board does not support %s.\n", command);
          retVal = 2;
        }
      } else if (strcmp(command, "runstatus") == 0) {
        int run;

        if ((run = funtab[running] (name, param)) == 1) {
          printf("%s is running.\n", name);
          retVal = 0;
        } else if (run == 0) {
          printf("%s is not running.\n", name);
          retVal = 1;
        } else {
          printf("Board does not support %s.\n", command);
          retVal = 2;
        }
      } else if (strcmp(command, "lvl1busy") == 0) {
        int lvl1busy;

        if ((lvl1busy = funtab[lvl1Busy] (name, param)) == 1) {
          printf("%s is lvl1 busy.\n", name);
          retVal = 0;
        } else if (lvl1busy == 0) {
          printf("%s is not lvl1 busy.\n", name);
          retVal = 1;
        } else {
          printf("Board does not support %s.\n", command);
          retVal = 2;
        }
      } else if (strcmp(command, "lvl2busy") == 0) {
        int lvl2busy;

        if ((lvl2busy = funtab[lvl2Busy] (name, param)) == 1) {
          printf("%s is lvl2 busy.\n", name);
          retVal = 0;
        } else if (lvl2busy == 0) {
          printf("%s is not lvl2 busy.\n", name);
          retVal = 1;
        } else {
          printf("Board does not support %s.\n", command);
          retVal = 2;
        }
      } else if (strcmp(command, "error") == 0) {
        int errorVal;

        if ((errorVal = funtab[error] (name, param)) == 1) {
          printf("%s has error.\n", name);
          retVal = 0;
        } else if (errorVal == 0) {
          printf("%s has no error.\n", name);
          retVal = 1;
        } else {
          printf("Board does not support %s.\n", command);
          retVal = 2;
        }
      } else if (strcmp(command, "statusregister") == 0) {
        char *s;
        int r;

        r = argc - 1;   /* the last argument */
        s = (char*)funtab[statusRegister] (name, param, argv[r]);

        if (s != NULL) {
          printf("%s: Content of status register %s: 0x%p\n",
                 name, argv[r], s);
          retVal = 0;
        } else {
          syslog(LOG_ERR, "Usage error");
          usage(argv[0]);
          retVal = -1;
        }
      } else if (strcmp(command, "status") == 0) {
        funtab[status] (name, param);
      } else {
        syslog(LOG_ERR, "Unknown command: %s", command);
        retVal = -1;
        usage(argv[0]);
      }
    }
  }
  desParam(param);
  closelog();
  return retVal;
}
