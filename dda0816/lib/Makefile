#
#   
#   This Makefile has been generated with LDDK Version 0.7-beta2
#   (c) 1996 by Claus Schroeter (clausi@chemie.fu-berlin.de)
#
#
#
#
#   Creation Date: Tue Nov  7 11:23:16 2000



AR = ar
INCLUDES = -I../dda08-16_new
CFLAGS = -O6 $(INCLUDES) $(DEFS)
OBJS = dda0816_SetPreScaler.o dda0816_SetPacerClock.o dda0816_SetOutputClock.o dda0816_SetCurve.o dda0816_SetUpdateGroup.o dda0816_ClearUpdateGroup.o dda0816_SetClockSource.o dda0816_SetOutputClockGeneration.o dda0816_ClearOutputClockGeneration.o dda0816_StartPacer.o dda0816_StopPacer.o dda0816_SetCycle.o dda0816_GetPacer.o
SRCS = dda0816_SetPreScaler.c dda0816_SetPacerClock.c dda0816_SetOutputClock.c dda0816_SetCurve.c dda0816_SetUpdateGroup.c dda0816_ClearUpdateGroup.c dda0816_SetClockSource.c dda0816_SetOutputClockGeneration.c dda0816_ClearOutputClockGeneration.c dda0816_StartPacer.c dda0816_StopPacer.c dda0816_SetCycle.c dda0816_GetPacer.c
INCLUDES = -I. -I../include

DEFS = 


BOLD		= "\033[1m"
NBOLD		= "\033[0m"

ECHO		= /bin/echo

COMPILE	= $(ECHO) "--- Compiling "$(BOLD)$<$(NBOLD)"..." ; gcc
DEPEND	= $(ECHO) "--- Checking dependencies..." ; gcc
RLINK	= $(ECHO) "--- Linking (relocatable) "$(BOLD)$@$(NBOLD)"..." ; ld -r
LINK	= $(ECHO) "--- Linking "$(BOLD)$@$(NBOLD)"..." ; gcc
YACC	= $(ECHO) --- Running bison on $(BOLD)$<$(NBOLD)...; bison -d -y
LEX	= $(ECHO) --- Running flex on $(BOLD)$<$(NBOLD)...; flex 
ARCHIVE = $(ECHO) --- Archiving $(BOLD)$<$(NBOLD)...; $(AR)

CC	= $(COMPILE) $(CFLAGS)


libdda0816.a : $(OBJS)
	-rm -f libdda0816.a
	$(ARCHIVE)  cr libdda0816.a $(OBJS)

#depend .depend: $(SRCS)
#	$(DEPEND) $(CFLAGS) -M $(SRCS)  >./.depend

#include .depend

clean: 
	rm *.o .depend
