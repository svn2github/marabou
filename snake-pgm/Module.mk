<<<<<<< Module.mk
# Module.mk for ddacontrol / ddaexec module
# Author: Rudi Lutter, 23-Mar-2009

MODDIR			:= snake-pgm
MODDIRS 		:= $(MODDIR)/src
MODDIRI 		:= $(MODDIR)/inc

DDADIR	 		:= $(MODDIR)
DDADIRS 		:= $(DDADIR)/src
DDADIRI 		:= $(DDADIR)/inc

DDAL			:= $(MODDIRI)/LinkDef.h
DDAGS			:= $(MODDIRS)/G__DDAControlDict.cxx
DDAGO			:= $(DDAGS:.cxx=.o)
DDAH			:= $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
DDAS			:= $(MODDIRS)/DDAControlPanel.cxx
DDAO			:= $(DDAS:.cxx=.o)

DDACO			:= $(MODDIRS)/DDAControl.o
DDAXO			:= $(MODDIRS)/DDAExec.o
DDAXMEO			:= $(MODDIRS)/DDAExecME.o
DDAMO			:= $(DDACO) $(DDAXO) $(DDAXMEO)
DDALO			:= $(filter-out $(DDAMO),$(DDAO))

DDADEP			:= $(DDALO:.o=.d) $(DDAGO:.o=.d)

DDACTRL 		:= bin/DDAControl
DDAX 			:= bin/DDAExec
DDAXME 			:= bin/DDAExecME
DDALIB			:= $(LPATH)/libDDAControl.so

ALLEXECS		+= $(DDACTRL)
#ALLEXECS		+= $(DDAX)
ALLEXECS		+= $(DDAXME)

ALLLIBS 		+= $(DDALIB)

ALLHDRS 		+= $(patsubst $(MODDIRI)/%.h,include/%.h,$(DDAH))
=======
# Module.mk for snake server module

MODDIR       := snake-lib
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

DDADIR      := $(MODDIR)
DDADIRS     := $(DDADIR)/src
DDADIRI     := $(DDADIR)/inc

DDAL        := $(MODDIRI)/LinkDef.h
DDADS       := $(MODDIRS)/G__DDADict.cxx
DDADO       := $(DDADS:.cxx=.o)         

DDAO       := $(MODDIRS)/TSnkDDA0816.o  $(MODDIRS)/TSnkDDAChannel.o  $(DDADO)                

DDAH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))

DDADEP      := $(DDAO:.o=.d)

DDALIB      := $(LPATH)/libTSnkDDA0816.$(SOEXT)
DDA0816DIRI	:= dda0816/include
DDA0816H	:= $(wildcard $(DDA0816DIRI)/*.h)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(DDAH))
ALLLIBS     += $(DDALIB)
>>>>>>> 1.8

# include all dependency files
INCLUDEFILES	+= $(DDADEP)

##### extra libs needed #####
SNKLIBS			:= -L$(LPATH) -lTSnkDDA0816 -lTMrbUtils -lTGMrbUtils

##### local rules #####

include/%.h:    $(DDADIRI)/%.h
<<<<<<< Module.mk
				cp $< $@

$(DDACTRL): 	$(DDALO) $(DDACO)
				$(LD) -g $(LDFLAGS) $(DDACO) $(DDAGO) $(DDALIB) $(SNKLIBS) $(ROOTGLIBS) -o $(DDACTRL)

$(DDAX):	 	$(DDAXO)
				$(LD) -g $(LDFLAGS) $(DDAXO) $(SNKLIBS) $(ROOTGLIBS) -o $(DDAX)

$(DDAXME):	 	$(DDAXMEO)
				$(LD) -g $(LDFLAGS) $(DDAXMEO) $(SNKLIBS) $(ROOTGLIBS) -o $(DDAXME)
=======
				cp $< $@
>>>>>>> 1.8

<<<<<<< Module.mk
$(DDALIB):		$(DDAGO) $(DDALO)
				@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
				"$(SOFLAGS)" libDDAControl.$(SOEXT) $@ "$(DDALO) $(DDAGO)"
=======
$(DDALIB):		$(DDAO) 
				@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" "$(SOFLAGS)" libTSnkDDA0816.$(SOEXT) $@ "$(DDAO)" 
>>>>>>> 1.8

<<<<<<< Module.mk
$(DDAGS):		$(DDAH) $(DDAL)
				$(ROOTCINT) -f $@ -c -p -Iinclude $(DDAH) $(DDAL)
=======
$(DDADS):		$(DDAH) $(DDAL)
				@echo "Generating dictionary $@..."
				$(ROOTCINT) -f $@ -c -Iinclude $(DDAH) $(DDAL)
>>>>>>> 1.8

<<<<<<< Module.mk
$(DDAGO):		$(DDAGS)
				$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<
=======
$(DDADO):		$(DDADS)
				$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<
>>>>>>> 1.8

<<<<<<< Module.mk
all-dda:		$(DDACTRL)
=======
all-dda:		$(DDALIB)
>>>>>>> 1.8

clean-dda:
<<<<<<< Module.mk
				@rm -f $(DDAO) $(DDAMO) $(DDAGO) $(DDAG)
=======
				@rm -f $(DDAO) $(DDADO) $(DDADS)
>>>>>>> 1.8

clean::         clean-dda

<<<<<<< Module.mk
distclean-dda:	clean-dda
				@rm -f $(DDADEP) $(DDAG) $(DDACTRL) $(DDALIB)
=======
distclean-dda:	clean-dda
				@rm -f $(DDADEP) $(DDADS) $(DDADH) $(DDALIB)
>>>>>>> 1.8

distclean::     distclean-dda
