# ------------ Template Makefile ---------------------------------------

RIO3 = $(shell uname -r | grep '3.1.0' >/dev/null && echo 1)

CC = gcc 
CFLAGS =  -pipe -g -ansi -pedantic -Wall

CXX = g++
CXXFLAGS = -pipe -g -ansi -Wall 

CPPFLAGS =

INCDIR = -I../support -I../liblvme -I../allParam/include

LDFLAGS =
LIBDIR = -L../support -L../liblvme -L../allParam -L/lib/ces
LOADLIBES = -ltclParam -lsupport -ltcl -llvme -lbus -lvme -lbma -luio -lnetinet
ifeq ($(RIO2), 1)
CPPFLAGS = -DHAVE_LIBCOMPAT=1
INCDIR = -I../support -I/usr/local/include/hades
LIBDIR = -L../support -L/usr/local/lib/hades -L/lib/ces
LOADLIBES = -lcompat -ltclParam -lsupport -ltcl -llvme -lvme -lbma -luio -lnetinet
endif


# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAG = -rv

# ------------ Install -------------------------------------------------

BIN.OBJS = 
LIB.OBJS = 
INC.OBJS = 

BIN_DIR = 
LIB_DIR = 
INC_DIR = 

# ------------ Objects -------------------------------------------------

TARGETS = racectrl racedbg makedpr

racectrl.OBJS = racectrl.o race_lib.o rc.o

racedbg.OBJS = racedbg.o rc.o

makedpr.OBJS = makedpr.o

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS = librace.a

librace.OBJS = race_lib.o rc.o

# ------------ Suffix Rules -------------------------------------------

.SUFFIXES:

# ----- C Code ---------
.c.o:
	$(CC) $< -c $(CPPFLAGS) $(CFLAGS) $(INCDIR) -o $@ 

.SUFFIXES: .o .c 

# ----- C++ - Code -----
.cc.o:
	$(CXX) $< -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) -o $@ 

.C.o:
	$(CXX) $< -c $(CPPFLAGS) $(CXXFLAGS) $(INCDIR) -o $@ 

.SUFFIXES: .o .cc .C

# ------------ Targets -------------------------------------------------

all: $(TARGETS) $(LIB_TARGETS)

racectrl: $(racectrl.OBJS)
	@echo LINKING $@
	$(CC) $^ $(LDFLAGS) $(LIBDIR) $(LOADLIBES) -o $@ 
	@echo DONE!

racedbg: $(racedbg.OBJS)
	@echo LINKING $@
	$(CC) $^ $(LDFLAGS) $(LIBDIR) $(LOADLIBES) -o $@ 
	@echo DONE!

makedpr: $(makedpr.OBJS)
	@echo LINKING $@
	$(CC) $^ $(LDFLAGS) $(LIBDIR) $(LOADLIBES) -o $@ 
	@echo DONE!

librace.a: $(librace.OBJS)
	@echo CREATING library $@
	$(AR) $(ARFLAGS) $@ $^
	@echo DONE!

lib: $(LIB_TARGETS)

distclean:
	rm -f *.o core core.* $(TARGETS) $(LIB_TARGETS)

clean:
	rm -f *.o core core.*

depend:
	gcc -MM $(CFLAGS) $(CPPFLAGS) *.c

install_bin: $(BIN.OBJS) 
	@echo Installing binaries to $(BIN_DIR)
	mkdir -p -f $(BIN_DIR)
	install -m 755 $^ $(BIN_DIR)/

install_lib: $(LIB.OBJS)
	@echo Installing libaries to $(LIB_DIR)
	mkdir -p -f $(LIB_DIR)
	install -m 755 $^ $(LIB_DIR)/

install_inc: $(INC.OBJS)
	@echo Installing include-files to $(INC_DIR)
	mkdir -p -f $(INC_DIR)
	install -m 644 $^ $(INC_DIR)

install: install_bin install_lib install_inc

# ------------ Dependencies --------------------------------------------

rc.o: rc.c rc.h

race_lib.o: race_lib.c rc.h race_lib.h 

racectrl.o: racectrl.c race_lib.h

racedbg.o: racedbg.c rc.h

makedpr.o: makedpr.c
