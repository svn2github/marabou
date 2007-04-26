# Module.mk for c2lynx module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODDIR       := c2lynx
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

C2LYNXDIR      := $(MODDIR)
C2LYNXDIRS     := $(C2LYNXDIR)/src
C2LYNXDIRI     := $(C2LYNXDIR)/inc

##### lib #####
C2LYNXL        := $(MODDIRI)/LinkDef.h
C2LYNXDS       := $(MODDIRS)/G__TMrbC2LynxDict.cxx
C2LYNXDO       := $(C2LYNXDS:.cxx=.o)
C2LYNXH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
C2LYNXDH       := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/TMrb*.h))
C2LYNXS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
C2LYNXO        := $(C2LYNXS:.cxx=.o)

C2LYNXDEP      := $(C2LYNXO:.o=.d) $(C2LYNXDO:.o=.d)

C2LYNXLIB      := $(LPATH)/libTMrbC2Lynx.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(C2LYNXH))
ALLLIBS     += $(C2LYNXLIB)

# include all dependency files
INCLUDEFILES += $(C2LYNXDEP)

##### local rules #####

include/%.h:    $(C2LYNXDIRI)/%.h
		cp $< $@

$(C2LYNXLIB):     $(C2LYNXDO) $(C2LYNXO) $(MAINLIBS) $(C2LYNXLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbC2Lynx.$(SOEXT) $@ "$(C2LYNXO) $(C2LYNXDO)" \
		   "$(C2LYNXLIBEXTRA)"

$(C2LYNXDS):     $(C2LYNXH) $(C2LYNXL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(C2LYNXDH) $(C2LYNXL)

$(C2LYNXDO):     $(C2LYNXDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-c2lynx:       $(C2LYNXLIB)

clean-c2lynx:
		@rm -f $(C2LYNXO) $(C2LYNXDO) $(C2LYNXDS)

clean::         clean-c2lynx

distclean-c2lynx: clean-c2lynx
		@rm -f $(C2LYNXDEP) $(C2LYNXDS) $(C2LYNXLIB)

distclean::     distclean-c2lynx