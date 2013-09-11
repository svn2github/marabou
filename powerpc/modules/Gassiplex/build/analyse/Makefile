# ------------ Compiler / Linker Options -------------------------------

INCDIR = -I$(ROOTSYS)/include
CPPFLAGS = 

CC = gcc
CFLAGS = -pipe -g -ansi -pedantic -Wall

CXX = g++
CXXFLAGS = -pipe -g -ansi -O3 -Wall

LD = $(CXX)

LDFLAGS = 
LIBDIR =
LOADLIBES = $(shell root-config --libs)

# ------------ TARGETS -------------------------------------------------

TARGETS = analyse

LIB_TARGETS = 

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

analyse: analyse.o

analyse.o: analyse.C
