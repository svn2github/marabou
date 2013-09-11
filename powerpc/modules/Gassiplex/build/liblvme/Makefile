# ------------ Compiler / Linker Options -------------------------------

INCDIR = -I.

CPPFLAGS =

ifeq ($(GSI_LYNX_PLATFORM), RIO4)
CPPFLAGS += -DRIO4 
endif

CC = gcc
CFLAGS = -pipe -g -ansi -Wall

CXX = g++g
CXXFLAGS = -pipe -g -ansi -Wall

LD = $(CC)

LDFLAGS =
LIBDIR = -L/lib/ces
LOADLIBES = -llvme -lbus -lvme -lbma -luio

# ------------ TARGETS -------------------------------------------------

TARGETS =

LIB_TARGETS = liblvme.a

# ------------ Libaries ------------------------------------------------

AR = ar
ARFLAGS = -srv

# ------------ Pattern Rules -------------------------------------------

# C Code:
%.o: %.c
	$(CC) $< -c $(CFLAGS) $(CPPFLAGS) $(INCDIR) -o $@ 

# C++ Code:
%.o: %.cpp
	$(CXX) $< -c $(CXXFLAGS) $(CPPFLAGS) $(INCDIR) -o $@ 

%.o: %.cc
	$(CXX) $< -c $(CXXFLAGS) $(CPPFLAGS) $(INCDIR) -o $@ 

%.o: %.C
	$(CXX) $< -c $(CXXFLAGS) $(CPPFLAGS) $(INCDIR) -o $@ 

# C/C++ Objects (set LD accordingly)
%: %.o
	@echo LINKING $@
	$(LD) $^ $(LDFLAGS) $(LIBDIR) $(LOADLIBES) -o $@ 
	@echo DONE!

# Libaries
%.a: $%
	@echo CREATING library $@
	$(AR) $(ARFLAGS) $@ $^
	@echo DONE!

%.so: $%
	@echo CREATING shared library $@
	$(LD) -shared -O $^ -o $@
	@echo DONE!

# ------------ Targets -------------------------------------------------

.PHONY: all
all: $(LIB_TARGETS) $(TARGETS)

.PHONY: clean 
clean:
	rm -f *.o core core.*
	rcsclean

.PHONY: distclean
distclean: clean
	rm -f $(TARGETS) $(LIB_TARGETS)
	rcsclean -u

.PHONY: depend
depend:
	$(CC) -MM $(CFLAGS) $(CPPFLAGS) $(INCDIR) *.c

# ------------ Dependencies --------------------------------------------

lvme.o: lvme.h lvme.c lbma.h lint.h lvme.c

xvmelib.o: xvmelib.c xvmelib.h

liblvme.a: lvme.o xvmelib.o
