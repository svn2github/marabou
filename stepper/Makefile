# ------------ Compiler / Linker Options -------------------------------

INCDIR = 
CPPFLAGS = 

CXX = g++
CXXFLAGS = -pipe -g -ansi -Wall -fPIC

LD = $(CXX)

LDFLAGS =
LIBDIR =
LOADLIBES =

# ------------ TARGETS -------------------------------------------------

TARGETS = reference moveabsolute moverelative currentposition testrun libStepper.so

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

motorDict.C:		motor.C
			rootcint -f motorDict.C -c -I. motor.h motorLinkDef.h

libStepper.so:		motor.o motorDict.o
				$(CXX) $(LDFLAGS) -shared motor.o motorDict.o -o libStepper.so
				cp libStepper.so ../lib
				
.PHONY: clean 
clean:
	rm -f *.o core core.*

.PHONY: distclean
distclean: clean
	rm -f $(TARGETS) $(LIB_TARGETS)

.PHONY: depend
depend:
	$(CC) -MM $(CFLAGS) $(CPPFLAGS) $(INCDIR) *.c

# ------------ Dependencies --------------------------------------------

reference:		reference.o motor.o
moveabsolute:		moveabsolute.o motor.o
moverelative:		moverelative.o motor.o
currentposition: 	currentposition.o motor.o
testrun:		testrun.o motor.o

reference.o:		reference.C motor.h
moveabsolute.o: 	moveabsolute.C motor.h
moverelative.o: 	moverelative.C motor.h
currentposition.o: 	currentposition.C motor.h
testrun.o:		testrun.C motor.h

motor.o:		motor.C motor.h


