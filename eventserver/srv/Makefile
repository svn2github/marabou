########################################################################
# Copyright:
#   GSI, Gesellschaft fuer Schwerionenforschung mbH
#   Planckstr. 1
#   D-64291 Darmstadt
#   Germany  
########################################################################
# MRevServ.make
# make multithreaded remote event server (MBS) for Linux: mrevserv
# assume all sources and header files in local directory
# created  1. 2.1999 by Horst Goeringer
########################################################################
# 26.11.1999, H.G.: change module name from ptrevserv to mrevserv
########################################################################
#
MAIN = mrevserv

CFLAGS = -O -D GSI__LINUX -D SYS_UNIX -D LITTLE__ENDIAN
CINCLS = -I. \
         -I/usr/include -I/usr/include/sys

TOOLLIB = librevtool.a

RANDINCS = f_ut_gen_rand_event.h \
           f_ut_ini_rand_event.h \
           random-coll.h
INCS = ptrevmbsdef.h \
       ptrevclient.h \
       ptrevcomm.h \
       ptrevoldcomm.h \
       ptrevent.h \
       ptrevlock.h \
       s_str_info.h

RANDSRC = f_ut_gen_evt.c \
          f_ut_ini_evt.c \
          random-coll.c
TOOLSRC = ptrevlock.c \
          ptrevmbs.c \
          ptrevproc.c \
          rconnect.c \
          rclose.c \
          swaplw.c

RANDOBJ = f_ut_gen_evt.o \
          f_ut_ini_evt.o \
          random-coll.o
TOOLOBJ = ptrevlock.o \
          ptrevmbs.o \
          ptrevproc.o \
          rconnect.o \
          rclose.o \
          swaplw.o

$(MAIN): $(MAIN).c $(MAIN).o $(INCS) $(TOOLLIB)
	cc -o $(MAIN) $(CFLAGS) $(CINCLS) \
           $(MAIN).o \
           -L /usr/lib -lpthread -lm \
           -L. -lrevtool

$(MAIN).o: $(MAIN).c $(INCS)
	cc $(CINCLS) $(CFLAGS) -c $(MAIN).c

$(TOOLLIB): $(TOOLOBJ) $(RANDOBJ) $(TOOLSRC) $(RANDSRC)
	ar $(ARFLAGS) $(TOOLLIB) $(TOOLOBJ) $(RANDOBJ)

# general rule how to compile .c
.c.o:  
	cc $(CINCLS) $(CFLAGS) -c $<

clean:
	rm $(MAIN).o $(TOOLOBJ) $(RANDOBJ) $(TOOLLIB)
