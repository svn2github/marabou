MODUL = -DMODULE -include /usr/src/linux-2.4/include/linux/modversions.h
INCLUDES= -I./ -I/usr/src/linux-2.4/include -I../include
CFLAGS = -Wall -Wstrict-prototypes $(INCLUDES) -D__KERNEL__ $(MODUL) 
LDFLAGS = -r


OBJS = dda0816_globals.o dda0816_irq_Service.o dda0816_write.o dda0816_SetPreScaler.o dda0816_SetPacerClock.o dda0816_SetOutputClock.o dda0816_SetCurve.o dda0816_SetUpdateGroup.o dda0816_ClearUpdateGroup.o dda0816_SetClockSource.o dda0816_SetOutputClockGeneration.o dda0816_ClearOutputClockGeneration.o dda0816_StartPacer.o dda0816_StopPacer.o dda0816_ioctl.o dda0816_open.o dda0816_release.o dda0816_sysctl.o dda0816_core.o dda0816_debug.o dda0816_error.o dda0816_procinfo.o

all: dda0816.o

dda0816.o: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS):	../include/dda0816_P.h
