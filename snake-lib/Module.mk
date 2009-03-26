# Module.mk for snake server module

MODDIR				:= snake-lib
MODDIRS 			:= $(MODDIR)/src
MODDIRI 			:= $(MODDIR)/inc

SNKDIR				:= $(MODDIR)
SNKDIRS				:= $(SNKDIR)/src
SNKDIRI				:= $(SNKDIR)/inc

SNKL 				:= $(MODDIRI)/LinkDef.h
SNKDS				:= $(MODDIRS)/G__SNKDict.cxx
SNKDO				:= $(SNKDS:.cxx=.o)         

SNKO 				:= $(MODDIRS)/TSnkDDA0816.o  $(MODDIRS)/TSnkDDAChannel.o  $(SNKDO)                

SNKH 				:= $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))

SNKDEP				:= $(SNKO:.o=.d)

SNKLIB				:= $(LPATH)/libTSnkDDA0816.$(SOEXT)

# used in the main Makefile
ALLHDRS 			+= $(patsubst $(MODDIRI)/%.h,include/%.h,$(SNKH))
ALLLIBS 			+= $(SNKLIB)

# include all dependency files
INCLUDEFILES		+= $(SNKDEP)
##### extra libs needed #####
TMRBLIBS			:= -L$(LPATH) -lTMrbUtils -lTGMrbUtils
EXTRALIBS			:= dda0816/lib/libdda0816.a

##### local rules #####

include/%.h:    	$(SNKDIRI)/%.h
					cp $< $@

$(SNKLIB):			$(SNKO) 
					@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" "$(SOFLAGS)" libTSnkDDA0816.$(SOEXT) $@ "$(SNKO)" 

$(SNKDS):			$(SNKH) $(SNKL)
					@echo "Generating dictionary $@..."
					$(ROOTCINT) -f $@ -c -Iinclude $(SNKH) $(SNKL)

$(SNKDO):			$(SNKDS)
					$(CXX) $(NOOPT) $(CXXFLAGS) -DROOTVERSION=$(ROOTVERS) -I. -o $@ -c $<

all-SNK:			$(SNKLIB)

clean-SNK:
					@rm -f $(SNKO) $(SNKDO) $(SNKDS)

clean:: 			clean-SNK

distclean-SNK:		clean-SNK
					@rm -f $(SNKDEP) $(SNKDS) $(SNKDH) $(SNKLIB)

distclean:: 		distclean-SNK
