# Module.mk for transport module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Otto Schaile

MODDIR       := transport
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

TRANSPDIR       := $(MODDIR)
TRANSPDIRS     := $(TRANSPDIR)/src
TRANSPDIRI     := $(TRANSPDIR)/inc

##### lib #####
TRANSPL        := $(MODDIRI)/LinkDef.h
TRANSPDS       := $(MODDIRS)/G__TMrbTransportDict.cxx
TRANSPPCM       := $(MODDIRS)/G__TMrbTransportDict_rdict.pcm
TRANSPDO       := $(TRANSPDS:.cxx=.o)
TRANSPH        := $(filter-out $(MODDIRI)/LinkDef%,$(wildcard $(MODDIRI)/*.h))
TRANSPS        := $(filter-out $(MODDIRS)/G__%,$(wildcard $(MODDIRS)/*.cxx))
TRANSPO        := $(TRANSPS:.cxx=.o)

MBSIOOBJS         := obj/mbsio.o obj/byte_order.o

TRANSPDEP      := $(TRANSPO:.o=.d) $(TRANSPDO:.o=.d)

TRANSPLIB      := $(LPATH)/libTMrbTransport.$(SOEXT)

# used in the main Makefile
ALLHDRS     += $(patsubst $(MODDIRI)/%.h,include/%.h,$(TRANSPH))
ALLLIBS     += $(TRANSPLIB)

# include all dependency files
INCLUDEFILES += $(TRANSPDEP)

##### local rules #####

include/%.h:    $(TRANSPDIRI)/%.h
		cp $< $@

$(TRANSPLIB):     $(TRANSPDO) $(TRANSPO) $(MAINLIBS) $(TRANSPLIBDEP) $(MBSIOOBJS)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libTMrbTransport.$(SOEXT) $@ "$(TRANSPO) $(TRANSPDO)" \
		   "$(MBSIOOBJS) $(TRANSPLIBEXTRA)"
		@(if [ -f $(TRANSPDIRS)/$(TRANSPPCM) ] ; then \
			echo "cp  $(TRANSPDIRS)/$(TRANSPPCM)----------------------" ; \
			cp $(TRANSPDIRS)/$(TRANSPPCM) $(LPATH); \
		fi)

$(TRANSPDS):     $(TRANSPH) $(TRANSPL)
		@echo "Generating dictionary $@..."
		$(ROOTCINT) -f $@ -c -Iinclude $(TRANSPH) $(TRANSPL)

$(TRANSPDO):     $(TRANSPDS)
		$(CXX) $(NOOPT) $(CXXFLAGS) -I. -o $@ -c $<

all-transport:       $(TRANSPLIB)

clean-transport:
		@rm -f $(TRANSPO) $(TRANSPDS) $(TRANSPDO)

clean::         clean-transport

distclean-transport: clean-transport
		@rm -f $(TRANSPDEP) $(TRANSPDS)  $(TRANSPDO) $(TRANSPLIB)

distclean::     distclean-transport
