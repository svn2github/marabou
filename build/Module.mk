# Module.mk for build module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := build

RMKDEPDIR    := $(MODDIR)/rmkdepend
BINDEXPDIR   := $(MODDIR)/win/bindexplib

##### rmkdepend #####
RMKDEPH      := $(wildcard $(RMKDEPDIR)/*.h)
RMKDEPS      := $(wildcard $(RMKDEPDIR)/*.c)
RMKDEPO      := $(RMKDEPS:.c=.o)
RMKDEP       := bin/rmkdepend$(EXEEXT)
ifeq ($(ARCH),win32)
#RMKDEPCFLAGS := -DINCLUDEDIR=\"/usr/include\" -DOBJSUFFIX=\".obj\"
RMKDEPCFLAGS := -DINCLUDEDIR=\"/usr/include\" -DOBJSUFFIX=\".o\"
else
RMKDEPCFLAGS := -DINCLUDEDIR=\"/usr/include\" -DOBJSUFFIX=\".o\"
endif

##### bindexplib #####
ifeq ($(ARCH),win32)
BINDEXPS     := $(wildcard $(BINDEXPDIR)/*.cxx)
BINDEXPO     := $(BINDEXPS:.cxx=.o)
BINDEXP      := bin/bindexplib$(EXEEXT)
endif

##### local rules #####
$(RMKDEP):      $(RMKDEPO)
		$(LD) $(LDFLAGS) -o $@ $(RMKDEPO)

ifeq ($(ARCH),win32)
$(BINDEXP):     $(BINDEXPO)
		$(LD) $(LDFLAGS) -o $@ $(BINDEXPO)

all-build:      $(RMKDEP) $(BINDEXP)
else
all-build:      $(RMKDEP)
endif

clean-build:
		@rm -f $(RMKDEPO) $(BINDEXPO)

clean::         clean-build

distclean-build: clean-build
		@rm -f $(RMKDEP) $(BINDEXP)

distclean::     distclean-build


##### dependencies #####
$(RMKDEPDIR)/cppsetup.o: $(RMKDEPDIR)/def.h $(RMKDEPDIR)/ifparser.h
$(RMKDEPDIR)/ifparser.o: $(RMKDEPDIR)/ifparser.h
$(RMKDEPDIR)/include.o:  $(RMKDEPDIR)/def.h
$(RMKDEPDIR)/main.o:     $(RMKDEPDIR)/def.h $(RMKDEPDIR)/imakemdep.h
$(RMKDEPDIR)/parse.o:    $(RMKDEPDIR)/def.h
$(RMKDEPDIR)/pr.o:       $(RMKDEPDIR)/def.h

##### local rules #####
$(RMKDEPO): %.o: %.c
	$(CC) $(OPT) $(CFLAGS) $(RMKDEPCFLAGS) -o $@ -c $<
