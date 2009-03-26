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

# include all dependency files
INCLUDEFILES	+= $(DDADEP)

##### extra libs needed #####
SNKLIBS			:= -L$(LPATH) -lTSnkDDA0816 -lTMrbUtils -lTGMrbUtils

##### local rules #####

include/%.h:    $(DDADIRI)/%.h
				cp $< $@

$(DDACTRL): 	$(DDALO) $(DDACO)
				$(LD) -g $(LDFLAGS) $(DDACO) $(DDAGO) $(DDALIB) $(SNKLIBS) $(ROOTGLIBS) -o $(DDACTRL)

$(DDAX):	 	$(DDAXO)
				$(LD) -g $(LDFLAGS) $(DDAXO) $(SNKLIBS) $(ROOTGLIBS) -o $(DDAX)

$(DDAXME):	 	$(DDAXMEO)
				$(LD) -g $(LDFLAGS) $(DDAXMEO) $(SNKLIBS) $(ROOTGLIBS) -o $(DDAXME)
$(DDALIB):		$(DDAGO) $(DDALO)
				@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
				"$(SOFLAGS)" libDDAControl.$(SOEXT) $@ "$(DDALO) $(DDAGO)"
$(DDAGS):		$(DDAH) $(DDAL)
				$(ROOTCINT) -f $@ -c -p -Iinclude $(DDAH) $(DDAL)
$(DDAGO):		$(DDAGS)
				$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<
all-dda:		$(DDACTRL)

clean-dda:
				@rm -f $(DDAO) $(DDAMO) $(DDAGO) $(DDAG)

clean::         clean-dda

distclean-dda:	clean-dda
				@rm -f $(DDADEP) $(DDAG) $(DDACTRL) $(DDALIB)

distclean::     distclean-dda
