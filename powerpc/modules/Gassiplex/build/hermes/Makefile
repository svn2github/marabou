# ------------ Template Makefile ---------------------------------------

RIO2 = $(shell uname -r | grep '2.5.1' >/dev/null && echo 1)

CC = gcc 
CFLAGS =  -pipe -O2 -g -ansi -pedantic -Wall

CXX = g++
CXXFLAGS = -pipe -g -ansi -Wall

CPPFLAGS =

INCDIR = -I../support -I../liblvme -I../allParam/include

LDFLAGS =
LIBDIR = -L../support  -L../liblvme -L../allParam -L/lib/ces
LOADLIBES = -lsupport -ltclParam -lsupport -ltcl -llvme -lbus -lvme -lbma -luio -lnetinet 

ifeq ($(RIO2), 1)
CPPFLAGS = -DHAVE_LIBCOMPAT=1
INCDIR = -I../support 
LIBDIR = -L../support  -L/lib/ces
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

TARGETS = hdtuctrl

hdtuctrl.OBJS = hdtuctrl.o hdtu.o

# ------------ Library-Objects ----------------------------------------

LIB_TARGETS =libhermes.a

libhermes.OBJS = hdtu.o hrace.o

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

hdtuctrl: $(hdtuctrl.OBJS)
	@echo LINKING $@
	$(CC) $^ $(LDFLAGS) $(LIBDIR) $(LOADLIBES) -o $@ 
	@echo DONE!

libhermes.a: $(libhermes.OBJS)
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

hdtu.o: hdtu.h hdtu.c

hdtuctrl.o: hdtuctrl.c

hrace.o: hrace.h hrace.c

